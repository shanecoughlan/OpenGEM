/************************************************************************

    EDICON 2.00 - icon editor for GEM

    Copyright (C) 1998,1999,2000  John Elliott <jce@seasip.demon.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*************************************************************************/

#include "sysfonti.h"	/* [JCE] prototypes to stop PPD moaning */

VOID TRACE (char *fmt, ...)
{
	FILE *fp = fopen("d:\\gemapp.log", "a+");
	va_list ap;

	va_start(ap, fmt);
	vfprintf(fp, fmt, ap);
	va_end(ap);

	fclose(fp);
}	



LPBYTE string_addr(WORD which)		/* returns a tedinfo LONG string addr	*/
{
	LPVOID	where;

	rsrc_gaddr(R_STRING, which, &where);
	return ((LPBYTE)where);
} 


UWORD inside(UWORD x, UWORD y, GRECT *pt)		/* determine if x,y is in rectangle	*/
{
	if ( (x >= pt->g_x) && (y >= pt->g_y) &&
	    (x < pt->g_x + pt->g_w) && (y < pt->g_y + pt->g_h) )
		return(TRUE);
	else
		return(FALSE);
} /* inside */

VOID grect_to_array(GRECT *area, WORD *array)
{
	*array++ = area->g_x;
	*array++ = area->g_y;
	*array++ = area->g_x + area->g_w - 1;
	*array = area->g_y + area->g_h - 1;
}

VOID
rast_op(WORD mode, GRECT *s_area, MFDB *s_mfdb, 
				   GRECT *d_area, MFDB *d_mfdb)
{
	WORD	pxy[8];

	grect_to_array(s_area, pxy);
	grect_to_array(d_area, &pxy[4]);
					/* pixel for pixel source 	*/
	/**/				/*  to destination copy 	*/
	vro_cpyfm(vdi_handle, mode, pxy, s_mfdb, d_mfdb);  
}

VOID vdi_fix(LPMFDB pfd, LPVOID theaddr, WORD wb, WORD h)
{
	pfd->fww = wb >> 1;     	/* # of bytes to words 		*/
	pfd->fwp = wb << 3;  		/* # of bytes to to pixels 	*/
	pfd->fh = h;			/* height in scan lines		*/
	pfd->np = 1; 			/* number of planes		*/
	pfd->mp = theaddr;  		/* memory pointer		*/
}


void dump_mfdb(LPMFDB src, char *s)
{
	char t[160];
	sprintf(t, "[2][%s|mp=%lx fwp=%d fh=%d fww=%d|"
	               "ff=%d np=%d r1=%d r2=%d r3=%d][OK]",
	               s,
				(long)src->mp, src->fwp, src->fh, src->fww, src->ff,
				src->np, src->r1, src->r2, src->r3);
		                
}


WORD vdi_trans(LPVOID saddr, UWORD swb, LPVOID daddr, UWORD dwb, UWORD h)
{
	MFDB		src, dst;	/* local MFDB			*/

	memset(&src, 0, sizeof(MFDB));
	memset(&dst, 0, sizeof(MFDB));
	
	vdi_fix(&src, saddr, swb, h);
	src.ff = TRUE;			/* standard format 		*/

	vdi_fix(&dst, daddr, dwb, h);
	dst.ff = FALSE;  		/* transform to device 		*/
	/**/				/*  specific format		*/   

//	dump_mfdb(&src, "Source:");
//	dump_mfdb(&dst, "Dest:");
	
	vr_trnfm(vdi_handle, &src, &dst ); 
}


WORD vdi_untrans(LPVOID saddr, UWORD swb, LPVOID daddr, UWORD dwb, UWORD h)
{
	MFDB		src, dst;	/* local MFDB			*/

	memset(&src, 0, sizeof(MFDB));
	memset(&dst, 0, sizeof(MFDB));
	
	vdi_fix(&src, saddr, swb, h);
	src.ff = FALSE;			/* standard format 		*/

	vdi_fix(&dst, daddr, dwb, h);
	dst.ff = TRUE;  		/* transform to device 		*/
	/**/				/*  specific format		*/   

	vr_trnfm(vdi_handle, &src, &dst ); 
}





VOID objc_xywh(LPTREE tree, WORD obj, GRECT *p)		/* get x,y,w,h for specified object	*/
{
	objc_offset(tree, obj, &(p->g_x), &(p->g_y));
	p->g_w = tree[obj].ob_width;	//LWGET(OB_WIDTH(obj));
	p->g_h = tree[obj].ob_height;	//LWGET(OB_HEIGHT(obj));
}



VOID wind_setl(WORD whndl, WORD what, LPVOID lptr)
{
	wind_set(whndl, what, FP_OFF(lptr), FP_SEG(lptr), 0, 0);
}



WORD align_x(WORD x)		/* forces word alignment for column position	*/
							/*   rounding to nearest word			*/
{
	return((x & 0xfff0) + ((x & 0x000c) ? 0x0010 : 0));
}	


WORD rsrc_alert(WORD rsrc, WORD defbtn)
{
	LPBYTE str;
	
	rsrc_gaddr(R_STRING, rsrc, (LPVOID *)&str);

	return form_alert(defbtn, str);

}

LPVOID local_realloc(LPVOID p, LONG n)
{
	LPVOID pmem;
	
	pmem = realloc(p,n);
	return pmem;
}

LPVOID local_alloc(LONG n)
{
	LPVOID pmem;

	pmem = malloc(n);
/*	{
		char s[90];
		LPVOID p;
		union REGS ir;

		ir.x.ax = 0x4800;
		ir.x.bx = 0xffff;
		intdos(&ir, &ir);
		
		sprintf(s,"[0][Memory available = %x alloc %lx][OK]", ir.x.bx, n);
		form_alert(1,s);

		p = (LPVOID)dos_alloc(n);

		ir.x.ax = 0x4800;
		ir.x.bx = 0xffff;
		intdos(&ir, &ir);
		
		sprintf(s,"[0][Memory available = %x at %lx][OK]", ir.x.bx, (long)p);
		form_alert(1,s);

		return p;
	}
*/
	return pmem;
//	return malloc(n);	
}

VOID local_free(LPVOID n)
{
	free(n);
/*	{
		char s[90];
		union REGS ir;

		ir.x.ax = 0x4800;
		ir.x.bx = 0xffff;
		intdos(&ir, &ir);
		
		sprintf(s,"[0][Memory available = %x free %lx][OK]", ir.x.bx, (long)n);
		form_alert(1,s);

		dos_free(n);

		ir.x.ax = 0x4800;
		ir.x.bx = 0xffff;
		intdos(&ir, &ir);
		
		sprintf(s,"[0][Memory available = %x freed][OK]", ir.x.bx);
		form_alert(1,s);

		return;
	}*/
//	free(n);
}



/* Utilities from the Professional GEM articles */


VOID send_redraw(WORD wh, GRECT *p)
	{
	WORD	msg[8];

	msg[0] = WM_REDRAW;		/* Defined in GEMBIND.H	    */
	msg[1] = gl_apid;		/* As returned by appl_init */
	msg[2] = 0;
	msg[3] = wh;			/* Handle of window to redraw */
	msg[4] = p->g_x;
	msg[5] = p->g_y;
	msg[6] = p->g_w;
	msg[7] = p->g_h;
	appl_write(gl_apid, 16, ADDR(msg));	/* Use ADDR(msg) for portability */
	}


VOID rc_constrain(GRECT *pc, GRECT *pt)
	{
	if (pt->g_x < pc->g_x)
		pt->g_x = pc->g_x;
	if (pt->g_y < pc->g_y)
		pt->g_y = pc->g_y;
	if ((pt->g_x + pt->g_w) > (pc->g_x + pc->g_w))
		pt->g_x = (pc->g_x + pc->g_w) - pt->g_w;
	if ((pt->g_y + pt->g_h) > (pc->g_y + pc->g_h))
		pt->g_y = (pc->g_y + pc->g_h) - pt->g_h;
	}



VOID hndl_full(WORD wh)
	{
	GRECT	prev;
	GRECT	curr;
	GRECT	full;

	wind_get(wh, WF_CXYWH, &curr.g_x, &curr.g_y, &curr.g_w, &curr.g_h);
	wind_get(wh, WF_PXYWH, &prev.g_x, &prev.g_y, &prev.g_w, &prev.g_h);
	wind_get(wh, WF_FXYWH, &full.g_x, &full.g_y, &full.g_w, &full.g_h);
	if ( rc_equal(&curr, &full) )
		{		/* Is full, change to previous 		*/
		graf_shrinkbox(prev.g_x, prev.g_y, prev.g_w, prev.g_h,
			full.g_x, full.g_y, full.g_w, full.g_h);
		wind_set(wh, WF_CXYWH, prev.g_x, prev.g_y, prev.g_w, prev.g_h);
				/* put send_redraw here if you need it */
		}
	else
		{		/* is not full, so set to full		*/
		graf_growbox(curr.g_x, curr.g_y, curr.g_w, curr.g_h,
			full.g_x, full.g_y, full.g_w, full.g_h);
		wind_set(wh, WF_CXYWH, full.g_x, full.g_y, full.g_w, full.g_h);
		}
	}



VOID get_path(BYTE *tmp_path, BYTE *spec)	/* get directory path name		*/
{
	WORD	cur_drv;

	cur_drv     = dos_gdrv();
	tmp_path[0] = cur_drv + 'A';
	tmp_path[1] = ':';
	tmp_path[2] = '\\';
	dos_gdir(cur_drv+1, ADDR(&tmp_path[3]));
	if (strlen(tmp_path) > 3) strcat(tmp_path, "\\");
	else                      tmp_path[2] = '\0';

	strcat(tmp_path, spec);
}


VOID add_file_name(BYTE *dname, BYTE *fname)
{
	BYTE	c;
	WORD	ii;

	ii = strlen(dname);
	while (ii && (((c = dname[ii-1])  != '\\') && (c != ':')))
		ii--;
	dname[ii] = '\0';
	strcat(dname, fname);
}




FILE *get_file(BOOLEAN loop, BYTE *file_name, BYTE *mask, char *mode)		
{
	WORD	fs_iexbutton;
	BYTE	fs_iinsel[13];
	FILE	*fp;
	
	while (TRUE)
	{
		get_path(file_name, mask);
		fs_iinsel[0] = '\0'; 

		fsel_input(ADDR(file_name), ADDR(fs_iinsel), &fs_iexbutton);
		if (fs_iexbutton)
		{
			add_file_name(file_name, fs_iinsel);
			fp = fopen(file_name, mode);
			if (!loop || (loop && fp != NULL))
				return(fp);
		}
		else   
		{
			return((FILE *)NULL);   
		}
	}
	return ((FILE *)NULL);
} /* get_file */


WORD lstrncmp(LPBYTE s1, LPBYTE s2, WORD count)
{
	while (count--)
	{
		if (*s1 != *s2) return *s1 - *s2;

		if (!*s1) return 0;
		++s1;
		++s2;
	}
	return 0;
}


/*
void *lpfix(LPVOID addr)
{
	return addr;
}
*/

void *lpfix(LPVOID addr);

#asm
	.globl	_lpfix
	.signat	_lpfix,4156
_lpfix:
	push	bp
	mov	bp,sp
	mov	ax,0+6[bp]
	mov	dx,2+6[bp]
	pop	bp
	retf	#4	;returns 4 bytes
#endasm
	
WORD lfwrite(LPVOID addr, size_t size, int count, FILE *fp)
{
	void *a2;

	a2 = lpfix(addr);
	
	fwrite(a2, size, count, fp);

}

VOID set_clip(WORD clip_flag, GRECT *s_area)    /* set clip to specified area */
{
        WORD    pxy[4];

        rc_grect_to_array(s_area, pxy);
        vs_clip(vdi_handle, clip_flag, pxy);
}


WORD LSTCPY(LPBYTE s1, LPBYTE s2)
{
	WORD n = 0;
	do
	{
		s1[n] = s2[n];
		++n;
	} while (s2[n-1]);
	return n;
}

