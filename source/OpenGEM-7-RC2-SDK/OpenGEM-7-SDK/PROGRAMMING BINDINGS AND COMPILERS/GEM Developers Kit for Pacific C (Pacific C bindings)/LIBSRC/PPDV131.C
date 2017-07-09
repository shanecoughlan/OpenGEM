/*****************************************************************************
 *
 *	-------------------------------------------------------------
 *	GEM Desktop					  Version 3.1
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1985 - 1989		Digital Research Inc.
 *	-------------------------------------------------------------
 *
 *****************************************************************************/

#include "ppdgem.h"
#include "ppdv0.h"

VOID vs_bkcolor(handle, color)
WORD handle, color;
{
   contrl[0] = 5;
   contrl[1] = 0;
   contrl[3] = 1;
   contrl[5] = 102;
   contrl[6] = handle;

   intin[0] = color;

   vdi();
}

