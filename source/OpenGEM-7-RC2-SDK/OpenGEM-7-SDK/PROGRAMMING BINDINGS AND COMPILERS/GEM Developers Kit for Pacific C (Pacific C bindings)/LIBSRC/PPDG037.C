

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
form_keybd(form, obj, nxt_obj, thechar, pnxt_obj, pchar)
	LPVOID		form;
	WORD		obj;
	WORD		nxt_obj;
	WORD		thechar;
	WORD		*pnxt_obj;
	WORD		*pchar;
{
	FM_FORM = form;
	FM_OBJ = obj;
	FM_INXTOB = nxt_obj;
	FM_ICHAR = thechar;
	gem_if( FORM_KEYBD );
	*pnxt_obj = FM_ONXTOB;
	*pchar = FM_OCHAR;
	return((WORD) RET_CODE );
}
