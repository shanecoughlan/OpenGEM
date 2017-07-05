

#include "ppdgem.h"
#include "ppdg0.h"



					/* Shell Manager		*/
	WORD
shel_read(pcmd, ptail)
	LPVOID		pcmd, ptail;
{
	SH_PCMD = pcmd;
	SH_PTAIL = ptail;
	return( gem_if( SHEL_READ ) );
}
