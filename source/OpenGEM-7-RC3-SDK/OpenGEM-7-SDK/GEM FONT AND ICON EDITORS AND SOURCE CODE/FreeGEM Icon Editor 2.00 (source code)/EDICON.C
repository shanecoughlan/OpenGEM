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

#include "ppdgem.h"	/* [JCE] prototypes to stop PPD moaning */
#include "edicon.h" /* demo  apl  resource  */

#define MAINMODULE
#include "ediconf.h"
#include "iconfile.h"
#include "editor.h"

#include <dos.h>		/* for FP_OFF and FP_SEG macros */
#include <string.h>		/* for strcpy() */
#include <conio.h>
#include <stdio.h>


/* EDICON.C: Icon editor for Intel GEM */


/* Find out if there is a document window with the focus; if there is,
 * enable various menu items 
 */
 
VOID do_enabling(VOID)
{
	BOOLEAN b = (topdoc == -1) ? 0 : 1;
		
	menu_ienable(gl_menu, FILESAVE, b);
	menu_ienable(gl_menu, SAVEAS,   b);

	b = (topedt == -1 && topdoc == -1) ? 0 : 1;

	menu_ienable(gl_menu, EDITCOPY, b);
	menu_ienable(gl_menu, EDITCUT,  b);
	menu_ienable(gl_menu, EDITPAST, b);

	b = (topedt == -1) ? 0 : 1;

	menu_ienable(gl_menu, REFLECTH, b);
	menu_ienable(gl_menu, REFLECTV, b);
	menu_ienable(gl_menu, ROTATEA,  b);
	menu_ienable(gl_menu, ROTATEC,  b);
	menu_ienable(gl_menu, MOVEU,    b);
	menu_ienable(gl_menu, MOVED,    b);
	menu_ienable(gl_menu, MOVEL,    b);
	menu_ienable(gl_menu, MOVERR,   b);
	menu_ienable(gl_menu, IMGINV,   b);

	menu_ienable(gl_menu, CAPTUP,   b);
	menu_ienable(gl_menu, CAPTDOWN, b);
	menu_ienable(gl_menu, LTRUP,    b);
	menu_ienable(gl_menu, LTRDN,    b);
	menu_ienable(gl_menu, LTRLT,    b);
	menu_ienable(gl_menu, LTRRT,    b);

	
}


/* Find out if the internal variables topdoc and topedt are correct */

VOID calc_topwnd(VOID)
{
	int n;
	
	if (topdoc == -1 && topedt == -1)
	{
		for (n = 0; n < MAXW; n++) if (editor_doc[n].exists) 
		{
			if (editor_doc[n].hwnd >= 0)
			{
				wind_setl(editor_doc[n].hwnd, WF_TOP, 0);
				topedt = n;
				topdoc = -1;
				return;
			}
		}
		for (n = 0; n < MAXW; n++) if (edicon_doc[n].exists) 
		{
			if (edicon_doc[n].hwnd >= 0)
			{
				wind_setl(edicon_doc[n].hwnd, WF_TOP, 0);
				topedt = -1;
				topdoc = n;
				return;
			}
		}
	}
}


static WORD winlist[MAXW + 1];


static int get_winlist(void)
{
	int nw, n;
	for (n = nw = 0; n < MAXW; n++)
	{
		if (edicon_doc[n].exists && edicon_doc[n].hwnd > 0)
		{
			winlist[nw++] = edicon_doc[n].hwnd;
		}
	}
	for (n = 0; n < MAXW; n++)
	{
		if (editor_doc[n].exists && editor_doc[n].hwnd > 0)
		{
			winlist[nw++] = editor_doc[n].hwnd;
		}
	}
	winlist[nw] = 0;
	return nw;
}


void tile_windows(void)
{
	WORD wc, x, y, y0;
	GRECT rc;

	wc = get_winlist();
	if (!wc) return;

	if (wc == 1) 
	{
		do_message(winlist[0], NULL, full_docwnd, full_editwnd);
	}
	else
	{
		x = (wc / 2);

		rc.g_x = align_x(gl_xfull) - 1;
		rc.g_h = gl_hfull / x;
		rc.g_w = gl_wfull / 2;
		for (y = 0; y < x; y++)
		{
			rc.g_y = gl_yfull + rc.g_h * y;
			do_message(winlist[y], &rc, resize_docwnd, resize_editwnd);
		}
		rc.g_x = align_x(gl_xfull + (gl_wfull/2)) - 1;
		rc.g_h = gl_hfull / (wc - x);
		for (y0 = y; y < wc; y++)
		{
			rc.g_y = gl_yfull + rc.g_h * (y-y0);
			do_message(winlist[y], &rc, resize_docwnd, resize_editwnd);
		}
	}
	
}

void cascade_windows(void)
{
	WORD nw, wc, xstep, ystep;
	GRECT rc;
	
	wc = get_winlist(); if (!wc) return;

	rc.g_w = gl_wfull / 2;
	rc.g_h = gl_hfull / 2;

	xstep = rc.g_w / wc;
	ystep = rc.g_h / wc;
	
	for (nw = 0; nw < wc; nw++)
	{
		rc.g_x = align_x(gl_xfull + (nw * xstep)) - 1;
		rc.g_y = gl_yfull + (nw * ystep);
	
		do_message(winlist[nw], &rc, resize_docwnd, resize_editwnd);
	}
}


/* This is written to allow multiple documents open at once; ICONFILE.C/H 
   contain what would (in C++) be the single class that handles document
   and view.
   
   This function finds out which doc+view object owns a window, and
   calls that object's message handler. Since we this is C and not C++,
   we pass the function a handle to the object.

   */



VOID do_message(WORD wh, GRECT *area, MSGFUNC f, MSGFUNC f2)
{
	int n;
	
	for (n = 0; n < MAXW; n++)
	{
		if (wh == edicon_doc[n].hwnd) 
		{
			f(n, area);
			return;
		}
	}
	for (n = 0; n < MAXW; n++)
	{
		if (wh == editor_doc[n].hwnd)
		{
			if (f2) f2(n, area);
			return;
		}
	}
	
}


WORD hndl_button(VOID)
{
	EDICON_DOC *pDoc;
	EDITOR_DOC *pEd;
	
	if (topdoc >= 0)
	{
		pDoc = &edicon_doc[topdoc];
		if (pDoc->hwnd >= 0)
		{
			if (mousex >= pDoc->wx && mousex <= (pDoc->wx + pDoc->ww) &&
			    mousey >= pDoc->wy && mousey <= (pDoc->wy + pDoc->wh))
			{
				click_docwnd(topdoc);
			}
		}
	}
	else if (topedt >= 0)
	{
		pEd = &editor_doc[topedt];
		if (pEd->hwnd >= 0)
		{
			if (mousex >= pEd->wx && mousex <= (pEd->wx + pEd->ww) &&
			    mousey >= pEd->wy && mousey <= (pEd->wy + pEd->wh))
			{
				click_editwnd(topedt);
			}
		}
	}
	return 0;
}



WORD hndl_menu(WORD title, WORD item)
{
	WORD	done;
	WORD	i;
	FILE	*fp;
	char	filename[160];
	
	graf_mouse(ARROW, 0x0L);
	done = FALSE;

	switch (title) 
	{
	case DESKMENU: /* Desk menu */
		if (item == APPABOUT)	/* 'About' menu item	*/
			do_aboutbox();
		break;

	case FILEMENU:
		switch(item)
		{
		case FILENEW:
			i = newfile_dlg();
			if (i > 0) i = new_iconfile(i);
			do_enabling();
			break;

		case FILEOPEN:
			fp = get_file(TRUE, filename, "*.IC?", "rb");
			if (fp) i = load_iconfile(filename, fp);
			do_enabling();
			break;

		case FILESAVE:
			strcpy(filename, edicon_doc[topdoc].title + 1);
			filename[strlen(filename)-1] = 0;

			fp = fopen(filename, "wb");
			if (fp) i = save_iconfile(topdoc, fp, -1, filename);
			fclose(fp);
			if (i < 0) remove(filename);
			break;

		case SAVEAS:
			i = saveas_dlg(); if (i < 0) break;
			fp = get_file(FALSE, filename, "*.IC?", "wb");
			if (fp) i = save_iconfile(topdoc, fp, i, filename);
			fclose(fp);
			if (i < 0) remove(filename);
			break;
			
		case FILEQUIT:	
			done = 1; 
			break;
		}
	case WNDMENU:
		switch(item)
		{
			case WNDTILE:	tile_windows();    break;
			case WNDCASC:	cascade_windows(); break;
		}
	case IMGMENU:
	case CAPTMENU:
		if (topedt != -1)
		{
			edit_menuopt(topedt, item);
		}
		break;

	case EDITMENU:
		if (item == EDITSET)
		{
			hndl_settings();
			break;
		}
		if (topedt != -1)
		{
			edit_menuopt(topedt, item);
		}
		break;
	}
	menu_tnormal(gl_menu,title,TRUE);
	graf_mouse(monumber, mofaddr); 
	return (done);	
}


WORD hndl_keybd(WORD kreturn)
{
	/* Menu shortcuts */
	WORD kcode = (kreturn & 0xFF);
	
	if (kcode == ('Q' - '@')) return hndl_menu(FILEMENU, FILEQUIT);
	if (kcode == ('S' - '@')) return hndl_menu(FILEMENU, FILESAVE);
	if (kcode == ('N' - '@')) return hndl_menu(FILEMENU, FILENEW);
	if (kcode == ('O' - '@')) return hndl_menu(FILEMENU, FILEOPEN);
	if (kcode == ('A' - '@')) return hndl_menu(FILEMENU, SAVEAS);
	if (kcode == ('X' - '@')) return hndl_menu(EDITMENU, EDITCUT);
	if (kcode == ('C' - '@')) return hndl_menu(EDITMENU, EDITCOPY);
	if (kcode == ('V' - '@')) return hndl_menu(EDITMENU, EDITPAST);
	return 0;
}

/*------------------------------*/
/*			hndl_msg			*/
/*------------------------------*/
BOOLEAN	hndl_msg()
{
	BOOLEAN	done; 
	WORD	wdw_hndl;
//	GRECT	work;

	done = FALSE;
	wdw_hndl = gl_rmsg[3]; 
	switch( gl_rmsg[0] )
	{
	case MN_SELECTED:
		done = hndl_menu(wdw_hndl, gl_rmsg[4]);// Title, Item	
		break;

	case WM_REDRAW:
		graf_mouse(M_OFF, 0x0L);  	// turn mouse off
		do_message(wdw_hndl, (GRECT *)&gl_rmsg[4], paint_docwnd, paint_editwnd);
    	graf_mouse(M_ON, 0x0L);
		break;

	case WM_TOPPED:
		do_message(wdw_hndl, NULL, topped_docwnd, topped_editwnd);
		break;
		
	case WM_CLOSED:
		do_message(wdw_hndl, NULL, ask_close_docwnd, close_editwnd);
		break;

	case WM_SIZED:
		do_message(wdw_hndl, (GRECT *)&gl_rmsg[4], resize_docwnd, resize_editwnd);
		break;

	case WM_MOVED:
		do_message(wdw_hndl, (GRECT *)&gl_rmsg[4], move_docwnd, move_editwnd);
		break;

	case WM_FULLED:
		do_message(wdw_hndl, NULL, full_docwnd, full_editwnd);
		break;

	case WM_VSLID:
		do_message(wdw_hndl, (GRECT *)&gl_rmsg[4], slider_docwnd, NULL);
		break;

	case WM_ARROWED:
		do_message(wdw_hndl, (GRECT *)&gl_rmsg[4], arrow_docwnd, NULL);
		break;

	case WM_UNTOPPED:
	case WM_NEWTOP:
		do_message(wdw_hndl, NULL, newtop_docwnd, newtop_editwnd);
		break;

/*	default:
		{
			char s[50];

			sprintf(s, "[1][Message %d][OK]", gl_rmsg[0]);
			form_alert(1,s);
		}
*/		
	} 
	do_enabling();
	return(done);
} 


/*
#define WM_HSLID 25
#define WM_UNTOPPED 30
#define WM_ONTOP 31
#define WM_OFFTOP 32
*/


VOID edicon(VOID)				/* main event multi loop		*/
{
	BOOLEAN	done;
	
	key_input = FALSE;
	done = FALSE;
	FOREVER
	{  	/* wait for           Button ,    Message, 1 Mouse,keyboard */
               /* 		      Event	  Event    Rect    Event    */
	
		ev_which = evnt_multi(MU_BUTTON | MU_MESAG | MU_M1 | MU_KEYBD,
		0x02, 0x01, 0x01, /* 2 clicks, 1 button, button down */
		m_out,  /* entry , desk_area x,y,w,h */ 
		(UWORD) desk_area.g_x, (UWORD) desk_area.g_y,
		(UWORD) desk_area.g_w, (UWORD) desk_area.g_h, 
		/* mouse rect 2 flags , x,y,w,h */
		0, 0, 0, 0, 0, 
		/* Message buffer, timer low count , high count 	*/
		ad_rmsg, 0, 0,  
		/* mouse posit ,  btn state, r & lshift, Ctrl, Alt 	*/
		&mousex, &mousey, &bstate, &kstate, 
		/* keybd key,# btn clicks */
		&kreturn, &bclicks);

		wind_update(BEG_UPDATE);

		if (!(ev_which & MU_KEYBD))	/* not KEYBD event */
		{
			if (key_input)		/* key_input TRUE? */
			{
//				curs_off();	/* turn cursor off */
//				key_input = FALSE;	
//				save_work();	
			}
		}

		if (ev_which & MU_MESAG)   
		if (hndl_msg()) 
			break;  

		if (ev_which & MU_BUTTON)
		if (hndl_button())
			break;

//		if (ev_which & MU_M1)
//		if (hndl_mouse())
//			break;

		if (ev_which & MU_KEYBD)
		if (hndl_keybd(kreturn))
			break;

		wind_update(END_UPDATE);
	}
}

/*************************************************************/



/*------------------------------*/
/*			edicon_term			*/
/*------------------------------*/
VOID edicon_term(WORD term_type)
{
	int nw;

// Clean up any document objects. 
	
	for (nw = 0; nw < MAXW; nw++) destroy_document(nw);
	
	switch (term_type)	/* NOTE: all cases fall through		*/
	{
		case (0 /* normal termination */):
		
		case (3 /* no window available */):
			menu_bar(0x0L, FALSE);
		case (2 /* not enough memory */): 
			graf_mouse(ARROW, 0x0L);
			v_clsvwk( vdi_handle );
		case (1 /* no .RSC file or no virt wksta */):
			wind_update(END_UPDATE);
			appl_exit();
		case (4 /* appl_init() failed */):
			break;
	}
}


/*************************************************************/



WORD edicon_init(WORD ARGC, BYTE *ARGV[])
{
	WORD	work_in[11];
	WORD	i;
	LPTREE	tree;

	memset(edicon_doc, 0, sizeof(edicon_doc));
	memset(&g_xbuf, 0, sizeof(g_xbuf));
	g_xbuf.buf_len = sizeof(g_xbuf);
	
	gl_apid = appl_init(&g_xbuf);			/* init application	*/
	if (gl_apid == -1)
		return(4); 

	wind_update(BEG_UPDATE);
	graf_mouse(HOUR_GLASS, 0x0L);
	if (!rsrc_load( ADDR("EDICON.RSC") ))
	{    	
		/* No Resource File  */
		graf_mouse(ARROW, 0x0L);
		form_alert(1,"[3][Fatal Error !|EDICON.RSC|File Not Found][ Abort ]");
		return(1);
	}
	/* open virtual workstation */
	for (i=0; i<10; i++)
	{
		work_in[i]=1; /* initial defaults: line style,color,etc	*/
	}
	work_in[10]=2;	/* raster coordinates	*/

	/* Get the VDI handle for GEM AES screen workstation */
	gem_handle = graf_handle(&gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);
	vdi_handle = gem_handle;
	
	v_opnvwk(work_in,&vdi_handle,work_out);
	if (vdi_handle == 0)
		return(1);

	scrn_width = work_out[0] + 1; 		/* # of pixels wide	*/
	scrn_height = work_out[1] + 1; 		/* # of pixels high	*/
	scrn_xsize = work_out[3];		/* pixel width (microns)*/
	scrn_ysize = work_out[4];		/* pixel height(microns)*/

	scrn_area.g_x = 0;
	scrn_area.g_y = 0;
	scrn_area.g_w = scrn_width;
	scrn_area.g_h = scrn_height;

	rsrc_gaddr(R_TREE, ABOUTBOX, (LPVOID *)&tree);
	trans_gimage(vdi_handle, tree, APPICON); 

	ad_rmsg = ADDR((BYTE *) &gl_rmsg[0]); /* init message address	*/

	/* Get Desktop work area	*/
	wind_get(DESK, WF_WXYWH, &gl_xfull, &gl_yfull, &gl_wfull, &gl_hfull);

	desk_area.g_x = gl_xfull;
	desk_area.g_y = gl_yfull;
	desk_area.g_w = gl_wfull;
	desk_area.g_h = gl_hfull;	
	
	/* initialize menu    */
	rsrc_gaddr(R_TREE, MAINMENU, &gl_menu);

	/* show menu	      */
	menu_bar(gl_menu, TRUE);	
		
	gl_xfull = align_x(gl_xfull);

	/* get work area of window */
//	wind_get(edicon_doc[i].hwnd, WF_WXYWH,

	/* Since we allow multiple windows, we can't just set a "work_area" to
	   the size of a window. Instead, we have to handle messages from the 
	   entire desk area */

//	&work_area.g_x, &work_area.g_y,
//	&work_area.g_w, &work_area.g_h);

	// Get the resource tree for editor windows
	rsrc_gaddr(R_TREE, BITMAPED, (LPVOID *)&trEdit);

	load_settings();	/* Load EDICON.CFG if there is one */
	
	// Load command line parameters
	for (i = 1; i < ARGC; i++)
	{
		FILE *fp = fopen(ARGV[i], "rb");
		load_iconfile(ARGV[i], fp);
	}
 	
	graf_mouse(ARROW, 0x0L);
	wind_update(END_UPDATE);
	do_enabling();
	
	return(0);	/* successful initialization */
}


/*

Page*/
/************************************************************************/
/************************************************************************/
/****															     ****/
/****			    			Main Program					     ****/
/****															     ****/
/************************************************************************/
/************************************************************************/

/*------------------------------*/
/*	GEMAIN			*/
/*------------------------------*/
WORD GEMAIN(WORD ARGC, BYTE *ARGV[])
{
	WORD	term_type;
	
	if (!(term_type = edicon_init(ARGC, ARGV)))
		{
		edicon();
		}
	edicon_term(term_type);

}




