

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_free(maddr)	/* free memory that was allocated via dos_alloc() */
	LPVOID	maddr;
{
	DOS_AX = 0x4900;
	DOS_ES = FP_SEG(maddr);

	__DOS();

	return((WORD) DOS_AX);
}
