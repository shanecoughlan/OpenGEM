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
 *    ems_free
 *
 *    free all pages from handle handle
 *
 * Input    :  handle handle which pages to free
 * Output   :  TRUE or FALSE
 ***********************************************************************/

GLOBAL WORD ems_free(handle)
WORD handle;
{
   if (ems_int(0x45,handle,0,0))
      return(TRUE);
   else
      return(FALSE);
}

