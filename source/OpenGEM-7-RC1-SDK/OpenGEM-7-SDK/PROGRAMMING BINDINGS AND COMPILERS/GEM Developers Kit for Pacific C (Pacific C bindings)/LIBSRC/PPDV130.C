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

VOID v_topbot(handle, height, char_width, char_height, cell_width, cell_height)
WORD handle, height;
WORD FAR *char_width;
WORD FAR *char_height;
WORD FAR *cell_width;
WORD FAR *cell_height;
{
   contrl[0] = 5;
   contrl[1] = 1;
   contrl[3] = 0;
   contrl[5] = 0x4845;
   contrl[6] = handle;

   ptsin[0] = 0;
   ptsin[1] = height;

   vdi();

   *char_width = intout[0];
   *char_height = intout[1];
   *cell_width = intout[2];
   *cell_height = intout[3];
}

