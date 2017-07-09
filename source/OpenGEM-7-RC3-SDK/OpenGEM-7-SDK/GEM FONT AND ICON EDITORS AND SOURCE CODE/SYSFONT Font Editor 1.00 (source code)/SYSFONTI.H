/*****************************************************************************
*   SYSFONT - Edit the GEM system font                                       *
*   Copyright 2002, John Elliott                                             *
*                                                                            *
* This program is free software; you can redistribute it and/or              *
* modify it under the terms of the GNU General Public License                *
* as published by the Free Software Foundation; either version 2             *
* of the License, or (at your option) any later version.                     *
*                                                                            *
* This program is distributed in the hope that it will be useful,            *
* but WITHOUT ANY WARRANTY; without even the implied warranty of             *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
* GNU General Public License for more details.                               *
*                                                                            *
* You should have received a copy of the GNU General Public License          *
* along with this program; if not, write to the Free Software                *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*
*                                                                           *
******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include "ppdgem.h"

#define MAXW 8	/* Allow at most 8 views open */

#include "sysfont.h"
#include "memfont.h"
#include "view.h"

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

/* Pass message to the correct object */

VOID do_message(WORD hwnd, GRECT *area, MSGFUNC f, MSGFUNC f2);
WORD align_x(WORD x);
WORD rsrc_alert(WORD rsrc, WORD defbtn);

/* In dlgs.c */
VOID do_aboutbox(VOID);
FONTSET *newfile_dlg(VOID);
WORD saveas_dlg(char type);
WORD transcode_dlg(char type);
VOID prefs_dlg(VOID);
VOID load_prefs(VOID);
VOID save_prefs(VOID);
VOID syserror();

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
VOID calc_topwnd(VOID);
FONTSET *load_font(FILE *fp);
int save_font(FONTSET *f, FILE *fp);
int merge_font(FONTSET *f, FILE *fp);
VOID set_clip(WORD clip_flag, GRECT *s_area);

/* Resource beautifier */
WORD rsrc_init(LPBYTE filename, WORD first_tree, WORD last_tree, WORD menu); 

/* Debugging only    
VOID TRACE (char *fmt, ...);
*/

/*------------------------------*/
/*	defines						*/
/*------------------------------*/

#define	ARROW		0  		/* mouse forms		 */
#define IBEAM		1
#define	HOUR_GLASS	2
#define CROSS		5

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


#ifdef INSTANTIATE

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
WORD	ev_which;			/* event multi return state(s)	*/
UWORD	mousex, mousey;			/* mouse x,y position		*/
UWORD	bstate, bclicks;		/* button state, & # of clicks	*/
UWORD	kstate, kreturn;		/* key state and keyboard char	*/
X_BUF_V2 g_xbuf;
WORD edit_colour[4];
WORD edit_shade[4];
WORD select_colour = BLACK; 
WORD window_colour = DBLACK; 
WORD window_shade  = 4; 
byte *clipboard = NULL;
WORD clip_h, clip_w;
WORD gem5 = -1;
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
extern WORD	ev_which;			/* event multi return state(s)	*/
extern UWORD	mousex, mousey;			/* mouse x,y position		*/
extern UWORD	bstate, bclicks;		/* button state, & # of clicks	*/
extern UWORD	kstate, kreturn;		/* key state and keyboard char	*/
extern WORD	monumber;			/* mouse form number		*/
extern LPVOID	mofaddr;			/* mouse form address		*/
extern X_BUF_V2 g_xbuf;
extern WORD select_colour;
extern WORD window_colour;
extern WORD window_shade;
extern byte *clipboard;
extern WORD clip_h, clip_w;
extern WORD gem5;
#endif

