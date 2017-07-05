

#include "ppdgem.h"
#include "ppdg0.h"


	WORD
xgrf_2box( xc, yc, w, h, corners, cnt, xstep, ystep, doubled)
	WORD		xc, yc, w, h;
	WORD		corners;
	WORD		cnt;
	WORD		xstep, ystep;
	WORD		doubled;

{
	XGR_I1 = cnt;
	XGR_I2 = xstep;
	XGR_I3 = ystep;
	XGR_I4 = doubled;
	XGR_I5 = corners;
	XGR_I6 = xc;
	XGR_I7 = yc;
	XGR_I8 = w;
	XGR_I9 = h;
	return( gem_if( XGRF_2BOX ) );
}
