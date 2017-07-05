

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
rsrc_saddr(rstype, rsid, lngval)
	WORD		rstype;
	WORD		rsid;
	LPVOID		lngval;
{
	RS_TYPE = rstype;
	RS_INDEX = rsid;
	RS_INADDR = lngval;
	return( gem_if(RSRC_SADDR) );
}
