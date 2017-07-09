

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
rsrc_gaddr(rstype, rsid, paddr)
	WORD		rstype;
	WORD		rsid;
	LPVOID		*paddr;
{
	RS_TYPE = rstype;
	RS_INDEX = rsid;
	gem_if(RSRC_GADDR);
	*paddr = RS_OUTADDR;
	return((WORD) RET_CODE );
}
