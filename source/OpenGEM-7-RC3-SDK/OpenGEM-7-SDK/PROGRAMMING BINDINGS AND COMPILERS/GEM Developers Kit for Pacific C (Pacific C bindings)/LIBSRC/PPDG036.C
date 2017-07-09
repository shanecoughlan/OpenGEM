

#include "ppdgem.h"
#include "ppdg0.h"


	WORD
form_center(tree, pcx, pcy, pcw, pch)
	LPTREE		tree;
	WORD		*pcx, *pcy, *pcw, *pch;
{
	FM_FORM = tree;
	gem_if(FORM_CENTER);
	*pcx = FM_XC;
	*pcy = FM_YC;
	*pcw = FM_WC;
	*pch = FM_HC;
	return((WORD) RET_CODE );
}
