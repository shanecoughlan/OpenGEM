

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
form_button(form, obj, clks, pnxt_obj)
	LPVOID		form;
	WORD		obj;
	WORD		clks;
	WORD		*pnxt_obj;
{
	FM_FORM = form;
	FM_OBJ = obj;
	FM_CLKS = clks;
	gem_if( FORM_BUTTON );
	*pnxt_obj = FM_ONXTOB;
	return((WORD) RET_CODE );
}
