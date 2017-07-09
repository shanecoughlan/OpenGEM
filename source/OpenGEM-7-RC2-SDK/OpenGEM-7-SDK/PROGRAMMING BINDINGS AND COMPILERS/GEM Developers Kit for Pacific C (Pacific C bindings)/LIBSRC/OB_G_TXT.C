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
 * Get pointer to string of a object (regardless of its type)
 *
 ******************************************************************************/

BYTE FAR *ob_get_text(tree, index, clear)
OBJECT FAR *tree;
WORD index;
WORD clear;
{
   REG BYTE FAR *p;

   switch(tree[index].ob_type & 0xff)
   {
      case G_TEXT:
      case G_FTEXT:
      case G_BOXTEXT:
      case G_FBOXTEXT:
         p = (BYTE FAR *) ((TEDINFO FAR *)tree[index].ob_spec)->te_ptext;
         break;

      case G_BUTTON:
      case G_STRING:
      case G_TITLE:
         p = (BYTE FAR *)tree[index].ob_spec;
         break;

      default : return(0l);
   }

   if (clear)
      *p = 0;

   return(p);
}

