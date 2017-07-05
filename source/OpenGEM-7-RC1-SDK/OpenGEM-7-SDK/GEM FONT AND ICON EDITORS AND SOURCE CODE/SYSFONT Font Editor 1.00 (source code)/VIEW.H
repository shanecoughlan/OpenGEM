/************************************************************************

    SYSFONT v1.00 - system font editor for GEM

    Copyright (C) 2002  John Elliott <jce@seasip.demon.co.uk>

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

typedef VOID (*ACTION)(FONT *f, WORD ch);
/* OOP in C */

typedef struct sysfont_view	
	{							/*  */
	WORD	exists;				/* Set to 1 if this object exists */
	WORD	hwnd;				/* GEM window handle, -1 if none */
	BYTE	title[80];			/* GEM window title */
	BYTE	info[80];			/* GEM window info line */
	FONTSET *fontset;
	LPTREE	tree_data;			/* Array of OBJECT structures for drawing */
	UWORD	tree_len;			/* Length of same */
	BYTE	dirty_flag;			/* Set if there have been changes */
	WORD	wx;				/* Measurements of the window interior */
	WORD	wy;
	WORD	ww;
	WORD	wh;				/* Visible height */
	WORD	lx, ly, lw, lh;
	WORD	rx, ry, rw, rh;
	WORD	m_outl, m_outr;
	WORD	ih;				/* Total height available for icons */
	WORD	icons_across;			/* No. of icons across the window */
	WORD	cur_char;			/* Currently selected character */
	WORD	scroll_xpos;			/* How across the scrollbar is (in characters) */
	WORD	scroll_ypos;			/* Which font are we using (1-3) */
	MFDB	memfdb, scrfdb;
	WORD	mark[2];
	} SYSFONT_VIEW;


WORD create_view       (char *filename, FONTSET *set);	/* "constructor". Returns a handle */
WORD open_view         (WORD nw, char *filename);	/* Open window */
VOID close_view        (WORD nw,  GRECT *dummy);
VOID measure_view      (WORD nw);		/* Lays out the icons in the window */
VOID redraw_view       (WORD nw);

/* Window message handler function: typedef */
typedef VOID (*MSGFUNC)(WORD nw, GRECT *rect);

/* Pass message to the correct object */

VOID do_message(WORD hwnd, GRECT *area, MSGFUNC f, MSGFUNC f2);

/* Specific handler functions */

VOID ask_close_view    (WORD nw,  GRECT *dummy);		/* Close (ask whether to save first) */
VOID paint_view        (WORD nw,  GRECT *rect);
VOID topped_view	(WORD nw,  GRECT *dummy);
VOID newtop_view	(WORD nw,  GRECT *dummy);
VOID resize_view       (WORD nw,  GRECT *rect);
VOID move_view		 (WORD nw,  GRECT *rect);
VOID full_view		 (WORD nw,  GRECT *rect);
VOID slider_view		 (WORD nw,  GRECT *rect);
VOID arrow_view		 (WORD nw,  GRECT *rect);
VOID destroy_view	 (WORD nw);
VOID click_view		 (WORD nw);

VOID rebuild_fdbs(WORD nw);
VOID gen_title(WORD nw, char *filename);
VOID gen_info(WORD nw);

VOID icon_drag(WORD nsw, WORD ndw, WORD nsi, WORD x, WORD y);
VOID do_copy		(WORD nw);
VOID do_paste		(WORD nw);
VOID do_action(WORD nw, ACTION ac);

VOID act_cut(FONT *f, WORD ch);
VOID act_reflecth(FONT *f, WORD ch);
VOID act_reflectv(FONT *f, WORD ch);
VOID act_rotatec(FONT *f, WORD ch);
VOID act_rotatea(FONT *f, WORD ch);
VOID act_moveu(FONT *f, WORD ch);
VOID act_moved(FONT *f, WORD ch);
VOID act_movel(FONT *f, WORD ch);
VOID act_mover(FONT *f, WORD ch);
VOID act_bold(FONT *f, WORD ch);
VOID act_italic(FONT *f, WORD ch);
VOID act_underline(FONT *f, WORD ch);
VOID act_invert(FONT *f, WORD ch);

#ifdef INSTANTIATE

	SYSFONT_VIEW sysfont_view[MAXW];
	WORD	   topdoc = -1;		/* Which window is on top? */

#else

	extern SYSFONT_VIEW sysfont_view[MAXW];
	extern WORD       topdoc;

#endif



