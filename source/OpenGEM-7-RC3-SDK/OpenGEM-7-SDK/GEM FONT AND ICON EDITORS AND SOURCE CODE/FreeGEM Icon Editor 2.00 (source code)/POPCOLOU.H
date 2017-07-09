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

WORD colour_popup(WORD vh, WORD x, WORD y);
WORD paint_colour(LPPARM lp);
WORD clr_tree_width(void);
WORD clr_tree_height(void);
VOID filled_rect(GRECT *area, WORD fg, WORD bg, WORD style, WORD index);
VOID draw_rect(GRECT *area);	/* Draw a rectangle round an area */
WORD contrast(WORD col);
VOID clr_vdi_handle(WORD vh);



