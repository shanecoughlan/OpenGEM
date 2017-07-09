

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vs_color( handle, index, rgb )
WORD handle, index, *rgb;
{
    WORD i;

    intin[0] = index;
    for ( i = 1; i < 4; i++ )
        intin[i] = *rgb++;

    contrl[0] = 14;
    contrl[1] = 0;
    contrl[3] = 4;
    contrl[6] = handle;
    vdi();
}
