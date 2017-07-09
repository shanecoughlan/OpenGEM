

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
scrp_write(pscrap)
	LPVOID		pscrap;
{
	SC_PATH = pscrap;
	return( gem_if( SCRP_WRITE ) );
}
