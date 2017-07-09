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
 *    ems_alloc
 *
 *    allocate given number of EMS-pages and return handle of them
 *
 * Input    :  number of pages to allocate (each is 16 KB)
 * Output   :  handle or FALSE
 ***********************************************************************/

GLOBAL WORD ems_alloc(pages)
WORD pages;
{
   if (ems_int(0x43,0,pages,0))
      return(emm_dx);
   else
      return(FALSE);
 }

