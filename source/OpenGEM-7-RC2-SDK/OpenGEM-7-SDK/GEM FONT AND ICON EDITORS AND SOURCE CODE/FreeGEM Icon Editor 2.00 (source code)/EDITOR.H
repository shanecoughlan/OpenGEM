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


typedef struct
{
	WORD	exists;				/* Set to 1 if this object exists */
	WORD	hwnd;				/* GEM window handle, -1 if none */
	WORD	wx, wy, ww, wh;		/* Window dimensions */

	EDICON_DOC *pDoc;
	WORD	nicon;
	GRECT	ZoomBox;
	char	title[80];
	char	desc[30];
	LPVOID	brush_spec[4];
	ICONBLK icn;
	TEDINFO tedi;
	LPBYTE  bmaddr, bdaddr, dibdata, dibmask;
	WORD	drawmode;
} EDITOR_DOC;

WORD create_editor(EDICON_DOC *pDoc, WORD nIcon, GRECT *box);
WORD open_editwnd(WORD nw);
VOID select_editwnd(WORD nw);
VOID measure_editwnd(WORD nw);
VOID edit_menuopt(WORD nw, WORD nopt);

VOID close_editwnd   	 (WORD nw,  GRECT *dummy);	
VOID paint_editwnd       (WORD nw,  GRECT *rect);
VOID topped_editwnd		 (WORD nw,  GRECT *dummy);
VOID newtop_editwnd		 (WORD nw,  GRECT *dummy);
VOID resize_editwnd      (WORD nw,  GRECT *rect);
VOID move_editwnd		 (WORD nw,  GRECT *rect);
VOID full_editwnd		 (WORD nw,  GRECT *rect);
VOID slider_editwnd		 (WORD nw,  GRECT *rect);
VOID arrow_editwnd		 (WORD nw,  GRECT *rect);

VOID click_editwnd       (WORD nw);

VOID edit_drag(EDICON_DOC *pSrc, EDITOR_DOC *pDst, WORD nicon);

#ifdef MAINMODULE

	EDITOR_DOC editor_doc[MAXW];
	WORD	   topedt = -1;		/* Which window is on top? */
	LPTREE	   trEdit;

#else

	extern EDITOR_DOC editor_doc[MAXW];
	extern WORD       topedt;
	extern LPTREE	  trEdit;

#endif



