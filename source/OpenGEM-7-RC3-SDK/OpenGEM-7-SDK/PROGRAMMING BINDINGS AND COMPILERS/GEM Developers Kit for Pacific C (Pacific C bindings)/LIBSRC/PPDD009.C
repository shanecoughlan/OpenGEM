

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_snext()	/* search for next matching file  		  */
		/* dos_sfirst() must be used just before this one */
{
	DOS_AX = 0x4f00;

	__DOS();

	return(!DOS_ERR);
}
