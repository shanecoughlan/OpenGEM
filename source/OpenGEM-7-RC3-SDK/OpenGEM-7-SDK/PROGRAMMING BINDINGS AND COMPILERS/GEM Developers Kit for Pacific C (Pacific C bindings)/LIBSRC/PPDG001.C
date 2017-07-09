

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
appl_init(LPXBUF lpx )
{
	gb.gb_pcontrol = &control[0]; 
	gb.gb_pglobal = &global[0];
	gb.gb_pintin = &int_in[0];
	gb.gb_pintout = &int_out[0];
	gb.gb_padrin = &addr_in[0];
	gb.gb_padrout = &addr_out[0];

	addr_in[0] = (LPVOID)lpx;	/* [JCE] XBLK parameter */
	
	ad_g =&gb;
	gem_if(APPL_INIT);
	return((WORD) RET_CODE );
}
