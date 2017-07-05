

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_arc( handle, xc, yc, rad, sang, eang )
WORD handle, xc, yc, rad, sang, eang;
{
    ptsin[0] = xc;
    ptsin[1] = yc;
    ptsin[2] = ptsin[3] = ptsin[4] = ptsin[5] = ptsin[7] = 0;
    ptsin[6] = rad;
    intin[0] = sang;
    intin[1] = eang;

    contrl[0] = 11;
    contrl[1] = 4;
    contrl[3] = 2;
    contrl[5] = 2;
    contrl[6] = handle;
    vdi();
}
