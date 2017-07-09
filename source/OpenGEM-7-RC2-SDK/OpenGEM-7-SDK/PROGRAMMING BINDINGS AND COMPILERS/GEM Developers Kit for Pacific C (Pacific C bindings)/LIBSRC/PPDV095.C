

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_contourfill( handle, x, y, index )
WORD handle, x, y, index;
{
    intin[0] = index;
    ptsin[0] = x;
    ptsin[1] = y;

    contrl[0] = 103;
    contrl[1] = contrl[3] = 1;
    contrl[6] = handle;
    vdi();
}
