

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vq_color( handle, index, set_flag, rgb )
WORD handle, index, set_flag, rgb[];
{
    intin[0] = index;
    intin[1] = set_flag;

    contrl[0] = 26;
    contrl[1] = 0;
    contrl[3] = 2;
    contrl[6] = handle;
    vdi();

    rgb[0] = intout[1];
    rgb[1] = intout[2];
    rgb[2] = intout[3];
}
