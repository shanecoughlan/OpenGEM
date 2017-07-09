

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
shel_find(ppath)
	LPVOID		ppath;
{
	SH_PATH = ppath;
	return( gem_if( SHEL_FIND ) );
}
