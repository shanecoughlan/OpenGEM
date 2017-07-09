

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_show_c( handle, reset )
WORD handle, reset;
{
    intin[0] = reset;

    contrl[0] = 122;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
}
