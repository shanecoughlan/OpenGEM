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
 * Convert x,y,w,h to upper left x,y and lower right x,y for raster ops
 *
 ******************************************************************************/

VOID rc_grect_to_array(area, array)
GRECT FAR *area;
WORD FAR *array;
{
   *array++ = area->g_x;
   *array++ = area->g_y;
   *array++ = area->g_x + area->g_w - 1;
   *array = area->g_y + area->g_h - 1;
}
