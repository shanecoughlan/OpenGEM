/************************************************************************

    POPCOLOUR - pop-up colour chooser for GEM

    Copyright (C) 1999,2000  John Elliott <jce@seasip.demon.co.uk>

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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "ppdgem.h"
#include "popmenu.h"
#include "popcolou.h"

static PPDUBLK col_ublk[16];

static WORD vdi_handle = 0;

MLOCAL VOID set_clip(WORD clip_flag, GRECT *s_area)	/* set clip to specified area		*/
{
	WORD	pxy[4];

	rc_grect_to_array(s_area, pxy);
	vs_clip(vdi_handle, clip_flag, pxy);
}

VOID clr_vdi_handle(WORD vh)
{
	vdi_handle = vh;
}


WORD paint_colour(LPPARM lp)
{
	PARMBLK pb;
	WORD col;
	WORD dotext;

	char str[3];
	LBCOPY((LPBYTE)ADDR(&pb), (LPBYTE)lp ,sizeof(PARMBLK));

    set_clip(TRUE, (GRECT *) &pb.pb_xc);

    col    =  (WORD)pb.pb_parm & 0xFFFF;
	dotext = ((LONG)pb.pb_parm & 0x10000L) ? 1 : 0;
    
	filled_rect((GRECT *)(&pb.pb_x), col, col, 1, 1);
    
	if((pb.pb_currstate != pb.pb_prevstate) ||
	   (pb.pb_currstate &  SELECTED))
	{   	
		if (pb.pb_currstate & SELECTED)
			{
		  	vsl_color(vdi_handle, contrast(col));
			}
		else
			{
		  	vsl_color(vdi_handle, col);
			}
		vsl_width(vdi_handle, 2);
		vsl_type(vdi_handle, FIS_SOLID);

		pb.pb_x+=2;
		pb.pb_y+=2;
		pb.pb_w-=4;
		pb.pb_h-=4;
		draw_rect((GRECT *) &pb.pb_x);
		pb.pb_x-=2;
		pb.pb_y-=2;
		pb.pb_w+=4;
		pb.pb_h+=4;
	}  
	if (dotext)
	{
		sprintf(str, "%d", col);

		vswr_mode(vdi_handle, 2);
		vst_color(vdi_handle, contrast(col));
		v_gtext(vdi_handle, pb.pb_x + (pb.pb_w/4), pb.pb_y + (pb.pb_h/2), str);
	}
	vsl_width(vdi_handle, 1);
	vsl_color(vdi_handle, BLACK);
	draw_rect((GRECT *) &pb.pb_x);
		
	set_clip(FALSE, (GRECT *)&pb.pb_xc);

	return 0;
}

static OBJECT popup[17] = 
{
-1,  1, 16, G_BOX,     NONE,   NORMAL, (LPVOID)0x00FF1100L,  0, 0, 16,8,
 2, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x30001100L,  0, 0,  4,2,
 3, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x31001071L,  4, 0,  4,2,
 4, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x32001072L,  8, 0,  4,2,
 5, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x33001173L, 12, 0,  4,2,
 6, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x34001074L,  0, 2,  4,2,
 7, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x35001175L,  4, 2,  4,2,
 8, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x36001176L,  8, 2,  4,2,
 9, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x37001077L, 12, 2,  4,2,
10, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x38001178L,  0, 4,  4,2,
11, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x39001079L,  4, 4,  4,2,
12, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x4100107AL,  8, 4,  4,2,
13, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x4200107BL, 12, 4,  4,2,
14, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x4300107CL,  0, 6,  4,2,
15, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x4400107DL,  4, 6,  4,2,
16, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x4500107EL,  8, 6,  4,2,
 0, -1, -1, G_BOXCHAR, LASTOB, NORMAL, (LPVOID)0x4600107FL, 12, 6,  4,2
};

static void fix(void)
{
	static int fixed = 0;
	int n;
	
	if (!fixed) 
	{
		fixed = 1;
		for (n = 0; n <= 16; n++) rsrc_obfix(popup, n);
	}
}


WORD colour_popup(WORD vh, WORD x, WORD y)
{
	WORD	n;
	LPTREE tr = &popup[0];

	vdi_handle = vh;
	fix();

	for (n = 0; n < 16; n++)
	{
		tr[n + 1].ob_state &= ~SELECTED;
		tr[n + 1].ob_flags |= SELECTABLE | TOUCHEXIT; 

		col_ublk[n].ub_parm = 0x10000L | n;
		col_ublk[n].ub_code = paint_colour;
		ppd_userdef(tr, n+1, &col_ublk[n]);
	}
	return menu_popup(x, y, tr);
}


WORD clr_tree_width(void)
{
	fix(); 
	return popup[0].ob_width;
}

WORD clr_tree_height(void)
{
	fix(); 
	return popup[0].ob_width;
}


VOID filled_rect(GRECT *area, WORD fg, WORD bg, WORD style, WORD index)
{
	WORD pxy[8];
	
   	pxy[0] = area->g_x;
   	pxy[1] = area->g_y;
   	pxy[2] = area->g_x + area->g_w - 1;
   	pxy[3] = area->g_y + area->g_h - 1;
   	pxy[4] = pxy[2];
   	pxy[5] = pxy[3];
   	pxy[3] = pxy[1];
   	pxy[6] = pxy[0];
   	pxy[7] = pxy[5];

   	vswr_mode   (vdi_handle, 1);
	vsf_interior(vdi_handle, 1);
	vsf_style(   vdi_handle, 1);
	vsf_color(   vdi_handle, bg);
	v_fillarea(  vdi_handle, 4, pxy);

	vswr_mode   (vdi_handle, 2);
	vsf_interior(vdi_handle, style);
	vsf_style(   vdi_handle, index);
	vsf_color(   vdi_handle, fg);
	v_fillarea(  vdi_handle, 4, pxy);

}


VOID draw_rect(GRECT *area)	/* Draw a rectangle round an area */
{
   	WORD	pxy[10];

   	pxy[0] = area->g_x;
   	pxy[1] = area->g_y;
   	pxy[2] = area->g_x + area->g_w - 1;
   	pxy[3] = area->g_y + area->g_h - 1;
   	pxy[4] = pxy[2];
   	pxy[5] = pxy[3];
   	pxy[3] = pxy[1];
  	pxy[6] = pxy[0];
   	pxy[7] = pxy[5];
   	pxy[8] = pxy[0];
   	pxy[9] = pxy[1];
   	v_pline(vdi_handle, 5, pxy);
}


WORD contrast(WORD col)
{
	if (col == 0 || col == 3 || col == 5 || col == 6) return BLACK;
	return WHITE;
}



