

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
wind_find(mx, my)
	WORD		mx, my;
{
	WM_MX = mx;
	WM_MY = my;
	return( gem_if( WIND_FIND ) );
}
