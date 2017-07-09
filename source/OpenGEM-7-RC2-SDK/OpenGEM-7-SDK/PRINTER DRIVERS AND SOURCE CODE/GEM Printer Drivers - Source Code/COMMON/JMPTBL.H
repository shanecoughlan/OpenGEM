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

/* Routines for the capture portion of the driver */

extern	VOID	v_nop();			/*   0 */
extern	VOID	v_opnwk();			/*   1 */
extern	VOID	v_clswk();			/*   2 */
extern	VOID	v_clrwk();			/*   3 */
extern	VOID	v_updwk();			/*   4 */
extern	VOID	c_escape();			/*   5 */
extern	VOID	c_pline();			/*   6 */
extern	VOID	c_pmarker();			/*   7 */
extern	VOID	c_gtext();			/*   8 */
extern	VOID	c_fillarea();			/*   9 */
extern	VOID	c_gdp();			/*  11 */
extern	VOID	cst_height();			/*  12 */
extern	VOID	cst_rotation();			/*  13 */
extern	VOID	csl_type();			/*  15 */
extern	VOID	csl_width();			/*  16 */
extern	VOID	csl_color();			/*  17 */
extern	VOID	csm_type();			/*  18 */
extern	VOID	csm_height();			/*  19 */
extern	VOID	csm_color();			/*  20 */
extern	VOID	cst_font();			/*  21 */
extern	VOID	cst_color();			/*  22 */
extern	VOID	csf_interior();			/*  23 */
extern	VOID	csf_style();			/*  24 */
extern	VOID	csf_color();			/*  25 */
extern	VOID	cswr_mode();			/*  32 */
extern	VOID	vql_attr();			/*  35 */
extern	VOID	vqm_attr();			/*  36 */
extern	VOID	vqf_attr();			/*  37 */
extern	VOID	dqt_attributes();		/*  38 */
extern	VOID	cst_alignment();		/*  39 */
extern	VOID	vq_extnd();			/* 102 */
extern	VOID	csf_perimeter();		/* 104 */
extern	VOID	cst_background();		/* 105 */
extern	VOID	cst_style();			/* 106 */
extern	VOID	cst_point();			/* 107 */
extern	VOID	csl_ends();			/* 108 */
extern	VOID	csf_udfl();			/* 112 */
extern	VOID	csl_udsty();			/* 113 */
extern	VOID	dqt_extent();			/* 116 */
extern	VOID	dqt_width();			/* 117 */
extern	VOID	dt_loadfont();			/* 119 */
extern	VOID	dt_unloadfont();		/* 120 */
extern	VOID	c_clip();			/* 129 */
extern	VOID	dqt_name();			/* 130 */
extern	VOID	dqt_fontinfo();			/* 131 */
extern  VOID	c_filrec();
/* Routines for the replay portion of the driver */

extern	VOID	r_escape();			/*   5 */
extern	VOID	r_pline();			/*   6 */
extern	VOID	r_pmarker();			/*   7 */
extern	VOID	r_gtext();			/*   8 */
extern	VOID	r_fillarea();			/*   9 */
extern	VOID	r_gdp();			/*  11 */
extern	VOID	rst_height();			/*  12 */
extern	VOID	rst_rotation();			/*  13 */
extern	VOID	rsl_type();			/*  15 */
extern	VOID	rsl_width();			/*  16 */
extern	VOID	rsl_color();			/*  17 */
extern	VOID	rsm_type();			/*  18 */
extern	VOID	rsm_height();			/*  19 */
extern	VOID	rsm_color();			/*  20 */
extern	VOID	rst_font();			/*  21 */
extern	VOID	rst_color();			/*  22 */
extern	VOID	rsf_interior();			/*  23 */
extern	VOID	rsf_style();			/*  24 */
extern	VOID	rsf_color();			/*  25 */
extern	VOID	rswr_mode();			/*  32 */
extern	VOID	rst_alignment();		/*  39 */
extern	VOID	rsf_perimeter();		/* 104 */
extern	VOID	rst_background();		/* 105 */
extern	VOID	rst_style();			/* 106 */
extern	VOID	rst_point();			/* 107 */
extern	VOID	rsl_ends();			/* 108 */
extern	VOID	rsf_udfl();			/* 112 */
extern	VOID	rsl_udsty();			/* 113 */
extern	VOID	r_clip();			/* 129 */
extern  VOID	r_filrec();
