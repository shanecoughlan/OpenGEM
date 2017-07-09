/************************************************************************
*
*     GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*     Filename: XMAIN.C
*
*     Date: 08/01/89
*
*************************************************************************/

/*-------------------- XGEM Include - Data -----------------------------*/
#include "portab.h"           /* portable coding conventions   */
#include "aes.h"              /* GEM-Bindings                  */
#include "vdi.h"              /* VDI-Bindings                  */
#include "rclib.h"

/*-------------------- Resource Definitions ----------------------------*/
#include  "events.h"           /* Resource-Data-Definitions   */
/*-------------------- Header data -------------------------------------*/
#include  "window.h"          /* Definitions for Window-Tools */
/*-------------------- Type definitions --------------------------------*/
#include  "types.h"
#include  "proto.h"

#if MSDOS
/*-------------------- Global GEM-Data ---------------------------------*/
GLOBAL WORD    contrl[11];       /* control inputs          */
GLOBAL WORD    intin[80];        /* max string length       */
GLOBAL WORD    ptsin[256];       /* polygon fill points     */
GLOBAL WORD    intout[45];       /* open workstation output */
GLOBAL WORD    ptsout[12];       /* points output           */
#endif

/*-------------------- Variables for init ------------------------------*/
GLOBAL WORD    gl_wchar;         /* character width      */
GLOBAL WORD    gl_hchar;         /* character height     */
GLOBAL WORD    gl_wbox;          /* box (cell) width     */
GLOBAL WORD    gl_hbox;          /* box (cell) height    */
GLOBAL WORD    appl_id;          /* Application-ID       */
GLOBAL WORD    gem_handle;       /* GEM vdi handle       */
GLOBAL WORD    vdi_handle;       /* vdi handle           */
GLOBAL WORD    work_out[57];

/*-------------------- Window - Variables ------------------------------*/
EXTERN W_TYP   w_[];
GLOBAL WORD    top_w_nr; 
GLOBAL WORD    x_full;
GLOBAL WORD    y_full;
GLOBAL WORD    w_full;
GLOBAL WORD    h_full;

GLOBAL OBJECT FAR *menu_tree;

GLOBAL   WORD   x_mouse_pos;      /* window relative Mouse position    */
GLOBAL   WORD   y_mouse_pos;

/*----------------------------------------------------------------------*/
/*  init():   Open Virtual Workstation,                                 */
/*            initialize global variables,                              */
/*            load Resource-Data and                                    */
/*            create Menu.                                              */
/*----------------------------------------------------------------------*/
WORD init()
{
   WORD i;
   WORD work_in[11];

   appl_id = appl_init(NULL);
   if (appl_id < 0)
      return(FALSE);

   /* Load Resource Data File */
   if (!rsrc_load("EVENTS.RSC"))
   {
      form_alert(1,"[3][Fatal Error !|EVENTS.RSC|File Not Found !][ Ok ]");
      return(FALSE);
   }

   for (i = 0;i < 10;i++)
   {
      work_in[i] = 1;
   }
   work_in[10] = 2;      /* Raster coordinate-System */
   gem_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
   vdi_handle = gem_handle;

   /* Open Virtual Workstation */
   v_opnvwk(work_in, &vdi_handle, work_out);

   if (vdi_handle == 0)
      return(FALSE);

   rsrc_gaddr(R_TREE, MENU, (LPVOID *)&menu_tree);

   /* Make menu-bar */
   menu_bar(menu_tree, TRUE);

   wind_get(DESK, WF_WXYWH, &x_full, &y_full, &w_full, &h_full);

   /* Initialize Window blocks */
   init_window();

   return(TRUE);
}

/*----------------------------------------------------------------------*/
/* init_trans(): Transform Normal Coordinates to Raster Coordinates     */
/*----------------------------------------------------------------------*/
VOID init_trans()
{
   WORD     i;

   for ( i=0; i < MAX_W; i++ )
   {
     if ( w_[i].init )
      {
      w_[i].x_fpos  = (WORD) ( (LONG) w_[i].x_npos  * w_full /  FAKT ) + x_full;
      w_[i].y_fpos  = (WORD) ( (LONG) w_[i].y_npos  * h_full /  FAKT ) + y_full;
      w_[i].fwidth  = (WORD) ( (LONG) w_[i].nwidth  * w_full /  FAKT );
      w_[i].fheight = (WORD) ( (LONG) w_[i].nheight * h_full /  FAKT );
      }
   }
}

/*-------------------------------------------------------------------------*/
/* init_klickfelder():                                                     */
/*-------------------------------------------------------------------------*/
VOID init_klickfelder(k_, n)
K_TYP k_[1];
WORD n;
{
   WORD i;

   for (i=0; i < n; i++)
   {
      k_[i].e_nr=-1;
      k_[i].init = FALSE;
   }
}

/*----------------------------------------------------------------------*/
/* term(): Terminate the Virtual Workstation                            */
/*----------------------------------------------------------------------*/
VOID term()
{
   WORD i;

   for (i = 0; (i<MAX_W) && (w_[i].init); i++)
      window_close(i);
   menu_bar(menu_tree, FALSE);
   rsrc_free();
  
   if (vdi_handle!=0)
      v_clsvwk( vdi_handle );
   if (appl_id!=-1)
      appl_exit();
}

/*----------------------------------------------------------------------*/
/* dialog():                                                            */
/*----------------------------------------------------------------------*/
VOID dialog( window_nr )
WORD window_nr;
{
   WORD ev_which;
   WORD done;
   MEVENT mevent;
   WORD msg[16];

   init_event( &mevent, msg);      /* Initialize  Event-Blocks */

   if (window_open( window_nr ))
   {
      do
      {
         ev_which = evnt_event( &mevent );

         x_mouse_pos=mevent.e_mx+w_[top_w_nr].x_move-w_[top_w_nr].x_wpos;
         y_mouse_pos=mevent.e_my+w_[top_w_nr].y_move-w_[top_w_nr].y_wpos;

         done = handle_event(ev_which,&mevent);
      }
      while (done==FALSE);
   }
}

/*----------------------------------------------------------------------*/
/* main():                                                              */
/*----------------------------------------------------------------------*/
VOID main()
{
   if (init())         /* Initialize Global Data and XGEM-Lib */
   {
      dialog_box(INFOBOX); 
      dialog(0);
   }
   term();
}
