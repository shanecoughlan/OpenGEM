

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
appl_read(rwid, length, pbuff)
	WORD		rwid;
	WORD		length;
	LPVOID		pbuff;
{
	AP_RWID = rwid;
	AP_LENGTH = length;
	AP_PBUFF = pbuff;
	return( gem_if(APPL_READ) );
}
