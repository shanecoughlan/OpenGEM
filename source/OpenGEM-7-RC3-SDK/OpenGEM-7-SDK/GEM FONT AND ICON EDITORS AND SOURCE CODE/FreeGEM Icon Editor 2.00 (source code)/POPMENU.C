/************************************************************************

    POPMENU - pop-up menu library for GEM

    Copyright (C) 1999  John Elliott <jce@seasip.demon.co.uk>

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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "ppdgem.h"
#include "popmenu.h"
#include "ediconf.h"
#include "farmem.h"

#include <conio.h>
#include	<stdio.h>

void rc_constrain(GRECT *, GRECT *);
extern GRECT desk_area;

int menu_popup(WORD xdial, WORD ydial, LPTREE tree)
{
	WORD	wdial, hdial;
	WORD	exitobj, old_exitobj;
	WORD 	indial = 1;
	UWORD	omx, omy, mx, my, mz;
	UWORD	ks, kr, br;
	WORD	v_handle;
	WORD	wchar, hchar, wbox, hbox;
	GRECT	menu_rect;
	WORD	xitem,yitem,witem,hitem;

	wdial = tree->ob_width;
	hdial = tree->ob_height;
	
	menu_rect.g_x = xdial;
	menu_rect.g_y = ydial;
	menu_rect.g_w = wdial;
	menu_rect.g_h = hdial;

	/* Constrain menu to fit in the Desktop */
	rc_constrain(&desk_area, &menu_rect);

	xdial = menu_rect.g_x;
	ydial = menu_rect.g_y;
	wdial = menu_rect.g_w;
	hdial = menu_rect.g_h;
	tree->ob_x = xdial;
	tree->ob_y = ydial;
	
	v_handle = graf_handle(&wchar, &hchar, &wbox, &hbox);

	/* Save space behind popup */
	
	form_dial(0, 0, 0, 0, 0, xdial-1, ydial-1, wdial+2, hdial+2);
	

	graf_mouse(M_OFF, 0x0L);
	objc_draw(tree, ROOT, MAX_DEPTH, xdial-1, ydial-1, wdial+2, hdial+2);
	graf_mouse(M_ON, 0x0L);

	vq_mouse(v_handle, (WORD *)&mz, (WORD *)&mx, (WORD *)&my);

	/* If the mouse pointer is in the menu, choose its target as current
	 * selection. Otherwise, first is current */
	
	if (rc_inside(mx, my, &menu_rect))
		 exitobj = objc_find(tree, ROOT, MAX_DEPTH, mx, my);
	else exitobj = 1;
	old_exitobj = 0;

	omx = omy = 0;

	wind_update(BEG_MCTRL);	/* Stop GEM's own menus getting in the way */
	while (indial)
	{
		WORD ev_type;

		/* Show the current selection */
		
		if (old_exitobj != exitobj)
		{
			
			graf_mouse(M_OFF, 0x0L);
			if (old_exitobj)
			{
				objc_change(tree, old_exitobj, 1, xdial, ydial, wdial, hdial,
				            tree[old_exitobj].ob_state & ~SELECTED, TRUE);
			}
			if (exitobj)
			{
				objc_change(tree, exitobj,     1, xdial, ydial, wdial, hdial,
				            tree[exitobj].ob_state | SELECTED, TRUE);
			}    
			graf_mouse(M_ON, 0x0L);

			objc_offset(tree, exitobj, &xitem, &yitem);
			witem = tree[exitobj].ob_width;
			hitem = tree[exitobj].ob_height;
			
			old_exitobj = exitobj;
		}

		/* Watch keyboard & mouse for menu activity */
		ev_type = evnt_multi(MU_KEYBD | MU_BUTTON | MU_M1,
							 2,	/* 2 clicks */
							 1,	/* Left button */
							 1,	/* Button down */
							 1, xitem, yitem, witem, hitem,	/* MU_M1 */
							 0, 0, 0, 0, 0,	/* MU_M2 */
							 NULL,			/* mepbuff */
							 200, 0,			/* Timer count */
							 &mx, &my, &mz, &ks, &kr, &br);
		if (ev_type & MU_BUTTON)
		{
			if (rc_inside(mx, my, &menu_rect))
			{
				exitobj = objc_find(tree, ROOT, MAX_DEPTH, mx, my);
				indial = 0;
			}
			else exitobj = -1;
			indial = 0;
		}
		if ((ev_type & MU_M1) && (mx != omx || my != omy))
		{
			omx = mx;
			omy = my;
			if (rc_inside(mx, my, &menu_rect))
			{
				exitobj = objc_find(tree, ROOT, MAX_DEPTH, mx, my);
			}
		}
		if (ev_type & MU_KEYBD)
		{
			switch(kr)
			{
				case 0x4800:	/* up */

					do
					{
						if (exitobj > 1) --exitobj;
					} while ((tree[exitobj].ob_state & DISABLED) && 
					          exitobj > 1);
					break;
				
				case 0x5000:	/* down */
					do
					{
						if (!(tree[exitobj].ob_flags & LASTOB)) ++exitobj;
					} while ((tree[exitobj].ob_state & DISABLED) && 
					        !(tree[exitobj].ob_flags & LASTOB));
					break;
			}
			/* If you want, shortcut keys could be done here */
			
			switch(kr & 0xFF)
			{
				case '\n':
				case '\r':
					indial = 0;
					break;
				case 0x1B:
					indial = 0;
					exitobj = -1;
					break;
					
			}
		}
		if (exitobj >= 0 && (tree[exitobj].ob_state & DISABLED))
		{
			exitobj = old_exitobj;
		}
	}

	/* Deselect the chosen object */
	tree[exitobj].ob_state &= ~SELECTED;
	
	wind_update(END_MCTRL);	

	/* Remove the popup */
	form_dial(3, 0, 0, 0, 0, xdial-1, ydial-1, wdial+2, hdial+2);

	return (exitobj);


}




int menu_popup_strings(WORD x, WORD y, int count, ...)
{
	va_list ap;
	OBJECT far *tr;
	char   *s;
	int    trlen = (1 + count) * sizeof(OBJECT);
	int    n, w = 1;
	
	va_start(ap, count);
	tr = local_alloc(trlen);
	if (!tr) return -2;

	fmemset(tr, 0, trlen);
/* tr will become an object tree for the menu. */
	tr[0].ob_next   = -1;
	tr[0].ob_head   = 1;
	tr[0].ob_tail   = count;
	tr[0].ob_type   = G_BOX;
	tr[0].ob_flags  = 0;
	tr[0].ob_state  = 0;
	tr[0].ob_spec   = (LPVOID)0xFF1100L;
	tr[0].ob_x      = 0;
	tr[0].ob_y      = 0;
	tr[0].ob_width  = w;
	tr[0].ob_height = count;

	for (n = 0; n < count; n++)
	{
		s = va_arg(ap, char *);
		
		tr[n+1].ob_next  = n+2;
		tr[n+1].ob_head  = -1;
		tr[n+1].ob_tail  = -1;
		tr[n+1].ob_type  = G_STRING;
		tr[n+1].ob_flags = 0;
		tr[n+1].ob_state = 0;
		tr[n+1].ob_spec  = (LPBYTE)s;
		tr[n+1].ob_x     = 0;
		tr[n+1].ob_y     = n;
		tr[n+1].ob_width = w;
		tr[n+1].ob_height= 1;

		if (strlen(s) > w) w = strlen(s);
	}
	tr[count].ob_next   = 0;
	tr[count].ob_flags |= LASTOB; 

	/* Now set all objects to the same width */
	for (n = 0; n <= count; n++) tr[n].ob_width = w;

	/* I set dimensions in characters. Switch to pixels */
	for (n = 0; n <= count; n++) rsrc_obfix(tr, n);
	
	n = menu_popup(x, y, tr);

	local_free(tr);
	va_end(ap);

	return n;
}

