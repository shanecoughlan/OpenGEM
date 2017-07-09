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

/*----------------------------------------------------------------------*/
/* AESTOOLS.C : misc utilities for services-related tasks.		*/
/*----------------------------------------------------------------------*/

#include "PORTAB.H"
#include "MACHINE.H"
#include "OBDEFS.H"

#include "gemcmds.h"
#include "gembind.h"
#include "dosbind.h"
#include "vdibind.h"

#include "draw.h"
#include "constdef.h"
#include "typedefs.h"
#include "extnvars.h"

/*----------------------------------------------------------------------*/
    OBJECT
*ob_addr( tree, obj )
LONG	tree ;
WORD	obj ;
{
    return( tree + obj * sizeof( OBJECT ) ) ;
} /* ob_addr() */

/*----------------------------------------------------------------------*/
    VOID
rect_union( r1, r2 )
    RECT	*r1, *r2 ;
{
WORD	right, bottom ;

    right = max( r1->x + r1->w, r2->x + r2->w ) ;
    r2->x = min( r1->x, r2->x ) ;
    r2->w = right - r2->x ;
    bottom = max( r1->y + r1->h, r2->y + r2->h ) ;
    r2->y = min( r1->y, r2->y ) ;
    r2->h = bottom - r2->y ;
} /* rect_union */

/*----------------------------------------------------------------------*/
    VOID
mv_rect( xd, yd, r ) 
    WORD  	xd, yd  ;
    RECT	*r ;
{
    r->x   += xd ;
    r->y   += yd ;
} /* mv_rect */

/*----------------------------------------------------------------------*/
/* returns TRUE iff inner is entirely within outer.			*/
    BOOLEAN
r_allin( outer, inner )
    RECT	*outer, *inner ;
{
    if (outer->x > inner->x)
	return ( FALSE ) ;
    if (outer->x + outer->w < inner->x + inner->w )
	return ( FALSE ) ;
    if (outer->y > inner->y)
	return ( FALSE ) ;
    if (outer->y + outer->h < inner->y + inner->h )
	return ( FALSE ) ;
    return( TRUE ) ;
} /* r_allin */

/*----------------------------------------------------------------------*/
/* Check if point is inside rectangle.					*/
    BOOLEAN
Pt_in_rect( x, y, r ) 
    WORD      		x, y ; 
    RECT		*r ;
{
    if ( x < r->x ) 
	return( FALSE ) ;
    if ( y < r->y ) 
	return( FALSE ) ;
    if ( x > r->x + r->w ) 
	return( FALSE ) ;
    if ( y > r->y + r->h ) 
	return( FALSE ) ;
    return( TRUE ) ;
} /* Pt_in_rect */


/*----------------------------------------------------------------------*/
    VOID
add_aper( r, a ) 
RECT 	*r ;
WORD		a ;
{
    r->x -= a ;
    r->y -= a ;
    r->w += 2 * a ;
    r->h += 2 * a ;
} /* add_aper */

/*----------------------------------------------------------------------*/
    VOID
rect_inter( r1, r2 )
    RECT		*r1, *r2 ;
{
RECT	tmp ;

    movs( sizeof( RECT ), r2, &tmp ) ;
    if (r1->x > r2->x && r1->x < r2->x + r2->w )
	tmp.x = r1->x ;
    if (r1->x + r1->w > r2->x && r1->x + r1->w < r2->x + r2->w )
        tmp.w = r1->x + r1->w - tmp.x ;
    if (r1->y > r2->y && r1->y < r2->y + r2->h )
	tmp.y = r1->y ;
    if (r1->y + r1->h > r2->y && r1->y + r1->h < r2->y + r2->h )
        tmp.h = r1->y + r1->h - tmp.y ;
    movs( sizeof( RECT ), &tmp, r2 ) ;
} /* rect_inter */

/*--------------------------------------------------------------*/
    BOOLEAN
rc_equal( p1, p2 )
	WORD		*p1, *p2;
{
	WORD		i ;

	for(i=0; i<4; i++)
	{
	  if (*p1++ != *p2++)
	    return(FALSE);
	}
	return(TRUE);
} /* rc_equal() */

/*----------------------------------------------------------------------*/
    BOOLEAN   
Overlap( r1, r2 )  
    RECT  	*r1, *r2 ;
{
    if ( r2->x > r1->x + r1->w )
	return( FALSE ) ;
    if ( r1->x > r2->x + r2->w )
	return( FALSE ) ;
    if ( r2->y > r1->y + r1->h )
	return( FALSE ) ;
    if ( r1->y > r2->y + r2->h )
	return( FALSE ) ;
    return( TRUE ) ;
} /* Overlap */

/*----------------------------------------------------------------------*/
/*  Turn box right-side up and frontwards if neccessary.		*/
    VOID
orient_box( r )
    RECT	*r ;
{
    if ( r->w < 0 )
	{
	r->w = abs( r->w ) ;
	r->x -= r->w ;
	}
    if ( r->h < 0 ) 
	{
	r->h = abs( r->h ) ;
	r->y -= r->h ;
	}
} /* orient_box */

#if DEBUG
/*----------------------------------------------------------------------*/
/* b comes in the range [0..15d] and is returned in the range [30..39h] */
/* and [41..46h] for the ascii characters [`0'..`F'].			*/
    VOID
i_to_hex( b )
BYTE	*b ;
{
    if ( ( *b < 0 ) || ( *b > 15 ) )
	*b = TILDE ;	
    else
	{
	if ( *b < 10 )
	    *b += '0' ;
	else 
	    *b += ('A' - 10) ;
	}
} /* i_to_hex */

/*----------------------------------------------------------------------*/
/* takes a string of the form ["hello"] and returns the string ["68.65. */
/* 6c.6c.6f.00.00.00.00....."].	Notice the new string is three times the*/
/* length of the original.						*/
    VOID
byte_to_hex( b )
BYTE	*b ;
{
BYTE	nib1, nib2, bite ;
WORD	ii ;
BYTE	tmp[ MX_MESG_LEN ] ;

    b_stuff( ADDR( tmp ), MX_MESG_LEN, PERIOD ) ;
    for ( ii = 0; 3*ii < MX_MESG_LEN; ii++ )
	{
	bite = b[ ii ] ;
	nib1 = bite >> 4 ;
	nib2 = bite & 0x0F ;
	i_to_hex( &nib1 ) ;
  	i_to_hex( &nib2 ) ;
	tmp[ ii*3 ] = nib1 ;
	tmp[ ii*3 + 1 ] = nib2 ;
	}
    tmp[ MX_MESG_LEN -1 ] = NULL ;
    strcpy( tmp, b ) ;
} /* byte_to_hex */

/*----------------------------------------------------------------------*/
    VOID
show_hex( mesg )
BYTE	*mesg ;
{
BYTE	lines[ 5 ][ MX_MESG_LEN ] ;
WORD	inc, ii ;

    b_stuff( ADDR( lines ), sizeof( lines ), NULL ) ;
    inc = MX_MESG_LEN / 3 ;
    for ( ii = 0; ii < 5; ii++ )
	{
	movs( inc, &mesg[ ii*inc ], &lines[ ii ] ) ;
	byte_to_hex( &lines[ ii ] ) ;
	}
    do_falert( &lines[ 0 ], &lines[ 1 ], &lines[ 2 ], &lines[ 3 ], &lines[ 4 ] ) ;
} /* show_hex */

#endif

/*----------------------------------------------------------------------*/
    WORD
alert_box( index )
    WORD   index ;
{
    WORD    ret ;
    LONG    addr ;

    rsrc_gaddr( 5, index, &addr ) ;
    graf_mouse( ARROW, 0x0L ) ;
    ret = form_alert( 1, addr ) ;   /* default to first exit button */
    graf_mouse( cur_mouse, 0x0L ) ;
    return( ret ) ;
} /* alert_box */

/*----------------------------------------------------------------------*/
/* puts up a form alert with the 'stop' icon and one exit button.	*/
    VOID
do_falert( line1, line2, line3, line4, line5 )
BYTE	*line1, *line2, *line3, *line4, *line5 ;
{
BYTE	tmp[ 256 ] ;

    strcpy( "[3][" , tmp ) ;
    strcat( line1, tmp ) ;
    strcat( "|", tmp ) ;
    strcat( line2, tmp ) ;
    strcat( "|", tmp ) ;
    strcat( line3, tmp ) ;
    strcat( "|", tmp ) ;
    strcat( line4, tmp ) ;
    strcat( "|", tmp ) ;
    strcat( line5, tmp ) ;
    strcat( "][ Exit ]" , tmp ) ;
    form_alert( 1, ADDR( tmp ) ) ;
} /* do_falert */

/*----------------------------------------------------------------------*/
	VOID
set_rbutton(tree, but_root, on_but)
	LONG tree;
	WORD but_root, on_but;
{
WORD cur_ob ;

	for (cur_ob=ob_addr( tree, but_root )->ob_head;
		cur_ob != but_root;
		cur_ob=ob_addr( tree, cur_ob )->ob_next ) 
	    {
	    if(cur_ob==on_but)
	        ob_addr( tree, on_but )->ob_state = SELECTED ;
	    else
	        ob_addr( tree, cur_ob )->ob_state = NORMAL ;
	    }
} /* set_rbutton() */

/*----------------------------------------------------------------------*/
	WORD
get_rbutton(tree, but_root)
	LONG tree;
	WORD but_root;
{
WORD	cur_ob;
OBJECT	*cur_ptr ;

	for (cur_ob=ob_addr( tree, but_root )->ob_head;
		cur_ob != but_root;
		cur_ob=ob_addr( tree, cur_ob )->ob_next ) 
	{
	    cur_ptr = ob_addr( tree, cur_ob ) ;
	    if( (cur_ptr->ob_flags & RBUTTON) && 
		    (cur_ptr->ob_state & SELECTED) )
	    {
		return( cur_ob );
	    }
	}
	return ( -1 );
} /* get_rbutton() */

/*----------------------------------------------------------------------*/
	WORD
okcan( tree, okob, canob)
	LONG tree;
	WORD okob, canob;
{
OBJECT	*can_ptr, *ok_ptr ;

	ok_ptr = ob_addr( tree, okob ) ;
	if ( ok_ptr->ob_state & SELECTED)
	{
	    ok_ptr->ob_state &= ~(SELECTED);
	    return( 1 ) ;
	}
	can_ptr = ob_addr( tree, canob ) ;
	if ( can_ptr->ob_state & SELECTED )
	{
	    can_ptr->ob_state &= ~(SELECTED) ;
	    return( -1 ) ;
	}
	return( 0 ) ;
} /* okcan() */


/*----------------------------------------------------------------------*/
	WORD
tggl_state(tree, ob, x, y, w, h, change, redraw)
	LONG tree;
	WORD x, y, w, h;
	WORD ob, change, redraw;
{
	WORD state;

	state = ob_addr( tree, ob )->ob_state ;
	state ^= change;
	objc_change(tree, ob, 0, x, y, w, h,
		    state, redraw);

	return ( state & change );
} /* tggl_state() */

/* end of aestools.c */

