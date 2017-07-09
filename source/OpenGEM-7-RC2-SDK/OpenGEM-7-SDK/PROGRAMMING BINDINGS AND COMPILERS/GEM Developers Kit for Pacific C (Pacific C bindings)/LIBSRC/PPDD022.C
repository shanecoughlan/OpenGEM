

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_mkdir(ppath)	/* create a sub-directory */
	LPBYTE		ppath;
{
	dos_lpvoid(0x3900, ppath);
	return(!DOS_ERR);
}
