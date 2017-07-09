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
#include "oblib.h"


/******************************************************************************
 *
 * Test flags in ob_flags of a object
 *
 ******************************************************************************/

WORD ob_isflag(tree, index, flag)
OBJECT FAR *tree;
WORD index;
WORD flag;
{
   return((tree[index].ob_flags & flag) ? TRUE : FALSE);
}

