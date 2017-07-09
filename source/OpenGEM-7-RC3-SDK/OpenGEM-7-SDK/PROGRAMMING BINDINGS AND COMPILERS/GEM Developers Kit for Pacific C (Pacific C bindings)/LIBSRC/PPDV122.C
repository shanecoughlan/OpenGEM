

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vqt_name( handle, element_num, name )
WORD handle, element_num;
BYTE name[];
{
    WORD i;

    intin[0] = element_num;

    /* GEM/5 tends not to touch intout for font 1. In which case,
     * better it should return a load of zeroes than random data
     */
    for (i = 0; i <= 32; i++) intout[i] = 0;

    contrl[0] = 130;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();

    for (i = 0 ; i < 32 ; i++)
	name[i] = intout[i + 1];
    return( intout[0] );
}
