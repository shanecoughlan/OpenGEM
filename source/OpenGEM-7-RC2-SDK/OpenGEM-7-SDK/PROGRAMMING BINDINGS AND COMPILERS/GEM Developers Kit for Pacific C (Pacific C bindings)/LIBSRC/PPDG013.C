

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
evnt_mesag(pbuff)
	LPVOID		pbuff;
{
	ME_PBUFF = pbuff;
	return( gem_if(EVNT_MESAG) );
}
