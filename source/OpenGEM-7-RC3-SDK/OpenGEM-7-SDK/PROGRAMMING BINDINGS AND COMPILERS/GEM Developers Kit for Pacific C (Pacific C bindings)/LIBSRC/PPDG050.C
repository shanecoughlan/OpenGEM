

#include "ppdgem.h"
#include "ppdg0.h"



				/* fseler Manager		*/
	WORD
fsel_input(pipath, pisel, pbutton)
	LPVOID		pipath, pisel;
	WORD		*pbutton;
{
	FS_IPATH = pipath;
	FS_ISEL = pisel;
	gem_if( FSEL_INPUT );
	*pbutton = FS_BUTTON;
	return((WORD) RET_CODE );
}
