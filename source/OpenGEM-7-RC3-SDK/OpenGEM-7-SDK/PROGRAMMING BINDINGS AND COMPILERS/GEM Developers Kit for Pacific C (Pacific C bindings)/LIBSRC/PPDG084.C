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

WORD xapp_getinfo(WORD ap_gtype, WORD *ap_gout1, WORD *ap_gout2,
				 WORD *ap_gout3, WORD *ap_gout4)
{
  WORD ret;

  XAP_GTYPE = ap_gtype;
 
  ret = gem_if( XAPP_GETINFO );

  *ap_gout1 = XAP_GOUT1;
  *ap_gout2 = XAP_GOUT2;
  *ap_gout3 = XAP_GOUT3;
  *ap_gout4 = XAP_GOUT4;

  return ret;
}

