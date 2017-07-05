/*******************************************************************/
/*                                                                 */
/*      Copyright 1999, Caldera Thin Clients, Inc.                 */
/*      This software is licenced under the GNU Public License.    */
/*      Please see LICENSE.TXT for further information.            */
/*                                                                 */
/*                 Historical Copyright                            */
/*******************************************************************/
/* Copyright (c) 1987 Digital Research Inc.		           */
/* The software contained in this listing is proprietary to        */
/* Digital Research Inc., Pacific Grove, California and is         */
/* covered by U.S. and other copyright protection.  Unauthorized   */
/* copying, adaptation, distribution, use or display is prohibited */
/* and may be subject to civil and criminal penalties.  Disclosure */
/* to others is prohibited.  For the terms and conditions of soft- */
/* ware code use refer to the appropriate Digital Research         */
/* license agreement.						   */
/*******************************************************************/

/*	OPTIMIZE.C	1/25/84 - 11/24/84	Lee Jay Lorenzen	*/

#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "odefs.h"


#if HIGH_C
/* routine to fool High-C's need for COMPACT library	10/24/85	*/
	void
compact()
{
}
pragma alias(compact, "COMPACT?");
#endif

    VOID
r_set(pxywh, x, y, w, h)
	WORD		*pxywh;
	WORD		x, y, w, h;
{
	pxywh[0] = x;
	pxywh[1] = y;
	pxywh[2] = w;
	pxywh[3] = h;
}

/*----------------------------------------------------------------------*/
    BOOLEAN   
overlap( r1, r2 )  
    RECTANGLE  	*r1, *r2 ;
{
    if ( r1->left > r2->right ) 
	return( FALSE ) ;
    if ( r1->right < r2->left ) 
	return( FALSE ) ;
    if ( r1->bottom < r2->top ) 
	return( FALSE ) ;
    if ( r1->top > r2->bottom ) 
	return( FALSE ) ;
    return( TRUE ) ;
} /* overlap */

	BOOLEAN
rc_intersect(p1, p2)
	GRECT		*p1, *p2;
{
	WORD		tx, ty, tw, th;

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


	VOID
rc_union(p1, p2)
	GRECT		*p1, *p2;
{
	WORD		tx, ty, tw, th;

	tw = max(p1->g_x + p1->g_w, p2->g_x + p2->g_w);
	th = max(p1->g_y + p1->g_h, p2->g_y + p2->g_h);
	tx = min(p1->g_x, p2->g_x);
	ty = min(p1->g_y, p2->g_y);
	p2->g_x = tx;
	p2->g_y = ty;
	p2->g_w = tw - tx;
	p2->g_h = th - ty;
}


	BOOLEAN
inside(x, y, pt)
	UWORD		x, y;
	GRECT		*pt;
{
	if ( (x >= pt->g_x) && (y >= pt->g_y) &&
	     (x < pt->g_x + pt->g_w) && (y < pt->g_y + pt->g_h) )
	  return(TRUE);
	else
	  return(FALSE);
} /* inside */


	WORD
min(a, b)
	WORD		a, b;
{
	return( (a < b) ? a : b );
}


	WORD
max(a, b)
	WORD		a, b;
{
	return( (a > b) ? a : b );
}


	WORD
strlen(p1)
	BYTE		*p1;
{
	WORD		len;

	len = 0;
	while( *p1++ )
	  len++;

	return(len);
}


	BOOLEAN
strcmp(p1, p2)
	BYTE		*p1, *p2;
{
	while(*p1)
	{
	  if (*p1++ != *p2++)
	    return(FALSE);
	}
	if (*p2)
	  return(FALSE);
	return(TRUE);
}


	VOID
strcpy(ps, pd)
	BYTE		*ps, *pd;
{
	while ( *ps )
	    *pd++ = *ps++  ;
	*pd = NULL ;
}


	VOID
strcat(ps, pd)
	BYTE		*ps, *pd;
{
	while(*pd)
	  pd++;
	while ( *ps )
	    *pd++ = *ps++ ;
	*pd = NULL ;
}


/*
*	Return <0 if s<t, 0 if s==t, >0 if s>t
*/

	WORD
strchk(s, t)
	BYTE		s[], t[];
{
	WORD		i;

	i = 0;
	while( s[i] == t[i] )
	  if (s[i++] == NULL)
	    return(0);
	return(s[i] - t[i]);
}

/* end of outoptmz.c */