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
#include "xfmlib.h"

EXTERN WORD x_xmul;              /* scaling factors for x transformation      */
EXTERN WORD x_xdiv;
EXTERN WORD x_xtrans;            /* translate factor for x transformation     */
EXTERN WORD x_ymul;              /* scaling factors for x transformation      */
EXTERN WORD x_ydiv;
EXTERN WORD x_ytrans;            /* translate factor for x transformation     */
EXTERN WORD x_wmicrons;
EXTERN WORD x_hmicrons;

/******************************************************************************
 *
 * This procedure matches an aspect ratio on the device with one
 * specified in user units. The match is done in physical units
 * rather than pixels, so a square specified in user units will look
 * square when displayed on the device. Calculating the aspect ratio
 * match in this fashion takes care of devices with non-square pixels.
 *
 *****************************************************************************/

VOID x_saspect(user_w, user_h, dev_w, dev_h, w_microns, h_microns)
WORD user_w, user_h;
WORD FAR *dev_w;
WORD FAR *dev_h;
WORD w_microns, h_microns;
{
   LONG y_ratio, x_ratio;    

   x_wmicrons = w_microns;
   x_hmicrons = h_microns;

   x_ratio = (LONG)(*dev_w) * (LONG)(x_wmicrons / 10) * (LONG)(user_h / 4);
   y_ratio = (LONG)(*dev_h) * (LONG)(x_hmicrons / 10) * (LONG)(user_w / 4);

/*
 * divide by ten to get pixel sizes back to VDI 1.x
 * sizes so longs don't wrap to negative numbers
 */
   if (y_ratio < x_ratio)
   {
      *dev_w = x_mul_div(*dev_h, x_hmicrons, x_wmicrons);
      *dev_w = x_mul_div(*dev_w, user_w, user_h);
   }
   else 
   {
      *dev_h = x_mul_div( *dev_w, x_wmicrons, x_hmicrons);
      *dev_h = x_mul_div( *dev_h, user_h, user_w);
   }
}

