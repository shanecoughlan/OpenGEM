/************************************************************************/
/*   File:   FREEMEM.C                                                  */
/************************************************************************/
/*                                                                      */
/*           GGGGG        EEEEEEEE     MM      MM                       */
/*         GG             EE           MMMM  MMMM                       */
/*         GG   GGG       EEEEE        MM  MM  MM                       */
/*         GG   GG        EE           MM      MM                       */
/*           GGGGG        EEEEEEEE     MM      MM                       */
/*                                                                      */
/************************************************************************/
/*                                                                      */
/*           +--------------------------+                               */
/*           | Digital Research, Inc.   |                               */
/*           | 70 Garden Court          |                               */
/*           | Monterey, CA.     93940  |                               */
/*           +--------------------------+                               */
/*                                                                      */
/*   The  source code  contained in  this listing is a non-copyrighted  */
/*   work which  can be  freely used.  In applications of  this source  */
/*   code you  are requested to  acknowledge Digital Research, Inc. as  */
/*   the originator of this code.                                       */
/*                                                                      */
/*   Author:    Robert Schneider                                        */
/*   PRODUCT:   GEM Sample Accessory                                    */
/*   Module:    FREEMEM.C                                               */
/*   Version:   1.0                                                     */
/*   Date:      08/01/89                                                */
/************************************************************************/

#include "portab.h"
#include "aes.h"
#include "oblib.h"
#include "rclib.h"
#include "freemem.h"
#include <dos.h>
#include <stdio.h>
#include <time.h>

GLOBAL WORD min();         /* required by rc_intersect                        */
GLOBAL WORD max();         /* required by rc_intersect                        */

MLOCAL VOID hndl_mesag();  /* handle a message like AC_OPEN, WM_REDRAW ...    */
MLOCAL VOID do_redraw();   /* do a redraw of the system-info-window           */

MLOCAL WORD gl_wchar;      /* Width of a char, required for window-snapping   */
MLOCAL WORD gl_hchar;      /* Height of a char,       - "" -                  */
MLOCAL WORD gl_apid;       /* Application ID                                  */
MLOCAL WORD gl_myacc;      /* Accessory ID                                    */
MLOCAL WORD msg_buf[8];    /* Message-Buffer                                  */
MLOCAL WORD win = 0;       /* Window-Handle                                   */
MLOCAL GRECT desk;         /* Rectangle of whole Desktop                      */

MLOCAL BYTE *gl_title   =  " System-Information ";

#ifdef GERMAN
MLOCAL BYTE *tage[] = { "So","Mo","Di","Mi","Do","Fr","Sa" };
#else
MLOCAL BYTE *tage[] = { "Su","Mo","Tu","We","Th","Fr","Sa" };
#endif

/*
 * Resource, created with RCS, saved as *.RSH, and handled for internal use
 */

#ifdef GERMAN
#define  TXT_MEMORY  "VerfÅgbarer Speicher"
#define  TXT_DATE    "Datum und Zeit"
#define  TXT_OS      "Betriebssystemversion"
#else
#define  TXT_MEMORY  "Available Memory"
#define  TXT_DATE    "Date and Time"
#define  TXT_OS      "Version of OS"
#endif

#define  TXT_COPYR   "(C)1989 Digital Research, Inc."

MLOCAL BYTE space_mem[20];
MLOCAL BYTE space_date[40];
MLOCAL BYTE space_os[10];
MLOCAL BYTE nullchar[2] = { 0, 0 };

TEDINFO rs_tedinfo[] = {

TXT_MEMORY, nullchar, nullchar, 5, 6, 2, 0x1180, 0x0, -1, 21,1,
space_mem,  nullchar, nullchar, 3, 6, 2, 0x1181, 0x0, -1, 10,1,
TXT_DATE,   nullchar, nullchar, 5, 6, 2, 0x1180, 0x0, -1, 15,1,
space_date, nullchar, nullchar, 3, 6, 2, 0x1181, 0x0, -1, 26,1,
TXT_OS,     nullchar, nullchar, 5, 6, 2, 0x1180, 0x0, -1, 22,1,
space_os,   nullchar, nullchar, 3, 6, 2, 0x1181, 0x0, -1, 14,1,
TXT_COPYR,  nullchar, nullchar, 5, 6, 2, 0x1180, 0x0, -1, 30,1

};

OBJECT ob_info[] = {

   -1, 1, 7, G_BOX, NONE, NORMAL, (LPVOID)0x11100L, 0,0, 29,7,
   2, -1, -1, G_TEXT, NONE, NORMAL, (BYTE FAR *)&rs_tedinfo[0], 0,0, 29,1,
   3, -1, -1, G_TEXT, NONE, NORMAL, (BYTE FAR *)&rs_tedinfo[1], 0,1, 29,1,
   4, -1, -1, G_TEXT, NONE, NORMAL, (BYTE FAR *)&rs_tedinfo[2], 0,2, 29,1,
   5, -1, -1, G_TEXT, NONE, NORMAL, (BYTE FAR *)&rs_tedinfo[3], 0,3, 29,1,
   6, -1, -1, G_TEXT, NONE, NORMAL, (BYTE FAR *)&rs_tedinfo[4], 0,4, 29,1,
   7, -1, -1, G_TEXT, NONE, NORMAL, (BYTE FAR *)&rs_tedinfo[5], 0,5, 29,1,
   0, -1, -1, G_TEXT, LASTOB, NORMAL, (BYTE FAR *)&rs_tedinfo[6], 0,6, 29,1
};

main()
{
   WORD a,dummy;
   UWORD udummy;
   WORD x,y;
   WORD ev_which;

/*
 * Initialize application and register accessory in the menu-bar
 */
   gl_apid = appl_init(NULL);
   gl_myacc = menu_register(gl_apid,"  System-Info");

/*
 * Relocate xy-coordinates of objects
 */
   a = -1;
   do
   {
      a++;
      rsrc_obfix(ob_info,a);
   }while (!(ob_info[a].ob_flags & LASTOB));

/*
 * Find out width and height of a char, and dimensions of desktop
 */
   graf_handle(&gl_wchar, &gl_hchar, &dummy, &dummy);
   wind_get(0,WF_WXYWH,&desk.g_x,&desk.g_y,&desk.g_w,&desk.g_h);

/*
 * Wait forever for events concerning this application
 */
   FOREVER
   {
      ev_which = evnt_multi((MU_MESAG | ((win) ? MU_TIMER : 0)),
                            2,1,1,
                            0,0,0,0,0,
                            0,0,0,0,0,
                            msg_buf,
                            1000,0,
                            &udummy, &udummy, &udummy, &udummy, &udummy, &udummy);

/*
 * If timer-event, do a pseudo-message for updating date and time
 */
      if ((ev_which & MU_TIMER) && !(ev_which & MU_MESAG) && win)
      {
         objc_offset(ob_info,DATTXT,&x,&y);
         msg_buf[0] = WM_REDRAW;
         msg_buf[1] = 0x55aa;
         msg_buf[3] = win;
         msg_buf[4] = x;
         msg_buf[5] = y;
         msg_buf[6] = ob_info[DATTXT].ob_width;
         msg_buf[7] = ob_info[DATTXT].ob_height;
         ev_which |= MU_MESAG;
      }

/*
 * User moved, sized or closed something
 */
      if (ev_which & MU_MESAG)
         hndl_mesag(msg_buf);
   }
}

MLOCAL VOID dos_version(WORD *vh, WORD *vl, WORD *oem, LONG *user)
{
	union REGS rgi, rgo;

	rgi.x.ax=0x3000;
	intdos(&rgi, &rgo);

	*vh = rgo.h.al;	// Major version can't be 1.x or GEM wouldn't be running
	*vl = rgo.h.ah;
	*oem= rgo.h.bh;
	*user=(((LONG)(rgo.h.bl)) << 16) | rgo.x.cx;	
}

MLOCAL VOID hndl_mesag( msg_buf )
WORD msg_buf[];
{
   GRECT open;
   WORD vl,vh,oem;
   LONG user;
   time_t t;
   struct tm *ptm;
   
   switch ( msg_buf[0] )
   {
      case AC_OPEN:
         if (msg_buf[4] != gl_myacc)
            return;
         if (win)
         {
            wind_set(win,WF_TOP,win, 0, 0, 0);
            return;
         }

         win = wind_create(CLOSER|MOVER|NAME, 0, 0,
               ob_info[0].ob_width, ob_info[0].ob_height);
         wind_set(win, WF_NAME, FPOFF(gl_title), FPSEG(gl_title), 0, 0);
         wind_calc(0,(CLOSER|MOVER|NAME),
                  40, 60, ob_info[0].ob_width, ob_info[0].ob_height,
                  &open.g_w, &open.g_y, &open.g_w, &open.g_h);
         open.g_x = (desk.g_w - open.g_w - 10) / gl_wchar * gl_wchar;
         open.g_y = open.g_y / gl_hchar * gl_hchar;
         wind_open(win, open.g_x, open.g_y, open.g_w, open.g_h);
         break;

      case WM_REDRAW:
#if !MS_C
         sprintf(space_mem, " %ld ", dos_avail());
#endif
         time(&t);
         ptm = localtime(&t);

#if !MS_C
         sprintf(space_date,
                 "%s, %02d.%02d.%04d - %02d:%02d:%02d",
                 tage[ptm->tm_wday], ptm->tm_mday, ptm->tm_mon, 
                 ptm->tm_year + 1900, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
#endif
         dos_version(&vh,&vl,&oem,&user);
#if !MS_C
         sprintf(space_os, "V%d.%02d", vh, vl);
#endif
         wind_update(BEG_UPDATE);
         do_redraw(&msg_buf[4]);
         wind_update(END_UPDATE);
         break;

      case WM_TOPPED:
         wind_set(win, WF_TOP, win, 0, 0, 0);
         break;

      case WM_SIZED:
      case WM_MOVED:
         msg_buf[4] = msg_buf[4] / gl_wchar * gl_wchar;
         msg_buf[5] = msg_buf[5] / gl_hchar * gl_hchar;
         wind_set(win,WF_CXYWH,msg_buf[4],msg_buf[5],msg_buf[6],msg_buf[7]);
         break;

      case AC_CLOSE:
         if (msg_buf[3] == gl_myacc)
            win = 0;
         break;

      case WM_CLOSED:
         if (msg_buf[3] != win)
            break;

         if (!win)
            break;
         wind_close(win);
         wind_delete(win);
         win = 0;
         break;

      default:
         break;
   }

   return;
}

MLOCAL VOID do_redraw(prect)
GRECT *prect;
{
   GRECT box;

   wind_get(win, WF_WXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
   ob_info[0].ob_x = box.g_x;
   ob_info[0].ob_y = box.g_y;
   wind_get(win, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
   while (box.g_w || box.g_h)
   {
      if (rc_intersect(prect, &box))
         if (rc_intersect(&desk, &box))
            objc_draw(ob_info,(msg_buf[1] == 0x55aa) ? DATTXT : ROOT,MAX_DEPTH,box.g_x, box.g_y, box.g_w, box.g_h);

      wind_get(win, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
   }
}
