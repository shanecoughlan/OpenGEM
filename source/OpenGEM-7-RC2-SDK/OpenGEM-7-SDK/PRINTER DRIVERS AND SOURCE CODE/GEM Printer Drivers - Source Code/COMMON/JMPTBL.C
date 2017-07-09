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
#include "jmptbl.h"
#include "gsxdef.h"
#include "defines.h"
#include "gsxextrn.h"

#if cdo_text
	EXTERN	VOID	finish_window();
	EXTERN	WORD	gvp;
	GLOBAL	WORD	devtext_y;
#endif

EXTERN	BOOLEAN	Dev_rect();
EXTERN	VOID	ini_page();
EXTERN	VOID	dvt_downfont();
EXTERN	VOID	flush_sp();
EXTERN	VOID	set_bufs();
EXTERN	VOID	enter_gr();
EXTERN	VOID	reinit_struct();
EXTERN	VOID	init_image();
EXTERN	VOID	rewnd_sp();
EXTERN	VOID	GT_BYTE();
EXTERN	VOID	reset_defaults();
EXTERN	VOID	scan_out();
EXTERN	VOID	exit_gr();
EXTERN	VOID	dt_preqcr();
EXTERN	WORD	dvt_fres();
EXTERN	VOID	ini_bufs();
EXTERN	VOID	arb_corner();

EXTERN	BOOLEAN		Abort;

GLOBAL	WORD		g_devdcnt = 0;
GLOBAL	WORD		g_devdout = 0;
GLOBAL	BOOLEAN		Bitmap = 0;
GLOBAL	BOOLEAN		first_page = 0;


/* "Collect" tables */
VOID	(*jmptb1[])() =
{
	v_nop,
	v_opnwk,
	v_clswk,
	v_clrwk,
	v_updwk,
	c_escape,
	c_pline,
	c_pmarker,
	c_gtext,
	c_fillarea,
	v_nop,
	c_gdp,
	cst_height,
	cst_rotation,
	v_nop,
	csl_type,
	csl_width,
	csl_color,
	csm_type,
	csm_height,
	csm_color,
	cst_font,
	cst_color,
	csf_interior,
	csf_style,
	csf_color,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	cswr_mode,
	v_nop,
	v_nop,
	vql_attr,
	vqm_attr,
	vqf_attr,
	dqt_attributes,
	cst_alignment
};

VOID	(*jmptb2[])() =
{
	v_nop,
	v_nop,
	vq_extnd,
	v_nop,
	csf_perimeter,
	v_nop,
	cst_style,
	cst_point,
	csl_ends,
	v_nop,
	v_nop,
	v_nop,
	csf_udfl,
	csl_udsty,
	c_filrec,
	v_nop,
	dqt_extent,
	dqt_width,
	v_nop,
	dt_loadfont,
	dt_unloadfont,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	c_clip,
	dqt_name,
	dqt_fontinfo,
};

/* "Replay" tables */
VOID	(*updtb1[])() =
{
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	r_escape,
	r_pline,
	r_pmarker,
	r_gtext,
	r_fillarea,
	v_nop,
	r_gdp,
	rst_height,
	rst_rotation,
	v_nop,
	rsl_type,
	rsl_width,
	rsl_color,
	rsm_type,
	rsm_height,
	rsm_color,
	rst_font,
	rst_color,
	rsf_interior,
	rsf_style,
	rsf_color,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	rswr_mode,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	rst_alignment
};

VOID	(*updtb2[])() =
{
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	rsf_perimeter,
	v_nop,
	rst_style,
	rst_point,
	rsl_ends,
	v_nop,
	v_nop,
	v_nop,
	rsf_udfl,
	rsl_udsty,
	r_filrec,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	v_nop,
	r_clip,
	v_nop,
	v_nop,
};


/********************************************************
 *  Executes a given routine in one of the jump tables.	*
 *  Called once only from pentry() in pentry.a86	*
 ********************************************************/
VOID	printer() 
{
	WORD	opcode;

	opcode = CONTRL[0];
	CONTRL[2] = CONTRL[4] = 0;
	
	if (opcode <= 39)
		(*jmptb1[opcode])();
	else if (opcode >= 100 && opcode <= 131)
		(*jmptb2[opcode - 100])();

}  /* end printer() */


/****************************************************************
 *  bld_page()							*
 *	Build the page.  This routine is the core of v_updwk()	*
 *	and out_win().						*
 ****************************************************************/
VOID	bld_page()
{
	WORD	opcode;
	WORD	xmn_save, xmx_save, ymn_save, ymx_save;
	
	flush_sp();
	set_bufs();
	rewnd_sp();
	GT_BYTE();
	if ((opcode = CONTRL[0] = TOKEN) == 0)
		return;

	if (first_page == TRUE) {
		ini_page();
		first_page = FALSE;
	}
	xmn_save = xmn_clip;
	xmx_save = xmx_clip;
	ymn_save = YCL_MIN;
	ymx_save = YCL_MAX;

	dvt_downfont();

	if (Bitmap) {
		enter_gr();			/* devdep.xxx */
		/* save current y position on page for any device text */
#if cdo_text
		if (WINDOW == TRUE)
			devtext_y = gvp;
#endif		
		init_image();

		YS_MIN = YW_MIN;
		YS_MAX = YW_MIN + slice_sz - 1;
		g_devdout = FALSE;
		while (YS_MIN <= YW_MAX) {
			rewnd_sp();
			GT_BYTE();
			if ((opcode = CONTRL[0] = TOKEN) == 0)
				return;
			reset_defaults();

			/* If a window is being output, turn */
			/* on clipping.                      */
			if ((clip = WINDOW) != FALSE) {
				ymn_clip = (YS_MIN > YCL_MIN) ?
					YS_MIN : YCL_MIN;
				ymx_clip = (YS_MAX < YCL_MAX) ?
					YS_MAX : YCL_MAX;
			}
			else {
				ymn_clip = YS_MIN;
				ymx_clip = YS_MAX;
			}

			if (ymx_clip > YW_MAX) ymx_clip = YW_MAX;
			if (xmn_clip < XW_MIN) xmn_clip = XW_MIN;
			if (xmx_clip > XW_MAX) xmx_clip = XW_MAX;

			while (opcode != 0) {
				if (opcode <= 39) (*updtb1[opcode])();
				else if ((opcode >= 100) && (opcode <= 131)) 
					(*updtb2[opcode - 100])();
				GT_BYTE();
				opcode = CONTRL[0] = TOKEN;
			}

			scan_out();
			if (Abort)
				break;
			YS_MIN += slice_sz;
			YS_MAX += slice_sz;
		}
		exit_gr();
	}  /* End if:  bit-map needs to be output. */

	if (g_devdcnt) {
		dt_preqcr();	/* init devdependent font pointer */
		g_devdout = TRUE;	
		rewnd_sp();
		GT_BYTE();

		reset_defaults();
		fcl_xmin = dvt_fres(XW_MIN = xmn_clip = 0);
		fcl_xmax = dvt_fres(XW_MAX = xmx_clip = xres);
		fcl_ymin = dvt_fres(YW_MIN = ymn_clip = 0);
		fcl_ymax = dvt_fres(YW_MAX = ymx_clip = yres);
		while ((opcode = CONTRL[0] = TOKEN) != 0) {
			if (opcode <= 39) (*updtb1[opcode])();
			else if ((opcode >= 100) && (opcode <= 131))
				(*updtb2[opcode - 100])();
			GT_BYTE();
		}
	}	   

	ini_bufs();
	need_update = FALSE;
	xmn_clip = xmn_save;
	xmx_clip = xmx_save;
	YCL_MIN = ymn_save;
	YCL_MAX = ymx_save;
		
}  /* end bld_page() */


/****************************************************************
 *  Called from the jump table and from v_clswk() in monout.c	*
 ****************************************************************/
VOID	v_updwk()
{
	WORD	copy_num;

	if (need_update) 
	    for (copy_num = 0; copy_num < copies; copy_num++) {
		if (xmn_rect > xmx_rect || ymn_rect > ymx_rect)
			Bitmap = WINDOW = FALSE;
		else
			WINDOW = Dev_rect();
		bld_page();
		if (copies > 1)
    		    reinit_struct();		/* for subsequent copies */
	    } /* End for: each copy of file */
	    
}  /* end v_updwk() */


/********************************************************
 *  Called once only from c_escape in monoprin.c	*
 ********************************************************/
VOID	out_win()
{
    /* Assume arbitrary corners were passed in.  Fix 'em. */
	arb_corner(PTSIN, ULLR);		/* in monoprin.c */
	XW_MIN = PTSIN[0];
	YW_MIN = PTSIN[1];
	XW_MAX = PTSIN[2];
	YW_MAX = PTSIN[3];
	WINDOW = TRUE;

    /* Build the page. */
	bld_page();

    /* Restore the window to default. */
	WINDOW = FALSE;
	XW_MIN = YW_MIN = 0;
	XW_MAX = xres;
	YW_MAX = yres;

    /* Move to left edge of page and get default font back */	
#if cdo_text	
	finish_window();
#endif
		
}  /* end out_win() */
