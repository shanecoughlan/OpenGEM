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


#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

/******************************************************************************
 *
 * Compute intersect of two rectangles
 *
 ******************************************************************************/

WORD rc_intersect(p1, p2)
GRECT FAR *p1;
GRECT FAR *p2;
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

