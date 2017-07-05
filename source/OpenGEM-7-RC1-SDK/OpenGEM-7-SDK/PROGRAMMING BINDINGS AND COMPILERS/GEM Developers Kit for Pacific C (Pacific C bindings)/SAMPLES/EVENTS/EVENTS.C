/************************************************************************/
/*	EVENTS.C for the GEM/3 Programmers Toolkit			*/
/************************************************************************/
/*	08/01/89							*/
/*	Translated from X/GEM						*/
/*	This example demonstrates EVENT TRACKING in GEM and produces	*/
/*	a graphic EVENT representation.					*/
/************************************************************************/

/*----- GEM Include - Files ----------------------------*/
#include "portab.h"
#include "aes.h"
#include "vdi.h"
#include "rclib.h"

/*----- KURVE Include - Files --------------------------*/
#include "events.h"       /* Resource-File-Definition 	*/
#include "types.h" 	  /* Choose block type  	*/
#include "window.h"
#include "proto.h"
#include	<string.h>
#define K_MAX      3
#define XY_MAX   100

/*------ External File ---------------------------------*/
EXTERN WORD   top_w_nr;
EXTERN OBJECT FAR *menu_tree;
EXTERN WORD   vdi_handle;


GLOBAL W_TYP   w_[ MAX_W ];

WORD   x_off,y_off;
K_TYP  k_[K_MAX];
WORD   graf_mode=0;
WORD   mul=0;
WORD   xy[XY_MAX];
WORD   zaehler=2;
WORD   gl_x=0, gl_y=0;
BYTE   *hex_strs = "0123456789ABCDEF";

/*----------------------------------------------------------------------*/
/*   init_event():   Initialization Evnt_event-Block                    */
/*----------------------------------------------------------------------*/
VOID init_event(e, msg)
MEVENT FAR *e;
WORD *msg;
{
   e->e_flags = MU_MESAG;

   e->e_bclk = 1;
   e->e_bmsk = 1;
   e->e_bst  = 1;

   e->e_m1flags = 0;
   e->e_m1.g_x  = 0;
   e->e_m1.g_y  = 0;
   e->e_m1.g_w  = 0;
   e->e_m1.g_h  = 0;

   e->e_m2flags = 0;
   e->e_m2.g_x  = 0;
   e->e_m2.g_y  = 0;
   e->e_m2.g_w  = 0;
   e->e_m2.g_h  = 0;

   e->e_mepbuf  = msg;

   e->e_time    = 0;

   e->e_xtra0   = 0;
   e->e_xtra1   = 0;
   e->e_xtra2   = 0;
}

/*----------------------------------------------------------------------*/
/*   init_window:   Initialization of the Window, type blocks           */
/*----------------------------------------------------------------------*/
VOID init_window()
{
   WORD i;                             /* Counter   			*/

   for ( i=0; i < MAX_W; i++ )         /* Initialize all Window types-	  */
                                       /*   				  */
   {
      w_[i].handle = -1;               /* All Windows are closed 	  */
      w_[i].init = FALSE;              /* Beschreibungsblock ist nicht    */
                                       /* initialize	   		  */
      w_[i].redrawed = FALSE;          /* No MASK was chosen for the	  */
                                       /* window. */
      w_[i].full = FALSE;              /* Windows are not available. 	  */
                                       /* Select window after clicking on */
   }				       /* FILLER.			  */	

   w_[0].kind = CLOSER|NAME|MOVER|SIZER|FULLER;
   w_[0].name = "EVENTS.GEM";
   w_[0].meta_name = w_[0].name;
   w_[0].klick_feld_anzahl = K_MAX;
   w_[0].klick_felder = k_;
   w_[0].x_npos = 200;                   /* relative Position (0..1000)      */
   w_[0].y_npos = 200;
   w_[0].nwidth = 800;                   /* relative Length, Height (0..1000 */
   w_[0].nheight = 600;
   w_[0].init = TRUE;
   init_klickfelder(k_, 2);

   init_trans ();
}

/*----------------------------------------------------------------------*/
/* handle_button:     Calculate result of Button Event			*/
/*----------------------------------------------------------------------*/
VOID handle_button(mevent)
MEVENT FAR *mevent;
{
}

/*----------------------------------------------------------------------*/
/* handle_keyboard:   Calculate result of Keyboard Event		*/
/*----------------------------------------------------------------------*/
VOID handle_keyboard(mevent)
MEVENT FAR *mevent;
{
}

/*----------------------------------------------------------------------*/
/* handle_timer:   Calculate the result of the Timer			*/
/*----------------------------------------------------------------------*/
VOID handle_timer(mevent)
MEVENT FAR *mevent;
{
   WORD y,i;
   WORD clxy[4];
   GRECT gr;

#if MSDOS
   wind_update(BEG_UPDATE);
   graf_mouse(M_OFF, 0l);
#endif

   x_off = w_[0].x_wpos;
   y_off = w_[0].y_wpos;
   vsl_color(vdi_handle, WHITE);
   vswr_mode(vdi_handle, MD_REPLACE);
   vsl_width(vdi_handle, 1);

   switch (graf_mode)
   {
      case 0:               /* Initialize */ 
         mul=(k_[2].x_max-k_[2].x_min)/(XY_MAX/2);
         if (mul < 1)
            mul=1;
         y=(k_[2].y_min+k_[2].y_max)/2+y_off;
         for(i=0; i<XY_MAX-1; i+=2)
         {
            xy[i]=k_[2].x_min+x_off+i*mul+1;
            xy[i+1]=y;
         }
         xy[0]=xy[0]+2;
         graf_mode=1;
         break;
      case 1:               /* Locate curve   */
       wind_get(w_[0].handle, WF_FIRSTXYWH, &gr.g_x, &gr.g_y, &gr.g_w, &gr.g_h);
        while (gr.g_w && gr.g_h)
        {
        rc_grect_to_array(&gr, clxy);
        vs_clip(vdi_handle, TRUE, clxy);
      
        v_pline(vdi_handle, XY_MAX/2, xy);
        wind_get(w_[0].handle, WF_NEXTXYWH, &gr.g_x, &gr.g_y, &gr.g_w, &gr.g_h);
         }
         break;
      case 2:               /* Curve version   */
         y=w_[top_w_nr].y_wpos-w_[top_w_nr].y_alt;
         for(i=0;i<XY_MAX-1;i+=2)
         {
            xy[i]=k_[2].x_min+x_off+i*mul+1;
            xy[i+1]=xy[i+1]+y;
         }
         graf_mode=1;
         break;
   }
   for(i = XY_MAX-1; i>1; i-=2)
      xy[i]=xy[i-2];
   zaehler=-zaehler;
   xy[1]=xy[3]+ xy[11]/3-xy[15]/3+ xy[23]/3-xy[25]/3+ xy[41]/3-xy[47]/3+
         xy[61]/3-xy[65]/3+ xy[97]/3-xy[99]/3+ zaehler;
   if(xy[1]<k_[2].y_min+y_off)
      xy[1]=k_[2].y_min+y_off+9;
   if(xy[1]>k_[2].y_max+y_off)
      xy[1]=k_[2].y_max+y_off-9;
   if (xy[1]>xy[3]+23)
      xy[1]=xy[3]+3;
   if (xy[1]<xy[3]-23)
      xy[1]=xy[3]-3;

   vsl_color(vdi_handle, BLACK);
   wind_get(w_[0].handle, WF_FIRSTXYWH, &gr.g_x, &gr.g_y, &gr.g_w, &gr.g_h);
   while (gr.g_w && gr.g_h)
   {
      rc_grect_to_array(&gr, clxy);
      vs_clip(vdi_handle, TRUE, clxy);
      v_pline(vdi_handle, XY_MAX/2, xy);
      wind_get(w_[0].handle, WF_NEXTXYWH, &gr.g_x, &gr.g_y, &gr.g_w, &gr.g_h);
   }

   vs_clip(vdi_handle, FALSE, clxy);

#if MSDOS
   graf_mouse(M_ON, 0l);
   wind_update(END_UPDATE);
#endif
}

/*----------------------------------------------------------------------*/
/* handle_m1:      Calculate the result of the 1st mouse segment	*/
/*----------------------------------------------------------------------*/
VOID handle_m1(mevent)
MEVENT FAR *mevent;
{
}

/*----------------------------------------------------------------------*/
/* handle_m2:      Calculate the result of the second mouse segment	*/
/*----------------------------------------------------------------------*/
VOID handle_m2(mevent)
MEVENT FAR *mevent;
{
}

/*----------------------------------------------------------------------*/
/* handle_message:      Calculate the result of  GEM-AES   		*/
/*----------------------------------------------------------------------*/
VOID handle_message(mevent)
MEVENT FAR *mevent;
{
   WORD x, y, w, h;

   handle_window(mevent);
 
   mevent->e_m1.g_x=k_[0].x_min+w_[top_w_nr].x_wpos;
   mevent->e_m1.g_y=k_[0].y_min+w_[top_w_nr].y_wpos;
   mevent->e_m2.g_x=k_[1].x_min+w_[top_w_nr].x_wpos;
   mevent->e_m2.g_y=k_[1].y_min+w_[top_w_nr].y_wpos;


   if ( (mevent->e_mepbuf[0] == WM_MOVED || mevent->e_mepbuf[0] == WM_FULLED)
        && (graf_mode != 0))
   {
      wind_get(w_[0].handle, WF_WORKXYWH, &x, &y, &w, &h);
      mevent->e_mepbuf[0] = WM_REDRAW;
      mevent->e_mepbuf[3] = w_[0].handle;
      mevent->e_mepbuf[4] = x;
      mevent->e_mepbuf[5] = y;
      mevent->e_mepbuf[6] = w;
      mevent->e_mepbuf[7] = h;
      handle_window(mevent);
      graf_mode = 2;
   }

   {               /* Clipping-Rectangle   */
      WORD clip[4], x, y;

      wind_get(w_[0].handle, WF_WXYWH, &clip[0], &clip[1], &x, &y);
      clip[2]=clip[0]+x-1;
      clip[3]=clip[1]+y-1;
      if (x>k_[2].x_max)
         clip[2]=clip[0]+k_[2].x_max-1;
      else
         clip[2]=clip[0]+x-1;
      if (y>k_[2].y_max)
         clip[3]=clip[1]+k_[2].y_max-1;
      else
         clip[3]=clip[1]+y-1;
      vs_clip(vdi_handle, TRUE, clip);
   }
}

/*----------------------------------------------------------------------*/
/* handle_menu:      Calculate the results of the Menu List		*/
/*----------------------------------------------------------------------*/
VOID handle_menu(mevent)
MEVENT FAR *mevent;
{
   if (mevent->e_mepbuf[3] == 3)
   {
      dialog_box(INFOBOX);
   }

   switch (mevent->e_mepbuf[4])               /* Calculation of Menu entries */
   {
      case TIMERON:                           /* Set Timer on                */
         mevent->e_flags |= MU_TIMER;         /* Set Timer bit               */
         menu_icheck(menu_tree, TIMERON,1);   /* TIMERON - ON entry          */
         menu_icheck(menu_tree, TIMEROFF,0);  /* TIMEROFF -OFF entry 	     */
         break;
      case S1000:
         mevent->e_time = 1000;         /* Set timer to 1000ms */
         menu_icheck(menu_tree, S1000,    1);
         menu_icheck(menu_tree, S500,    0);
         menu_icheck(menu_tree, S250,   0);
         menu_icheck(menu_tree, S100,  0);
         menu_icheck(menu_tree, S10,  0);
         menu_icheck(menu_tree, S1,  0);
         menu_icheck(menu_tree, S0, 0);
         break;
      case S500:
         mevent->e_time=500;         /* Set Timer to 500ms */
         menu_icheck(menu_tree, S1000,    0);
         menu_icheck(menu_tree, S500,    1);
         menu_icheck(menu_tree, S250,   0);
         menu_icheck(menu_tree, S100,  0);
         menu_icheck(menu_tree, S10,  0);
         menu_icheck(menu_tree, S1,  0);
         menu_icheck(menu_tree, S0, 0);
         break;
      case S250:            /* Set Timer to 250ms */
         mevent->e_time=250;
         menu_icheck(menu_tree, S1000,    0);
         menu_icheck(menu_tree, S500,    0);
         menu_icheck(menu_tree, S250,   1);
         menu_icheck(menu_tree, S100,  0);
         menu_icheck(menu_tree, S10,  0);
         menu_icheck(menu_tree, S1,  0);
         menu_icheck(menu_tree, S0, 0);
         break;
      case S100:            /* Set Timer to 100ms */
         mevent->e_time=100;
         menu_icheck(menu_tree, S1000,    0);
         menu_icheck(menu_tree, S500,    0);
         menu_icheck(menu_tree, S250,   0);
         menu_icheck(menu_tree, S100,  1);
         menu_icheck(menu_tree, S10,  0);
         menu_icheck(menu_tree, S1,  0);
         menu_icheck(menu_tree, S0, 0);
         break;
      case S10:            /* Set Timer to  10ms */
         mevent->e_time=10;
         menu_icheck(menu_tree, S1000,    0);
         menu_icheck(menu_tree, S500,    0);
         menu_icheck(menu_tree, S250,   0);
         menu_icheck(menu_tree, S100,  0);
         menu_icheck(menu_tree, S10,  1);
         menu_icheck(menu_tree, S1,  0);
         menu_icheck(menu_tree, S0, 0);
         break;
      case S1:            /* Set Timer to  1ms */
         mevent->e_time=1;
         menu_icheck(menu_tree, S1000,    0);
         menu_icheck(menu_tree, S500,    0);
         menu_icheck(menu_tree, S250,   0);
         menu_icheck(menu_tree, S100,  0);
         menu_icheck(menu_tree, S10,  0);
         menu_icheck(menu_tree, S1,  1);
         menu_icheck(menu_tree, S0, 0);
         break;
      case S0:            /* Set Timer to 0ms */
         mevent->e_time=0;
         menu_icheck(menu_tree, S1000,    0);
         menu_icheck(menu_tree, S500,    0);
         menu_icheck(menu_tree, S250,   0);
         menu_icheck(menu_tree, S100,  0);
         menu_icheck(menu_tree, S10,  0);
         menu_icheck(menu_tree, S1,  0);
         menu_icheck(menu_tree, S0, 1);
         break;
      case TIMEROFF:            /* Timer stop */
         mevent->e_flags&=~MU_TIMER;      /* Timer-Bit reset */
         menu_icheck(menu_tree, TIMERON, 0);
         menu_icheck(menu_tree, TIMEROFF, 1);
         break;
      case M1ON:            /* Set Mouse control to 1 */
         mevent->e_flags|=MU_M1;      /* Set M1-Bit */
         menu_icheck(menu_tree, M1ON, 1);
         menu_icheck(menu_tree, M1OFF, 0);
            /* Set and calculate M1-Coordinates */
         mevent->e_m1.g_x=k_[0].x_min+w_[top_w_nr].x_wpos;
         mevent->e_m1.g_y=k_[0].y_min+w_[top_w_nr].y_wpos;
         mevent->e_m1.g_w=k_[0].x_max-k_[0].x_min;
         mevent->e_m1.g_h=k_[0].y_max-k_[0].y_min;
         break;
      case M1EINTRI:            /* Result at start */
         mevent->e_m1flags=0;
         menu_icheck(menu_tree, M1EINTRI, 1);
         menu_icheck(menu_tree, M1AUSTRI, 0);
         break;
      case M1AUSTRI:            /* Result at the end */
         mevent->e_m1flags=1;
         menu_icheck(menu_tree, M1EINTRI, 0);
         menu_icheck(menu_tree, M1AUSTRI, 1);
         break;
      case M1OFF:            /* M1 Stop */
         mevent->e_flags&=~MU_M1;      /* Set M1-Bit */
         menu_icheck(menu_tree, M1ON, 0);
         menu_icheck(menu_tree, M1OFF, 1);
         break;
      case M2ON:            /* Set Mouse control to 2 */
         mevent->e_flags|=MU_M2;      /* Set M2-Bit */
         menu_icheck(menu_tree, M2ON, 1);
         menu_icheck(menu_tree, M2OFF, 0);
            /* Set and calculate M2-Coordinates */
         mevent->e_m2.g_x=k_[1].x_min+w_[top_w_nr].x_wpos;
         mevent->e_m2.g_y=k_[1].y_min+w_[top_w_nr].y_wpos;
         mevent->e_m2.g_w=k_[1].x_max-k_[1].x_min;
         mevent->e_m2.g_h=k_[1].y_max-k_[1].y_min;
         break;
      case M2EINTRI:            /* Results at start */
         mevent->e_m2flags=0;
         menu_icheck(menu_tree, M2EINTRI,1);
         menu_icheck(menu_tree, M2AUSTRI,0);
         break;
      case M2AUSTRI:            /* Results at the end */
         mevent->e_m2flags=1;
         menu_icheck(menu_tree, M2EINTRI,0);
         menu_icheck(menu_tree, M2AUSTRI,1);
         break;
      case M2OFF:            /* Turn off M2 */
         mevent->e_flags&=~MU_M2;      /* Set M2-Bit */
         menu_icheck(menu_tree, M2ON, 0);
         menu_icheck(menu_tree, M2OFF, 1);
         break;
      case BON:            /* Turn on Button statistics */
         mevent->e_flags|=MU_BUTTON;      /* Button-Bit */
         menu_icheck(menu_tree, BON, 1);
         menu_icheck(menu_tree, BOFF, 0);
         break;
      case CLICK0:            /* Wait for 0-Clicks */
         mevent->e_bclk=0;
         menu_icheck(menu_tree, CLICK0, 1);
         menu_icheck(menu_tree, CLICK1, 0);
         menu_icheck(menu_tree, CLICK2, 0);
         break;
      case CLICK1:            /* Wait for the first Click */
         mevent->e_bclk=1;
         menu_icheck(menu_tree, CLICK0, 0);
         menu_icheck(menu_tree, CLICK1, 1);
         menu_icheck(menu_tree, CLICK2, 0);
         break;
      case CLICK2:            /* Wait for the second Click */
         mevent->e_bclk=2;
         menu_icheck(menu_tree, CLICK0, 0);
         menu_icheck(menu_tree, CLICK1, 0);
         menu_icheck(menu_tree, CLICK2, 1);
         break;
      case B1:            /* Stop Button 1 statistics */
         if(mevent->e_bmsk&1)
         {
            mevent->e_bmsk&=~1;         /* Stop       */
            menu_icheck(menu_tree, B1, 0);
         }
         else
         {
            mevent->e_bmsk|=1;         /* Start       */
            menu_icheck(menu_tree, B1, 1);
         }
         break;
      case B2:            /* Button 2 -/ Stop         */
         if (mevent->e_bmsk&2)
         {
            mevent->e_bmsk&=~2;         /* Stop       */
            menu_icheck(menu_tree, B2, 0);
         }
         else
         {
            mevent->e_bmsk|=2;         /* Turn on     */
            menu_icheck(menu_tree, B2, 1);
         }
         break;
      case B3:            /* Button 3 -/ Stop  	      */
         if (mevent->e_bmsk&4)
         {
            mevent->e_bmsk&=~4;         /* Stop       */
            menu_icheck(menu_tree, B3, 0);
         }
         else
         {
            mevent->e_bmsk|=4;         /* Turn on     */
            menu_icheck(menu_tree, B3, 1);
         }
         break;
      case GEDRUECK:            /* Wait for Printer   */
         mevent->e_bst=1;
         menu_icheck(menu_tree, GEDRUECK, 1);
         menu_icheck(menu_tree, NGEDRUE, 0);
         break;
      case NGEDRUE:            /* Wait for printing time-out   */
         mevent->e_bst=0;
         menu_icheck(menu_tree, GEDRUECK, 0);
         menu_icheck(menu_tree, NGEDRUE, 1);
         break;
      case BOFF:            /* Button Statistics Stop  */
         mevent->e_flags&=~MU_BUTTON;
         menu_icheck(menu_tree, BON, 0);
         menu_icheck(menu_tree, BOFF, 1);
         break;
      case KON:            /* Keyboard - Turn on */
         mevent->e_flags|=MU_KEYBD;
         menu_icheck(menu_tree, KON, 1);
         menu_icheck(menu_tree, KOFF, 0);
         break;
      case KOFF:            /* Keyboard - Stop */
         mevent->e_flags&=~MU_KEYBD;
         menu_icheck(menu_tree, KON, 0);
         menu_icheck(menu_tree, KOFF, 1);
         break;
   }
               /* Restore Menu state */
   menu_tnormal(menu_tree, mevent->e_mepbuf[3],TRUE);
}

VOID draw_event_return(ev_which, mevent)
WORD ev_which;
MEVENT FAR *mevent;
{
   WORD i;         /* Counter for CASE loop   */
   BYTE flag[20];
   BYTE state[4];
   WORD dummy;
   WORD clxy[4];
   GRECT gr;

#if MSDOS
   wind_update(BEG_UPDATE);
   graf_mouse(M_OFF, 0l);
#endif

   vst_height(vdi_handle, 11, &dummy, &dummy, &dummy, &dummy);
   vst_color(vdi_handle, 2);

   for(i=0; i<7; i++)         /* Place '-' into string   */
      flag[i]='-';
     flag[7]=0;

   if (ev_which&MU_M1)
      flag[1]='1';
   if (ev_which&MU_M2)
      flag[2]='2';
   if (ev_which&MU_KEYBD)
      flag[4]='K';
   if (ev_which&MU_MESAG)
      if (mevent->e_mepbuf[0] == MN_SELECTED)
         flag[6]='M';
      else
         flag[5]='W';
   if (ev_which&MU_BUTTON)
      flag[3]='B';
   if (ev_which&MU_TIMER)
      flag[0]='T';

   state[0] = hex_strs[mevent->e_br & 0xf];
   state[1] = hex_strs[mevent->e_mb & 0xf];
   state[2] = hex_strs[mevent->e_ks & 0xf];
   state[3] = 0;

   strcat( flag, state );
/*
 * Button release state 
 */
   wind_get(w_[0].handle, WF_FIRSTXYWH, &gr.g_x, &gr.g_y, &gr.g_w, &gr.g_h);
   while (gr.g_w && gr.g_h)
   {
     rc_grect_to_array(&gr, clxy);
     vs_clip(vdi_handle, TRUE, clxy);

     v_gtext(vdi_handle,w_[top_w_nr].x_wpos + 2,w_[top_w_nr].y_wpos + 12, flag);

     wind_get(w_[0].handle, WF_NEXTXYWH, &gr.g_x, &gr.g_y, &gr.g_w, &gr.g_h);
   }

#if MSDOS
   graf_mouse(M_ON, 0l);
   wind_update(END_UPDATE);
#endif
}

/*----------------------------------------------------------------------*/
/* handle_event():   Task distribution to various functions             */
/*----------------------------------------------------------------------*/
WORD handle_event(ev_which, mevent)
WORD ev_which;
MEVENT FAR *mevent;
{
   draw_event_return(ev_which, mevent);   /* Output button state   */

   if (ev_which&MU_M1)
      handle_m1(mevent);
   if (ev_which&MU_M2)
      handle_m2(mevent);
   if (ev_which&MU_KEYBD)
      handle_keyboard(mevent);
   if (ev_which&MU_BUTTON)
      handle_button(mevent);
   if (ev_which&MU_MESAG)
      if (mevent->e_mepbuf[0] == MN_SELECTED)
         handle_menu(mevent);
      else
         handle_message(mevent);
   if (ev_which&MU_TIMER)
      handle_timer(mevent);
               /* Application  */
               /* Window       */
   if (((mevent->e_mepbuf[0]==MN_SELECTED) && (mevent->e_mepbuf[4]==QUIT))||
   (mevent->e_mepbuf[0]==WM_CLOSED))
      return( /*done=*/ TRUE);
   else
      return( /*done=*/ FALSE);
}
