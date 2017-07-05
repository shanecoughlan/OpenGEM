/*****************************************************************************
 *
 *	-------------------------------------------------------------
 *	GEM Desktop					  Version 3.1
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1985 - 1989		Digital Research Inc.
 *	-------------------------------------------------------------
 *
 *****************************************************************************/

#include "ppdgem.h"
#include "ppdg0.h"

WORD shel_put(pbuffer, len)
BYTE FAR *pbuffer;
WORD len;
{
   SH_PBUFFER = pbuffer;
   SH_LEN     = len;

   return( gem_if( SHEL_PUT ) );
}

