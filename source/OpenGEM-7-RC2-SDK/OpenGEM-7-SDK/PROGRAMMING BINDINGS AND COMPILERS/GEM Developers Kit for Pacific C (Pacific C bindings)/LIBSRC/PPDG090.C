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

WORD appl_xbvget(ULONG *bvdisk, ULONG *bvhard)
{
  WORD result;

  AP_XBVMODE  = 0;
  result = gem_if( APPL_XBVSET );

  if (bvdisk) *bvdisk = AP_XBVDISKL | (((LONG)(AP_XBVDISKH)) << 16);
  if (bvhard) *bvhard = AP_XBVHARDL | (((LONG)(AP_XBVHARDH)) << 16);

  return result;
}

