

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_gdir(drive, pdrvpath)	/*	get current directory	*/
	WORD		drive;
	LPBYTE		pdrvpath;
{
	DOS_AX = 0x4700;
	DOS_DX = (UWORD) drive;	/* 0 = default drive, 1 = A:,etc */
	DOS_SI = FP_OFF(pdrvpath);
	DOS_DS = FP_SEG(pdrvpath);

	__DOS();

	return(TRUE);
}
