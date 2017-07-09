

#include "ppdgem.h"
#include "ppdv0.h"


    BOOLEAN
vqp_filmname( handle, index, name )
WORD handle;
WORD index;
BYTE name[];
{
    WORD i;

    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[5] = 92;
    contrl[6] = handle;
    intin[0] = index;
    vdi();

    if ( contrl[4] != 0) {
	for (i = 0; i < contrl[4]; i++)
	    name[i] = intout[i];
	return(TRUE);
    }
    return(FALSE);
}
