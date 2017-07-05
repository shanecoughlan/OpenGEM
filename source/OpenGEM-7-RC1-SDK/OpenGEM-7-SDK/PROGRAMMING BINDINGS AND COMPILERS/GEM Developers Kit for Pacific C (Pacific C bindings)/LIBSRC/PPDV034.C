

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vsc_expose( handle, state )
WORD handle;
WORD state;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 2;
    contrl[5] = 93;
    contrl[6] = handle;
    intin[0] = state;
    vdi();
}
