

#include "ppdgem.h"
#include "ppdv0.h"

	VOID
v_etext( handle, x, y, string, offsets)
WORD handle, x, y, offsets[];
UBYTE string[];
{
    WORD	*intptr, *ptsptr;
    
    intptr = intin;
    ptsptr = ptsin;
    *ptsptr++ = x;
    *ptsptr++ = y;
    while ((*intptr++ = *string++) != 0) {
	*ptsptr++ = *offsets++;
	*ptsptr++ = *offsets++;
    }
    contrl[0] = 11;
    contrl[1] = intptr - intin;
    contrl[3] = intptr - intin - 1;
    contrl[5] = 11;
    contrl[6] = handle;
    vdi();
}
