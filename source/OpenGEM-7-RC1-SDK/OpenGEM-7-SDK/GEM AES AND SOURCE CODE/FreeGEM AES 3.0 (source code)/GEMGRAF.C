/****************************************************************************
*   Copyright 1999, Caldera Thin Client Systems, Inc.                       *
*   This software is licensed under the GNU Public License                  *
*   For further information, please see LICENSE.TXT                         *
*                                                                           *
*   Historical Copyright                                                    *
*                                                                           *
*   Copyright (c) 1985,1987, 1991, 1992 Digital Research Inc.		    *
*   All rights reserved.						    *
*   The Software Code contained in this listing is proprietary to Digital   *
*   Research Inc., Monterey, California, and is covered by U.S. and other   *
*   copyright protection.  Unauthorized copying, adaption, distribution,    *
*   use or display is prohibited and may be subject to civil and criminal   *
*   penalties.  Disclosure to others is prohibited.  For the terms and      *
*   conditions of software code use, refer to the appropriate Digital       *
*   Research License Agreement.						    *
*****************************************************************************
*		      U.S. GOVERNMENT RESTRICTED RIGHTS			    *
*                    ---------------------------------                      *
*  This software product is provided with RESTRICTED RIGHTS.  Use, 	    *
*  duplication or disclosure by the Government is subject to restrictions   *
*  as set forth in FAR 52.227-19 (c) (2) (June, 1987) when applicable or    *
*  the applicable provisions of the DOD FAR supplement 252.227-7013 	    *
*  subdivision (b)(3)(ii) (May 1981) or subdivision (c)(1)(ii) (May 1987).  *
*  Contractor/manufacturer is Digital Research Inc. / 70 Garden Court /     *
*  BOX DRI / Monterey, CA 93940.					    *
*****************************************************************************
* $Header: g:/groups/panther/aes/rcs/gemgraf.c 4.4 92/04/09 15:00:33 sbc Exp $
* $Log:	gemgraf.c $
 * Revision 4.4  92/04/09  15:00:33  sbc
 * AES no longer frees the prev background for a new one. (done in dsk)
 * 
 * Revision 4.2  92/02/27  15:23:49  rsf
 * Conversion to medium model. Replace GEM.H with VIEWRUN.H
 * 
 * Revision 4.3  92/02/11  11:32:07  Fontes
 * Background image display implementation
 * 
 * Revision 4.2  92/01/03  13:16:26  Fontes
 * Susan's cleanup
 * 
 * Revision 4.0  91/09/05  11:53:26  system
 * Prototyping and cleanup plus fix to file selector code
 * 
*/

/****************************************************************************
* File:		gemgraf.c
*
* Description:	
*
* Build Info:	ndmake -f aes.mak
*
* Overview:	
*  
*****************************************************************************/

/*	GEMGRAF.C	04/11/84 - 09/17/85	Lee Lorenzen		*/
/*	merge High C vers. w. 2.2 		8/21/87		mdf	*/ 
/*	fix gr_gicon null text			11/18/87	mdf	*/
/*	910326	WHF	add 3D drawing routines & new color		*/

#include "aes.h"

#define ORGADDR 0x0L
#define	MAX_CCLINES 128

#define HILITE3d 1
#define MaxTH 4


/* [JCE 14-1-2000] Made the pseudo-round border a runtime flag
 * rather than a #define */
 
						/* in GSXBIND.C		*/

EXTERN THEGLO	D;

GLOBAL WORD	gl_height;

GLOBAL WORD     gl_nrows;
GLOBAL WORD     gl_ncols;

GLOBAL WORD	gl_wschar;
GLOBAL WORD	gl_hschar;

GLOBAL WORD	gl_wptschar;
GLOBAL WORD	gl_hptschar;

GLOBAL WORD	gl_wsptschar;
GLOBAL WORD	gl_hsptschar;


GLOBAL FDB		gl_src;
GLOBAL FDB		gl_dst;

GLOBAL WS		gl_ws;
GLOBAL WORD		contrl[12];
GLOBAL WORD		ptsin[20];


GLOBAL WORD		gl_mode;
GLOBAL WORD		gl_tcolor;
GLOBAL WORD		gl_lcolor;
GLOBAL WORD		gl_fis;
GLOBAL WORD		gl_patt;
GLOBAL WORD		gl_font;

GLOBAL GRECT		gl_rzero;

GLOBAL GRECT		gl_rscreen;
GLOBAL GRECT		gl_rcenter;
GLOBAL GRECT		gl_rmenu;

GLOBAL FDB FAR		*bmp_fdb = 0L;
GLOBAL ICONBLK FAR	*clr_icon = 0L;

/* 910327WHF begin */
/*
*	Color Category data structure: extend color capcbilities in AES
*	by supporting more sophisticated coloring algorithms (i.e.
*	dithered colors).
*/
#define CC_MAX	24

/* [JCE 21-9-1999] Categories 16-24 cannot be used by objects (they only
 *                 have a nibble for colour) but hold things like the 
 *                 3D shadow/highlight colours and the colours for the
 *                 radio button dot.
 * [JCE 14-1-2000] Category 19 CC_NAMETEXT is the colour of window title
 *                 text: Foreground = text colour  Background = border colour
 *                       Fill style = 1 for solid, 0 for transparent.
 *
 */

CLRCAT cc_s[CC_MAX] = 
   {					/* default colors: solid */
	{ 0,	0,	1,	0},	/* color category 0 */
	{ 1,	0,	1,	0},	/* color category 1 */
	{ 2,	0,	1,	0},	/* color category 2 */
	{ 3,	0,	1,	0},	/* color category 3 */
	{ 4,	0,	1,	0},	/* color category 4 */
	{ 5,	0,	1,	0},	/* color category 5 */
	{ 6,	0,	1,	0},	/* color category 6 */
	{ 7,	0,	1,	0},	/* color category 7 */
	{ BLUE,   WHITE, FIS_PATTERN, 6}, /* color category 8: CC_NAME */
	{ BLUE,   CYAN,  FIS_PATTERN, 3}, /* color category 9: CC_SLIDER */
	{ WHITE,  BLUE,  FIS_SOLID,   4}, /* color category 10: CC_DESKTOP */
	{ LWHITE, 0,     FIS_SOLID,   0}, /* color category 11: CC_BUTTON */
	{ LWHITE, WHITE, FIS_PATTERN, 4}, /* color category 12: CC_INFO */
	{ RED,    0,     FIS_SOLID,   0}, /* color category 13: CC_ALERT */
	{ CYAN,   0,     FIS_SOLID,   0}, /* color category 14: CC_SLCTDNAME */
	{ 15,	  0,	 1,	          0}, /* color category 15 */

	{ WHITE,  LBLACK,1, 0},	/* 16: CC_3DSHADE   3D highlight + shadow */ 
	{ GREEN,  LGREEN,1,	0},	/* 17: CC_RADIO     Radio "dot" inside, outside */
	{ RED,	  RED,	1,	0},	/* 18: CC_CHECK     Checkbox "tick" */
	{ BLACK,  BLACK,1,	0},	/* 19: CC_NAMETEXT  Window title text */
	{ BLACK,  BLACK,1,	0},	/* 20: CC_NNAMETEXT Inactive window title text */
	{ BLACK,  BLACK,0,	0},	/* 21: CC_INFOTEXT  Info bar text */
	{ 6,	  0,	1,	0},	/* color category 22 */
	{ 7,	  0,	1,	0},	/* color category 23 */
	};

/*
*	Color category setting routines (handles dithered colors)
*/
	VOID
gsx_setclr( clr, fgclr, bkclr, style, pattern )
WORD	clr, fgclr, bkclr, style, pattern;
{
	if( clr<0 || clr>=CC_MAX ) return;

	cc_s[clr].cc_foreground = fgclr;
	cc_s[clr].cc_background = bkclr;
	cc_s[clr].cc_style = style;
	cc_s[clr].cc_pattern = pattern;

	if (clr == CC_NAMETEXT)
	{
		sys_fg = ((fgclr << 8) & 0xF00) | ((bkclr << 12) & 0xF000);
		sys_tr = style ? 0x80 : 0;
	}
	if (clr == CC_NNAMETEXT)
	{
		syn_fg = ((fgclr << 8) & 0xF00) | ((bkclr << 12) & 0xF000);
		syn_tr = style ? 0x80 : 0;
	}
	if (clr == CC_INFOTEXT)
	{
		syi_fg = ((fgclr << 8) & 0xF00) | ((bkclr << 12) & 0xF000);
		syi_tr = style ? 0x80 : 0;
	}
}
/* 910327WHF end */



/*
*	Routine to set the clip rectangle.  If the w,h of the clip
*	is 0, then no clip should be set.  Ohterwise, set the 
*	appropriate clip.
*/
	VOID
gsx_sclip(pt)
	GRECT		*pt;
{
	r_get(pt, &gl_xclip, &gl_yclip, &gl_wclip, &gl_hclip);

	if ( gl_wclip && gl_hclip )
	{
	  ptsin[0] = gl_xclip;
	  ptsin[1] = gl_yclip;
	  ptsin[2] = gl_xclip + gl_wclip - 1;
	  ptsin[3] = gl_yclip + gl_hclip - 1;
	  vst_clip( TRUE, &ptsin[0]);
	}
	else
	  vst_clip( FALSE, &ptsin[0]);
}


/*
*	Routine to get the current clip setting
*/
	VOID
gsx_gclip(pt)
	GRECT		*pt;
{
	r_set(pt, gl_xclip, gl_yclip, gl_wclip, gl_hclip);
}


/*
*	Routine to set if the passed in rectangle intersects the
*	current clip rectangle
*/
	WORD
gsx_chkclip(pt)
	GRECT		*pt;
{
						/* if clipping is on	*/
	if (gl_wclip && gl_hclip)
	{
 	  if ((pt->g_y + pt->g_h) < gl_yclip)
	    return(FALSE);			/* all above	*/
	  if ((pt->g_x + pt->g_w) < gl_xclip) 
	    return(FALSE);			/* all left	*/
	  if ((gl_yclip + gl_hclip) <= pt->g_y)
	    return(FALSE);			/* all below	*/
	  if ((gl_xclip + gl_wclip) <= pt->g_x)
	    return(FALSE);			/* all right	*/
	}
	return(TRUE);
}



	VOID
gsx_xline( WORD ptscount, WORD *ppoints )
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
	    linexy = ( *ppoints < *( ppoints + 2 )) ? ppoints : ppoints + 2;
	    st = hztltbl[( *(linexy + 1) & 1)];
	  }
	  vsl_udsty( st );
	  v_pline( 2, ppoints );
	  ppoints += 2;
	}
	vsl_udsty( 0xffff );
}	


/* RSF: Not used
*	Routine to draw a certain number of points in a polyline
*	relative to a given x,y offset.
*
	VOID
gsx_pline(offx, offy, cnt, pts)
	WORD		offx, offy;
	WORD		cnt;
	WORD		*pts;
{
	WORD		i, j;

	for (i=0; i<cnt; i++)
	{
	  j = i * 2;
	  ptsin[j] = offx + pts[j];
	  ptsin[j+1] = offy + pts[j+1];
	}

	gsx_xline( cnt, &ptsin[0]);
}
*/

/*
*	Routine to draw a clipped polyline, hiding the mouse as you
*	do it.
*/
	VOID
gsx_cline(x1, y1, x2, y2)
	UWORD		x1, y1, x2, y2;
{
	WORD pts[4];	/* [JCE] The "stack hack" doesn't work in Pacific */

	pts[0] = x1; pts[1] = y1; pts[2] = x2; pts[3] = y2;

	gsx_moff();
	v_pline( 2, pts );
	gsx_mon();
}



/*
*	Routine to set the text, writing mode, and color attributes.
*/
	VOID
gsx_attr(text, mode, color)
	UWORD		text, mode, color;
{
	WORD		tmp;

	tmp = intin[0];
	contrl[1] = 0;
	contrl[3] = 1;
	contrl[6] = gl_handle;
	if (mode != gl_mode)
	{
	  contrl[0] = SET_WRITING_MODE;
	  intin[0] = gl_mode = mode;
	  gsx2();
	}
	contrl[0] = FALSE;
	if (text)
	{
	  if (color != gl_tcolor)
	  {
	    contrl[0] = S_TEXT_COLOR;
	    gl_tcolor = color;
	  }
	}	
	else
	{
	  if (color != gl_lcolor)
	  {
	    contrl[0] = S_LINE_COLOR;
	    gl_lcolor = color;
	  }
	}
	if (contrl[0])
	{
	  intin[0] = color;
	  gsx2();
	}
	intin[0] = tmp;
}


/*
*	Routine to set up the points for drawing a box.
*/
	VOID
gsx_bxpts(pt)
	GRECT		*pt;
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


/*
*	Routine to draw a box using the current attributes.
*/
	VOID
gsx_box(pt)
	GRECT		*pt;
{
	gsx_bxpts(pt);
	v_pline( 5, &ptsin[0] );
}


/*
*	Routine to draw a box that will look right on a dithered
*	surface.
*/
	VOID
gsx_xbox(pt)
	GRECT		*pt;
{
	gsx_bxpts(pt);
	gsx_xline( 5, &ptsin[0] );
}


/*
*	Routine to draw a portion of the corners of a box that will
*	look right on a dithered surface.
*/
	VOID
gsx_xcbox(pt)
	GRECT		*pt;
{
	WORD		wa, ha;

	wa = 2 * gl_wbox;
	ha = 2 * gl_hbox;
	ptsin[0] = pt->g_x;
	ptsin[1] = pt->g_y + ha;
	ptsin[2] = pt->g_x;
	ptsin[3] = pt->g_y;
	ptsin[4] = pt->g_x + wa;
	ptsin[5] = pt->g_y;
	gsx_xline( 3, &ptsin[0] );
	ptsin[0] = pt->g_x + pt->g_w - wa;
	ptsin[1] = pt->g_y;
	ptsin[2] = pt->g_x + pt->g_w - 1;
	ptsin[3] = pt->g_y;
	ptsin[4] = pt->g_x + pt->g_w - 1;
	ptsin[5] = pt->g_y + ha;
	gsx_xline( 3, &ptsin[0] );
	ptsin[0] = pt->g_x + pt->g_w - 1;
	ptsin[1] = pt->g_y + pt->g_h - ha;
	ptsin[2] = pt->g_x + pt->g_w - 1;
	ptsin[3] = pt->g_y + pt->g_h - 1;
	ptsin[4] = pt->g_x + pt->g_w - wa;
	ptsin[5] = pt->g_y + pt->g_h - 1;
	gsx_xline( 3, &ptsin[0] );
	ptsin[0] = pt->g_x + wa;
	ptsin[1] = pt->g_y + pt->g_h - 1;
	ptsin[2] = pt->g_x;
	ptsin[3] = pt->g_y + pt->g_h - 1;
	ptsin[4] = pt->g_x;
	ptsin[5] = pt->g_y + pt->g_h - ha;
	gsx_xline( 3, &ptsin[0] );
}


/*
*	Routine to set the writing mode.
*/
MLOCAL	VOID
gsx_mode(UWORD mode)
{
	WORD		tmp;

	tmp = intin[0];		/* why is this being saved? 910327WHF */
	contrl[1] = 0;
	contrl[3] = 1;
	contrl[6] = gl_handle;
	if (mode != gl_mode)
	{
	  contrl[0] = SET_WRITING_MODE;
	  intin[0] = gl_mode = mode;
	  gsx2();
	}
	intin[0] = tmp;
}


/*
*	Routine to fix up the MFDB of a particular raster form
*/
	VOID
gsx_fix(pfd, theaddr, wb, h)
	FDB		*pfd;
	LPBYTE		theaddr;
	WORD		wb, h;
{
	if (theaddr == ORGADDR)
	{
	  pfd->fd_w = gl_ws.ws_xres + 1;
	  pfd->fd_wdwidth = pfd->fd_w / 16;
	  pfd->fd_h = gl_ws.ws_yres + 1;
	  pfd->fd_nplanes = gl_nplanes;
	}
	else
	{
	  pfd->fd_wdwidth = wb / 2;
	  pfd->fd_w = wb * 8;
	  pfd->fd_h = h;
	  pfd->fd_nplanes = 1;
	}
	pfd->fd_stand = FALSE;
	pfd->fd_addr = theaddr;
}


/*
*	Routine to blit, to and from a specific area
*/
	VOID
gsx_blt(saddr, sx, sy, swb, daddr, dx, dy, dwb, w, h, rule, fgcolor, bgcolor)
	LPBYTE		saddr;
	UWORD		sx, sy, swb;
	LPBYTE		daddr;
	UWORD		dx, dy, dwb;
	UWORD		w, h, rule;
	WORD		fgcolor, bgcolor;
{
	gsx_fix(&gl_src, saddr, swb, h);
	gsx_fix(&gl_dst, daddr, dwb, h);

	gsx_moff();
	ptsin[0] = sx;
	ptsin[1] = sy;
	ptsin[2] = sx + w - 1;
	ptsin[3] = sy + h - 1;
	ptsin[4] = dx;
	ptsin[5] = dy;
	ptsin[6] = dx + w - 1;
	ptsin[7] = dy + h - 1 ;
	if (fgcolor == -1)
	  vro_cpyfm( rule, &ptsin[0], &gl_src, &gl_dst);
	else
	  vrt_cpyfm( rule, &ptsin[0], &gl_src, &gl_dst, fgcolor, bgcolor);
	gsx_mon();
}


/*
*	Routine to blit around something on the screen
*/
	VOID
bb_screen(scrule, scsx, scsy, scdx, scdy, scw, sch)
	WORD		scrule, scsx, scsy, scdx, scdy, scw, sch;
{
	gsx_blt(0x0L, scsx, scsy, 0, 
		0x0L, scdx, scdy, 0,
		scw, sch, scrule, -1, -1);
}


/*
*	Routine to transform a standard form to device specific
*	form.
*/
	VOID
gsx_trans(saddr, swb, daddr, dwb, h)
	LPBYTE		saddr;
	UWORD		swb;
	LPBYTE		daddr;
	UWORD		dwb;
	UWORD		h;
{
	gsx_fix(&gl_src, saddr, swb, h);
	gl_src.fd_stand = TRUE;
	gl_src.fd_nplanes = 1;

	gsx_fix(&gl_dst, daddr, dwb, h);
	vrn_trnfm( &gl_src, &gl_dst );
}

/*
*	Routine to initialize all the global variables dealing
*	with a particular workstation open
*/
	VOID
gsx_start()
{
	WORD		char_height, nc;

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
	char_height = gl_ws.ws_chminh;
	vst_height( char_height, &gl_wsptschar, &gl_hsptschar, 
				&gl_wschar, &gl_hschar );
	char_height = gl_ws.ws_chmaxh;
	vst_height( char_height, &gl_wptschar, &gl_hptschar, 
				&gl_wchar, &gl_hchar );
	gl_ncols = gl_width / gl_wchar;
	gl_nrows = gl_height / gl_hchar;
	gl_hbox = gl_hchar + 3;
	gl_wbox = (gl_hbox * gl_ws.ws_hpixel) / gl_ws.ws_wpixel;
	vsl_type( 7 );
	vsl_width( 1 );
	vsl_udsty( 0xffff );
	r_set(&gl_rscreen, 0, 0, gl_width, gl_height);
	r_set(&gl_rfull, 0, gl_hbox, gl_width, (gl_height - gl_hbox));
	r_set(&gl_rzero, 0, 0, 0, 0);
	r_set(&gl_rcenter, (gl_width-gl_wbox)/2, (gl_height-(2*gl_hbox))/2, 
			gl_wbox, gl_hbox);
	r_set(&gl_rmenu, 0, 0, gl_width, gl_hbox);
	ad_intin = (LPWORD)ADDR(&intin[0]);
	if( gl_nplanes <= 1 )
	{
		/* handle monochrome color categories */
		gsx_setclr( CC_BUTTON,    WHITE, 0, FIS_SOLID, 0 );
		gsx_setclr( CC_DESKTOP,   WHITE, BLACK, FIS_PATTERN, 4 );
		gsx_setclr( CC_NAME,      WHITE, BLACK, FIS_PATTERN, 6 );
		gsx_setclr( CC_SLCTDNAME, WHITE, BLACK, FIS_PATTERN, 2 );
		gsx_setclr( CC_SLIDER,    WHITE, BLACK, FIS_PATTERN, 2 );
		gsx_setclr( CC_INFO,      WHITE, 0, FIS_SOLID, 0 );

		gsx_setclr( CC_3DSHADE,   WHITE, BLACK, FIS_SOLID, 0 );
		gsx_setclr( CC_RADIO,     WHITE, BLACK, FIS_SOLID, 0 );
		gsx_setclr( CC_CHECK,     BLACK, BLACK, FIS_SOLID, 0 );
		gl_domono = TRUE;
	} else	gl_domono = FALSE;
}

/*
*	Routine to do a filled bit blit, (a rectangle).
*/
	VOID
bb_fill(mode, fis, patt, hx, hy, hw, hh)
	WORD		mode, fis, patt, hx, hy, hw, hh;
{

	gsx_fix(&gl_dst, 0x0L, 0, 0);
	ptsin[0] = hx;
	ptsin[1] = hy;
	ptsin[2] = hx + hw - 1;
	ptsin[3] = hy + hh - 1;

	gsx_attr(TRUE, mode, gl_tcolor);
	if (fis != gl_fis)
	{
	  vsf_interior( fis);
	  gl_fis = fis;
	}
	if (patt != gl_patt)
	{
	  vsf_style( patt );
	  gl_patt = patt;
	}
	vr_recfl( (WORD *)&ptsin[0], (WORD *)&gl_dst );
}


	UWORD
ch_width(fn)
	WORD		fn;
{
	if (fn == IBM)
	  return(gl_wchar);
	if (fn == SMALL)
	  return(gl_wschar);
	return(0);
}



	UWORD
ch_height(fn)
	WORD		fn;
{
	if (fn == IBM)
	  return(gl_hchar);
	if (fn == SMALL)
	  return(gl_hschar);
	return(0);
}

	VOID
gsx_tcalc(font, ptext, ptextw, ptexth, pnumchs)
	WORD		font;
	LPBYTE		ptext;
	WORD		*ptextw;
	WORD		*ptexth;
	WORD		*pnumchs;
{
	WORD		wc, hc;

	wc = ch_width(font);
	hc = ch_height(font);
						/* figure out the	*/
						/*   width of the text	*/
						/*   string in pixels	*/

	*pnumchs = LBWMOV(ad_intin, ptext);
	*ptextw = min(*ptextw, *pnumchs * wc );
						/* figure out the height*/
						/*   of the text	*/
	*ptexth = min(*ptexth, hc );
	if (*ptexth / hc)
	  *pnumchs = min(*pnumchs, *ptextw / wc );
	else
	  *pnumchs = 0;
}


	VOID
gsx_tblt(tb_f, x, y, tb_nc)
	WORD		tb_f;
	WORD		x, y;
	WORD		tb_nc;
{
	WORD		pts_height;

	if (tb_f == IBM)
	{
	  if (tb_f != gl_font)
	  {
	    pts_height = gl_ws.ws_chmaxh;
	    vst_height( pts_height, &gl_wptschar, &gl_hptschar, 
				&gl_wchar, &gl_hchar );
	    gl_font = tb_f;
	  }
	  y += gl_hptschar - 1;
	}

	if (tb_f == SMALL)
	{
	  if (tb_f != gl_font)
	  {
	    pts_height = gl_ws.ws_chminh;
	    vst_height( pts_height, &gl_wsptschar, &gl_hsptschar, 
				&gl_wschar, &gl_hschar );
	    gl_font = tb_f;
	  }
	  y += gl_hsptschar - 1;
	}

	contrl[0] = 8;		/* TEXT */
	contrl[1] = 1;
	contrl[6] = gl_handle;
	ptsin[0] = x;
	ptsin[1] = y;
	contrl[3] = tb_nc;
	gsx2();
}






/*
*	Routine to draw a string of graphic text.
*/
	VOID
gr_gtext(just, font, ptext, pt)
	WORD		just;
	WORD		font;
	LPBYTE		ptext;
	GRECT		*pt;
{
	WORD		numchs;
	GRECT		t;
						/* figure out where &	*/
						/*   how to put out	*/
						/*   the text		*/
	rc_copy(pt, &t);
	numchs = gr_just(just, font, ptext, t.g_w, t.g_h, &t);
	if (numchs > 0)
	  gsx_tblt(font, t.g_x, t.g_y, numchs);
}



/*
*	Routine to crack out the border color, text color, inside pattern,
*	and inside color from a single color information word.
*/
	VOID
gr_crack(color, pbc, ptc, pip, pic, pmd)
	UWORD		color;
	WORD		*pbc, *ptc, *pip, *pic, *pmd;
{
						/* 4 bit encoded border	*/
						/*   color 		*/
	*pbc = (LHIBT(color) >> 4) & 0x0f;
						/* 4 bit encoded text	*/
						/*   color		*/
	*ptc = LHIBT(color) & 0x0f;
						/* 3 bit encoded pattern*/
	*pip = (LLOBT(color) >> 4) & 0x0f;
						/* 4th bit used to set	*/
						/*   text writing mode	*/
	if (*pip & 0x08)
	{
	  *pip &= 0x07;
	  *pmd = MD_REPLACE;
	}
	else
	  *pmd = MD_TRANS;
						/* 4 bit encoded inside	*/
						/*   color		*/
	*pic = LLOBT(color) & 0x0f;
}


        VOID
gr_gblt(pimage, pi, col1, col2)
	LPBYTE		pimage;
	GRECT		*pi;
	WORD		col1, col2;
{
	gsx_blt(pimage, 0, 0, pi->g_w/8, 0x0L, pi->g_x, pi->g_y, 
			gl_width/8, pi->g_w, pi->g_h, MD_TRANS,
			col1, col2); 
}


/*
*	Routine to draw an icon, which is a graphic image with a text
*	string underneath it.
*/
	VOID
gr_gicon(state, pmask, pdata, ptext, ch, chx, chy, pi, pt)
	WORD		state;
	LPBYTE		pmask;
	LPBYTE		pdata;
	LPBYTE		ptext;
	WORD		ch, chx, chy;
	GRECT		*pi;
	GRECT		*pt;
{
	WORD		ifgcol, ibgcol;
	WORD		tfgcol, tbgcol, tmp;
						/* crack the color/char	*/
						/*   definition word	*/
	tfgcol = ifgcol = (ch >> 12) & 0x000f;
	tbgcol = ibgcol = (ch >> 8) & 0x000f;
	ch &= 0x00ff;
						/* invert if selected	*/
	if (state & SELECTED)
	{
	  tmp = tfgcol;
	  tfgcol = tbgcol;
	  tbgcol = tmp;
	  if ( !(state & DRAW3D) )
	  {
	    tmp = ifgcol;
	    ifgcol = ibgcol;
	    ibgcol = tmp;
	  }
	}
						/* do mask unless its on*/
						/* a white background	*/
	if ( !( (state & WHITEBAK) && (ibgcol == WHITE) ) )
	  gr_gblt(pmask, pi, ibgcol, ifgcol);

	if ( !( (state & WHITEBAK) && (tbgcol == WHITE) ) )
	{
	  if (pt->g_w)
	    gr_rect(tbgcol, IP_SOLID, pt);
	}
						/* draw the data	*/
	gr_gblt(pdata, pi, ifgcol, ibgcol);
	
	if ( (state & SELECTED) &&
	     (state & DRAW3D) )
	{
	  pi->g_x--;
	  pi->g_y--;
	  gr_gblt(pmask, pi, ifgcol, ibgcol);
	  pi->g_x += 2;
	  pi->g_y += 2;
	  gr_gblt(pmask, pi, ifgcol, ibgcol);
	  pi->g_x--;
	  pi->g_y--;
	}
						/* draw the character	*/
	gsx_attr(TRUE, MD_TRANS, ifgcol);
	if ( ch )
	{
	  intin[0] = ch;
	  gsx_tblt(SMALL, pi->g_x+chx, pi->g_y+chy, 1);
	}

	
						/* draw the label	*/
	gsx_attr(TRUE, MD_TRANS, tfgcol);
	gr_gtext(TE_CNTR, SMALL, ptext, pt);
}


/*
*	Routine to draw a box of a certain thickness using the current
*	attribute settings
*/
	VOID
gr_box(x, y, w, h, th)
	WORD		x, y, w, h, th;
{
	GRECT		t, n;

	r_set(&t, x, y, w, h);
	if (th != 0)
	{
	  if (th < 0)
	    th--;
	  gsx_moff();
	  do
	  {
	    th += (th > 0) ? -1 : 1;
	    rc_copy(&t, &n);
	    gr_inside(&n, th);
	    gsx_box(&n);
	  } while (th != 0);
	  gsx_mon();
	}
}


/*
*	Routine to filled rectangle using the color category scheme.
*/
	VOID
grcc_rect( clr, pt )
WORD	clr;
GRECT	*pt;
{
	WORD	ii;
#if 0
	WORD	whats_left;

	if( clr<0 || clr>=CC_MAX ) return;	/* range checking */

	if ((ii=cc_s[clr].cc_style) != gl_fis)
	{
	  vsf_interior( ii );
	  gl_fis = ii;
	}
	if ((ii=cc_s[clr].cc_pattern) != gl_patt)
	{
	  vsf_style( ii );
	  gl_patt = ii;
	}

	gsx_fix(&gl_dst, 0x0L, 0, 0);		/* grabbed from bb_fill() */
	ptsin[0] = pt->g_x;
	ptsin[1] = pt->g_y;
	ptsin[2] = pt->g_x + pt->g_w - 1;
	ptsin[3] = pt->g_y;			/* add to this in loop */
	whats_left = pt->g_h-1;

	do {
	  if( (cc_s[clr].cc_style == FIS_SOLID) || (whats_left < MAX_CCLINES) )
		ii = whats_left;
	  else	ii = MAX_CCLINES;
	  whats_left -= ii;

	  ptsin[3] = ptsin[1] + ii;

	  /* render the foreground color */
	  vsf_color( cc_s[clr].cc_foreground );
	  gsx_mode( MD_TRANS );
	  vr_recfl( ptsin, (WORD*)&gl_dst );

	  if( cc_s[clr].cc_style != FIS_SOLID )
	  {
	    /* render the background color */
	    vsf_color( cc_s[clr].cc_background );
	    gsx_mode( MD_ERASE );
	    vr_recfl( ptsin, (WORD*)&gl_dst );
	  }
	  ptsin[1] += ii;
	} while(whats_left);
#else

	if( clr<0 || clr>=CC_MAX ) return;	/* range checking */

	if ((ii=cc_s[clr].cc_style) != gl_fis)
	{
	  vsf_interior( ii );
	  gl_fis = ii;
	}
	if ((ii=cc_s[clr].cc_pattern) != gl_patt)
	{
	  vsf_style( ii );
	  gl_patt = ii;
	}

	gsx_fix(&gl_dst, 0x0L, 0, 0);		/* grabbed from bb_fill() */
	ptsin[0] = pt->g_x;
	ptsin[1] = pt->g_y;
	ptsin[2] = pt->g_x + pt->g_w - 1;
	ptsin[3] = pt->g_y + pt->g_h - 1;

	  /* render the foreground color */
	  vsf_color( cc_s[clr].cc_foreground );
	  gsx_mode( MD_TRANS );
	  vr_recfl( ptsin, (WORD*)&gl_dst );

	  if( cc_s[clr].cc_style != FIS_SOLID )
	  {
	    /* render the background color */
	    vsf_color( cc_s[clr].cc_background );
	    gsx_mode( MD_ERASE );
	    vr_recfl( ptsin, (WORD*)&gl_dst );
	  }
#endif
}


/*
*	Routine to convert a rectangle to its inside dimensions.
*/
	VOID
gr_inside(pt, th)
	GRECT		*pt;
	WORD		th;
{
	pt->g_x += th;
	pt->g_y += th;
	pt->g_w -= ( 2 * th );
	pt->g_h -= ( 2 * th );
}


/*
*	Routine to draw a colored, patterned, rectangle.
*/
	VOID
gr_rect(icolor, ipattern, pt)
	UWORD		icolor;
	UWORD		ipattern;
	GRECT		*pt;
{
	WORD		fis;

	fis = FIS_PATTERN;
	if (ipattern == IP_HOLLOW)
	  fis = FIS_HOLLOW;
	else if (ipattern == IP_SOLID)
	  fis = FIS_SOLID;

	vsf_color(icolor);
	bb_fill(MD_REPLACE, fis, ipattern, 
	  	pt->g_x, pt->g_y, pt->g_w, pt->g_h);
}


/*
*	Routine to adjust the x,y starting point of a text string
*	to account for its justification.  The number of characters
*	in the string is also returned.
*/
	WORD
gr_just(just, font, ptext, w, h, pt)
	WORD		just;
	WORD		font;
	LPBYTE		ptext;
	WORD		w, h;
	GRECT		*pt;
{
	WORD		numchs, diff;
						/* figure out the	*/
						/*   width of the text	*/
						/*   string in pixels	*/
	gsx_tcalc(font, ptext, &pt->g_w, &pt->g_h, &numchs);

	h -= pt->g_h;
	if ( h > 0 )				/* check height		*/
	  pt->g_y += (h + 1) / 2;

	w -= pt->g_w;
	if ( w > 0 )
	{
						/* do justification	*/
	  if (just == TE_CNTR)
	    pt->g_x += (w + 1) / 2;
	  else if (just == TE_RIGHT)
	    pt->g_x += w;
						/* try to byte align	*/
	  if ( (font == IBM) &&
	       (w > 16) &&
	       ((diff = (pt->g_x & 0x0007)) != 0))
	  {
	    if (just == TE_LEFT)
	      pt->g_x += 8 - diff;
	    else if (just == TE_CNTR)
	    {
	      if (diff > 3)
	        diff -= 8;
	      pt->g_x -= diff;
	    }
	    else if (just == TE_RIGHT)
	      pt->g_x -= diff;
	  }
	}

	return(numchs);
}


/*
*	Routine to draw a color icon, which is a Windows icon image with text
*	string underneath it.
*/
	VOID
gr_clricon(state, pmask, pdata, ptext, ch, chx, chy, pi, pt)
	WORD		state;
	LPBYTE		pmask;
	LPBYTE		pdata;
	LPBYTE		ptext;
	WORD		ch, chx, chy;
	GRECT		*pi;
	GRECT		*pt;
{
	WORD		ifgcol, ibgcol;
	WORD		tfgcol, tbgcol, tmp;
						/* crack the color/char	*/
						/*   definition word	*/
	tfgcol = ifgcol = (ch >> 12) & 0x000f;
	tbgcol = ibgcol = (ch >> 8) & 0x000f;
	ch &= 0x00ff;
						/* invert if selected	
						 * [JCE: Panther bug?] 
						 * These aren't used for the icon itself
						 * so it won't invert */
	if (state & SELECTED)
	{
	  tmp = tfgcol;
	  tfgcol = tbgcol;
	  tbgcol = tmp;
	  if ( !(state & DRAW3D) )
	  {
	    tmp = ifgcol;
	    ifgcol = ibgcol;
	    ibgcol = tmp;
	  }
	}
						/* draw the data	*/
	/* [JCE 8-8-1999] Passing an extra "selected" parameter */
	render_ico( (FDB far *)pmask, (FDB far *)pdata, pi->g_x, pi->g_y, state & SELECTED);
		
						/* draw the character	*/
	gsx_attr(TRUE, MD_TRANS, ifgcol);
	if ( ch )
	{
	  intin[0] = ch;
	  gsx_tblt(SMALL, pi->g_x+chx, pi->g_y+chy, 1);
	}

						/* draw the label	*/
	if ( !( (state & WHITEBAK) && (tbgcol == WHITE) ) )
	{
	  if (pt->g_w)
	    gr_rect(tbgcol, IP_SOLID, pt);
	}

	
	gsx_attr(TRUE, MD_TRANS, tfgcol);
	gr_gtext(TE_CNTR, SMALL, ptext, pt);
}


/*
*	Routine to draw the image contained in the MFDB passed in as the
*	desktop image.  We have usurped fd_r1 to indicate whether
*	the image is to be centered or tiled.
*/
	VOID
gr_dtmfdb(FDB far *spec, GRECT *t)
{
	GRECT	g;
	
	rc_copy(t, &g);

	switch (spec->fd_r1)
	{
		case DT_TILE:
			while (g.g_y < t->g_h)
			{
				g.g_x = t->g_x;
				while (g.g_x < t->g_w)
				{
					/* Opportunity to shave off a	*/
					/* bit by doing  render_bmp's	*/
					/* work ourselves.		*/
					render_bmp(spec, g.g_x, g.g_y);
					g.g_x += spec->fd_w;
				}
				g.g_y += spec->fd_h;
			}
			break;
		case DT_CENTER:
#if 0
			grcc_rect(CC_DESKTOP, t);
#else
			r_set(&g, t->g_x, t->g_y, t->g_w, 
			    (t->g_h - spec->fd_h)/2);
			grcc_rect(CC_DESKTOP, &g);			
			r_set(&g, t->g_x, t->g_y + (t->g_h - spec->fd_h)/2,
			    (t->g_w - spec->fd_w)/2, spec->fd_h);
			grcc_rect(CC_DESKTOP, &g);			
			r_set(&g, t->g_x + spec->fd_w + (t->g_w-spec->fd_w)/2,
			    t->g_y + (t->g_h - spec->fd_h)/2,
			    (t->g_w - spec->fd_w)/2, spec->fd_h);
			grcc_rect(CC_DESKTOP, &g);			
			r_set(&g, t->g_x, 
			    t->g_y + ((t->g_h - spec->fd_h)/2) + spec->fd_h,
			    t->g_w, (t->g_h - spec->fd_h)/2);
			grcc_rect(CC_DESKTOP, &g);			
#endif
			render_bmp(spec, t->g_x + ((t->g_w - spec->fd_w)/2),
				t->g_y + ((t->g_h - spec->fd_h)/2));
			break;
	}
}


/*
*	Routine to draw a (single thickness) border with a 3d edge effect
*/
MLOCAL	VOID gsx_3xpts( GRECT *pt, WORD reverse)
{
	ptsin[0] = pt->g_x + pt->g_w -1;
	ptsin[1] = pt->g_y;
	ptsin[2] = pt->g_x;
	ptsin[3] = pt->g_y;
	ptsin[4] = pt->g_x;
	ptsin[5] = pt->g_y + pt->g_h - 1;
	gsx_attr( FALSE, MD_REPLACE, 
	          reverse ? cc_s[CC_3DSHADE].cc_background : 
	                    cc_s[CC_3DSHADE].cc_foreground );
	v_pline( 3, &ptsin[0] );

	ptsin[2] = ptsin[0];
	ptsin[3] = ptsin[5];
	gsx_attr( FALSE, MD_REPLACE, 
	          reverse ? cc_s[CC_3DSHADE].cc_foreground : 
	                    cc_s[CC_3DSHADE].cc_background );
	v_pline( 3, &ptsin[0] );
}


/*
*	Routine to draw pseudo-rounded small box
*/
MLOCAL	VOID
gsx_psbox(GRECT *pt)
{
	ptsin[0] = pt->g_x + 1;
	ptsin[1] = pt->g_y;
	ptsin[2] = pt->g_x + pt->g_w - 2;
	ptsin[3] = pt->g_y;
	ptsin[4] = pt->g_x + pt->g_w - 1;
	ptsin[5] = pt->g_y + 1;
	ptsin[6] = pt->g_x + pt->g_w - 1;
	ptsin[7] = pt->g_y + pt->g_h - 2;
	ptsin[8] = pt->g_x + pt->g_w - 2;
	ptsin[9] = pt->g_y + pt->g_h - 1;
	ptsin[10] = pt->g_x+1;
	ptsin[11] = pt->g_y + pt->g_h - 1;
	ptsin[12] = pt->g_x;
	ptsin[13] = pt->g_y + pt->g_h - 2;
	ptsin[14] = pt->g_x;
	ptsin[15] = pt->g_y + 1;
	ptsin[16] = pt->g_x + 1;
	ptsin[17] = pt->g_y;
	v_pline( 9, &ptsin[0] );
}


/*
*	Routine to draw a box border with 3D effects
*	of a certain thickness.
*	Note: this adds +1 to thickness of 3D objects.
*/
VOID gr_3dbox( GRECT *tt, WORD th, WORD do_border, WORD reverse)
{
	GRECT	nn;
	WORD	ii;

	rc_copy(tt, &nn);
	if( th < 0 )
	{
		gr_inside( &nn, th );
		th = -th;
	}

	/* draw a black border */
	if (do_border)
	{
		gsx_attr( FALSE, MD_REPLACE, BLACK );

		if (gl_opts.rounded) gsx_psbox( &nn );
		else                 gsx_box( &nn );

	}
	else 
	{
		gr_inside(&nn, -1); /* Inflate the rectangle over where the
	                          * border would have been */
	}
#if MaxTH
	for( ; th > MaxTH; th-- )
	{
	    gr_inside( &nn, 1 );
	    gsx_box( &nn );
	}
#endif
	
	for( ii=0; ii<th; ++ii )
	{
	    gr_inside( &nn, 1 );
	    gsx_3xpts( &nn, reverse );
	}

#if HILITE3d
	gsx_attr( FALSE, MD_REPLACE, 
	          reverse ? cc_s[CC_3DSHADE].cc_background : 
	                    cc_s[CC_3DSHADE].cc_foreground );
	ptsin[0] = nn.g_x+nn.g_w-1; ptsin[1] = nn.g_y+nn.g_h-1;
	ptsin[2] = ptsin[0]+th-1; ptsin[3] = ptsin[1]+th-1;

	if (!gl_opts.rounded)
	{
		if (ptsin[2] > ptsin[0] && ptsin[3] > ptsin[1])
		{
			ptsin[2]--;
			ptsin[3]--;
			v_pline( 2, &ptsin[0] );
		}
	}
	else v_pline( 2, &ptsin[0] );
#endif
	gsx_attr( FALSE, MD_REPLACE, BLACK );	/* because it works */
}


/*
*	Routine to draw a selected 3D box border
*/
VOID gr_3dpress( GRECT *tt, WORD th, WORD do_border )
{	
	GRECT	nn;

	rc_copy(tt, &nn);
	if( th < 0 )
	{
		gr_inside(&nn,th);
		th = -th;
	}

	gr_inside( &nn, 1 );	/* leave the black border alone */
#if MaxTH
	if( th > MaxTH )
	{
	    gr_inside( &nn, th - MaxTH );
	    th = MaxTH;
	}
#endif

#if HILITE3d

	bb_screen(S_ONLY, nn.g_x+th-1, nn.g_y+th-1, 
			nn.g_x+2*th-1, nn.g_y+2*th-1, 
			nn.g_w-2*th+1, nn.g_h-2*th+1 );

	if (!do_border) gr_inside( &nn, -1 );	/* leave the black border alone */
	/* draw thicker dark gray upper left edge */
	vsf_color( cc_s[CC_3DSHADE].cc_background );
	bb_fill(MD_REPLACE, FIS_SOLID, IP_SOLID, 
	  	nn.g_x, nn.g_y, nn.g_w, 2*th-1);
	bb_fill(MD_REPLACE, FIS_SOLID, IP_SOLID, 
	  	nn.g_x, nn.g_y, 2*th-1, nn.g_h);

	gsx_attr( FALSE, MD_REPLACE, cc_s[CC_3DSHADE].cc_foreground );
	ptsin[0] = nn.g_x; ptsin[1] = nn.g_y;
	ptsin[2] = nn.g_x+th+1; ptsin[3] = nn.g_y+th+1;
	v_pline( 2, &ptsin[0] );
	gsx_attr( FALSE, MD_REPLACE, BLACK );

#else
	bb_screen(S_ONLY, nn.g_x+th, nn.g_y+th, 
			nn.g_x+2*th, nn.g_y+2*th, 
			nn.g_w-2*th, nn.g_h-2*th );

	/* draw thicker dark gray upper left edge */
	vsf_color( cc_s[CC_3DSHADE].cc_background );
	bb_fill(MD_REPLACE, FIS_SOLID, IP_SOLID, 
	  	nn.g_x, nn.g_y, nn.g_w, 2*th);
	bb_fill(MD_REPLACE, FIS_SOLID, IP_SOLID, 
	  	nn.g_x, nn.g_y, 2*th, nn.g_h);
#endif
}


	VOID
inner_dots( GRECT * pt, WORD clrlite, WORD clrdark )
{
	if (!gl_opts.rounded) return;

	gsx_attr( FALSE, MD_REPLACE, clrlite );
	ptsin[0] = pt->g_x;
	ptsin[1] = pt->g_y;
	ptsin[2] = ptsin[0];
	ptsin[3] = ptsin[1];
	v_pline( 2, &ptsin[0] );
	
	gsx_attr( FALSE, MD_REPLACE, clrdark );
	ptsin[0] = pt->g_x + pt->g_w - 1;
	ptsin[1] = pt->g_y;
	ptsin[2] = ptsin[0];
	ptsin[3] = ptsin[1];
	v_pline( 2, &ptsin[0] );

	ptsin[0] = pt->g_x + pt->g_w - 1;
	ptsin[1] = pt->g_y + pt->g_h - 1;
	ptsin[2] = ptsin[0];
	ptsin[3] = ptsin[1];
	v_pline( 2, &ptsin[0] );

	ptsin[0] = pt->g_x;
	ptsin[1] = pt->g_y + pt->g_h - 1;
	ptsin[2] = ptsin[0];
	ptsin[3] = ptsin[1];
	v_pline( 2, &ptsin[0] );

	gsx_attr( FALSE, MD_REPLACE, BLACK );
}
