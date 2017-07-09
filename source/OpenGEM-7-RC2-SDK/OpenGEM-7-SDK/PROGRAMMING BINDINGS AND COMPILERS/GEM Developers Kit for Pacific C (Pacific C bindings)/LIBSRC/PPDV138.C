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

MLOCAL VOID v_bezier_all(opcode, handle, count, xyarr, bezarr, minmax, npts,
									  nmove)
WORD opcode;
WORD handle, count;
WORD FAR *xyarr;
UBYTE FAR *bezarr;
WORD FAR *minmax;
WORD FAR *npts;
WORD FAR *nmove;
{
   contrl[0] = opcode;
   contrl[1] = count;
   contrl[3] = (count + 1) / 2;
   contrl[5] = 13;
   contrl[6] = handle;

   pblock.intin = (WORD FAR *)bezarr;
   pblock.ptsin = xyarr;

   vdi();

   pblock.intin = intin;
   pblock.ptsin = ptsin;

   *npts = intout[0];
   *nmove = intout[1];
   minmax[0] = ptsout[0];
   minmax[1] = ptsout[1];
   minmax[2] = ptsout[2];
   minmax[3] = ptsout[3];   
}

VOID v_bez(handle, count, xyarr, bezarr, minmax, npts, nmove)
WORD handle, count;
WORD FAR *xyarr;
UBYTE FAR *bezarr;
WORD FAR *minmax;
WORD FAR *npts;
WORD FAR *nmove;
{
   v_bezier_all(6, handle, count, xyarr, bezarr, minmax, npts, nmove);
}

VOID v_bezfill(handle, count, xyarr, bezarr, minmax, npts, nmove)
WORD handle, count;
WORD FAR *xyarr;
UBYTE FAR *bezarr;
WORD FAR *minmax;
WORD FAR *npts;
WORD FAR *nmove;
{
   v_bezier_all(9, handle, count, xyarr, bezarr, minmax, npts, nmove);
}


