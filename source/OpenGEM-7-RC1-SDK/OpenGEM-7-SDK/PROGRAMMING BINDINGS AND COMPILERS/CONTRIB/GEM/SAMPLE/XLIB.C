/*----------------------------------------------------------------------*/
/* The source code contained in this listing is a non-copyrighted	*/
/* work which can be freely used. In applications of the source code	*/
/* you are requested to acknowledge Digital Research, Inc. as the	*/
/* originator of this code.						*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* xlib.c - transformation library.					*/
/* all funcs and vars begin with "x_"					*/
/*----------------------------------------------------------------------*/

#include "portab.h"
#include "machine.h"

GLOBAL WORD	x_xmul = 1 ;	/* scaling factors for x transformation	*/
GLOBAL WORD	x_xdiv = 1 ;
GLOBAL WORD	x_xtrans = 0 ;	/* translate factor for x transformation */
GLOBAL WORD	x_ymul = 1 ;	/* scaling factors for x transformation	*/
GLOBAL WORD	x_ydiv = 1 ;
GLOBAL WORD	x_ytrans = 0 ;	/* translate factor for x transformation */


/* multiplies two integers into a long, divides back into an integer.	*/
EXTERN WORD	SMUL_DIV() ;


/*----------------------------------------------------------------------*/
/* return number of pixels in x-direction physically equal to "y"	*/
/* number of pixels in y-direction.					*/
    WORD
x_ytox(y, dev_attr )
    WORD	y ;
    WORD	dev_attr[] ;
{
    return( SMUL_DIV( y, dev_attr[ 4 ], dev_attr[ 3 ] ) ) ;
}


/*----------------------------------------------------------------------*/
/* the following 8 functions can be implemented as #define macros if	*/
/* speed is essential and space is not.					*/


/*----------------------------------------------------------------------*/
/* transform an x-value from user space into device space.		*/
    WORD
x_udx_xform( user_x )
    WORD	user_x ;
{
    return( SMUL_DIV( user_x, x_xmul, x_xdiv ) + x_xtrans) ;
}

/*----------------------------------------------------------------------*/
/* transform a y-value from user space into device space.		*/
    WORD
x_udy_xform(user_y)
    WORD	user_y ;
{
    return( SMUL_DIV( user_y, x_ymul, x_ydiv ) + x_ytrans) ;
}

/*----------------------------------------------------------------------*/
/* transform a x-value from device space into user space.		*/
    WORD
x_dux_xform( x )
    WORD	x ;
{
    return( SMUL_DIV( x - x_xtrans, x_xdiv, x_xmul ) ) ;
}


/*----------------------------------------------------------------------*/
/* transform an x-value from device space into user space.		*/
    WORD
x_duy_xform( y )
    WORD	y ;
{
    return( SMUL_DIV( y - x_ytrans, x_ydiv, x_ymul ) ) ;
}

/*----------------------------------------------------------------------*/
/* scale an x-distance from user space into device space.		*/
    WORD
x_udx_scale( user_dx )
    WORD	user_dx ;
{
    return( SMUL_DIV( user_dx, x_xmul, x_xdiv ) ) ;
}

/*----------------------------------------------------------------------*/
/* scale a y-distance from user space into device space.		*/
    WORD
x_udy_scale( user_dy )
    WORD	user_dy ;
{
    return( SMUL_DIV( user_dy, x_ymul, x_ydiv ) ) ;
}

/*----------------------------------------------------------------------*/
/* scale an x-distance from device space into user space.		*/
    WORD
x_dux_scale( dev_x )
    WORD	dev_x ;
{
    return( SMUL_DIV( dev_x, x_xdiv, x_xmul ) ) ;
}


/*----------------------------------------------------------------------*/
/* scale a x-distance from device space into user space.		*/
    WORD
x_duy_scale( dev_y )
    WORD	dev_y ;
{
    return( SMUL_DIV( dev_y, x_ydiv, x_ymul ) ) ;
}


/*----------------------------------------------------------------------*/
/* set translation values, given widths and heights for both user and	*/
/* device space are set to non-zero.					*/
x_sxform( user_x, user_y, user_w, user_h, dev_x, dev_y, dev_w, dev_h )
    WORD     user_x, user_y, user_w, user_h, dev_x, dev_y, dev_w, dev_h ;
{
    if ( !(dev_w && dev_h && user_w && user_h) )
	return(FALSE);
    x_xmul = dev_w ;
    x_xdiv = user_w ;
    x_ymul = dev_h ;
    x_ydiv = user_h ;
    x_xtrans = dev_x - SMUL_DIV( user_x, x_xmul, x_xdiv ) ;
    x_ytrans = dev_y - SMUL_DIV( user_y, x_ymul, x_ydiv ) ;
} /* x_set_xform */


/*----------------------------------------------------------------------*/
/* this procedure matches an aspect ratio on the device with one 	*/
/* specified in user units. The match is done in physical units 	*/
/* rather than pixels, so a square specified in user units will look	*/
/* square when displayed on the device. Calculating the aspect ratio	*/
/* match in this fashion takes care of devices with non-square pixels.	*/
    VOID
x_saspect( user_w, user_h, dev_w, dev_h, dev_attr )
    WORD	user_w, user_h ;
    WORD	*dev_w, *dev_h ;
    WORD	dev_attr[] ;
{
     LONG	y_ratio, x_ratio ;    

    x_ratio = (LONG)(*dev_w) * (LONG)(dev_attr[ 3 ]/10) * (LONG)(user_h/4) ;
    y_ratio = (LONG)(*dev_h) * (LONG)(dev_attr[ 4 ]/10) * (LONG)(user_w/4) ;
		    /* divide by ten to get pixel sizes back to VDI 1.x	*/
		    /* sizes so longs don't wrap to negative numbers	*/
    if (y_ratio < x_ratio)
    {
	*dev_w = SMUL_DIV( *dev_h, dev_attr[ 4 ], dev_attr[ 3 ] ) ;
	*dev_w = SMUL_DIV( *dev_w, user_w, user_h ) ;
    }
    else 
    {  

	*dev_h = SMUL_DIV( *dev_w, dev_attr[ 3 ], dev_attr[ 4 ] ) ;
	*dev_h = SMUL_DIV( *dev_h, user_h, user_w ) ;
    } 
} /* x_saspect */
