
/*****************************************************************************
 *
 *	-------------------------------------------------------------
 *	GEM Desktop					  Version 3.1
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1985 - 1989		Digital Research Inc.
 *	-------------------------------------------------------------
 *
 *****************************************************************************/

#include "djgppgem.h"


/******************************************************************************
 *
 * Unset state in ob_state of a object
 *
 ******************************************************************************/

VOID ob_undostate(LPTREE tree, WORD index, WORD state)
{
   REG LPWORD addr = tree + (index * sizeof(OBJECT)) + 10;

   LWSET(addr, LWGET(addr) & ~state);
}

/******************************************************************************
 *
 * Test state in ob_state of a object
 *
 ******************************************************************************/

WORD ob_isstate(LPTREE tree, WORD index, WORD state)
{
   REG LPWORD addr = tree + (index * sizeof(OBJECT)) + 10;

   return(( LWGET(addr)  & state) ? TRUE : FALSE);
}



/******************************************************************************
 *
 * Unset flags in ob_flags of a object
 *
 ******************************************************************************/

VOID ob_undoflag(LPTREE tree, WORD index, WORD flag)
{
   REG LPWORD addr = tree + (index * sizeof(OBJECT)) + 8;

   LWSET(addr, LWGET(addr) & ~flag);
}


/******************************************************************************
 *
 * Test flags in ob_flags of a object
 *
 ******************************************************************************/

WORD ob_isflag(LPTREE tree, WORD index, WORD flag)
{
   REG LPWORD addr = tree + (index * sizeof(OBJECT)) + 8;

   return(( LWGET(addr)  & flag) ? TRUE : FALSE);
}


/******************************************************************************
 *
 * Get position and dimensions of a object
 *
 ******************************************************************************/

VOID ob_xywh(LPTREE tree, WORD index, GRECT *rec)
{
   WORD lx, ly;
   LPWORD addr = tree + (index * sizeof(OBJECT)) + 20;

   objc_offset(tree, index, &lx, &ly);

   rec->g_x = lx;
   rec->g_y = ly;
   rec->g_w = LWGET(addr);
   rec->g_h = LWGET(addr + 2);
}


/******************************************************************************
 *
 * Get pointer to string of a object (regardless of its type)
 *
 ******************************************************************************/

LPBYTE ob_get_text(LPTREE tree, WORD index, WORD clear)
{
   REG LPBYTE p;
   LPWORD addr = tree + (index * sizeof(OBJECT)) + 6;	/* -> ob_type */


   switch(LWGET(addr) & 0xff)
   {
      case G_TEXT:
      case G_FTEXT:
      case G_BOXTEXT:
      case G_FBOXTEXT:
         p = LSGET(LSGET(addr + 6));  /* -> ob_spec -> te_ptext */
         break;

      case G_BUTTON:
      case G_STRING:
      case G_TITLE:
         p = (LPBYTE)LSGET(addr + 6);	/* -> ob_spec */
         break;

      default : return(0L);
   }

   if (clear) LBSET(p, 0);

   return(p);
}


/******************************************************************************
 *
 * Draw a dialog with an optional growing box
 *
 ******************************************************************************/

VOID ob_draw_dialog(LPTREE tree, WORD xl, WORD yl, WORD wl, WORD hl)
{
   GRECT rec;

   form_center(tree, &rec.g_x, &rec.g_y, &rec.g_w, &rec.g_h);
   form_dial(FMD_START, rec.g_x, rec.g_y, rec.g_w, rec.g_h,
                        rec.g_x, rec.g_y, rec.g_w, rec.g_h);
   if (xl || yl || wl || hl)
      form_dial(FMD_GROW, xl, yl, wl, hl,
                   rec.g_x - 5, rec.g_y - 5,
                   rec.g_w + 10, rec.g_h + 10);
   objc_draw(tree, ROOT, MAX_DEPTH,
             rec.g_x - 5, rec.g_y - 5, rec.g_w + 10, rec.g_h + 10);
}



/******************************************************************************
 *
 * Undraw a dialog with an optional shrinking box
 *
 ******************************************************************************/

VOID ob_undraw_dialog(LPTREE tree, WORD xl, WORD yl, WORD wl, WORD hl)
{
   GRECT rec;

   ob_xywh(tree, 0, &rec);
   form_dial(FMD_FINISH,rec.g_x - 5, rec.g_y - 5, rec.g_w + 10, rec.g_h + 10,
                        rec.g_x - 5, rec.g_y - 5, rec.g_w + 10, rec.g_h + 10);
   if (xl || yl || wl || hl)
      form_dial(FMD_SHRINK, xl, yl, wl, hl, rec.g_x, rec.g_y, rec.g_w, rec.g_h);
}

/******************************************************************************
 *
 * Set flags in ob_flags of a object
 *
 ******************************************************************************/

VOID ob_doflag(LPTREE tree, WORD index, WORD flag)
{
   REG LPWORD addr = tree + (index * sizeof(OBJECT)) + 8;

   LWSET(addr, LWGET(addr) | flag);
}


/******************************************************************************
 *
 * Set state in ob_state of a object
 *
 ******************************************************************************/

VOID ob_dostate(LPTREE tree, WORD index, WORD state)
{
   REG LPWORD addr = tree + (index * sizeof(OBJECT)) + 10;

   LWSET(addr, LWGET(addr) | state);
}


/******************************************************************************
 *
 * Set pointer to a string of a object (regardless of its type)
 *
 ******************************************************************************/

VOID ob_set_text(LPTREE tree, WORD index, LPBYTE p)
{
   LPWORD addr = tree + (index * sizeof(OBJECT)) + 6;	/* -> ob_type */


   switch(LWGET(addr) & 0xff)
   {
      case G_TEXT:
      case G_FTEXT:
      case G_BOXTEXT:
      case G_FBOXTEXT:
	 LSSET(LSGET(addr + 6), p); /* -> ob_spec -> te_ptext */
         break;

      case G_BUTTON:
      case G_STRING:
      case G_TITLE:
         LSSET(addr + 6, p);	/* -> ob_spec */
         break;

      default : break;
   }
}

