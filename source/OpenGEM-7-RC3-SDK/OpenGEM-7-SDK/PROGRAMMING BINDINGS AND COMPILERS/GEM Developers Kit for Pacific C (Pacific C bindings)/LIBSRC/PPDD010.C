

#include "ppdgem.h"
#include "ppdd0.h"



	WORD
dos_open(pname, access)	/* open file */
	LPBYTE		pname; /* filename */
	WORD		access;/* 0 = read, 1 = write, 2 = both */
{
	dos_lpvoid((UWORD) 0x3d00 + access, pname);
	return((WORD) DOS_AX);	/* DOS_AX contains file handle */
}
