

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_get_pixel( handle, x, y, pel, index )
WORD handle, x, y, *pel, *index;
{
    ptsin[0] = x;
    ptsin[1] = y;

    contrl[0] = 105;
    contrl[1] = 1;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    *pel = intout[0];
    *index = intout[1];
}
