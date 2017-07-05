

#include "ppdgem.h"
#include "ppdg0.h"


					/* Graphics Manager		*/
	WORD
graf_rubbox(xorigin, yorigin, wmin, hmin, pwend, phend)
	WORD		xorigin, yorigin;
	WORD		wmin, hmin;
	WORD		*pwend, *phend;
{
	GR_I1 = xorigin;
	GR_I2 = yorigin;
	GR_I3 = wmin;
	GR_I4 = hmin;
	gem_if( GRAF_RUBBOX );
	*pwend = GR_O1;
	*phend = GR_O2;
	return((WORD) RET_CODE );
}
