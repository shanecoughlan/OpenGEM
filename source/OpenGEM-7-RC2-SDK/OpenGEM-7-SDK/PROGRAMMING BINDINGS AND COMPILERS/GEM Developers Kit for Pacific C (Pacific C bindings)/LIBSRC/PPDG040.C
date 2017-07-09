

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
graf_dragbox(w, h, sx, sy, xc, yc, wc, hc, pdx, pdy)
	WORD		w, h;
	WORD		sx, sy;
	WORD		xc, yc, wc, hc;
	WORD		*pdx, *pdy;
{
	GR_I1 = w;
	GR_I2 = h;
	GR_I3 = sx;
	GR_I4 = sy;
	GR_I5 = xc;
	GR_I6 = yc;
	GR_I7 = wc;
	GR_I8 = hc;
	gem_if( GRAF_DRAGBOX );
	*pdx = GR_O1;
	*pdy = GR_O2;
	return((WORD) RET_CODE );
}
