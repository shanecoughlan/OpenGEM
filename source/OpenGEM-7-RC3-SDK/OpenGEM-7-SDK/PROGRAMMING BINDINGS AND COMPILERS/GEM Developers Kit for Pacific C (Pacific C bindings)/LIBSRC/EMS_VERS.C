/*****************************************************************************
 *
 *	-------------------------------------------------------------
 *	GEM Desktop					  Version 3.1
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1985 - 1989		Digital Research Inc.
 *	-------------------------------------------------------------
 *
 *****************************************************************************/

#include <portab.h>

EXTERN WORD  ems_int _((WORD fkt, WORD dx, WORD bx, WORD al));

EXTERN UWORD emm_bx;          /* return code of bx                            */
EXTERN UWORD emm_dx;          /* return code of dx                            */
EXTERN UWORD emm_ax;          /* return code of ax                            */
/***********************************************************************
 *    ems_version
 *
 *    Get version number of EMS-manager
 *
 * Input    :  none
 * Output   :  decimal version number (e.g. 35 for 3.5, 40 for 4.0)
 ***********************************************************************/

GLOBAL WORD ems_version()
{
   if (ems_int(0x46,0,0,0))
      return( (emm_ax & 0x0f) + (emm_ax >> 4) * 10);
   else
      return(FALSE);
}

