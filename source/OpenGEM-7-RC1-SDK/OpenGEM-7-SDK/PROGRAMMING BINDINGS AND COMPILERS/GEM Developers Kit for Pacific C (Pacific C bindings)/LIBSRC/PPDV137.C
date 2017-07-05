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

VOID vs_grayoverride(handle, grayval)
WORD handle, grayval;
{
   contrl[0] = 133;
   contrl[1] = 0;
   contrl[3] = 1;
   contrl[6] = handle;

   intin[0] = grayval;

   vdi();
}


