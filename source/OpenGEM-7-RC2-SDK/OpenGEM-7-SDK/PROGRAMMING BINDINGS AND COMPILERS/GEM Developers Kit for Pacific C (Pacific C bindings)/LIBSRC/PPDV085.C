

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vswr_mode( handle, mode )
WORD handle, mode;
{
    intin[0] = mode;

    contrl[0] = 32;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
