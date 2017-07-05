

#include "ppdgem.h"
#include "ppdd0.h"



UWORD write_piece(WORD handle, UWORD cnt, LPVOID pbuffer)	/*  write to a file */
{
	DOS_CX = cnt;
	DOS_BX = handle;
	dos_lpvoid(0x4000, pbuffer);
	return((WORD) DOS_AX);
}
