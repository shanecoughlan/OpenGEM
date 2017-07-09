/*****************************************************************************
 *
 *	-------------------------------------------------------------
 *	GEM Desktop					  Version 3.1
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1985 - 1989		Digital Research Inc.
 *	-------------------------------------------------------------
 *
 *****************************************************************************/

#include "portab.h"
#include "aes.h"
#include "rclib.h"


/******************************************************************************
 *
 * Copy source to destination rectangle
 *
 ******************************************************************************/

VOID rc_copy(psbox, pdbox)
GRECT FAR *psbox;
GRECT FAR *pdbox;
{
   *pdbox = *psbox;  /* ANSI-C allows structure assignment */
}

