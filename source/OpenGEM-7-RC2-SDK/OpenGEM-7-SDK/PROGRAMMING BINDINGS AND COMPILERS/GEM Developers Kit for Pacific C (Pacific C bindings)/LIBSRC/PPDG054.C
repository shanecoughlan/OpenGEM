

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
wind_delete(handle)
	WORD		handle;
{
	WM_HANDLE = handle;
	return( gem_if( WIND_DELETE ) );
}
