

#include "ppdgem.h"
#include "ppdg0.h"


/* Added JCE 7 Jan 1998 - the ViewMAX "set colours" call. */

	WORD
xgrf_colour( type, fg, bg, style, index )
{
	XGR_I1 = type;
	XGR_I2 = fg;	/* Foreground colour */
	XGR_I3 = bg;	/* Background colour */
	XGR_I4 = style;	/* Fill style */
	XGR_I5 = index;	/* Fill index */
	return ( gem_if ( XGRF_COLOUR ) );
}
