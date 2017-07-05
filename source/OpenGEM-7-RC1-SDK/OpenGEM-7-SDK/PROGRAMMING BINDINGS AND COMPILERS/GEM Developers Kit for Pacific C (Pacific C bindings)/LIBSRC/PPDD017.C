

#include "ppdgem.h"
#include "ppdd0.h"


	LONG
dos_avail()  /* Returns the amount of memory available in paragraphs */
{
	LONG		mlen;

	DOS_AX = 0x4800;
	DOS_BX = 0xffff;

	__DOS();

	mlen = ((LONG) DOS_BX) << 4;
	return(mlen);
}
