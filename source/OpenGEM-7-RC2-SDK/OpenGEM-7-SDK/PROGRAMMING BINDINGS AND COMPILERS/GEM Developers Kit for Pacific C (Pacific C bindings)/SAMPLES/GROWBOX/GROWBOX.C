#include "ppdgem.h"
#include <dos.h>				// for FP_OFF and FP_SEG macros
#include <time.h>
#include <stdio.h>

#define	DESK		0

#define END_UPDATE	0
#define	BEG_UPDATE	1


/*------------------------------*/
/*			Local				*/
/*------------------------------*/

WORD	gl_wchar;			/* character width		*/
WORD	gl_hchar;			/* character height		*/
WORD	gl_wbox;			/* box (cell) width		*/
WORD	gl_hbox;			/* box (cell) height		*/
WORD	gem_handle;			/* GEM vdi handle		*/
WORD	vdi_handle;			/* growbox vdi handle		*/
WORD	work_out[57];			/* open virt workstation values	*/
GRECT	work_area;			/* current window work area	*/
WORD	gl_apid;			/* application ID		*/
WORD	gl_rmsg[8];			/* message buffer		*/
LPBYTE	ad_rmsg;			/* LONG pointer to message bfr	*/
WORD	gl_xfull;			/* full window 'x'		*/
WORD	gl_yfull;			/* full window 'y'		*/
WORD	gl_wfull;			/* full window 'w' width	*/
WORD	gl_hfull;			/* full window 'h' height	*/
WORD	ev_which;			/* event message returned value	*/
WORD	type_size;			/* system font cell size	*/



/*------------------------------*/
/*	hndl_mesag		*/
/*------------------------------*/
BOOLEAN	hndl_mesag()
{
	return(FALSE);
} /* hndl_mesag */


/* A particularly minimalist event handler. */

growbox()
{
	BOOLEAN	done;

	/**/								/* loop handling user	*/
	/**/								/*   input until done	*/
	done = FALSE;						/*   -or- if DESKACC	*/
	while( !done )						/*   then forever	*/
	{
		ev_which = evnt_mesag(ad_rmsg);	/* wait for message	*/
		wind_update(BEG_UPDATE);		/* begin window update	*/
										/* This DA ignores all messages */
		wind_update(END_UPDATE);		/* end window update	*/
		done = FALSE;					/* never exit loop for desk accessory	*/
	}
}


/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Termination				     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*		growbox_term			*/
/*------------------------------*/  
growbox_term() 
{
	return(FALSE);			/* Desk Accessory never ends	*/
}


/* usleep() busy-waits for a short length of time (about 1/300 of a 
   second) */

static unsigned long sleeptime;

void usleep(void)
{
	long t,s;

	for (t = 0; t < (sleeptime / 300); t++) s=time(NULL);
}

/* Animate a rectangle moving between two places */

void animate_rect(GRECT *rectSrc, GRECT *rectDest)
{
	float xs = (rectDest->g_x - rectSrc->g_x) / 25.0;
	float ys = (rectDest->g_y - rectSrc->g_y) / 25.0;
	float ws = (rectDest->g_w - rectSrc->g_w) / 25.0;
	float hs = (rectDest->g_h - rectSrc->g_h) / 25.0;
	static WORD c[10];
	WORD nr, ns, nc;
	WORD *coords;
		
	/* [v0.3] Don't animate if the rectangles are in the same place */
	if (xs == 0 && ys == 0 && ws == 0 && hs == 0) return;
	
	for (ns = 0; ns < 2; ns++) for (nr = 0; nr < 25; nr++)
	{
		int sx = (int)(nr * xs);
		int sy = (int)(nr * ys);
		int sw = (int)(nr * ws);
		int sh = (int)(nr * hs);
	
		coords = c;
		
		coords[0] = rectSrc->g_x +                sx;
		coords[1] = rectSrc->g_y +                sy;	
		coords[2] = rectSrc->g_x + rectSrc->g_w + sx + sw;
		coords[3] = rectSrc->g_y +                sy;
		coords[4] = rectSrc->g_x + rectSrc->g_w + sx + sw;
		coords[5] = rectSrc->g_y + rectSrc->g_h + sy + sh;
		coords[6] = rectSrc->g_x +                sx;
		coords[7] = rectSrc->g_y + rectSrc->g_h + sy + sh;
		coords[8] = rectSrc->g_x +                sx;	
		coords[9] = rectSrc->g_y +                sy;	

		/* [v0.3] Check the coordinates don't go negative */
		for (nc = 0; nc < 10; nc++) 
		{
			if (coords[nc] < 0)         coords[nc] = 0;
		}
		v_pline(vdi_handle, 5, coords);
		usleep();
	}
}

/* Intercept AES calls and make any changes we like.
 *
 * This function adds graf_growbox() and graf_shrinkbox() back into the AES;
 * also the two zoombox functions in form_dial()
 *
 */

static GRECT rects[3];


WORD myaes(LPGEMBLK gb)
{	
	WORD func = gb->gb_pcontrol[0];
	WORD diff_w, diff_h;
	WORD lineattr[10];
	
	if (func == GRAF_GROWBOX || func == GRAF_SHRINKBOX)
	{
//		FILE *fp = fopen("d:/gemapp.log", "a");
		LWCOPY((LPWORD)(rects), (LPWORD)(gb->gb_pintin), 8);
//		fprintf(fp, "%s %d,%d,%d,%d %d,%d,%d,%d\n",
//				(func == GRAF_GROWBOX) ? "graf_growbox  " : "graf_shrinkbox",
//				rects[0].g_x, rects[0].g_y, rects[0].g_w, rects[0].g_h,
//				rects[1].g_x, rects[1].g_y, rects[1].g_w, rects[1].g_h);
//		fclose(fp);
		
		LWCOPY((LPWORD)(rects), (LPWORD)(gb->gb_pintin), 8);
		
	}
	if (func == FORM_DIAL)
	{
		if (gb->gb_pintin[0] == 1)
		{
			func = GRAF_GROWBOX;
			LWCOPY((LPWORD)(rects), (LPWORD)(&gb->gb_pintin[1]), 8);
		} 
		if (gb->gb_pintin[0] == 2)
		{
			func = GRAF_SHRINKBOX;
			LWCOPY((LPWORD)(rects), (LPWORD)(&gb->gb_pintin[1]), 8);
		} 
	}

	if (func == GRAF_GROWBOX)
	{
		/* This code changes various vdi settings, so we carefully change
		 * them back afterwards */
		 
		vql_attributes(vdi_handle, lineattr);
		 
		vswr_mode(vdi_handle, 3);
		vsl_color(vdi_handle, 1);
		vsl_udsty(vdi_handle, 0x5555);
		vsl_type (vdi_handle, work_out[6] - 1);
		vsl_width(vdi_handle, 1);

		/* Work out the intermediate rectangle - it should be in the
		 * position of the "final" rectangle but the size of the "initial" 
		 * one */

		diff_w = (rects[1].g_w - rects[0].g_w) / 2;
		diff_h = (rects[1].g_h - rects[0].g_h) / 2;

		rects[2].g_x = rects[1].g_x + diff_w;
		rects[2].g_y = rects[1].g_y + diff_h;
		rects[2].g_w = rects[0].g_w;
		rects[2].g_h = rects[0].g_h;
		
		animate_rect(&rects[0], &rects[2]);
		animate_rect(&rects[2], &rects[1]);

		vsl_type (vdi_handle, lineattr[0]);
		vsl_color(vdi_handle, lineattr[1]);
		vswr_mode(vdi_handle, lineattr[2]);
		vsl_width(vdi_handle, lineattr[3]);
		
		
		/* If we have called the AES or the VDI then we MUST return zero.
		 * 
		 * To have the effect of doing something and then passing the call
		 * through to the underlying AES/VDI, make the call explicitly and
		 * then return zero. For example, in this case:
		 *
		 * ppd_setresult(gem(gb)) would do it.
		 *
		 */
		
		return 0;
	}
	if (func == GRAF_SHRINKBOX)	/* GRAF_GROWBOX in reverse */
	{
		vql_attributes(vdi_handle, lineattr);
		 
		vswr_mode(vdi_handle, 3);
		vsl_color(vdi_handle, 1);
		vsl_udsty(vdi_handle, 0x5555);
		vsl_type (vdi_handle, work_out[6] - 1);
		vsl_width(vdi_handle, 1);

		diff_w = (rects[1].g_w - rects[0].g_w) / 2;
		diff_h = (rects[1].g_h - rects[0].g_h) / 2;

		rects[2].g_x = rects[1].g_x + diff_w;
		rects[2].g_y = rects[1].g_y + diff_h;
		rects[2].g_w = rects[0].g_w;
		rects[2].g_h = rects[0].g_h;
		
		animate_rect(&rects[1], &rects[2]);
		animate_rect(&rects[2], &rects[0]);

		vsl_type (vdi_handle, lineattr[0]);
		vsl_color(vdi_handle, lineattr[1]);
		vswr_mode(vdi_handle, lineattr[2]);
		vsl_width(vdi_handle, lineattr[3]);
		
		return 0;
	}

	/* Return 1 if you have not called the AES or the VDI, and you want
	 * GEM to handle the call. */
	
	return 1;
}

/*------------------------------*/
/*		growbox_init			*/
/*------------------------------*/
WORD
growbox_init()
{
	WORD	i;
	WORD	work_in[11];
	WORD	attributes[10];

	gl_apid = appl_init(NULL);			/* initialize libraries	*/
	wind_update(BEG_UPDATE);
	for (i=0; i<10; i++)
	{
		work_in[i]=1;
	}
	work_in[10]=2;
	gem_handle = graf_handle(&gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);
	vdi_handle = gem_handle;
	v_opnvwk(work_in,&vdi_handle,work_out);	/* open virtual work stn*/
	vqt_attributes(vdi_handle, attributes);	/* get text attributes	*/
	type_size = attributes[7];		/* get system font hbox	*/
	
	ad_rmsg = ADDR((BYTE *) &gl_rmsg[0]);

	wind_get(DESK, WF_WXYWH, &gl_xfull, &gl_yfull, &gl_wfull, &gl_hfull);
	return(TRUE);
}


/*------------------------------*/
/*	GEMAIN			*/
/*------------------------------*/
WORD GEMAIN(WORD argc, BYTE *ARGV[])
{
	/* Estimate CPU speed. sleeptime = no. of calls to time() per second. */
	
	time_t t1, t2;
	sleeptime = 0;

	/* Wait until the start of a new second */
	
	t1 = time(NULL); while ((t2 = time(NULL)) == t1);

	/* Then count how many times we can call time() before the next second. 
     *
     * This will, of course, wrap round if we call it more than 4294967295
     * times per second. A processor running at 20-odd GHz might do this.
	 */
	
	while (t2 == time(NULL)) ++sleeptime;

	/* Insert our AES into the call chain */
	
	ppd_hookon(myaes, NULL, NULL);
	
	if (growbox_init())			/* initialization	*/
	{
		wind_update(END_UPDATE);
		growbox();
	}

	/* If this were not a DA, you would have to put in a ppd_hookoff() 
	 * around here. As it is a DA, you don't. */
	
	return 0;
}




