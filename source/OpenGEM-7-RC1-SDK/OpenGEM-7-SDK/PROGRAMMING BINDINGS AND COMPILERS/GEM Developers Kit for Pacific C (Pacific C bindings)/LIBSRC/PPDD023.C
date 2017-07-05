

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_delete(pname)	/* delete file */
	LPBYTE		pname;
{
	dos_lpvoid(0x4100, pname);
	return((WORD) DOS_AX);
}
