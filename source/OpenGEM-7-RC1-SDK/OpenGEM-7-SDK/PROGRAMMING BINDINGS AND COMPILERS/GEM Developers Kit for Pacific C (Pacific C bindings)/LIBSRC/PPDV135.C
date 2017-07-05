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

WORD v_bez_qual(handle, prcnt)
WORD handle, prcnt;
{
   contrl[0] = 5;
   contrl[1] = 0;
   contrl[3] = 3;
   contrl[5] = 99;
   contrl[6] = handle;

   intin[0] = 32;
   intin[1] = 1;
   intin[2] = prcnt;

   vdi();

   return (intout[0]);
}

