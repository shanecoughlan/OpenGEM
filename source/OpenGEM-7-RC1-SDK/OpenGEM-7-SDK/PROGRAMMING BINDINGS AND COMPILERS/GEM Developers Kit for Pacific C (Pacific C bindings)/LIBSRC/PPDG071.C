

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
xgrf_stepcalc(orgw, orgh, xc, yc, w, h, pcx, pcy, pcnt, pxstep, pystep)
	WORD		orgw, orgh;
	WORD		xc, yc, w, h;
	WORD		*pcx, *pcy;
	WORD		*pcnt, *pxstep, *pystep;
{
	XGR_I1 = orgw;
	XGR_I2 = orgh;
	XGR_I3 = xc;
	XGR_I4 = yc;
	XGR_I5 = w;
	XGR_I6 = h;
	gem_if( XGRF_STEPCALC );
	*pcx = XGR_O1;
	*pcy = XGR_O2;
	*pcnt = XGR_O3;
	*pxstep = XGR_O4;
	*pystep = XGR_O5;
	return((WORD) RET_CODE );
}
