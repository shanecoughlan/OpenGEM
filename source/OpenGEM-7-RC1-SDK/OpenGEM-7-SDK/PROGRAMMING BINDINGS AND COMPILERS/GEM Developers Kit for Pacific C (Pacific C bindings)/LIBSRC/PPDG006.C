

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
appl_tplay(tbuffer, tlength, tscale)
	LPVOID		tbuffer;
	WORD		tlength;
	WORD		tscale;
{
	AP_TBUFFER = tbuffer;
	AP_TLENGTH = tlength;
	AP_TSCALE = tscale;
	return( gem_if(APPL_TPLAY) );
}
