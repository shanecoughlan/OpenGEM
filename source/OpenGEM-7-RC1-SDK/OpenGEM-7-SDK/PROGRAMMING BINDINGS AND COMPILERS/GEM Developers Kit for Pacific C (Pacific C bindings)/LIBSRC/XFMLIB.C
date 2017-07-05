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

#include "portab.h"
#include "xfmlib.h"

GLOBAL WORD x_xmul = 1;          /* scaling factors for x transformation      */
GLOBAL WORD x_xdiv = 1;
GLOBAL WORD x_xtrans = 0;        /* translate factor for x transformation     */
GLOBAL WORD x_ymul = 1;          /* scaling factors for x transformation      */
GLOBAL WORD x_ydiv = 1;
GLOBAL WORD x_ytrans = 0;        /* translate factor for x transformation     */
GLOBAL WORD x_wmicrons = 0;
GLOBAL WORD x_hmicrons = 0;

