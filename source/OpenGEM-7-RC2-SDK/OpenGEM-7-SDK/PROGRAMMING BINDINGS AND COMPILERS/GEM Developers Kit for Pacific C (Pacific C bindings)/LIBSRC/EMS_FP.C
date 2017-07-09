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
 *    ems_free_page()
 *
 *    Get number of free pages
 *
 * Input    :  none
 * Output   :  number of free pages or FALSE
 ***********************************************************************/

GLOBAL WORD ems_free_page()
{
   if (ems_int(0x42,0,0,0))
      return(emm_bx);
   else
      return(FALSE);
}

