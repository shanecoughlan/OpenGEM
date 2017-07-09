

#include "ppdgem.h"
#include "ppdg0.h"


					/* Event Manager		*/
	UWORD
evnt_keybd()
{
	return((UWORD) gem_if(EVNT_KEYBD) );
}
