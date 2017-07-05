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

#include "ediconf.h"
#include "iconfile.h"
#include "editor.h"

#include <dos.h>		/* for FP_OFF and FP_SEG macros */
#include <string.h>		/* for strcpy() */
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

VOID remake_specs(LPTREE tree)
{
	static int ctls[] = {CHCOL1, CHSHD1, CHCOL2, CHSHD2, CHCOL3, CHSHD3, 
	                     CHCOL4, CHSHD4, CHCOL5, CHSHD5};
	int n, s;
	LPLONG l;

	for (n = s = 0; n < 8; n++)
	{
		l = (LPLONG)&tree[ctls[n]].ob_spec;

		(*l) &= ~0x07F; 
		(*l) |= (edit_colour[s] & 0x0F);
		(*l) |= ((edit_shade[s] & 7) << 4);
		if ((n & 1)) ++s;
	}
	for (n = 0; n < 2; n++)
	{
		l = (LPLONG)&tree[ctls[n+8]].ob_spec;

		(*l) &= ~0x07F; 
		(*l) |= (window_colour & 0x0F);
	    (*l) |= ((window_shade & 7) << 4);
	}
}


VOID hndl_settings(VOID)
{
	LPTREE	tree;
	WORD	xdial, ydial, wdial, hdial, exitobj;
	WORD	xtype, clr;

	GRECT	box;

	WORD	oclr[6];

	memcpy(&oclr[0], edit_colour,   4*sizeof(WORD));
	memcpy(&oclr[4], &window_colour,  sizeof(WORD));
	
	objc_xywh(gl_menu, EDITMENU, &box);
	rsrc_gaddr(R_TREE, SETTINGS, (LPVOID *)&tree);
	remake_specs(tree);
	
	form_center(tree, &xdial, &ydial, &wdial, &hdial);  /* returns 	*/
	/**/				/* screen center x,y,w,h	*/
	
	form_dial(0, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* reserves*/
	/**/				/* screen space for dialog box	*/

	form_dial(1, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/*  draws 	*/
	/**/				/* expanding box		*/

	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);
	FOREVER
	{
		exitobj = form_do(tree, 0) & 0x7FFF;
		xtype = (tree[exitobj].ob_type & 0xFF00) >> 8;	

		if (!xtype)  /* is not extended type */
			break;

		if (xtype >= 0x10 && xtype < 0x2F)
		{
			WORD x, y;

			objc_offset(tree, exitobj, &x, &y);
			x += tree[exitobj].ob_width - clr_tree_width();
			y += tree[exitobj].ob_height;
			
			clr = colour_popup(vdi_handle, x, y);

			if (clr > 0) 
			{
				if (xtype <= 0x1F) edit_colour  [xtype-0x10] = clr-1;
				else			   window_colour             = clr-1;

				remake_specs(tree);
				
			}
			objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);
		}
		if (xtype >= 0x30 && xtype < 0x4F)
		{
			WORD x, y;

			objc_offset(tree, exitobj, &x, &y);
			x += tree[exitobj].ob_width - shd_tree_width();
			y += tree[exitobj].ob_height;
			
			clr = shade_popup(vdi_handle, x, y);

			if (clr > 0) 
			{
				if (xtype <= 0x3F) edit_shade   [xtype-0x30] = clr-1;
				else			   window_shade              = clr-1;

				remake_specs(tree);
				
			}
			objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);
		}
			
	}

	form_dial(2, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* draws a	*/
	/**/				/* shrinking box 		*/
  

	tree[exitobj].ob_state &= ~SELECTED; /* deselect OK button	*/

	if (exitobj == OKBTN)
	{
		save_settings();

		/* Force redraw over whole screen */
		form_dial(3, box.g_x, box.g_y, box.g_w, box.g_h, 
				0, 0, gl_wfull, gl_hfull);
		
	}
	else
	{
		form_dial(3, box.g_x, box.g_y, box.g_w, box.g_h, 
					xdial, ydial, wdial, hdial);/* free 	*/
		/**/				/* screen space, causes redraw	*/
		memcpy(edit_colour,    &oclr[0], 4*sizeof(WORD));
		memcpy(&window_colour, &oclr[4],  sizeof(WORD));
		remake_specs(tree);		
	}
}


MLOCAL VOID dflt_settings(VOID)
{
	edit_colour[0] = 0x0C;	/* Dark blue */
	edit_colour[1] = 0x00;	/* White */
	edit_colour[2] = 0x05;	/* Cyan */
	edit_colour[3] = 0x0D;	/* Dark Cyan */

	edit_shade[0]  = 7; /* Solid */
	edit_shade[1]  = 7; /* Solid */
	edit_shade[2]  = 2; /* Pale */
	edit_shade[3]  = 5; /* Dark */
	
	window_colour = 6;	/* Yellow */
	window_shade  = 4;	/* 50% */
}




WORD buftoi(char *buf)
{
	char *s = strchr(buf, ':');

	if (!s) return 0;

	return xtoi(s+2);
}


static char cfgfile[160];

VOID load_settings(VOID)
{
	FILE *fp;
	int nw;
	char buf[80];

	if (!cfgfile[0])
	{
		char *s;
		
		strcpy(cfgfile, "EDICON.APP");
		shel_find(ADDR(cfgfile));
		s = strrchr(cfgfile, '.');
		if (s) strcpy(s, ".CFG");
	}
	
	dflt_settings();
	fp = fopen(cfgfile, "r");
	if (!fp) return;

	while (fgets(buf, 79, fp))
	{					
		if (!strnicmp(buf, "EDICON.Shade.Edit.",18))
		{
			nw = atoi(buf + 18);
			edit_shade[nw] = buftoi(buf);	
		}
		if (!strnicmp(buf, "EDICON.Colour.Edit.",19))
		{
			nw = atoi(buf + 19);
			edit_colour[nw] = buftoi(buf);	
		}
		if (!strnicmp(buf, "EDICON.Shade.Window.0",21))
		{
			//nw = atoi(buf + 20);
			window_shade  = buftoi(buf);	
		}
		if (!strnicmp(buf, "EDICON.Colour.Window.0",22))
		{
			//nw = atoi(buf + 21);
			window_colour = buftoi(buf);	
		}
		
	}
	fclose(fp);
}


VOID save_settings(VOID)
{
	int n;
	FILE *fp = fopen(cfgfile, "w");
	for (n = 0; n < 4; n++) fprintf(fp, "EDICON.Colour.Edit.%d: %04x\n", n, edit_colour[n]);
	for (n = 0; n < 4; n++) fprintf(fp, "EDICON.Shade.Edit.%d: %04x\n",  n, edit_shade[n]);
	fprintf(fp, "EDICON.Colour.Window.0: %04x\n", window_colour);
	fprintf(fp, "EDICON.Shade.Window.0: %04x\n",  window_shade);
	fclose(fp);
}



