/*************************************************************************
**       Copyright 1999, Caldera Thin Clients, Inc.                     ** 
**       This software is licenced under the GNU Public License.        ** 
**       Please see LICENSE.TXT for further information.                ** 
**                                                                      ** 
**                  Historical Copyright                                ** 
**									**
**									**
**  Copyright (c) 1987, Digital Research, Inc. All Rights Reserved.	**
**  The Software Code contained in this listing is proprietary to	**
**  Digital Research, Inc., Monterey, California and is covered by U.S.	**
**  and other copyright protection.  Unauthorized copying, adaptation,	**
**  distribution, use or display is prohibited and may be subject to 	**
**  civil and criminal penalties.  Disclosure to others is prohibited.	**
**  For the terms and conditions of software code use refer to the 	**
**  appropriate Digital Research License Agreement.			**
**									**
*************************************************************************/

#include "portab.h"
#include "gsxdef.h"
#include "defines.h"
#include "gsxextrn.h"

#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define FF_IF	0		/* adv_form() parameter: conditionally FF */

EXTERN	VOID	v_gdp();
EXTERN	VOID	plygn();
EXTERN	VOID	do_line();
EXTERN	VOID	gdp_arc();
EXTERN	VOID	gdp_ell();
EXTERN	VOID	clc_arc();
EXTERN	VOID	Calc_pts();
EXTERN	VOID	st_fl_ptr();
EXTERN	VOID	wline();
EXTERN	VOID	perp_off();
EXTERN	VOID	quad_xform();
EXTERN	VOID	do_circ();
EXTERN	VOID	s_fa_attr();
EXTERN	VOID	r_fa_attr();
EXTERN	VOID	do_arrow();
EXTERN	VOID	arrow();
EXTERN	WORD	df_pport();
EXTERN	WORD	SMUL_DIV();
EXTERN	VOID	set_page();
EXTERN	WORD	allo_buf();
EXTERN	VOID	text_init();
EXTERN	VOID	s_orient();
EXTERN	WORD	range();
EXTERN	VOID	init_g();
EXTERN	VOID	dinit_g();
EXTERN	VOID	init_rect();
EXTERN	VOID	v_updwk();
EXTERN	VOID	clear_image();
EXTERN	WORD	dvt_close();
/* EXTERN	VOID	init_lfu(); */
EXTERN	VOID	clear_sp();
EXTERN	VOID	dvt_clear();
EXTERN	VOID	ini_bufs();
EXTERN	VOID	clearmem();
EXTERN	VOID	adv_form();
EXTERN	VOID	save_defaults();
EXTERN	VOID	HABLINE();
EXTERN	VOID	d_justified();
EXTERN	VOID	v_etext();
EXTERN	VOID	alphaout();
EXTERN	VOID	ABLINE();
EXTERN	VOID	CLC_FLIT();
EXTERN	WORD	Icos();
EXTERN	WORD	Isin();
EXTERN	WORD	arctan();
EXTERN	WORD	vec_len();
#if cdo_rule
	EXTERN	VOID	set_hv();
	EXTERN	VOID	draw_rule();
#endif

#if cdo_text
	EXTERN	VOID 	init_device_fonts();
#else
	EXTERN	VOID	clear_alpha();
#endif

EXTERN	WORD		dseg_seg;
EXTERN	WORD		end_dseg;
EXTERN	WORD		g_devdcnt;
EXTERN	WORD		g_devdout;
EXTERN	WORD		images;
EXTERN	BOOLEAN		Bitmap;
EXTERN	BOOLEAN		first_page;

EXTERN	WORD		m_cross[];
EXTERN	WORD		m_dmnd[];
EXTERN	WORD		m_dot[];
EXTERN	WORD		m_plus[];
EXTERN	WORD		m_square[];
EXTERN	WORD		m_star[];

EXTERN	WORD		io_type;
EXTERN	BYTE		io_port;
EXTERN	BYTE		io_fname[];
MLOCAL	BYTE		type_tbl[7] = {2, 2, 2, 1, 1, 3, 0};
MLOCAL	BYTE		port_tbl[7] = {0, 1, 2, 0, 1, 0, 0};

GLOBAL	WORD	req_y;		/* requested page size in dots (NOT -1) */
				/* (NOT necessarily a multiple of G_SL_HGT) */

/***********************************************************************
 *
 ***********************************************************************/
VOID	v_opnwk()
{
	WORD	df_ioinf;
	BYTE	*ptr, index;

    /* Get default port information. */
	df_ioinf = df_pport();
	io_type = type_tbl[df_ioinf];
	io_port = port_tbl[df_ioinf];

    /* Set requested page size */
	if (CONTRL[3] > 11) {
	    switch ((INTIN[11] >> 8) & 0xff) {	      /* size in high byte */
		case  0:	
		case 20:    
		default:    xres = X_LETTER;
		            req_y = Y_LETTER;
			    break;
		case 5:     xres = X_HALF;
			    req_y = Y_HALF;
			    break;
		case 10:    xres = X_B5;
			    req_y = Y_B5;
			    break;
		case 30:    xres = X_A4;
			    req_y = Y_A4;
			    break;
		case 40:    xres = X_LEGAL;
			    req_y = Y_LEGAL;
			    break;
		case 50:    xres = X_DOUBLE;
			    req_y = Y_DOUBLE;
			    break;
		case 55:    xres = X_BROAD;
			    req_y = Y_BROAD;
			    break;

		case 0xff:		/* variable page size requested */
		/* converted 100ths of an inch to dots */
		    xres = SMUL_DIV(INTIN[101], C_XDPI, 100);

	        /* subtract side margins */
		    xres -= LEFT_MARGIN + RIGHT_MARGIN;

		/* make sure request is no larger than device maximum 	*/
		/* if not, make x request a multiple of 8, -1.		*/
		    if (xres > C_RESXMX)
			    xres = C_RESXMX;
	  	    else xres = ((xres / 8) * 8) - 1;

		/* converted 100ths of an inch to dots */    
		    req_y = SMUL_DIV(INTIN[102], C_YDPI, 100);

		/* make sure request is no larger than device maximum */
		    if (req_y > C_RESYMX)
			    req_y = C_RESYMX;

		/* subtract side margins */
		    req_y -= (TOP_MARGIN + BOT_MARGIN);

		    break;
	    } /* end page size switch case */
	    
	    yres = ((req_y / C_GSLHGT) * C_GSLHGT) - 1;

	/* Set requested port */
	    if ((INTIN[11] & 0xff) != 0xff)
		switch (io_type = (INTIN[11] & 0xff)) {
		    case 0:				/* file */
			    ptr = io_fname;
			    for (index = 12; index < (12 + 66); index++)
			    *ptr++ = (BYTE)(INTIN[index]);
			    break;
		    case 1:				/* serial port */
		    case 2:				/* parallel port */
			    /* one less: BIOS calls are zero-based */
			    io_port = (BYTE)(INTIN[12]) - 1;
			    break;
		    case 3:			/* device-specific (direct) */
			    break;
	        }  /* end switch: set port */
	}
	else {				/* accomodate old v_opnwk() call */
		xres = X_LETTER;	
		yres = ((Y_LETTER / C_GSLHGT) * C_GSLHGT) - 1;
		req_y = Y_LETTER;
        }
	
	set_page();

 	if (!allo_buf()) {		/* buffer allocation error */  
		CONTRL[6] = 0;
		return;
	} 

	CONTRL[2] = 6;
	CONTRL[4] = 45;
	CONTRL[7] = dseg_seg;
	CONTRL[8] = end_dseg;

    /* initialize bitmap fonts */	
	text_init();		

#if cdo_text		
	init_device_fonts();
#endif	

    /* get v_opnwk()'s return values */
	for (index = 0; index < 45; index++)
		INTOUT[index] = dev_tab[index];
	for (index = 0; index < 12; index++)
		PTSOUT[index] = siz_tab[index];

	copies = 1;			/* print at least one copy */
	tray = 0;			/* default paper tray */
	p_orient = 0;			/* default to portrait orientation */
	s_orient();

	line_qi =
		(line_index = range(INTIN[1] - 1, 0, MAX_LINE_STYLE, 0)) + 1;
	line_color = MAP_COL[line_qc = range(INTIN[2], 0, MAX_COLOR - 1, 1)];
	mark_qi =
	    (mark_index = range(INTIN[3] - 1, 0, MAX_MARK_INDEX - 1, 2)) + 1;
	mark_color = MAP_COL[mark_qc = range(INTIN[4], 0, MAX_COLOR - 1, 1)];
	mark_height = DEF_MKHT;
	mark_scale = 1;
	fill_style = range(INTIN[7], 0, MX_FIL_STYLE, 0);
	fill_qi = (fill_index = range(INTIN[8], 0, MX_FIL_INDEX - 1, 0)) + 1;
	fill_color = MAP_COL[fill_qc = range(INTIN[9], 0, MAX_COLOR - 1, 1)];
	xfm_mode = INTIN[10];

	st_fl_ptr();

	WRT_MODE = 0;
	write_qm = 1;
	line_qw = line_width = DEF_LWID;
	line_beg = line_end = 0;
	fill_per = TRUE;
	XW_MIN = YW_MIN = xmn_clip = ymn_clip = YCL_MIN = 0;
	XW_MAX = xmx_clip = xres;
	YW_MAX = ymx_clip = YCL_MAX = yres;
	WINDOW = clip = FALSE;
	images = 0;
	init_g();
	if (abort) {
		dinit_g();
		CONTRL[6] = 0;
		return;
	}  /* end if:  error occurred. */
	init_rect();
	FLIP_Y = 1;
	need_update = FALSE;
	Bitmap = g_devdcnt = FALSE;
	first_page = TRUE;			/* flag for ini_page() */
	
	CONTRL[3] = 1;
	INTIN[0] = 3;
	
}  /* end v_opnwk() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	v_clswk()
{
	WORD	status;
	
	if (need_update)
		v_updwk();
	clear_image();
	
#if !cdo_text	
	clear_alpha();		/* release map table memory */
#endif

	status = dvt_close();
	dinit_g();
	
}  /* end v_clswk() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	v_clrwk()
{
	clear_sp();
	dvt_clear();
	ini_bufs();
	clearmem();
	clear_image();
	adv_form(FF_IF);
	init_rect();
	save_defaults();
	Bitmap = g_devdcnt = FALSE;

}  /* end v_clrwk() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	vq_extnd()
{
	WORD	i;
	WORD	*ints_ptr, *pts_ptr;

	CONTRL[2] = 6;
        CONTRL[4] = 45;
	FLIP_Y = 1;

	if ( !INTIN[0] )
	{
	    ints_ptr = dev_tab;
	    pts_ptr = siz_tab;
	}
	else
	{
	    ints_ptr = inq_tab;
	    pts_ptr = inq_pts;
	}
	
	for (i = 0; i < 12; i++)
	    PTSOUT[i] = *pts_ptr++;
	for (i = 0; i < 45; i++)
	    INTOUT[i] = *ints_ptr++;

}  /* end vq_extnd() */


/***********************************************************************
 *
 ***********************************************************************/
VOID 	line_draw(step)
WORD step;
{
	WORD	i, j;
#if MIN_L_WIDTH > 1
	WORD	dx, dy;
#endif

	step = step;		/* gets rid of compiler "not used" warning */
	j = 0;
	LSTLIN = TRUE;
	for (i = CONTRL[1] - 1; i > 0; i--) {
		if (i == 1)	      
			LSTLIN = FALSE;
		X1 = PTSIN[j++];
		Y1 = PTSIN[j++];	
		X2 = PTSIN[j];
		Y2 = PTSIN[j + 1];
		if (X1 == X2 && Y1 == Y2)
			continue;
#if MIN_L_WIDTH > 1
		if (step != 0) {
			if (X1 == X2) {
				X1 += step;
				X2 += step;
			}  /* End if:  vertical line. */
			else if (Y1 == Y2) {
				Y1 -= step;
				Y2 -= step;
			}  /* End elseif:  horizontal line. */
			else {
				dx = X2 - X1;
				dy = Y2 - Y1;
				if (dx == dy) {
					X1 += step;
					X2 += step;
					do_line();
					Y1 -= step;
					Y2 -= step;
					do_line();
					continue;
				}  /* End if:  -45 degree line. */
				else if (dx == -dy) {
					X1 -= step;
					X2 -= step;
					do_line();
					Y1 -= step;
					Y2 -= step;
					do_line();
					continue;
				}  /* End elseif:  45 degree line. */
				else {
					dx = ABS(dx);
					dy = ABS(dy);
					if (dx > dy) {
						Y1 -= step;
						Y2 -= step;
					}  /* End elseif:  low slope. */
					else {
						X1 += step;
						X2 += step;
					}  /* End else:  high slope. */
				}  /* End else:  not slope 1 or -1. */
			}  /* End else:  neither horizontal nor vertical. */
		}  /* End if:  draw parallel line. */
#endif
		do_line();
	}
	
}  /* end line_draw() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	pline()
{
#if MIN_L_WIDTH > 1
	WORD distance, i, j, save_mask;

	/* Draw the initial line. */
	save_mask = LN_MASK;
	line_draw(0);

	/* Draw parallel lines. */
	for (i = distance = 1; i < MIN_L_WIDTH; ) {
		LN_MASK = save_mask;
		line_draw(distance);
		i++;
		if (i > MIN_L_WIDTH)
			break;
		LN_MASK = save_mask;
		line_draw(-distance);
		distance++;
		i++;
	}  /* End for:  over minimum line width count. */
#else
	line_draw(0);
#endif

}  /* end pline() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	v_pline()
{
	LN_MASK = LINE_STYL[line_index];
	FG_BP_1 = line_color;
	if (line_width == 1) {
	    pline();
	    if ( (line_beg | line_end ) & ARROWED )
		    do_arrow();
	}  /* end if:  normal polyline. */
	else
		wline();
	
}  /* end v_pline() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	v_pmarker()
{
#define MARKSEGMAX 5
	static WORD *markhead[] = 
	{ 
	   m_dot, m_plus, m_star, m_square, m_cross, m_dmnd
	};
	WORD i, j, k, num_lines, num_vert, x_center, y_center;
	WORD sav_index, sav_color, sav_width;
	WORD sav_beg, sav_end, sav_clip;
	WORD *mark_ptr, half_height;

	/* Save the current polyline attributes which will be used. */
	sav_index = line_index;
	sav_color = line_color;
	sav_width = line_width;
	sav_beg = line_beg;
	sav_end = line_end;
	sav_clip = clip;

	/* Set the appropriate polyline attributes. */
	line_index = 0;
	line_color = mark_color;
	line_width = 1;
	line_beg = 0;
	line_end = 0;
	clip = 1;

	/* Copy the PTSIN values which will be overwritten by the */
	/* polyline arrays. 	                                  */
	num_vert = 2*CONTRL[1];
	for (i = 0; i < 2*MARKSEGMAX; i++)
		PTSOUT[i] = PTSIN[i];

	/* Calculate half the marker height for clipping. */
	half_height = (mark_height + 1) >> 1;

	/* Loop over the number of points. */
	for (i = 0; i < num_vert; i++) {
		/* Get the (x, y) position for the marker. */
		if (i < 2 * MARKSEGMAX) {
			x_center = PTSOUT[i++];
			y_center = PTSOUT[i];
		}  /* end if:  grab from PTSOUT. */
		else {
			x_center = PTSIN[i++];
			y_center = PTSIN[i];
		}  /* end else:  grab from PTSIN. */

		/* If no part of the marker is within the slice, ignore it. */
		if (y_center + half_height < ymn_clip ||
			y_center - half_height > ymx_clip)
			continue;

		/* Get pointer to the appropriate marker type definition. */
		mark_ptr = markhead[mark_index];
		num_lines = *mark_ptr++;

		/* Loop over the number of polylines defining the marker. */
		for (j = 0; j < num_lines; j++) {
			/* How many points?  Get them. */
			CONTRL[1] = *mark_ptr++;
			for (k = 0; k < CONTRL[1] << 1; ) {
				PTSIN[k++] = x_center +
					mark_scale*(*mark_ptr++);
				PTSIN[k++] = y_center +
					mark_scale*(*mark_ptr++);
			}  /* end for:  extract points. */

			/* Output the polyline. */
			v_pline();
		}  /* end for:  over polylines defining the marker. */
	}  /* end for:  over marker points. */

	/* Restore the current polyline attributes. */
	line_index = sav_index;
	line_color = sav_color;
	line_width = sav_width;
	line_beg = sav_beg;
	line_end = sav_end;
	clip = sav_clip;
	
}  /* end v_pmarker() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	v_filrec()
{
#if cdo_rule
#if no_tintrule
        WORD		filtmp;

	if ((fill_style == 1) || (fill_style == 2 && fill_index == 7 &&
			fill_color != 0)) {
		if (g_devdout) {
			set_hv(PTSIN[0], PTSIN[1]);
			draw_rule(PTSIN[2] - PTSIN[0], PTSIN[3] - PTSIN[1],
				fill_style, fill_qi );
		}  /* End if:  device dependent output. */
	}  /* End if:  solid. */
	else {
		if (!g_devdout) {
			filtmp = fill_per;
			fill_per = FALSE;
			CONTRL[5] = 1;
			v_gdp();
			fill_per = filtmp;	
		}  /* End if:  not device dependent output. */
	}  /* End else:  patterned. */
#else
	if (g_devdout) {
		set_hv( PTSIN[0], PTSIN[1] );
		draw_rule( (PTSIN[2] - PTSIN[0]), (PTSIN[3] - PTSIN[1]),
			fill_style, fill_qi );
	}
#endif
#else
        WORD		filtmp;
        filtmp = fill_per;
	fill_per = FALSE;
	CONTRL[5] = 1;
	v_gdp();
	fill_per = filtmp;	
#endif

}  /* End v_filrec() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	gdp_rbox()
{
	WORD i, j, rdeltax, rdeltay;

	X1 = PTSIN[0];
	Y1 = PTSIN[1];
	X2 = PTSIN[2];
	Y2 = PTSIN[3];
	rdeltax = (X2 - X1) >> 1;
	rdeltay = (Y1 - Y2) >> 1;
	xrad = xres >> 6;
	if (xrad > rdeltax)
		xrad = rdeltax;
	yrad = SMUL_DIV(xrad, xsize, ysize);
	if (yrad > rdeltay)
		yrad = rdeltay;
	PTSIN[0] = 0;
	PTSIN[1] = yrad;
	PTSIN[2] = SMUL_DIV(Icos(675), xrad, 32767);
	PTSIN[3] = SMUL_DIV(Isin(675), yrad, 32767);
	PTSIN[4] = SMUL_DIV(Icos(450), xrad, 32767);
	PTSIN[5] = SMUL_DIV(Isin(450), yrad, 32767);
	PTSIN[6] = SMUL_DIV(Icos(225), xrad, 32767);
	PTSIN[7] = SMUL_DIV(Isin(225), yrad, 32767);
	PTSIN[8] = xrad;
	PTSIN[9] = 0;
	xc = X2 - xrad;
	yc = Y1 - yrad;
	j = 10;
	for ( i = 9; i >= 0 ; i--) {
		PTSIN[j + 1] = yc + PTSIN[i--];
		PTSIN[j] = xc + PTSIN[i];
		j += 2;
	}
	xc = X1 + xrad; 
	j = 20;
	for ( i = 0; i < 10; i++) {
		PTSIN[j++] = xc - PTSIN[i++];
		PTSIN[j++] = yc + PTSIN[i];
	}
	yc = Y2 + yrad;
	j = 30;
	for ( i = 9; i >= 0; i--) {
		PTSIN[j+1] = yc - PTSIN[i--];
		PTSIN[j] = xc - PTSIN[i];
		j += 2;
	}
	xc = X2 - xrad;
	j = 0;
	for ( i = 0; i < 10; i++) {
		PTSIN[j++] = xc + PTSIN[i++];
		PTSIN[j++] = yc - PTSIN[i];
	}
	PTSIN[40] = PTSIN[0];
	PTSIN[41] = PTSIN[1]; 
	iptscnt = 21;
    	if (gdp_code == 8) {
		LN_MASK = LINE_STYL[line_index];
		FG_BP_1 = line_color;		
		if (line_width == 1)
			pline();
		else
			wline();
	}
	else
		plygn();
	return;
	
}  /* end gdp_rbox() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	v_gdp()
{
	WORD i, ltmp_end, rtmp_end;

	i = CONTRL[5];
	if (i < 10 && (g_devdout))
		return;
	if (i > 0 && i < 12)  {
		i--;
		switch (i) {
			case 0:	/* GDP BAR */
				FG_BP_1 = fill_color;
				LSTLIN = FALSE;
				X1 = PTSIN[0];
				X2 = PTSIN[2];
				fill_maxy = PTSIN[1];
				fill_miny = PTSIN[3];
				if (fill_miny < ymn_clip) {
					if (fill_maxy >= ymn_clip)
						fill_miny = ymn_clip;
					else
						return;
				}
				if (fill_maxy > ymx_clip) {
					if (fill_miny <= ymx_clip)
						fill_maxy = ymx_clip;
					else
						return;
				}
				if (clip) {
					if (X1 < xmn_clip) {
						if (X2 >= xmn_clip)
							X1 = xmn_clip;
						else
							return;
					}
					if (X2 > xmx_clip) {
						if (X1 <= xmx_clip)
							X2 = xmx_clip;
						else
							return;
					}
				}  /* End if:  clipping on. */

				for (Y1 = fill_miny; Y1 <= fill_maxy; Y1++)
					HABLINE();

				if (fill_per) {
					LN_MASK = 0xffff;
					PTSIN[5] = PTSIN[7] = PTSIN[3];
					PTSIN[3] = PTSIN[9] = PTSIN[1];
					PTSIN[4] = PTSIN[2];
					PTSIN[6] = PTSIN[8] = PTSIN[0];
					CONTRL[1] = 5;
					pline();
				}
				break;	

			case 1: /* GDP ARC */
				gdp_arc();
				break;

			case 2: /* GDP PIE */
				gdp_arc();
				break;
	 	
			case 3: /* GDP CIRCLE */
				xc = PTSIN[0];
				yc = PTSIN[1];   
				xrad = PTSIN[4];   
				yrad = SMUL_DIV(xrad, xsize, ysize); 
				del_ang = 3600;
				beg_ang = 0;
				end_ang = 3600;
				n_steps = MAX_ARC_CT;
				clc_arc();
				break;

			case 4: /* GDP ELLIPSE */
				xc = PTSIN[0];
				yc = PTSIN[1];   
				xrad = PTSIN[2];
				yrad = PTSIN[3];
				if (xfm_mode < 2)
					yrad = yres - yrad;	
				del_ang = 3600;
				beg_ang = 0;
				end_ang = 3600;
				n_steps = MAX_ARC_CT;
				clc_arc();
				break;
		
			case 5: /* GDP ELLIPTICAL ARC */
				gdp_ell();
				break;
		
			case 6: /* GDP ELLIPTICAL PIE */
				gdp_ell();
				break;

			case 7: /* GDP Rounded Box */
				ltmp_end = line_beg;
				line_beg = SQUARED;
				rtmp_end = line_end;
				line_end = SQUARED;
				gdp_rbox();
				line_beg = ltmp_end;
				line_end = rtmp_end; 	
				break;

			case 8: /* GDP Rounded Filled Box */
				gdp_rbox();
				break;

			case 9: /* GDP Justified Text */
				d_justified();
				break;

			case 10: /* GDP Escapement Text */
				v_etext();
				break;
		}
	}
	
}  /* end v_gdp() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	alpha_text()
{
	if (CONTRL[3] > 0)
		alphaout();

}  /* end alpha_text() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	q_scan()
{
	CONTRL[2] = 0;
	CONTRL[4] = 5;
	INTOUT[0] = C_GSLHGT;
	INTOUT[1] = G_PAGE;
	INTOUT[2] = C_ASLHGT;
	INTOUT[3] = A_PAGE;
	INTOUT[4] = DIV_FACT;

}  /* end q_scan() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	vql_attr()
{
	INTOUT[0] = line_qi;
	INTOUT[1] = line_qc;
	INTOUT[2] = write_qm;
	INTOUT[3] = line_beg;
	INTOUT[4] = line_end;
	PTSOUT[0] = line_qw;
	PTSOUT[1] = 0;
	CONTRL[2] = 1;
	CONTRL[4] = 5;

}  /* end vql_attr() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	vqm_attr()
{
	INTOUT[0] = mark_qi;
	INTOUT[1] = mark_qc;
	INTOUT[2] = write_qm;
	CONTRL[4] = 3;
	PTSOUT[0] = mark_scale*DEF_MKWD;
	PTSOUT[1] = mark_scale*DEF_MKHT;
	CONTRL[2] = 1;
	FLIP_Y = 1;

}  /* end vqm_attr() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	vqf_attr()
{
	INTOUT[0] = fill_style;
	INTOUT[1] = fill_qc;
	INTOUT[2] = fill_qi;
	INTOUT[3] = write_qm;
	INTOUT[4] = fill_per;
	CONTRL[4] = 5;

}  /* end vqf_attr() */


/***********************************************************************
 *
 ***********************************************************************/
WORD	clip_line()
{
	WORD deltax, deltay, x1y1_clip_flag;
	WORD x2y2_clip_flag, line_clip_flag, x_value;
	WORD *x, *y;

	while ((x1y1_clip_flag = code(X1, Y1)) |
		(x2y2_clip_flag = code(X2, Y2))) {
		if (x1y1_clip_flag & x2y2_clip_flag)
			return(FALSE);
		if (x1y1_clip_flag) {
			line_clip_flag = x1y1_clip_flag;
			x = &X1;
			y = &Y1;
		}
		else {
			line_clip_flag = x2y2_clip_flag;
			x = &X2;
			y = &Y2;
		}
		deltax = X2 - X1;
		deltay = Y2 - Y1;
		if (line_clip_flag & 1) {	/* left ? */
			*y = Y1 + SMUL_DIV(deltay, xmn_clip - X1, deltax);
			*x = xmn_clip;
		}
		else if (line_clip_flag & 2) {	/* right ? */
			*y = Y1 + SMUL_DIV(deltay, xmx_clip - X1, deltax);
			*x = xmx_clip;
		}
		else if (line_clip_flag & 4) {	/* top ? */
			x_value = X1 +
				SMUL_DIV(deltax, ymn_clip - Y1 - 1, deltay);
			if ((deltax > 0 && deltay > 0) ||
				(deltax < 0 && deltay < 0))
				x_value++;
			else if (deltax != 0)
				x_value--;
			*x = x_value;
			*y = ymn_clip;
		}
		else if (line_clip_flag & 8) {	/* bottom ? */
			*x = X1 + SMUL_DIV(deltax, ymx_clip - Y1, deltay);
			*y = ymx_clip;
		}
	}
	return(TRUE);
	
}  /* end clip_line() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	do_line()
{
	if (clip) {
		if (clip_line())
			ABLINE();
	}
	else	
		ABLINE();
	
}  /* end do_line() */


/***********************************************************************
 *
 ***********************************************************************/
WORD	code(x, y)
WORD x, y;
{
	WORD clip_flag;

	clip_flag = 0;
	if (x < xmn_clip)
		clip_flag = 1;
	else if (x > xmx_clip)
		clip_flag = 2;
	if (y < ymn_clip)
		clip_flag +=4;
	else if (y > ymx_clip)
		clip_flag +=8;
	return(clip_flag);
	
}  /* end code() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	plygn()
{
	WORD i, j, k;

	FG_BP_1 = fill_color;
	LSTLIN  = FALSE;
	fill_miny = fill_maxy = PTSIN[1];
	j = 3;
	for (i = CONTRL[1]; i > 0; i--) {
		k = PTSIN [j++];
		j++;
		if (k < fill_miny)
			fill_miny = k;
		else if (k > fill_maxy)
			fill_maxy = k;
	}

	if (fill_miny < ymn_clip) {
		if (fill_maxy >= ymn_clip)
			fill_miny = ymn_clip;
		else
			return;
	}

	if (fill_maxy > ymx_clip) {
		if (fill_miny <= ymx_clip)
			fill_maxy = ymx_clip;
		else
			return;
	}

	j = CONTRL[1] << 1;
	PTSIN[j] = PTSIN[0];
	PTSIN[j + 1] = PTSIN [1];

	for (Y1 = fill_maxy; Y1 >= fill_miny; Y1--) {
		fill_intersect = 0;
		CLC_FLIT();
	}

	if (fill_per) {
		LN_MASK = 0xffff;
		CONTRL[1]++;
		pline();
	}
	
}  /* end plygn() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	gdp_arc()
{
	beg_ang = INTIN[0];
	end_ang = INTIN[1];
	del_ang = end_ang - beg_ang;
	if (del_ang < 0)
		del_ang += 3600; 
	n_steps = SMUL_DIV(del_ang, MAX_ARC_CT, 3600);
	if (n_steps == 0)
		return;
	xc = PTSIN[0];
	yc = PTSIN[1];
	xrad = PTSIN[6];   
	yrad = SMUL_DIV(xrad, xsize, ysize);
	clc_arc();
	return;
	
}  /* end gdp_arc() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	gdp_ell()
{
	beg_ang = INTIN[0];
	end_ang = INTIN[1];
	del_ang = end_ang - beg_ang;
	if (del_ang < 0)
		del_ang += 3600;
	n_steps = SMUL_DIV(del_ang, MAX_ARC_CT, 3600);
	if (n_steps == 0)
		return;
	xc = PTSIN[0];
	yc = PTSIN[1];
	xrad = PTSIN[2];   
	yrad = PTSIN[3];
	if (xfm_mode < 2)
		yrad = yres - yrad;	
	clc_arc();
	return;
	
}  /* end gdp_ell() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	clc_arc()
{
	WORD i, j;

	if (clip && (xc + xrad < xmn_clip || xc - xrad > xmx_clip))
		return;

	while (beg_ang < 0 || end_ang < 0) {
		beg_ang += 3600;
		end_ang += 3600;
	}  /* End while. */

	if (xrad != yrad) {
		if (beg_ang != 0 && beg_ang != 900 && beg_ang != 1800 &&
				beg_ang != 2700 && beg_ang != 3600)
			beg_ang = arctan(xrad, Isin(beg_ang), yrad,
				Icos(beg_ang));
		if (end_ang != 0 && end_ang != 900 && end_ang != 1800 &&
				end_ang != 2700 && end_ang != 3600)
			end_ang = arctan(xrad, Isin(end_ang), yrad,
				Icos(end_ang));
	}  /* End if:  elliptical. */

	start = angle = beg_ang;
	while (end_ang < beg_ang)
		end_ang += 3600;
    	i = j = 0;
    	Calc_pts(j);
	for (i = 1; i < n_steps; i++) {
		angle = SMUL_DIV(del_ang, i , n_steps) + start;
		if (angle > end_ang)
			break;
		j += 2;
		Calc_pts(j);
	}
	j += 2;
    	angle = end_ang;
    	Calc_pts(j);

	if (gdp_code == 3 || gdp_code == 7) {
		j +=2;
		PTSIN[j] = xc;
		PTSIN[j + 1] = yc;
	}
	iptscnt = (j >> 1) + 1;
    	if (gdp_code == 2 || gdp_code == 6)
		v_pline();
    	else
		plygn();
	
}  /* end clc_arc() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	Calc_pts(j) 
WORD j;
{
	WORD		k;
	k = SMUL_DIV(Icos(angle), xrad, 32767) + xc;
    	PTSIN[ j ] = k;
        k = yc - SMUL_DIV(Isin(angle), yrad, 32767);
    	PTSIN[ j + 1 ] = k;
	
}  /* end Calc_pts() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	st_fl_ptr()
{
	patmsk = 0;
	switch ( fill_style ) {
		case 0:
			patptr = &HOLLOW;
			break;

		case 1:
			patptr = &SOLID;
			break;

		case 2:
			if (fill_index >= dev_tab[11])
				fill_index = 0;
			if (fill_index < 8) {
				patmsk = DITHRMSK;
				patptr = &DITHER[fill_index*(patmsk+1)];
			}
			else {
				patmsk = OEMMSKPAT;
				patptr = &OEMPAT[(fill_index - 8)*(patmsk+1)]; 
			}
			break;

		case 3:
			if (fill_index >= dev_tab[12])
				fill_index = 0;
			if (fill_index < 6) {
				patmsk = HAT_0_MSK;
				patptr = &HATCH0[fill_index*(patmsk + 1)];
			}
			else {
				patmsk = HAT_1_MSK;
				patptr = &HATCH1[(fill_index - 6)*(patmsk+1)];
			}
			break;

		case 4:
			patmsk = 0x000f;
			patptr = UD_PATRN;
			break;
	}
	
}  /* end st_fl_ptr() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	wline()
{
	WORD i, j, k;
	WORD numpts, wx1, wy1, wx2, wy2, vx, vy;
	WORD vx_max, vy_max;

	/* Don't attempt wide lining on a degenerate polyline. */
	if ((numpts = CONTRL[1]) < 2)
		return;

	/* It is possible that a signed 16 bit multiply will be done later */
	/* with values which will cause an overflow.  Determine what the   */
	/* maximum x and y values are for the multiplications so that we   */
	/* can avoid the overflow.                                         */
	if (num_qc_lines != 0 && q_circle[0] != 0) {
		vx_max = 32767/q_circle[0];
		vy_max = 32767/num_qc_lines;
	}  /* End if:  at least one quarter-circle line. */
	else
		vx_max = vy_max = 32767;

	/* If the ends are arrowed, output them. */
	if ((line_beg | line_end) & ARROWED)
		do_arrow();

	/* Set up the attribute environment.  Save attribute globals */
	/* which need to be saved.                                   */
	s_fa_attr();

	/* Since the PTSIN array has to be overwritten for the call */
	/* to plygn, save the values stored in the elements which   */
	/* will be overwritten.                                     */
	for (i = 2; i < 10; i++)
		PTSOUT[i] = PTSIN[i];

	/* Initialize the starting point for the loop. */
	j = 0;
	wx1 = PTSIN[j++];
	wy1 = PTSIN[j++];

	/* If the end style for the first point is a circle, output it. */
	if (s_begsty == CIRCLED)
		do_circ(wx1, wy1);

	/* Loop over the number of points passed in. */
	for (i = 1; i < numpts; i++) {
		/* Get the ending point for the line segment and the */
		/* vector from the start to the end of the segment.  */
		if (j < 10) {
			wx2 = PTSOUT[j++];
			wy2 = PTSOUT[j++];   
		}  /* End if:  copy from saved elements. */
		else {
			wx2 = PTSIN[j++];
			wy2 = PTSIN[j++];
		}  /* End else:  copy from PTSIN array. */

		vx = wx2 - wx1;
		vy = wy2 - wy1;

		/* Ignore lines of zero length. */
		if (vx == 0 && vy == 0)
			continue;

		/* Calculate offsets to fatten the line.  If the line       */
		/* segment is horizontal or vertical, do it the simple way. */
		if (vx == 0) {
			vx = q_circle[0];
			vy = 0;
		}  /* End if:  vertical. */

		else if (vy == 0) {
			vx = 0;
			vy = num_qc_lines - 1;
		}  /* End else if:  horizontal. */

		else {
			/* Find the offsets in x and y for a point  */
			/* perpendicular to the line segment at the */
			/* appropriate distance.                    */
			k = SMUL_DIV(-vy, ysize, xsize);
			vy = SMUL_DIV(vx, xsize, ysize);
			vx = k;
			if (ABS(vx) > vx_max || ABS(vy) > vy_max) {
				vx /= 10;
				vy /= 10;
			}  /* End if:  shorten to prevent overflow. */
			perp_off(&vx, &vy);
		}  /* End else:  neither horizontal nor vertical. */

		/* Prepare for the polygon call. */
		CONTRL[1] = 4;
		PTSIN[0] = wx1 + vx;
		PTSIN[1] = wy1 + vy;
		PTSIN[2] = wx1 - vx;
		PTSIN[3] = wy1 - vy;
		PTSIN[4] = wx2 - vx;
		PTSIN[5] = wy2 - vy;
		PTSIN[6] = wx2 + vx;
		PTSIN[7] = wy2 + vy;
		plygn();

		/* If the terminal point of the line segment is an */
		/* internal joint, output a filled circle.         */
		if (i < numpts - 1 || s_endsty == CIRCLED)
			do_circ(wx2, wy2);

		/* The line segment end point becomes the starting point */
		/* for the next line segment.                            */
		wx1 = wx2;
		wy1 = wy2;
	}  /* End for:  over number of points. */

	/* Restore the attribute environment. */
	r_fa_attr();
	
}  /* end wline() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	perp_off(vx, vy)
WORD *vx, *vy;
{
	WORD 	x, y, u, v, quad, mag, min_val;
	WORD	x_val = 0;
	WORD	y_val = 0;
	
    /* Mirror transform the vector so that it is in the first quadrant. */
	if (*vx >= 0)
		quad = (*vy >= 0) ? 1 : 4;
	else
		quad = (*vy >= 0) ? 2 : 3;
	quad_xform(quad, *vx, *vy, &x, &y);

	/* Traverse the circle in a dda-like manner and find the */
	/* coordinate pair (u, v) such that the magnitude of     */
	/* (u*y - v*x) is minimized.  In case of a tie, choose   */
	/* the value which causes (u - v) to be minimized.  If   */
	/* not possible, do something.                           */
	min_val = 32767;
	u = q_circle[0];
	v = 0;
	FOREVER {
		/* Check for new minimum, same minimum, or finished. */
		if ((mag = ABS(u*y - v*x)) < min_val ||
			(mag == min_val && ABS(x_val - y_val) > ABS(u - v))) {
			min_val = mag;
			x_val = u;
			y_val = v;
		}  /* End if:  new minimum. */
		else
			break;

		/* Step to the next pixel. */
		if (v == num_qc_lines - 1) {
			if (u == 1)
				break;
			else
				u--;
		}  /* End if:  doing top row. */

		else {
			if (q_circle[v + 1] >= u - 1) {
				v++;
				u = q_circle[v];
			}  /* End if:  do next row up. */
			else
				u--;
		}  /* End else:  other than top row. */
	}  /* End FOREVER loop. */

	/* Transform the solution according to the quadrant. */
	quad_xform(quad, x_val, y_val, vx, vy);
	
}  /* end perp_off() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	quad_xform(quad, x, y, tx, ty)
WORD quad, x, y, *tx, *ty;
{
	switch (quad) {
		case 1:
		case 4:
			*tx = x;
			break;

		case 2:
		case 3:
			*tx = -x;
			break;
	}  /* End switch. */

	switch (quad) {
		case 1:
		case 2:
			*ty = y;
			break;

		case 3:
		case 4:
			*ty = -y;
			break;
	}  /* End switch. */
	
}  /* end quad_xform() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	do_circ(cx, cy)
WORD cx, cy;
{
	WORD k;

	/* Only perform the act if the circle has radius. */
	if (num_qc_lines > 0) {
		/* Do the horizontal line through the center of the circle. */
		X1 = cx - q_circle[0];
		X2 = cx + q_circle[0];
		Y1 = Y2 = cy;
		if (clip_line())
			ABLINE();

		/* Do the upper and lower semi-circles. */
		for (k = 1; k < num_qc_lines; k++) {
			/* Upper semi-circle. */
			X1 = cx - q_circle[k];
			X2 = cx + q_circle[k];
			Y1 = Y2 = cy - k;
			if (clip_line())
				ABLINE();

			/* Lower semi-circle. */
			X1 = cx - q_circle[k];
			X2 = cx + q_circle[k];
			Y1 = Y2 = cy + k;
			if (clip_line())
				ABLINE();
		}  /* End for. */
	}  /* End if:  circle has positive radius. */
	
}  /* end do_circ() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	s_fa_attr()
{
	/* Set up the fill area attribute environment. */
	LN_MASK = LINE_STYL[0];
	s_fill_color = fill_color;
	fill_color = line_color;
	s_fill_per = fill_per;
	fill_per = TRUE;
	s_patptr = patptr;
	patptr = &SOLID;
	s_patmsk = patmsk;
	patmsk = 0;
	s_begsty = line_beg;
	s_endsty = line_end;
	line_beg = line_end = SQUARED;
	
}  /* end s_fa_attr() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	r_fa_attr()
{
	/* Restore the fill area attribute environment. */
	fill_color = s_fill_color;
	fill_per = s_fill_per;
	patptr = s_patptr;
	patmsk = s_patmsk;
	line_beg = s_begsty;
	line_end = s_endsty;
	
}  /* end r_fa_attr() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	do_arrow()
{
	WORD	x_start, y_start, new_x_start, new_y_start;

	/* Set up the attribute environment. */
	s_fa_attr();

	/* Function "arrow" will alter the end of the line segment.  */
	/* Save the starting point of the polyline in case two calls */
	/* to "arrow" are necessary.                                 */
	new_x_start = x_start = PTSIN[0];
	new_y_start = y_start = PTSIN[1];

	if (s_begsty & ARROWED) {
		arrow(&PTSIN[0], 2);
		new_x_start = PTSIN[0];
		new_y_start = PTSIN[1];
	}  /* End if:  beginning point is arrowed. */

	if (s_endsty & ARROWED) {
		PTSIN[0] = x_start;
		PTSIN[1] = y_start;
		arrow(&PTSIN[(CONTRL[1] << 1) - 2], -2);
		PTSIN[0] = new_x_start;
		PTSIN[1] = new_y_start;
	}  /* End if:  ending point is arrowed. */

	/* Restore the attribute environment. */
	r_fa_attr();
	
}  /* end do_arrow() */


/***********************************************************************
 *
 ***********************************************************************/
VOID	arrow(xy, inc)
WORD *xy, inc;
{
	WORD	i, arrow_len, arrow_wid, line_len;
	WORD	*xybeg, sav_contrl, triangle[6];
	WORD	dx, dy;
	WORD	base_x, base_y, ht_x, ht_y;

    /* Set up the arrow-head length and width as a function */
    /* of the line width.                                   */
	arrow_wid = (arrow_len = (line_width == 1) ? 8 : 3*line_width - 1)/2;

    /* Initialize the beginning pointer. */
	xybeg = xy;

    /* Find the first point which is not so close to the end point that it */
    /* will be obscured by the arrowhead.                                  */
	for (i = 1; i < CONTRL[1]; i++) {
		/* Find the deltas between the next point and the end     */
		/* point.  Transform to a space such that the aspect      */
		/* ratio is uniform and the x axis distance is preserved. */
		xybeg += inc;
		dx = *xy - *xybeg;
		dy = SMUL_DIV(*(xy + 1) - *(xybeg + 1), ysize, xsize);

		/* Get the length of the vector connecting the point   */
		/* with the end point.  If the vector is of sufficient */
		/* length, the search is over.                         */
		if ( (line_len = vec_len(ABS(dx), ABS(dy))) >= arrow_len)
			break;
	}  /* End for:  over i. */

	/* If the longest vector isn't long enough, don't draw an arrow. */
	if (line_len < arrow_len)
		return;

	/* Rotate the arrow-head height and base vectors.  Perform */
	/* calculations in 1000x space.                            */
	ht_x = SMUL_DIV(arrow_len, SMUL_DIV(dx, 1000, line_len), 1000);
	ht_y = SMUL_DIV(arrow_len, SMUL_DIV(dy, 1000, line_len), 1000);
	base_x = SMUL_DIV(arrow_wid, SMUL_DIV(dy, -1000, line_len), 1000);
	base_y = SMUL_DIV(arrow_wid, SMUL_DIV(dx, 1000, line_len), 1000);

	/* Transform the y offsets back to the correct aspect ratio space. */
	ht_y = SMUL_DIV(ht_y, xsize, ysize);
	base_y = SMUL_DIV(base_y, xsize, ysize);

	/* Save the parameter arrays we're about to squash.  Note that four */
	/* vertices must be saved because plygn will stomp on the fourth.   */
	sav_contrl = CONTRL[1];
	for (i = 0; i < 8; i++)
		PTSOUT[i] = PTSIN[i];

	/* Build a polygon to send to plygn.  Build into a local array  */
	/* first since xy will probably be pointing to the PTSIN array. */
	CONTRL[1] = 3;
	triangle[0] = *xy + base_x - ht_x;
	triangle[1] = *(xy + 1) + base_y - ht_y;
	triangle[2] = *xy - base_x - ht_x;
	triangle[3] = *(xy + 1) - base_y - ht_y;
	triangle[4] = *xy;
	triangle[5] = *(xy + 1);
	for (i = 0; i < 6; i++)
		PTSIN[i] = triangle[i];
	plygn();

	/* Restore the squashed parameter arrays. */
	CONTRL[1] = sav_contrl;
	for (i = 0; i < 8; i++)
		PTSIN[i] = PTSOUT[i];

	/* Adjust the end point and all points skipped. */
	*xy -= ht_x;
	*(xy + 1) -= ht_y;
	while ( (xybeg -= inc) != xy) {
		*xybeg = *xy;
		*(xybeg + 1) = *(xy + 1);
	}  /* End while. */
	
}  /* end arrow() */
