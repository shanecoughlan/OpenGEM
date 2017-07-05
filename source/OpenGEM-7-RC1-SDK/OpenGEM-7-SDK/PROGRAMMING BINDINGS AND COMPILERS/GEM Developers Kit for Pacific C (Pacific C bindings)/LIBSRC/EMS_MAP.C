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
 *    ems_map
 *
 *    show logical page logp from handle handle in physical page
 *    physp.
 *
 * Input    :  handle   handle of allocated pages
 *             logp     logical to be shown
 *             physp    physical page
 * Output   :  TRUE or FALSE
 ***********************************************************************/

GLOBAL WORD ems_map(handle,logp,physp)
WORD handle;
WORD logp;
WORD physp;
{
   if (ems_int(0x44,handle,logp,physp))
      return(TRUE);
   else
      return(FALSE);
 }

