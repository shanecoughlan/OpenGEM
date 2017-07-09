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

#define FF_ALWAYS	1		/* adv_form() param */

#if Y_ASPECT > X_ASPECT
GLOBAL	WORD	q_circle[MAX_L_WIDTH];
#else
GLOBAL	WORD	q_circle[(MAX_L_WIDTH*X_ASPECT/Y_ASPECT)/2 + 1];
#endif

GLOBAL	WORD		TOKEN = 0;
GLOBAL	WORD		WINDOW = 0;
GLOBAL	WORD		XW_MAX = 0;
GLOBAL	WORD		XW_MIN = 0;
GLOBAL	WORD		YCL_MAX = 0;
GLOBAL	WORD		YCL_MIN = 0;
GLOBAL	WORD		YW_MAX = 0;
GLOBAL	WORD		YW_MIN = 0;
GLOBAL	WORD		angle = 0;
GLOBAL	WORD		beg_ang = 0;
GLOBAL	WORD		copies = 0;
GLOBAL	WORD		d_clip = 0;
GLOBAL	WORD		d_fa_color = 0;
GLOBAL	WORD		d_fa_index = 0;
GLOBAL	WORD		d_fa_style = 0;
GLOBAL	WORD		d_fill_per = 0;
GLOBAL	WORD		d_ln_beg = 0;
GLOBAL	WORD		d_ln_color = 0;
GLOBAL	WORD		d_ln_end = 0;
GLOBAL	WORD		d_ln_index = 0;
GLOBAL	WORD		d_ln_width = 0;
GLOBAL	WORD		d_mk_color = 0;
GLOBAL	WORD		d_mk_height = 0;
GLOBAL	WORD		d_mk_index = 0;
GLOBAL	WORD		d_mk_scale = 0;
GLOBAL	WORD		d_patmsk = 0;
GLOBAL	WORD		d_qc_fill = 0;
GLOBAL	WORD		d_qc_line = 0;
GLOBAL	WORD		d_qc_mark = 0;
GLOBAL	WORD		d_qi_fill = 0;
GLOBAL	WORD		d_qi_line = 0;
GLOBAL	WORD		d_qi_mark = 0;
GLOBAL	WORD		d_wrt_mode = 0;
GLOBAL	WORD		d_xmn_clip = 0;
GLOBAL	WORD		d_xmx_clip = 0;
GLOBAL	WORD		d_ycl_max = 0;
GLOBAL	WORD		d_ycl_min = 0;
GLOBAL	WORD		del_ang = 0;
GLOBAL	WORD		deltay1 = 0;
GLOBAL	WORD		deltay2 = 0;
GLOBAL	WORD		deltay = 0;
GLOBAL	WORD		end_ang = 0;
GLOBAL	WORD		fcl_xmax = 0;
GLOBAL	WORD		fcl_xmin = 0;
GLOBAL	WORD		fcl_ymax = 0;
GLOBAL	WORD		fcl_ymin = 0;
GLOBAL	WORD		fill_color = 0;
GLOBAL	WORD		fill_index = 0;
GLOBAL	WORD		fill_intersect = 0;
GLOBAL	WORD		fill_maxy = 0;
GLOBAL	WORD		fill_miny = 0;
GLOBAL	WORD		fill_per = 0;
GLOBAL	WORD		fill_qc = 0;
GLOBAL	WORD		fill_qi = 0;
GLOBAL	WORD		fill_style = 0;
GLOBAL	WORD		line_beg = 0;
GLOBAL	WORD		line_color = 0;
GLOBAL	WORD		line_end = 0;
GLOBAL	WORD		line_index = 0;
GLOBAL	WORD		line_qc = 0;
GLOBAL	WORD		line_qi = 0;
GLOBAL	WORD		line_qw = 0;
GLOBAL	WORD		line_width = 0;
GLOBAL	WORD		mark_color = 0;
GLOBAL	WORD		mark_height = 0;
GLOBAL	WORD		mark_index = 0;
GLOBAL	WORD		mark_qc = 0;
GLOBAL	WORD		mark_qi = 0;
GLOBAL	WORD		mark_scale = 0;
GLOBAL	WORD		n_steps = 0;
GLOBAL	WORD		need_update = 0;
GLOBAL	WORD		new_clip = 0;
GLOBAL	WORD		num_qc_lines = 0;
GLOBAL	WORD		odeltay = 0;
GLOBAL	WORD		p_orient = 0;
GLOBAL	WORD		patmsk = 0;
GLOBAL	WORD		s_begsty = 0;
GLOBAL	WORD		s_endsty = 0;
GLOBAL	WORD		s_fill_color = 0;
GLOBAL	WORD		s_fill_per = 0;
GLOBAL	WORD		s_patmsk = 0;
GLOBAL	WORD		start = 0;
GLOBAL	WORD		tray = 0;
GLOBAL	WORD		write_qm = 0;
GLOBAL	WORD		xc = 0;
GLOBAL	WORD		xmn_rect = 0;
GLOBAL	WORD		xmx_rect = 0;
GLOBAL	WORD		xrad = 0;
GLOBAL	WORD		y = 0;
GLOBAL	WORD		yc = 0;
GLOBAL	WORD		ymn_rect = 0;
GLOBAL	WORD		ymx_rect = 0;
GLOBAL	WORD		yrad = 0;

GLOBAL	WORD		*d_patptr = 0;
GLOBAL	WORD		*patptr = 0;
GLOBAL	WORD		*s_patptr = 0;

EXTERN	BOOLEAN		Abort;
EXTERN	BOOLEAN		Bitmap;
EXTERN	BOOLEAN		dv_loaded;
EXTERN	WORD		clip;
EXTERN	WORD		FLIP_Y;
EXTERN	WORD		VMU;
EXTERN	WORD		WRT_MODE;
EXTERN	WORD		xmn_clip;
EXTERN	WORD		xmx_clip;
EXTERN	WORD		Y1;
EXTERN	WORD		Y2;
EXTERN	WORD		ymn_clip;
EXTERN	WORD		ymx_clip;
EXTERN	WORD		YS_MAX;
EXTERN	WORD		YS_MIN;
EXTERN	WORD		g_devdcnt;
EXTERN	WORD		g_devdout;
EXTERN	WORD		loaded;

EXTERN	BYTE		CURALPHA;
EXTERN	BYTE		REQALPHA;

EXTERN	WORD		CONTRL[];
EXTERN	WORD		dev_tab[];
EXTERN	WORD		INTIN[];
EXTERN	WORD		INTOUT[];
EXTERN	WORD		LINE_STYL[];
EXTERN	WORD		MAP_COL[];
EXTERN	WORD		PTSIN[];
EXTERN	WORD		PTSOUT[];
EXTERN	WORD		siz_tab[];
EXTERN	WORD		UD_PATRN[];

EXTERN	WORD	range();
EXTERN	VOID	v_pmarker();
EXTERN	VOID	v_pline();	
EXTERN	VOID	Put_Poly();	
EXTERN	VOID	s_orient();
EXTERN	VOID	PT_OP_BY();
EXTERN	VOID	dqt_extent();
EXTERN	VOID	bound_rect();
EXTERN	VOID	PT_PTSIN();
EXTERN	VOID	PT_INTIN();
EXTERN	VOID	just_chk();
EXTERN	VOID	GT_PTSIN();
EXTERN	VOID	GT_INTIN();
EXTERN	VOID	v_gtext();
EXTERN	VOID	plygn();
EXTERN	VOID	v_filrec();
EXTERN	WORD	MUL_DIV();
EXTERN	VOID	PT_OP_WD();
EXTERN	VOID	GT_WORD();
EXTERN	VOID	v_gdp();
EXTERN	VOID	st_fl_ptr();
EXTERN	VOID	GT_BYTE();
EXTERN	WORD	dvt_fres();
EXTERN	VOID	open_sp();
EXTERN	VOID	clearmem();
EXTERN	VOID	init_p();
EXTERN	VOID	init_dev();
EXTERN	VOID	save_text_defaults();
EXTERN	VOID	reset_text_defaults();
EXTERN	VOID	dini_dev();
EXTERN	VOID	dinit_p();
EXTERN	VOID	close_sp();
EXTERN	VOID	adv_form();
EXTERN	VOID	out_win();
EXTERN	VOID	clear_sp();
EXTERN	VOID	dvt_clear();
EXTERN	VOID	ini_bufs();
EXTERN	VOID	c_image();		/* image.c */
EXTERN	WORD	r_image();		/* image.c */
EXTERN	VOID	clear_image();		/* image.c */
EXTERN	VOID	init_lfu();
EXTERN	VOID	q_scan();
EXTERN	VOID	alpha_text();
EXTERN	VOID	Get_Poly();
EXTERN	VOID	save_defaults();

#if cdo_text
    EXTERN	VOID	dvt_dflag();
    EXTERN	BOOLEAN	font_type();
    #define	DEVICE		1		/* font_type() return */
#endif

#if autocopy
    EXTERN	VOID	set_copies();
#endif



/**************************************************/
VOID	v_nop()
{
}  /* end v_nop() */


/**************************************************/
VOID	init_rect()
{
	xmx_rect = ymx_rect = 0;
	xmn_rect = xres;
	ymn_rect = yres;
	new_clip = TRUE;
	
}  /* end init_rect() */


/**************************************************/
VOID	chk_clip()
{
	if (xmn_clip < xmn_rect)
		xmn_rect = xmn_clip;
	if (xmx_clip > xmx_rect)
		xmx_rect = xmx_clip;
	if (YCL_MIN < ymn_rect)
		ymn_rect = YCL_MIN;
	if (YCL_MAX > ymx_rect)
		ymx_rect = YCL_MAX;
	new_clip = FALSE;
	
}  /* end chk_clip() */


/**************************************************/
VOID	c_escape()
{
	switch (CONTRL[5]) {
		case 1:		/* inquire addressable character cells */
			CONTRL[4] = 2;
			INTOUT[0] = INTOUT[1] = -1;
			break;

		case 20:	/* advance form */
			adv_form(FF_ALWAYS);
			break;

		case 21:	/* output window */
			need_update = TRUE;
			out_win();
			break;

		case 22:	/* clear display list */
			clear_sp();
			dvt_clear();
			ini_bufs();
			clearmem();
			clear_image();
			save_defaults();
			init_lfu();
			init_rect();
			Bitmap = g_devdcnt = FALSE;
			break;

		case 23:	/* output bit image */
		case 101:	/* output rotated bit image */
			if (new_clip)
				chk_clip();
			need_update= TRUE;
			c_image();
			Bitmap = TRUE;
			break;

		case 24:	/* inquire printer scan heights */
			q_scan();
			break;

		case 25:	/* output alpha text */
			need_update = TRUE;
			alpha_text();
			Bitmap = TRUE;
			break;

		case 27:	/* set page orientation */
			p_orient = INTIN[0];
			s_orient();
			break;

		case 28:	/* set # copies */
#if autocopy
			set_copies(INTIN[0]);
#else
			if (INTIN[0] > 0)
				copies = INTIN[0];
#endif			
			break;
			
		case 29:	/* set paper tray */
			tray = INTIN[0];
			break;
	}  /* end switch */
	
}  /* end c_escape() */


/**************************************************/
VOID	r_escape()
{
	GT_BYTE();

	if ((TOKEN == 23 || TOKEN == 101) && (!g_devdout))
		r_image();

}  /* end r_escape() */


/**************************************************/
VOID	c_pline()
{
	if (new_clip)
		chk_clip();
	Put_Poly(0);
	Bitmap = need_update = TRUE;
	
}  /* end c_pline() */


/**************************************************/
VOID	r_pline()
{	
	WORD	offset;

	Get_Poly();
	if (!g_devdout) {
		offset = ( (line_beg | line_end) & ARROWED) ?
			4*num_qc_lines : num_qc_lines;
		if (Y1 - offset <= YS_MAX && Y2 + offset >= YS_MIN)
			v_pline();
	}
	
}  /* end r_pline() */


/**************************************************/
VOID	c_pmarker()
{
	if (new_clip)
		chk_clip();
	Put_Poly((mark_height + 1) << 1);
	Bitmap = need_update = TRUE;
	
}  /* end c_pmarker() */


/**************************************************/
VOID	r_pmarker()
{
	Get_Poly();
	if (!g_devdout && Y1 <= YS_MAX && Y2 >= YS_MIN)
		v_pmarker();
	
}  /* end r_pmarker() */


/**************************************************/
VOID	c_gtext()
{
	WORD		i, x_save, y_save;

#if cdo_text
	if (font_type() == DEVICE)
		dvt_dflag();
	else if (new_clip)
		chk_clip();
#endif

	if (!loaded && !dv_loaded && (CONTRL[3] <= 0))
		return;

#if cdo_text	
	if (font_type() != DEVICE) {
#endif		
		/* If the writing mode is replace, output appropriate bar. */
		if (WRT_MODE == 0) {
			/* Output commands to set the fill area color to */
			/* background and the fill area style to solid.  */
			/* Turn the perimeter visibility off.            */
			if (fill_color) {
				TOKEN = 0;
				CONTRL[0] = 25;
				PT_OP_BY();
			}  /* End if:  must send fill color. */
			if (fill_style != 1) {
				TOKEN = 1;
				CONTRL[0] = 23;
				PT_OP_BY();
			}  /* End if:  must send fill area style. */
			if (fill_per) {
				TOKEN = FALSE;
				CONTRL[0] = 104;
				PT_OP_BY();
			}  /* End if:  turn off fill perimeter visibility. */

			/* Find the text extents. */
			x_save = PTSIN[0];
			y_save = PTSIN[1];
			dqt_extent();
			CONTRL[2] = 0;

			/* Get the bounding rectangle and output a bar. */
			bound_rect();
			for (i = 0; i < 4; i++)
				PTSIN[i] = PTSOUT[i];
			CONTRL[0] = 11;
			TOKEN = 1;
			PT_OP_BY();
			CONTRL[1] = 2;
			PT_PTSIN();

			/* Restore. */
			if (fill_color) {
				TOKEN = fill_color;
				CONTRL[0] = 25;
				PT_OP_BY();
			}  /* End if:  must restore fill color. */
			if (fill_style != 1) {
				TOKEN = fill_style;
				CONTRL[0] = 23;
				PT_OP_BY();
			}  /* End if:  must restore fill area style. */
			if (fill_per) {
				TOKEN = fill_per;
				CONTRL[0] = 104;
				PT_OP_BY();
			}  /* End if:  restore fill perimeter visibility. */

			/* Restore the parameter arrays. */
			PTSIN[0] = x_save;
			PTSIN[1] = y_save;
			CONTRL[0] = 8;
		}  /* End if:  replace mode. */
#if cdo_text		
	}  /* end if: font_type() != DEVICE */
#endif
	CONTRL[1] = 1;
	TOKEN = CONTRL[3];	
	PT_OP_BY();
	PT_PTSIN();
	PT_INTIN();
	need_update = TRUE;
	just_chk();
	
}  /* end c_gtext() */


/**************************************************/
VOID	r_gtext()
{
	GT_BYTE();
	CONTRL [3] = TOKEN;
	CONTRL[1] = 1;
	GT_PTSIN();
	GT_INTIN();
	v_gtext();
	
}  /* end r_gtext() */


/**************************************************/
VOID	c_fillarea()
{
	if (new_clip)
		chk_clip();
	Put_Poly(0);
	Bitmap = need_update = TRUE;
	
}  /* end c_fillarea() */


/**************************************************/
VOID	r_fillarea()
{
	Get_Poly();
	if (!g_devdout && Y1 <= YS_MAX && Y2 >= YS_MIN)
		plygn();
	
}  /* end r_fillarea() */


/**************************************************/
VOID	clip_x(xptr)
WORD *xptr;
{
	WORD	x;

	x = *xptr;
	if (x < xmn_clip)
		*xptr = xmn_clip;
	else if (x > xmx_clip)
		*xptr = xmx_clip;
	
}  /* end clip_x() */


/**************************************************/
VOID 	clip_y(yptr)
WORD *yptr;
{
	WORD		y;

	y = *yptr;
	if (y < YCL_MIN)
		*yptr = YCL_MIN;
	else if (y > YCL_MAX)
		*yptr = YCL_MAX;
	
}  /* end clip_y() */


/**************************************************/
VOID	arb_corner(xy, type)
WORD xy[], type;
{
	WORD	temp;

	/* Fix the x coordinate values, if necessary. */
	if (xy[0] > xy[2]) {
		temp = xy[0];
		xy[0] = xy[2];
		xy[2] = temp;
	}  /* End if:  "x" values need to be swapped. */

	/* Fix y values based on whether traditional (ll, ur) or raster-op */
	/* (ul, lr) format is desired.                                     */
	if ((type == LLUR && xy[1] < xy[3]) ||
		(type == ULLR && xy[1] > xy[3])) {
		temp = xy[1];
		xy[1] = xy[3];
		xy[3] = temp;
	}  /* End if:  "y" values need to be swapped. */
	
}  /* end arb_corner() */


/**************************************************/
VOID	c_filrec()
{
	WORD		isdev;
#if MIN_L_WIDTH > 1
	WORD		dx, dy;
#endif

	isdev = 0;

	if ((PTSIN[0] < xmn_clip && PTSIN[2] < xmn_clip) ||
		(PTSIN[0] > xmx_clip && PTSIN[2] > xmx_clip) ||
		(PTSIN[1] < YCL_MIN && PTSIN[3] < YCL_MIN) ||
		(PTSIN[1] > YCL_MAX && PTSIN[3] > YCL_MAX))
		return;

	clip_x(&PTSIN[0]);
	clip_y(&PTSIN[1]);
	clip_x(&PTSIN[2]);
	clip_y(&PTSIN[3]);

#if cdo_rule
#if no_tintrule
	if ((fill_style == 1) || (fill_style == 2 && fill_index == 7 &&
		fill_color != 0)) {
		g_devdcnt = TRUE;
		arb_corner(PTSIN, ULLR);
	        isdev = 1;
	}  /* End if:  solid. */
	else {
		if (new_clip)
			chk_clip();
		arb_corner(PTSIN, LLUR);
		Bitmap = TRUE;
	}  /* End else:  patterned. */
#else
	g_devdcnt = TRUE;
	arb_corner(PTSIN, ULLR);
	isdev = 1;
#endif
#else
	if (new_clip)
		chk_clip();
	arb_corner(PTSIN, LLUR);
	Bitmap = TRUE;
#endif
#if MIN_L_WIDTH > 1
	dx = PTSIN[2] - PTSIN[0];
	if (dx < 0)
		dx = -dx;
	dy = PTSIN[3] - PTSIN[1];
	if (dy < 0)
		dy = -dy;
	if (dx < MIN_L_WIDTH)
		PTSIN[2] = PTSIN[0] + MIN_L_WIDTH;
	if (dy < MIN_L_WIDTH) {
		if (PTSIN[3] < PTSIN[1])
			PTSIN[3] = PTSIN[1] - MIN_L_WIDTH;
		else
			PTSIN[1] = PTSIN[3] - MIN_L_WIDTH;
		PTSIN[2] += MIN_L_WIDTH;
	}  /* End if:  y must be stretched. */
	if (isdev)
		arb_corner(PTSIN, ULLR);
	else
		arb_corner(PTSIN, LLUR);
#endif
	Put_Poly(0);
	need_update = TRUE;
	
}  /* end c_filrec */


/**************************************************/
VOID	r_filrec()
{
	Get_Poly();
	v_filrec();
	
}  /* end r_filrec() */


/**************************************************/
VOID	c_gdp()
{
	WORD		i;

	TOKEN = CONTRL[5];
	if (TOKEN > 0 && TOKEN < 12)  {
		PT_OP_BY();
		if (TOKEN < 10) {
			Bitmap = TRUE;
			if (new_clip)
				chk_clip();
		}
		switch (TOKEN) {
			case 1: /* GDP BAR */
			case 8: /* GDP ROUNDED RECTANGLE */
			case 9: /* GDP FILLED ROUNDED RECTANGLE */
				CONTRL[1] = 2;
				arb_corner(PTSIN, LLUR);
				PT_PTSIN();
				break;

			case 2: /* GDP ARC */
			case 3: /* GDP PIE */
				CONTRL[1] = 5;
				yrad = MUL_DIV(PTSIN[6], xsize, ysize);
				PTSIN[8] = PTSIN[1] - yrad;
				PTSIN[9] = PTSIN[1] + yrad;

				PT_PTSIN();
				CONTRL[3] = 2;
				PT_INTIN();
				break;

			case 4: /* GDP CIRCLE */
				CONTRL[1] = 4;
				yrad = MUL_DIV(PTSIN[4], xsize, ysize);
				PTSIN[6] = PTSIN[1] - yrad;
				PTSIN[7] = PTSIN[1] + yrad;
				PT_PTSIN();
				break;

			case 5: /* GDP ELLIPSE */
				CONTRL[1] = 3;
				PTSIN[4] = PTSIN[1] - PTSIN[3];
				PTSIN[5] = PTSIN[1] + PTSIN[3];
				PT_PTSIN();
				break;

			case 6: /* GDP ELLIPTICAL ARC */
			case 7: /* GDP ELLIPTICAL PIE */
				CONTRL[1] = 3;
				PTSIN[4] = PTSIN[1] - PTSIN[3];
				PTSIN[5] = PTSIN[1] + PTSIN[3];
				PT_PTSIN();
				CONTRL[3] = 2;
				PT_INTIN();
				break;

			case 10: /* GDP JUSTIFIED TEXT */
				CONTRL[1] = 2;
				PT_PTSIN();
				TOKEN = CONTRL[3];
				PT_OP_WD();
				PT_INTIN();				
				just_chk();
#if cdo_text
				if (font_type() == DEVICE)
					dvt_dflag();
#endif
				break;

			case 11: /* GDP ESCAPEMENT TEXT */
				TOKEN = CONTRL[3];
				PT_OP_WD();
				PT_INTIN();
				CONTRL[1] = CONTRL[3] + 1;
				for (i = 2; i < CONTRL[1]; i++)
					PTSIN[i] = PTSIN[(i - 1) << 1];
				CONTRL[1] = (CONTRL[1] + 1) >> 1;
				PT_PTSIN();
				just_chk();
#if cdo_text
				if (font_type() == DEVICE)
					dvt_dflag();
#endif
				break;
		}
	}
	need_update = TRUE;
	
}  /* end c_gdp() */


/**************************************************/
VOID	r_gdp()
{
	WORD	offset;

	GT_BYTE();
	CONTRL[5] = TOKEN;
	switch (TOKEN) {
		case 2: /* GDP ARC */
		case 6: /* GDP ELLIPTICAL ARC */
		case 8: /* GDP ROUNDED RECTANGLE */
			offset = ( (line_beg | line_end) & ARROWED) ?
				4*num_qc_lines : num_qc_lines;
			break;

		case 1: /* GDP BAR */
		case 3: /* GDP PIE */
		case 4: /* GDP CIRCLE */
		case 5: /* GDP ELLIPSE */
		case 7: /* GDP ELLIPTICAL PIE */
		case 9: /* GDP FILLED ROUNDED RECTANGLE */
		case 10: /* GDP JUSTIFIED TEXT */
		case 11: /* GDP ESCAPEMENT TEXT */
			offset = 0;
			break;
	}  /* End switch. */

	switch (TOKEN) {
		case 1: /* GDP BAR */
		case 8: /* GDP ROUNDED RECTANGLE */
		case 9: /* GDP FILLED ROUNDED RECTANGLE */
			CONTRL[1] = 2;
			GT_PTSIN();
			if (PTSIN[1] + offset < YS_MIN ||
				PTSIN[3] - offset > YS_MAX)
				return;
			break;

		case 2: /* GDP ARC */
		case 3: /* GDP PIE */
			CONTRL[1] = 5;
			GT_PTSIN();
			CONTRL[3] = 2;
			GT_INTIN();
			if (PTSIN[9] + offset < YS_MIN ||
				PTSIN[8] - offset > YS_MAX)
				return;
			break;

		case 4: /* GDP CIRCLE */
			CONTRL[1] = 4;
			GT_PTSIN();
			if (PTSIN[7] < YS_MIN || PTSIN[6] > YS_MAX)
				return;
			break;

		case 5: /* GDP ELLIPSE */
			CONTRL[1] = 3;
			GT_PTSIN();
			if (PTSIN[5] < YS_MIN || PTSIN[4] > YS_MAX)
				return;
			break;

		case 6: /* GDP ELLIPTICAL ARC */
		case 7: /* GDP ELLIPTICAL PIE */
			CONTRL[1] = 3;
			GT_PTSIN();
			CONTRL[3] = 2;
			GT_INTIN();
			if (PTSIN[5] + offset < YS_MIN ||
				PTSIN[4] - offset > YS_MAX)
				return;
			break;

		case 10: /* GDP JUSTIFIED TEXT */
			CONTRL[1] = 2;
			GT_PTSIN();
			GT_BYTE();
			GT_WORD();       
			CONTRL[3] = TOKEN;
			GT_INTIN();
			break;

		case 11: /* GDP ESCAPEMENT TEXT */
			GT_BYTE();
			GT_WORD();       
			CONTRL[3] = TOKEN;
			GT_INTIN();
			CONTRL[1] = (CONTRL[3] + 2) >> 1;
			GT_PTSIN();
			break;
	}  /* End switch. */
	v_gdp();
	
}  /* end r_gdp() */


/**************************************************/
VOID	csl_type()
{
	CONTRL[4]=1;
	TOKEN = range(INTIN[0] - 1, 0, MAX_LINE_STYLE, 0);
	if (TOKEN != line_index) {
		line_qi = (line_index = TOKEN) + 1;
		PT_OP_BY();
	}
	INTOUT[0] = line_qi;
	
}   /* end csl_type() */


/**************************************************/
VOID	rsl_type()
{

	GT_BYTE();
	line_qi = (line_index = TOKEN) + 1;
	
}  /* end rsl_type() */


/**************************************************/
VOID	csl_width()
{
	TOKEN = PTSIN[0];
	if (TOKEN > MAX_L_WIDTH)
		TOKEN = MAX_L_WIDTH;
	else if (TOKEN <= MIN_L_WIDTH)
		TOKEN = 1;

        /* Make the line width an odd number (one less, if even). */
        TOKEN = (((TOKEN - 1) >> 1) << 1) + 1;
	if (TOKEN != line_width) {
		line_width = TOKEN;
		PT_OP_WD();
	}

	CONTRL[2] = 1;
	PTSOUT[0] = line_qw = line_width;
	PTSOUT[1] = 0;
	
}  /* end csl_width() */


/**************************************************/ 
VOID	rsl_width()
{
	WORD	i, x, y, d;
#if Y_ASPECT > X_ASPECT
	WORD	j, low, high;
#endif	

	/* Return if the line width is being set to one. */
	GT_WORD();
	if ( (line_qw = line_width = TOKEN) == 1)
		return;

	/* Initialize the circle DDA.  "y" is set to the radius. */
	x = 0;
	y = (line_width + 1) >> 1;
	d = 3 - 2*y;

	/* Do an octant, starting at north.  The values for the next octant */
	/* (clockwise) will be filled by transposing x and y.               */
	while (x < y) {
		q_circle[y] = x;
		q_circle[x] = y;
		if (d < 0)
			d = d + 4*x + 6;
		else {
			d = d + 4*(x - y) + 10;
			y--;
		}  /* End else. */
		x++;
	}  /* End while. */

	if (x == y)
		q_circle[x] = x;

	/* Calculate the number of vertical pixels required. */
	num_qc_lines = ((line_width*xsize/ysize) >> 1) + 1;

	/* Fake a pixel averaging when converting to non-1:1 aspect ratio. */
#if Y_ASPECT > X_ASPECT
	low = 0;
	for (i = 0; i < num_qc_lines; i++) {
		high = (((i << 1) + 1)*ysize/xsize) >> 1;
		d = 0;
		for (j = low; j <= high; j++)
			d += q_circle[j];
		q_circle[i] = d/(high - low + 1);
		low = high + 1;
	}  /* End for loop. */
#else
	for (i = num_qc_lines - 1; i >= 0; i--)
		q_circle[i] = q_circle[((i << 1)*ysize/xsize + 1) >> 1];
#endif

}  /* end rsl_width() */


/**************************************************/
VOID	csl_ends()
{
	CONTRL[4] = 2;
	INTOUT[0] = line_beg = INTIN[0] = range(INTIN[0], 0, 2, 0);
	INTOUT[1] = line_end = INTIN[1] = range(INTIN[1], 0, 2, 0);
	PT_OP_BY();
	PT_INTIN();
	
}  /* end csl_ends() */


/**************************************************/
VOID	rsl_ends()
{
	CONTRL[3] = 2;
	GT_BYTE();
	GT_INTIN();
	line_beg = INTIN[0];
	line_end = INTIN[1];
	
}  /* end rsl_ends() */


/**************************************************/
VOID	csl_color()
{
	CONTRL[4]=1;
	INTOUT[0] = line_qc = TOKEN = range(INTIN[0], 0, MAX_COLOR - 1, 1);
	if (MAP_COL[line_qc] != line_color) {
		TOKEN = line_color = MAP_COL[line_qc];
		PT_OP_BY();
	}
	
}  /* end csl_color() */


/**************************************************/
VOID	rsl_color()
{
	GT_BYTE();
	line_color = TOKEN;
	
}  /* end rsl_color() */


/**************************************************/
VOID	csm_type()
{
	if ( (TOKEN = range(INTIN[0] - 1, 0, MAX_MARK_INDEX - 1 , 2)) !=
		mark_index) {
		mark_qc = (mark_index = TOKEN) + 1;
		PT_OP_BY();
	}
	CONTRL[4] = 1;
	INTOUT[0] = mark_qc;
	
}  /* end csm_type() */


/**************************************************/
VOID	rsm_type()
{
	GT_BYTE();
	mark_qc = (mark_index = TOKEN) + 1;
	
}  /* end rsm_type() */


/**************************************************/
VOID	csm_height()
{
	/* Limit the requested marker height to a reasonable value. */
	if ( (TOKEN = PTSIN[1]) < DEF_MKHT)
		TOKEN = DEF_MKHT;
	else if (TOKEN > MAX_MKHT)
		TOKEN = MAX_MKHT;

	/* If this marker height is different than the last one, put it */
	/* into the display list and update the globals.                */
	if (TOKEN != mark_height) {
		mark_scale = ((mark_height = TOKEN) + DEF_MKHT/2)/DEF_MKHT;
		PT_OP_WD();
	}

	/* Set the marker height internals and the return parameters. */
	CONTRL[2] = 1;
	PTSOUT[0] = mark_scale*DEF_MKWD;
	PTSOUT[1] = mark_scale*DEF_MKHT;
	FLIP_Y = 1;
	
}  /* end csm_height() */


/**************************************************/
VOID	rsm_height()
{
	GT_WORD();
	mark_scale = ( (mark_height = TOKEN) + DEF_MKHT/2)/DEF_MKHT;
	
}  /* end rsm_height() */


/**************************************************/
VOID	csm_color()
{
	CONTRL[4] = 1;
	INTOUT[0] = mark_qc = TOKEN = range(INTIN[0], 0, MAX_COLOR - 1, 1);
	if (MAP_COL[mark_qc] != mark_color) {
		TOKEN = mark_color = MAP_COL[mark_qc];
		PT_OP_BY();
	}
	
}  /* end csm_color() */


/**************************************************/
VOID	rsm_color()
{
	GT_BYTE();
	mark_color = TOKEN;
	
}  /* end rsm_color() */


/**************************************************/
VOID	csf_interior()
{
	CONTRL[4]=1;
	if ( (INTOUT[0] = TOKEN = range(INTIN[0], 0, MX_FIL_STYLE, 0)) !=
		fill_style) {
		fill_style = TOKEN;
		PT_OP_BY();
	}
	
}  /* end csf_interior() */


/**************************************************/	
VOID	rsf_interior()
{
	GT_BYTE();
	fill_style = TOKEN;
	st_fl_ptr();
	
}  /* end rsf_interior() */


/**************************************************/
VOID	csf_style()
{
	CONTRL[4]=1;
	INTOUT[0] = fill_qi =
		(TOKEN = range(INTIN[0] - 1, 0, MX_FIL_INDEX - 1, 0)) + 1;
	if (TOKEN != fill_index) {
		fill_index = TOKEN;
		PT_OP_BY();
	}
	
}  /* end csf_style() */

/**************************************************/
VOID	rsf_style()
{
	GT_BYTE();
	fill_qi = (fill_index = TOKEN) + 1;
	st_fl_ptr();
	
}  /* end rsf_style() */

/**************************************************/
VOID	csf_color()
{
	CONTRL[4]=1;
	INTOUT[0] = fill_qc = TOKEN = range(INTIN[0], 0, MAX_COLOR - 1, 1);
	if (MAP_COL[fill_qc] != fill_color) {
		TOKEN = fill_color = MAP_COL[fill_qc];
		PT_OP_BY();
	}
	
}  /* end csf_color() */


/**************************************************/
VOID	rsf_color()
{
	GT_BYTE();
	fill_color = TOKEN;
	
}  /* end rsf_color() */


/**************************************************/
VOID	cswr_mode()
{
	CONTRL[4]=1;
	INTOUT[0] = write_qm =
		(TOKEN = range(INTIN[0] - 1, 0, MAX_WRITE_MODE, 0)) + 1;
	if (TOKEN != WRT_MODE) {
		WRT_MODE = TOKEN;
		PT_OP_BY();
	}
	
}  /* end cswr_mode() */


/**************************************************/
VOID	rswr_mode()
{
	GT_BYTE();
	WRT_MODE = TOKEN;
	
}  /* end rswr_mode() */


/**************************************************/
VOID	csf_perimeter()
{
	CONTRL[4]=1;
	if ((INTOUT[0] = TOKEN = (INTIN[0]) ? TRUE : FALSE) != fill_per) {
		fill_per = TOKEN;
		PT_OP_BY();
	}
	
}  /* end csf_perimeter() */


/**************************************************/
VOID	rsf_perimeter()
{
	GT_BYTE();
	fill_per = TOKEN;
	
}  /* end rsf_perimeter() */


/**************************************************/
VOID	csf_udfl()
{
	CONTRL[3] = 16;
	PT_OP_BY();
	PT_INTIN();
	
}  /* end csf_udfl() */


/**************************************************/
VOID	rsf_udfl()
{
	WORD	i;
	
	CONTRL[3] = 16;
	GT_BYTE();
	GT_INTIN();

	for (i = 0; i < 16; i++)
		UD_PATRN[i] = INTIN[i];
	
}  /* end rsf_udfl() */


/**************************************************/
VOID	csl_udsty()
{
	if ((TOKEN = INTIN[0]) != LINE_STYL[6]) {
		LINE_STYL[6] = TOKEN;
		PT_OP_WD();
	}
	
}  /* end csl_udsty() */


/**************************************************/
VOID	rsl_udsty()
{
	GT_WORD();
	LINE_STYL[6] = TOKEN;
	
}  /* end rsl_udsty() */


/**************************************************/
VOID	c_clip()
{
	TOKEN = clip = INTIN[0];		/* enable/disable clipping */
	if (clip) {
		arb_corner(PTSIN, ULLR);
		if (PTSIN[0] < 0)
			PTSIN[0] = 0;
		if (PTSIN[2] > xres)
			PTSIN[2] = xres;
		if (PTSIN[1] < 0)
			PTSIN[1] = 0;
		if (PTSIN[3] > yres)
			PTSIN[3] = yres;
	}	
	else {
		PTSIN[0] = PTSIN[1] = 0;
		PTSIN[2] = xres;
		PTSIN[3] = yres;
	}  /* End else:  clipping turned off. */

	xmn_clip = PTSIN[0];
	YCL_MIN = PTSIN[1];
	xmx_clip = PTSIN[2];
	YCL_MAX = PTSIN[3];

	PT_OP_BY();
	CONTRL[1] = 2;
	PT_PTSIN();
	new_clip = TRUE;
	
}  /* end c_clip() */


/**************************************************/
VOID	r_clip()
{
	GT_BYTE();
	clip = WINDOW | TOKEN;
	CONTRL[1] = 2;
	GT_PTSIN();

	xmn_clip = (XW_MIN > PTSIN[0]) ? XW_MIN : PTSIN[0];
	YCL_MIN = (YW_MIN > PTSIN[1]) ? YW_MIN : PTSIN[1];
	ymn_clip = (YS_MIN > YCL_MIN) ? YS_MIN : YCL_MIN;
	xmx_clip = (XW_MAX < PTSIN[2]) ? XW_MAX : PTSIN[2];
	YCL_MAX = (YW_MAX < PTSIN[3]) ? YW_MAX : PTSIN[3];
	ymx_clip = (YS_MAX < YCL_MAX) ? YS_MAX : YCL_MAX;

	if (g_devdout) {
		fcl_xmin = dvt_fres(xmn_clip);
		fcl_ymin = dvt_fres(YCL_MIN);
		fcl_xmax = dvt_fres(xmx_clip);
		fcl_ymax = dvt_fres(YCL_MAX);
	}  /* End if:  device dependent output time. */
	
}  /* end r_clip() */


/**************************************************/
VOID	Get_Poly()
{
	WORD	i;

	GT_BYTE();
	CONTRL[1] = TOKEN;
	i = ((CONTRL[1] = TOKEN) << 1) - 1;
	GT_PTSIN();
	CONTRL[1]--;
	Y2 = PTSIN[i];
	Y1 = PTSIN[i-1];
	
}  /* end Get_Poly() */


/**************************************************/
VOID	put_poly(extent_offset)
WORD extent_offset;
{
	WORD	i, j, k;

	if (CONTRL[1] > 0) {
		Y1 = Y2 = PTSIN[1];
		j = 1;
		for (i = 2; i <= CONTRL[1]; i++) {
			j += 2;
			k = PTSIN[j];
			if (k > Y2)
				Y2 = k;
			else if (k < Y1)
				Y1 = k;
		}

		PTSIN[i = CONTRL[1] << 1] = Y1 - extent_offset;
		PTSIN[++i] = Y2 + extent_offset;
		CONTRL[1] = TOKEN = CONTRL[1] + 1;
		PT_OP_BY();
		PT_PTSIN();
	}
	
}  /* end put_poly() */


/************************************************
 * Called ONCE ONLY, from v_opnwk() in monout.c	*
 ************************************************/	 
VOID	init_g()
{
	Abort = FALSE;
	save_defaults();
	open_sp();
	clearmem();
	init_p();

	if (!Abort) {
		init_dev();
		CURALPHA = 0xff;
		REQALPHA = 0;
		VMU = 0;
	}  /* End if:  successful. */
	
}  /* end init_g() */


/**************************************************/
VOID	save_defaults()
{
	d_clip = clip;
	d_xmn_clip = xmn_clip;
	d_ycl_min = YCL_MIN;
	d_xmx_clip = xmx_clip;
	d_ycl_max = YCL_MAX;
	d_ln_index = line_index;
	d_qi_line = line_qi;
	d_ln_color = line_color;
	d_qc_line = line_qc;
	d_mk_index = mark_index;
	d_qi_mark = mark_qi;
	d_mk_color = mark_color;
	d_qc_mark = mark_qc;
	d_fa_style = fill_style;
	d_fa_index = fill_index;
	d_qi_fill = fill_qi;
	d_fa_color = fill_color;
	d_qc_fill = fill_qc;

	d_patmsk = patmsk;
	d_patptr = patptr;
	d_wrt_mode = WRT_MODE;
	d_mk_height = mark_height;
	d_mk_scale = mark_scale;
	d_ln_width = line_width;
	d_ln_beg = line_beg;
	d_ln_end = line_end;
	d_fill_per = fill_per;

	save_text_defaults();
	
}  /* end save_defaults() */


/**************************************************/
VOID	reset_defaults()
{
	clip =  d_clip;
	xmn_clip = d_xmn_clip;
	YCL_MIN = d_ycl_min;
	xmx_clip = d_xmx_clip;
	YCL_MAX = d_ycl_max;
	line_index = d_ln_index;
	line_qi = d_qi_line;
	line_color = d_ln_color;
	line_qc = d_qc_line;
	mark_index = d_mk_index;
	mark_qi = d_qi_mark;
	mark_color = d_mk_color;
	mark_qc = d_qc_mark;
	fill_style = d_fa_style;
	fill_index = d_fa_index;
	fill_qi = d_qi_fill;
	fill_color = d_fa_color;
	fill_qc = d_qc_fill;

	patmsk = d_patmsk;
	patptr = d_patptr;
	WRT_MODE = d_wrt_mode;
	mark_height = d_mk_height;
	mark_scale = d_mk_scale;
	line_width = d_ln_width;
	line_beg = d_ln_beg;
	line_end = d_ln_end;
	fill_per = d_fill_per;

	reset_text_defaults();
	
}  /* end reset_defaults() */


/****************************************
 * Called from v_clswk() in monout.c	*
 *   and from v_opnwk() if (abort)	*
 ****************************************/
VOID	dinit_g()
{
	dini_dev();
	dinit_p();
	close_sp();
	
}  /* end dinit_g() */


/**************************************************/
WORD	range(value, low, high, def)
WORD value, low, high, def;
{
	if (value < low || value > high)
		return(def);
	else
		return(value);
	
}  /* end range() */


