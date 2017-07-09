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


/* The following struct really should be a class. As it is, what would be 
  its "member functions" take a pointer or a handle to an instance of it.
 
  The handle scheme is: there is an array, edicon_doc[MAXW], of all these
  objects. The handle nw refers to the object edicon_doc[nw].

  MAXW is currently 8, since GEM only allows 8 windows. 

  It is a document+view which displays as a set of icons.

  Implementation is in iconfile.c

*/


typedef struct EDICON_DOCUMENT	/*  */
	{							/*  */
	WORD	exists;				/* Set to 1 if this object exists */
	WORD	hwnd;				/* GEM window handle, -1 if none */
	BYTE	title[80];			/* GEM window title */
	BYTE	info[80];			/* GEM window info line */
	LPICON  icon_data;			/* Array of ICONBLK structures for its icons */
	UWORD	icon_len;			/* Length of same */
	UWORD	icon_count;			/* No. of icons actually in use */
	LPBYTE	title_data;			/* ASCII title data */
	UWORD	title_len;			/* Length of ASCII title data allocated */
	UWORD	title_strlen;		/* Length of title data used */
	LPWORD	bitmap_data;		/* Bitmap data */
	UWORD	bitmap_len;			/* Length of bitmap data allocated */
	UWORD	bitmap_cur;			/* Length of bitmap data used */
	LPTREE	tree_data;			/* Array of OBJECT structures for drawing */
	UWORD	tree_len;			/* Length of same */
	BYTE	dirty_flag;			/* Set if there have been changes */
	WORD	wx;					/* Measurements of the window interior */
	WORD	wy;
	WORD	ww;
	WORD	wh;					/* Visible height */
	WORD	ih;					/* Total height available for icons */
	WORD	icons_across;		/* No. of icons across the window */
	WORD	scroll_offs;		/* How far down the scrollbar is (in pixels) */
	WORD	file_format;		/* 0 for GEMDESK, 1 for Windows, 2 for RCS */
	} EDICON_DOC;


WORD create_document     (VOID);		/* "constructor". Returns a handle */
WORD load_iconfile       (LPBYTE name, FILE *fp);
										/* "constructor" that loads a file */
WORD new_iconfile        (WORD type);	/* "constructor" that creates some type of file */
VOID destroy_document    (WORD nw);		/* "destructor". */
WORD open_docwnd         (WORD nw);		/* Open window */
VOID close_docwnd        (WORD nw,  GRECT *dummy);
VOID measure_docwnd      (WORD nw);		/* Lays out the icons in the window */
VOID set_scroll          (EDICON_DOC *pDoc); /* Sets the scrollbar */

WORD load_ico            (WORD nw, FILE *fp);	/* Load from .ICO file */
WORD load_icn            (WORD nw, FILE *fp);   /* Load from .ICN file */
WORD load_rcs_icn        (WORD nw, FILE *fp);   /* Load from RCS's .ICN file */

WORD alloc_docdata		 (EDICON_DOC *pDoc);		/* Allocate the dynamic arrays */
WORD add_icon            (WORD nw, ICONBLK *ib);	/* Add an icon to the view */
VOID delete_icon         (WORD nw, WORD ni); /* Delete an ICONBLK */
VOID doc_edit			 (EDICON_DOC *pDoc, WORD ni, GRECT *box); /* Edit an icon */
VOID set_title		     (EDICON_DOC *pDoc, WORD ni, LPBYTE name);/* Set the title of an icon */

WORD save_iconfile       (WORD ndoc, FILE *fp, WORD format, char *filename);


/* Window message handler function: typedef */
typedef VOID (*MSGFUNC)(WORD nw, GRECT *rect);

/* Pass message to the correct object */

VOID do_message(WORD hwnd, GRECT *area, MSGFUNC f, MSGFUNC f2);

/* Specific handler functions */

VOID ask_close_docwnd    (WORD nw,  GRECT *dummy);		/* Close (ask whether to save first) */
VOID paint_docwnd        (WORD nw,  GRECT *rect);
VOID topped_docwnd		 (WORD nw,  GRECT *dummy);
VOID newtop_docwnd		 (WORD nw,  GRECT *dummy);
VOID resize_docwnd       (WORD nw,  GRECT *rect);
VOID move_docwnd		 (WORD nw,  GRECT *rect);
VOID full_docwnd		 (WORD nw,  GRECT *rect);
VOID slider_docwnd		 (WORD nw,  GRECT *rect);
VOID arrow_docwnd		 (WORD nw,  GRECT *rect);

VOID click_docwnd		 (WORD nw);

VOID icon_drag(WORD nsw, WORD ndw, WORD nsi, WORD x, WORD y);


#ifdef MAINMODULE

	EDICON_DOC edicon_doc[MAXW];
	WORD	   topdoc = -1;		/* Which window is on top? */

#else

	extern EDICON_DOC edicon_doc[MAXW];
	extern WORD       topdoc;

#endif



