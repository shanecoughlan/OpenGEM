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


#include <stdio.h>
#include <ctype.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

typedef struct
{
	WORD ifh_names;
	WORD ifh_base;
} ICNFILEHEADER;

/* In edicon.c */

VOID edicon_term(WORD);
VOID calc_topwnd(VOID);
VOID do_enabling(VOID);

/* In newfile.c */

WORD newfile_dlg(VOID);
WORD saveas_dlg(VOID);

/* In ediconst.c */

VOID hndl_settings(VOID);
VOID load_settings(VOID);
VOID save_settings(VOID);

/* In aboutbox.c */

VOID do_aboutbox(VOID);

/* In utils.c */

WORD lfwrite(LPVOID addr, size_t size, int count, FILE *fp);

VOID TRACE(char *fmt, ...);

LPBYTE string_addr(WORD which);
UWORD inside(UWORD x, UWORD y, GRECT *pt);
VOID grect_to_array(GRECT *area, WORD *array);
VOID rast_op(WORD mode, GRECT *s_area, MFDB *s_mfdb, 
					    GRECT *d_area, MFDB *d_mfdb);
VOID vdi_fix(LPMFDB pfd, LPVOID theaddr, WORD wb, WORD h);
WORD vdi_trans(LPVOID saddr, UWORD swb, LPVOID daddr, UWORD dwb, UWORD h);
WORD vdi_untrans(LPVOID saddr, UWORD swb, LPVOID daddr, UWORD dwb, UWORD h);
VOID objc_xywh(LPTREE tree, WORD obj, GRECT *p);
VOID wind_setl(WORD whndl, WORD what, LPVOID lptr);
WORD align_x(WORD x);
WORD rsrc_alert(WORD rsrc, WORD defbtn);
WORD lstrncmp(LPBYTE s1, LPBYTE s2, WORD count);

LPVOID local_alloc(LONG n);
VOID   local_free(LPVOID p);
LPVOID local_realloc(LPVOID p, LONG n);

VOID send_redraw(WORD wh, GRECT *p);
VOID rc_constrain(GRECT *pc, GRECT *pt);
VOID hndl_full(WORD wh);
VOID get_path(BYTE *tmp_path, BYTE *spec);
VOID add_file_name(BYTE *dname, BYTE *fname);
FILE *get_file(BOOLEAN loop, BYTE *file_name, BYTE *mask, char *mode);


#define MAXW 8

/*------------------------------*/
/*	defines						*/
/*------------------------------*/

#define	ARROW		0  		/* mouse forms		 */
#define	HOUR_GLASS	2

#define	DESK		0 		/* Desktop window handle */

#define END_UPDATE	0   		/* Window Update Flags	 */
#define	BEG_UPDATE	1

#define	PEN_INK		BLACK
#define	PEN_ERASER	WHITE

#define	PEN_FINE	1
#define	PEN_MEDIUM	5
#define	PEN_BROAD	9

#define X_FWD		0x0100		/* extended object types */
#define X_BAK		0x0200		/* used with scrolling	 */
#define X_SEL		0x0300		/* selectors		 */
#define N_COLORS	15L

#define YSCALE(x)	UMUL_DIV(x, scrn_xsize, scrn_ysize) 

#include "popcolou.h"
#include "popshade.h"

#ifdef MAINMODULE

/*------------------------------*/
/*	Local Data Structures	*/
/*------------------------------*/

WORD	gl_wchar;			/* character width		*/
WORD	gl_hchar;			/* character height		*/
WORD	gl_wbox;			/* box (cell) width		*/
WORD	gl_hbox;			/* box (cell) height		*/
WORD	gl_hspace;			/* height of space between lines*/
WORD	gem_handle;			/* GEM vdi handle		*/
WORD	vdi_handle;			/* demo vdi handle		*/
WORD	work_out[57];			/* open virt workstation values	*/
GRECT	scrn_area;			/* whole screen area		*/
//GRECT	work_area;			/* drawing area of main window  */
GRECT	undo_area;			/* area equal to work_area	*/
GRECT	save_area;			/* save area for full/unfulling */
GRECT	desk_area;			/* Area of desktop workspace */
WORD	gl_rmsg[8];			/* message buffer		*/
LPBYTE	ad_rmsg;			/* LONG pointer to message bfr	*/
LPVOID	gl_menu;			/* menu tree address		*/
WORD	gl_apid;			/* application ID		*/
WORD	gl_xfull;			/* full window 'x'		*/
WORD	gl_yfull;			/* full window 'y'		*/
WORD	gl_wfull;			/* full window 'w'		*/
WORD	gl_hfull;			/* full window 'h'		*/
WORD	scrn_width;			/* screen width in pixels	*/
WORD	scrn_height;			/* screen height in pixels	*/
WORD	scrn_planes;			/* number of color planes	*/
WORD	scrn_xsize;			/* width of one pixel		*/
WORD	scrn_ysize;			/* height of one pixel		*/
UWORD	m_out = FALSE;			/* mouse in/out of window flag	*/
WORD	ev_which;			/* event multi return state(s)	*/
UWORD	mousex, mousey;			/* mouse x,y position		*/
UWORD	bstate, bclicks;		/* button state, & # of clicks	*/
UWORD	kstate, kreturn;		/* key state and keyboard char	*/
MFDB	undo_mfdb;			/* undo buffer mmry frm def blk */
MFDB	scrn_mfdb;			/* screen memory form defn blk	*/
LONG	buff_size;			/* buffer size req'd for screen	*/
LPBYTE	buff_location;			/* screen buffer pointer	*/
WORD	pen_shade = PEN_INK;		/* saved pen shade		*/
WORD	char_fine;			/* character height for fine	*/
WORD	char_medium;			/* character height for medium	*/
WORD	char_broad;			/* character height for broad	*/
WORD	monumber = 5;			/* mouse form number		*/
LPVOID	mofaddr = NULL;			/* mouse form address		*/
WORD	file_handle;			/* file handle -> pict ld/sv	*/
BYTE	file_name[64] = "";		/* current pict file name	*/
BYTE    name[13] = "";			/* Save As pict file name       */
BOOLEAN	key_input;			/* key inputting state		*/
WORD	key_xbeg;			/* x position for line beginning*/
WORD	key_ybeg;			/* y position for line beginning*/
WORD	key_xcurr;			/* current x position		*/
WORD	key_ycurr;			/* current y position		*/
X_BUF_V2 g_xbuf;
WORD edit_colour[4];
WORD edit_shade[4];
WORD window_colour, window_shade;
#else


/*------------------------------*/
/*	Local Data Structures	*/
/*------------------------------*/

extern WORD	gl_wchar;			/* character width		*/
extern WORD	gl_hchar;			/* character height		*/
extern WORD	gl_wbox;			/* box (cell) width		*/
extern WORD	gl_hbox;			/* box (cell) height		*/
extern WORD	gl_hspace;			/* height of space between lines*/
extern WORD	gem_handle;			/* GEM vdi handle		*/
extern WORD	vdi_handle;			/* demo vdi handle		*/
extern WORD	work_out[57];			/* open virt workstation values	*/
extern GRECT	scrn_area;			/* whole screen area		*/
extern GRECT	work_area;			/* drawing area of main window  */
extern GRECT	undo_area;			/* area equal to work_area	*/
extern GRECT	save_area;			/* save area for full/unfulling */
extern GRECT desk_area;
extern WORD	gl_rmsg[8];			/* message buffer		*/
extern LPBYTE	ad_rmsg;			/* LONG pointer to message bfr	*/
extern LPVOID	gl_menu;			/* menu tree address		*/
extern WORD	gl_apid;			/* application ID		*/
extern WORD	gl_xfull;			/* full window 'x'		*/
extern WORD	gl_yfull;			/* full window 'y'		*/
extern WORD	gl_wfull;			/* full window 'w'		*/
extern WORD	gl_hfull;			/* full window 'h'		*/
extern WORD	scrn_width;			/* screen width in pixels	*/
extern WORD	scrn_height;			/* screen height in pixels	*/
extern WORD	scrn_planes;			/* number of color planes	*/
extern WORD	scrn_xsize;			/* width of one pixel		*/
extern WORD	scrn_ysize;			/* height of one pixel		*/
extern UWORD	m_out;			/* mouse in/out of window flag	*/
extern WORD	ev_which;			/* event multi return state(s)	*/
extern UWORD	mousex, mousey;			/* mouse x,y position		*/
extern UWORD	bstate, bclicks;		/* button state, & # of clicks	*/
extern UWORD	kstate, kreturn;		/* key state and keyboard char	*/
extern MFDB	undo_mfdb;			/* undo buffer mmry frm def blk */
extern MFDB	scrn_mfdb;			/* screen memory form defn blk	*/
extern LONG	buff_size;			/* buffer size req'd for screen	*/
extern LPBYTE	buff_location;			/* screen buffer pointer	*/
extern WORD	pen_shade;			/* saved pen shade		*/
extern WORD	char_fine;			/* character height for fine	*/
extern WORD	char_medium;			/* character height for medium	*/
extern WORD	char_broad;			/* character height for broad	*/
extern WORD	monumber;			/* mouse form number		*/
extern LPVOID	mofaddr;			/* mouse form address		*/
extern WORD	file_handle;			/* file handle -> pict ld/sv	*/
extern BYTE	file_name[64];			/* current pict file name	*/
extern BYTE     name[13];			/* Save As pict file name       */
extern BOOLEAN	key_input;			/* key inputting state		*/
extern WORD	key_xbeg;			/* x position for line beginning*/
extern WORD	key_ybeg;			/* y position for line beginning*/
extern WORD	key_xcurr;			/* current x position		*/
extern WORD	key_ycurr;			/* current y position		*/
extern X_BUF_V2 g_xbuf;
extern WORD edit_colour[4];
extern WORD edit_shade[4];
extern WORD window_colour, window_shade;
#endif
