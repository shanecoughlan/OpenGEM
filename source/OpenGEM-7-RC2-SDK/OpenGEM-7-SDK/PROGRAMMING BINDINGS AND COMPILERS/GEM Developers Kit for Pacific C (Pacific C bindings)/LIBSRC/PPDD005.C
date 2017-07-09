

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_gdrv()	/*	get current drive	*/
{
	DOS_AX = 0x1900;

	__DOS();
	return((WORD) DOS_AX & 0x00ff);	/* 	0 = A:, 1 = B: etc */
}
