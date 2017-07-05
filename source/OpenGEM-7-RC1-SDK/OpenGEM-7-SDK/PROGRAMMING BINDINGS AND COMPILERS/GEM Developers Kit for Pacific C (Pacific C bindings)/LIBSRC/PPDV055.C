

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_ellarc( handle, xc, yc, xrad, yrad, sang, eang )
WORD handle, xc, yc, xrad, yrad, sang, eang;
{
    ptsin[0] = xc;
    ptsin[1] = yc;
    ptsin[2] = xrad;
    ptsin[3] = yrad;
    intin[0] = sang;
    intin[1] = eang;

    contrl[0] = 11;
    contrl[1] = 2;
    contrl[3] = 2;
    contrl[5] = 6;
    contrl[6] = handle;
    vdi();
}
