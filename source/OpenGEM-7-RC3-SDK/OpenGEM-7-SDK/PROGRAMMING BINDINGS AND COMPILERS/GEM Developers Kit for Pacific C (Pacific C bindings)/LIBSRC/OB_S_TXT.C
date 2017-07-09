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
 * Set pointer to a string of a object (regardless of its type)
 *
 ******************************************************************************/

VOID ob_set_text(tree, index, p)
OBJECT FAR *tree;
WORD index;
BYTE FAR *p;
{
   switch(tree[index].ob_type & 0xff)
   {
      case G_TEXT:
      case G_FTEXT:
      case G_BOXTEXT:
      case G_FBOXTEXT:
         ((TEDINFO FAR *)tree[index].ob_spec)->te_ptext = p;
         break;

      case G_BUTTON:
      case G_STRING:
      case G_TITLE:
         tree[index].ob_spec = p;
         break;

      default : break;
   }
}

