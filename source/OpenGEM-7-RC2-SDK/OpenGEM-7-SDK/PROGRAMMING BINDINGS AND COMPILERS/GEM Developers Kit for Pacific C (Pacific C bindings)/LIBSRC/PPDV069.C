

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vsm_color( handle, index )
WORD handle, index;
{
    intin[0] = index;

    contrl[0] = 20;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
