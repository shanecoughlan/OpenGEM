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
#include "fontdef.h" 
#include "gsxextrn.h"

EXTERN	VOID	PT_OP_WD();
EXTERN	VOID	GT_WORD();
EXTERN	BOOLEAN	dvt_set_font();
EXTERN	VOID	PT_OP_BY();
EXTERN	VOID	GT_BYTE();
EXTERN	WORD	range();
EXTERN	VOID	inc_lfu();
EXTERN	VOID	PT_INTIN();
EXTERN	VOID	GT_INTIN();
EXTERN	VOID	chk_fnt();
EXTERN	VOID	chk_clip();
EXTERN	VOID	hdr_addr();
EXTERN	VOID	itxt_style();
EXTERN	VOID	set_effects();
EXTERN	VOID	dqt_extent();
EXTERN	VOID	set_just();
EXTERN	VOID	horz_alig();
EXTERN	VOID	vert_alig();
EXTERN	VOID	chk_hdr();
EXTERN	VOID	text_blt();
EXTERN	WORD	clip_line();
EXTERN	VOID	ABLINE();

#if cdo_text
	EXTERN	VOID	set_font();
	EXTERN	VOID	set_undl();
	EXTERN	VOID	set_hmi();
	EXTERN	VOID	set_hv();
	EXTERN	VOID	dvt_etext();	
	EXTERN	VOID	dvt_init();
	EXTERN	VOID	dvt_lf();
	EXTERN	WORD	dvt_fres();
	EXTERN	WORD	dvt_bres();
	EXTERN	WORD	text_out();	
	EXTERN	VOID	just_align();
#endif	

#if cdo_text
	EXTERN	WORD		g_devdcnt;
	EXTERN	WORD		devtext_y;		/* 12/22 */
#endif

EXTERN	WORD		FG_BP_1;
EXTERN	WORD		FLIP_Y;
EXTERN	WORD		LN_MASK;
EXTERN	WORD		WRT_MODE;
EXTERN	WORD		X1;
EXTERN	WORD		X2;
EXTERN	WORD		XFM_MODE;
EXTERN	WORD		Y1;
EXTERN	WORD		Y2;
EXTERN	WORD		buff_seg;
EXTERN	WORD		dv_attr;
EXTERN	WORD		dv_font;
EXTERN	WORD		dv_ht;
EXTERN	WORD		dv_pt;
EXTERN	WORD		dv_rot;
EXTERN	WORD		g_devdout;
EXTERN	WORD		off_fmgr;
EXTERN	WORD		seg_fmgr;
EXTERN	WORD		txbuf1;
EXTERN	WORD		txbuf2;
EXTERN	BOOLEAN		Bitmap;
EXTERN	BOOLEAN		dv_cur;
EXTERN	BOOLEAN		dv_loaded;
EXTERN	BOOLEAN		dv_selpt;
EXTERN	WORD		dev_tab[];
EXTERN	WORD		siz_tab[];
EXTERN	FHEAD		*dv_first;
EXTERN	FHEAD		*dv_mem;

GLOBAL	WORD		CHUP = 0;
GLOBAL	WORD		DELX = 0;
GLOBAL	WORD		DELY = 0;
GLOBAL	WORD		DESTX = 0;
GLOBAL	WORD		DESTY = 0;
GLOBAL	WORD		DOUBLE = 0;
GLOBAL	WORD		FWIDTH = 0;
GLOBAL	WORD		LITEMASK = 0;
GLOBAL	WORD		L_OFF = 0;
GLOBAL	WORD		PSTYLE = 0;
GLOBAL	WORD		R_OFF = 0;
GLOBAL	WORD		SKEWMASK = 0;
GLOBAL	WORD		SOURCEX = 0;
GLOBAL	WORD		SOURCEY = 0;
GLOBAL	WORD		STYLE = 0;
GLOBAL	WORD		TEXT_BP = 0;
GLOBAL	WORD		WEIGHT = 0;
GLOBAL	WORD		charx = 0;
GLOBAL	WORD		chary = 0;
GLOBAL	WORD		d_LITEMASK = 0;
GLOBAL	WORD		d_L_OFF = 0;
GLOBAL	WORD		d_R_OFF = 0;
GLOBAL	WORD		d_SKEWMASK = 0;
GLOBAL	WORD		d_STYLE = 0;
GLOBAL	WORD		d_WEIGHT = 0;
GLOBAL	WORD		d_chup = 0;
GLOBAL	WORD		d_dv_attr = 0;
GLOBAL	WORD		d_dv_font = 0;
GLOBAL	WORD		d_dv_ht = 0;
GLOBAL	WORD		d_dv_pt = 0;
GLOBAL	WORD		d_dv_rot = 0;
GLOBAL	WORD		d_h_align = 0;
GLOBAL	WORD		d_rq_attr = 0;
GLOBAL	WORD		d_text_color = 0;
GLOBAL	WORD		d_text_qc = 0;
GLOBAL	WORD		d_v_align = 0;
GLOBAL	WORD		h_align = 0;
GLOBAL	WORD		height = 0;
GLOBAL	WORD		in_char = 0;
GLOBAL	WORD		in_word = 0;
GLOBAL	WORD		loaded = 0;
GLOBAL	WORD		rmdchar = 0;
GLOBAL	WORD		rmdcharx = 0;
GLOBAL	WORD		rmdchary = 0;
GLOBAL	WORD		rmdword = 0;
GLOBAL	WORD		rmdwordx = 0;
GLOBAL	WORD		rmdwordy = 0;
GLOBAL	WORD		rq_attr = 0;
GLOBAL	WORD		spaces = 0;
GLOBAL	WORD		spw_max = 0;
GLOBAL	WORD		spw_min = 0;
GLOBAL	WORD		tcell_ht = 0;
GLOBAL	WORD		tdelv = 0;
GLOBAL	WORD		text_color = 0;
GLOBAL	WORD		text_qc = 0;
GLOBAL	WORD		v_align = 0;
GLOBAL	WORD		width = 0;
GLOBAL	WORD		wordx = 0;
GLOBAL	WORD		wordy = 0;
GLOBAL	BOOLEAN		d_dv_cur = 0;
GLOBAL	BOOLEAN		d_dv_selpt = 0;
GLOBAL	UWORD		*FOFF = 0;
GLOBAL	BYTE		*dv_map = 0;
GLOBAL	FHEAD		*act_font = 0;
GLOBAL	FHEAD		*act_save = 0;
GLOBAL	FHEAD		*cur_font = 0;
GLOBAL	FHEAD		*d_cur_font = 0;
GLOBAL	FHEAD		*prev_font = 0;
GLOBAL	FHEAD		first = 
{
	-1,				/* WORD font_id		*/
	-1,				/* WORD point size	*/
	"dummy font",			/* BYTE name[32]	*/
	0,				/* UWORD first_ade	*/
	127,				/* UWORD last_ade	*/
	1,	    			/* UWORD top		*/
	1,	    			/* UWORD ascent		*/
	1,	    			/* UWORD half		*/
	1,	    			/* UWORD descent	*/
	1,				/* UWORD bottom		*/
	1,				/* UWORD max_char_width	*/
	1,				/* UWORD max_cell_width	*/
	1,				/* UWORD left_offset	*/
	1,				/* UWORD right_offset	*/
	-1,				/* UWORD thicken	*/
	-1,				/* UWORD ul_size	*/
	-1,     			/* UWORD lighten	*/
	-1,				/* UWORD skew		*/
	MONOSPACE | DEFAULT,		/* UWORD flags        	*/
	0,        			/* UBYTE *hor_table	*/
	0,        			/* UWORD *off_table	*/
	0,         			/* UWORD *dat_table	*/
	-1,				/* UWORD form_width	*/
	-1,				/* UWORD form_height	*/
	0,				/* UWORD *next_font	*/
	0,				/* FHEAD *next_sect   	*/
	0,				/* WORD lfu_low       	*/
	0,				/* WORD lfu_high      	*/
	0,				/* WORD *file_off     	*/
	0,				/* WORD data_size     	*/
	0, 0, 0, 0, 0,			/* WORD reserve[5]    	*/
	0,				/* BYTE *map_addr     	*/
	0,				/* UWORD dev_flags    	*/
	""				/* BYTE esc_seq[32]   	*/
};


/********************************************************
 *  Find a font in the chain that has the given effect	*
 ********************************************************/
FHEAD	*sel_effect(f_ptr)
FHEAD *f_ptr;
{
	WORD		size;
	BYTE		eff;
	BYTE		find;
	BYTE		font;
	FHEAD		*bold;
	FHEAD		*italic;
	FHEAD		*normal;
	FHEAD		*ptr;

    /* Determine what is being searched for. */
	font = f_ptr->font_id;
	size = f_ptr->point;
	find = rq_attr & (SKEW | THICKEN);
	STYLE = rq_attr;
	normal = italic = bold = (FHEAD *)NULLPTR;

    /* Scan for a font matching the effect exactly. */
	for (ptr = f_ptr; ptr && ((ptr->font_id & 0xff) == font) &&
			(ptr->point == size); ptr = ptr->next_font) {
		eff = (BYTE)((ptr->font_id >> 8) & 0x00ff);
		if (eff == find) {
			STYLE &= (~find);
			return(ptr);
		}  /* End if:  effect found. */

		if (eff == 0x0)
			normal = ptr;
		else if (eff == THICKEN)
			bold = ptr;
		else if (eff == SKEW)
			italic = ptr;
	}  /* End for:  over fonts. */

    /* No exact match was found.  If the effect is bold-italic, */
    /* return italic (if possible) or bold (if possible).       */
	if (find == (SKEW | THICKEN)) {
		if (italic) {
			STYLE &= (~SKEW);
			return(italic);
		}  /* End if:  returning italic. */
		else if (bold) {
			STYLE &= (~THICKEN);
			return(bold);
		}  /* End elseif:  returning bold. */
	}  /* End if:  bold-italic. */

    /* Running out of alternatives.  Set the STYLE, try to return normal.*/
	STYLE = rq_attr;
	if (normal)
		return(normal);
	else
		return(f_ptr);

}  /* end sel_effect() */


/**********************************************************************
 *
 **********************************************************************/
VOID	dst_height()
{
	WORD		font_id;
	FHEAD		*new_font;

	if (XFM_MODE == 0)
		PTSIN[1] = (dev_tab[1] + 1) - PTSIN[1];

	dv_ht = PTSIN[1];
	if (dvt_set_font())
		return;

	font_id = set_id();
	new_font = cur_font;
	while (new_font->top <= PTSIN[1] &&
			(new_font->font_id & 0xff) == font_id) {
		cur_font = new_font;
		if (!new_font->next_font)
			break;
		new_font = new_font->next_font;
	}	
	cur_font = act_font = sel_effect(cur_font);
	
}  /* end dst_height() */


/**********************************************************************
 *
 **********************************************************************/
VOID	cst_height()
{
	if (cur_font->font_id != -1) {
		TOKEN = PTSIN[1];
		PT_OP_WD();
		dv_selpt = FALSE;
		dst_height();
	}  /* End if:  fonts are loaded. */

	CONTRL[2] = 2;
	PTSOUT[0] = cur_font->max_char_width;
	PTSOUT[1] = cur_font->top;
	PTSOUT[2] = cur_font->max_cell_width;
	PTSOUT[3] = PTSOUT[1] + cur_font->bottom + 1;
	FLIP_Y = 1;

}  /* end cst_height() */


/**********************************************************************
 *
 **********************************************************************/
VOID	rst_height()
{
	GT_WORD();
	PTSIN[1] = TOKEN;
	dv_selpt = FALSE;
	chk_hdr(2, TOKEN);
	dst_height();

}  /* end rst_height() */


/**********************************************************************
 *
 **********************************************************************/
VOID	dst_point()
{
	WORD	font_id;
	FHEAD	*new_font;

	dv_pt = INTIN[0];
	if (dvt_set_font())
		return;

	font_id = set_id();
	new_font = cur_font;
	while (new_font->point <= INTIN[0] &&
			(new_font->font_id & 0xff) == font_id) {
		cur_font = new_font;
		if (!new_font->next_font)
			break;
		if (new_font->point == INTIN[0])
			break;
		new_font = new_font->next_font;
	}	
	cur_font = act_font = sel_effect(cur_font);

}  /* end dst_point() */


/************************************************************************
 *  Set font.  First try to get a device font (Laserjet), then look for	*
 *  a bitmap font.  If there's no font, try to give the first loaded	*
 *  bitmap font.  If no bitmap fonts are loaded, return the dummy font.	*
 *  In case a vst_point() call or a vst_height call preceded this call,	*
 *  set the point/height from here.					*
 ************************************************************************/
VOID	dst_font()
{
	dv_font = INTIN[0];

#if cdo_text	
	if (dvt_set_font()) {
		DOUBLE = STYLE = 0;
		return;
	}  /* End if:  device font found. */
#endif

	for (cur_font = &first; ((cur_font->font_id & 0xff) != INTIN[0]) &&
			cur_font->next_font; cur_font = cur_font->next_font)
		;
	if ((cur_font->font_id & 0xff) != INTIN[0]) {
		if (loaded)
			cur_font = first.next_font;
		else
			cur_font = &first;
	}  /* End if:  font wasn't found. */
	
	if (dv_selpt) {
		INTIN[0] = dv_pt;
		dst_point();
	}  /* End if:  height selected via points. */
	else {
		PTSIN[1] = dv_ht;
		dst_height();
	}  /* End else:  height selected via absolute. */

}  /* end dst_font() */


/**********************************************************************
 *
 **********************************************************************/
VOID	cst_rotation()
{
	CHUP = ((INTIN[0] + 450) / 900) * 900;
	while (CHUP >= 3600)
		CHUP -= 3600;
	while (CHUP < 0)
		CHUP += 3600;
	dv_rot = TOKEN = INTOUT[0] = CHUP;
	PT_OP_WD();
	CONTRL[4] = 1;
	INTIN[0] = dv_font;
	dst_font();

}  /* end cst_rotation() */


/**********************************************************************
 *
 **********************************************************************/
VOID	rst_rotation()
{
	GT_WORD();
	dv_rot = CHUP = TOKEN;
	INTIN[0] = dv_font;
	dst_font();

}  /* end rst_rotation() */


/**********************************************************************
 *
 **********************************************************************/
VOID	cst_font()
{
	TOKEN = INTIN[0];
	PT_OP_BY();
	dst_font();

	CONTRL[4] = 1;
	INTOUT[0] = cur_font->font_id & 0xff;

}  /* end cst_font() */


/**********************************************************************
 *
 **********************************************************************/
VOID	rst_font()
{
	GT_BYTE();
	INTIN[0] = TOKEN;

	chk_hdr(0, TOKEN);
	dst_font();

}  /* end rst_font() */


/**********************************************************************
 *
 **********************************************************************/
VOID	cst_color()
{
	CONTRL[4] = 1;
	INTOUT[0] = text_qc = TOKEN = range(INTIN[0], 0, MAX_COLOR - 1, 1);

	if (MAP_COL[TOKEN] != text_color) {
		TOKEN = text_color = TEXT_BP = MAP_COL[TOKEN];
		PT_OP_BY();
	}  /* End if:  color has changed. */
	
}  /* end cst_color() */


/**********************************************************************
 *
 **********************************************************************/
VOID	rst_color()
{
	GT_BYTE();
	text_color = TEXT_BP = TOKEN;
	
}  /* end rst_color() */


/**********************************************************************
 *
 **********************************************************************/
VOID	cst_style()
{
	BOOLEAN		temp;

	rq_attr = dv_attr = STYLE = TOKEN = INTIN[0] & inq_tab[2];
	PT_OP_BY();
	temp = cur_font->flags & DEVICE_FONT;
	if (!dvt_set_font() && temp) {
		INTIN[0] = dv_font;
		dst_font();
	}  /* End if:  not device text. */
	else
	    itxt_style();

	INTOUT[0] = rq_attr;
	CONTRL[4] = 1;

}  /* end cst_style() */


/**********************************************************************
 *
 **********************************************************************/
VOID	itxt_style()
{
	INTIN[0] = cur_font->point;
	dst_point();

}  /* end itxt_style() */


/**********************************************************************
 *
 **********************************************************************/
VOID	rst_style()
{
	BOOLEAN		temp;

	GT_BYTE();
	rq_attr = dv_attr = STYLE = TOKEN;
	temp = cur_font->flags & DEVICE_FONT;	    /* THIS MAY BE WRONG */
	if (!dvt_set_font() && temp) {
		INTIN[0] = dv_font;
		dst_font();
	}  /* End if:  not device text. */
	else
	    itxt_style();

}  /* end rst_style() */


/**********************************************************************
 *
 **********************************************************************/
VOID	cst_point()
{
	TOKEN = INTIN[0];
	PT_OP_WD();
	dv_selpt = TRUE;
	dst_point();
	inc_lfu();

	CONTRL[2]=2;
	CONTRL[4]=1;
	INTOUT[0] = cur_font->point;
	PTSOUT[0] = cur_font->max_char_width;
	PTSOUT[1] = cur_font->top;
	PTSOUT[2] = cur_font->max_cell_width;
	PTSOUT[3] = PTSOUT[1] + cur_font->bottom + 1;
	FLIP_Y = 1;

}  /* end cst_point() */


/**********************************************************************
 *
 **********************************************************************/
VOID	rst_point()
{
	GT_WORD();
	INTIN[0] = TOKEN;
	dv_selpt = TRUE;
	chk_hdr(1, TOKEN);
	dst_point();
	inc_lfu();

}  /* end rst_point() */


/**********************************************************************
 *
 **********************************************************************/
VOID	cst_alignment()
{
	CONTRL[4] = 2;
	h_align = INTOUT[0] = INTIN[0] = range(INTIN[0], 0, 2, 0);
	v_align = INTOUT[1] = INTIN[1] = range(INTIN[1], 0, 5, 0);
	PT_OP_BY();
	PT_INTIN();

}  /* end cst_alignment() */


/**********************************************************************
 *
 **********************************************************************/
VOID	rst_alignment()
{
	GT_BYTE();
	CONTRL[3] = 2;
	GT_INTIN();

	h_align = INTIN[0];
	v_align = INTIN[1];

}  /* end rst_alignment() */


/**********************************************************************
 *
 **********************************************************************/
BOOLEAN	get_multi(ade)
WORD ade;
{
	FHEAD	*ptr;

	for (ptr = act_font->next_sect; ptr; ptr = ptr->next_sect) {
		if (ade >= ptr->first_ade && ade <= ptr->last_ade) {
			act_save = act_font;
			cur_font = act_font = ptr;
			return(TRUE);
		}  /* End if:  found segment. */
	}  /* End for:  over multi-segments. */
	return(FALSE);

}  /* end get_multi() */


/**********************************************************************
 *
 **********************************************************************/
VOID	res_multi()
{
	act_font = cur_font = act_save;
	act_save = (FHEAD *)NULLPTR;
	
}  /* end res_multi() */


/************************************************************************
 *  Called on the replay side of v_gtext() and v_justified() calls.	*
 ************************************************************************/
VOID	v_gtext()
{		 	   
	WORD	i, j;
	WORD	delh, startx, starty;
	WORD	temp, tx1, tx2, ty1, ty2, xfact, yfact;
	WORD	temp2;
#if cdo_text
	WORD	bottom, dv_x, dv_y, ix, monosts, sp_width;
	WORD	mon_wd, top, wd, str_wid;
#endif	

    /* Do nothing if didn't get v_loadfonts() or no fonts to load */
	if (cur_font == (FHEAD *)NULLPTR || cur_font->font_id == -1)
		return;

 
/********************/
	if (cur_font->flags & DEVICE_FONT) {
		if (!g_devdout)
			return;
	}
	else if (g_devdout)
		return;

#if cdo_text
	if (cur_font->flags & DEVICE_FONT) {	/* Process device text. */ 
		width = 0;	
		j = cur_font->first_ade;
		monosts = cur_font->flags & MONOSPACE ? TRUE : FALSE;

		if (monosts)
		    width = CONTRL[3]*(mon_wd = cur_font->max_cell_width);
		else {
		    for (i = 0; i < CONTRL[3]; i++) {
			ix = INTIN[i] - j;
			width += act_font->off_table[ix + 1] - \
						      act_font->off_table[ix];
		    }  /* End for:  over string. */
		}  /* End else:  proportionally spaced font. */

		if (CONTRL[0] == 11)
			str_wid = PTSIN[2];
		else
			PTSIN[2] = (str_wid = width) >> 1;

		just_align(str_wid);

	    /* Perform trivial rejection clipping. */
		dv_x = dvt_fres(PTSIN[0]);
		dv_y = dvt_fres(PTSIN[1] + devtext_y);	/* 12/22 */
		top = cur_font->top;
		bottom = cur_font->bottom;
		if (dv_rot == 0) {
			if (dv_y - top <= fcl_ymin ||
				dv_y + bottom >= fcl_ymax)
				return;
			if (dv_x >= fcl_xmax)
				return;
		}  /* End if:  portrait. */
		else {
			if (dv_x - top <= fcl_xmin ||
				dv_x + bottom >= fcl_xmax)
				return;
			if (dv_y <= fcl_ymin)
				return;
		}  /* End else: landscape (90 degrees). */

		if (cur_font != prev_font) {
			set_font(&act_font->esc_seq[0]);
			prev_font = cur_font;
		}  /* End if:  font selection needs to be output. */

	    /* Set up justification parameters. */
		ix = ' ' - cur_font->first_ade;
		if (monosts)
			sp_width = mon_wd;
		else
			sp_width = act_font->off_table[ix + 1] -
				act_font->off_table[ix];
		set_hmi(PTSIN[2], width, spaces, sp_width, monosts);
		dv_map = act_font->map_addr;

	    /* Pre-clip the left edge of the string. */
		for (i = 0; i < CONTRL[3]; i++) {
			ix = INTIN[i] - j;
			if (monosts)
				wd = mon_wd;
			else
				wd = act_font->off_table[ix + 1] -
					act_font->off_table[ix];

			if (dv_rot == 0) {
				if (dv_x >= fcl_xmax)
					return;
				if (dv_x >= fcl_xmin)  /* 1/13 WAS dv_x - 1 */
					break;
			}  /* End if:  portrait. */
			else {
				if (dv_y <= fcl_ymin)
					return;
				if (dv_y + 1 <= fcl_ymax)
					break;
			}  /* End else:  landscape (90 degrees). */
			
			wd += charx + chary;
			if (rmdchar > 0) {
				wd += rmdcharx + rmdchary;
				rmdchar--;
			}  /* End if:  remainder available. */
			
			if (INTIN[i] == 32) {
				wd += wordx + wordy;
				if (rmdword > 0)  {
					wd += rmdwordx + rmdwordy;
					rmdword--;
				}  /* End if:  remainder available. */
			}  /* End if:  space character. */
			
			if (dv_rot == 0)
				dv_x += wd;
			else
				dv_y += wd;
		}  /* End for:  over string. */
		
		if (i == CONTRL[3])
			return;
		
		if ((STYLE & UNDER) != (PSTYLE & UNDER)) {
			PSTYLE = STYLE;
			set_undl(STYLE & UNDER);
		}  /* End if:  underlining on. */
		
		set_hv(dvt_bres(dv_x), dvt_bres(dv_y));

		for ( ; i < CONTRL[3]; i++) {
			/* Perform character clipping. */
			ix = INTIN[i] - j;
			if (monosts)
				wd = mon_wd;
			else
				wd = act_font->off_table[ix + 1] -
					act_font->off_table[ix];

			if (dv_rot == 0) {
				if (dv_x >= fcl_xmax)
					break;
				dv_x += wd;
			}  /* End if:  portrait. */
			else {
				if (dv_y <= fcl_ymin)
					break;
				dv_y -= wd;
			}  /* End else:  landscape (90 degrees). */

		    /* Output the character and add in any additional width */
			wd = text_out(INTIN[i], monosts);
			if (dv_rot == 0)
				dv_x += wd;
			else
				dv_y += wd;
		}  /* End for:  over characters. */
		if (STYLE & UNDER) {
			PSTYLE &= ~UNDER;
			set_undl(0);
		}			
	}  /* End if:  device font. */
	else {
	
#endif  /* driver can download device fonts */

	/* Process bitmap text */
	if (CONTRL[0] != 11 && clip_reject())
			return;

		set_effects();
	    /*******************/
		if (h_align == 0)
			delh = 0;
		else {
			if (CONTRL[0] != 11)
				dqt_extent();
			CONTRL[2] = 0;
			delh = (h_align == 1) ? width/2 : width;
		}
	    /*******************/
		if (STYLE & SKEW)
			delh += cur_font->left_offset;
	    /*******************/
		switch (CHUP) {
			case 0:
				startx = DESTX = PTSIN[0] - delh;
				starty = DESTY + cur_font->top +
					cur_font->ul_size + 1;
				xfact = 0;
				yfact = 1;
				break;
			case 900:
				DESTY = PTSIN[1] + delh;
				if (DESTY < ymn_clip)
					return;
				DESTX = PTSIN[0] - tdelv;
				startx = DESTX + cur_font->top +
					cur_font->ul_size + 1;
				starty = DESTY;
				xfact = 1;
				yfact = 0;
				break;
			case 1800:
				startx = DESTX = PTSIN[0] + delh;
				starty = DESTY + tcell_ht - cur_font->top -
					(cur_font->ul_size + 1) - 1;
				xfact = 0;
				yfact = -1;
				break;
			case 2700:
				DESTY = PTSIN[1] - delh;
				if (DESTY > ymx_clip)
					return;
				DESTX = PTSIN[0] - ((tcell_ht - 1) - tdelv);
				startx = DESTX + tcell_ht - cur_font->top -
					(cur_font->ul_size + 1) - 1;
				starty = DESTY;
				xfact = -1;
				yfact = 0;
				break;
		}  /* End switch: CHUP */
	    /*******************/
		act_save = (FHEAD *)NULLPTR;

		for (j = 0; j < CONTRL[3]; j++) {
			if((temp = INTIN[j]) < cur_font->first_ade ||
				temp > cur_font->last_ade) {
				if (!get_multi(temp))
					temp = 32;
			}
			chk_fnt();

			temp -= cur_font->first_ade;

			if (act_font->flags & HORZ_OFF) {
				temp2 = temp << 1;
				switch (CHUP)  {
					case 0:
						DESTX -= act_font->
							hor_table[temp2];
						break;
					case 900:
						DESTY += act_font->
							hor_table[temp2];
						break;
					case 1800:
						DESTX += act_font->
							hor_table[temp2];
						break;
					case 2700:
						DESTY -= act_font->
							hor_table[temp2];
						break;
				}  /* End switch. */
			}  /* End if:  horizontal offset defined. */

			SOURCEX = act_font->off_table[temp];
			DELX = act_font->off_table[temp+1] - SOURCEX;

			SOURCEY = 0;
			DELY = cur_font->form_height;

			FOFF = act_font->dat_table;
			FWIDTH = cur_font->form_width;

			text_blt();

			if (CONTRL[0] == 11) {
				DESTX += charx;
				DESTY += chary;
				if (rmdchar) {
					DESTX += rmdcharx;
					DESTY += rmdchary;
					rmdchar--;
				}

				if (INTIN[j] == 32) {
					DESTX += wordx;
					DESTY += wordy;
					if (rmdword) {
						DESTX += rmdwordx;
						DESTY += rmdwordy;
						rmdword--;
					}
				}
			}
			if (act_font->flags & HORZ_OFF) {
				switch (CHUP) {
					case 0:
						DESTX -= act_font->
							hor_table[temp2 + 1];
						break;
					case 900:
						DESTY += act_font->
							hor_table[temp2 + 1];
						break;
					case 1800:
						DESTX += act_font->
							hor_table[temp2 + 1];
						break;
					case 2700:
						DESTY -= act_font->
							hor_table[temp2 + 1];
						break;
				}  /* End switch. */
			}  /* End if:  horizontal offset defined. */

			if (act_save)
				res_multi();
		} /* End for:  over string. */
	    /*******************/
		if (STYLE & UNDER) {
			X1 = startx;
			Y1 = starty;
			if (CHUP % 1800 == 0) {
				X2 = DESTX;
				Y2 = Y1;
			}
			else {
				X2 = X1;
				Y2 = DESTY;
			}

			LN_MASK = STYLE & LIGHT ? cur_font->lighten : 0xffff;
			FG_BP_1 = TEXT_BP;

			for (i = 0 ; i < cur_font->ul_size ; i++) {
				tx1 = X1;
				tx2 = X2;
				ty1 = Y1;
				ty2 = Y2;
				if (clip) {
					if (clip_line())
						ABLINE();
				}
				else
					ABLINE();

				X1 = tx1 + xfact;
				X2 = tx2 + xfact;
				Y1 = ty1 + yfact;
				Y2 = ty2 + yfact;

				if (LN_MASK & 1)
					LN_MASK = (LN_MASK >> 1) | 0x8000;
				else
					LN_MASK = LN_MASK >> 1;
			} /* End for: 0 to cur_font->ul_size */
		} /* End if: STYLE & UNDER */
#if cdo_text		
	}  /* End else:  bitmapped font. */
#endif

}  /* end v_gtext() */


/**********************************************************************
 *  Escapement text.
 **********************************************************************/
VOID	v_etext()
{		 	   
	WORD		first, i, j, last, last_wid;
	WORD		save_dest, save_valign, temp, top, total;
	WORD		tx1, tx2, ty1, ty2, x_und, y_und;
	WORD		*dest;

	/* Bail out if the timing is wrong. */
	if (cur_font == (FHEAD *)NULLPTR || cur_font->font_id == -1)
		return;
	if (cur_font->flags & DEVICE_FONT) {
		if (!g_devdout)
			return;
	}
	else if (g_devdout)
		return;

#if cdo_text
	if (cur_font->flags & DEVICE_FONT) {	/* Process device text. */
		if (dv_cur) {
			dvt_etext();
			return;
		}
	} else {
#endif
	    /* Process bitmap text */
		save_valign = v_align;
		v_align = 0;
		if (clip_reject())
			return;
		v_align = save_valign;

		set_effects();

		DESTX = PTSIN[0];
		DESTY = PTSIN[1];
		top = cur_font->top;
		if (CHUP == 900)
			for (i = 2; i < CONTRL[3] + 1; i++)
				PTSIN[i] = -PTSIN[i];
		if (CHUP == 0) {
			DESTY -= top;
			dest = &DESTX;
		}  /* End if:  portrait. */
		else {
			DESTX -= top;
			dest = &DESTY;
		}  /* End else:  landscape. */

		act_save = (FHEAD *)NULLPTR;
		first = cur_font->first_ade;
		last = cur_font->last_ade;
		for (j = 0; j < CONTRL[3]; j++) {
			temp = INTIN[j];
			if (temp < first || temp > last)
				if (!get_multi(temp))
					temp = 32;
			chk_fnt();
			temp -= cur_font->first_ade;

			SOURCEX = act_font->off_table[temp];
			DELX = act_font->off_table[temp + 1] - SOURCEX;
			last_wid = DELX;
			SOURCEY = 0;
			DELY = cur_font->form_height;
			FOFF = act_font->dat_table;
			FWIDTH = cur_font->form_width;

			save_dest = *dest;
			text_blt();
			*dest = save_dest + PTSIN[j + 2];

			if (act_save)
				res_multi();
		} /* End for:  over string. */

		if (STYLE & UNDER) {
			total = CHUP == 0 ? last_wid : -last_wid;
			for (i = 2; i < CONTRL[3] + 1; i++)
				total += PTSIN[i];
			X1 = X2 = PTSIN[0];
			Y1 = Y2 = PTSIN[1];
			if (CHUP == 0) {
				X2 += total;
				x_und = 0;
				y_und = 1;
			}  /* End if:  horizontal. */
			else {
				Y2 += total;
				x_und = 1;
				y_und = 0;
			}  /* End else:  vertical. */

			LN_MASK = STYLE & LIGHT ? cur_font->lighten : 0xffff;
			FG_BP_1 = TEXT_BP;

			for (i = 0 ; i < cur_font->ul_size ; i++) {
				tx1 = X1;
				tx2 = X2;
				ty1 = Y1;
				ty2 = Y2;
				if (clip) {
					if (clip_line())
						ABLINE();
				}
				else ABLINE();
				X1 = tx1 + x_und;
				X2 = tx2 + x_und;
				Y1 = ty1 + y_und;
				Y2 = ty2 + y_und;

				if (LN_MASK != 0xffff) {
				    if (LN_MASK & 1)
					LN_MASK = (LN_MASK >> 1) | 0x8000;
				    else LN_MASK = LN_MASK >> 1;
			        }  /* end if:  non-solid line mask. */
			}  /* end for: all ul_sizes */
		}  /* end if: STYLE & UNDER */
		
#if cdo_text		
	}  /* end if: bitmap text */
#endif	

}  /* end v_etext() */


/**********************************************************************
 *  Set text effects.
 **********************************************************************/
VOID	set_effects()
{
	if (STYLE & THICKEN)
		WEIGHT = cur_font->thicken;
	if (STYLE & LIGHT)
		LITEMASK = cur_font->lighten;
	if (STYLE & SKEW) {
		L_OFF = cur_font->left_offset;
		R_OFF = cur_font->right_offset;
		SKEWMASK = cur_font->skew;
	}
	else
		L_OFF = R_OFF = 0;

}  /* end set_effects() */


/**********************************************************************
 *  Called at open workstation time.
 **********************************************************************/
VOID	text_init()
{
	WORD		i, j, id_save;
	FHEAD		*def_save;

	siz_tab[0] = siz_tab[1] = 32767;
	siz_tab[2] = siz_tab[3] = 0;
	
	loaded = FALSE;
	cur_font = &first;
	id_save = cur_font->font_id & 0xff;
	
    /* look through the whole bitmap font chain */
	for (i = 0, j = 0; TRUE; cur_font = cur_font-> next_font) {
		if (cur_font->flags & DEFAULT)
			def_save = cur_font;
		if ((cur_font->font_id & 0xff) != id_save) {   
			j++;
			id_save = cur_font->font_id & 0xff;
		}
		if ((cur_font->font_id & 0xff) == 1) {
			if (siz_tab[0] > cur_font->max_char_width)
				siz_tab[0] = cur_font->max_char_width;
			if (siz_tab[1] > cur_font->top)
				siz_tab[1] = cur_font->top;
			if (siz_tab[2] < cur_font->max_char_width)
				siz_tab[2] = cur_font->max_char_width; 
			if (siz_tab[3] < cur_font->top)
				siz_tab[3] = cur_font->top;
			i++;
		}
		if (cur_font->next_font == 0)
			break;
	}

	/* If no fonts, set something innocuous for the size table info. */
	if (!siz_tab[2])
		j = -(siz_tab[0] = siz_tab[1] = siz_tab[2] = siz_tab[3] = 1);

	dev_tab[5] = i;
	dev_tab[10] = ++j;

	cur_font = def_save;
	FLIP_Y = 1;

	xmx_clip = dev_tab[0];
	ymx_clip = dev_tab[1];

	text_qc = INTIN[6];
	if (text_qc >= MAX_COLOR || text_qc < 0)
		text_qc = 1; 
	text_color = TEXT_BP = MAP_COL[text_qc];
	clip = xmn_clip = ymn_clip = STYLE = h_align = rq_attr = 
		v_align = CHUP = 0;
	spw_min = 5;
	spw_max = 80;
	
    /* for device fonts */	
	dv_font = dv_ht = -1;
	dv_pt = 10;
	dv_selpt = TRUE;
	dv_rot = dv_attr = 0;
	dv_cur = dv_loaded = FALSE;
	dv_first = dv_mem = (FHEAD *)NULLPTR;
#if cdo_text
	dvt_init();
#endif

}  /* end text_init() */


/**********************************************************************
 *
 **********************************************************************/
VOID	init_lfu()
{
	FHEAD		*fptr;

	for (fptr = &first; fptr != 0; fptr = fptr->next_font) 
		fptr->lfu_low = fptr->lfu_high = 0;

}  /* end init_lfu() */


/************************************************************************
 *  Set GLOBAL cur_font pointer to first font in chain with the current	*
 *  font_id. Returns current font_id.					*
 ************************************************************************/
WORD	set_id()
{
	WORD	font_id;

	font_id = cur_font->font_id & 0xff;
	cur_font = &first;
	while (cur_font && ((cur_font->font_id & 0xff) != font_id))
		cur_font = cur_font->next_font;
	if (!cur_font) {
		cur_font = first.next_font;
		font_id = cur_font->font_id & 0xff;
	}  /* End if:  no font found. */
	return(font_id);

}  /* end set_id() */


/**********************************************************************
 *
 **********************************************************************/
VOID	dqt_extent()
{
	WORD		i, ix, temp;

	if (cur_font->flags & MONOSPACE)
		width = CONTRL[3]*cur_font->max_cell_width;
	else {
		act_save = (FHEAD *)NULLPTR;
		width = 0;	
		for (i = 0; i < CONTRL[3]; i++) {
			if ((temp = INTIN[i]) < cur_font->first_ade ||
					temp > cur_font->last_ade) {
				if (!get_multi(temp))
					temp = 32;
			}  /* End if:  character not in range. */
			chk_fnt();
			ix = temp - cur_font->first_ade;
			width += act_font->off_table[ix + 1] -
				act_font->off_table[ix];
			ix <<= 1;
			if (act_font->flags & HORZ_OFF)
				width -= act_font->hor_table[ix] +
					act_font->hor_table[ix + 1];
			if (act_save)
				res_multi();
		}  /* End for:  over characters. */
	}  /* End else:  proportionally spaced font. */

	if (STYLE & THICKEN)
		width += CONTRL[3]*cur_font->thicken;

	height = cur_font->top + cur_font->bottom + 1;
	CONTRL[2] = 4;

	switch (CHUP) {
		case 0:
			PTSOUT[0] = PTSOUT[1] = PTSOUT[3] = PTSOUT[6] = 0;
			PTSOUT[2] = PTSOUT[4] = width;
			PTSOUT[5] = PTSOUT[7] = height;
			break;
		case 900:
			PTSOUT[0] = PTSOUT[2] = height;
			PTSOUT[3] = PTSOUT[5] = width;
			PTSOUT[1] = PTSOUT[4] = PTSOUT[6] = PTSOUT[7] = 0;
			break;
		case 1800:
			PTSOUT[0] = PTSOUT[6] = width;
			PTSOUT[1] = PTSOUT[3] = height;
			PTSOUT[2] = PTSOUT[4] = PTSOUT[5] = PTSOUT[7] = 0;
			break;
		case 2700:
			PTSOUT[0] = PTSOUT[2] = PTSOUT[3] = PTSOUT[5] = 0;
			PTSOUT[1] = PTSOUT[7] = width;
			PTSOUT[4] = PTSOUT[6] = height;
			break;
	}  /* End switch. */
	FLIP_Y = 1;
	
}  /* end dqt_extent() */


/**********************************************************************
 *
 **********************************************************************/
VOID	dqt_attributes()
{
}  /* end dqt_attributes() */


/**********************************************************************
 *
 **********************************************************************/
VOID	dqt_width()
{
}  /* end dqt_width() */


/**********************************************************************
 *
 **********************************************************************/
VOID	dqt_name()
{
	WORD 	i;
	BYTE 	*name;
	FHEAD	*tmp_font;

	tmp_font = &first;
	for (i = 1; (i < INTIN[0]) && tmp_font; \
					i++, tmp_font = tmp_font->next_font)
		while (tmp_font->font_id == tmp_font->next_font->font_id)
			tmp_font = tmp_font->next_font;
	if (!tmp_font)
		tmp_font = &first;
	INTOUT[0] = tmp_font->font_id;
	
	for (i = 1, name = tmp_font->name; INTOUT[i++] = *name++; )
		;
                
	while (i < 33)
		INTOUT[i++] = 0;
	CONTRL[4] = 33;
	
}  /* end dqt_name() */


/**********************************************************************
 *
 **********************************************************************/
VOID	dqt_fontinfo()
{
}  /* end dqt_fontinfo() */


/**********************************************************************
 *
 **********************************************************************/
VOID	dqt_justified()
{
}  /* end dqt_justified() */


/**********************************************************************
 *
 **********************************************************************/
VOID	just_chk()
{
#if cdo_text
	if (cur_font->flags & DEVICE_FONT)		
		g_devdcnt = TRUE;
	else {
		Bitmap = TRUE;
		if (new_clip)
			chk_clip();
	}  /* End else:  current font is a bitmap */
#else
	Bitmap = TRUE;
	if (new_clip)
		chk_clip();
#endif

}  /* end just_chk() */


/**********************************************************************
 *
 **********************************************************************/
VOID	d_justified()
{
	WORD	first, i, last;

	if (cur_font == (FHEAD *)NULLPTR || cur_font->font_id == -1)
		return;
	if ((CONTRL[3] -= 2) < 1)
		return;
	
	if (cur_font->flags & DEVICE_FONT) {
		if (!g_devdout)
			return;
	}
	else {
		if (g_devdout)
			return;
		if (clip_reject())
			return;
	}

	in_word = INTIN[0];
	in_char = INTIN[1];

	first = cur_font->first_ade;
	last = cur_font->last_ade;

	for (i = 0, spaces = 0; i < CONTRL[3]; i++) {
	    if ((INTIN[i] = INTIN[i+2]) == 32)
			spaces++;
	}  /* End for:  over characters. */

	if (!(cur_font->flags & DEVICE_FONT)) {
		if (CONTRL[3] > 1) {
			dqt_extent();
			CONTRL[2] = 0;
			set_just(width, spaces);
			width = PTSIN[2];
			CONTRL[0] = 11;
		}  /* End if:  more than one character. */

		/* If only one character to print, move it from INTIN[2] to */
		/* INTIN[0].  Pretend that it was really the graphics text  */
		/* primitive which was requested.                           */
		else {
			CONTRL[0] = 8;
			INTIN[0] = INTIN[2];
		}  /* end else:  only one character. */
	} /* End if:  bitmapped font. */
	v_gtext();
	
}  /* end d_justified() */


/**********************************************************************
 *
 **********************************************************************/
VOID	set_just(wid, spaces)
WORD wid, spaces;
{
	WORD		i, delword, delchar;
	WORD		direction, max_sp, min_sp, sp_width;

	if (CONTRL[0] != 11 || CONTRL[3] <= 1) {
		wordx = wordy = rmdword = charx = chary = rmdchar = 0;
		return;
	}  /* End if:  degenerate. */

#if cdo_text
	if (cur_font->flags & DEVICE_FONT)
		PTSIN[2] = dvt_fres(PTSIN[2]);
#endif

	wordx = wordy = rmdwordx = rmdwordy = rmdword =
		charx = chary = rmdcharx = rmdchary = rmdchar = 0;

	if (in_word && spaces) {
		delword = (PTSIN[2] - wid)/spaces;
		rmdword = (PTSIN[2] - wid)%spaces;

		if (rmdword < 0) {
			direction = -1;
			rmdword = 0 - rmdword;
		}
		else
			direction = 1;

		if (in_char) {
			act_save = (FHEAD *)NULLPTR;
			get_multi((WORD)' ');
			chk_fnt();
			i = ' ' - cur_font->first_ade;
			if (cur_font->flags & MONOSPACE)
			    sp_width = cur_font->max_cell_width;
			else
			    sp_width = act_font->off_table[i + 1] -
			act_font->off_table[i];
			max_sp = spw_max*sp_width/10;
			min_sp = -spw_min*sp_width/10;
			if (act_save)
			    res_multi();

			if (delword > max_sp) {
				delword = max_sp;
				rmdword = 0;
			}
			else if (delword < min_sp) {
				delword = min_sp;
				rmdword = 0;
			}
			wid += delword*spaces + rmdword*direction;
		}

		switch (CHUP) {
			case 0:
				wordx = delword;
				wordy = rmdwordy = 0;
				rmdwordx = direction;
				break;
			case 900:
				wordx = rmdwordx = 0;
				wordy = 0 - delword;
				rmdwordy = 0 - direction;
				break;
			case 1800:
				wordx = 0 - delword;
				wordy = rmdwordy = 0;
				rmdwordx = 0 - direction;
				break;
			case 2700:
				wordx = rmdwordx = 0;
				wordy = delword;
				rmdwordy = direction;
				break;
		}  /* End switch. */
	}
	else
		wordx = wordy = rmdword = 0;

	if (in_char) {
		delchar = (PTSIN[2] - wid)/(CONTRL[3] - 1);
		rmdchar = (PTSIN[2] - wid)%(CONTRL[3] - 1);

		if (rmdchar < 0) {
			direction = -1;
			rmdchar = 0 - rmdchar;
		}
		else
			direction = 1;

		switch (CHUP) {
			case 0:
				charx = delchar;
				chary = rmdchary = 0;
				rmdcharx = direction;
				break;
			case 900:
				charx = rmdcharx = 0;
				chary = 0 - delchar;
				rmdchary = 0 - direction;
				break;
			case 1800:
				charx = 0 - delchar;
				chary = rmdchary = 0;
				rmdcharx = 0 - direction;
				break;
			case 2700:
				charx = rmdcharx = 0;
				chary = delchar;
				rmdchary = direction;
				break;
		}  /* End switch. */
	}
	else
		charx = chary = rmdchar = 0;
	
}  /* end set_just() */


/**********************************************************************
 *
 **********************************************************************/
VOID	dt_preqcr()
{
	prev_font = cur_font;
	prev_font++;		/* init font pointer to be not equal to cur */
	PSTYLE = 0x0000;	/* make sure it gets set correctly */
#if cdo_text
	set_undl(PSTYLE);	/* make sure it is turned off */
#endif

}  /* end dt_preqcr() */


/************************************************************************
 *  Load BITMAP fonts.							*
 ************************************************************************/
VOID	dt_loadfont()
{
	WORD	id;
	union {
		WORD	w_ptr[2];
		FHEAD	*f_ptr;
	} fontptr;
	FHEAD		*this_font;
	EXTERN	FHEAD	first;

    /* The scratch text buffer segment is passed in CONTRL[8].  The     */
    /* size of the buffer is passed in CONTRL[9].  Use them to set the  */
    /* text scratch buffer addresses.                                   */
	buff_seg = CONTRL[8];
	txbuf1 = 0;
	txbuf2 = CONTRL[9]/2;

    /* The font segment is passed in CONTRL[7].  Put it into a pointer. */
	fontptr.w_ptr[0] = 0;
	fontptr.w_ptr[1] = CONTRL[7];

    /* Entry point to GDOS font manager passed in CONTRL[5]:CONTRL[10].	*/
	seg_fmgr = CONTRL[5];	
	off_fmgr = CONTRL[10];
	hdr_addr();			/* get address of header manager */

    /* If font's offset:segment are not already linked into chain, do so. */
	for (this_font = &first; (this_font->next_font != 0) &&
		(this_font->next_font != fontptr.f_ptr);
	this_font = this_font->next_font);

	CONTRL[4] = 1;
	INTOUT[0] = 0;

	if (this_font->next_font == 0) {
	    /* Patch the last link to point to the first new font. */
		this_font->next_font = fontptr.f_ptr;
		id = this_font->font_id & 0xff;

	    /* Find out how many distinct font id numbers have just */
	    /* been linked in.                                      */
		while ((this_font = this_font->next_font) != (FHEAD *) 0) {
		    /* Update count of font id numbers, if necessary. */
			if ((this_font->font_id & 0xff) != id) {
				id = this_font->font_id & 0xff;
				INTOUT[0]++;
			}  /* End if:  new font id found. */
		}  /* End while:  over fonts. */
	}  /* End if:  must link in. */

	/* Indicate that fonts are alive. */
	loaded = TRUE;

	/* Perform any required device-specific actions. */
#if cdo_text
	dvt_lf();
#endif

}  /* end dt_loadfont() */


/**********************************************************************
 *  Why isn't the memory freed?
 **********************************************************************/
VOID	dt_unloadfont()
{
	first.next_font = (FHEAD *)NULLPTR;
	loaded = FALSE;
	
}  /* end dt_unloadfont() */


/**********************************************************************
 *
 **********************************************************************/
VOID	save_text_defaults()
{
	d_cur_font = cur_font;
	d_text_color = text_color;
	d_text_qc = text_qc;
	d_h_align = h_align;
	d_v_align = v_align;
	d_chup = CHUP;
	d_STYLE = STYLE;
	d_rq_attr = rq_attr;
	d_WEIGHT = WEIGHT;
	d_LITEMASK = LITEMASK;
	d_L_OFF = L_OFF;
	d_R_OFF = R_OFF;
	d_SKEWMASK = SKEWMASK;
	d_dv_font = dv_font;
	d_dv_pt = dv_pt;
	d_dv_ht = dv_ht;
	d_dv_rot = dv_rot;
	d_dv_attr = dv_attr;
	d_dv_selpt = dv_selpt;
	d_dv_cur = dv_cur;
	
}  /* end save_text_defaults() */


/**********************************************************************
 *
 **********************************************************************/
VOID	reset_text_defaults()
{
	cur_font = d_cur_font;
	text_color = TEXT_BP = d_text_color;
	text_qc = d_text_qc;
	h_align = d_h_align;
	v_align = d_v_align;
	CHUP = d_chup;
	rq_attr = d_rq_attr;
	STYLE = d_STYLE;
	WEIGHT = d_WEIGHT;
	LITEMASK = d_LITEMASK;
	L_OFF = d_L_OFF;
	R_OFF = d_R_OFF;
	SKEWMASK = d_SKEWMASK;
	dv_font = d_dv_font;
	dv_pt = d_dv_pt;
	dv_ht = d_dv_ht;
	dv_rot = d_dv_rot;
	dv_attr = d_dv_attr;
	dv_selpt = d_dv_selpt;
	dv_cur = d_dv_cur;
	
}  /* end reset_text_defaults() */


/**********************************************************************
 *
 **********************************************************************/
BOOLEAN	clip_reject()
{
	WORD	retval;

	retval = TRUE;
	if (CHUP != 0 && CHUP != 180)
		retval = FALSE;
	tcell_ht = cur_font->form_height;

	switch (v_align) {
		case 0:
			tdelv = cur_font->top;
			break;
		case 1:
			tdelv = cur_font->top - cur_font->half;
			break;
		case 2:
			tdelv = cur_font->top - cur_font->ascent;
			break;
		case 3:
			tdelv = cur_font->top + cur_font->bottom;
			break;
		case 4:
			tdelv = cur_font->top + cur_font->descent;
			break;
		case 5:
			tdelv = 0;
			break;
	}
	DESTY = CHUP == 0 ? PTSIN[1] - tdelv :
		PTSIN[1] - ((tcell_ht - 1) - tdelv);
	return(retval && (DESTY > ymx_clip || DESTY + tcell_ht < ymn_clip));
	
} /* end clip_reject() */


/************************************************************************
 *   This will never be called when cur_font is of type DEVICE.		*
 ************************************************************************/
VOID	bound_rect()
{
	WORD	ht, wd;

	/* The bounding rectangle will be a function of the current */
	/* character up and text alignment.                         */
	switch (CHUP)
	{
	    case 0:
		horz_alig(&wd, &ht);
		PTSOUT[0] = PTSIN[0];
		switch (h_align)
		{
		    case 1:  /* center */
			PTSOUT[0] -= wd/2;
			break;
		    case 2:  /* right */
			PTSOUT[0] -= wd;
			break;
		}  /* End switch:  on horizontal alignment. */
		PTSOUT[2] = PTSOUT[0] + wd;
		PTSOUT[1] = PTSIN[1];
		switch (v_align)
		{
		    case 0:  /* baseline */
			PTSOUT[1] += cur_font->bottom;
			break;
		    case 1:  /* half line */
			PTSOUT[1] += cur_font->half + cur_font->bottom + 1;
			break;
		    case 2:  /* ascent line */
			PTSOUT[1] += cur_font->ascent + cur_font->bottom + 1;
			break;
		    case 4:  /* descent line */
			PTSOUT[1] += cur_font->bottom - cur_font->descent;
			break;
		    case 5:  /* top line */
			PTSOUT[1] += ht;
			break;
		}  /* End switch:  on vertical alignment. */
		PTSOUT[3] = PTSOUT[1] - ht;
		break;

	    case 900:
		vert_alig(&wd, &ht);
		PTSOUT[1] = PTSIN[1];
		switch (h_align)
		{
		    case 1:  /* center */
			PTSOUT[1] += wd/2;
			break;
		    case 2:  /* right */
			PTSOUT[1] += wd;
			break;
		}  /* End switch:  on horizontal alignment. */
		PTSOUT[3] = PTSOUT[1] - wd;
		PTSOUT[0] = PTSIN[0];
		switch (v_align)
		{
		    case 0:  /* baseline */
			PTSOUT[0] -= cur_font->top + 1;
			break;
		    case 1:  /* half line */
			PTSOUT[0] -= cur_font->top - cur_font->half;
			break;
		    case 2:  /* ascent line */
			PTSOUT[0] -= cur_font->top - cur_font->ascent;
			break;
		    case 3:  /* bottom line */
			PTSOUT[0] -= ht;
			break;
		    case 4:  /* descent line */
			PTSOUT[0] -= cur_font->top + cur_font->descent + 1;
			break;
		}  /* End switch:  on vertical alignment. */
		PTSOUT[2] = PTSOUT[0] + ht;
		break;

	    case 1800:
		horz_alig(&wd, &ht);
		PTSOUT[0] = PTSIN[0];
		switch (h_align)
		{
		    case 0:  /* left */
			PTSOUT[0] -= wd;
			break;
		    case 1:  /* center */
			PTSOUT[0] -= wd/2;
			break;
		}  /* End switch:  on horizontal alignment. */
		PTSOUT[2] = PTSOUT[0] + wd;
		PTSOUT[1] = PTSIN[1];
		switch (v_align)
		{
		    case 0:  /* baseline */
			PTSOUT[1] += cur_font->top;
			break;
		    case 1:  /* half line */
			PTSOUT[1] += cur_font->top - cur_font->half;
			break;
		    case 2:  /* ascent line */
			PTSOUT[1] += cur_font->top - cur_font->ascent;
			break;
		    case 3:  /* bottom line */
			PTSOUT[1] += ht;
			break;
		    case 4:  /* descent line */
			PTSOUT[1] += cur_font->top + cur_font->descent + 1;
			break;
		}  /* End switch:  on vertical alignment. */
		PTSOUT[3] = PTSOUT[1] - ht;
		break;

	    case 2700:
		vert_alig(&wd, &ht);
		PTSOUT[1] = PTSIN[1];
		switch (h_align)
		{
		    case 0:  /* left */
			PTSOUT[1] += wd;
			break;
		    case 1:  /* center */
			PTSOUT[1] += wd/2;
			break;
		}  /* End switch:  on horizontal alignment. */
		PTSOUT[3] = PTSOUT[1] - wd;
		PTSOUT[0] = PTSIN[0];
		switch (v_align)
		{
		    case 0:  /* baseline */
			PTSOUT[0] -= cur_font->bottom;
			break;
		    case 1:  /* half line */
			PTSOUT[0] -= cur_font->bottom + cur_font->half + 1;
			break;
		    case 2:  /* ascent line */
			PTSOUT[0] -= cur_font->bottom + cur_font->ascent + 1;
			break;
		    case 4:  /* descent line */
			PTSOUT[0] -= cur_font->bottom - cur_font->descent;
			break;
		    case 5:  /* top line */
			PTSOUT[0] -= ht;
			break;
		}  /* End switch:  on vertical alignment. */
		PTSOUT[2] = PTSOUT[0] + ht;
		break;
	}  /* End switch:  on angle. */
	
}  /* end bound_rect() */


/**********************************************************************
 *
 **********************************************************************/
VOID	horz_alig(wd, ht)
WORD *wd, *ht;
{
	*wd = PTSOUT[4] - PTSOUT[0];
	if (*wd < 0)
	    *wd = -(*wd);
	*ht = PTSOUT[5] - PTSOUT[1];
	if (*ht < 0)
	    *ht = -(*ht);
    
}  /* end horz_alig() */


/**********************************************************************
 *
 **********************************************************************/
VOID	vert_alig(wd, ht)
WORD *wd, *ht;
{
	*wd = PTSOUT[5] - PTSOUT[1];
	if (*wd < 0)
	    *wd = -(*wd);
	*ht = PTSOUT[4] - PTSOUT[0];
	if (*ht < 0)
	    *ht = -(*ht);

}  /* end vert_alig() */
