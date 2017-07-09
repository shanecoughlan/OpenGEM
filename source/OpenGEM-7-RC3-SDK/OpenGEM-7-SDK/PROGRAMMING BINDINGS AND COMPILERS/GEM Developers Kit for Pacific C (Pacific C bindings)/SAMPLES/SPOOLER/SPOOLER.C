/***************************************************************************
 *
 *    GEM/3 Programmers Toolkit - Digital Research, Inc.
 *
 *    Filename: SPOOLER.C
 *
 *    Date: 08/01/89
 *
 **************************************************************************
 *
 * Sample Application to show how to use the message conversation to
 * the CALCLOCK spooler.
 *
 * Version  :  1.00
 * Created  :  02.06.89
 * Author   :  Robert Schneider, DRI GmbH
 *
 * Note:
 *
 *    One must not declare the message buffer for the spooler message
 *    as an auto variable. Because of the fact that AES does not copy
 *    the contents of the message but the pointer to the message, the
 *    message buffer must not be modified as long as the spooler has
 *    given his acknowledge.
 *
 *    One has to check, whether the file the user choosed exists.
 *    If an inexisting filename will be given to the spooler, the
 *    system hangs up.
 *
 *****************************************************************************/

#include <portab.h>
#include <aes.h>
#include "spooler.h"
#include <stdlib.h>
#include <string.h> 
#include <dos.h>
 
EXTERN BYTE *strrchr();

MLOCAL OBJECT FAR *menu;         /* holds tree for menu bar                   */
MLOCAL WORD gl_apid;             /* application id of us                      */
MLOCAL WORD spool[8];            /* message buffer for appl_write !!!!!!!!!!! */
MLOCAL BYTE path[80];            /* buffer for path name                      */
MLOCAL BYTE name[20];            /* buffer for file name                      */
MLOCAL BYTE fullname[100];       /* buffer for complete file name             */

WORD file_select(BYTE *path, BYTE *file);
WORD call_spooler(BYTE *pname);

VOID main()
{
   WORD which; //,id;
   UWORD mx, my, mb, ks, br;
   UWORD kr;
//   BYTE s[200];
   WORD msg[8];
   WORD done = FALSE;

/*
 * So file_select gets current directory by itself
 */
   path[0] = name[0] = 0;

/*
 * Initialize AES for our application
 */
   gl_apid = appl_init(NULL);

/*
 * Load resources for the menu bar
 */
   if (!rsrc_load("SPOOLER.RSC"))
   {
      form_alert(1,"[3][SPOOLER.RSC not found!][Abort]");
      appl_exit();
      exit(1);
   }

/*
 * Get pointer to tree and show menu bar
 */
   rsrc_gaddr(R_TREE,SPMENU,(LPVOID *)&menu);
   menu_bar(menu,TRUE);

/*
 * Wait until X-mas
 */
   do
   {
      which = evnt_multi(MU_MESAG,
                         0, 0, 0, 
                         0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0,
                         msg,
                         0, 0,
                         &mx, &my, &mb, &ks, &kr, &br);

/*
 * Is it a message event?
 */
      if (which & MU_MESAG)
      {
         switch (msg[0])
         {
            case 101:
               form_alert(1,"[1][Spooler acknowledged!][OK]");
               break;

            case MN_SELECTED:
               switch(msg[4])
               {
                  case MNABOUT:
                     form_alert(1,"[0][Spooler Test Application|    ½1989 by DR GmbH|Author:  Robert Schneider][OK]");
                     break;

                  case MNQUIT:
                     done = TRUE;
                     break;

                  case MNSPOOL:
/*
 * Get file name
 */
                     if (!file_select(path,name))
                        break;
                     strcpy(fullname,path);
                     strcat(fullname,"\\");
                     strcat(fullname,name);
/*
 * Check whether file exists or not
 */
                     if (!dos_sfirst(fullname,0xff))
                        form_alert(1,"[3][File not found!][Argh]");
                     else
                        call_spooler(fullname);
                     break;

                  default:
                     break;
               } /* switch (msg[4]) */

/*
 * Unselect menu title
 */
               menu_tnormal(menu,msg[3],TRUE);
               break;

            default:
               break;   
         } /* switch (msg[0]) */
      } /* if which & MU_MESAG */
   }while (!done);

/*
 * That's it, folks
 */
   appl_exit();
   exit(0);
}

WORD call_spooler(pname)
BYTE *pname;
{
   WORD id;
/*
 * find out the id of the CALCLOCK accessory
 */
   if ((id = appl_find("CALCLOCK")) < 0)
   {
      form_alert(1,"[1][CALCLOCK is not present!][OK]");
      return;
   }

/*
 * Setup the message buffer
 */
   spool[0] = 100;
   spool[1] = gl_apid;
   spool[2] = -1;
   spool[3] = strlen(pname);
   spool[4] = FPOFF((BYTE FAR *)pname);
   spool[5] = FPSEG((BYTE FAR *)pname);
   spool[6] = 1;
   spool[7] = 0;

/*
 * Send message to spooler
 */
   appl_write(id,16,spool);
}

WORD file_select(path,file)
BYTE *path;
BYTE *file;
{
   REG BYTE *p;
   BYTE cwd[100];
   WORD drive;
   WORD button;

/*
 * If path is not set, get current drive and path
 */
   if (path[0] == 0)
   {
      drive  = dos_gdrv();
      cwd[0] = drive + 'A';
      cwd[1] = ':';
      cwd[2] = '\\';
      dos_gdir(drive + 1, &cwd[3]);
      if (cwd[3] == '\\')
         strcpy(&cwd[2],&cwd[3]);
   }
   else
      strcpy(cwd,path);

   if (cwd[strlen(cwd) - 1] != '\\')
      strcat(cwd,"\\");

   strcat(cwd,"*.*");

/*
 * Get file from file selector box
 */
   fsel_input(cwd,file,&button);

   if (!button)
      return(FALSE);

/*
 * split path and name
 */
   if (p = strrchr(cwd,'\\'))
      *p = 0;

   strcpy(path,cwd);

   return(TRUE);
}
