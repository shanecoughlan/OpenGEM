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
#include "aes.h"
#include "oblib.h"


/******************************************************************************
 *
 * Undraw a dialog with an optional shrinking box
 *
 ******************************************************************************/

VOID ob_undraw_dialog(tree, xl, yl, wl, hl)
OBJECT FAR *tree;
WORD xl, yl, wl, hl;
{
   GRECT rec;

   ob_xywh(tree, 0, &rec);
   form_dial(FMD_FINISH,rec.g_x - 5, rec.g_y - 5, rec.g_w + 10, rec.g_h + 10,
                        rec.g_x - 5, rec.g_y - 5, rec.g_w + 10, rec.g_h + 10);
   if (xl || yl || wl || hl)
      form_dial(FMD_SHRINK, xl, yl, wl, hl, rec.g_x, rec.g_y, rec.g_w, rec.g_h);
}
