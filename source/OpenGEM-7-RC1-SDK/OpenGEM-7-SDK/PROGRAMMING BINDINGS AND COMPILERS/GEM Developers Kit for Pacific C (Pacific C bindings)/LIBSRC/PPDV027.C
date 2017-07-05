

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vq_scan( handle, g_height, g_slice, a_height, a_slice, factor )
WORD handle, *g_height, *g_slice, *a_height, *a_slice, *factor;
{
    contrl[0] = 5;
    contrl[1] = contrl[3] = 0;
    contrl[5] = 24;
    contrl[6] = handle;
    vdi();

    *g_height = intout[0];
    *g_slice = intout[1];
    *a_height = intout[2];
    *a_slice = intout[3];
    *factor = intout[4];
}
