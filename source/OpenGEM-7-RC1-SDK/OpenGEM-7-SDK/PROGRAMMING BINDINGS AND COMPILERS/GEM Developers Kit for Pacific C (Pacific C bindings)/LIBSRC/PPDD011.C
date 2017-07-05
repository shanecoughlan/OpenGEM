

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_close(handle)	/* close file */
	WORD		handle;
{
	DOS_AX = 0x3e00;
	DOS_BX = handle;

	__DOS();

	return(!DOS_ERR);
}
