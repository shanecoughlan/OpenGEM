

#include "ppdgem.h"
#include "ppdg0.h"



				/* fseler Manager		*/
	WORD
fsel_exinput(pipath, pisel, pbutton, title)
	LPVOID		pipath, pisel;
	WORD		*pbutton;
	LPBYTE		title;
{
	FS_IPATH = pipath;
	FS_ISEL = pisel;
	FS_ITITLE = title;
	gem_if( FSEL_EXINPUT );
	*pbutton = FS_BUTTON;
	return((WORD) RET_CODE );
}
