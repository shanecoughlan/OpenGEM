/************************************************************************

    POPSHADE - pop-up shade chooser for GEM

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
#include "popshade.h"
#include "popcolou.h"

static PPDUBLK shd_ublk[8];

static WORD vdi_handle = 0;

MLOCAL VOID set_clip(WORD clip_flag, GRECT *s_area)	/* set clip to specified area		*/
{
	WORD	pxy[4];

	rc_grect_to_array(s_area, pxy);
	vs_clip(vdi_handle, clip_flag, pxy);
}

VOID shd_vdi_handle(WORD vh)
{
	vdi_handle = vh;
}


WORD paint_shade(LPPARM lp)
{
	PARMBLK pb;
	WORD index, style;
	WORD dotext;

	char str[3];
	LBCOPY((LPBYTE)ADDR(&pb), (LPBYTE)lp ,sizeof(PARMBLK));

    set_clip(TRUE, (GRECT *) &pb.pb_xc);

    index  =  (WORD)pb.pb_parm & 0xFFFF;
	dotext = ((LONG)pb.pb_parm & 0x10000L) ? 1 : 0;

	style = 2;
	if (!index    ) style = 0;
	if (index == 7) style = 1;
	
   	filled_rect((GRECT *)(&pb.pb_x), BLACK, WHITE, style, index);

	if (pb.pb_currstate & SELECTED)
	{
	  	vsl_color(vdi_handle, 3);
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
		sprintf(str, "%d", index);

		vswr_mode(vdi_handle, 1);
		vst_color(vdi_handle, BLACK);
		v_gtext(vdi_handle, pb.pb_x + (pb.pb_w/4), pb.pb_y + (pb.pb_h/2), str);
	}
	vsl_width(vdi_handle, 1);
	vsl_color(vdi_handle, BLACK);
	draw_rect((GRECT *) &pb.pb_x);
	
	set_clip(FALSE, (GRECT *)&pb.pb_xc);

	return 0;
}

static OBJECT popup[9] = 
{
-1,  1,  8, G_BOX,     NONE,   NORMAL, (LPVOID)0x00FF1100L,  0, 0, 16,4,
 2, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x30001101L,  0, 0,  4,2,
 3, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x31001011L,  4, 0,  4,2,
 4, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x32001021L,  8, 0,  4,2,
 5, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x33001131L, 12, 0,  4,2,
 6, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x34001041L,  0, 2,  4,2,
 7, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x35001151L,  4, 2,  4,2,
 8, -1, -1, G_BOXCHAR, NONE,   NORMAL, (LPVOID)0x36001161L,  8, 2,  4,2,
 0, -1, -1, G_BOXCHAR, LASTOB, NORMAL, (LPVOID)0x37001071L, 12, 2,  4,2,
};

static void fix(void)
{
	static int fixed = 0;
	int n;
	
	if (!fixed) 
	{
		fixed = 1;
		for (n = 0; n <= 8; n++) rsrc_obfix(popup, n);
	}
}


WORD shade_popup(WORD vh, WORD x, WORD y)
{
	WORD	n;
	LPTREE tr = &popup[0];

	clr_vdi_handle(vh);
	vdi_handle = vh;
	fix();

	for (n = 0; n < 8; n++)
	{
		tr[n + 1].ob_state &= ~SELECTED;
		tr[n + 1].ob_flags |= SELECTABLE | TOUCHEXIT; 

		shd_ublk[n].ub_parm = 0x10000L | n;
		shd_ublk[n].ub_code = paint_shade;
		ppd_userdef(tr, n+1, &shd_ublk[n]);
	}
	return menu_popup(x, y, tr);
}


WORD shd_tree_width(void)
{
	fix(); 
	return popup[0].ob_width;
}

WORD shd_tree_height(void)
{
	fix(); 
	return popup[0].ob_width;
}




