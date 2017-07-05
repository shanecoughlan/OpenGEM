

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
shel_envrn(ppath, psrch)
	LPVOID		ppath;
	LPVOID		psrch;
{
	SH_PATH = ppath;
	SH_SRCH = psrch;
	return( gem_if( SHEL_ENVRN ) );
}
