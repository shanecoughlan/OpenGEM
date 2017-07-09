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



#include "ppdgem.h"
#include "edicon.h"
#include "ediconf.h"
#include "iconfile.h"
#include "editor.h"
#include "farmem.h"
#include "clipbd.h"

#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>


PPDUBLK edit_control;

#define COL_CBLACK (edit_colour[0])
#define COL_CWHITE (edit_colour[1])
#define COL_TWHITE (edit_colour[2])
#define COL_TBLACK (edit_colour[3])

#define FILL_CBLACK (edit_shade[0] + 1)
#define FILL_CWHITE (edit_shade[1] + 1)
#define FILL_TWHITE (edit_shade[2] + 1)
#define FILL_TBLACK (edit_shade[3] + 1)

#define WDW_STYLE  (NAME | MOVER | CLOSER | FULLER | SIZER)

static WORD xcascade, ycascade;

/*
 * Extended type 1 => colour setting
 * Extended type 2 => move caption
 * Extended type 3 => move letter
 * Extended type 4 => icon fiddle
 * Extended type 5 => the edit panel itself
 */

//static LPTREE	tree;
static WORD	xdial, ydial, wdial, hdial, exitobj;

static WORD clrs[] = {CLR1,   CLR2,   CLR3,   CLR4};

extern UWORD x_mx, x_my, x_mb;
extern WORD x_form_do(REG LPTREE tree, WORD start_fld, LPOBJ pOb, EDITOR_DOC *pEd);

VOID redraw_preview(EDITOR_DOC *pEd);


VOID show_drawmode(EDITOR_DOC *pEd)
{
	int n;

	for (n = 0; n < 4; n++)
	{
		if (pEd->drawmode == n) trEdit[clrs[n]].ob_state |=  CHECKED;
		else	  		   		trEdit[clrs[n]].ob_state &= ~CHECKED;
		trEdit[clrs[n]].ob_state &= ~SELECTED;
	}
}


VOID icon_fg(EDITOR_DOC *pEd, GRECT *g)
{
	UWORD clr = colour_popup(vdi_handle, g->g_x, g->g_y+g->g_h);

	if (clr > 0)
	{
		clr = ((clr - 1) << 12) & 0xFF00;
		pEd->icn.ib_char = (pEd->icn.ib_char & 0xFFF) | clr;
	}
	redraw_preview(pEd);
}

VOID icon_bg(EDITOR_DOC *pEd, GRECT *g)
{
	UWORD clr = colour_popup(vdi_handle, g->g_x, g->g_y+g->g_h);

	if (clr > 0)
	{
		clr = ((clr - 1) << 8) & 0xF00;
		pEd->icn.ib_char = (pEd->icn.ib_char & (~0xF00)) | clr;
	}
	redraw_preview(pEd);
}





VOID set_colour(EDITOR_DOC *pEd, int exitobj)
{
	int n;

	for (n = 0; n < 4; n++) if (clrs[n] == exitobj) pEd->drawmode = n;
	show_drawmode(pEd);
	for (n = 0; n < 4; n++) objc_draw(trEdit, clrs[n], 1, trEdit[0].ob_x, trEdit[0].ob_y, 
		                                  trEdit[0].ob_width, trEdit[0].ob_height); 
}


VOID redraw_preview(EDITOR_DOC *pEd)
{
	/* The preview window uses the device-dependent form of the bitmap;
	 * so generate it from the device-independent form */
	 
	WORD wb     = pEd->icn.ib_wicon >> 3;
	WORD hl     = pEd->icn.ib_hicon;
	WORD fg     = (pEd->icn.ib_char >> 12) & 0x0F;
	WORD bg     = (pEd->icn.ib_char >> 8)  & 0x0F;
	LPLONG os;
	GRECT rect;
	
	os = (LPLONG)&trEdit[FGBTN].ob_spec;
	*os = (*os & (~0x0F)) | fg;
	os = (LPLONG)&trEdit[BGBTN].ob_spec;
	*os = (*os & (~0x0F)) | bg;
	
	vdi_trans(pEd->dibmask, wb, pEd->icn.ib_pmask, wb, hl); /* transform mask	  */
	vdi_trans(pEd->dibdata, wb, pEd->icn.ib_pdata, wb, hl); /* transform bitmap */	 

/* Doesn't work well if the window isn't on top.


	objc_draw(trEdit, PREVIEWB,  2, trEdit[0].ob_x, trEdit[0].ob_y, 
	          trEdit[0].ob_width, trEdit[0].ob_height);
	objc_draw(trEdit, FGBTN,  2, trEdit[0].ob_x, trEdit[0].ob_y, 
	          trEdit[0].ob_width, trEdit[0].ob_height);
	objc_draw(trEdit, BGBTN,  2, trEdit[0].ob_x, trEdit[0].ob_y, 
	          trEdit[0].ob_width, trEdit[0].ob_height);
*/
	objc_offset(trEdit, PREVIEWB, &rect.g_x, &rect.g_y);
	rect.g_w = trEdit[PREVIEWB].ob_width;
	rect.g_h = trEdit[PREVIEWB].ob_height;

	send_redraw(pEd->hwnd, &rect);

	objc_offset(trEdit, FGBTN, &rect.g_x, &rect.g_y);
	rect.g_w = trEdit[FGBTN].ob_width * 2;
	rect.g_h = trEdit[FGBTN].ob_height;

	send_redraw(pEd->hwnd, &rect);

	
}





static VOID get_ib_point(ICONBLK far *ib, WORD y, WORD x, BYTE *bdata, BYTE *bmask)
{
	WORD wb, hl, off, mask;
	
	wb     = ib->ib_wicon >> 3;
	hl     = ib->ib_hicon;
	off    = (wb * y + ((x >> 3)^1)); 
	mask   = 0x80 >> (x & 7);

	*bdata = ib->ib_pdata[off] & mask;
	*bmask = ib->ib_pmask[off] & mask;
}


static VOID get_point(EDITOR_DOC *pEd, WORD y, WORD x, BYTE *bdata, BYTE *bmask)
{
	WORD wb, hl, off, mask;
	
	wb     = pEd->icn.ib_wicon >> 3;
	hl     = pEd->icn.ib_hicon;
	off    = (wb * y + ((x >> 3)^1)); 
	mask   = 0x80 >> (x & 7);

	*bdata = pEd->dibdata[off] & mask;
	*bmask = pEd->dibmask[off] & mask;
}


static VOID set_point(EDITOR_DOC *pEd, WORD y, WORD x, BYTE bdata, BYTE bmask)
{
	WORD wb, hl, off, mask;
	
	wb     = pEd->icn.ib_wicon >> 3;
	hl     = pEd->icn.ib_hicon;
	off    = (wb * y + ((x >> 3)^1)); 
	mask   = 0x80 >> (x & 7);

	if (bdata) pEd->dibdata[off] |= mask; else pEd->dibdata[off] &= ~mask;
	if (bmask) pEd->dibmask[off] |= mask; else pEd->dibmask[off] &= ~mask;
}



VOID reflect_h(EDITOR_DOC *pEd, WORD hl, WORD wm)
{
	WORD n, nl;
	BYTE db, mb, db2, mb2;
		
	for (nl = 0; nl < hl; nl++)
	{
		WORD wm = (pEd->icn.ib_wicon / 2);
		
		for (n = 0; n < wm; n++)
		{
			get_point(pEd, nl,                         n, &db,  &mb );
			get_point(pEd, nl, pEd->icn.ib_wicon - n - 1, &db2, &mb2);
			set_point(pEd, nl, pEd->icn.ib_wicon - n - 1,  db,   mb );
			set_point(pEd, nl,                         n,  db2,  mb2);
		}
	}

}

VOID reflect_v(EDITOR_DOC *pEd, WORD hl, WORD wb)
{
	WORD n, nl, nb, lb;
	BYTE db, mb;
			
	for (nl = 0; nl < (hl / 2); nl++)
	{
		for (n = 0; n < wb; n++)
		{
			nb =  nl       * wb + n;
			lb = (hl-nl-1) * wb + n;
			db = pEd->dibdata[nb]; 
			mb = pEd->dibmask[nb];
			pEd->dibdata[nb] = pEd->dibdata[lb];
			pEd->dibmask[nb] = pEd->dibmask[lb];
			pEd->dibdata[lb] = db;
			pEd->dibmask[lb] = mb;
			
		}
	}
}


VOID redraw_cell(EDITOR_DOC *pEd, LPICON pIcon, WORD full, WORD cx, WORD cy, LPOBJ pObj)
{
	WORD xs, ys, x, y;
	GRECT rBox;
	WORD attrs[5];
	WORD pts[4];
	WORD xmin, xmax, ymin, ymax;
	
	/* All changes are made to the device-independent bitmap - work from
	 * it. */
	
	xs = pObj->ob_width  / pIcon->ib_wicon;
	ys = pObj->ob_height / pIcon->ib_hicon;

	/* Get the original fill settings */
	vqf_attributes(vdi_handle, attrs);
	
	if (full)
	{
		xmin = 0;
		ymin = 0;
		xmax = pIcon->ib_wicon;
		ymax = pIcon->ib_hicon;
	}
	else
	{
		xmin = cx; ymin = cy; xmax = cx+1; ymax = cy+1;
	}
	for (x = xmin; x < xmax; x++) for (y = ymin; y < ymax; y++)
	{
		BYTE b,m;

		rBox.g_x = pObj->ob_x + (x*xs);
		rBox.g_y = pObj->ob_y + (y*ys);
		rBox.g_w = rBox.g_x + xs;
		rBox.g_h = rBox.g_y + ys;

		
		get_point(pEd, y, x, &b, &m);	//b = dibdata[wb*y + ((x>>3)^1)];
									//m = dibmask[wb*y + ((x>>3)^1)];

		//bit = (0x100) >> ((x & 7) + 1); 

		//b &= bit; m &= bit;

		vsf_interior(vdi_handle, 2);
		vsf_perimeter(vdi_handle, 0);
		vswr_mode(vdi_handle, MD_REPLACE);
		if (b && !m) 
		{	
			vsf_color(vdi_handle, COL_TBLACK);
			vsf_style(vdi_handle, FILL_TBLACK);
		}
		else if (b && m)
		{	
			vsf_color(vdi_handle, COL_CBLACK);
			vsf_style(vdi_handle, FILL_CBLACK);
		}
		else if (!b && m)
		{	
			vsf_color(vdi_handle, COL_CWHITE);
			vsf_style(vdi_handle, FILL_CWHITE);
		}
		else
		{	
			vsf_color(vdi_handle, COL_TWHITE);
			vsf_style(vdi_handle, FILL_TWHITE);
		}
		
		v_bar(vdi_handle, &rBox.g_x);
	}
	for (x = 0; x <= pIcon->ib_wicon; x++) 
	{
		pts[0] = x * xs + pObj->ob_x;
		pts[1] = pObj->ob_y;
		pts[2] = pts[0];
		pts[3] = pObj->ob_y + ys * ymax; // + pObj->ob_height;
		v_pline(vdi_handle, 2, pts);	
	}
	for (y = 0; y <= pIcon->ib_hicon; y++)
	{
		pts[0] = pObj->ob_x;
		pts[1] = y * ys + pObj->ob_y;
		pts[2] = pObj->ob_x + xs * xmax; // + pObj->ob_width;
		pts[3] = pts[1];
		v_pline(vdi_handle, 2, pts);	
	}
	vsf_interior(vdi_handle, attrs[0]);
	vsf_style(vdi_handle, attrs[1]);
	vsf_color(vdi_handle, attrs[2]);
	vsf_perimeter(vdi_handle, attrs[4]);
	return;
}









VOID frob_bitmap(EDITOR_DOC *pEd, LPOBJ pOb)
{
	WORD xs, ys, x, y;
	OBJECT ob;
	fmemcpy(&ob, pOb, sizeof(OBJECT));
	
 	xs = pOb->ob_width  / pEd->icn.ib_wicon;
	ys = pOb->ob_height / pEd->icn.ib_hicon;

	ob.ob_x = pOb->ob_x + trEdit[0].ob_x;
	ob.ob_y = pOb->ob_y + trEdit[0].ob_y;
	
	x = (x_mx - ob.ob_x) / xs;
	y = (x_my - ob.ob_y) / ys;

	if (x >= pEd->icn.ib_wicon || y >= pEd->icn.ib_hicon) return;
	
	switch(pEd->drawmode)
	{
		case 0:	/* Ink */
		set_point(pEd, y,x,1,1);
		break;

		case 1: /* Paper */
		set_point(pEd, y,x,0,1);
		break;

		case 2: /* Transparent */
		set_point(pEd, y,x,0,0);
		break;

		case 3: /* Invert */
		set_point(pEd, y,x,1,0);
		break;
	}
	graf_mouse(M_OFF, 0);
	/* Redraw the cell we changed */
	redraw_cell(pEd, &pEd->icn, 0, x, y, &ob);
	/* Redraw the preview object */
	redraw_preview(pEd);
	graf_mouse(M_ON, 0);
}
 

/* Draw the bitmap image */
 

WORD draw_ectl(LPPARM pb)
{
	LPICON pIcon;
	LPTREE tr;
	OBJECT ob;
	EDITOR_DOC *pEd;

	pEd   = (EDITOR_DOC *)(long)(pb->pb_parm);
	pIcon = &(pEd->icn);
	
	tr    = (LPTREE)pb->pb_tree;

	fmemcpy(&ob, &tr[pb->pb_obj], sizeof(OBJECT));

	ob.ob_x = ob.ob_x + tr[0].ob_x;
	ob.ob_y = ob.ob_y + tr[0].ob_y;
	
	redraw_cell(pEd, pIcon, 1, pb->pb_x, pb->pb_y, &ob);
	
	return 0;
}





void doc_edit(EDICON_DOC *pDoc, WORD ni, GRECT *box)
{
	WORD	hedit;
	EDITOR_DOC *pEd;
	LPICON  pIcon = &(pDoc->icon_data[ni]);
	WORD	wb     = (pIcon->ib_wicon) >> 3;
	WORD	hl     = (pIcon->ib_hicon);

	pEd = NULL;	/* Not set at this point */

	/* Make a local copy of the icon, which is currently in device-dependent 
	 * format for display. We keep one copy in device-independent format
	 * and one copy in device-dependent format */
	 
	hedit = create_editor(pDoc, ni, box);

	
	if (hedit < 0) return;
	
	pEd = &editor_doc[hedit];

// should be done in create_editor()	fmemcpy(&pEd->icn, pIcon, sizeof(ICONBLK));
	
	
	/* Put the "dib*" copy into device-independent format */
	
	wb     = (pIcon->ib_wicon) >> 3;
	hl     = (pIcon->ib_hicon);
	
	vdi_untrans(pEd->bmaddr, wb, pEd->dibmask, wb, hl); /* transform mask	  */
	vdi_untrans(pEd->bdaddr, wb, pEd->dibdata, wb, hl); /* transform bitmap */	 
	
	pEd->icn.ib_pmask = pEd->bmaddr;
	pEd->icn.ib_pdata = pEd->bmaddr + wb*hl;	
	pEd->icn.ib_ptext = ADDR("Caption");

	open_editwnd(hedit);
}



/***********************************************************************/
/***********************************************************************/

WORD create_editor(EDICON_DOC *pDoc, WORD nIcon, GRECT *box)
{
	WORD nw, fg, bg;
	EDITOR_DOC *pEd;
	LPICON  pIcon = &(pDoc->icon_data[nIcon]);
	WORD	wb     = (pIcon->ib_wicon) >> 3;
	WORD	hl     = (pIcon->ib_hicon);
	LPLONG os;


	for (nw = 0; nw < MAXW; nw++)
	{
		if (!(editor_doc[nw].exists)) break;
	}
	if (nw >= MAXW) return -1;	// No document available

	pEd = &editor_doc[nw];
	
	fmemcpy(&(pEd->icn), pIcon, sizeof(ICONBLK));

	/* Make a local copy of the icon, which is currently in device-dependent 
	 * format for display. We keep one copy in device-independent format
	 * and one copy in device-dependent format */
	 
	pEd->bmaddr  = local_alloc(wb*hl*4); if (!pEd->bmaddr) return -1;
	pEd->bdaddr  = pEd->bmaddr +   (wb*hl);
	pEd->dibmask = pEd->bmaddr + 2*(wb*hl);
	pEd->dibdata = pEd->bmaddr + 3*(wb*hl);
	
	fmemcpy(pEd->bmaddr, pIcon->ib_pmask, wb*hl);
	fmemcpy(pEd->bdaddr, pIcon->ib_pdata, wb*hl);

	/* Put the "dib*" copy into device-independent format */
	
	vdi_untrans(pEd->bmaddr, wb, pEd->dibmask, wb, hl); /* transform mask	  */
	vdi_untrans(pEd->bdaddr, wb, pEd->dibdata, wb, hl); /* transform bitmap */	 
	
	pEd->icn.ib_pmask = pEd->bmaddr;
	pEd->icn.ib_pdata = pEd->bmaddr + wb*hl;	
	pEd->icn.ib_ptext = ADDR("Caption");

	pEd->pDoc   = pDoc;
	pEd->nicon  = nIcon;

	memcpy(&pEd->ZoomBox, box, sizeof(GRECT));
	fstrncpy(pEd->desc, pIcon->ib_ptext, 29); 
	pEd->desc[29] = 0;
	sprintf(pEd->title, "[%s] Icon %d", pDoc->title, nIcon + 1);
	fmemcpy(&pEd->tedi,  trEdit[EDITNAME].ob_spec, sizeof(TEDINFO));

	fg     = (pEd->icn.ib_char >> 12) & 0x0F;
	bg     = (pEd->icn.ib_char >> 8)  & 0x0F;
	os = (LPLONG)&trEdit[FGBTN].ob_spec;
	*os = (*os & (~0x0F)) | fg;
	os = (LPLONG)&trEdit[BGBTN].ob_spec;
	*os = (*os & (~0x0F)) | bg;


	
	pEd->tedi.te_ptext = ADDR(pEd->desc);
	pEd->brush_spec[0] = trEdit[CLR1].ob_spec;
	pEd->brush_spec[1] = trEdit[CLR2].ob_spec;
	pEd->brush_spec[2] = trEdit[CLR3].ob_spec;
	pEd->brush_spec[3] = trEdit[CLR4].ob_spec;

	pEd->exists = 1;
	
	return nw;
}

static VOID save_icon(WORD nw)
{
	EDITOR_DOC *pEd   = &editor_doc[nw];
	EDICON_DOC *pDoc  = pEd->pDoc;
	int nIcon         = pEd->nicon;
	LPICON     pIcon  = &(pDoc->icon_data[nIcon]);
	WORD	wb     = (pIcon->ib_wicon) >> 3;
	WORD	hl     = (pIcon->ib_hicon);

	fmemcpy(pIcon->ib_pmask, pEd->bmaddr, wb*hl);
	fmemcpy(pIcon->ib_pdata, pEd->bdaddr, wb*hl);

	pIcon->ib_xchar = pEd->icn.ib_xchar;	
	pIcon->ib_ychar = pEd->icn.ib_ychar;	
	pIcon->ib_xtext = pEd->icn.ib_xtext;	
	pIcon->ib_ytext = pEd->icn.ib_ytext;	
	pIcon->ib_char  = (pIcon->ib_char & 0xFF) | (pEd->icn.ib_char & (~0xFF));	

	set_title(pDoc, nIcon, pEd->desc);
	
	pDoc->dirty_flag = 1;	
}


VOID destroy_editor(WORD nw)
{
	if (!editor_doc[nw].exists) return;
	
	if (editor_doc[nw].hwnd != -1) close_editwnd(nw, NULL);

	if (editor_doc[nw].bmaddr)     local_free(editor_doc[nw].bmaddr);
	
	editor_doc[nw].bmaddr = NULL;
	editor_doc[nw].exists = 0;
	editor_doc[nw].hwnd   = -1;
}



VOID close_editwnd(WORD nw, GRECT *dummy)
{
	EDITOR_DOC *pEd  = &editor_doc[nw];
	WORD hwnd        = pEd->hwnd;	

	save_icon(nw);	
	measure_docwnd(nw);
	
	graf_shrinkbox(pEd->ZoomBox.g_x, pEd->ZoomBox.g_y,
	               pEd->ZoomBox.g_w, pEd->ZoomBox.g_h,
	               pEd->wx, pEd->wy, pEd->ww, pEd->wh);
	wind_close(hwnd);
	wind_delete(hwnd);

	pEd->hwnd = -1;
	if (topedt == nw) topedt = -1;

	destroy_editor(nw);
	calc_topwnd();
	do_enabling();
}




VOID select_editwnd(WORD nw)
{
	EDITOR_DOC *pEd = &editor_doc[nw];
	
	trEdit[PREVIEW] .ob_spec = &(pEd->icn);
	trEdit[EDITNAME].ob_spec = &(pEd->tedi);
	trEdit[CLR1]    .ob_spec = pEd->brush_spec[0];
	trEdit[CLR2]    .ob_spec = pEd->brush_spec[1];
	trEdit[CLR3]    .ob_spec = pEd->brush_spec[2];
	trEdit[CLR4]    .ob_spec = pEd->brush_spec[3];
	ppd_userdef(trEdit, GRIDBOX, &edit_control);
	edit_control.ub_parm = (LONG)(pEd);
	edit_control.ub_code = draw_ectl;

//	ppd_userdef(tree, EDITOR, &edit_control);

	show_drawmode(pEd);
	measure_editwnd(nw);
}


VOID measure_editwnd(WORD nw)
{
	register EDITOR_DOC *pEd = &editor_doc[nw];
	
	wind_get(pEd->hwnd, WF_WXYWH, &(pEd->wx), &(pEd->wy), 
	                              &(pEd->ww), &(pEd->wh));


	trEdit->ob_x      = pEd->wx;	/* Position the AES object tree */
	trEdit->ob_y      = pEd->wy;	/* within the window */
	trEdit->ob_width  = pEd->ww;
	trEdit->ob_height = pEd->wh;

	trEdit[GRIDBOX].ob_width   = pEd->ww - trEdit[CONTROLS].ob_width;
	trEdit[GRIDBOX].ob_height  = pEd->wh;
	trEdit[CONTROLS].ob_x      = trEdit[GRIDBOX].ob_x + trEdit[GRIDBOX].ob_width;
}



WORD open_editwnd(WORD nw)
{
	WORD hwnd, half_w, half_h;
	GRECT *pBox = &editor_doc[nw].ZoomBox;
	GRECT rect;
	WORD lx, ly, lw, lh;
	
	rect.g_x = xcascade + gl_xfull;
	rect.g_y = ycascade + gl_yfull;
	rect.g_w = gl_wfull / 2;
	rect.g_h = gl_hfull / 2;

	half_w = gl_wfull / 2;
	half_h = gl_hfull / 2;
	
	/* create window */
	editor_doc[nw].hwnd = hwnd = wind_create(WDW_STYLE, gl_xfull, gl_yfull,
				gl_wfull, gl_hfull);
	if (hwnd == -1)
	{ 
		/* No Window Available		   */
		form_alert(1, string_addr(NOWING));
		return(-1);
	}
	wind_setl(hwnd, WF_NAME, &editor_doc[nw].title[0]);
	
	rect.g_w = max(trEdit[CONTROLS].ob_height +
	               trEdit[CONTROLS].ob_width,    rect.g_w);
	rect.g_h = max(trEdit[CONTROLS].ob_height,   rect.g_h);

	rect.g_x = align_x(rect.g_x);
	rect.g_w = align_x(rect.g_w);
	wind_calc(0, WDW_STYLE, rect.g_x, rect.g_y, rect.g_w, rect.g_h,
				 &lx, &ly, &lw, &lh);

	ly = ycascade + gl_yfull;
	
	graf_growbox(pBox->g_x, pBox->g_y, pBox->g_w, pBox->g_h,
	             lx, ly, lw, lh);
	wind_open(hwnd, lx, ly, lw, lh);


	topedt = nw;
	topdoc = -1;
	
	measure_editwnd(nw);
	
	xcascade += 32;
	ycascade += 32;

	if (xcascade >= half_w || ycascade >= half_h)
	{
		xcascade = ycascade = 0;
	}
	do_enabling();
	return 0;
}



VOID paint_editwnd       (WORD nw,  GRECT *area)
{
	GRECT		box;
	WORD 	    wh   = editor_doc[nw].hwnd;
	WORD	xy[4];
	LPLONG	lp;
	
	select_editwnd(nw);

	
	lp     = (LPLONG)(&trEdit[CLR1].ob_spec);
	(*lp) &= ~0x7F; (*lp) |= (COL_CBLACK | edit_shade[0] << 4);
	editor_doc[nw].brush_spec[0] = (LPVOID)(*lp);	
	lp     = (LPLONG)(&trEdit[CLR2].ob_spec);
	(*lp) &= ~0x7F; (*lp) |= (COL_CWHITE | edit_shade[1] << 4);
	editor_doc[nw].brush_spec[1] = (LPVOID)(*lp);	
	lp     = (LPLONG)(&trEdit[CLR3].ob_spec);
	(*lp) &= ~0x7F; (*lp) |= (COL_TWHITE | edit_shade[2] << 4);
	editor_doc[nw].brush_spec[2] = (LPVOID)(*lp);	
	lp     = (LPLONG)(&trEdit[CLR4].ob_spec);
	(*lp) &= ~0x7F; (*lp) |= (COL_TBLACK | edit_shade[3] << 4);
	editor_doc[nw].brush_spec[3] = (LPVOID)(*lp);	
	
	
	wind_calc(1, WDW_STYLE, area->g_x, area->g_y, area->g_w, area->g_h,
				&box.g_x, &box.g_y, &box.g_w, &box.g_h);
				
	wind_get(wh, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	while ( box.g_w && box.g_h ) 
	{   // AES returns zero width and height when no more rectangles 
	    if (rc_intersect(area, &box))
	    {  
	    	xy[0] = box.g_x;
	    	xy[1] = box.g_y;
	    	xy[2] = box.g_x + box.g_w;
	    	xy[3] = box.g_y + box.g_h;
			vs_clip(vdi_handle, 1, xy);
			objc_draw(trEdit, ROOT, MAX_DEPTH, box.g_x, box.g_y, 
					   box.g_w, box.g_h);
					   
		}
	    // get next rectangle in window's rectangle list		
		wind_get(wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	}
	vs_clip(vdi_handle, 0, xy);
}


VOID topped_editwnd		 (WORD nw,  GRECT *dummy) 
{
	wind_setl(editor_doc[nw].hwnd, WF_TOP, 0);
	topedt = nw;
	topdoc = -1;
}

	
VOID newtop_editwnd		 (WORD nw,  GRECT *dummy)
{
	if (topedt == nw) topedt = -1;
}
	

VOID resize_editwnd      (WORD nw,  GRECT *rect)
{
	GRECT box;
	
	select_editwnd(nw);

	rc_constrain(&desk_area, rect);
	wind_calc(1, WDW_STYLE, rect->g_x, rect->g_y, rect->g_w, rect->g_h,
				&box.g_x, &box.g_y, &box.g_w, &box.g_h);

	box.g_w = max(trEdit[CONTROLS].ob_height +
	              trEdit[CONTROLS].ob_width,    box.g_w);
	box.g_h = max(trEdit[CONTROLS].ob_height,   box.g_h);

	box.g_x = align_x(box.g_x);
	box.g_w = align_x(box.g_w);
	wind_calc(0, WDW_STYLE, box.g_x, box.g_y, box.g_w, box.g_h,
				 &rect->g_x, &rect->g_y, &rect->g_w, &rect->g_h);

	wind_set(editor_doc[nw].hwnd, WF_CXYWH, rect->g_x,
			rect->g_y, rect->g_w, rect->g_h);

	measure_editwnd(nw);

	send_redraw(editor_doc[nw].hwnd, rect);	
}



VOID move_editwnd		 (WORD nw,  GRECT *area)
{
	GRECT box;

	select_editwnd(nw);

	rc_constrain(&desk_area, area);
	
	wind_calc(1, WDW_STYLE, area->g_x, area->g_y, area->g_w, area->g_h,
				&box.g_x, &box.g_y, &box.g_w, &box.g_h);

	box.g_x = align_x(box.g_x);
	box.g_w = align_x(box.g_w);
	wind_calc(0, WDW_STYLE, box.g_x, box.g_y, box.g_w, box.g_h,
				 &area->g_x, &area->g_y, &area->g_w, &area->g_h);
	
	wind_set(editor_doc[nw].hwnd, WF_CXYWH, area->g_x,
			area->g_y, area->g_w, area->g_h);
	measure_editwnd(nw);
}



VOID full_editwnd		 (WORD nw,  GRECT *rect)
{
	GRECT area;
	
	hndl_full(editor_doc[nw].hwnd);
	measure_editwnd(nw);

	area.g_x = editor_doc[nw].wx;
	area.g_y = editor_doc[nw].wy;
	area.g_w = editor_doc[nw].ww;
	area.g_h = editor_doc[nw].wh;

	send_redraw(editor_doc[nw].hwnd, &area);	
}


VOID rename_icon(EDITOR_DOC *pEd)
{
	LPTEDI	tiname;
	LPTREE	trDlg;
	OBJECT	ob;
	char str[30];

	strncpy(str, pEd->desc, 29); str[29] = 0;
	
	fmemcpy(&ob, &(trEdit[EDITNAME]), sizeof(OBJECT));
	ob.ob_x += trEdit[0].ob_x + trEdit[CONTROLS].ob_x;
	ob.ob_y += trEdit[0].ob_y + trEdit[CONTROLS].ob_y;
	
	rsrc_gaddr(R_TREE, ICONNAME, (LPVOID *)&trDlg);

	tiname = (LPTEDI)trDlg[EDITTTL].ob_spec;
	tiname->te_txtlen = 29;
	tiname->te_ptext  = str;
	
	form_center(trDlg, &xdial, &ydial, &wdial, &hdial);  /* returns 	*/
	/**/				/* screen center x,y,w,h	*/

	
	form_dial(0, ob.ob_x, ob.ob_y, ob.ob_width, ob.ob_height, 
				xdial, ydial, wdial, hdial);/* reserves*/
	/**/				/* screen space for dialog box	*/

	form_dial(1, ob.ob_x, ob.ob_y, ob.ob_width, ob.ob_height, 
				xdial, ydial, wdial, hdial);/*  draws 	*/
	/**/				/* expanding box		*/

	objc_draw(trDlg, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	FOREVER
	{
		exitobj = form_do(trDlg, 0) & 0xFF;
		if (exitobj == ENOK)     { strcpy(pEd->desc, str); break; }
		if (exitobj == ENCANCEL) break; 
		trDlg[exitobj].ob_state &= ~SELECTED;
	}
	trDlg[ENOK].ob_state     &= ~SELECTED; /* deselect OK button	*/
	trDlg[ENCANCEL].ob_state &= ~SELECTED; 

	form_dial(2, ob.ob_x, ob.ob_y, ob.ob_width, ob.ob_height, 
				xdial, ydial, wdial, hdial);/*  draws 	*/
	/**/				/* shrinking box 		*/
  
	form_dial(3, ob.ob_x, ob.ob_y, ob.ob_width, ob.ob_height, 
				xdial, ydial, wdial, hdial);/*  draws 	*/
	/**/				/* screen space, causes redraw	*/


//	objc_draw(trEdit, EDITNAME,  1, trEdit[0].ob_x, trEdit[0].ob_y, 
//	                     trEdit[0].ob_width, trEdit[0].ob_height);

}




VOID click_editwnd(WORD nw)
{
	EDITOR_DOC *pEd = &editor_doc[nw];
	WORD x1, y1;
	LPOBJ pOb = &(trEdit[GRIDBOX]);
	WORD next_obj;
	GRECT ob_rect;
	
	select_editwnd(nw);

	x_mx = x1 = mousex;
	x_my = y1 = mousey;

	
    next_obj = objc_find(trEdit, ROOT, MAX_DEPTH, x1, y1);

    if (next_obj == NIL)
    {
		next_obj = 0;
	}
	else 
	{
		objc_offset(trEdit, next_obj, &ob_rect.g_x, &ob_rect.g_y);
		ob_rect.g_w = trEdit[next_obj].ob_width;
		ob_rect.g_h = trEdit[next_obj].ob_height;
		
		switch(next_obj)
		{
			case CLR1:
	  		case CLR2:
	  		case CLR3:
	  		case CLR4: set_colour(pEd, next_obj); 	break;
	  		case GRIDBOX: frob_bitmap(pEd, pOb);	break;
	  		case EDNBTN: rename_icon(pEd); 			break;	  
	  		case FGBTN:  icon_fg(pEd,&ob_rect);	    break;
	  		case BGBTN:  icon_bg(pEd,&ob_rect);	    break;
		}
	}
}


VOID edit_menuopt(WORD nw, WORD item)
{
	EDITOR_DOC *pEd = &editor_doc[nw];
	WORD wb, hl, nb, lb, n, nl;
	BYTE db, mb, db2, mb2;
	
	select_editwnd(nw);

	wb     = pEd->icn.ib_wicon >> 3;
	hl     = pEd->icn.ib_hicon;
	lb     = wb*hl;		/* Length in bytes of icon data */

	switch(item)
	{
		case IMGINV:	// Invert the bitmap, but not the mask// 
		for (nb = 0; nb < hl; nb++)
		{
			for (n = 0; n < pEd->icn.ib_wicon; n++)
			{
				get_point(pEd, nb, n, &db, &mb);
				if (db != mb) // not in the mask //
				{
					set_point(pEd, nb, n, !db, !mb);
				}
			}
		}
		break;

		case CAPTUP:	/* Caption moves up */
		--pEd->icn.ib_ytext;
		break;
		
		case CAPTDOWN:
		++pEd->icn.ib_ytext;
		break;
		
		case LTRUP:	/* Character moves up */
		--pEd->icn.ib_ychar;
		break;
		
		case LTRDN:
		++pEd->icn.ib_ychar;
		break;

		case LTRLT:	/* Character moves left */
		--pEd->icn.ib_xchar;
		break;
		
		case LTRRT:
		++pEd->icn.ib_xchar;
		break;

		case REFLECTH:	/* Reflect horizontally */
		reflect_h(pEd, hl, pEd->icn.ib_wicon / 2);
		break;

		case REFLECTV:
		reflect_v(pEd, hl, wb);
		break;

		case MOVEU:
		for (nl = 0; nl < hl - 1; nl++)
		{
			for (n = 0; n < wb; n++)
			{
				nb =  nl      * wb + n;
				lb = (nl + 1) * wb + n;
				pEd->dibdata[nb] = pEd->dibdata[lb];
				pEd->dibmask[nb] = pEd->dibmask[lb];
			}
		}
		for (n = 0; n < pEd->icn.ib_wicon; n++)
		{
			set_point(pEd, hl - 1, n, 0, 0);
		} 
		break;

		case MOVED:
		for (nl = hl - 1; nl > 0; --nl)
		{
			for (n = 0; n < wb; n++)
			{
				nb =  nl      * wb + n;
				lb = (nl - 1) * wb + n;
				pEd->dibdata[nb] = pEd->dibdata[lb];
				pEd->dibmask[nb] = pEd->dibmask[lb];
			}
		}
		for (n = 0; n < pEd->icn.ib_wicon; n++)
		{
			set_point(pEd, 0, n, 0, 0);
		} 
		break;
		
		case MOVEL:
		for (nl = 0; nl < hl; nl++)
		{
			WORD wm = pEd->icn.ib_wicon - 1;
			
			for (n = 0; n < wm; n++)
			{
				get_point(pEd, nl, n+1, &db, &mb);
				set_point(pEd, nl, n,   db,  mb);
			}
			set_point(pEd, nl, n, 0, 0);
		}
		break;

		case MOVERR:
		for (nl = 0; nl < hl; nl++)
		{
			WORD wm = pEd->icn.ib_wicon - 1;
			
			for (n = wm; n > 0; n--)
			{
				get_point(pEd, nl, n-1, &db, &mb);
				set_point(pEd, nl, n,   db,  mb);
			}
			set_point(pEd, nl, 0, 0, 0);
		}
		break;

		case ROTATEA:
		if (pEd->icn.ib_wicon != pEd->icn.ib_hicon)
		{
			reflect_h(pEd, hl, pEd->icn.ib_wicon / 2);
			reflect_v(pEd, hl, wb);
			break;
		}
		lb = pEd->icn.ib_wicon;
		for (nl = 0; nl < (lb / 2); nl++)
		{
			nb = pEd->icn.ib_hicon - nl;
			for (n = nl; n < nb - 1; n++)
			{
				get_point(pEd,      nl,       n, &db,  &mb);
				get_point(pEd,       n, lb-nl-1, &db2, &mb2);
				set_point(pEd,      nl,       n,  db2,  mb2);
				get_point(pEd, lb-nl-1,  lb-n-1, &db2, &mb2);
				set_point(pEd,       n, lb-nl-1,  db2,  mb2);
				get_point(pEd,  lb-n-1,      nl, &db2, &mb2);
				set_point(pEd, lb-nl-1,  lb-n-1,  db2,  mb2);
				set_point(pEd,  lb-n-1,      nl,  db,   mb);
				
			}
		}
		break;
		
		case ROTATEC:
		if (pEd->icn.ib_wicon != pEd->icn.ib_hicon)
		if (pEd->icn.ib_wicon != pEd->icn.ib_hicon)
		{
			reflect_h(pEd, hl, pEd->icn.ib_wicon / 2);
			reflect_v(pEd, hl, wb);
			break;
		}
		lb = pEd->icn.ib_wicon;
		for (nl = 0; nl < (lb / 2); nl++)
		{
			nb = pEd->icn.ib_hicon - nl;
			for (n = nl; n < nb - 1; n++)
			{
				get_point(pEd,      nl,       n, &db,  &mb);
				get_point(pEd,  lb-n-1,      nl, &db2, &mb2);
				set_point(pEd,      nl,       n,  db2,  mb2);
				get_point(pEd, lb-nl-1,  lb-n-1, &db2, &mb2);
				set_point(pEd,  lb-n-1,      nl,  db2,  mb2);
				get_point(pEd,       n, lb-nl-1, &db2, &mb2);
				set_point(pEd, lb-nl-1,  lb-n-1,  db2,  mb2);
				set_point(pEd,       n, lb-nl-1,  db,   mb);
			}
		}
		break;

		case EDITCOPY:

		cb_copy(pEd->icn.ib_wicon, pEd->icn.ib_hicon, pEd->dibdata, pEd->dibmask);
		return;	// Don't do the redraw, it isn't necessary
		
		case EDITCUT:
		cb_copy(pEd->icn.ib_wicon, pEd->icn.ib_hicon, pEd->dibdata, pEd->dibmask);

		for (n = 0; n < (wb*hl); n++)
		{
			pEd->dibdata[n] = 0;
			pEd->dibmask[n] = 0;
		}
		break;

		case EDITPAST:
		cb_paste(pEd->icn.ib_wicon, pEd->icn.ib_hicon, pEd->dibdata, pEd->dibmask);
		break;
	}
	redraw_preview(pEd);
	objc_draw(trEdit, GRIDBOX,   1, trEdit[0].ob_x,     trEdit[0].ob_y,
	                                trEdit[0].ob_width, trEdit[0].ob_height);
	
}



VOID edit_drag(EDICON_DOC *pSrc, EDITOR_DOC *pDest, WORD nicon)
{
	ICONBLK far *si = &(pSrc->icon_data[nicon]);
	WORD x,y,w,h, wb;
	BYTE db,mb;
	
	// Put the source icon in device-independent format
	trans_gimage(vdi_handle, pSrc->tree_data, nicon+1);
	
	w = min(si->ib_wicon, pDest->icn.ib_wicon);
	h = min(si->ib_hicon, pDest->icn.ib_hicon);

	for (x = 0; x < w; x++)
		for (y = 0; y < h; y++)
		{
			get_ib_point(si, y, x, &db, &mb);
			set_point(pDest, y, x,  db,  mb);
		}
	trans_gimage(vdi_handle, pSrc->tree_data, nicon+1);
		
	/* Put the "dib*" copy into device-independent format */

	wb = (pDest->icn.ib_wicon + 7) / 8;
//	vdi_untrans(pDest->bmaddr, wb, pDest->dibmask, wb, h); /* transform mask	  */
//	vdi_untrans(pDest->bdaddr, wb, pDest->dibdata, wb, h); /* transform bitmap */	 
	
	pDest->icn.ib_char  = si->ib_char;
	pDest->icn.ib_xchar = si->ib_xchar;
	pDest->icn.ib_xicon = si->ib_xicon;
	pDest->icn.ib_xtext = si->ib_xtext;
	pDest->icn.ib_ytext = si->ib_ytext;
	pDest->icn.ib_wtext = si->ib_wtext;
	pDest->icn.ib_htext = si->ib_htext;

	redraw_preview(pDest);
	send_redraw(pDest->hwnd, (GRECT *)(&pDest->wx));
}

