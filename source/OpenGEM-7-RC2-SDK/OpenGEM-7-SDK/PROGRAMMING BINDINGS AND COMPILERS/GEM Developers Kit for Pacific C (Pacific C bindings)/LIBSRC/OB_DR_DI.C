/*****************************************************************************
 *
 *	-------------------------------------------------------------
 *	GEM Desktop					  Version 3.1
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1985 - 1989		Digital Research Inc.
 *	-------------------------------------------------------------
 *
 *****************************************************************************/

#include "portab.h"
#include "ppdgem.h"


/******************************************************************************
 *
 * Draw a dialog with an optional growing box
 *
 ******************************************************************************/

VOID ob_draw_dialog(tree, xl, yl, wl, hl)
OBJECT FAR *tree;
WORD xl, yl, wl, hl;
{
   GRECT rec;

   form_center(tree, &rec.g_x, &rec.g_y, &rec.g_w, &rec.g_h);
   form_dial(FMD_START, rec.g_x, rec.g_y, rec.g_w, rec.g_h,
                        rec.g_x, rec.g_y, rec.g_w, rec.g_h);
   if (xl || yl || wl || hl)
      form_dial(FMD_GROW, xl, yl, wl, hl,
                   rec.g_x - 5, rec.g_y - 5,
                   rec.g_w + 10, rec.g_h + 10);
   objc_draw(tree, ROOT, MAX_DEPTH,
             rec.g_x - 5, rec.g_y - 5, rec.g_w + 10, rec.g_h + 10);
}

