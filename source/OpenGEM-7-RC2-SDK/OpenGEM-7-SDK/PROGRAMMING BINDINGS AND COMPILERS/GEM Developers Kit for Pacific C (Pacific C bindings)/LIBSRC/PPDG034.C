

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
form_alert(defbut, astring)
	WORD		defbut;
	LPBYTE		astring;
{
	FM_DEFBUT = defbut;
	FM_ASTRING = astring;
	return( gem_if( FORM_ALERT ) );
}
