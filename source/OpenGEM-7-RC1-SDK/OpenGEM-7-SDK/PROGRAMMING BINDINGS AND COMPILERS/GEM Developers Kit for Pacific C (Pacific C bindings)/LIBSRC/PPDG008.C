

#include "ppdgem.h"
#include "ppdg0.h"


	WORD
appl_bvset(bvdisk, bvhard)
	UWORD		bvdisk;
	UWORD		bvhard;
{
	AP_BVDISK = bvdisk;
	AP_BVHARD = bvhard;
	return( gem_if(APPL_BVSET) );
}
