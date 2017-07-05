
#include "ppdgem.h"
#include "ppdg0.h"



/* ViewMAX/3 beta: Set desktop image */

WORD xgrf_dtimage(LPMFDB lpImage)
{
	XGR_DTIMAGE = lpImage;
	return( gem_if( XGRF_DTIMAGE ));
}

