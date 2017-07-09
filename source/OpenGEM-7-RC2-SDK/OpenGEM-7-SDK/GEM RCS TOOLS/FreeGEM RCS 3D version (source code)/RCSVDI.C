/*	RCSVDI.C	04/09/85	Tim Oren converted from:	*/
/*	APGSXIF.C	05/06/84 - 12/21/84	Lee Lorenzen		*/
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
#include "ppdrcs.h"

#define ORGADDR 0x0L

GLOBAL WORD	gl_width;
GLOBAL WORD	gl_height;

GLOBAL WORD	gl_nrows;
GLOBAL WORD	gl_ncols;

GLOBAL WORD	gl_wchar;
GLOBAL WORD	gl_hchar;

GLOBAL WORD	gl_wschar;
GLOBAL WORD	gl_hschar;

GLOBAL WORD	gl_wptschar;
GLOBAL WORD	gl_hptschar;

GLOBAL WORD	gl_wsptschar;
GLOBAL WORD	gl_hsptschar;

GLOBAL WORD	gl_wbox;
GLOBAL WORD	gl_hbox;

GLOBAL WORD	gl_xclip;
GLOBAL WORD	gl_yclip;
GLOBAL WORD	gl_wclip;
GLOBAL WORD	gl_hclip;

GLOBAL WORD 	gl_nplanes;
GLOBAL WORD 	gl_handle;

GLOBAL MFDB		gl_src;
GLOBAL MFDB		gl_dst;
GLOBAL MFDB		gl_tmp;

GLOBAL WS		gl_ws;

GLOBAL WORD		gl_mode;
GLOBAL WORD		gl_tcolor;
GLOBAL WORD		gl_lcolor;
GLOBAL WORD		gl_fis;
GLOBAL WORD		gl_patt;

GLOBAL GRECT		gl_rscreen;
GLOBAL GRECT		gl_rfull;
GLOBAL GRECT		gl_rzero;
GLOBAL GRECT		gl_rcenter;
GLOBAL GRECT		gl_rmenu;

MLOCAL WORD intin[128], ptsin[128];

	VOID
gsx_moff()
	{
	graf_mouse(M_OFF, 0x0L);
	}

	VOID
gsx_mon()
	{
	graf_mouse(M_ON, 0x0L);
	}


/************************************************************************/
/* g r _ r e c t							*/
/************************************************************************/
	VOID
gr_rect(icolor, ipattern, pt)
	WORD		icolor;
	WORD		ipattern;
	GRECT		*pt;
	{
	WORD		fis;

	fis = FIS_PATTERN;
	if (ipattern == IP_HOLLOW)
	  	fis = FIS_HOLLOW;
	else if (ipattern == IP_SOLID)
	  	fis = FIS_SOLID;

	vsf_color(gl_handle, icolor);
	bb_fill(MD_REPLACE, fis, ipattern, 
	  	pt->g_x, pt->g_y, pt->g_w, pt->g_h);
	} 

VOID gsx_sclip(GRECT *pt)
{
	r_get(pt, &gl_xclip, &gl_yclip, &gl_wclip, &gl_hclip);

	if ( gl_wclip && gl_hclip )
	{
	  ptsin[0] = gl_xclip;
	  ptsin[1] = gl_yclip;
	  ptsin[2] = gl_xclip + gl_wclip - 1;
	  ptsin[3] = gl_yclip + gl_hclip - 1;
	  vs_clip( gl_handle, TRUE, &ptsin[0]);
	}
	else vs_clip( gl_handle, FALSE, &ptsin[0]);
	return;
} /* gsx_setclip */



VOID gsx_attr(WORD text, WORD mode, WORD color)
{
	WORD		tmp;

	tmp = intin[0];
	if (mode != gl_mode) vswr_mode(gl_handle, gl_mode = mode);
	if (text)
	{
	  if (color != gl_tcolor) vst_color(gl_handle, gl_tcolor = color);
	}	
	else
	{
	  if (color != gl_lcolor) vsl_color(gl_handle, gl_lcolor = color);
	}
	intin[0] = tmp;
}



VOID gsx_bxpts(GRECT *pt)
{
	ptsin[0] = pt->g_x;
	ptsin[1] = pt->g_y;
	ptsin[2] = pt->g_x + pt->g_w - 1;
	ptsin[3] = pt->g_y;
	ptsin[4] = pt->g_x + pt->g_w - 1;
	ptsin[5] = pt->g_y + pt->g_h - 1;
	ptsin[6] = pt->g_x;
	ptsin[7] = pt->g_y + pt->g_h - 1;
	ptsin[8] = pt->g_x;
	ptsin[9] = pt->g_y;
}

	VOID
gsx_xbox(pt)
	GRECT		*pt;
{
	gsx_bxpts(pt);
	gsx_xline( 5, &ptsin[0] );
}


VOID gsx_fix(MFDB *pfd, LPVOID theaddr, WORD wb, WORD h)
{
	if (theaddr == ORGADDR)
	{
	  pfd->fwp = gl_ws.ws_xres + 1;
	  pfd->fww = pfd->fwp / 16;
	  pfd->fh  = gl_ws.ws_yres + 1;
	  pfd->np  = gl_nplanes;
	}
	else
	{
	  pfd->fww = wb / 2;
	  pfd->fwp = wb * 8;
	  pfd->fh  = h;
	  pfd->np  = 1;
	}
	pfd->ff = FALSE;
	pfd->mp = theaddr;
}


VOID gsx_untrans(LPVOID saddr, WORD swb, LPVOID daddr, WORD dwb, WORD h)
{
	gsx_fix(&gl_src, saddr, swb, h);
	gl_src.ff = FALSE;
	gl_src.np = 1;

	gsx_fix(&gl_dst, daddr, dwb, h);
	vr_trnfm(gl_handle, &gl_src, &gl_dst );
}

VOID gsx_trans(LPVOID saddr, WORD swb, LPVOID daddr, WORD dwb, WORD h)
{
	gsx_fix(&gl_src, saddr, swb, h);
	gl_src.ff = TRUE;
	gl_src.np = 1;

	gsx_fix(&gl_dst, daddr, dwb, h);
	vr_trnfm(gl_handle, &gl_src, &gl_dst );
}

VOID gsx_outline(GRECT *pt)
	{
	vs_clip(gl_handle, FALSE, &ptsin[0]);
	gsx_attr(FALSE, MD_XOR, BLACK);
	gsx_bxpts(pt);
	v_pline(gl_handle, 5, &ptsin[0]);
	}

	
VOID gsx_invert(GRECT *pt)
	{
	vsf_color(gl_handle, BLACK);
	vs_clip(gl_handle, FALSE, &ptsin[0]); 
	bb_fill(MD_XOR, FIS_SOLID, IP_SOLID, pt->g_x, pt->g_y, pt->g_w, pt->g_h);
	}

VOID bb_fill(WORD mode, WORD fis, WORD patt, 
             WORD hx, WORD hy, WORD hw, WORD hh)
{

	gsx_fix(&gl_dst, 0x0L, 0, 0);
	ptsin[0] = hx;
	ptsin[1] = hy;
	ptsin[2] = hx + hw - 1;
	ptsin[3] = hy + hh - 1;

	gsx_attr(TRUE, mode, gl_tcolor);
	if (fis  != gl_fis ) vsf_interior( gl_handle, gl_fis  = fis  );
	if (patt != gl_patt) vsf_style   ( gl_handle, gl_patt = patt );
	vr_xrecfl( gl_handle, &ptsin[0], &gl_dst );
}

	WORD
ch_width(fn)
	WORD		fn;
{
	if (fn == IBM)
	  return(gl_wchar);
	if (fn == SMALL)
	  return(gl_wschar);
	return(0);
}

	WORD
ch_height(fn)
	WORD		fn;
{
	if (fn == IBM)
	  return(gl_hchar);
	if (fn == SMALL)
	  return(gl_hschar);
	return(0);
}

VOID gsx_xline( WORD ptscount, WORD *ppoints )
{
	static	UWORD	hztltbl[2] = { 0x5555, 0xaaaa };
	static  UWORD	verttbl[4] = { 0x5555, 0xaaaa, 0xaaaa, 0x5555 };
	WORD		*linexy,i;
	WORD		st;

	for ( i = 1; i < ptscount; i++ )
	{
	  if ( *ppoints == *(ppoints + 2) )
	  {
	    st = verttbl[( (( *ppoints) & 1) | ((*(ppoints + 1) & 1 ) << 1))];
	  }	
	  else
	  {
		if (ppoints[0] < ppoints[2]) linexy = ppoints;
		else                         linexy = ppoints + 2;
	    st = hztltbl[( *(linexy + 1) & 1)];
	  }
	  vsl_udsty( gl_handle, st );
	  v_pline  ( gl_handle, 2, ppoints );
	  ppoints += 2;
	}
	vsl_udsty( gl_handle, -1) ; /* 0xffff ); */
}	

	WORD
gsx_start()
{
	WORD		char_height, nc, attrib[10];

	gl_xclip = 0;
	gl_yclip = 0;
	gl_width = gl_wclip = gl_ws.ws_xres + 1;
	gl_height = gl_hclip = gl_ws.ws_yres + 1;

	nc = gl_ws.ws_ncolors;
	gl_nplanes = 0;
	while (nc != 1)
	{
	  nc >>= 1;
	  gl_nplanes++;
	}
	if (!vqt_attributes( gl_handle, &attrib[0] )) return 0;
	gl_ws.ws_chmaxh = attrib[7];
	gl_ws.ws_pts2 = attrib[6];
	char_height = gl_ws.ws_chminh;
	vst_height( gl_handle, char_height, &gl_wsptschar, &gl_hsptschar, 
				&gl_wschar, &gl_hschar );
	char_height = gl_ws.ws_chmaxh;
	vst_height( gl_handle, char_height, &gl_wptschar, &gl_hptschar, 
				&gl_wchar, &gl_hchar );
	gl_ncols = gl_width / gl_wchar;
	gl_nrows = gl_height / gl_hchar;
	gl_hbox = gl_hchar + 3;
	gl_wbox = (gl_hbox * gl_ws.ws_hpixel) / gl_ws.ws_wpixel;
	vsl_type ( gl_handle, 7 );
	vsl_width( gl_handle, 1 );
	vsl_udsty( gl_handle, -1); /* 0xffff ); */
	r_set(&gl_rscreen, 0, 0, gl_width, gl_height);
	r_set(&gl_rfull, 0, gl_hbox, gl_width, (gl_height - gl_hbox));
	r_set(&gl_rzero, 0, 0, 0, 0);
	r_set(&gl_rcenter, (gl_width-gl_wbox)/2, (gl_height-(2*gl_hbox))/2, 
			gl_wbox, gl_hbox);
	r_set(&gl_rmenu, 0, 0, gl_width, gl_hbox);
	return 1;
}

	VOID
gsx_vopen()
{
	WORD		i;

	for(i=0; i<10; i++)
	  intin[i] = 1;
	intin[10] = 2;			/* device coordinate space */

	v_opnvwk( &intin[0], &gl_handle, (WORD *)&gl_ws );
}

VOID gsx_vclose()
	{
	v_clsvwk(gl_handle);
	}

	
VOID vr_xrecfl(WORD handle, WORD *pxyarray, MFDB *pdesMFDB )
{
	/* XXX What's the MFDB for? */
	vr_recfl(handle, pxyarray);
}


VOID bb_set(WORD sx, WORD sy, WORD sw, WORD sh, WORD *pts1, WORD *pts2, 
            MFDB *pfd, MFDB *psrc, MFDB *pdst)
{
	WORD		oldsx;

						/* get on word boundary	*/
	oldsx = sx;
	sx = (sx / 16) * 16;
	sw = ( ((oldsx - sx) + (sw + 15)) / 16 ) * 16;

	gl_tmp.ff  = TRUE;
	gl_tmp.fww = sw / 16;
	gl_tmp.fwp = sw;
	gl_tmp.fh  = sh;

	gsx_moff();
	pts1[0] = sx;
	pts1[1] = sy;
	pts1[2] = sx + sw - 1;
	pts1[3] = sy + sh - 1;
	pts2[0] = 0;
	pts2[1] = 0;
	pts2[2] = sw - 1;
	pts2[3] = sh - 1 ;

	gsx_fix(pfd, 0x0L, 0, 0);
	vro_cpyfm( gl_handle, S_ONLY, &ptsin[0], psrc, pdst );
	gsx_mon();
}


	VOID
bb_save(ps)
	GRECT		*ps;
{	
	bb_set(ps->g_x, ps->g_y, ps->g_w, ps->g_h, &ptsin[0], &ptsin[4], 
		&gl_src, &gl_src, &gl_tmp);
}


	VOID
bb_restore(pr)
	GRECT		*pr;
{
	bb_set(pr->g_x, pr->g_y, pr->g_w, pr->g_h, &ptsin[4], &ptsin[0], 
		&gl_dst, &gl_tmp, &gl_dst);
}

