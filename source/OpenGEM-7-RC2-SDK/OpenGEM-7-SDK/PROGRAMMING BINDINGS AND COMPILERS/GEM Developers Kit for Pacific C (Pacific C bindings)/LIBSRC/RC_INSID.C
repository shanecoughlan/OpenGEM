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
#include "rclib.h"


/******************************************************************************
 *
 * Determine if x,y is in rectangle
 *
 ******************************************************************************/

WORD rc_inside(x, y, pt)
WORD x, y;
GRECT FAR *pt;
{
   if (x < pt->g_x)
      return(FALSE);
   if (y < pt->g_y)
      return(FALSE);
   if (x >= pt->g_x + pt->g_w)
      return(FALSE);
   if (y >= pt->g_y + pt->g_h)
      return(FALSE);

   return(TRUE);
}

