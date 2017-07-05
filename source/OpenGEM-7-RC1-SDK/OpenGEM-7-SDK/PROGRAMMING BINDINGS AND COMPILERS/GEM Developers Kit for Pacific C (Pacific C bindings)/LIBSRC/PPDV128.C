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

VOID v_ps_halftone(handle, index, angle, frequency)
WORD handle, index, angle, frequency;
{
   contrl[0] = 5;
   contrl[1] = 0;
   contrl[3] = 3;
   contrl[5] = 32;
   contrl[6] = handle;

   intin[0] = index;
   intin[1] = angle;
   intin[2] = frequency;

   vdi();
}

