

#include "ppdgem.h"
#include "ppdd0.h"


	LPVOID
dos_alloc(nbytes)	/* allocate memory */
	LONG		nbytes;
{
	LPVOID		maddr;

	DOS_AX = 0x4800;
	if (nbytes == 0xFFFFFFFFL)	/* convert number */
	  DOS_BX = 0xffff;		/* 	of bytes  */
	else				/*	to	  */
	  DOS_BX = (nbytes + 15L) >> 4L;/*	paragraphs*/
	__DOS();

	if (DOS_ERR)
	  maddr = (LPVOID)NULL;
	else
	  maddr = MK_FP(DOS_AX, 0);

	return(maddr); /* return location of allocated memory block */
}
