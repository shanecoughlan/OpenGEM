

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_sdrv(newdrv)	/* select new drive, new drive becomes default */
	WORD		newdrv;
{
	DOS_AX = 0x0e00;
	DOS_DX = newdrv;

	__DOS();

	return((WORD) DOS_AX & 0x00ff);	/* 	0 = A:, 1 = B: etc */
}
