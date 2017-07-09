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
 *    ems_restore_map()
 *
 *    restore a previosly saved map of the physical pages.
 *
 * Input    :  handle of previosly allocated pages
 * Output   :  TRUE or FALSE
 ***********************************************************************/

GLOBAL WORD ems_restore_map(handle)
WORD handle;
{
   if (ems_int(0x48,handle,0,0))
      return(TRUE);
   else
      return(FALSE);
}
