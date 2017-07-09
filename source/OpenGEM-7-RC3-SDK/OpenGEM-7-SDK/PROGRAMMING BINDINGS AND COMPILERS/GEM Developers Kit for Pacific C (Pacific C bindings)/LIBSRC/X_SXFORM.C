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

/*****************************************************************************
 *
 * set translation values, given widths and heights for both user and
 * device space are set to non-zero.
 *
 *****************************************************************************/

WORD x_sxform(user_x, user_y, user_w, user_h, dev_x, dev_y, dev_w, dev_h)
WORD user_x, user_y, user_w, user_h, dev_x, dev_y, dev_w, dev_h;
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

