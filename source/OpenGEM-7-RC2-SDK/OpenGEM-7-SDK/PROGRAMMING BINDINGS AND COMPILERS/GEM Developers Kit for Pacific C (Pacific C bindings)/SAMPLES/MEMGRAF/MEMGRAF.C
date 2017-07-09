/******************************************************************************
*
*  GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*  MEMGRAF.C - Simple dynamic graphing program for X/GEM on FlexOS
*
*  Date: 08/01/89
*
*******************************************************************************
Copyright (c) 1985,1989  Digital Research Inc. 
All rights reserved. The Software Code contained in this listing is copyrighted
and may be used and copied only under terms of the Digital  Research  Inc.  End
User License Agreement.  This code may be used only by the registered user, and
may not be resold or transfered  without the consent of Digital  Research  Inc.
Unauthorized reproduction,  transfer, or use of this material may be a criminal
offense under Federal and/or State law.
                     U.S. GOVERNMENT RESTRICTED RIGHTS
This software product is provided with RESTRICTED RIGHTS.  Use, duplication  or
disclosure  by the Government  is subject  to restrictions  as set forth in FAR
52.227-19 (c) (2) (June, 1987) when applicable  or the applicable provisions of
the DOD FAR supplement  252.227-7013  subdivision  (b) (3) (ii) (May,  1981) or
subdivision  (c)  (1) (ii)  (May,  1987).   Contractor/manufacturer  is Digital
Research Inc. / 70 Garden Court / BOX DRI / Monterey, CA 93940.
******************************************************************************
******************************************************************************/

#include "portab.h"                 /* portable coding conv   */

#include "aes.h"
#include "vdi.h"
#include "rclib.h"
#include <stdio.h>
#include <string.h>
#include <dos.h>

#if FLEXOS
#ifdef DESKACC
#undef DESKACC                         /* no desk accessories under FlexOS    */
#endif

#include "flexif.h"
#endif

#define SQUARED   0
#define SOLID     1
#define DOT       3
#define DESK      0
#define MAX_PTS  64

#define INDENT    2
                                       /* border window create flag           */
WORD   gl_wflags = NAME | CLOSER | MOVER | SIZER ;

WORD gl_apid;                          /* application id                      */
WORD gl_accid;                         /* accessory id                        */
WORD gl_wh = -1;                       /* border window handle                */
WORD gl_vh;                            /* vdi handle                          */
#if FLEXOS
BYTE gl_title[80] = "FlexOS Memory In Use";
#else
BYTE gl_title[80] = "DOS Memory In Use";
#endif
BYTE *percent_str[3] = { " 75 ", " 50 ", " 25 " };
WORD	is_modified = 0;
GRECT gl_wdw;
WORD cboxw, cboxh;
/*
 * data vars
 */
#define MAX_D_VALS 512

LONG d_vals[MAX_D_VALS];               /* where each data point is stored     */
LONG d_max_val = 0;                    /* max data val (for normalization)    */
WORD d_cur = -1;                       /* current end of valid data           */
WORD d_count = 0;                      /* count of values to be displayed     */
WORD d_shift = 0;                      /* amount data was last shifted        */

/* graph vars */
WORD bar_space = 5;                    /* space between each data line        */
WORD bar_thick = 1;                    /* thickness of data line              */
WORD bar_color = BLACK;                /* bar color                           */
WORD graph_color = BLACK;              /* draw graph reference lines          */
WORD bkg_color = WHITE;                /* background color                    */

WORD prf_secs_p_smpl = 1;              /* seconds to wait between sampling    */
BOOLEAN prf_area_chart = TRUE;         /* TRUE use v_fillarea else use v_pline*/
BOOLEAN prf_graph = TRUE;              /* TRUE means draw 25%, 50% & 75% lines*/
GRECT last_close;                      /* rectangle of last window-position   */

#ifdef __PACIFIC__
#define MSDOS 1
#endif

/*
 * checks range of d_cur var
 */
VOID check_d_cur()
{
   WORD target;
//   BYTE s[200];

   if( d_cur > d_count )
   {
      target  = d_count / 2;
      d_shift = d_cur - target;
      memcpy( &d_vals[0], &d_vals[d_shift], d_shift * sizeof(*d_vals));
      d_cur   = target;
   }
}

/*
 * captures a data item for the graph
 */
VOID data_capture()
{
   struct {
      LONG m_free;
      LONG m_total;
      LONG m_system;
   } mm;

#if FLEXOS
   d_shift = 0;
   s_get( T_MEM, 0L, (BYTE FAR*)&mm, (LONG)sizeof(mm) );
   d_cur++;
   check_d_cur();
   d_vals[d_cur] = mm.m_total - mm.m_free;   /* amount of mem in use */
#endif
#if MSDOS
   d_shift = 0;
   mm.m_total = 655360l;
   d_cur++;
   check_d_cur();
   d_vals[d_cur] = d_max_val - dos_avail();


   // Stop compiler warnings in Pacific
   mm.m_free   = 0;
   mm.m_system = 0;
   
#endif

   if((is_modified == 0) || (d_max_val != mm.m_total))
   {
      d_max_val = mm.m_total;
#if FLEXOS
      sprintf(gl_title,"FlexOS memory in use (%% of %ld)",d_max_val);	
#endif
#if MSDOS
#if MSC_C && DESKACC
      strcpy(gl_title, "DOS Memory in Use");
#else
      sprintf(gl_title,"DOS Memory in Use (%% of %ld)",d_max_val);
#endif
#endif
      wind_set(gl_wh, WF_NAME, FPOFF(gl_title), FPSEG(gl_title), 0, 0);
      is_modified = 1;
   }
}

/*
 * displays the accumulated data
 */
VOID data_display(redraw)
BOOLEAN redraw;
{
   WORD dd, ii, ist, xlast, ylast, x_incr, ybase;
   WORD xy[2*(3+MAX_PTS)];

   check_d_cur();
   if( d_shift )
      redraw = TRUE;

   vswr_mode(gl_vh, 2);

   if( redraw )
   {
      vswr_mode(gl_vh, 1);
      xy[0] = gl_wdw.g_x;
      xy[1] = gl_wdw.g_y;
      xy[2] = gl_wdw.g_x + gl_wdw.g_w - 1;
      xy[3] = gl_wdw.g_y + gl_wdw.g_h - 1;
      vsf_interior(gl_vh, 3);
      vsf_color(gl_vh, bkg_color);
      vr_recfl(gl_vh, xy);
      vswr_mode(gl_vh, 2);

      if( prf_graph )
      {
         vsl_color(gl_vh, graph_color);
         vsl_width(gl_vh, 1);
         vsl_type(gl_vh, DOT);
         xy[0] = gl_wdw.g_x + (cboxw * INDENT);
         xy[2] = gl_wdw.g_x + gl_wdw.g_w - 1;
         for(ii = 1; ii < 4; ii++)
         {
            xy[1] = xy[3] = gl_wdw.g_y+((ii * gl_wdw.g_h) / 4);
            v_pline(gl_vh,2,xy);
            v_gtext(gl_vh, gl_wdw.g_x, gl_wdw.g_y + ((ii * gl_wdw.g_h) / 4),
                                                           percent_str[ii - 1]);
         }
      }
   }
   if (!prf_area_chart)
   {
       vsl_color(gl_vh, bar_color);
       vsl_width(gl_vh, bar_thick);
       vsl_type(gl_vh, SOLID);
       vsl_ends(gl_vh, SQUARED, SQUARED);
   }
   else
       vsf_color(gl_vh, bar_color);

   xy[0] = gl_wdw.g_x + (cboxw * INDENT);
   ist = 1;
   x_incr = bar_space;
   ybase = gl_wdw.g_y + gl_wdw.g_h;
   for( dd=0; dd<=d_cur; )
   {
      for( ii=ist; dd<=d_cur && ii<2*MAX_PTS; ++ii, ++dd )
      {
         xy[ii++] = ybase - ((gl_wdw.g_h * d_vals[dd]) / d_max_val);
         xy[ii] = xy[ii-2] + x_incr;
      }
      xlast = xy[ii-3];
      ylast = xy[ii-2];
      if (!prf_area_chart)
      {
         v_pline(gl_vh,ii/2,xy);
      }
      else
      {
         xy[ii-1] = xy[ii-3];
         xy[ii] = ybase;
         xy[ii+1] = xy[0];
         xy[ii+2] = ybase;
         xy[ii+3] = xy[0];
         xy[ii+4] = xy[1];
         v_fillarea(gl_vh,ii/2+3,xy);
      }
      xy[0] = xlast;
      xy[1] = ylast;
      xy[2] = xy[0] + x_incr;
      ist = 3;
   }

   vswr_mode(gl_vh, 1);
}

VOID do_redraw(redraw)
BOOLEAN redraw;
{
//   WORD a;
   GRECT rect, box;
   WORD xy[4];

#if MSDOS
   wind_update(BEG_UPDATE);
   graf_mouse(M_OFF, 0l);
#endif

   wind_get(gl_wh, WF_WXYWH, &rect.g_x, &rect.g_y, &rect.g_w, &rect.g_h);
   wind_get(gl_wh, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
   while (box.g_w || box.g_h)
   {
      if (rc_intersect(&rect, &box))
      {
         xy[0] = box.g_x;
         xy[1] = box.g_y;
         xy[2] = box.g_x + box.g_w - 1;
         xy[3] = box.g_y + box.g_h - 1;
         vs_clip(gl_vh, TRUE, xy);
         data_display(redraw);
      }

      wind_get(gl_wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
   }

   vs_clip(gl_vh, FALSE, xy);

#if MSDOS
   graf_mouse(M_ON, 0l);
   wind_update(END_UPDATE);
#endif
}

/*
 * moves or resizes window
 */
VOID change_window(wdw)
GRECT *wdw;
{
   wind_set(gl_wh, WF_CXYWH, wdw->g_x, wdw->g_y, wdw->g_w, wdw->g_h);
   wind_get(gl_wh, WF_WXYWH, &gl_wdw.g_x, &gl_wdw.g_y,&gl_wdw.g_w, &gl_wdw.g_h);
   d_count = (gl_wdw.g_w - (INDENT * cboxw)) / bar_space;
   if (d_count > MAX_D_VALS )
      d_count = MAX_D_VALS;
   do_redraw(TRUE);
}

BOOLEAN open_window()
{
   graf_mouse(M_OFF, 0l);

   gl_wh = wind_create(gl_wflags, 0, 0, 0, 0);

   wind_set(gl_wh, WF_NAME, FPOFF(gl_title), FPSEG(gl_title), 0, 0);

   wind_get(DESK, WF_WXYWH, &gl_wdw.g_x, &gl_wdw.g_y,&gl_wdw.g_w, &gl_wdw.g_h);

   if (last_close.g_x < 0)
      wind_open(gl_wh, gl_wdw.g_x, gl_wdw.g_y,  gl_wdw.g_w/4, 2*gl_wdw.g_h/3);
   else
      wind_open(gl_wh, last_close.g_x, last_close.g_y,
                       last_close.g_w, last_close.g_h);

   wind_get(gl_wh, WF_WXYWH, &gl_wdw.g_x, &gl_wdw.g_y,&gl_wdw.g_w, &gl_wdw.g_h);
   
   d_count = gl_wdw.g_w / bar_space;
   if (d_count > MAX_D_VALS)
      d_count = MAX_D_VALS;

   graf_mouse(M_ON, 0l);
}

BOOLEAN initialize_app()
{
   WORD work_in[11], work_out[57];
   WORD ii;

   gl_apid = appl_init(NULL);

   gl_vh = graf_handle( &ii, &ii, &cboxw, &cboxh );   /* get AES vdi handle */
   for( ii=0; ii<10; ++ii )
      work_in[ii] = 1;                                /* use defaults       */
   work_in[10] = 2;                                   /* raster coords      */
   v_opnvwk(work_in, &gl_vh, work_out);               /* get our vdi handle */

   last_close.g_x = -1;

#if !DESKACC
   return (open_window());
#else
   gl_accid = menu_register(gl_apid, "  MemGraf");
   return (TRUE);
#endif
}

#if! DESKACC
VOID exit_app()
{
   graf_mouse(M_OFF, 0l);
   wind_close(gl_wh);
   wind_delete(gl_wh);
   graf_mouse(M_ON, 0l);
   v_clsvwk(gl_vh);
   appl_exit();
}
#endif

/*
 * main application entry point
 */
main()
{
    WORD   ret, msg[8], done;
    MEVENT   e;

   if ( initialize_app() )
   {
      done = FALSE;
      while(!done)
      {
         e.e_flags = (gl_wh > 0) ? (MU_MESAG | MU_TIMER) : (MU_MESAG);
         e.e_mepbuf = msg;
         e.e_time = prf_secs_p_smpl * 1000L;

         ret = evnt_event(&e);
         if ((ret & MU_TIMER) && gl_wh > 0)
         {
            data_capture();
            do_redraw(FALSE);
         }

         if (ret & MU_MESAG)
         {
            switch(msg[0])
            {
#if DESKACC
               case AC_OPEN:
                  if (msg[4] != gl_accid)
                     break;
                  if (gl_wh > 0)
                     wind_set(gl_wh, WF_TOP, gl_wh, 0, 0, 0);
                  else
                     open_window();
                  break;

                case AC_CLOSE:
                  if (msg[3] == gl_accid && gl_wh > 0)
                  {
                    wind_get(gl_wh, WF_CXYWH, &last_close.g_x, &last_close.g_y,
                                              &last_close.g_w, &last_close.g_h);
                    gl_wh = -1;
                  }
                  break;

               case WM_CLOSED:
                  if (msg[3] != gl_wh || gl_wh <= 0)
                     break;
                  graf_mouse(M_OFF, 0l);
                  wind_get(gl_wh, WF_CXYWH, &last_close.g_x, &last_close.g_y,
                                            &last_close.g_w, &last_close.g_h);
                  wind_close(gl_wh);
                  wind_delete(gl_wh);
                  graf_mouse(M_ON, 0l);
                  gl_wh = -1;
                  break;
#endif
               case WM_TOPPED:
                  wind_set(gl_wh, WF_TOP, gl_wh, 0, 0, 0);
                  break;

               case WM_REDRAW:
                  if (gl_wh > 0)
                     do_redraw(TRUE);
                  break;

               case WM_MOVED:
               case WM_SIZED:
                  if (gl_wh > 0)
                  {
                     change_window((GRECT *)&msg[4]);
                     do_redraw(TRUE);
                  }
                  break;
#if !DESKACC
               case WM_CLOSED:
                  done = TRUE;
                  break;
#endif
            } /* end switch */
         } /* end if (ret & MU_MESAG) */
#if DESKACC
         done = FALSE;
#endif
      } /* while */

#if !DESKACC
      exit_app();
#endif
   } /* if initialize_app */
}
