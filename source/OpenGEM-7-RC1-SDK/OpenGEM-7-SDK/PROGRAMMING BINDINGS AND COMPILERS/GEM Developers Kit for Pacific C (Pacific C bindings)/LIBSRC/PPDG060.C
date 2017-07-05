

#include "ppdgem.h"
#include "ppdg0.h"


					/* Resource Manager		*/
	WORD
rsrc_load(rsname)
	LPBYTE	rsname;
{
	RS_PFNAME = rsname;
	return( gem_if(RSRC_LOAD) );
}
