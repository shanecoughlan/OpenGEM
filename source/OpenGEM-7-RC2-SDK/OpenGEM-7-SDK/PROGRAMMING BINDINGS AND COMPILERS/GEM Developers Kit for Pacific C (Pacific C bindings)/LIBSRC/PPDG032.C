

#include "ppdgem.h"
#include "ppdg0.h"




					/* Form Manager			*/
	WORD
form_do(form, start)
	LPVOID		form;
	WORD		start;
{
	FM_FORM = form;
	FM_START = start;
	return( gem_if( FORM_DO ) );
}
