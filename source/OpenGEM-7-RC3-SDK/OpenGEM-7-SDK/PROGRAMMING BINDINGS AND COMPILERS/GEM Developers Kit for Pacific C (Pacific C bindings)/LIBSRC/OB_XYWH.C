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
 * Get position and dimensions of a object
 *
 ******************************************************************************/

VOID ob_xywh(tree, index, rec)
OBJECT FAR *tree;
WORD index;
GRECT FAR *rec;
{
   WORD lx, ly;

   objc_offset(tree,index, &lx, &ly);

   rec->g_x = lx;
   rec->g_y = ly;
   rec->g_w = tree[index].ob_width;
   rec->g_h = tree[index].ob_height;
}

