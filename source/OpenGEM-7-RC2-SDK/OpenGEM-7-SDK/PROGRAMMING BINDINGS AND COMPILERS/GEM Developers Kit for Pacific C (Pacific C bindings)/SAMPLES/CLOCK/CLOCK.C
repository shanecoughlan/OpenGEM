/*******************************************************************************
*  CLOCK.C - X/GEM analog clock
*
*  GEM/3 Programmers Toolkit - Digital Research, Inc.
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
*******************************************************************************
*******************************************************************************/

#include "ppdgem.h"				/* portable coding conv	*/

#include <dos.h>	/* For fp_off & fp_seg */
#include <time.h>	/* For time() */
#include <stdio.h>	/* For sprintf */

#ifdef __PACIFIC__
#define MSDOS 1
#endif

#define DESK      0
#define CENTER    1
#define HALF_LINE 1
#define UNDEFINED -1

#define PI 3.14159

#define N_CLOCK_STRS 13

/*
 * border window create flag
 */
WORD     gl_wflags = NAME | CLOSER | MOVER | SIZER ;
WORD     gl_accid;                  /* Accessory id */
WORD     gl_apid;                   /* Application id */

WORD     gl_wh = -1;                /* border window handle      */
WORD     gl_vh;                     /* vdi handle */
BYTE    *gl_title = "Analog Clock";
GRECT    gl_wdw;                    /* xywh of working area of window */

WORD     systxt_height;             /* height of system text */
WORD     systxt_width;              /* width of system text */
WORD     pxl_width;                 /* pixel width in microns */
WORD     pxl_height;                /* pixel height in microns */

WORD     clk_x,clk_y;               /* middle of clock */
WORD     clk_radius;                /* radius of clock circle */
WORD     clk_bkg_col;               /* background color */
WORD     clk_fill_col;              /* internal fill color */
UWORD    clk_fill_typ;              /* internal fill type */
UWORD    clk_fill_sty;              /* internal fill style */
UWORD    clk_peri_wid;              /* perimeter thickness */
UWORD    clk_peri_col;              /* perimeter color */
UWORD    clk_hand_col;              /* color of hour, min, sec hands */
WORD     clk_hour_len;              /* hour hand length */
WORD     clk_hour_wid;              /* hour hand width */
WORD     clk_min_len;               /* minute hand length */
WORD     clk_min_wid;               /* minute hand width */
WORD     clk_sec_len;               /* second hand length */
WORD     clk_sec_wid;               /* second hand width */
UWORD    clk_txt_col;               /* color of numeric text */
WORD     clk_txt_size;              /* size of numeric text */
WORD     clk_old_hour;              /* hour currently displayed on clock */
WORD     clk_old_min;               /* minute ... */
WORD     clk_old_sec;               /* second ... */
WORD     prf_secs_p_update = 1;     /* how often should we update the clock */

BYTE    *clock_strs[N_CLOCK_STRS] = { 

#if FLEXOS
   "X/GEM",
#endif
#if MSDOS
   "GEM/3",
#endif
   "12", "1", "2", "3", "4", "5",
   "6", "7", "8", "9", "10", "11"

};

WORD clock_s_x[N_CLOCK_STRS], clock_s_y[N_CLOCK_STRS];

#if MSDOS

BYTE    *wday_strs[] = {

   "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"

};

BYTE    *month_strs[] = {

   "",
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"

};

#endif

/**** clock_draw() routines ****/
/*--------------------------------------------------------------------------*/
/* calc_hand() - computes offset of where the (hour/min/sec) hand should be */
/*        Note - the basic algorithm is included here in the optimized form */
/*               to give a hint as to where we're headed.  Below this       */
/*               algorithm, we include a version that does not use floating */
/*               point.                                                     */
/*--------------------------------------------------------------------------*/

#if 000

/*
 * calc_hand() - the basic definition
 */

VOID calc_hand(minit, len, *x_offset, *y_offset)
WORD minit;
WORD len;
WORD *x_offset;
WORD *y_offset;
{
   FLOAT radians, cos(), sin();

   while( minit < 0 )
      minit += 60;
   while( minit >= 60 )
      minit -= 60;
   minit = 15-minit;
   if( minit<0 )
      minit+=60.;
   radians = (((float)minit)/60.) * 2. * PI;
   *x_offset = (int) ((float)len * cos(radians));
   *y_offset = -((int) ((float)len * sin(radians)));
/*
 * Note: negate y_offset to compensate for origin in top left corner
 */
}

#endif
/*---------------------------------------------------------------------*/
/* calc_hand() - the optimized version                                 */
/* calc_init() - precompute array for fixed point calcs by calc_hand() */
/*   Note: this routine writes out arrays which need to be included    */
/*         back into the source.                                       */
/*---------------------------------------------------------------------*/
#define FIXPOINT 256

#if 000

VOID calc_init()
{
   FLOAT rads, sin(), cos();
   WORD ii, minit, result;

   printf("int icos[60] = {\n\t");
   for( ii=0, minit=15; ii<60; ++ii,--minit )
   {
      rads = (((float)minit)/30.) * PI;
      result = (int) (cos(rads)*(float)FIXPOINT);
      printf("%d",result);
      if( minit == 0 )
         minit = 60;
      if( 9 != ii%10 )
         printf(", ");
      else if( 59 != ii )
         printf(",\n\t");
      else
         printf("\n};\n");
   }
}

int icos[60];

#else

/*
 * This array computed by above routine
 */
WORD icos[60] = {
   0, 26, 53, 79, 104, 128, 150, 171, 190, 207,
   221, 233, 243, 250, 254, 256, 254, 250, 243, 233,
   221, 207, 190, 171, 150, 127, 104, 79, 53, 26,
   0, -26, -53, -79, -104, -128, -150, -171, -190, -207,
   -221, -233, -243, -250, -254, -255, -254, -250, -243, -233,
   -221, -207, -190, -171, -150, -127, -104, -79, -53, -26
};

#endif
/*---------------------------------------------------------------------*/
MLOCAL VOID calc_hand(minit, len, x_offset, y_offset)

WORD minit;
WORD len;
WORD *x_offset;
WORD *y_offset;
{
   while( minit < 0 )
      minit += 60;
   while( minit >= 60 )
      minit -= 60;
#if 000
   /* long arithmetic to avoid overflow */
   *x_offset = (WORD )(((LONG )len * (LONG )icos[minit]) / FIXPOINT);
#else
   *x_offset = x_mul_div( len, icos[minit], FIXPOINT );
#endif
   minit += 15;   /* take advantage that sin(x)==cos(x+PI/2) */
   if( minit>=60 )
      minit -= 60;
#if 000
   *y_offset = (WORD )(-(((LONG )len * (LONG )icos[minit]) / FIXPOINT));
   *y_offset = (int)(((long)*y_offset * (long)pxl_width) / (long)pxl_height);
#else
   *y_offset = - x_mul_div( len, icos[minit], FIXPOINT );
   /* Note: negate y_offset to compensate for origin in top left corner */
   *y_offset = x_mul_div( *y_offset, pxl_width, pxl_height );
   /* handle aspect ratio */
#endif
}
/*** end of calc_hand() ***/

/*----------------------------------------------------------------------*/
/*                                                                      */
/*    draw_hand()                                                       */
/*                                                                      */
/*----------------------------------------------------------------------*/

MLOCAL VOID draw_hand( pos, len, wid)
WORD pos, len, wid;
{
    WORD xx, yy, npoints;
    WORD xys[8];

   calc_hand( pos, len, &xx, &yy );
   xys[0] = clk_x + xx;
   xys[1] = clk_y + yy;
   if( wid == 0 )
      return;         /* don't draw the hand with width 0 */
   else if( wid == 1 )
   {
      xys[2] = clk_x;
      xys[3] = clk_y;
      npoints = 2;      /* draw a simple line segment */
   }
   else
   {
      calc_hand( pos+15, wid, &xx, &yy );
      xys[2] = clk_x + xx;
      xys[3] = clk_y + yy;
      calc_hand( pos-15, wid, &xx, &yy );
      xys[4] = clk_x + xx;
      xys[5] = clk_y + yy;
      xys[6] = xys[0];
      xys[7] = xys[1];
      npoints = 4;      /* draw a triangle with base == 2*wid */
   }
   v_pline( gl_vh, npoints, xys );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* clock_update() - assumes no more need to redraw, just update the display */
/*                                                                          */
/*--------------------------------------------------------------------------*/

VOID clock_update( just_drawn )
BOOLEAN just_drawn;
{
   WORD now_hour;
   GRECT rect,box;
   WORD xy[4];
   struct tm *tm_now;
   time_t timer;

   time( &timer );
   tm_now = localtime( &timer );
   gl_title = asctime(tm_now);  
   gl_title[11] = 0;

   sprintf(gl_title,"%s %s %02d",
           wday_strs[tm_now->tm_wday],month_strs[tm_now->tm_mon],tm_now->tm_mday);

   wind_set(gl_wh, WF_NAME, FP_OFF(gl_title), FP_SEG(gl_title), 0, 0);
   /* note - hours get converted to 1/60s units around the clock */
   /*        this routine assumes hours 0-23 */
   if( (now_hour = tm_now->tm_hour) >= 12 )
   {
      now_hour -= 12;
   /* set a PM flag here, if needed */
   }
   /* adjust hour hand to simulate analog */
   now_hour = now_hour * 5 + ((6+tm_now->tm_min) / 12);
   if( now_hour >= 60 )
      now_hour -= 60;
   vswr_mode( gl_vh, MD_XOR );
   vsl_width( gl_vh, 1 );
   vsl_color( gl_vh, clk_hand_col );
   vsl_type( gl_vh, 1 );   /* solid line */

#if MSDOS
   wind_update(BEG_UPDATE);
   graf_mouse(M_OFF,0l);
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

         if( ! just_drawn )
         {
            draw_hand( clk_old_hour, clk_hour_len, clk_hour_wid );
            draw_hand( clk_old_min, clk_min_len, clk_min_wid );
            draw_hand( clk_old_sec, clk_sec_len, clk_sec_wid );
         }
         draw_hand( now_hour, clk_hour_len, clk_hour_wid );
         draw_hand( tm_now->tm_min, clk_min_len, clk_min_wid );
         draw_hand( tm_now->tm_sec, clk_sec_len, clk_sec_wid );
      }

      wind_get(gl_wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
   }

#if MSDOS
   graf_mouse(M_ON,0l);
   wind_update(END_UPDATE);
#endif

   clk_old_hour = now_hour;
   clk_old_min = tm_now->tm_min;
   clk_old_sec = tm_now->tm_sec;
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/* clock_draw() - draws a clock                                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/

VOID clock_draw()
{
   WORD ii;
   WORD dmy, bkg[4];

   vswr_mode( gl_vh, MD_REPLACE );

   /* color the background */
   if( clk_bkg_col != UNDEFINED )
   {
      bkg[0] = gl_wdw.g_x;
      bkg[1] = gl_wdw.g_y;
      bkg[2] = gl_wdw.g_x + gl_wdw.g_w - 1;
      bkg[3] = gl_wdw.g_y + gl_wdw.g_h - 1;
      vsf_color( gl_vh, clk_bkg_col );
      vr_recfl( gl_vh, bkg );
   }

   /* draw the clock face */
   if( clk_fill_col != UNDEFINED )
   {
      vsf_color( gl_vh, clk_fill_col );
      vsf_interior( gl_vh, clk_fill_typ );
      vsf_style( gl_vh, clk_fill_sty );
      vsf_perimeter( gl_vh, TRUE);
      v_circle( gl_vh, clk_x, clk_y, clk_radius );
   }

   vsl_type( gl_vh, 1 );   /* solid line */
   /* allow optional thicker circle around clock face */
   if( clk_peri_wid > 1  ||  clk_peri_col != clk_fill_col )
   {
      vsl_width( gl_vh, clk_peri_wid );
      vsl_color( gl_vh, clk_peri_col );
      v_arc( gl_vh, clk_x, clk_y, clk_radius, 0, 3600 );
   }

   /* draw the numeric & labeling text */
   /* use default text font unless otherwise specified */
   if( clk_txt_size != UNDEFINED )
      vst_height( gl_vh, clk_txt_size, &dmy, &dmy, &dmy, &dmy );
   vst_color( gl_vh, clk_txt_col );
   vst_alignment( gl_vh, CENTER, HALF_LINE, &ii, &ii );
   vswr_mode( gl_vh, MD_TRANS );
   for( ii=N_CLOCK_STRS-1; ii>=0; --ii )
   {
      v_gtext( gl_vh, clk_x+clock_s_x[ii], clk_y+clock_s_y[ii],clock_strs[ii] );
   }
   /* draw the hands of the clock */
}

VOID do_redraw()
{
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
         clock_draw();
      }

      wind_get(gl_wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
   }

   vs_clip(gl_vh, FALSE, xy);

#if MSDOS
   graf_mouse(M_ON, 0l);
   wind_update(END_UPDATE);
#endif

   clock_update( TRUE );
}

VOID clock_put_text()
{
   WORD mm, ii, xx, yy;

#if MSDOS
   wind_update(BEG_UPDATE);
   graf_mouse(M_OFF, 0l);
#endif

   /* convention: 1st text entry is clock label */
   calc_hand( 30, clk_radius/2, &xx, &yy );
   clock_s_x[0] = xx;
   clock_s_y[0] = yy;
   /* now spread the numbers around the clock */
   for( ii=1, mm=0; mm<60; ii++, mm+= 5 )
   {
      calc_hand( mm, clk_radius-clk_txt_size, &xx, &yy );
      clock_s_x[ii] = xx;
      clock_s_y[ii] = yy;
   }

#if MSDOS
   graf_mouse(M_ON, 0l);
   wind_update(END_UPDATE);
#endif
}

VOID invert_colors()
{
   if( clk_bkg_col == WHITE )
   {
      clk_bkg_col =  BLACK;
      clk_peri_col = WHITE;
      clk_txt_col =  WHITE;
      clk_hand_col = WHITE;
   }
   else
   {
      clk_bkg_col =  WHITE;
      clk_peri_col = BLACK;
      clk_txt_col =  BLACK;
      clk_hand_col = BLACK;
   }
   do_redraw();
}

VOID move_clock()
{
   WORD y_radius;

   clk_radius = gl_wdw.g_w/2;
   y_radius = x_mul_div( clk_radius, pxl_width, pxl_height );
   if( y_radius > gl_wdw.g_h/2 )
      clk_radius = x_mul_div( gl_wdw.g_h/2, pxl_height, pxl_width );
   clk_radius -= 1;
   clk_x = gl_wdw.g_x + gl_wdw.g_w/2;
   clk_y = gl_wdw.g_y + gl_wdw.g_h/2;
   clk_hour_len = (2 * clk_radius) / 3;
   clk_min_len = clk_radius - 1;
   clk_sec_len = clk_radius - 2;

   clock_put_text();
}

/*----------------------------------------------------------------------*/
/* clock_init() - allocs and initializes the clock variables            */
/*----------------------------------------------------------------------*/

VOID clock_init()
{
    clk_fill_col = UNDEFINED;
    clk_fill_typ = 1;
    clk_fill_sty = 1;
    clk_peri_wid = 1;
    clk_txt_size = systxt_height;
    clk_bkg_col =  BLACK;
    clk_peri_col = WHITE;
    clk_txt_col =  WHITE;
    clk_hand_col = WHITE;
    clk_hour_wid = 4;
    clk_min_wid =  4;
    clk_sec_wid =  1;
    clk_old_hour = UNDEFINED;
    clk_old_min =  UNDEFINED;
    clk_old_sec =  UNDEFINED;

    move_clock();
}

/*
 * moves or resizes window
 */
VOID change_window(wdw)
GRECT *wdw;
{
   wind_set(gl_wh, WF_CXYWH, wdw->g_x, wdw->g_y, wdw->g_w, wdw->g_h);
   wind_get(gl_wh, WF_WXYWH, &gl_wdw.g_x, &gl_wdw.g_y,&gl_wdw.g_w, &gl_wdw.g_h);
   move_clock();
}

VOID hndl_menu( )
{
   invert_colors();
}

BOOLEAN open_window()
{
   WORD ww;

   if ((gl_wh = wind_create(gl_wflags, 0, 0, 0, 0)) <= 0)
      return(FALSE);

   wind_set(gl_wh, WF_NAME, FP_OFF(gl_title), FP_SEG(gl_title), 0, 0);

   wind_get(DESK, WF_WXYWH, &gl_wdw.g_x, &gl_wdw.g_y, 
              &gl_wdw.g_w, &gl_wdw.g_h);
   ww = 25 * systxt_width;
   gl_wdw.g_h = x_mul_div( ww, pxl_width, pxl_height );
   gl_wdw.g_x += gl_wdw.g_w - ww;
   gl_wdw.g_w = ww;
   wind_open(gl_wh, gl_wdw.g_x, gl_wdw.g_y,  gl_wdw.g_w, gl_wdw.g_h);
   wind_get(gl_wh, WF_WXYWH, &gl_wdw.g_x, &gl_wdw.g_y,&gl_wdw.g_w, &gl_wdw.g_h);

   clock_init();   

   return(TRUE);
}

BOOLEAN initialize_app()
{
   WORD work_in[11], work_out[57];
   WORD ii;

   appl_init(NULL);

   gl_vh = graf_handle(&systxt_width, &systxt_height, &ii, &ii);

   for( ii=0; ii<10; ++ii )
      work_in[ii] = 1;                       /* use defaults         */
   work_in[10] = 2;                          /* raster coords        */
   v_opnvwk(work_in, &gl_vh, work_out);      /* get our vdi handle   */
   pxl_width = work_out[3];
   pxl_height = work_out[4];

#if !DESKACC
   return (open_window());
#else
   gl_accid = menu_register(gl_apid, "  Clock");
   return (TRUE);
#endif
}

/*------------------------------------------------------------------------
 * main() - main application entry point
 ------------------------------------------------------------------------*/
main()
{
   WORD ret, msg[8], done;
   WORD top, dummy;
   MEVENT e;

   if ( initialize_app() )
   {
      done = FALSE;

      while(!done)
      {
         e.e_flags   = MU_MESAG | ((gl_wh > 0) ? MU_TIMER : 0);
         e.e_mepbuf  = msg;
         e.e_time    = prf_secs_p_update * 1000L;

         ret = evnt_event(&e);

         if (ret & MU_MESAG)
         {
            switch(msg[0])
            {
#if DESKACC
               case AC_OPEN:
                  if (msg[4] != gl_accid)
                     break;
                  if (gl_wh > 0)
                  {
                     wind_get(0, WF_TOP, &top, &dummy, &dummy, &dummy);
                     if (top != gl_wh)
                        wind_set(gl_wh, WF_TOP, gl_wh, 0, 0, 0);
                     else
                        hndl_menu();
                  }
                  else
                     open_window();
                  break;

                case AC_CLOSE:
                  if (msg[3] == gl_accid)
                     gl_wh = -1;
                  break;

               case WM_CLOSED:
                  if (msg[3] != gl_wh || gl_wh <= 0)
                     break;
                  graf_mouse(M_OFF, 0l);
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
                  do_redraw();
                  break;

#if !DESKACC
               case WM_CLOSED:
                  done = TRUE;
                  break;
#endif
               case WM_MOVED:
               case WM_SIZED:
                  change_window((GRECT *)&msg[4]);
                  do_redraw();
                  break;

               case MN_SELECTED:
                  hndl_menu();
                  break;
            } /* end switch */
         }

         if (gl_wh <= 0)
            continue;

         if (ret & MU_TIMER)
         {
            clock_update(FALSE);
         }

#if DESKACC
         done = FALSE;
#endif
      } /* while */

#if (!DESKACC)	/* Defeat compiler warnings */
	top = 0;
	dummy = 0;
#endif
      wind_close(gl_wh);
      wind_delete(gl_wh);
      v_clsvwk(gl_vh);
      appl_exit();
   } /* if initialize_app */
}

#if FLEXOS

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

/*--------------------------------------------------------------------------
 *
 * Compute intersect of two rectangles
 *
 *--------------------------------------------------------------------------*/

WORD rc_intersect(p1, p2)
GRECT FAR *p1, FAR *p2;
{
   WORD tx, ty, tw, th;

   tw = min(p2->g_x + p2->g_w, p1->g_x + p1->g_w); 
   th = min(p2->g_y + p2->g_h, p1->g_y + p1->g_h); 
   tx = max(p2->g_x, p1->g_x);   
   ty = max(p2->g_y, p1->g_y);   
   p2->g_x = tx;    
   p2->g_y = ty;   
   p2->g_w = tw - tx;   
   p2->g_h = th - ty;   

   return( (tw > tx) && (th > ty) );   
}

#endif
