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

EXTERN	WORD		BG_BP_1;
EXTERN	WORD		BG_BP_2;
EXTERN	WORD		BG_BP_3;
EXTERN	WORD		BG_BP_4;
EXTERN	WORD		clip;
EXTERN	WORD		DITHRMSK;
EXTERN	WORD		DIV_FACT;
EXTERN	WORD		FG_BP_1;
EXTERN	WORD		FLIP_Y;
EXTERN	WORD		G_PAGE;
EXTERN	WORD		A_PAGE;
EXTERN	WORD		HAT_0_MSK;
EXTERN	WORD		HAT_1_MSK;
EXTERN	WORD		HOLLOW;
EXTERN	WORD		LN_MASK;
EXTERN	WORD		LSTLIN;
EXTERN	WORD		OEMMSKPAT;
EXTERN	WORD		SOLID;
EXTERN	WORD		TOKEN;
EXTERN	WORD		VMU;
EXTERN	WORD		WINDOW;
EXTERN	WORD		WRT_MODE;
EXTERN	WORD		X1;
EXTERN	WORD		X2;
EXTERN	WORD		xmn_clip;
EXTERN	WORD		xmx_clip;
EXTERN	WORD		XW_MAX;
EXTERN	WORD		XW_MIN;
EXTERN	WORD		Y1;
EXTERN	WORD		Y2;
EXTERN	WORD		YCL_MAX;
EXTERN	WORD		YCL_MIN;
EXTERN	WORD		ymn_clip;
EXTERN	WORD		ymx_clip;
EXTERN	WORD		YS_MAX;
EXTERN	WORD		YS_MIN;
EXTERN	WORD		YW_MAX;
EXTERN	WORD		YW_MIN;
EXTERN	WORD		angle;
EXTERN	WORD		beg_ang;
EXTERN	WORD		del_ang;
EXTERN	WORD		deltay1;
EXTERN	WORD		deltay2;
EXTERN	WORD		deltay;
EXTERN	WORD		end_ang;
EXTERN	WORD		fcl_xmax;
EXTERN	WORD		fcl_xmin;
EXTERN	WORD		fcl_ymax;
EXTERN	WORD		fcl_ymin;
EXTERN	WORD		fill_color;
EXTERN	WORD		fill_index;
EXTERN	WORD		fill_intersect;
EXTERN	WORD		fill_maxy;
EXTERN	WORD		fill_miny;
EXTERN	WORD		fill_per;
EXTERN	WORD		fill_qc;
EXTERN	WORD		fill_qi;
EXTERN	WORD		fill_qp;
EXTERN	WORD		fill_style;
EXTERN	WORD		line_beg;
EXTERN	WORD		line_color;
EXTERN	WORD		line_end;
EXTERN	WORD		line_index;
EXTERN	WORD		line_qc;
EXTERN	WORD		line_qi;
EXTERN	WORD		line_qw;
EXTERN	WORD		line_width;
EXTERN	WORD		mark_color;
EXTERN	WORD		mark_height;
EXTERN	WORD		mark_index;
EXTERN	WORD		mark_qc;
EXTERN	WORD		mark_qi;
EXTERN	WORD		mark_scale;
EXTERN	WORD		n_steps;
EXTERN	WORD		need_update;
EXTERN	WORD		new_clip;		/* needed only for text.c */
EXTERN	WORD		num_qc_lines;
EXTERN	WORD		odeltay;
EXTERN	WORD		p_orient;
EXTERN	WORD		patmsk;
EXTERN	WORD		s_begsty;
EXTERN	WORD		s_endsty;
EXTERN	WORD		s_fill_color;
EXTERN	WORD		s_fill_per;
EXTERN	WORD		s_patmsk;
EXTERN	WORD		slice_sz;
EXTERN	WORD		start;
EXTERN	WORD		copies;
EXTERN	WORD		tray;
EXTERN	WORD		write_qm;
EXTERN	WORD		xc;
EXTERN	WORD		xfm_mode;
EXTERN	WORD		xmn_rect;
EXTERN	WORD		xmx_rect;
EXTERN	WORD		xrad;
EXTERN	WORD		y;
EXTERN	WORD		yc;
EXTERN	WORD		ymn_rect;
EXTERN	WORD		ymx_rect;
EXTERN	WORD		yrad;

EXTERN	BOOLEAN		abort;

EXTERN	BYTE		curalpha;
EXTERN	BYTE		reqalpha;

EXTERN	WORD		*patptr;
EXTERN	WORD		*s_patptr;

EXTERN	WORD		CONTRL[];
EXTERN	WORD		dev_tab[];
EXTERN	WORD		DITHER[];
EXTERN	WORD		HATCH0[];
EXTERN	WORD		HATCH1[];
EXTERN	WORD		inq_pts[];
EXTERN	WORD		inq_tab[];
EXTERN	WORD		INTIN[];
EXTERN	WORD		INTOUT[];
EXTERN	WORD		LINE_STYL[];
EXTERN	WORD		MAP_COL[];
EXTERN	WORD		OEMPAT[];
EXTERN	WORD		PTSIN[];
EXTERN	WORD		PTSOUT[];
EXTERN	WORD		siz_tab[];
EXTERN	WORD		UD_PATRN[];
EXTERN	WORD		q_circle[];

