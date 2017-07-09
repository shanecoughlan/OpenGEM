

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_rename(poname, pnname)	/* rename file */
	LPBYTE		poname;
	LPBYTE		pnname;
{
	DOS_DI = FP_OFF(pnname);
	DOS_ES = FP_SEG(pnname);
	dos_lpvoid(0x5600, poname);
	return((WORD) DOS_AX);
}
