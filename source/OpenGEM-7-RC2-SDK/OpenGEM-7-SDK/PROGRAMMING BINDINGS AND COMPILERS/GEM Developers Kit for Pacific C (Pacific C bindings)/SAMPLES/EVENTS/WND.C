/************************************************************************
*
*     GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*     Filename: WINDOW.C
*
*     Date: 08/01/89
*
*************************************************************************/

/*-------------------- XGEM Include - Data -----------------------------*/
#include "portab.h"
#include "aes.h"
#include "vdi.h"
#include "rclib.h"

/*-------------------- KURVE Include - Data ----------------------------*/
#include "window.h"
#include "types.h"
#include "proto.h"
#include <dos.h>

EXTERN WORD vdi_handle;          /* vdi handle                          */
EXTERN W_TYP w_[];               /* Window block                        */
EXTERN WORD top_w_nr;            
EXTERN WORD x_full;              
EXTERN WORD y_full;              
EXTERN WORD w_full;              
EXTERN WORD h_full;             

/*-------------------- Dialog box - Variables --------------------------*/
GLOBAL WORD i_x, i_y, i_w, i_h;  /* Dialogbox coordinates               */

/*----------------------------------------------------------------------*/
/* window_open():                                                       */
/*----------------------------------------------------------------------*/
WORD window_open( w_nr )
WORD w_nr;
{
   if ( w_[w_nr].init )
   {
      /* freiraum fuer Window ermitteln*/
      wind_get ( DESK, WF_WXYWH, &x_full, &y_full, &w_full, &h_full );

      /* Window initialization */
      w_[w_nr].handle=wind_create(w_[w_nr].kind, w_[w_nr].x_fpos, 
                                  w_[w_nr].y_fpos, w_[w_nr].fwidth,
                                  w_[w_nr].fheight);

      if ( w_[w_nr].handle >= 0 )
      {
         /* Window name set */
         wind_set ( w_[w_nr].handle, WF_NAME, FPOFF(w_[w_nr].name) ,
                    FPSEG((BYTE FAR *)w_[w_nr].name) , 0, 0 );

         /* Window open*/
         wind_open ( w_[w_nr].handle, w_[w_nr].x_fpos, w_[w_nr].y_fpos,
                     w_[w_nr].fwidth, w_[w_nr].fheight );

         /* Horizontal */
         wind_set ( w_[w_nr].handle, WF_HSLSIZE, 1000, 0, 0, 0 );
         /* Vertical */
         wind_set ( w_[w_nr].handle, WF_VSLSIZE, 1000, 0, 0, 0 );
         /* Horizontal position */
         wind_set ( w_[w_nr].handle, WF_HSLIDE, 0, 0, 0, 0 );
         /* Vertical */
         wind_set ( w_[w_nr].handle, WF_VSLIDE, 0, 0, 0, 0 );

         /* Work area size */
         wind_get (w_[w_nr].handle, WF_WXYWH, &w_[w_nr].x_wfpos,
                     &w_[w_nr].y_wfpos, &w_[w_nr].wfwidth, &w_[w_nr].wfheight);

         w_[w_nr].x_wpos=w_[w_nr].x_wfpos;
         w_[w_nr].y_wpos=w_[w_nr].y_wfpos;
         w_[w_nr].wwidth=w_[w_nr].wfwidth;
         w_[w_nr].wheight=w_[w_nr].wfheight;

         w_[w_nr].x_move=0;
         w_[w_nr].y_move=0;

         top_w_nr=w_nr;

         w_[w_nr].meta_buffer = meta_read(w_[w_nr].meta_name);
      if (meta_trans(w_[w_nr].meta_buffer,0,0,w_[w_nr].wwidth,w_[w_nr].wheight))
        {
            meta_find_objekts(w_nr,w_[w_nr].meta_buffer);
            return(TRUE);
        }
      }
   }
   form_alert (1,"[3][Window cannot be opened !][ Ok ]");
   return(FALSE);
}

/*----------------------------------------------------------------------*/
/* window_close():                                                      */
/*----------------------------------------------------------------------*/
VOID window_close( w_nr )
WORD w_nr;
{
   if (w_[w_nr].handle>=0)
   {
      dos_free(w_[w_nr].meta_buffer);

      wind_close(w_[w_nr].handle);      /* Window close                 */
      wind_delete(w_[w_nr].handle);      /* Window delete               */
      w_[w_nr].handle=-1;
   }
}

/*----------------------------------------------------------------------*/
/* draw():                                                              */
/*----------------------------------------------------------------------*/
VOID draw(w_nr,x,y,w,h)
WORD w_nr;
WORD x,y,w,h;
{
   WORD   xy[4];

   vsf_interior ( vdi_handle, 0 );
   xy[0] = x;
   xy[1] = y;
   xy[2] = x+w-1;
   xy[3] = y+h-1;
   vs_clip( vdi_handle, TRUE, xy );   /* vs_clip */
   v_bar( vdi_handle, xy );      /* v_bar  */
   meta_out( vdi_handle, w_[w_nr].meta_buffer,
              w_[w_nr].x_wpos - w_[w_nr].x_move,
              w_[w_nr].y_wpos - w_[w_nr].y_move  );
   vs_clip( vdi_handle, FALSE, xy );      /* vs_clip  */
}

/*----------------------------------------------------------------------*/
/* redraw():                                                            */ 
/*----------------------------------------------------------------------*/
VOID redraw(w_nr, x, y, w, h)
WORD w_nr;
WORD x, y, w, h;
{
   GRECT t1, t2;

#if MSDOS
   wind_update(BEG_UPDATE);
   graf_mouse(M_OFF, 0l);
#endif

   t2.g_x=x;
   t2.g_y=y;
   t2.g_w=w;
   t2.g_h=h;
   wind_get (w_[w_nr].handle, WF_FIRSTXYWH, &t1.g_x, &t1.g_y, &t1.g_w, &t1.g_h);
   while (t1.g_w&&t1.g_h)
   {
    if (rc_intersect(&t2,&t1))
       draw(w_nr,t1.g_x,t1.g_y,t1.g_w,t1.g_h);

    wind_get (w_[w_nr].handle, WF_NEXTXYWH, &t1.g_x, &t1.g_y, &t1.g_w, &t1.g_h);
   }

#if MSDOS
   graf_mouse(M_ON, 0l);
   wind_update(END_UPDATE);
#endif
}

/*----------------------------------------------------------------------*/
/* dialog_box():                                                        */
/*----------------------------------------------------------------------*/
WORD dialog_box( dialogbox_nr )
WORD dialogbox_nr;
{
 OBJECT FAR *tree;
 WORD button;

   rsrc_gaddr(R_TREE,dialogbox_nr,(LPVOID *)&tree);
   ob_draw_dialog(tree,0,0,0,0);
   button = form_do(tree,0);
   ob_undraw_dialog(tree,0,0,0,0);
   ob_undostate(tree,button,SELECTED);
   return(button);
}

/*----------------------------------------------------------------------*/
/* find_window():                                                       */
/*----------------------------------------------------------------------*/
WORD find_window( handle )
WORD handle;            /* Window handle         */
{
   WORD i;

   for (i=0;((i<MAX_W)&&(w_[i].handle!=handle));)
      i++;
   if (i < MAX_W)
   {
      return(i);
   }
   else
   {
      return(-1);
   }
}

/*----------------------------------------------------------------------*/
/* find_click_area():                                                   */
/*----------------------------------------------------------------------*/
WORD find_click_area()
{
   WORD i;
   K_TYP *k_;

   k_=w_[top_w_nr].klick_felder;
   for (i=0; i < w_[top_w_nr].klick_feld_anzahl; i++)
   {
      if (   (x_mouse_pos>=k_[i].x_min)&&(y_mouse_pos>=k_[i].y_min)
           && (x_mouse_pos<=k_[i].x_max)&&(y_mouse_pos<=k_[i].y_max)
           && (k_[i].init==TRUE))
      return(i);
   }
   return(-1);
}

/*----------------------------------------------------------------------*/
/* handle_window():                                                     */
/*----------------------------------------------------------------------*/
VOID handle_window( mevent )
MEVENT FAR *mevent;
{
   WORD w_nr;
   WORD FAR *msg;

   msg = mevent->e_mepbuf;

   w_nr=find_window( msg[3] );
   if (w_nr>=0)
   {
      switch (msg[0])
      {

         case WM_CLOSED:
            window_close(w_nr);
            break;

         case WM_REDRAW:
            redraw(w_nr,msg[4],msg[5],msg[6],msg[7]);
            /* Redraw */
            break;

         case WM_ONTOP:            /*         */
            top_w_nr=w_nr;
            break;

         case WM_TOPPED:
            wind_set(msg[3],WF_TOP,0,0,0,0);
            top_w_nr = w_nr;
            break;

         case WM_SIZED:
            w_[w_nr].full = FALSE;
            if (msg[6]>w_[w_nr].fwidth)
               msg[6]=w_[w_nr].fwidth;
            if (msg[7]>w_[w_nr].fheight)
               msg[7]=w_[w_nr].fheight;
            wind_set(msg[3], WF_CXYWH,msg[4],msg[5],msg[6],msg[7]);
            w_[w_nr].x_slider_size =(WORD)((LONG)msg[6]*FAKT/w_[w_nr].wfwidth);
            w_[w_nr].y_slider_size =(WORD)((LONG)msg[7]*FAKT/w_[w_nr].wfheight);
            wind_set(msg[3],WF_HSLSIZE,w_[w_nr].x_slider_size,0,0,0);
            wind_set(msg[3],WF_VSLSIZE,w_[w_nr].y_slider_size,0,0,0);
            wind_get(msg[3],WF_WXYWH,&w_[w_nr].x_wpos, &w_[w_nr].y_wpos,
                                            &w_[w_nr].wwidth,&w_[w_nr].wheight);
            if ( w_[w_nr].x_move > w_[w_nr].wfwidth-w_[w_nr].wwidth )
               w_[w_nr].x_move = w_[w_nr].wfwidth-w_[w_nr].wwidth;
            if ( w_[w_nr].y_move > w_[w_nr].wfheight-w_[w_nr].wheight )
               w_[w_nr].y_move = w_[w_nr].wfheight-w_[w_nr].wheight;
            if ( w_[w_nr].wfwidth != w_[w_nr].wwidth )
               w_[w_nr].h_slider_pos=(WORD)((LONG)w_[w_nr].x_move*MAX_SLIDER /
                                            (w_[w_nr].wfwidth-w_[w_nr].wwidth));
            else
               w_[w_nr].h_slider_pos=0;
            if (w_[w_nr].h_slider_pos>MAX_SLIDER)
               w_[w_nr].h_slider_pos=MAX_SLIDER;
            else if (w_[w_nr].h_slider_pos<MIN_SLIDER)
               w_[w_nr].h_slider_pos=MIN_SLIDER;
            if (w_[w_nr].wfheight!=w_[w_nr].wheight)
               w_[w_nr].v_slider_pos=(WORD)((LONG)w_[w_nr].y_move*MAX_SLIDER /
                                          (w_[w_nr].wfheight-w_[w_nr].wheight));
            else
               w_[w_nr].v_slider_pos=0;
            if ( w_[w_nr].v_slider_pos>MAX_SLIDER)
               w_[w_nr].v_slider_pos=MAX_SLIDER;
            else if (w_[w_nr].v_slider_pos<MIN_SLIDER)
               w_[w_nr].v_slider_pos = MIN_SLIDER;
            wind_set(msg[3],WF_VSLIDE,w_[w_nr].v_slider_pos,0,0,0);
            wind_set(msg[3],WF_HSLIDE,w_[w_nr].h_slider_pos,0,0,0);
            break;

         case WM_MOVED:
            w_[w_nr].full=FALSE;
            w_[w_nr].x_alt=w_[w_nr].x_wpos;
            w_[w_nr].y_alt=w_[w_nr].y_wpos;
            w_[w_nr].w_alt=w_[w_nr].wwidth;
            w_[w_nr].h_alt=w_[w_nr].wheight;
            wind_set(msg[3], WF_CXYWH, msg[4], msg[5], msg[6], msg[7]);
            wind_get(msg[3], WF_WXYWH, &w_[w_nr].x_wpos, &w_[w_nr].y_wpos,
                                          &w_[w_nr].wwidth, &w_[w_nr].wheight);
            break;

         case WM_FULLED:
            if (w_[w_nr].full)
            {
               wind_set(msg[3], WF_CXYWH, w_[w_nr].x_alt, w_[w_nr].y_alt,
                                               w_[w_nr].w_alt, w_[w_nr].h_alt );
               w_[w_nr].x_move = w_[w_nr].x_move_alt;
               w_[w_nr].y_move = w_[w_nr].y_move_alt;
               wind_get(msg[3], WF_WXYWH, &w_[w_nr].x_wpos, &w_[w_nr].y_wpos,
                                            &w_[w_nr].wwidth,&w_[w_nr].wheight);
               w_[w_nr].x_slider_size = w_[w_nr].h_ss_alt;
               w_[w_nr].y_slider_size = w_[w_nr].v_ss_alt;
               wind_set(msg[3], WF_HSLSIZE, w_[w_nr].x_slider_size, 0, 0, 0);
               wind_set(msg[3], WF_VSLSIZE, w_[w_nr].y_slider_size, 0, 0, 0);
               w_[w_nr].h_slider_pos = w_[w_nr].h_sp_alt;
               w_[w_nr].v_slider_pos = w_[w_nr].v_sp_alt;
               wind_set(msg[3], WF_HSLIDE, w_[w_nr].h_slider_pos, 0, 0, 0);
               wind_set(msg[3], WF_VSLIDE, w_[w_nr].v_slider_pos, 0, 0, 0);
               if ((w_[w_nr].x_alt==x_full) && (w_[w_nr].y_alt==y_full))
                  draw(w_nr, w_[w_nr].x_wpos, w_[w_nr].y_wpos, w_[w_nr].wwidth,
                                                             w_[w_nr].wheight);
               w_[w_nr].full=FALSE;
            }
            else
            {
               wind_get(msg[3], WF_CXYWH, &w_[w_nr].x_alt, &w_[w_nr].y_alt,
                                              &w_[w_nr].w_alt, &w_[w_nr].h_alt);
               wind_set(msg[3], WF_CXYWH, w_[w_nr].x_fpos, w_[w_nr].y_fpos,
                                             w_[w_nr].fwidth, w_[w_nr].fheight);
               w_[w_nr].x_move_alt = w_[w_nr].x_move;
               w_[w_nr].y_move_alt = w_[w_nr].y_move;
               w_[w_nr].x_move = 0;
               w_[w_nr].y_move = 0;
               wind_get(msg[3], WF_WXYWH, &w_[w_nr].x_wpos, &w_[w_nr].y_wpos,
                                          &w_[w_nr].wwidth, &w_[w_nr].wheight);
               w_[w_nr].v_ss_alt = w_[w_nr].y_slider_size;
               w_[w_nr].h_ss_alt = w_[w_nr].x_slider_size;
               wind_set(msg[3], WF_HSLSIZE, MAX_SLIDER, 0, 0, 0);
               wind_set(msg[3], WF_VSLSIZE, MAX_SLIDER, 0, 0, 0);
               w_[w_nr].h_sp_alt = w_[w_nr].h_slider_pos;
               w_[w_nr].v_sp_alt = w_[w_nr].v_slider_pos;
               wind_set(msg[3], WF_HSLIDE, MIN_SLIDER, 0, 0, 0);
               wind_set(msg[3], WF_VSLIDE, MIN_SLIDER, 0, 0, 0);
               w_[w_nr].full = TRUE;
            } 
            break;

         case WM_ARROWED:
            switch (msg[4])
            {
               case WA_UPPAGE:
                w_[w_nr].y_move-=(WORD)((LONG)w_[w_nr].wheight*PAGE_FAKT/FAKT);
                  break;
               case WA_DNPAGE:
                w_[w_nr].y_move+=(WORD)((LONG)w_[w_nr].wheight*PAGE_FAKT/FAKT);
                  break;
               case WA_UPLINE:
                w_[w_nr].y_move-=SLIDER_STEP;
                  break;
               case WA_DNLINE:
                w_[w_nr].y_move+=SLIDER_STEP;
                  break;
               case WA_LFPAGE:
                w_[w_nr].x_move-=(WORD)((LONG)w_[w_nr].wwidth*PAGE_FAKT/FAKT);
                  break;
               case WA_RTPAGE:
                w_[w_nr].x_move+=(WORD)((LONG)w_[w_nr].wwidth*PAGE_FAKT/FAKT);
                  break;
               case WA_LFLINE:
                w_[w_nr].x_move-=SLIDER_STEP;
                  break;
               case WA_RTLINE:
                w_[w_nr].x_move+=SLIDER_STEP;
                  break;
            }
            if (w_[w_nr].x_move > w_[w_nr].wfwidth-w_[w_nr].wwidth)
               w_[w_nr].x_move = w_[w_nr].wfwidth-w_[w_nr].wwidth;
            else if (w_[w_nr].x_move < 0)
               w_[w_nr].x_move = 0;
            if (w_[w_nr].y_move > w_[w_nr].wfheight-w_[w_nr].wheight)
               w_[w_nr].y_move = w_[w_nr].wfheight-w_[w_nr].wheight;
            else if (w_[w_nr].y_move < 0)
               w_[w_nr].y_move = 0;
            draw(w_nr, w_[w_nr].x_wpos, w_[w_nr].y_wpos, w_[w_nr].wwidth,
                                                              w_[w_nr].wheight);

            if (w_[w_nr].wfwidth!=w_[w_nr].wwidth)
               w_[w_nr].h_slider_pos = (WORD)((LONG)w_[w_nr].x_move*MAX_SLIDER/
                                            (w_[w_nr].wfwidth-w_[w_nr].wwidth));
            else
               w_[w_nr].h_slider_pos = 0;

            if (w_[w_nr].wfheight!=w_[w_nr].wheight)
               w_[w_nr].v_slider_pos = (WORD)((LONG)w_[w_nr].y_move*MAX_SLIDER/
                                          (w_[w_nr].wfheight-w_[w_nr].wheight));
            else
               w_[w_nr].v_slider_pos = 0;
            if (w_[w_nr].v_slider_pos > MAX_SLIDER)
               w_[w_nr].v_slider_pos = MAX_SLIDER;
            else if (w_[w_nr].v_slider_pos < MIN_SLIDER)
               w_[w_nr].v_slider_pos = MIN_SLIDER;
            wind_set(msg[3], WF_VSLIDE, w_[w_nr].v_slider_pos, 0, 0, 0);
            wind_set(msg[3], WF_HSLIDE, w_[w_nr].h_slider_pos, 0, 0, 0);
            break;

         case WM_HSLID:
            w_[w_nr].h_slider_pos = msg[4];
            wind_set(msg[3], WF_HSLIDE, w_[w_nr].h_slider_pos, 0, 0, 0);
            w_[w_nr].x_move=(WORD)((LONG)w_[w_nr].h_slider_pos *
                                       (w_[w_nr].wfwidth-w_[w_nr].wwidth)/FAKT);
            if (w_[w_nr].x_move > w_[w_nr].wfwidth-w_[w_nr].wwidth)
               w_[w_nr].x_move = w_[w_nr].wfwidth-w_[w_nr].wwidth;
            draw(w_nr, w_[w_nr].x_wpos, w_[w_nr].y_wpos, w_[w_nr].wwidth,
                                                              w_[w_nr].wheight);
            break;

         case WM_VSLID:
            w_[w_nr].v_slider_pos = msg[4];
            wind_set(msg[3], WF_VSLIDE, w_[w_nr].v_slider_pos, 0, 0, 0);
            w_[w_nr].y_move = (WORD)((LONG)w_[w_nr].v_slider_pos *
                                     (w_[w_nr].wfheight-w_[w_nr].wheight)/FAKT);
            if (w_[w_nr].y_move > w_[w_nr].wfheight-w_[w_nr].wheight)
               w_[w_nr].y_move = w_[w_nr].wfheight-w_[w_nr].wheight;
            draw(w_nr, w_[w_nr].x_wpos, w_[w_nr].y_wpos, w_[w_nr].wwidth,
                                                              w_[w_nr].wheight);
            break;
      }
   }
}
