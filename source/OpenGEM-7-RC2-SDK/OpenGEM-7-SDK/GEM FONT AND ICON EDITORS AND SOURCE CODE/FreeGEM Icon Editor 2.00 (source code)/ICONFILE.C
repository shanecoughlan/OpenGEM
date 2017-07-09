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


#include "ppdgem.h"
#include "edicon.h"
#include "ediconf.h"
#include "iconfile.h"
#include "editor.h"
#include "farmem.h"
#include "win3ico.h"
#include "popmenu.h"

#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>

#define WDW_STYLE  (NAME | INFO | MOVER | CLOSER | FULLER | SIZER | VSLIDE | UPARROW | DNARROW)
#define XSPACE 80
#define YSPACE 42
#define XBDR 8
#define YBDR 8
/*
UWORD egH[];

void watch_egH()
{
	char s[200];
	int n;
	
	strcpy(s, "[1][");
	for (n = 0; n < 4; n++) sprintf(s + strlen(s), "%04x ", egH[n]);
	strcat(s, "][ Ok ]");
	form_alert(1,s);
}
*/

static WORD xcascade, ycascade;

/* I'm starting to miss C++. These are the constructor and destructor
 * of the document+view class...
 */

WORD create_document(VOID)	/* Create a document+view object (no window) */
{							/* Returns a handle (0-MAXW) to the object   */
	WORD nw;

	/* Find a blank document in the master array */
	
	for (nw = 0; nw < MAXW; nw++) if (!edicon_doc[nw].exists) break;

	if (nw >= MAXW) 		/* No blank documents available */
	{
		rsrc_alert(NOWINE, 1);
		return (-1);
	}
	memset(&edicon_doc[nw], 0, sizeof(EDICON_DOC));
	edicon_doc[nw].exists    = 1;	/* Document exists */
	edicon_doc[nw].hwnd      = -1;	/* No view window  */
	return nw;
}

WORD demo_data(WORD nw);
WORD initial_data(WORD nw, WORD type);


/* Construct from a disc file */
WORD load_iconfile(LPBYTE name, FILE *fp)
{
	int ndoc = create_document();
	int n;
	LPBYTE str;
	WORD id[2];
	char idstr[50];
	
	if (ndoc < 0) 
	{
		fclose(fp);
		return -1;
	}
	str = &(edicon_doc[ndoc].title[0]);
	n = 0;
	str[n] = ' ';
	do
	{
		++n;
		str[n] = name[n-1];
	}
	while (str[n]);
	str[n++] = ' ';
	str[n] = 0;
	
	if (open_docwnd(ndoc) < 0) 
	{
		destroy_document(ndoc);
		fclose(fp);
		return -1;
	}	

	/* Now we find out what sort of file we have. Windows icon files start
	 * with words 0 then 1. */

	if (fread(id, 2, 2, fp) < 2)
	{
		destroy_document(ndoc);
		fclose(fp);
		return -1;
	}
	fseek(fp, 0, SEEK_SET);
	fgets(idstr, 50, fp);
	fseek(fp, 0, SEEK_SET);

	if (!lstrncmp(ADDR(idstr), "/* GEM Icon Definition: */", 26)) /* RCS's .ICN format */
	{
		if (load_rcs_icn(ndoc, fp))
		{
			destroy_document(ndoc);
			fclose(fp);
			return -1;
		}
	}
	else if (id[0] == 0 && id[1] == 1)	/* .ICO format */
	{
		if (load_ico(ndoc, fp))
		{
			destroy_document(ndoc);
			fclose(fp);
			return -1;
		}
	}
	else
	{
		if (load_icn(ndoc, fp))
		{
			destroy_document(ndoc);
			fclose(fp);
			return -1;
		}
	}
	fclose(fp);
	return 0;
}


/* Construct a new file in one of 5 formats */
WORD new_iconfile(WORD type)
{
	int ndoc = create_document();
	int n;
	LPBYTE str;
	char *title = " Untitled ";
	
	if (ndoc < 0) return -1;

	str = &(edicon_doc[ndoc].title[0]);
	n = -1;
	do
	{
		++n;
		str[n] = title[n];
	}
	while (str[n]);

	if (open_docwnd(ndoc) < 0) 
	{
		destroy_document(ndoc);
		return -1;
	}
	if (initial_data(ndoc, type) < 0) 
	{
		destroy_document(ndoc);
		return -1;
	}

	
	return 0;
}





VOID destroy_document(WORD nw)
{
	if (!edicon_doc[nw].exists) return;

	edicon_doc[nw].exists = 0;
	if (edicon_doc[nw].hwnd != -1) close_docwnd(nw, NULL);
	
	if (edicon_doc[nw].icon_data)   local_free(edicon_doc[nw].icon_data);	
	if (edicon_doc[nw].title_data)  local_free(edicon_doc[nw].title_data);
	if (edicon_doc[nw].bitmap_data) local_free(edicon_doc[nw].bitmap_data);
	if (edicon_doc[nw].tree_data)   local_free(edicon_doc[nw].tree_data);
}




/* And these are its window create/destroy functions */

WORD open_docwnd(WORD nw)
{
	WORD hwnd, ld_x, ld_y, ld_w, ld_h, half_w, half_h;

	ld_x = xcascade + gl_xfull;
	ld_y = ycascade + gl_yfull;
	ld_w = gl_wfull / 2;
	ld_h = gl_hfull / 2;

	ld_x = align_x(ld_x) - 1;

	half_w = gl_wfull / 2;
	half_h = gl_hfull / 2;
	
	/* create window with all components*/
	edicon_doc[nw].hwnd = hwnd = wind_create(WDW_STYLE, gl_xfull, gl_yfull,
				gl_wfull, gl_hfull);
	if (hwnd == -1)
	{ 
		/* No Window Available		   */
		form_alert(1, string_addr(NOWING));
		return(-1);
	}
	wind_setl(hwnd, WF_NAME, &edicon_doc[nw].title[0]);
	wind_setl(hwnd, WF_INFO, &edicon_doc[nw].info[0]);

	graf_growbox(half_w, half_h, 21, 21, ld_x, ld_y, ld_w, ld_h);
	wind_open(hwnd,                  ld_x, ld_y, ld_w, ld_h);
	topdoc = nw;
	
	xcascade += 32;
	ycascade += 32;

	if (xcascade >= half_w || ycascade >= half_h)
	{
		xcascade = ycascade = 0;
	}
	return 0;
}


VOID close_docwnd(WORD nw, GRECT *dummy)
{
	EDICON_DOC *pDoc = &edicon_doc[nw];
	WORD hwnd        = pDoc->hwnd;	

	measure_docwnd(nw);
	
	graf_shrinkbox(gl_wfull/2, gl_hfull/2, 21, 21, pDoc->wx, pDoc->wy, pDoc->ww, pDoc->wh);
	wind_close(hwnd);
	wind_delete(hwnd);

	edicon_doc[nw].hwnd = -1;
	if (topdoc == nw) topdoc = -1;
	calc_topwnd();
}


/* Called when the layout of a window changes. Also updates the "info" line */

VOID measure_docwnd(WORD nw)
{
	register EDICON_DOC *pDoc = &edicon_doc[nw];
	int ni, maxi, ix, iy, scroll, acc, maxac;
	long win_height, icon_height;
	LPTREE tree;
	
	wind_get(pDoc->hwnd, WF_WXYWH, &(pDoc->wx), &(pDoc->wy), 
	                               &(pDoc->ww), &(pDoc->wh));

	switch(pDoc->file_format)
	{
		case 1: 
		sprintf(pDoc->info, "GEM desktop format. %d icon%s.", 
		        pDoc->icon_count, (pDoc->icon_count == 1) ? "" : "s");
		break;

		case 2: 
		sprintf(pDoc->info, "Windows ICO format. %d image%s.",
		        pDoc->icon_count, (pDoc->icon_count == 1) ? "" : "s");
		break;

		case 3:		
		sprintf(pDoc->info, "GEM RCS format. %d icon%s.",
		        pDoc->icon_count, (pDoc->icon_count == 1) ? "" : "s");
		break;

		default:
		sprintf(pDoc->info, "No default format. %d icon%s.",
		        pDoc->icon_count, (pDoc->icon_count == 1) ? "" : "s");
		break;
	}
	wind_setl(pDoc->hwnd, WF_INFO, &(pDoc->info[0]));

	                               
	tree = pDoc->tree_data;
	if (tree) 
	{
		tree[0].ob_x      = pDoc->wx;	/* Position the AES object tree */
		tree[0].ob_y      = pDoc->wy;	/* within the window */
		tree[0].ob_width  = pDoc->ww;
		tree[0].ob_height = pDoc->wh;

		maxi = pDoc->icon_count;

		ix = XBDR; iy = YBDR;

		acc = maxac = 0;	/* Count how many icons across */
		
		for (ni = 0; ni < maxi; ni++)
		{
			tree[ni + 1].ob_x = ix;
			tree[ni + 1].ob_y = iy;

			ix += XSPACE;
			++acc;
			if (acc > maxac) maxac = acc;
			
			if ((ix + XSPACE > tree[0].ob_width) && ix > XBDR)
			{
				ix = XBDR;
				iy += YSPACE;
				acc = 0;
			} 

		}	
	}
	pDoc->icons_across = maxac;
	
	if (ix > XBDR) iy += YSPACE;

	/* iy is the height of the tree, allowing for a margin at both ends */                    
	iy += YBDR;
	
	win_height  = pDoc->wh;
	pDoc->ih = icon_height = iy;
	if (tree && pDoc->ih > pDoc->wh) tree[0].ob_height = pDoc->ih;

	/* Compute the height of the slider in the scrollbar */
	
	scroll = (WORD)min(1000, 1000 * win_height / icon_height);
	wind_set(pDoc->hwnd, WF_VSLSIZE, scroll, 0, 0, 0);

	/* Reset scroll position after a resize. */
	pDoc->scroll_offs = 0;

	set_scroll(pDoc);
}





/* Predefined icon patterns (for file / new). These patterns were created by
   the GEM RCS, and converted from the .ICN files that it wrote.

 */

static char eg_capt[50] = "Caption     ";


UWORD egH[] =
{ 0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x7FFF, 0xFFFE, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4015, 0x5552, 
  0x470A, 0xAAA2, 0x4715, 0x5552, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x7FFF, 0xFFFE, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0xFFFF, 0xFFFF, 0x8000, 0x0001, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFEA, 0xAAAD, 
  0xB8F5, 0x555D, 0xB8EA, 0xAAAD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0x8000, 0x0001, 0xFFFF, 0xFFFF, 
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};


UWORD egL[] =
{ 0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x1FFF, 0xFFFF, 0xFFF8, 
  0x1000, 0x0000, 0x0008, 0x1000, 
  0x0000, 0x0008, 0x1000, 0x0000, 
  0x0008, 0x1000, 0x0000, 0x0008, 
  0x1000, 0x0000, 0x0008, 0x1000, 
  0x0000, 0x0008, 0x1000, 0x0000, 
  0x0008, 0x1000, 0x0000, 0x0008, 
  0x1000, 0x0000, 0x0008, 0x1000, 
  0x0000, 0x0008, 0x1000, 0x0000, 
  0x0008, 0x1000, 0x0000, 0x0008, 
  0x1000, 0x0000, 0x0008, 0x1000, 
  0xAAAA, 0xAA08, 0x1000, 0x5555, 
  0x5408, 0x11C0, 0xAAAA, 0xAA08, 
  0x1000, 0x0000, 0x0008, 0x1000, 
  0x0000, 0x0008, 0x1FFF, 0xFFFF, 
  0xFFF8, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0FFF, 0xFFFF, 0xFFF0, 0x0FFF, 
  0xFFFF, 0xFFF0, 0x0FFF, 0xFFFF, 
  0xFFF0, 0x0FFF, 0xFFFF, 0xFFF0, 
  0x0FFF, 0xFFFF, 0xFFF0, 0x0FFF, 
  0xFFFF, 0xFFF0, 0x0FFF, 0xFFFF, 
  0xFFF0, 0x0FFF, 0xFFFF, 0xFFF0, 
  0x0FFF, 0xFFFF, 0xFFF0, 0x0FFF, 
  0xFFFF, 0xFFF0, 0x0FFF, 0xFFFF, 
  0xFFF0, 0x0FFF, 0xFFFF, 0xFFF0, 
  0x0FFF, 0xFFFF, 0xFFF0, 0x0FFF, 
  0x5555, 0x55F0, 0x0FFF, 0xAAAA, 
  0xABF0, 0x0E3F, 0x5555, 0x55F0, 
  0x0FFF, 0xFFFF, 0xFFF0, 0x0FFF, 
  0xFFFF, 0xFFF0, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000
};

UWORD appH[] =
{ 0x0000, 0x0000, 0x7FFF, 0xFFFE, 
  0x7FFF, 0xFFFE, 0x7FFF, 0xFFFE, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x4000, 0x0002, 
  0x4000, 0x0002, 0x7FFF, 0xFFFE, 
  0x0000, 0x0000, 0x0000, 0x0000,
  0xFFFF, 0xFFFF, 0x8000, 0x0001, 
  0x8000, 0x0001, 0x8000, 0x0001, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0x8000, 0x0001, 
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};

UWORD appL[] =
{ 0x0000, 0x0000, 0x0000, 0x7FFF, 
  0xFFFF, 0xFFFE, 0x7FFF, 0xFFFF, 
  0xFFFE, 0x7FFF, 0xFFFF, 0xFFFE, 
  0x6000, 0x0000, 0x0006, 0x6000, 
  0x0000, 0x0006, 0x6000, 0x0000, 
  0x0006, 0x6000, 0x0000, 0x0006, 
  0x6000, 0x0000, 0x0006, 0x6000, 
  0x0000, 0x0006, 0x6000, 0x0000, 
  0x0006, 0x6000, 0x0000, 0x0006, 
  0x6000, 0x0000, 0x0006, 0x6000, 
  0x0000, 0x0006, 0x6000, 0x0000, 
  0x0006, 0x6000, 0x0000, 0x0006, 
  0x6000, 0x0000, 0x0006, 0x6000, 
  0x0000, 0x0006, 0x6000, 0x0000, 
  0x0006, 0x6000, 0x0000, 0x0006, 
  0x6000, 0x0000, 0x0006, 0x6000, 
  0x0000, 0x0006, 0x7FFF, 0xFFFF, 
  0xFFFE, 0x0000, 0x0000, 0x0000,
  0xFFFF, 0xFFFF, 0xFFFF, 0x8000, 
  0x0000, 0x0001, 0x8000, 0x0000, 
  0x0001, 0x8000, 0x0000, 0x0001, 
  0x9FFF, 0xFFFF, 0xFFF9, 0x9FFF, 
  0xFFFF, 0xFFF9, 0x9FFF, 0xFFFF, 
  0xFFF9, 0x9FFF, 0xFFFF, 0xFFF9, 
  0x9FFF, 0xFFFF, 0xFFF9, 0x9FFF, 
  0xFFFF, 0xFFF9, 0x9FFF, 0xFFFF, 
  0xFFF9, 0x9FFF, 0xFFFF, 0xFFF9, 
  0x9FFF, 0xFFFF, 0xFFF9, 0x9FFF, 
  0xFFFF, 0xFFF9, 0x9FFF, 0xFFFF, 
  0xFFF9, 0x9FFF, 0xFFFF, 0xFFF9, 
  0x9FFF, 0xFFFF, 0xFFF9, 0x9FFF, 
  0xFFFF, 0xFFF9, 0x9FFF, 0xFFFF, 
  0xFFF9, 0x9FFF, 0xFFFF, 0xFFF9, 
  0x9FFF, 0xFFFF, 0xFFF9, 0x9FFF, 
  0xFFFF, 0xFFF9, 0x8000, 0x0000, 
  0x0001, 0xFFFF, 0xFFFF, 0xFFFF
};




UWORD docH[] =
{ 0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x1FFF, 0xFF00, 
  0x1000, 0x0380, 0x1000, 0x02C0, 
  0x1000, 0x0260, 0x1000, 0x0230, 
  0x1000, 0x03F8, 0x1000, 0x0008, 
  0x1000, 0x0008, 0x1000, 0x0008, 
  0x1000, 0x0008, 0x1000, 0x0008, 
  0x1000, 0x0008, 0x1000, 0x0008, 
  0x1000, 0x0008, 0x1000, 0x0008, 
  0x1000, 0x0008, 0x1000, 0x0008, 
  0x1000, 0x0008, 0x1000, 0x0008, 
  0x1000, 0x0008, 0x1000, 0x0008, 
  0x1000, 0x0008, 0x1000, 0x0008, 
  0x1000, 0x0008, 0x1000, 0x0008, 
  0x1FFF, 0xFFF8, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0xFFFF, 0xFFC0, 
  0xFFFF, 0xFFE0, 0xE000, 0x00F0, 
  0xEFFF, 0xFC78, 0xEFFF, 0xFD3C, 
  0xEFFF, 0xFD9E, 0xEFFF, 0xFDCF, 
  0xEFFF, 0xFC07, 0xEFFF, 0xFFF7, 
  0xEFFF, 0xFFF7, 0xEFFF, 0xFFF7, 
  0xEFFF, 0xFFF7, 0xEFFF, 0xFFF7, 
  0xEFFF, 0xFFF7, 0xEFFF, 0xFFF7, 
  0xEFFF, 0xFFF7, 0xEFFF, 0xFFF7, 
  0xEFFF, 0xFFF7, 0xEFFF, 0xFFF7, 
  0xEFFF, 0xFFF7, 0xEFFF, 0xFFF7, 
  0xEFFF, 0xFFF7, 0xEFFF, 0xFFF7, 
  0xEFFF, 0xFFF7, 0xEFFF, 0xFFF7, 
  0xEFFF, 0xFFF7, 0xEFFF, 0xFFF7, 
  0xE000, 0x0007, 0xFFFF, 0xFFFF, 
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};

UWORD docL[] =
{ 0x0000, 0x0000, 0x0000, 0x0FFF, 
  0xFFFF, 0xC000, 0x0C00, 0x0000, 
  0xF000, 0x0C00, 0x0000, 0xDC00, 
  0x0C00, 0x0000, 0xC700, 0x0C00, 
  0x0000, 0xC1C0, 0x0C00, 0x0000, 
  0xFFE0, 0x0C00, 0x0000, 0x0060, 
  0x0C00, 0x0000, 0x0060, 0x0C00, 
  0x0000, 0x0060, 0x0C00, 0x0000, 
  0x0060, 0x0C00, 0x0000, 0x0060, 
  0x0C00, 0x0000, 0x0060, 0x0C00, 
  0x0000, 0x0060, 0x0C00, 0x0000, 
  0x0060, 0x0C00, 0x0000, 0x0060, 
  0x0C00, 0x0000, 0x0060, 0x0C00, 
  0x0000, 0x0060, 0x0C00, 0x0000, 
  0x0060, 0x0C00, 0x0000, 0x0060, 
  0x0C00, 0x0000, 0x0060, 0x0C00, 
  0x0000, 0x0060, 0x0FFF, 0xFFFF, 
  0xFFE0, 0x0000, 0x0000, 0x0000,
  0x3FFF, 0xFFFF, 0xE000, 0x3000, 
  0x0000, 0x3800, 0x33FF, 0xFFFF, 
  0x0E00, 0x33FF, 0xFFFF, 0x2380, 
  0x33FF, 0xFFFF, 0x38E0, 0x33FF, 
  0xFFFF, 0x3E38, 0x33FF, 0xFFFF, 
  0x0018, 0x33FF, 0xFFFF, 0xFF98, 
  0x33FF, 0xFFFF, 0xFF98, 0x33FF, 
  0xFFFF, 0xFF98, 0x33FF, 0xFFFF, 
  0xFF98, 0x33FF, 0xFFFF, 0xFF98, 
  0x33FF, 0xFFFF, 0xFF98, 0x33FF, 
  0xFFFF, 0xFF98, 0x33FF, 0xFFFF, 
  0xFF98, 0x33FF, 0xFFFF, 0xFF98, 
  0x33FF, 0xFFFF, 0xFF98, 0x33FF, 
  0xFFFF, 0xFF98, 0x33FF, 0xFFFF, 
  0xFF98, 0x33FF, 0xFFFF, 0xFF98, 
  0x33FF, 0xFFFF, 0xFF98, 0x33FF, 
  0xFFFF, 0xFF98, 0x3000, 0x0000, 
  0x0018, 0x3FFF, 0xFFFF, 0xFFF8
};



ICONBLK egiconH = { (LPBYTE)&egH[64], (LPBYTE)&egH[0], (LPBYTE)eg_capt, 
					0x1041, 6, 11, 0x17, 0x17, 32, 32, 0, 32, 72, 10};

ICONBLK egiconL = { (LPBYTE)&egL[72], (LPBYTE)&egL[0], (LPBYTE)eg_capt, 
					0x1041, 7, 8, 0x0F, 0x0F, 48, 24, 4, 24, 72, 10};

ICONBLK egappH = { (LPBYTE)&appH[64], (LPBYTE)&appH[0], (LPBYTE)eg_capt, 
					0x1000, 0, 0, 0x17, 0x17, 32, 32, 0, 32, 72, 10};

ICONBLK egappL = { (LPBYTE)&appL[72], (LPBYTE)&appL[0], (LPBYTE)eg_capt, 
					0x1000, 0, 0, 0x0F, 0x0F, 48, 24, 4, 24, 72, 10};

ICONBLK egdocH = { (LPBYTE)&docH[64], (LPBYTE)&docH[0], (LPBYTE)eg_capt, 
					0x1000, 0, 0, 0x17, 0x17, 32, 32, 0, 32, 72, 10};

ICONBLK egdocL = { (LPBYTE)&docL[72], (LPBYTE)&docL[0], (LPBYTE)eg_capt, 
					0x1000, 0, 0, 0x0F, 0x0F, 48, 24, 4, 24, 72, 10};
					
/*
LPVOID malloc_debug(int n)
{
	char s[60];
	LPVOID p;

	FILE *fp = fopen("d:\\gemapp.log", "a+");
	
	sprintf(s,"[0][Malloc %d bytes ", n);
	p = local_alloc(n);
	if (p) strcat(s, "succeeded][OK]\n");
	else   strcat(s, "failed[Oalp]\n");
	fprintf(fp, s);
	fclose(fp);
	return p;	
}


LPVOID realloc_debug(LPVOID p, WORD n)
{
	LPVOID q;

	TRACE("Realloc %lx to", (long)p);
	
	q = local_realloc(p, n);

	TRACE("%lx [%d]\n", (long)q, n);

	return q;
}

#define local_alloc malloc_debug
#define local_realloc realloc_debug
*/


void debug_tree(LPTREE pTree, int nobj, char *caption);


void diewith(LPBYTE p, LPBYTE s)
{
	char t[300];
	FILE *fp;
	
	sprintf(t,"[0][Assertion failed!|%s|%s|[ Stop ]", p, s);
	
	fp = fopen("d:\\gemapp.log", "a+");
	fprintf(fp, "%s\n", t);
	fclose(fp);
//	form_alert(1,t);
	edicon_term(0);
	exit(0);
}
					

void assert_sane(WORD nw, LPBYTE p)
{
	EDICON_DOC *pDoc = &edicon_doc[nw];
	WORD max = pDoc->icon_count;
	WORD ni;
	LPICON	pIcon;	 WORD iconlen;
	LPBYTE	pBitmap; WORD bitlen;
	LPTREE	pTree;
	LPBYTE	pTitle;	 WORD titlen;
	LPVOID  spec;

	char s[320];
	
	pIcon   = pDoc->icon_data;	 iconlen = pDoc->icon_len;
	pBitmap = (LPBYTE)pDoc->bitmap_data; bitlen  = pDoc->bitmap_len;
	pTree	= pDoc->tree_data;
	pTitle	= pDoc->title_data;	 titlen  = pDoc->title_len;
	
	for (ni = 0; ni < max; ni++)
	{
		spec = pTree[ni + 1].ob_spec;

		sprintf(s, "Icon %d at %lx below range %lx-%lx", ni,
					(long)(pIcon), (long)spec, ((long)spec) + iconlen); 

		if ((LPBYTE)spec < (LPBYTE)pIcon) diewith(p,s);

		sprintf(s, "Icon %d at %lx above range %lx-%lx", ni,
					(long)(pIcon), (long)spec, ((long)spec) + iconlen);

		if ((LPBYTE)spec > ((LPBYTE)pIcon) + iconlen) diewith(p,s);

		sprintf(s, "Icon %d bitmap below range", ni);
		if (pIcon[ni].ib_pdata < pBitmap) diewith(p,s);

		sprintf(s, "Icon %d mask below range", ni);
		if (pIcon[ni].ib_pmask < pBitmap) diewith(p,s);
		 
		sprintf(s, "Icon %d title below range", ni);
		if (pIcon[ni].ib_ptext < pTitle) diewith(p,s);

		sprintf(s, "Icon %d bitmap %lx above range %lx-%lx", ni,
					(long)(pIcon[ni].ib_pdata), 
					(long)pBitmap, (long)(pBitmap + bitlen));
		if (pIcon[ni].ib_pdata > pBitmap + bitlen) diewith(p,s);

		sprintf(s, "Icon %d mask above range", ni);
		if (pIcon[ni].ib_pmask > pBitmap + bitlen) diewith(p,s);

		sprintf(s, "Icon %d title above range", ni);
		if (pIcon[ni].ib_ptext > pTitle  + titlen) diewith(p,s);
	}
	
}

WORD demo_data(WORD nw)
{
	int n;

	for (n = 0; n < 72; n++) 
	{
		sprintf(eg_capt, "Caption %d", n);
		if (add_icon(nw, &egiconH) < 0) return -1;
	}
	measure_docwnd(nw);
	assert_sane(nw, "Add demo data");
		
	return 0;	
}




WORD initial_data(WORD nw, WORD type)
{
	int n;
	EDICON_DOC *pDoc = &edicon_doc[nw];
	ICONBLK *pb;
	
	switch(type)
	{
		case 1:	/* GEM desktop, 72 icons */
		case 2:
		for (n = 0; n < 72; n++) 
		{
			if (n < 8)  
				{
				pb = (type == 1 ? &egiconH : &egiconL);
				sprintf(eg_capt, "Drive %d", n);
				}
			else if  (n < 40) 
				{
				pb = (type == 1 ? &egappH : &egappL);
				sprintf(eg_capt, "App %d", n-8);
				}
			else if  (n < 72)
				{
				pb = (type == 1 ? &egdocH : &egdocL);
				sprintf(eg_capt, "Doc %d", n-40);
				}

			if (add_icon(nw, pb) < 0) return -1;
		}
		pDoc->file_format = 1;
		break;

		case 3: /* Single icon */
		case 4:
		strcpy(eg_capt, "Icon");
		if (add_icon(nw, (type == 3 ? &egiconH : &egiconL)) < 0) return -1;
		pDoc->file_format = 2;
		break;
			
		case 5:	/* Double icon */
		strcpy(eg_capt, "32x32");
		if (add_icon(nw, &egiconH) < 0) return -1;
		strcpy(eg_capt, "48x24");
		if (add_icon(nw, &egiconL) < 0) return -1;
		pDoc->file_format = 2;
		break;
	}
	measure_docwnd(nw);
		
	return 0;	
}



/*
* Note: The memory allocation code is for a dynamic system, in which the
* memory allotted to an icon file grows as the file does, using 
* repeated calls to realloc() to double the sizes of arrays.
*
* Since Pacific C's malloc() and realloc() get horribly confused if you 
* do this too much (due to memory fragmentation) I'm going to get round 
* it by allocating large chunks of memory to begin with. 
*
* The following statistics assume 72 icons, the maximum size of a DESK*.ICN
* file. Actual files will be smaller because different icons share the
* same bitmaps.
*
* 24k for bitmaps;5k for iconblks; 3k for object definitions and 
* 2k for titles. 
*
*
*/

WORD alloc_docdata(EDICON_DOC *pDoc)
{
	if (!pDoc->icon_data)
	{
		pDoc->icon_data  = local_alloc(72*sizeof(ICONBLK));
		if (!pDoc->icon_data) return -1;
		
		pDoc->icon_len   = 72 * sizeof(ICONBLK);
		pDoc->icon_count = 0;
	}

	if (!pDoc->title_data)
	{
		pDoc->title_data  = local_alloc(2048);
		if (!pDoc->title_data) return -1;
		
		pDoc->title_len    = 2048;
		pDoc->title_strlen = 0;
	}

	if (!pDoc->tree_data)
	{
		LPTREE tree;
		
		pDoc->tree_data       = local_alloc(73*sizeof(OBJECT));
		if (!pDoc->tree_data) return -1;
		pDoc->tree_len        = 73*sizeof(OBJECT);

		/* Initialise the tree data */

		tree = pDoc->tree_data;
	
		fmemset(tree, 0, sizeof(OBJECT));

		tree->ob_next  = tree->ob_head = tree->ob_tail = -1;
		tree->ob_type  = G_BOX;
		tree->ob_flags = LASTOB;
		tree->ob_state = NORMAL;
		tree->ob_spec  = (LPVOID)(0x01140L | (window_colour & 0x0F)); 
	}

	if (!pDoc->bitmap_data)	
	{
		pDoc->bitmap_data = local_alloc(24576);	
		if (!pDoc->bitmap_data) return -1;
		pDoc->bitmap_len  = 24576;
		pDoc->bitmap_cur  = 0;
	}
	
	return 0;
}




void debug_tree(LPTREE pTree, int nobj, char *caption)
// Tree debug code...
	{
		int n;
		static char s[512];
		static char s1[90];
		
		sprintf(s,"[0][%s|", caption);
		for (n = 0; n <= nobj; n++)
		{
			sprintf(s1, "%02x:%02x,%02x,%02x;%04x;(%d,%d) %d x %d|",n,
						pTree[n].ob_next,
						pTree[n].ob_head,
						pTree[n].ob_tail,
						pTree[n].ob_flags,
						pTree[n].ob_x,
						pTree[n].ob_y,
						pTree[n].ob_width,
						pTree[n].ob_height);
			strcat(s, s1);
		}
		strcat(s,"][OK|Stop]");

		if (form_alert(1, ADDR(s)) > 1)
		{
			edicon_term(0);
			exit(1);
		}
		

	}

/* When data areas allocated by malloc() are resized, the objects in the
 * documents will have dangling pointers to the old data. This function
 * rejigs the pointers.
 */
	

VOID repoint_offset(ULONG far *p, ULONG offs) { *p += offs;}
	


VOID repoint_bitmaps(LPICON picon, LPBYTE pbmp1, LPBYTE pbmp2, UWORD max)
{
	UWORD n;
	ULONG offs = pbmp2 - pbmp1;
	
	for (n = 0; n < max; n++)
	{
		repoint_offset((ULONG far *)&(picon[n].ib_pmask), offs);
		repoint_offset((ULONG far *)&(picon[n].ib_pdata), offs);
	}
	
}
	

VOID repoint_titles(LPICON picon, LPBYTE pttl1, LPBYTE pttl2, UWORD max)
{
	UWORD n;
	ULONG offs = pttl2 - pttl1;
	
	for (n = 0; n < max; n++)
	{
		repoint_offset((ULONG far *)&(picon[n].ib_ptext), offs);
	}
}

VOID repoint_icons(LPTREE tree, LPBYTE picn1, LPBYTE picn2, UWORD max)
{
	UWORD n;
	LONG offs = picn2 - picn1;

	for (n = 1; n < max; n++)
	{
		repoint_offset((ULONG far *)&(tree[n].ob_spec), offs);
	}
	
}


VOID set_title		     (EDICON_DOC *pDoc, WORD ni, LPBYTE caption)
{
	LPICON pIcon      = &(pDoc->icon_data[ni]);
	LPBYTE pTitle     = pDoc->title_data;
	LPBYTE pOldTitle  = pIcon->ib_ptext;
	int clen, maxs, strcur;

	if (fstrlen(pOldTitle) >= fstrlen(caption) || fstrlen(caption) < 13)
	{
		fstrcpy(pOldTitle, caption);
		return;
	}
	
	maxs = pDoc->title_len;
	strcur= pDoc->title_strlen;
	clen  = fstrlen(caption) + 1; if (clen < 13) clen = 13;

	
	while (clen + strcur > maxs)	/* More space needed for strings */
	{
		pTitle = local_realloc(pTitle, 2 * maxs);
		if (!pTitle) return; /* No memory for new string area */

		repoint_titles(pDoc->icon_data, pDoc->title_data, pTitle, 
		               pDoc->icon_count);		

		pDoc->title_data = pTitle;
		pDoc->title_len  *= 2;
		maxs *= 2;
	}

	fmemcpy(&(pTitle[strcur]), caption, clen);
	pDoc->title_strlen += clen;
	pIcon->ib_ptext = &(pTitle[strcur]);
}

	

/* Add an icon (described by the ICONBLK) to the document */

WORD add_icon(WORD nw, ICONBLK *ib)
{
	EDICON_DOC *pDoc = &edicon_doc[nw];
	UWORD   maxi, maxt, maxs, nicon, nobj, strcur, clen, blen, bmcur, maxb;
	LPICON pIcon;
	LPTREE pTree;
	LPBYTE pTitle, pBitmap, caption;
	LPBYTE pBiBits, pBiMask;
	UWORD	bitmap_len;

	
	if (alloc_docdata(pDoc) < 0) return -1;	/* Ensure that memory is allocated */
	
	pIcon	   = pDoc->icon_data;
	pTree	   = pDoc->tree_data;
	pTitle     = pDoc->title_data;
	pBitmap	   = (LPBYTE)pDoc->bitmap_data;
	caption    = ib->ib_ptext;
	bitmap_len = (WORD)(((LONG)ib->ib_wicon * ib->ib_hicon) / 8);

	/* Check if the document's ICONBLK, TREE etc. arrays are */
	/* big enough. If not, double their sizes */

	/* This code seems to crash (I'm not sure whether it's my fault
	 * or the Pacific C malloc()) so at the moment, you simply can't
	 * have > 72 icons in a file. */

	maxi = pDoc->icon_len / sizeof(ICONBLK);
	maxt = pDoc->tree_len / sizeof(OBJECT);
	maxb = pDoc->bitmap_len;
	bmcur= pDoc->bitmap_cur;
	maxs = pDoc->title_len;
	nicon = pDoc->icon_count;
	strcur= pDoc->title_strlen;
	clen  = fstrlen(caption) + 1; if (clen < 13) clen = 13;
	blen  = 2 * bitmap_len;

	if (pDoc->icon_count >= maxi)	
	{
		rsrc_alert(FILEFULL,1);
		return -1;
	}
	
	
	while (blen + bmcur > maxb)	/* More space needed for bitmaps */
	{
		pBitmap = local_realloc(pBitmap, 2 * maxb);
		if (!pBitmap) return -1;	/* No memory for new bitmap area */

		repoint_bitmaps(pDoc->icon_data, (LPBYTE)(pDoc->bitmap_data), 
						(LPBYTE)pBitmap, pDoc->icon_count);

		pDoc->bitmap_data = (LPWORD)pBitmap;
		pDoc->bitmap_len  *= 2;
		maxb *= 2;
	}
	

	while (clen + strcur > maxs)	/* More space needed for strings */
	{
		pTitle = local_realloc(pTitle, 2 * maxs);
		if (!pTitle) return -1; /* No memory for new string area */

		repoint_titles(pDoc->icon_data, pDoc->title_data, pTitle, 
		               pDoc->icon_count);		

		pDoc->title_data = pTitle;
		pDoc->title_len  *= 2;
		maxs *= 2;
	}

	
	if (pDoc->icon_count >= maxi)	
	{
		pIcon = local_realloc(pIcon, 2 * pDoc->icon_len);
		if (!pIcon) return -1;	/* No memory for the new icon */

		repoint_icons(pDoc->tree_data, (LPBYTE)pDoc->icon_data, 
					  (LPBYTE)pIcon, pDoc->icon_count + 1);		

		pDoc->icon_data = pIcon;
		pDoc->icon_len  *= 2;
	}
	
	
	if (pDoc->icon_count >= (maxt - 1))	/* -1 for the panel holding the icons */
	{
		pTree = local_realloc(pTree, 2 * pDoc->tree_len);
		if (!pTree) return -1;	/* No memory for the new icon-holding object */

		pDoc->tree_data = pTree;
		pDoc->tree_len  *= 2;		
	}

	// Copy in the title
	fmemcpy(&(pTitle[strcur]), caption, clen);
	pDoc->title_strlen += clen;

	pBiBits = pBitmap + bmcur + bitmap_len;
	pBiMask = pBitmap + bmcur;

	// Copy in the bitmaps
	fmemcpy(pBiMask, ib->ib_pmask, bitmap_len);
	fmemcpy(pBiBits, ib->ib_pdata, bitmap_len);
	pDoc->bitmap_cur   += blen;

	// Copy in the iconblk.
	fmemcpy(&(pIcon[nicon]), ib, sizeof(ICONBLK));
	pIcon[nicon].ib_pmask = pBiMask;
	pIcon[nicon].ib_pdata = pBiBits;
	pIcon[nicon].ib_ptext = &(pTitle[strcur]);

	if (ib->ib_wicon <= 32) pIcon[nicon].ib_xicon = 23;
	else                    pIcon[nicon].ib_xicon = 24;
	pIcon[nicon].ib_yicon = 0;
	pIcon[nicon].ib_ytext = pIcon[nicon].ib_hicon;
		
	++pDoc->icon_count;
	
	nobj = nicon + 1;

	fmemset(&(pTree[nobj]), 0, sizeof(OBJECT));
	
	pTree[nobj].ob_next = pTree[nobj].ob_head = pTree[nobj].ob_tail = -1;
	pTree[nobj].ob_type = G_ICON;
	pTree[nobj].ob_flags= LASTOB;
	pTree[nobj].ob_state= NORMAL;
	pTree[nobj].ob_spec = &(pIcon[nicon]);
	pTree[nobj].ob_width= XSPACE;
	pTree[nobj].ob_height=YSPACE;

	objc_add(pTree, 0, nobj);	// Add the icon to the tree

	if (nobj > 1) 
	{
		pTree[nobj - 1].ob_flags &= ~LASTOB;
	}
//	debug_tree(pTree, nobj, "After add()");

	trans_gimage(vdi_handle, pTree, nobj);	// Make it into display format 	

	return 0;
}

	

// Delete an icon from the tree

VOID delete_icon(WORD nw, WORD nicon)
{
	int ni;

	EDICON_DOC *pDoc = &edicon_doc[nw];

	// TO DO
	//
	// Although we have got rid of the ICONBLK we are deleting, its
	// caption and bitmaps are still in memory. Currently we don't have
	// garbage collection, and they will stay until the document is 
	// closed.  
	//
	// We should really eliminate the bitmaps and caption.

	
	// Are there icons above the one being deleted?
	if ((pDoc->icon_count - 1) > nicon)
	{
		for (ni = nicon + 1; ni < pDoc->icon_count; ni++)
		{
			fmemcpy(&pDoc->icon_data[nicon-1],
			        &pDoc->icon_data[nicon],
			        sizeof(ICONBLK));
			pDoc->tree_data[ni].ob_spec = pDoc->tree_data[ni+1].ob_spec;
		}
	}
	objc_delete(pDoc->tree_data, pDoc->icon_count);
	--pDoc->icon_count;
	pDoc->tree_data[pDoc->icon_count - 1].ob_flags |= LASTOB;
	measure_docwnd(nw);
	// Repaint window
	send_redraw(pDoc->hwnd, (GRECT *)&(pDoc->wx));
}


/* Window message handlers */


VOID paint_docwnd(WORD nw, GRECT *area) 
{
	GRECT		box;
	WORD 	    wh   = edicon_doc[nw].hwnd;
	EDICON_DOC *pDoc = &edicon_doc[nw];
	WORD	xy[4];
	
	if (pDoc->tree_data) pDoc->tree_data[0].ob_y -= pDoc->scroll_offs;
	
	pDoc->tree_data->ob_spec  = (LPVOID)(0x01140L | (window_colour & 0x0F)); 
	wind_get(wh, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	while ( box.g_w && box.g_h ) 
	{   // AES returns zero width and height when no more rectangles 
	    if (rc_intersect(area, &box))
	    {  
			if (!pDoc->tree_data)
			{
		    	xy[0] = box.g_x;
		    	xy[1] = box.g_y;
		    	xy[2] = box.g_x + box.g_w;
		    	xy[3] = box.g_y + box.g_h;
		    	v_bar(vdi_handle, xy);
			}
			else 
				objc_draw(pDoc->tree_data, ROOT, MAX_DEPTH, box.g_x, box.g_y, 
					   box.g_w, box.g_h);
					   
		}
	    // get next rectangle in window's rectangle list		
		wind_get(wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	}
	if (pDoc->tree_data) pDoc->tree_data[0].ob_y += pDoc->scroll_offs;	
}


VOID resize_docwnd(WORD nw, GRECT *area)
{
	WORD hwnd = edicon_doc[nw].hwnd;
	GRECT work;

	// Get working area 
	
	wind_calc(1, WDW_STYLE, area->g_x, area->g_y, area->g_w, area->g_h,
	                        &work.g_x, &work.g_y, &work.g_w, &work.g_h); 

	work.g_x = align_x(work.g_x);	// WORD alignment	
	work.g_w = align_x(work.g_w); 	//  for performance 	

	if (work.g_w < XBDR + XSPACE) work.g_w = XBDR + XSPACE;
	
	// get border area x, y, w, h	
	wind_calc(0, WDW_STYLE,  work.g_x,    work.g_y,   work.g_w,   work.g_h,
                            &area->g_x, &area->g_y, &area->g_w, &area->g_h);

	// set current x,y,w,h - borders, title bar, (info)      
	wind_set(hwnd, WF_CXYWH, area->g_x, area->g_y, area->g_w, area->g_h);

	measure_docwnd(nw);                            

	// Force a redraw
	send_redraw(edicon_doc[nw].hwnd, area);

}


VOID move_docwnd(WORD nw, GRECT *area)
{
	rc_constrain(&desk_area, area);
	
	area->g_x = align_x(area->g_x);
	wind_set(edicon_doc[nw].hwnd, WF_CXYWH, area->g_x,
			area->g_y, area->g_w, area->g_h);
	measure_docwnd(nw);
}


VOID full_docwnd(WORD nw, GRECT *dummy)
{
	GRECT area;
	
	hndl_full(edicon_doc[nw].hwnd);
	measure_docwnd(nw);

	area.g_x = edicon_doc[nw].wx;
	area.g_y = edicon_doc[nw].wy;
	area.g_w = edicon_doc[nw].ww;
	area.g_h = edicon_doc[nw].wh;
	
	send_redraw(edicon_doc[nw].hwnd, &area);

}


VOID ask_close_docwnd(WORD nw, GRECT *dummy)
{
	if (edicon_doc[nw].dirty_flag) 
	{
		int rv = rsrc_alert(ASKSAVE, 3);

		if (rv == 1);					/* Save */
		else if (rv == 3) return;		/* Cancel */
	}
	destroy_document(nw);
}


VOID topped_docwnd(WORD nw, GRECT *dummy)
{
	wind_setl(edicon_doc[nw].hwnd, WF_TOP, 0);
	topdoc = nw;
	topedt = -1;
}

VOID newtop_docwnd(WORD nw, GRECT *dummy)
{
	if (topdoc == nw) topdoc = -1;
}


/* A slider has been changed. Convert its position from scrollbar units to
 * pixels 
 */
 

VOID slider_docwnd(WORD nw, GRECT *rect)
{
	long total_doc, seen_doc, pos;
	
	EDICON_DOC *pDoc = &edicon_doc[nw];

	total_doc = pDoc->ih;
	seen_doc  = pDoc->wh;
	pos       = rect->g_x;
	
	if (pDoc->ih < pDoc->wh) pDoc->scroll_offs = 0;
	else pDoc->scroll_offs = (WORD)(pos * (total_doc - seen_doc) / 1000);
	
	set_scroll(pDoc);

}

VOID set_scroll(EDICON_DOC *pDoc)
{
	long total_doc, seen_doc, pos;
	WORD ni, scroll;

	/* Set the scrollbar position to that given by pDoc->scroll_offs, 
	 * converting it from pixels to scrollbar units
	 */
	
	total_doc = pDoc->ih;
	seen_doc  = pDoc->wh;
	if (total_doc <= seen_doc) pos = 0L;
	else pos = 1000L * (pDoc->scroll_offs) / (total_doc - seen_doc);
	
	wind_get(pDoc->hwnd, WF_VSLIDE, &scroll, &ni, &ni, &ni);
	
	if (scroll != pos) 
	{
		wind_set(pDoc->hwnd, WF_VSLIDE, pos, 0, 0, 0);
		send_redraw(pDoc->hwnd, (GRECT *)&(pDoc->wx));
	}
}




VOID arrow_docwnd(WORD nw, GRECT *rect)
{
	EDICON_DOC *pDoc = &edicon_doc[nw];
	WORD newpos;
	
	switch(rect->g_x)
	{
		case WA_UPPAGE:
			newpos = pDoc->scroll_offs - pDoc->wh;
			break;
		case WA_DNPAGE:
			newpos = pDoc->scroll_offs + pDoc->wh; 
			break;
		case WA_UPLINE:
			newpos = pDoc->scroll_offs - YSPACE;
			break;
		case WA_DNLINE:
			newpos = pDoc->scroll_offs + YSPACE;	
			break;
	}
	/* Keep the new scroll position in limits */
	
	if (newpos < 0)           newpos = 0;
	if (pDoc->ih <= pDoc->wh) newpos = 0;
	else if (newpos > (pDoc->ih - pDoc->wh)) newpos = pDoc->ih - pDoc->wh; 
	
	if (pDoc->ih <= pDoc->wh) pDoc->scroll_offs = 0;
	else pDoc->scroll_offs = newpos;
	set_scroll(pDoc);
}


VOID click_docwnd(WORD nw)
{
	EDICON_DOC *pDoc = &edicon_doc[nw];
	WORD x,y, ox, oy, ow, oh;
	WORD nicon, ni;
	GRECT recticon;
	
	x = mousex - pDoc->wx - XBDR;
	y = mousey - pDoc->wy - YBDR + pDoc->scroll_offs;

	if (x < 0 || y < 0) return;

	nicon = ((y / YSPACE) * pDoc->icons_across) + (x / XSPACE);

	if (nicon >= pDoc->icon_count) return;

	/* There has been a click on an icon. Set its Selected field, & redraw it */

	/* See if any previous icon was selected. If so, deselect it */
	for (ni = 0; ni < pDoc->icon_count; ni++)
	{
		if (ni != nicon && (pDoc->tree_data[ni+1].ob_state & SELECTED))
		{
			pDoc->tree_data[ni+1].ob_state &= ~SELECTED;
			recticon.g_x = pDoc->tree_data[ni + 1].ob_x + pDoc->wx;
			recticon.g_y = pDoc->tree_data[ni + 1].ob_y - pDoc->scroll_offs + pDoc->wy;

			if (recticon.g_y < pDoc->wy) recticon.g_y = pDoc->wy;
			recticon.g_w = XSPACE;
			recticon.g_h = YSPACE;
		
			send_redraw(pDoc->hwnd, &recticon);
		}	
	}	
	pDoc->tree_data[nicon + 1].ob_state ^= SELECTED;

	recticon.g_x = pDoc->tree_data[nicon + 1].ob_x + pDoc->wx;
	recticon.g_y = pDoc->tree_data[nicon + 1].ob_y - pDoc->scroll_offs + pDoc->wy;

	if (recticon.g_y < pDoc->wy) recticon.g_y = pDoc->wy;
	recticon.g_w = XSPACE;
	recticon.g_h = YSPACE;

		
	send_redraw(pDoc->hwnd, &recticon);

	ox  = recticon.g_x;
	oy  = recticon.g_y;
	ow  = pDoc->icon_data[nicon].ib_wicon;
	oh  = pDoc->icon_data[nicon].ib_hicon;
	ox += pDoc->icon_data[nicon].ib_xicon;
	oy += pDoc->icon_data[nicon].ib_yicon; 
	
	if (bclicks == 2)	/* Double-clicked the icon? */
	{
		doc_edit(pDoc, nicon, &recticon);
	}
	else 
	{
		UWORD pxy[4], bbutton, breturn;
		WORD drag = FALSE;

		pxy[0] = mousex;
		pxy[1] = mousey;      

		/* See which happens first - mouse up or mouse move */
		
		ev_which = evnt_multi(MU_BUTTON | MU_M1, 
					0x01, 0x01, 0x00, /* 1 click, 1 button, button up */
					1, pxy[0], pxy[1], 1, 1,
					0, 0, 0, 0, 0,
					ad_rmsg, 125, 0, 
					&pxy[2], &pxy[3], &bbutton, &kstate,
					&kreturn, &breturn);

		/* It moved. Do a drag. */
		if (ev_which & MU_M1)
		{
			graf_mouse(FLAT_HAND, 0x0L);
			graf_dragbox(ow, oh, 
						ox, oy, /*pxy[0] - (ow/2), pxy[1] - (oh / 2), */ 
			            desk_area.g_x,
			            desk_area.g_y,
			            desk_area.g_w,
			            desk_area.g_h,
			            (WORD *)&pxy[0], (WORD *)&pxy[1]);
			graf_mouse(monumber, mofaddr);
			pxy[2] = pxy[0];
			pxy[3] = pxy[1];
			drag = TRUE;
		}

		if (drag)
		{
			WORD target = wind_find(pxy[0], pxy[1]);
			WORD n;

			for (n = 0; n < MAXW; n++)
			{
				if (edicon_doc[n].hwnd == target && 
				    edicon_doc[n].exists)
				{
					icon_drag(nw, n, nicon, pxy[0], pxy[1]);
					break;
				}
				if (editor_doc[n].hwnd == target &&
				    editor_doc[n].exists)
				{
					edit_drag(pDoc, &editor_doc[n], nicon);
					break;
				}
			}
			if (n == MAXW)
			{
				int nv = menu_popup_strings(pxy[0], pxy[1], 2, 
				          " Delete icon ", " Cancel ");

				if (nv == 1) delete_icon(nw, nicon); 
			}
				
		}
		
	}	/* if */
}


/* 64-element ordered dither array matrix, holds values 63-126 */
static int dither[8][8] = 
{
	{	 63,  95,  71, 103,  65,  97,  73, 105 },
	{	111,  79, 119,  87, 113,  81, 121,  89 },	
	{	 75, 107,  67,  99,  77, 109,  69, 101 },
	{	123,  91, 115,  83, 125,  93, 117,  85 },	
	{	 66,  98,  74, 106,  64,  96,  72, 104 },
	{	114,  82, 122,  90, 112,  80, 120,  88 },	
	{	 78, 110,  70, 102,  76, 108,  68, 100 },	
	{	126,  94, 118,  86, 124,  92, 116,  84 }
};	


BYTE clr(WORD px, WORD py, WIN31BM16 *bm, unsigned char col)
{
	UWORD r,g,b;
	UWORD bright, lev;

	col &= 0x0F;
	
	r = bm->bm_pal[4*col]    & 0xFF;
	g = bm->bm_pal[4*col+1]  & 0xFF;
	b = bm->bm_pal[4*col+2]  & 0xFF;

	bright = 3*r+3*g+2*b;	/* Brightness of pixel, 0-2048 */
	bright /= 32;			/* Brightness is 0-63 */
	bright += 64;			/* Brightness is 64-127 */

	lev = dither[py&7][px&7];
/*
	{
		FILE *fp = fopen("d:/gemapp.log", "a");
		fprintf(fp, "y=%03d x=%03d col=%02x rgb=%02x:%02x:%02x bright=%03d lev=%03d %d\n",
					py,px,col,r,g,b,bright, lev, (lev >= bright));
		fclose(fp);
	}
*/	
	if (lev < bright) return 0;
	else		      return 1;
}


/* Load a bitmap, dithering down from 16 colours to mono */

WORD load_bmp_body16(FILE *fp, ICONBLK *ib)
{
	WIN31BM16 bm;
	BYTE *pico, *pmask;
	WORD fw, fh, fwb, fwd;
	WORD x,y, gx, gm, px;
	BYTE bt, lo, hi;
	
	if (fread(&bm, sizeof(bm), 1, fp) < 1) return -1;
	
	fw  = bm.bm_width;					/* Form width */
	fwb = ((bm.bm_width + 15) / 16)*2;	/* Width in GEM's DIB */
	fwd = ((bm.bm_width +  7) / 8 )*4;	/* Width in Windows's DIB */

	fh = bm.bm_height / 2;

	pico  = (BYTE *)malloc(fwb * fh); 
	if (!pico) return -1;
	pmask = (BYTE *)malloc(fwb * fh); 
	if (!pmask) 
	{
		free(pico);
		return -1;
	}
	/* We now pull in the rows for the bitmap. */

	for (y = fh - 1; y >= 0; y--)
	{
		gx = 0;
		px = 0;
		gm = 0x80;
		for (x = 0; x < fwd; x++)
		{
			bt = fgetc(fp);

			lo = clr(px++, y, &bm, bt & 0x0f); 
			hi = clr(px++, y, &bm, bt >> 4);

			if (hi) pico[y * fwb + (gx ^ 1)] |= gm;
			else	pico[y * fwb + (gx ^ 1)] &= ~gm;
			gm = gm >> 1;
			if (lo) pico[y * fwb + (gx ^ 1)] |= gm;
			else	pico[y * fwb + (gx ^ 1)] &= ~gm;
			gm = gm >> 1;

			if (!gm) { gm = 0x80; ++gx; } 
		}
	}
	/* And the mask, which is always 1-bit */
	fwd = ((bm.bm_width + 31) / 32)*4;	/* Width in Windows's DIB */
	for (y = fh - 1; y >= 0; y--)
		for (x = 0; x < fwd; x++)
		{
			bt = fgetc(fp);

			if ((x^1) < fwb) pmask[y * fwb + (x^1)] = bt;
		}

	for (y = 0; y < fwb*fh; y++)
	{
		BYTE mb = pmask[y];
		BYTE ib = pico[y];

		pmask[y] = ~mb;
		pico[y]  = ib & (~mb);
	}
	ib->ib_pdata = ADDR(pico);
	ib->ib_pmask = ADDR(pmask);
		
	return 0;
	
}



WORD load_bmp_body(FILE *fp, ICONBLK *ib)
{
	WIN31BM bm;
	BYTE *pico, *pmask;
	WORD fw, fh, fwb, fwd;
	WORD x,y;
	BYTE bt;
	
	if (fread(&bm, sizeof(bm), 1, fp) < 1) return -1;
	
	fw  = bm.bm_width;					/* Form width */
	fwb = ((bm.bm_width + 15) / 16)*2;	/* Width in GEM's DIB */
	fwd = ((bm.bm_width + 31) / 32)*4;	/* Width in Windows's DIB */

	fh = bm.bm_height / 2;

	pico  = (BYTE *)malloc(fwb * fh); 
	if (!pico) return -1;
	pmask = (BYTE *)malloc(fwb * fh); 
	if (!pmask) 
	{
		free(pico);
		return -1;
	}
	/* We now pull in the rows for the bitmap. */

	for (y = fh - 1; y >= 0; y--)
		for (x = 0; x < fwd; x++)
		{
			bt = fgetc(fp);
			
			if ((x^1) < fwb) pico[y * fwb + (x^1)] = bt;

		}
	/* And the mask */
	
	for (y = fh - 1; y >= 0; y--)
		for (x = 0; x < fwd; x++)
		{
			bt = fgetc(fp);

			if ((x^1) < fwb) pmask[y * fwb + (x^1)] = bt;
		}

	for (y = 0; y < fwb*fh; y++)
	{
		BYTE mb = pmask[y];
		BYTE ib = pico[y];

		pmask[y] = ~mb;
		pico[y]  = (~ib) ^ mb;		
	}
	ib->ib_pdata = ADDR(pico);
	ib->ib_pmask = ADDR(pmask);
		
	return 0;
	
}





					
WORD load_ico(WORD ndoc, FILE *fp)
{
	WIN31IH ih;
	WIN31IR ir;
	int n, ni;
	ICONBLK cur_icon;
	long fpos;
	
	/* Load the file header */
	if (fread(&ih, sizeof(ih), 1, fp) < 1) return -1;

	ni = 1;

	/* Load all mono icons from the file */
	for (n = 0; n < ih.ih_icno; n++)
	{
		if (fread(&ir, sizeof(ir), 1, fp) < 1) return -1;

		if (ir.ir_cdepth == 2 || ir.ir_cdepth == 16)	/* 2 colours */
		{
			/* Load it */

			fpos = ftell(fp);

			/* Seek to start of BMP */
			fseek(fp, ir.ir_offset, SEEK_SET);
			switch(ir.ir_cdepth)
			{
				case 2:
					if (load_bmp_body(fp, &cur_icon) < 0) return -1;
					break;
				case 16:
					if (load_bmp_body16(fp, &cur_icon) < 0) return -1;
					break;
			}
			cur_icon.ib_ptext = (LPBYTE)eg_capt;
			cur_icon.ib_char  = 0x1000;	/* No letter */
			cur_icon.ib_xchar = 0;
			cur_icon.ib_ychar = 0;

			/* The code to calculate xicon, yicon and xtext produces */
			/* correct results for 48x24 and 32x32 icons. What it    */
			/* does to other sizes, I dread to think :-)             */
			
			cur_icon.ib_xicon = 39 - (ir.ir_width / 2);
			cur_icon.ib_yicon = ir.ir_height - 9;
			cur_icon.ib_wicon = ir.ir_width;
			cur_icon.ib_hicon = ir.ir_height;
			cur_icon.ib_xtext = (ir.ir_width / 4) - 8;
			cur_icon.ib_ytext = ir.ir_height;
			cur_icon.ib_wtext = 72;
			cur_icon.ib_htext = 10;

			sprintf(eg_capt, "Icon %d", ni);
			++ni;
		
			if (add_icon(ndoc, &cur_icon) < 0) 
			{
				free(cur_icon.ib_pdata);
				free(cur_icon.ib_pmask);
				return -1;
			}
			fseek(fp, fpos, SEEK_SET);
			free(cur_icon.ib_pdata);
			free(cur_icon.ib_pmask);
		}
	}

	if (ni == 1)
	{
		rsrc_alert(NOICOS, 1);
		return -1;
	}
	
	edicon_doc[ndoc].file_format = 2;	/* ICO */

	measure_docwnd(ndoc);
	assert_sane(ndoc, "Load ICO file");
		
	return 0;	
}
                 


					
WORD load_rcs_icn(WORD ndoc, FILE *fp)
{
	int n, ni;
	ICONBLK cur_icon;
	char strbuf[50];
	char title[50];
	char *sdata;
	int ww, w,h, sz;
	UWORD w1, w2, w3, w4;
	LPUWORD pic, pim;

	if (!fgets(strbuf, 50, fp)) return -1;	/* "GEM Icon Definition" line */
	{
		if (!fgets(strbuf, 50, fp)) return -1;
		if (strncmp(strbuf, "#define ICON_W ", 15)) return -1;
		w = xtoi(strbuf + 18);

		if (!fgets(strbuf, 50, fp)) return -1;
		if (strncmp(strbuf, "#define ICON_H ", 15)) return -1;
		h = xtoi(strbuf + 18);
		
		if (!fgets(strbuf, 50, fp)) return -1;
		if (strncmp(strbuf, "#define DATASIZE ", 17)) return -1;
		sz = xtoi(strbuf + 20);

		if (!fgets(strbuf, 50, fp)) return -1;
		sdata = strchr(strbuf, '[');
		if (sdata) *sdata = 0;

		fstrcpy(title, strbuf + 6);
		cur_icon.ib_ptext = (LPBYTE)title;
		cur_icon.ib_char  = 0x1000;	/* No letter */
		cur_icon.ib_xchar = 0;
		cur_icon.ib_ychar = 0;

		/* The code to calculate xicon, yicon and xtext produces */
		/* correct results for 48x24 and 32x32 icons. What it    */
		/* does to other sizes, I dread to think :-)             */
			
		cur_icon.ib_xicon = 39 - (w / 2);
		cur_icon.ib_yicon = h - 9;
		cur_icon.ib_wicon = w;
		cur_icon.ib_hicon = h;
		cur_icon.ib_xtext = (w / 4) - 8;
		cur_icon.ib_ytext = h;
		cur_icon.ib_wtext = 72;
		cur_icon.ib_htext = 10;

		ww = (w + 15) / 16;

		cur_icon.ib_pdata = malloc(2*sz);
		cur_icon.ib_pmask = malloc(2*sz);

		pic = (LPUWORD)(cur_icon.ib_pdata);
		pim = (LPUWORD)(cur_icon.ib_pmask);
		
		for (n = 0; n < sz;)
		{
			if (!fgets(strbuf, 50, fp)) return -1;

			sscanf(strbuf+2,"0x%x, 0x%x, 0x%x, 0x%x", &w1, &w2, &w3, &w4);

			pic[n++] = w1;
			pic[n++] = w2;
			pic[n++] = w3;
			pic[n++] = w4;
		}
		/* Skip to the mask bitmap */
		do
		{
			if (!fgets(strbuf, 50, fp)) return -1;
		} while (strncmp(strbuf, "UWORD ", 6));

		for (n = 0; n < sz;)
		{
			if (!fgets(strbuf, 50, fp)) return -1;

			sscanf(strbuf+2,"0x%x, 0x%x, 0x%x, 0x%x", &w1, &w2, &w3, &w4);

			pim[n++] = w1;
			pim[n++] = w2;
			pim[n++] = w3;
			pim[n++] = w4;
		}
		
		if (add_icon(ndoc, &cur_icon) < 0) 
		{
			free(cur_icon.ib_pdata);
			free(cur_icon.ib_pmask);
			return -1;
		}
		++ni;
		free(cur_icon.ib_pdata);
		free(cur_icon.ib_pmask);
	}
	
	if (ni == 1)
	{
		rsrc_alert(NOICOS, 1);
		return -1;
	}
	
	edicon_doc[ndoc].file_format = 3;	/* RCS icon */

	measure_docwnd(ndoc);
	assert_sane(ndoc, "Load RCS ICN file");
		
	return 0;	
}




WORD load_icn(WORD ndoc, FILE *fp)
{
	ICNFILEHEADER ifh;
	int nic;
	ICONBLK ib;
	long lenbmp;
	long pos;
	WORD captions[32];
	BYTE *pData, *pMask;
	
	/* Load the file header */
	if (fread(&ifh, sizeof(ifh), 1, fp) < 1) return -1;

	pos = ftell(fp);
	fseek(fp, ifh.ifh_names - ifh.ifh_base, SEEK_SET);
	if (fread(captions, 2, 32, fp) < 32) return -1;
	fseek(fp, pos, SEEK_SET);
	
	for (nic = 0; nic < 72; nic++)
	{
		if (fread(&ib, sizeof(ib), 1, fp) < 1) return -1;

		/* Length of a bitmap. This assumes all icons in the file are 
		 * the same size.
		 */
		lenbmp = ((ib.ib_wicon + 15) / 16) * 2 * ib.ib_hicon;

		pos = ftell(fp);
		pData = (BYTE *)malloc(2*lenbmp);
		if (!pData) return -1;
		pMask = pData + lenbmp;

		/* Load the icon and mask bitmaps */
		fseek(fp, 0x994 + (lenbmp * (long)(ib.ib_pdata)), SEEK_SET);
		fread(pData, 1, lenbmp, fp);
		fseek(fp, 0x994 + (lenbmp * (long)(ib.ib_pmask)), SEEK_SET);
		fread(pMask, 1, lenbmp, fp);

		ib.ib_pdata = pData;
		ib.ib_pmask = pMask;

		if (nic < 8)
		{
			sprintf(eg_capt, "Drive %d", nic + 1);
			ib.ib_ptext = eg_capt;
			ib.ib_char  = (ib.ib_char & 0xFF00) | (nic + 'A');
		}
		else
		{
			char *ptr = eg_capt;
			int ch;
			
			fseek(fp, captions[(nic - 8) % 32] - ifh.ifh_base, SEEK_SET);
			do
			{
				ch = fgetc(fp);
				*ptr = ch;
				++ptr;
			} while (ch != 0 && ch != EOF);
			ib.ib_ptext = eg_capt;
			
		}
	
		if (add_icon(ndoc, &ib) < 0) 
		{
			free(pData);
			return -1;
		}
		free(pData);
		fseek(fp, pos, SEEK_SET);
	}
	
	edicon_doc[ndoc].file_format = 1;	/* ICN */

	measure_docwnd(ndoc);
	assert_sane(ndoc, "Load ICN file");
	
	return 0;
}


typedef struct
{
	LONG	ib_nmask;
	LONG	ib_ndata;
	LONG	ib_ntext;
	UWORD	ib_char;
	WORD	ib_xchar;
	WORD	ib_ychar;
	WORD	ib_xicon;
	WORD	ib_yicon;
	WORD	ib_wicon;
	WORD	ib_hicon;
	WORD	ib_xtext;
	WORD	ib_ytext;
	WORD	ib_wtext;
	WORD	ib_htext;
} FILE_ICON;


WORD save_icn(WORD ndoc, FILE *fp)
{
	ICNFILEHEADER ifh;
	int nic, nbmp,w,h;
	ICONBLK ib;
	FILE_ICON fib;
	long lenbmp;
	WORD bmpcount;
	WORD captions[32];
	static WORD map_data[72], map_mask[72];
	LPBYTE pData, pMask;
	EDICON_DOC *pDoc = &edicon_doc[ndoc];
	LPBYTE ic_capt;
	LPTREE pTree = pDoc->tree_data;
	
	ifh.ifh_base = 0x908;	/* Arbitrary. This is the ViewMAX/2 value. */


	if (pDoc->icon_count != 72)
	{
		rsrc_alert(NOT72, 1);
		return -1;
	}

	for (nic = 0; nic < 72; nic++)
	{
		fmemcpy(&ib, &(pDoc->icon_data[nic]), sizeof(ICONBLK));
		if (!nic)
		{
			w = ib.ib_wicon;
			h = ib.ib_hicon;
		}	
		else
		{
			if (w != ib.ib_wicon || h != ib.ib_hicon)
			{
				rsrc_alert(NOT72, 1);
				return -1;
			}
		}
	}

	bmpcount = 0;
	for (nic = 0; nic < 72; nic++)
	{
		fmemcpy(&ib, &(pDoc->icon_data[nic]), sizeof(ICONBLK));

		/* Length of a bitmap. This assumes all icons in the file are 
		 * the same size.
		 */
		lenbmp = ((ib.ib_wicon + 15) / 16) * 2 * ib.ib_hicon;
		
		ib.ib_ptext = (LPBYTE)(-1);
		/* Find the lowest possible bitmap/mask IDs for this bitmap */

		map_data[nic] = -1;
		map_mask[nic] = -1;
		for (nbmp = 0; nbmp < nic; nbmp++)
		{
			if (map_data[nic] == -1 && 
			    !fmemcmp(pDoc->icon_data[nbmp].ib_pdata, ib.ib_pdata, lenbmp))
			{
				map_data[nic] = map_data[nbmp];
			}
			if (map_mask[nic] == -1 && 
			    !fmemcmp(pDoc->icon_data[nbmp].ib_pmask, ib.ib_pmask, lenbmp))
			{
				map_mask[nic] = map_mask[nbmp];
			}
		}
		if (map_mask[nic] == -1) map_mask[nic] = bmpcount++;
		if (map_data[nic] == -1) map_data[nic] = bmpcount++;

		/* Transform to device-independent before writing */
		
		trans_gimage(vdi_handle, pTree, nic + 1);
		pData = ib.ib_pdata;
		pMask = ib.ib_pmask;

		memcpy(&fib, &ib, sizeof(fib));
		fib.ib_ndata = map_data[nic];
		fib.ib_nmask = map_mask[nic];
		fib.ib_char &= 0xFF00;
		
		/* Write the header back at the very end. For now, concentrate on
		 * writing out the ICONBLKs */
	
		fseek(fp, 4 + (sizeof(ICONBLK) * nic), SEEK_SET);
		if (fwrite(&fib, sizeof(fib), 1, fp) < 1) return -1;

		fseek(fp, 0x994 + (lenbmp * fib.ib_ndata), SEEK_SET);
		lfwrite(pData, 1, lenbmp, fp);
		fseek(fp, 0x994 + (lenbmp * fib.ib_nmask), SEEK_SET);
		lfwrite(pMask, 1, lenbmp, fp);

		/* Transform back to screen format */
		
		trans_gimage(vdi_handle, pTree, nic + 1);
	}
	fseek(fp, 0x994 + (lenbmp * bmpcount), SEEK_SET);

	for (nic = 0; nic < 32; nic++)
	{
		nbmp = 0;
		ic_capt = pDoc->icon_data[nic+8].ib_ptext;
		
		captions[nic] = ftell(fp) + ifh.ifh_base;

		do
		{
			fputc(ic_capt[nbmp], fp);
		} while (ic_capt[nbmp++]);
	}
	ifh.ifh_names = ftell(fp) + ifh.ifh_base;
	fwrite(captions, 1, sizeof(captions), fp);
	fseek(fp, 0, SEEK_SET);
	fwrite(&ifh, 1, sizeof(ifh), fp);

	return 0;
}


static unsigned char sampal[8] = {0,0,0,0,255,255,255,0};

WORD save_bmp_body(FILE *fp, ICONBLK *ib, int nic, EDICON_DOC *pDoc)
{
	WIN31BM bm;
	LPBYTE pico, pmask;
	WORD fw, fh, fwb, fwd;
	WORD x,y,o;
	LONG lenbmp;
	LPTREE pTree = pDoc->tree_data;
	
	/* Size of one of the two bitmaps (dword-aligned) */
	lenbmp = ((ib->ib_wicon + 31) / 32) * 4 * ib->ib_hicon;

	memset(&bm, 0, sizeof(bm));
	bm.bm_srecl  = sizeof(bm) - 8;	/* Don't include bm_pal */
	bm.bm_width = ib->ib_wicon;
	bm.bm_height = 2 * ib->ib_hicon;
	bm.bm_nplanes= 1;
	bm.bm_cdepth = 1;	/* 1 bit */
	bm.bm_compr  = 0;	/* Not compressed */
	bm.bm_dsize  = lenbmp;
	
	fw  = bm.bm_width;					/* Form width */
	fwb = ((bm.bm_width + 15) / 16)*2;	/* Width in GEM's DIB */
	fwd = ((bm.bm_width + 31) / 32)*4;	/* Width in Windows's DIB */
		
		
	memcpy(bm.bm_pal, sampal, 8);
	if (fwrite(&bm, sizeof(bm), 1, fp) < 1) return -1;

	/* Transform icon to device-independent */
	trans_gimage(vdi_handle, pTree, nic + 1);

	fh = ib->ib_hicon;

	/* We now emit the rows for the bitmap. */

	pico  = ib->ib_pdata;
	pmask = ib->ib_pmask;
	
	for (y = fh - 1; y >= 0; y--)
		for (x = 0; x < fwd; x++)
		{
			o = y * fwb + (x^1);
			
			if ((x^1) < fwb) 
			{
				fputc(pico[o] ^ pmask[o], fp);
			}
			else fputc(0, fp);
		}
	/* And the mask */
	
	for (y = fh - 1; y >= 0; y--)
		for (x = 0; x < fwd; x++)
		{
			o = y * fwb + (x^1);

			if ((x^1) < fwb) 
			{
				fputc(~pmask[o], fp);
			}
			else fputc(0, fp);
		}

	/* Transform back to display format */
	trans_gimage(vdi_handle, pTree, nic + 1);
	
	return 0;
}


WORD save_ico(WORD ndoc, FILE *fp)
{
	WIN31IH ih;
	WIN31IR ir;
	int n, ni;
	ICONBLK ib;
	long fpos, lenbmp;
	EDICON_DOC *pDoc = &edicon_doc[ndoc];

	ih.ih_magic = 0;
	ih.ih_typ   = 1;	/* Icons rather than cursors */
	ih.ih_icno  = pDoc->icon_count;
	
	/* Write the file header */
	if (fwrite(&ih, sizeof(ih), 1, fp) < 1) return -1;

	ni = 1;

	/* Address of the first data record */
	fpos = sizeof(ih) + ih.ih_icno * sizeof(ir);
	
	/* Write the icon directory */
	for (n = 0; n < ih.ih_icno; n++)
	{
		fmemcpy(&ib, &(pDoc->icon_data[n]), sizeof(ICONBLK));

		/* Size of one of the two bitmaps (dword-aligned for Windows) */
		lenbmp = ((ib.ib_wicon + 31) / 32) * 4 * ib.ib_hicon;
		
		memset(&ir, 0, sizeof(ir));
		ir.ir_width  = ib.ib_wicon;
		ir.ir_height = ib.ib_hicon;
		ir.ir_cdepth = 2;
		ir.ir_len    = lenbmp + lenbmp + sizeof(WIN31BM);
		ir.ir_offset = fpos;

		fpos += ir.ir_len;
		if (fwrite(&ir, sizeof(ir), 1, fp) < 1) return -1;
	}

	/* Write the icons proper */
	for (n = 0; n < ih.ih_icno; n++)
	{
		fmemcpy(&ib, &(pDoc->icon_data[n]), sizeof(ICONBLK));

		if (save_bmp_body(fp, &ib, n, pDoc)) return -1;
	}
		
	return 0;	
}


char *makename(LPBYTE text)
{
	static char nbuf[14];
	int n;
	
	for (n = 0; n < 13;)
	{
		if (!*text) break;
		
		if (isalpha(*text) || (n && isdigit(*text)))
		{
			nbuf[n] = tolower(*text);
			++n;
		}
		++text;
	}
	nbuf[n] = 0;

	if (n) return nbuf; 
	return "icon";
}


WORD save_rcs(WORD ndoc, FILE *fp)
{
	int img, n, m, p, hw;
	ICONBLK ib;
	long lenbmp;
	EDICON_DOC *pDoc = &edicon_doc[ndoc];
	LPTREE pTree = pDoc->tree_data;
	LPWORD pW;

	if (pDoc->icon_count > 1)
	{
		int rv = rsrc_alert(NOT1, 1);
		if (rv > 1) return -1;
	}
	
	for (n = 0; n < pDoc->icon_count; n++)
	{
		fmemcpy(&ib, &(pDoc->icon_data[n]), sizeof(ICONBLK));

		lenbmp = ((ib.ib_wicon + 15) / 16) * 2 * ib.ib_hicon;

		/* Transform icon to device-independent */
		trans_gimage(vdi_handle, pTree, n + 1);

		for (img = 0; img < 2; img++)
		{
			fprintf(fp, "/* GEM Icon Definition: */\r\n");
			fprintf(fp, "#define ICON_W 0x%04X\r\n", ib.ib_wicon);
			fprintf(fp, "#define ICON_H 0x%04X\r\n", ib.ib_hicon);
			fprintf(fp, "#define DATASIZE 0x%04lX\r\n", (lenbmp+1)/2);
			fprintf(fp, "UWORD %s[DATASIZE] =\r\n", makename(ib.ib_ptext));


			if (img) pW = (LPWORD)(ib.ib_pmask); 
			else	 pW = (LPWORD)(ib.ib_pdata);
		
			hw = (lenbmp + 7) / 8;
			for (m = 0; m < hw; m++)
			{
				if (!m) fprintf(fp, "{ ");
				else	fprintf(fp, "  ");
				for (p = 0; p < 4; p++)
				{
					fprintf(fp, "0x%04X, ", pW[4 * m + p]);
				}
				fprintf(fp, "\r\n");
			}
			fprintf(fp, "};\r\n");
		}
		/* Transform icon back to device-dependent */
		trans_gimage(vdi_handle, pTree, n + 1);
	}
	return 0;	
}




WORD save_iconfile(WORD ndoc, FILE *fp, WORD fmt, char *filename)
{
	int r;
	EDICON_DOC *pDoc = &edicon_doc[ndoc];

	if (fmt == -1) fmt = pDoc->file_format;
	
	switch(fmt)
	{	
		case 1: r = save_icn(ndoc, fp); break;
		case 2: r = save_ico(ndoc, fp); break;
		case 3: r = save_rcs(ndoc, fp); break;
	}	
	if (!r) 
	{
		strcpy(pDoc->title, " ");
		strcat(pDoc->title, filename);
		strcat(pDoc->title, " ");

		pDoc->file_format = fmt;

		/* Filename may have changed. Repaint window. */
		send_redraw(pDoc->hwnd, (GRECT *)&(pDoc->wx));
	}
	return 0;
}


VOID icon_drag(WORD nsw, WORD ndw, WORD nsi, WORD mousex, WORD mousey)
{
	WORD x,y, nv, allow_r;
	WORD ndi, rv;
	char titlebuf[50];
	LPICON sib, dib;
	EDICON_DOC *pSrc  = &edicon_doc[nsw];
	EDICON_DOC *pDest = &edicon_doc[ndw];
	LPTREE      pTree = pSrc->tree_data;
	
	x = mousex - pDest->wx - XBDR;
	y = mousey - pDest->wy - YBDR + pDest->scroll_offs;

	if (x < 0 || y < 0) ndi = -1;
	else ndi = ((y / YSPACE) * pDest->icons_across) + (x / XSPACE);

	allow_r = 1;
	if (ndi < 0 || ndi >= pDest->icon_count) allow_r = 0;
	else
	{
		if (pDest->icon_data[ndi].ib_wicon != 
		     pSrc->icon_data[nsi].ib_wicon)    allow_r = 0;

		if (pDest->icon_data[ndi].ib_hicon != 
		     pSrc->icon_data[nsi].ib_hicon)    allow_r = 0;
	}

	if (!allow_r)
	{
		if (pSrc == pDest)
		{
			nv = menu_popup_strings(mousex,mousey,  2, " Copy icon ", " Cancel ");
			if (nv > 1) nv = -1;
		}
		else 
		{
			nv = menu_popup_strings(mousex, mousey, 3, 
				          " Copy icon ", " Move icon", " Cancel ");

			if (nv > 2) nv = -1;
		}
	}
	else
	{
		sprintf(titlebuf, " Replace \"%s\" ", pDest->icon_data[ndi].ib_ptext);
		/* Dragged onto an existing icon */
		nv = menu_popup_strings(mousex, mousey, 4, 
				          " Copy icon ", " Move icon", titlebuf, " Cancel ");
		if (nv > 3) nv = -1;
	}

	if (nv == 1 || nv == 2)	/* Copy or move */
	{
		ICONBLK ib;
		fmemcpy(&ib, &(pSrc->icon_data[nsi]), sizeof(ICONBLK));

		// Make it into device-independent format for the add_icon()
		trans_gimage(vdi_handle, pTree, nsi+1);
		rv = add_icon(ndw, &ib);
		pTree = pSrc->tree_data;	/* the add_icon() might have changed it */
		trans_gimage(vdi_handle, pTree, nsi+1);
		if (rv >= 0 && nv == 2) delete_icon(nsw, nsi);
		measure_docwnd(ndw);
		send_redraw(pDest->hwnd, (GRECT *)&(pDest->wx));

		if (rv >= 0) pDest->dirty_flag = 1;    		
	}
	if (nv == 3) /* Replace */
	{
		sib = &(pSrc ->icon_data[nsi]);
		dib = &(pDest->icon_data[ndi]);
		
		fmemcpy(dib->ib_pmask,
		        sib->ib_pmask,
		        (sib->ib_wicon * sib->ib_hicon) / 8);

		fmemcpy(dib->ib_pdata,
		        sib->ib_pdata,
		        (sib->ib_wicon * sib->ib_hicon) / 8);

		dib->ib_char  = sib->ib_char;
		dib->ib_xchar = sib->ib_xchar;
		dib->ib_xicon = sib->ib_xicon;
		dib->ib_xtext = sib->ib_xtext;
		dib->ib_ytext = sib->ib_ytext;
		dib->ib_wtext = sib->ib_wtext;
		dib->ib_htext = sib->ib_htext;

		set_title(pDest, ndi, sib->ib_ptext);
		send_redraw(pDest->hwnd, (GRECT *)&(pDest->wx));

		pDest->dirty_flag = 1;    		
	}
	
}

