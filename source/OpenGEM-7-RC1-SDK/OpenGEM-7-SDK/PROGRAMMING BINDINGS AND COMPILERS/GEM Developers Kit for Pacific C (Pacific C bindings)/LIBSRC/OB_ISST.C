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
 * Test state in ob_state of a object
 *
 ******************************************************************************/

WORD ob_isstate(tree, index, state)
OBJECT FAR *tree;
WORD index;
WORD state;
{
   return((tree[index].ob_state & state) ? TRUE : FALSE);
}

