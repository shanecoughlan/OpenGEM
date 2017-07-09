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

VOID v_set_app_buff(address, nparagraphs)
VOID FAR *address;
WORD nparagraphs;
{
   contrl[0] = -1;
   contrl[1] = 0;
   contrl[3] = 3;

   intin[0] = LLOWD((LONG)address);
   intin[1] = LHIWD((LONG)address);
   intin[2] = nparagraphs;

   vdi();
}

