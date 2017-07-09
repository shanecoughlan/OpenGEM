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

WORD prop_gui_set(WORD property, WORD value)
{
  PROP_NUM   = property;
  PROP_VALUE = value;

  return gem_if( PROP_GUI_SET );
}

