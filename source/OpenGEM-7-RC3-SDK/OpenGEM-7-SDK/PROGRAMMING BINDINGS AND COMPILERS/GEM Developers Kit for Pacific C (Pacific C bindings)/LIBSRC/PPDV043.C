

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vq_tdimensions( handle, xdimension, ydimension )
WORD handle, *xdimension, *ydimension;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[5] = 84;
    contrl[6] = handle;
    vdi();
    *xdimension = intout[0];
    *ydimension = intout[1];
}
