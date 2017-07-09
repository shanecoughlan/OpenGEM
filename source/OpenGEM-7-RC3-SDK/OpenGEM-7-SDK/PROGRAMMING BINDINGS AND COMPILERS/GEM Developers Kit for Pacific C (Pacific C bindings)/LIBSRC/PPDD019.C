

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_space(drv, ptotal, pavail)	/* get disk free space     */
	WORD		drv;	/* 0 = default, 1 = A: etc */
	LONG		*ptotal, *pavail;
{
	DOS_AX = 0x3600;
	DOS_DX = drv;
	__DOS();

	/*	DOS_AX contains number of sectors per cluster */
	/*	DOS_BX contains number of available clusters  */
	/*	DOS_CX contains number of bytes per sector    */
	/*	DOS_DX contains total number of clusters      */
	
	DOS_AX *= DOS_CX;
	*ptotal = (LONG) DOS_AX * (LONG) DOS_DX;
	*pavail = (LONG) DOS_AX * (LONG) DOS_BX;
}
