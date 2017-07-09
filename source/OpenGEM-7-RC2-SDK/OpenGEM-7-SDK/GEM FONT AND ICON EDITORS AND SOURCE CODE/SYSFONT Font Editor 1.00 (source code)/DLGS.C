/************************************************************************

    SYSFONT 1.00 - Edit the GEM system font

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


#include "sysfonti.h"



VOID do_aboutbox(VOID)
	{
	LPTREE	tree;
	WORD	xdial, ydial, wdial, hdial;

	GRECT	box;

	objc_xywh(gl_menu, MENUDESK, &box);
	rsrc_gaddr(R_TREE, ABOUTBOX, (LPVOID *)&tree);
	
	form_center(tree, &xdial, &ydial, &wdial, &hdial);  /* returns 	*/
	/**/				/* screen center x,y,w,h	*/
	
	form_dial(0, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* reserves*/
	/**/				/* screen space for dialog box	*/

	form_dial(1, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/*  draws 	*/
	/**/				/* expanding box		*/
	
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	form_do(tree, 0);

	form_dial(2, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* draws a	*/
	/**/				/* shrinking box 		*/
  
	form_dial(3, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* free 	*/
	/**/				/* screen space, causes redraw	*/

	tree[ABOUTOK].ob_state &= ~SELECTED; /* deselect OK button	*/
}


FONTSET *newfile_dlg()
{
	LPTREE	tree;
	WORD	xdial, ydial, wdial, hdial, exitobj;
	static byte sizes[3] = { 6, 8, 16 };
	static WORD edit [3] = { SET1, SET2, SET3 };
	GRECT	box;
	WORD 	nfnts = 1, onfnts = 1;
	LPLONG	spec;
	WORD 	n, v, okay;
	char 	buf[5];

	objc_xywh(gl_menu, MENUFILE, &box);
	rsrc_gaddr(R_TREE, NEWFONT, (LPVOID *)&tree);
	
	form_center(tree, &xdial, &ydial, &wdial, &hdial);  /* returns 	*/
	/**/				/* screen center x,y,w,h	*/
	
	form_dial(0, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* reserves*/
	/**/				/* screen space for dialog box	*/

	form_dial(1, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/*  draws 	*/
	/**/				/* expanding box		*/

	for (n = 0; n < 3; n++)
	{
		LPTEDI tedi = tree[edit[n]].ob_spec;
		sprintf(buf, "%d", sizes[n]);
		LSTCPY(tedi->te_ptext, buf);
	}
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	spec = (LPLONG)(&tree[SET0].ob_spec);
	FOREVER
	{
		exitobj = form_do(tree, 0) & 0x7FFF;

		switch(exitobj)
		{
			case SPPLUS:	if (nfnts < 3) ++nfnts; break;
			case SPMINUS:	if (nfnts > 1) --nfnts; break;
		}

		tree[exitobj].ob_state &= ~SELECTED; /* deselect OK button	*/
 		if (exitobj == NEWCNCL) break;
		if (exitobj == NEWOK)
		{
			okay = 1;
			for (n = 0; n < 3; n++)
			{
				LPTEDI tedi = tree[edit[n]].ob_spec;
				
				LSTCPY(buf, tedi->te_ptext);
				v = atoi(buf);
				if (v >= 1 && v <= 32) sizes[n] = v;
				else
				{
					sprintf(buf, "%d", sizes[n]);
					LSTCPY(tedi->te_ptext, buf);
					okay = 0;
				}
			}
			if (okay) break;
			rsrc_alert(ALFSIZE, 1);
			objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);
		}
		if (onfnts != nfnts)
		{
			if (nfnts < 3) 
			{
				tree[SET3  ].ob_flags |= HIDETREE;
				tree[SPPLUS].ob_state &= ~DISABLED;
			}
			else
			{
				tree[SET3  ].ob_flags &= ~HIDETREE;
				tree[SPPLUS].ob_state |=  DISABLED;
			}
			if (nfnts < 2) 
			{
				tree[SET2   ].ob_flags |= HIDETREE;
				tree[SPMINUS].ob_state |= DISABLED;
			}
			else
			{
				tree[SET2].ob_flags &= ~HIDETREE;
				tree[SPMINUS].ob_state &= ~DISABLED;
			}
			*spec &= 0xFFFFFFL;
			*spec |=  ((long)('0' + nfnts)) << 24;
			objc_draw(tree, NEWBOX, MAX_DEPTH, xdial, ydial, wdial, hdial);
			onfnts = nfnts;
		}
	}

	form_dial(2, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* draws a	*/
	/**/				/* shrinking box 		*/
  
	form_dial(3, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* free 	*/
	/**/				/* screen space, causes redraw	*/
	if (exitobj == NEWCNCL) return NULL;
	return alloc_fontset(nfnts, sizes);
}



WORD saveas_dlg(char type)
{
	LPTREE	tree;
	WORD	xdial, ydial, wdial, hdial, exitobj, n;
	GRECT	box;
	WORD	nsel;
	static WORD ctl[] = { SVAS1, SVAS2, SVAS3, SVAS4, SVAS5, SVAS6 };
        static char types[] = "RPZEAW";

	for (nsel = 0; types[nsel]; nsel++)
	{
		if (types[nsel] == type) break;	
	}
	if (!types[nsel]) nsel = -1;

	objc_xywh(gl_menu, MENUFILE, &box);
	rsrc_gaddr(R_TREE, DLGSVAS, (LPVOID *)&tree);
	if (gem5) 
	{
		tree[SVASBOX].ob_type = G_IBOX;
	}
	
	form_center(tree, &xdial, &ydial, &wdial, &hdial);  /* returns 	*/
	/**/				/* screen center x,y,w,h	*/
	
	form_dial(0, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* reserves*/
	/**/				/* screen space for dialog box	*/

	form_dial(1, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/*  draws 	*/
	/**/				/* expanding box		*/

	for (n = 0; types[n]; n++)
	{
		tree[ctl[n]].ob_state &= ~SELECTED;
		tree[ctl[n]].ob_flags |= TOUCHEXIT;
		if (nsel == n)	tree[ctl[n]].ob_state |=  CHECKED;
		else		tree[ctl[n]].ob_state &= ~CHECKED;
	}
	if (nsel == -1)
	{
		tree[ctl[0]].ob_state |= CHECKED;
		nsel = 0;
	}
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	FOREVER
	{
		exitobj = form_do(tree, 0) & 0x7FFF;

		if (exitobj == SVASOK || exitobj == SVASCNCL) break;

		for (n = 0; types[n]; n++)
		{
			WORD st = tree[ctl[n]].ob_state;
			if (exitobj == ctl[n])	
			{
				tree[ctl[n]].ob_state |= CHECKED;
				nsel = n;
			}
			else
			{
				tree[ctl[n]].ob_state &= ~CHECKED;	
			}
			tree[ctl[n]].ob_state &= ~SELECTED;
			if (g_xbuf.arch == 0 && !gem5) objc_draw(tree, SVASBOX, MAX_DEPTH, xdial, ydial, wdial, hdial);
			else if (st != tree[ctl[n]].ob_state) objc_draw(tree, ctl[n], 1, xdial, ydial, wdial, hdial);
		}
	}

	form_dial(2, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* draws a	*/
	/**/				/* shrinking box 		*/
  
	form_dial(3, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* free 	*/
	/**/				/* screen space, causes redraw	*/
	tree[exitobj].ob_state &= ~SELECTED; /* deselect OK button	*/
	if (exitobj == SVASCNCL) return -1;
	return nsel;
}

MLOCAL WORD buftoi(char *buf)
{
        char *s = strchr(buf, ':');

        if (!s) return 0;

        return xtoi(s+2);
}

static char cfgfile[160];

VOID load_prefs(VOID)
{
        FILE *fp;
        char buf[80];

        if (!cfgfile[0])
        {
                char *s;

                strcpy(cfgfile, "SYSFONT.APP");
                shel_find(ADDR(cfgfile));
                s = strrchr(cfgfile, '.');
                if (s) strcpy(s, ".CFG");
        }
	select_colour = BLACK;
	window_colour = DBLACK;
	window_shade  = 4;
        fp = fopen(cfgfile, "r");
        if (!fp) return;

        while (fgets(buf, 79, fp))
        {
                if (!strnicmp(buf, "SYSFONT.Shade.Window",20))
                {
                        window_shade = buftoi(buf);
                }
                if (!strnicmp(buf, "SYSFONT.Colour.Window",21))
                {
                        window_colour = buftoi(buf);
                }
                if (!strnicmp(buf, "SYSFONT.Colour.Select",21))
                {
                        select_colour = buftoi(buf);
                }
	}
	fclose(fp);
}

VOID save_prefs(VOID)
{
        FILE *fp = fopen(cfgfile, "w");
        fprintf(fp, "SYSFONT.Colour.Select: %04x\n", select_colour);
        fprintf(fp, "SYSFONT.Colour.Window: %04x\n", window_colour);
        fprintf(fp, "SYSFONT.Shade.Window: %04x\n",  window_shade);
        fclose(fp);
}





VOID prefs_dlg()
{
	LPTREE	tree;
	WORD	xdial, ydial, wdial, hdial, exitobj, n;
	GRECT	box;
	WORD    wc, ws, sc, x1, y1, w1, h1, x2, y2;

	wc = window_colour;
	ws = window_shade;
	sc = select_colour;

	objc_xywh(gl_menu, MENUEDIT, &box);
	rsrc_gaddr(R_TREE, DLGPREFS, (LPVOID *)&tree);
	
	form_center(tree, &xdial, &ydial, &wdial, &hdial);  /* returns 	*/
	/**/				/* screen center x,y,w,h	*/
	
	form_dial(0, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* reserves*/
	/**/				/* screen space for dialog box	*/

	form_dial(1, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/*  draws 	*/
	/**/				/* expanding box		*/

	x1 = tree[COL0].ob_x;
	y1 = tree[COL0].ob_y;
	w1 = tree[COL0].ob_width;
	h1 = tree[COL0].ob_height;
	x2 = tree[COL16].ob_x;
	y2 = tree[COL16].ob_y;

	for (n = 0; n < 16; n++)
	{
		tree[COL0  + n].ob_spec = (LPVOID)(n | (7 << 4) | (1 << 12) | 0x20FF0000L);
		tree[COL16 + n].ob_spec = (LPVOID)(n | (7 << 4) | (1 << 12) | 0x20FF0000L);
		tree[COL0  + n].ob_flags = TOUCHEXIT;
		tree[COL16 + n].ob_flags = TOUCHEXIT;
		tree[COL0  + n].ob_state &= ~CHECKED;
		tree[COL16 + n].ob_state &= ~CHECKED;
		if (n < 8)
		{
			tree[GREY0 + n].ob_spec  = (LPVOID)(1 | (n << 4) | (1 << 12) | 0x20FF0000L);
			tree[GREY0 + n].ob_flags = TOUCHEXIT;
			tree[GREY0 + n].ob_state &= ~CHECKED;
			tree[COL0  + n].ob_x = x1 + n * w1;
			tree[COL0  + n].ob_y = y1;
			tree[COL16 + n].ob_x = x2 + n * w1;
			tree[COL16 + n].ob_y = y2;
		}
		else
		{
			tree[COL0  + n].ob_x = x1 + (n-8) * w1;
			tree[COL0  + n].ob_y = y1 + h1;
			tree[COL16 + n].ob_x = x2 + (n-8) * w1;
			tree[COL16 + n].ob_y = y2 + h1;
		}	
	}
	tree[COL0  + select_colour].ob_state |= CHECKED;
	tree[COL16 + window_colour].ob_state |= CHECKED;
	tree[GREY0 + window_shade ].ob_state |= CHECKED;
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	FOREVER
	{
		exitobj = form_do(tree, 0) & 0x7FFF;

		tree[exitobj].ob_state &= ~SELECTED; /* deselect OK button	*/
		if (exitobj == PREFOK || exitobj == PREFCNCL) break;

		for (n = 0; n < 16; n++)
		{
			if (exitobj == COL0 + n)
			{
				tree[COL0 + select_colour].ob_state &= ~CHECKED;
				tree[COL0 + n].ob_state |= CHECKED;
				objc_draw(tree, COL0 + select_colour, 1, xdial, ydial, wdial, hdial);
				objc_draw(tree, COL0 + n, 1, xdial, ydial, wdial, hdial);
				select_colour = n;
				break;
			}
			if (exitobj == COL16 + n)
			{
				tree[COL16 + window_colour].ob_state &= ~CHECKED;
				tree[COL16 + n].ob_state |= CHECKED;
				objc_draw(tree, COL16 + window_colour, 1, xdial, ydial, wdial, hdial);
				objc_draw(tree, COL16 + n, 1, xdial, ydial, wdial, hdial);
				window_colour = n;
				break;
			}
			if (exitobj == GREY0 + n)
			{
				tree[GREY0 + window_shade].ob_state &= ~CHECKED;
				tree[GREY0 + n].ob_state |= CHECKED;
				objc_draw(tree, GREY0 + window_shade, 1, xdial, ydial, wdial, hdial);
				objc_draw(tree, GREY0 + n, 1, xdial, ydial, wdial, hdial);
				window_shade = n;
				break;
			}
		}
	}

	form_dial(2, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* draws a	*/
	/**/				/* shrinking box 		*/
  
	form_dial(3, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* free 	*/
	/**/				/* screen space, causes redraw	*/
	if (exitobj == PREFCNCL) 
	{
		wc = window_colour;
		ws = window_shade;
		sc = select_colour;
	}
	else save_prefs();
}


WORD transcode_dlg(char type)
{
	LPTREE	tree;
	WORD	xdial, ydial, wdial, hdial, exitobj, n;
	GRECT	box;
	WORD	nsel;
	static WORD ctl[] = { TRANSC1, TRANSC2, TRANSC3, TRANSC4, 
				TRANSC5, TRANSC6, 0 };

	switch(type)
	{
		case 'R': case 'P': nsel = 0; break;
		case 'W':	    nsel = 1; break;
		case 'Z':	    nsel = 0; break; //2; break;
		case 'E': case 'A': nsel = 3; break;
	}

	objc_xywh(gl_menu, MENUFONT, &box);
	rsrc_gaddr(R_TREE, DLGTRANS, (LPVOID *)&tree);
	if (gem5) 
	{
		tree[SVASBOX].ob_type = G_IBOX;
	}
	
	form_center(tree, &xdial, &ydial, &wdial, &hdial);  /* returns 	*/
	/**/				/* screen center x,y,w,h	*/
	
	form_dial(0, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* reserves*/
	/**/				/* screen space for dialog box	*/

	form_dial(1, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/*  draws 	*/
	/**/				/* expanding box		*/

	for (n = 0; ctl[n]; n++)
	{
		tree[ctl[n]].ob_state &= ~SELECTED;
		tree[ctl[n]].ob_flags |= TOUCHEXIT;
		if (nsel == n)	tree[ctl[n]].ob_state |=  CHECKED;
		else		tree[ctl[n]].ob_state &= ~CHECKED;
	}
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);

	FOREVER
	{
		exitobj = form_do(tree, 0) & 0x7FFF;

		if (exitobj == TRANSOK || exitobj == TRANSCNC) break;

		for (n = 0; ctl[n]; n++)
		{
			WORD st = tree[ctl[n]].ob_state;
			if (exitobj == ctl[n])	
			{
				tree[ctl[n]].ob_state |= CHECKED;
				nsel = n;
			}
			else
			{
				tree[ctl[n]].ob_state &= ~CHECKED;	
			}
			tree[ctl[n]].ob_state &= ~SELECTED;
			if (g_xbuf.arch == 0 && !gem5) objc_draw(tree, SVASBOX, MAX_DEPTH, xdial, ydial, wdial, hdial);
			else if (st != tree[ctl[n]].ob_state) objc_draw(tree, ctl[n], 1, xdial, ydial, wdial, hdial);
		}
	}

	form_dial(2, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* draws a	*/
	/**/				/* shrinking box 		*/
  
	form_dial(3, box.g_x, box.g_y, box.g_w, box.g_h, 
				xdial, ydial, wdial, hdial);/* free 	*/
	/**/				/* screen space, causes redraw	*/
	tree[exitobj].ob_state &= ~SELECTED; /* deselect OK button	*/
	if (exitobj == TRANSCNC) return -1;
	return nsel;
}


