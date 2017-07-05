/************************************************************************

    SYSFONT v1.00 - system font editor for GEM

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
#define WDW_STYLE1  (NAME | INFO | MOVER | CLOSER | FULLER | SIZER | HSLIDE | LFARROW | RTARROW)
#define WDW_STYLE2  (NAME | INFO | MOVER | CLOSER | FULLER | SIZER | HSLIDE | LFARROW | RTARROW | UPARROW | DNARROW)

static WORD xcascade, ycascade;
MLOCAL VOID get_panes(WORD nw, GRECT *rcl, GRECT *rcr);
WORD hndl_menu(WORD title, WORD item);

WORD create_view     (char *filename, FONTSET *set)		/* "constructor". Returns a handle */
{
	/* Find a blank slot */
	WORD nw, y, n;
	MFDB *srcfdb, *destfdb;

	for (nw = 0; nw < MAXW; nw++) if (!sysfont_view[nw].exists) break;

	if (nw >= MAXW)
	{
		rsrc_alert(ALNOWIN, 1);
		return -1;
	}
	memset(&sysfont_view[nw], 0, sizeof (sysfont_view[nw]));
	sysfont_view[nw].exists = 1;
	sysfont_view[nw].fontset = set;
	sysfont_view[nw].hwnd = -1;


/* Create font bitmap (in device-independent form) and translate it to
 * screen form. Set up FDBs for the 2 bitmaps. */
	srcfdb  = &sysfont_view[nw].memfdb;
	destfdb = &sysfont_view[nw].scrfdb;

	sysfont_view[nw].mark[0] = -1;
	sysfont_view[nw].mark[1] = -1;
	
	y = 0;
	for (n = 0; n < set->nfonts; n++) 
	{
		y += set->font[n].height + 2;
	}
	srcfdb->mp = dos_alloc(y * 512);
	destfdb->mp = ((LPBYTE)srcfdb->mp) + (y * 256);
	if (srcfdb->mp == NULL)
	{
		destroy_view(nw);
		rsrc_alert(ALNOMEM, 1);
		return -1;
	}
	srcfdb->fwp = destfdb->fwp = 2048;	/* Fixed width */
	srcfdb->fh  = destfdb->fh  = y;
	srcfdb->fww = destfdb->fww = 128;
	srcfdb->ff  = 1;
	destfdb->ff = 0;
	srcfdb->np  = destfdb->np = 1;

	rebuild_fdbs(nw);
	if (open_view(nw, filename) < 0)
	{
		destroy_view(nw);
		return -1;
	}
	measure_view(nw);
	topdoc = nw;
	return nw;
}


VOID rebuild_fdbs(WORD nw)
{
	LPWORD wdest;
	MFDB *srcfdb, *destfdb;
	WORD x,y,n,h,w,m;
	LPBYTE str;
	FONTSET *set = sysfont_view[nw].fontset;

	srcfdb  = &sysfont_view[nw].memfdb;
	destfdb = &sysfont_view[nw].scrfdb;

	x = y = 0;
	wdest = (LPWORD)(srcfdb->mp);
/* Transform the font to a standard GEM bitmap */
	for (n = 0; n < set->nfonts; n++) 
	{
		h   = set->font[n].height;
		for (x = 0; x < 128; x++) wdest[y * 128 + x] = 0;
		++y;
		for (m = 0; m < set->font[n].height; m++)
		{
			for (x = 0; x < 128; x++)
			{
				str = (LPBYTE)set->font[n].data + 2 * x * h;
				
				w = str[m] & 0xFF;
				w = w << 8;
				w |= str[m+h] & 0xFF;
				wdest[y * 128 + x] = w;
			} 
			++y;
		}	
		for (x = 0; x < 128; x++) wdest[y * 128 + x] = 0;
		++y;
	}
	vr_trnfm(vdi_handle, srcfdb, destfdb);
}	


VOID destroy_view      (WORD nw)		/* "destructor". */
{
	if (!sysfont_view[nw].exists) return;
	sysfont_view[nw].exists = 0;

	if (sysfont_view[nw].hwnd != -1) close_view(nw, NULL);
	if (sysfont_view[nw].fontset) free_fontset(sysfont_view[nw].fontset);
	if (sysfont_view[nw].memfdb.mp) dos_free(sysfont_view[nw].memfdb.mp);
}


VOID gen_title(WORD nw, char *filename)
{
	char *str;
	int n;

	str = &(sysfont_view[nw].title[0]);
	n = 0;
	str[n] = ' ';
	do 
	{
		++n;
		str[n] = filename[n-1];
	} while (str[n]);	
	str[n++] = ' ';
	str[n] = 0;
        wind_setl(sysfont_view[nw].hwnd, WF_NAME, &sysfont_view[nw].title[0]);
}


VOID gen_info(WORD nw)
{
	char *buf = sysfont_view[nw].info;
	char buf2[80];
	int n;

	buf[0] = 0;
	if (!sysfont_view[nw].fontset) return;
	switch(sysfont_view[nw].fontset->format)
	{
		case 'A': strcpy(buf, "TOS 1.00 ROM image. "); break;
		case 'E': strcpy(buf, "GEM driver. "); break;
		case 'P': strcpy(buf, "PSF (Linux console) font. "); break;
		case 'R': strcpy(buf, "Raw DOS font. "); break;
		case 'W': strcpy(buf, "Windows font. "); break;
		case 'Z': strcpy(buf, "Spectrum +3 font. "); break;
	}
	sprintf(buf2, "Size%s: ", (sysfont_view[nw].fontset->nfonts == 1) ? "" : "s");
	strcat(buf, buf2);
	for (n = 0; n < sysfont_view[nw].fontset->nfonts; n++)
	{
		sprintf(buf2, "%s8x%d", n ? "," : "",
				sysfont_view[nw].fontset->font[n].height);
		strcat(buf, buf2);
	}
        wind_setl(sysfont_view[nw].hwnd, WF_INFO, &sysfont_view[nw].info[0]);
}

WORD open_view         (WORD nw, char *filename)		/* Open window */
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
	if (sysfont_view[nw].fontset->nfonts == 1)
        	sysfont_view[nw].hwnd = hwnd = wind_create(WDW_STYLE1, 
				gl_xfull, gl_yfull, gl_wfull, gl_hfull);
        else	sysfont_view[nw].hwnd = hwnd = wind_create(WDW_STYLE2, 
				gl_xfull, gl_yfull, gl_wfull, gl_hfull);
        if (hwnd == -1)
        {
                /* No Window Available             */
                form_alert(1, string_addr(ALNOWING));
                return(-1);
        }
	gen_title(nw, filename);
	gen_info(nw);


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


VOID close_view        (WORD nw,  GRECT *dummy)
{
	SYSFONT_VIEW *self = &sysfont_view[nw];
        WORD hwnd        = self->hwnd;

        measure_view(nw);

        graf_shrinkbox(gl_wfull/2, gl_hfull/2, 21, 21, self->wx, self->wy, self->ww, self->wh);
        wind_close(hwnd);
        wind_delete(hwnd);

        self->hwnd = -1;
        if (topdoc == nw) topdoc = -1;
        calc_topwnd();
}



VOID measure_view      (WORD nw)
{
	register SYSFONT_VIEW *self = &sysfont_view[nw];
	GRECT rcl, rcr;
	long total_doc, seen_doc, pos;
	WORD ni, scroll;

	/* Set the scrollbar position to that given by self->scroll_xpos,
	 * converting it from characters to scrollbar units */
	get_panes(nw, &rcl, &rcr);

	total_doc = 256;
	seen_doc  = rcr.g_w / 8;
	if (total_doc <= seen_doc) pos = 0L;
	else pos = 1000L * (self->scroll_xpos) / (total_doc - seen_doc);

	wind_get(self->hwnd, WF_HSLIDE, &scroll, &ni, &ni, &ni);

	if (scroll != pos)
	{
		  wind_set(self->hwnd, WF_HSLIDE, pos, 0, 0, 0);
		  send_redraw(self->hwnd, &rcl);
		  send_redraw(self->hwnd, &rcr);
	}
	wind_get(self->hwnd, WF_WXYWH, &(self->wx), &(self->wy),
                                       &(self->ww), &(self->wh));
	self->lx = rcl.g_x;
	self->ly = rcl.g_y;
	self->lw = rcl.g_w;
	self->lh = rcl.g_h;	
	self->rx = rcr.g_x;
	self->ry = rcr.g_y;
	self->rw = rcr.g_w;
	self->rh = rcr.g_h;	
}



/* Pass message to the correct object */

VOID do_message(WORD hwnd, GRECT *area, MSGFUNC f, MSGFUNC f2);

/* Specific handler functions */

VOID ask_close_view    (WORD nw,  GRECT *dummy)		/* Close (ask whether to save first) */
{
        if (sysfont_view[nw].dirty_flag)
        {
                int rv = rsrc_alert(ALDIRTY, 3);

                if (rv == 1)
		{
			WORD otd = topdoc;
			hndl_menu(MENUFILE, MENUSAVE);
			topdoc = otd;		
		}
                else if (rv == 3) return;               /* Cancel */
        }
        destroy_view(nw);
}

MLOCAL VOID get_panes(WORD nw, GRECT *rcl, GRECT *rcr)
{
        GRECT rc;
	WORD lh, lw;

        wind_get(sysfont_view[nw].hwnd, WF_WXYWH, &(rc.g_x), &(rc.g_y),
                                          &(rc.g_w), &(rc.g_h));
        memcpy(rcl, &rc, sizeof(rc));
        memcpy(rcr, &rc, sizeof(rc));

	// Work out the size of rcl. 
	lh = sysfont_view[nw].fontset->font[sysfont_view[nw].scroll_ypos].height;
	lw = 8;

	rcl->g_w = rcl->g_h * lw / lh;
	if (rcl->g_w > 64) rcl->g_w = 64;
	// Make it a multiple of character width (lw) + 1
	rcl->g_w -= (rcl->g_w % lw);
	rcl->g_w++;
	if (rcl->g_w > rc.g_w) rcl->g_w = rc.g_w;

        rcr->g_w -= rcl->g_w;
        rcr->g_x += rcl->g_w;
}


MLOCAL VOID paint_left(WORD nw, GRECT *rcl)
{
	WORD x,y,xs,ys;
	WORD xy[4];
	WORD cellw, cellh;
	GRECT rc;
	byte *data, mask;
	SYSFONT_VIEW *self = &sysfont_view[nw];

	cellw = 8;
	cellh = self->fontset->font[self->scroll_ypos].height;
        vsf_interior(vdi_handle, 1);
        vsf_color(vdi_handle, WHITE);
        rc_grect_to_array(rcl, xy);
//        vr_recfl(vdi_handle, xy);

	xs = rcl->g_w / cellw;
	ys = rcl->g_h / cellh;
	if (ys > xs) ys = xs;

	data = self->fontset->font[self->scroll_ypos].data
	     + (self->cur_char * cellh);

	vsf_color(vdi_handle, BLACK);
	for (x = 0; x <= cellw; x++)
	{
		rc.g_x = rcl->g_x + x * xs;
		rc.g_y = rcl->g_y;
		rc.g_w = 1;
		rc.g_h = cellh * ys;
		rc_grect_to_array(&rc, xy);		
		vr_recfl(vdi_handle, xy);
	}
	for (y = 0; y <= cellh; y++)
	{
		rc.g_x = rcl->g_x;
		rc.g_y = rcl->g_y + y * ys;
		rc.g_w = rcl->g_w;
		rc.g_h = 1;
		rc_grect_to_array(&rc, xy);		
		vr_recfl(vdi_handle, xy);
	}
	rc.g_w = xs - 1;
	rc.g_h = ys - 1;
	for (y = 0; y < cellh; y++)
	{
		mask = 0x80;
		for (x = 0; x < cellw; x++)
		{
			rc.g_x = rcl->g_x + x * xs + 1;
			rc.g_y = rcl->g_y + y * ys + 1;
			rc_grect_to_array(&rc, xy);		
			if (data[0] & mask) vsf_color(vdi_handle, BLACK);
			else	  	    vsf_color(vdi_handle, WHITE);
			vr_recfl(vdi_handle, xy);
			mask = mask >> 1;
		}
		++data;
	}
	rc.g_x = rcl->g_x;
	rc.g_y = rcl->g_y + cellh * ys + 1;
	rc.g_w = rcl->g_w;
	rc.g_h = rcl->g_h - (cellh * ys);
	rc_grect_to_array(&rc, xy);		
	vsf_color(vdi_handle, window_colour);
	vsf_style(vdi_handle, window_shade);
	vsf_interior(vdi_handle, 2);
	vr_recfl(vdi_handle, xy);
}



MLOCAL VOID paint_right(WORD nw, GRECT *rcr)
{
        GRECT rcr2;
        WORD n, xy[8], m0, m1;
        MLOCAL WORD fgbg[] = {WHITE, BLACK};
        MFDB    scrMFDB;
	SYSFONT_VIEW *self = &sysfont_view[nw];
	MFDB	*fdb = &self->scrfdb;

        /* scrMFDB represents the screen */

        scrMFDB.mp  = 0;
        scrMFDB.fwp = work_out[0];
        scrMFDB.fh  = work_out[1];
        scrMFDB.fww = work_out[0] / 16;
        scrMFDB.ff  = 0;

        /* Paint the right pane */

	vsf_color(vdi_handle, window_colour);
	vsf_style(vdi_handle, window_shade);
	vsf_interior(vdi_handle, 2);
        rc_grect_to_array(rcr, xy);
        vr_recfl(vdi_handle, xy);
	memcpy(&rcr2, rcr, sizeof(rcr2));
//	vsf_interior(vdi_handle, 1);

	for (n = 0; n < self->fontset->nfonts; n++) 
	{
		rcr2.g_h = 2 + self->fontset->font[n].height;
		vsf_color(vdi_handle, WHITE);
       		rc_grect_to_array(&rcr2, xy);
       		vr_recfl(vdi_handle, xy);
		if (n == self->scroll_ypos)
		{
			vsf_interior(vdi_handle, 1);
/* Show currently selected character in yellow */

			if (self->mark[0] == -1 && self->mark[1] == -1)
			{
				vsf_color(vdi_handle, select_colour);
				rcr2.g_w = 8;
				rcr2.g_x += 8 * (self->cur_char - self->scroll_xpos);
       		 		rc_grect_to_array(&rcr2, xy);
        			vr_recfl(vdi_handle, xy);
			}
			else
			{
				vsf_color(vdi_handle, select_colour);
				if (self->mark[0] < self->mark[1])
				{
					m0 = self->mark[0];
					m1 = self->mark[1] + 1;		
				}
				else
				{
					m0 = self->mark[1];
					m1 = self->mark[0] + 1;		
				}
				m0 -= self->scroll_xpos; if (m0 < 0) m0 = 0;
				m1 -= self->scroll_xpos; if (m1 < 0) m1 = 0;
				
				rcr2.g_w  = 8 * (m1 - m0);
				rcr2.g_x += 8 * m0;
				if (rcr2.g_w > 0)
				{
       		 			rc_grect_to_array(&rcr2, xy);
        				vr_recfl(vdi_handle, xy);
				}	

			}
			rcr2.g_w = rcr->g_w;
			rcr2.g_x = rcr->g_x;
		}
		rcr2.g_y += rcr2.g_h;
	}

        xy[0] = xy[1] = 0;
        xy[2] = fdb->fwp - 1;
        xy[3] = fdb->fh  - 1;

        xy[4] = rcr->g_x + xy[0];
        xy[5] = rcr->g_y + xy[1];
        xy[6] = rcr->g_x + xy[2];
        xy[7] = rcr->g_y + xy[3];
        xy[0] += 8 * self->scroll_xpos;     /* Add on offset caused by scrolling */
        xy[2] += 8 * self->scroll_xpos;

        if (xy[2] > fdb->fwp - 1)
        {
                xy[2] = fdb->fwp - 1;
                xy[6] = xy[4] + xy[2] - xy[0];
        }
	vrt_cpyfm(vdi_handle, 3, xy, fdb, &scrMFDB, fgbg);

}


MLOCAL VOID paint_rect(WORD nw, GRECT *rect)
{
	GRECT rcl, rcr;
	get_panes(nw, &rcl, &rcr);
        /* Paint the left pane */

	paint_left(nw, &rcl);
	paint_right(nw, &rcr);
}


VOID paint_view        (WORD nw,  GRECT *rect)
{
        GRECT       box;
        WORD        wh   = sysfont_view[nw].hwnd;

        wind_get(wh, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
        while ( box.g_w && box.g_h )
        {   // AES returns zero width and height when no more rectangles 
		if (rc_intersect(rect, &box))
		{
                        set_clip(TRUE, &box);
                        paint_rect(nw, &box);
                        set_clip(FALSE, &box);
		}
		// get next rectangle in window's rectangle list            
		wind_get(wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h); 
	}
}


VOID topped_view	(WORD nw,  GRECT *dummy)
{
        wind_setl(sysfont_view[nw].hwnd, WF_TOP, 0);
        topdoc = nw;
}

VOID newtop_view	(WORD nw,  GRECT *dummy)
{
	if (topdoc == nw) topdoc = -1;
}


VOID resize_view       (WORD nw,  GRECT *area)
{
	WORD hwnd = sysfont_view[nw].hwnd;
	GRECT work;
	// Get working area
        wind_calc(1, WDW_STYLE1, area->g_x, area->g_y, area->g_w, area->g_h,
                                &work.g_x, &work.g_y, &work.g_w, &work.g_h);
        if (work.g_w < work.g_h) work.g_w = work.g_h;

        work.g_x = align_x(work.g_x);   // WORD alignment       
        work.g_w = align_x(work.g_w);   //  for performance     

        // get border area x, y, w, h   
	wind_calc(0, WDW_STYLE1,  work.g_x,    work.g_y,   work.g_w,   work.g_h,
                            &area->g_x, &area->g_y, &area->g_w, &area->g_h);
        // set current x,y,w,h - borders, title bar, (info)      
        wind_set(hwnd, WF_CXYWH, area->g_x, area->g_y, area->g_w, area->g_h);

        measure_view(nw);
        send_redraw(sysfont_view[nw].hwnd, area);
}


VOID move_view		 (WORD nw,  GRECT *area)
{
        rc_constrain(&desk_area, area);

        area->g_x = align_x(area->g_x);
        wind_set(sysfont_view[nw].hwnd, WF_CXYWH, area->g_x,
                        area->g_y, area->g_w, area->g_h);
        measure_view(nw);
}


VOID full_view		 (WORD nw,  GRECT *rect)
{
        hndl_full(sysfont_view[nw].hwnd);
        measure_view(nw);
	redraw_view(nw);
}


VOID redraw_view(WORD nw)
{
        GRECT area;

        area.g_x = sysfont_view[nw].wx;
        area.g_y = sysfont_view[nw].wy;
        area.g_w = sysfont_view[nw].ww;
        area.g_h = sysfont_view[nw].wh;

        send_redraw(sysfont_view[nw].hwnd, &area);
}


VOID slider_view		 (WORD nw,  GRECT *rect)
{
        long total_doc, seen_doc, pos;
        GRECT rcl, rcr;
	SYSFONT_VIEW *self = &sysfont_view[nw];

        get_panes(nw, &rcl, &rcr);

        total_doc = 256;
        seen_doc  = rcr.g_w / 8;
        pos       = rect->g_x;

        if (total_doc <= seen_doc) self->scroll_xpos = 0;
        else self->scroll_xpos = (WORD)(pos * (total_doc - seen_doc) / 1000);

	if (self->cur_char <  self->scroll_xpos) 
		self->cur_char = self->scroll_xpos;
	if (self->cur_char >= (self->scroll_xpos + seen_doc))	
	{
		self->cur_char = self->scroll_xpos + seen_doc - 1;
		if (self->cur_char > 255) self->cur_char = 255;
	}
	measure_view(nw);
}


VOID arrow_view		 (WORD nw,  GRECT *rect)
{
        long total_doc, seen_doc;
        WORD newpos, draw = 0;
        GRECT rcl, rcr;
	SYSFONT_VIEW *self = &sysfont_view[nw];

        get_panes(nw, &rcl, &rcr);
        total_doc = 256;
        seen_doc  = rcr.g_w / 8;

	newpos = self->scroll_xpos;
        switch(rect->g_x)
        {
                case WA_LFPAGE:
                        newpos -= seen_doc;
                        break;
                case WA_RTPAGE:
                        newpos += seen_doc;
                        break;
                case WA_LFLINE:
                        newpos--;
                        break;
                case WA_RTLINE:
                        newpos++;
                        break;
		case WA_UPLINE:
			if (self->scroll_ypos > 0) self->scroll_ypos--;	
			draw = 1;
			break;
		case WA_DNLINE:
			if (self->scroll_ypos < self->fontset->nfonts - 1) 
				self->scroll_ypos++;	
			draw = 1;
			break;
	}
        /* Keep the new scroll position in limits */

        if (newpos < 0)               newpos = 0;
        if (total_doc    <= seen_doc) newpos = 0;
        else if (newpos > total_doc - 1) newpos = total_doc - 1;

	if (self->cur_char <  newpos) 
		self->cur_char = newpos;
	if (self->cur_char >= (newpos + seen_doc))	
	{
		self->cur_char = newpos + seen_doc - 1;
		if (self->cur_char > 255) self->cur_char = 255;
	}
        self->scroll_xpos = newpos;
        measure_view(nw);
	if (draw)
	{
		redraw_view(nw);
	}
}



MLOCAL VOID click_left(WORD nw, GRECT *rcl)
{
	UWORD pxy[4], bbutton, breturn;
	WORD x,y,xs,ys;
	WORD cellw, cellh;
	byte *data, mask, odata;
	SYSFONT_VIEW *self = &sysfont_view[nw];
	WORD	draw;
	GRECT 	rc;
	WORD	xy[4];

	cellw = 8;
	cellh = self->fontset->font[self->scroll_ypos].height;

	xs = rcl->g_w / cellw;
	ys = rcl->g_h / cellh;
	if (ys > xs) ys = xs;

	data = self->fontset->font[self->scroll_ypos].data
	     + (self->cur_char * cellh);

	x = (mousex - rcl->g_x) / xs;
	y = (mousey - rcl->g_y) / ys;

	mask = 0x80 >> x;
	draw = (data[y] & mask) ? 0 : 1;
	
	rc.g_w = xs - 1;
	rc.g_h = ys - 1;
        vsf_interior(vdi_handle, 1);
	while(1)	
	{
		if (x >= 0 && x <= 8 && y >= 0 && y < self->fontset->font[self->scroll_ypos].height)
		{
			odata = data[y];
			if   (draw) data[y] |= mask;
			else        data[y] &= ~mask;
			// Update the cell we just did.

			if (odata != data[y])
			{	
				rc.g_x = rcl->g_x + x * xs + 1;
				rc.g_y = rcl->g_y + y * ys + 1;
				rc_grect_to_array(&rc, xy);		
				graf_mouse(M_OFF, 0x0L);	
				if (draw) vsf_color(vdi_handle, BLACK);
				else 	  vsf_color(vdi_handle, WHITE);
				vr_recfl(vdi_handle, xy);
				graf_mouse(M_ON, 0x0L);
			}
		}
                pxy[0] = rcl->g_x + x * xs;
                pxy[1] = rcl->g_y + y * ys;

                ev_which = evnt_multi(MU_BUTTON | MU_M1,
                                        0x01, 0x01, 0x00, /* 1 click, 1 button, button up */
                                        1, pxy[0], pxy[1], xs, ys, 
                                        0, 0, 0, 0, 0,
                                        ad_rmsg, 125, 0,
                                        &mousex, &mousey, &bbutton, &kstate,
                                        &kreturn, &breturn);
		if (ev_which & MU_BUTTON) break;	// button up
		// Must have been a mouse move
		x = (mousex - rcl->g_x) / xs;
		y = (mousey - rcl->g_y) / ys;

		mask = 0x80 >> x;
	}
	sysfont_view[nw].dirty_flag = 1;
	rebuild_fdbs(nw);
	rc.g_x = rcl->g_x + rcl->g_w + 8 * (self->cur_char - self->scroll_xpos);
	rc.g_y = rcl->g_y;
	rc.g_w = 8;
	rc.g_h = rcl->g_h;
	send_redraw(self->hwnd, &rc);
}	


VOID click_view(WORD nw)
{
        GRECT rcl, rcr;
	SYSFONT_VIEW *self = &sysfont_view[nw];
	WORD n;

        get_panes(nw, &rcl, &rcr);

	/* Click on left rect. Paint pixels until mouse button goes up. */
	if (mousex < rcr.g_x)
	{
		click_left(nw, &rcl);
	}
	/* Click on right rect. See if it's a click or a drag.
	 * If it's a click: Activate the target character. 
	 * If it's a drag: Select the required character range. */
        else
        {
                UWORD pxy[4], bbutton, breturn;

                pxy[0] = mousex;
                pxy[1] = mousey;
                pxy[2] = mousex - rcr.g_x;
                pxy[3] = mousey - rcr.g_y;
		for (n = 0; n < self->fontset->nfonts; n++)
		{
			if (pxy[3] < self->fontset->font[n].height)
			{
				self->cur_char = self->scroll_xpos + (pxy[2] / 8);
				if (self->cur_char > 255) self->cur_char = 255;
				self->scroll_ypos = n;
				graf_mouse(M_OFF, NULL);
                        	set_clip(TRUE, &rcr);
				paint_right(nw, &rcr);
                        	set_clip(FALSE, &rcr);
				graf_mouse(M_ON, NULL);
 				send_redraw(self->hwnd, &rcl);
				break;
			}
			else pxy[3] -= self->fontset->font[n].height;
		}
			

                /* See which happens first - mouse up or mouse move */

                ev_which = evnt_multi(MU_BUTTON | MU_M1,
                                        0x01, 0x01, 0x00, /* 1 click, 1 button, */
                                        1, pxy[0], pxy[1], 1, 1,
                                        0, 0, 0, 0, 0,
                                        ad_rmsg, 125, 0,
                                        &pxy[2], &pxy[3], &bbutton, &kstate,
                                        &kreturn, &breturn);

                /* Button up. Select a new character*/
                if (ev_which & MU_BUTTON)
                {
			self->mark[0] = -1;
			self->mark[1] = -1;
			send_redraw(self->hwnd, &rcr);
		}
		else 
		{
			self->mark[0] = self->scroll_xpos + ((pxy[2] - rcr.g_x) / 8);
			while(1)
			{
         	      	 	pxy[0] = pxy[2]; 
              			pxy[1] = pxy[3];
				/* Drag-select. */
       			        ev_which = evnt_multi(MU_BUTTON | MU_M1,
                                        0x01, 0x01, 0x00, /* 1 click, 1 button, */
                                        1, pxy[0], pxy[1], 1, 1,
                                        0, 0, 0, 0, 0,
                                        ad_rmsg, 125, 0,
                                        &pxy[2], &pxy[3], &bbutton, &kstate,
                                        &kreturn, &breturn);
				/* End drag */
				if (ev_which & MU_BUTTON)
				{
					send_redraw(self->hwnd, &rcr);
					measure_view(nw);
					return;
				}
/* Auto-scroll if the drag moves outside rcr to left or right. */
				if (pxy[2] < rcr.g_x && self->scroll_xpos > 0) --self->scroll_xpos;
				if (pxy[2] >= rcr.g_x + rcr.g_w && self->scroll_xpos < 255) ++self->scroll_xpos;
				/* Show updated drag. This gets rather 
				 * flickery but I can't really help it. */
				self->mark[1] = self->scroll_xpos + ((pxy[2] - rcr.g_x) / 8);
				graf_mouse(M_OFF, NULL);
                        	set_clip(TRUE, &rcr);
				paint_right(nw, &rcr);
                        	set_clip(FALSE, &rcr);
				graf_mouse(M_ON, NULL);
			}

		}
	}	
}





VOID do_action(WORD nw, ACTION action)
{
	SYSFONT_VIEW *self = &sysfont_view[nw];
	WORD m0, m1, x, h;

	if (self->mark[0] >= 0 && self->mark[1] >= 0)
	{
		m0 = self->mark[0];
		m1 = self->mark[1];
		if (m0 >= m1) { m0 = self->mark[1]; m1 = self->mark[0]; }
	}
	else
	{
		m0 = m1 = self->cur_char;
	}
	h = self->fontset->font[self->scroll_ypos].height;
	for (x = m0; x <= m1; x++)
	{
		(*action)(&self->fontset->font[self->scroll_ypos], x);
	}
	sysfont_view[nw].dirty_flag = 1;
	rebuild_fdbs(nw);
	send_redraw(self->hwnd, (GRECT *)&self->wx);
}


VOID act_cut(FONT *f, WORD ch)
{
	WORD offset = ch * f->height;
	memset(f->data + offset, 0, f->height);
}

VOID do_cut(WORD nw)
{
	do_action(nw, act_cut);
}


VOID do_copy(WORD nw)
{
	SYSFONT_VIEW *self = &sysfont_view[nw];
	WORD m0, m1, x, y;

	if (self->mark[0] >= 0 && self->mark[1] >= 0)
	{
		m0 = self->mark[0];
		m1 = self->mark[1];
		if (m0 >= m1) { m0 = self->mark[1]; m1 = self->mark[0]; }
	}
	else
	{
		m0 = m1 = self->cur_char;
	}
	if (clipboard) free(clipboard);
	clip_h = self->fontset->font[self->scroll_ypos].height;
	clip_w = (m1 + 1) - m0;
	clipboard = malloc(clip_h * clip_w);
	if (!clipboard) 
	{
		rsrc_alert(ALNOMEM, 1);
		return;
	}
	for (x = 0; x < clip_w; x++)
	{
		for (y = 0; y < clip_h; y++)
		{
			clipboard[x * clip_h + y] = 
				self->fontset->font[self->scroll_ypos].data
				[(x+m0) * clip_h + y];
		}	
	}
}



VOID do_paste(WORD nw)
{
	SYSFONT_VIEW *self = &sysfont_view[nw];
	WORD m0, m1, x, y, h;

	if (!clipboard) return;
	if (self->mark[0] >= 0 && self->mark[1] >= 0)
	{
		m0 = self->mark[0];
		m1 = self->mark[1];
		if (m0 >= m1) { m0 = self->mark[1]; m1 = self->mark[0]; }
	}
	else
	{
		m0 = self->cur_char;
		m1 = m0 + clip_w - 1;
		if (m1 > 255) m1 = 255;
	}
	h = self->fontset->font[self->scroll_ypos].height;
	for (x = 0; x < clip_w; x++)
	{
		if ((x + m0) > m1) break;
		for (y = 0; y < h; y++)
		{
			WORD offset = ((m0 + x) * h) + y;
			if (y >= clip_h) self->fontset->font[self->scroll_ypos].data [offset] = 0;
			else		 self->fontset->font[self->scroll_ypos].data [offset] = clipboard[x * clip_h + y];
		}
	}
	sysfont_view[nw].dirty_flag = 1;
	rebuild_fdbs(nw);
	send_redraw(self->hwnd, (GRECT *)&self->wx);
}


VOID act_reflecth(FONT *f, WORD ch)
{
	WORD n;
	byte b, m1, m2;
	WORD offset = ch * f->height;

	for (n = 0; n < f->height; n++)
	{
		b = 0;
		m1 = 0x80;
		m2 = 0x01;
		do	
		{
			if (f->data[offset+n] & m1) b |= m2;
			m1 = m1 >> 1;
			m2 = m2 << 1;
		} while (m1 != 0);
		f->data[offset+n] = b;
	}
}



VOID act_reflectv(FONT *f, WORD ch)
{
	WORD n, y;
	WORD offset = ch * f->height;
	byte b, *p, *q;

	y = f->height / 2;
	p = f->data + offset;
	q = p + f->height - 1;
	for (n = 0; n < y; n++)
	{
		b = *p; *p = *q; *q = b;
		++p;
		--q;
	}
}


VOID act_rotatec(FONT *f, WORD ch)
{
	WORD offset = ch * f->height;
	byte nc[8], m1, m2;
	WORD x,y;

	memset(nc, 0, sizeof(nc));
	if (f->height != 8)
	{
		act_reflectv(f, ch);
		act_reflecth(f, ch);
		return;
	}
	m1 = 1;
	for (y = 0; y < 8; y++)
	{
		m2 = 0x80;
		for (x = 0; x < 8; x++)
		{
			if (f->data[offset+y] & m2) nc[x] |= m1;
			m2 = m2 >> 1;
		}
		m1 = m1 << 1;
	} 	
	memcpy(f->data + offset, nc, 8);
}



VOID act_rotatea(FONT *f, WORD ch)
{
	WORD offset = ch * f->height;
	byte nc[8], m1, m2;
	WORD x,y;

	memset(nc, 0, sizeof(nc));
	if (f->height != 8)
	{
		act_reflectv(f, ch);
		act_reflecth(f, ch);
		return;
	}
	m1 = 0x80;
	for (y = 0; y < 8; y++)
	{
		m2 = 0x01;
		for (x = 0; x < 8; x++)
		{
			if (f->data[offset+y] & m2) nc[x] |= m1;
			m2 = m2 << 1;
		}
		m1 = m1 >> 1;
	} 	
	memcpy(f->data + offset, nc, 8);
}

VOID act_moveu(FONT *f, WORD ch)
{
	WORD n;
	WORD offset = ch * f->height;

	for (n = 1; n < f->height; n++)
	{
		f->data[offset+n-1] = f->data[offset+n];
	}
	f->data[offset+n-1] = 0;
}



VOID act_moved(FONT *f, WORD ch)
{
	WORD n;
	WORD offset = ch * f->height;

	for (n = f->height - 1; n > 0; n--)
	{
		f->data[offset+n] = f->data[offset+n-1];
	}
	f->data[offset] = 0;
}




VOID act_movel(FONT *f, WORD ch)
{
	WORD n;
	WORD offset = ch * f->height;

	for (n = 0; n < f->height; n++)
	{
		f->data[offset+n] = f->data[offset+n] << 1;
	}
}




VOID act_mover(FONT *f, WORD ch)
{
	WORD n;
	WORD offset = ch * f->height;

	for (n = 0; n < f->height; n++)
	{
		f->data[offset+n] = (f->data[offset+n] >> 1) & 0x7F;
	}
}





VOID act_invert(FONT *f, WORD ch)
{
	WORD n;
	WORD offset = ch * f->height;

	for (n = 0; n < f->height; n++)
	{
		f->data[offset+n] ^= 0xFF;
	}
}





VOID act_bold(FONT *f, WORD ch)
{
	WORD n;
	WORD offset = ch * f->height;

	for (n = 0; n < f->height; n++)
	{
		f->data[offset+n] |= f->data[offset+n] >> 1;
	}
}






VOID act_underline(FONT *f, WORD ch)
{
	WORD n;
	WORD offset = ch * f->height;
	WORD offse2 = '_' * f->height;

	for (n = 0; n < f->height; n++)
	{
		f->data[offset+n] |= f->data[offse2 + n];
	}
}






VOID act_italic(FONT *f, WORD ch)
{
	WORD n;
	WORD offset = ch * f->height;
	float ds;	
	WORD ishift;

	ds = 6.0 / f->height;

	for (n = 0; n < f->height; n++)
	{
		ishift = (WORD)(ds * n);
		switch(ishift)
		{
			case 0:  f->data[offset+n] = f->data[offset + n] >> 2; break;
			case 1:  f->data[offset+n] = f->data[offset + n] >> 1; break;
			case 3:  f->data[offset+n] = f->data[offset + n] << 1; break;
			case 4:  f->data[offset+n] = f->data[offset + n] << 2; break;
		}
	}
}






