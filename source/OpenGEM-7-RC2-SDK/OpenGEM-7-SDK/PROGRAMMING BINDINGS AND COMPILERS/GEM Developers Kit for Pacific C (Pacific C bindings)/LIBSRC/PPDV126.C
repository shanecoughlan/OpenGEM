

#include "ppdgem.h"
#include "ppdv0.h"


    VOID
v_orient( handle, orientation )
WORD	handle, orientation ;
{
    contrl[ 0 ] = 5 ;
    contrl[ 1 ] = 0 ;
    contrl[ 3 ] = 1 ;
    contrl[ 5 ] = 27 ;
    contrl[ 6 ] = handle ;
    intin[ 0 ] = orientation ;
    vdi() ;
} /* v_orient() */

/* end of vtray.c */

    VOID
v_tray( handle, tray )
WORD	handle, tray ;
{
    contrl[ 0 ] = 5 ;
    contrl[ 1 ] = 0 ;
    contrl[ 3 ] = 1 ;
    contrl[ 5 ] = 29 ;
    contrl[ 6 ] = handle ;
    intin[ 0 ] = tray ;
    vdi() ;
} /* v_tray() */

/* end of vtray.c */

    WORD
v_xbit_image( handle, filename, aspect, x_scale, y_scale, h_align, v_align, 
			rotate, background, foreground, xy )
WORD handle, aspect, x_scale, y_scale, h_align, v_align;
WORD rotate, background, foreground;
WORD xy[];
BYTE *filename;
{
    WORD ii;

    for (ii = 0; ii < 4; ii++)
	ptsin[ ii ] = xy[ ii ] ;
    intin[0] = aspect;
    intin[1] = x_scale;
    intin[2] = y_scale;
    intin[3] = h_align;
    intin[4] = v_align;
    intin[5] = rotate ;
    intin[6] = background ;
    intin[7] = foreground ;
    ii = 8 ;
    while ( *filename )
    	{
	    intin[ ii++ ] = (WORD)( *filename++ ) ;
	}
    intin[ ii ] = 0 ;		/* final null */
    contrl[0] = 5;
    contrl[1] = 2;
    contrl[3] = ii ;
    contrl[5] = 101 ;
    contrl[6] = handle ; 
    vdi();
    return( TRUE ) ;
}
