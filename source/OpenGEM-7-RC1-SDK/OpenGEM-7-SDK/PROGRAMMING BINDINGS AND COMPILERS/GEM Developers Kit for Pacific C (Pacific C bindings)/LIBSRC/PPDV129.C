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

VOID v_setrgbi(handle, primtype, r, g, b, i)
WORD handle, primtype, r, g, b, i;
{
   contrl[0] = 5;
   contrl[1] = 0;
   contrl[3] = 5;
   contrl[5] = 0x4844;
   contrl[6] = handle;

   intin[0] = primtype;
   intin[1] = r;
   intin[2] = g;
   intin[3] = b;
   intin[4] = i;

   vdi();
}


