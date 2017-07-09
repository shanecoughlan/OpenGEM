

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_create(pname, attr)	/* create file */
	LPBYTE		pname;
	WORD		attr;
{
	DOS_CX = attr;
	dos_lpvoid(0x3c00, pname);

	return((WORD) DOS_AX);
}
