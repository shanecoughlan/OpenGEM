

#include "ppdgem.h"
#include "ppdg0.h"


	WORD
form_error(errnum)
	WORD		errnum;
{
	FM_ERRNUM = errnum;
	return( gem_if( FORM_ERROR ) );
}
