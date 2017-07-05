

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vsp_film( handle, index, lightness )
WORD handle;
WORD index;
WORD lightness;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 2;
    contrl[5] = 91;
    contrl[6] = handle;
    intin[0] = index;
    intin[1] = lightness;
    vdi();
}
