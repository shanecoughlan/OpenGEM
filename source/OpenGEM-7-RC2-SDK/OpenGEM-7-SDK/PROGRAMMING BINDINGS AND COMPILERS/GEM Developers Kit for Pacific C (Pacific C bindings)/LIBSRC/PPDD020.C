

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_rmdir(ppath)	/* remove directory entry */
	LPBYTE		ppath;
{
	dos_lpvoid(0x3a00, ppath);
	return(!DOS_ERR);
}
