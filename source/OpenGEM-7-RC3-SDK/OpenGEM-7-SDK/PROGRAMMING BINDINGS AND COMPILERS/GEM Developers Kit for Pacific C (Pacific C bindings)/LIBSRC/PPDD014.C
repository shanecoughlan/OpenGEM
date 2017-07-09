

#include "ppdgem.h"
#include "ppdd0.h"



	LONG
dos_lseek(handle, smode, sofst) /* move file read / write pointer */
	WORD		handle;
	WORD		smode; /* 0 = from beginning, 1 from current */
			       /* 2 = EOF plus offset                */
	LONG		sofst; /* offset in bytes 		     */
{
	DOS_AX = 0x4200;
	DOS_AX += smode;
	DOS_BX = handle;
	DOS_CX = LHIWD(sofst); /* contains the      */
	DOS_DX = LLOWD(sofst); /* desired offset    */

	__DOS();

	return(LONG)( DOS_AX + HW(DOS_DX ));   /* return pointers new location */
}
