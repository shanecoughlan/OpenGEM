/******************************************************************************
 *
 *    Modulname   :  XFMLIB.C
 *
 *    Author      :  unknown
 *    Version     :  ?.??
 *    Created     :  ??.??.1985
 *
 *    Author      :  Robert Schneider
 *    Version     :  1.1
 *    Modified    :  April 20, 1989
 *       Cleaned up for new packaging.
 *       Inserted x_mul_div instead of SMUL_DIV, MUL_DIV and UMUL_DIV.
 *
 *****************************************************************************
 *
 * The source code contained in this listing is non-copyrighted
 * work which can be freely used. In applications of the source code,
 * you are requested to acknowledge Digital Research, Inc. as the
 * originator of this code.
 *
 * XFMLIB.C - transformation library.
 * all functions and variables begin with "x_"
 *
 ******************************************************************************/

#include "djgppgem.h"

MLOCAL WORD x_xmul = 1;          /* scaling factors for x transformation      */
MLOCAL WORD x_xdiv = 1;
MLOCAL WORD x_xtrans = 0;        /* translate factor for x transformation     */
MLOCAL WORD x_ymul = 1;          /* scaling factors for x transformation      */
MLOCAL WORD x_ydiv = 1;
MLOCAL WORD x_ytrans = 0;        /* translate factor for x transformation     */
MLOCAL WORD x_wmicrons = 0;
MLOCAL WORD x_hmicrons = 0;


/******************************************************************************
 *
 * return number of pixels in x-direction physically equal to "y"
 * number of pixels in y-direction.
 *
 ******************************************************************************/

WORD x_ytox(WORD y)
{
    return( x_mul_div( y, x_hmicrons, x_wmicrons));
}

/******************************************************************************
 *
 * transform an x-value from user space into device space.
 *
 *****************************************************************************/

WORD x_udx_xform(WORD user_x)
{
   return( x_mul_div( user_x, x_xmul, x_xdiv) + x_xtrans);
}

/******************************************************************************
 *
 * transform a y-value from user space into device space.
 *
 ******************************************************************************/

WORD x_udy_xform(WORD user_y)

{
   return( x_mul_div( user_y, x_ymul, x_ydiv) + x_ytrans);
}


/******************************************************************************
 *
 * transform a x-value from device space into user space.
 *
 ******************************************************************************/

WORD x_dux_xform(WORD x)
{
   return( x_mul_div( x - x_xtrans, x_xdiv, x_xmul));
}


/******************************************************************************
 *
 * transform an x-value from device space into user space.
 *
 ******************************************************************************/

WORD x_duy_xform(WORD y)
{
   return( x_mul_div( y - x_ytrans, x_ydiv, x_ymul));
}


/******************************************************************************
 *
 * scale an x-distance from user space into device space.
 *
 ******************************************************************************/

WORD x_udx_scale(WORD user_dx)
{
   return( x_mul_div( user_dx, x_xmul, x_xdiv));
}


/******************************************************************************
 *
 * scale a y-distance from user space into device space.
 *
 ******************************************************************************/

WORD x_udy_scale(WORD user_dy)
{
   return( x_mul_div( user_dy, x_ymul, x_ydiv));
}


/******************************************************************************
 *
 * scale an x-distance from device space into user space.
 *
 ******************************************************************************/

WORD x_dux_scale(WORD dev_x)
{
   return( x_mul_div( dev_x, x_xdiv, x_xmul));
}


/******************************************************************************
 *
 * scale a x-distance from device space into user space.
 *
 ******************************************************************************/

WORD x_duy_scale(WORD dev_y)
{
   return( x_mul_div( dev_y, x_ydiv, x_ymul));
}


/*****************************************************************************
 *
 * set translation values, given widths and heights for both user and
 * device space are set to non-zero.
 *
 *****************************************************************************/

WORD x_sxform(WORD user_x, WORD user_y, WORD user_w, WORD user_h, 
              WORD dev_x,  WORD dev_y,  WORD dev_w, WORD dev_h)
{
   if (!(dev_w && dev_h && user_w && user_h))
      return(FALSE);

   x_xmul   = dev_w;
   x_xdiv   = user_w;
   x_ymul   = dev_h;
   x_ydiv   = user_h;
   x_xtrans = dev_x - x_mul_div(user_x, x_xmul, x_xdiv);
   x_ytrans = dev_y - x_mul_div(user_y, x_ymul, x_ydiv);

   return(TRUE);
}

/******************************************************************************
 *
 * This procedure matches an aspect ratio on the device with one
 * specified in user units. The match is done in physical units
 * rather than pixels, so a square specified in user units will look
 * square when displayed on the device. Calculating the aspect ratio
 * match in this fashion takes care of devices with non-square pixels.
 *
 *****************************************************************************/

VOID x_saspect(WORD user_w, WORD user_h, WORD *dev_w, WORD *dev_h, 
               WORD w_microns, WORD h_microns)
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


/******************************************************************************
 *
 * ( ( ( m1 * 2 * m2 ) / d1 ) + 1 ) / 2
 * The point of this routine is to get floating point accuracy without
 * going to the expense of floating point (sk)
 *
 ******************************************************************************/

WORD x_mul_div(WORD m1, WORD m2, WORD d1)
{
   UWORD ret;
   LONG lm1, lm2, ld1;

   lm1 = m1;
   lm2 = m2;
   ld1 = d1;
   ret = (WORD) ((((lm1 * 2l * lm2) / ld1) + 1l) / 2l);

   return(ret);
}
