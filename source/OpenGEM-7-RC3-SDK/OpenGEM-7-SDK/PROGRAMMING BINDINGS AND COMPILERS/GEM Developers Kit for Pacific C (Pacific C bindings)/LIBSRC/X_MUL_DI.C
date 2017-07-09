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
 * ( ( ( m1 * 2 * m2 ) / d1 ) + 1 ) / 2
 * The point of this routine is to get floating point accuracy without
 * going to the expense of floating point (sk)
 *
 ******************************************************************************/

WORD x_mul_div(m1, m2, d1)
WORD m1, m2, d1;
{
   UWORD ret;
   LONG lm1, lm2, ld1;

   lm1 = m1;
   lm2 = m2;
   ld1 = d1;
   ret = (WORD) ((((lm1 * 2l * lm2) / ld1) + 1l) / 2l);

   return(ret);
}
