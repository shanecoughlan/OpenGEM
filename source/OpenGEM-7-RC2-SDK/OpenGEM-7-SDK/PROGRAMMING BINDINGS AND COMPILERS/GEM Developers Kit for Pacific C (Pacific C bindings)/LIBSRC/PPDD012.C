

#include "ppdgem.h"
#include "ppdd0.h"


UWORD read_piece(WORD handle, UWORD cnt, LPVOID pbuffer)	/* read file */
{
	DOS_CX = cnt;
	DOS_BX = handle;
	dos_lpvoid(0x3f00, pbuffer);
	return((WORD) DOS_AX);	/* DOS_AX = number of bytes actually read */
}
