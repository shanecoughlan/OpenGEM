/******************************************************************************
 *
 * ev_event.c
 *
 *    This source handles the call to evnt_multi by passing it the right
 *    parameters from the MEVENT-structure, so a DOS-user need not be
 *    concerned with all unneeded parameters.
 *
 * Input :
 *    MEVENT *mevent;
 *
 * Ouput :
 *    WORD event     -     type of event that occured
 *
 ******************************************************************************/

#include "portab.h"
#include "aes.h"
#include <dos.h>	/* for FP_OFF and FP_SEG */

WORD evnt_event(mevent)

MEVENT *mevent;
{
   return ( evnt_multi(
               mevent->e_flags,
               mevent->e_bclk, mevent->e_bmsk, mevent->e_bst,
               mevent->e_m1flags,
               mevent->e_m1.g_x, mevent->e_m1.g_y,
               mevent->e_m1.g_w, mevent->e_m1.g_h,
               mevent->e_m2flags,
               mevent->e_m2.g_x, mevent->e_m2.g_y,
               mevent->e_m2.g_w, mevent->e_m2.g_h,
               mevent->e_mepbuf,
               FPOFF(mevent->e_time), FPSEG(mevent->e_time),
               (UWORD *)&mevent->e_mx, (UWORD *)&mevent->e_my, (UWORD *)&mevent->e_mb,
               (UWORD *)&mevent->e_ks, (UWORD *)&mevent->e_kr,
               (UWORD *)&mevent->e_br));
}
