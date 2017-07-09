

#include "ppdgem.h"
#include "ppdg0.h"




	WORD
appl_trecord(tbuffer, tlength)
	LPVOID		tbuffer;
	WORD		tlength;
{
	AP_TBUFFER = tbuffer;
	AP_TLENGTH = tlength;
	return( gem_if(APPL_TRECORD) );
}
