/*************************************************************************
*
*  GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*  Modulename: FDTEST.C
*  Date:       07-27-89
*  Author:     Robert Schneider, DRI GmbH
*
*************************************************************************/

#include "portab.h"
#include "aes.h"
#include "oblib.h"
#include "fdtest.h"

#include <stdlib.h>
#include	<stdio.h>

main()
{
   OBJECT FAR *tree;
   WORD ret;
   BYTE s[200];
   WORD a;

   appl_init(NULL);

   if (!rsrc_load("FDTEST.RSC"))
   {
      form_alert(1,"[3][RSC-file FDTEST|not found!][Abort]");
      appl_exit();
      exit(0);
   }

   rsrc_gaddr(R_TREE, FDTEST, (LPVOID *)&tree);

   for (a = FDSTR1; a <= FDSTR7; a++)
      ob_get_text(tree, a, TRUE);

   do
   {
      ob_draw_dialog(tree, 0, 0, 0, 0);
      ret = form_exdo(tree, 0);
      ob_undostate(tree, ret & 0x7fff, SELECTED);
      ob_undraw_dialog(tree, 0, 0, 0, 0);

      sprintf(s,"[0][Exit-Button = $%04x][Rerun|Exit]",ret);
      ret = form_alert(1,s);
   } while (ret != 2);

   appl_exit();
   exit(0);
}
