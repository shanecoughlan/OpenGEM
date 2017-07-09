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
 * Tests for two rectangles equal
 *
 ******************************************************************************/

WORD rc_equal(GRECT *p1, GRECT *p2)
{
   if ((p1->g_x != p2->g_x) || (p1->g_y != p2->g_y) ||
       (p1->g_w != p2->g_w) || (p1->g_h != p2->g_h))
      return(FALSE);

   return(TRUE);
}

/******************************************************************************
 *
 * Copy source to destination rectangle
 *
 ******************************************************************************/

VOID rc_copy(GRECT *psbox, GRECT *pdbox)
{
   *pdbox = *psbox;  
}


#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

WORD rc_intersect(GRECT *p1, GRECT *p2)
{
   WORD tx, ty, tw, th;

   tw = min(p2->g_x + p2->g_w, p1->g_x + p1->g_w); 
   th = min(p2->g_y + p2->g_h, p1->g_y + p1->g_h); 
   tx = max(p2->g_x, p1->g_x);   
   ty = max(p2->g_y, p1->g_y);   
   p2->g_x = tx;    
   p2->g_y = ty;   
   p2->g_w = tw - tx;   
   p2->g_h = th - ty;   

   return( (tw > tx) && (th > ty) );   
}



/******************************************************************************
 *
 * Determine if x,y is in rectangle
 *
 ******************************************************************************/

WORD rc_inside(WORD x, WORD y, GRECT *pt)
{
   if (x < pt->g_x)
      return(FALSE);
   if (y < pt->g_y)
      return(FALSE);
   if (x >= pt->g_x + pt->g_w)
      return(FALSE);
   if (y >= pt->g_y + pt->g_h)
      return(FALSE);

   return(TRUE);
}



/******************************************************************************
 *
 * Convert x,y,w,h to upper left x,y and lower right x,y for raster ops
 *
 ******************************************************************************/

VOID rc_grect_to_array(GRECT *area, WORD *array)
{
   *array++ = area->g_x;
   *array++ = area->g_y;
   *array++ = area->g_x + area->g_w - 1;
   *array = area->g_y + area->g_h - 1;
}
