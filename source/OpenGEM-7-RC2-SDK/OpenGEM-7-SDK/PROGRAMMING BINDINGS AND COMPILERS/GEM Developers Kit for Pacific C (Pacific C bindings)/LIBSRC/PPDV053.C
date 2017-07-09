

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_circle( handle, xc, yc, rad )
WORD handle, xc, yc, rad;
{
    ptsin[0] = xc;
    ptsin[1] = yc;
    ptsin[2] = ptsin[3] = ptsin[5] = 0;
    ptsin[4] = rad;

    contrl[0] = 11;
    contrl[1] = 3;
    contrl[3] = 0;
    contrl[5] = 4;
    contrl[6] = handle;
    vdi();
}
