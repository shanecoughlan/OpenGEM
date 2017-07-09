

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
menu_unregister(mid)
	WORD		mid;
{
	MM_MID = mid;
	return( gem_if( MENU_UNREGISTER ) );
}
