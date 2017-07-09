

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vq_curaddress( handle, row, column )
WORD handle, *row, *column;
{
    contrl[0] = 5;
    contrl[1] = contrl[3] = 0;
    contrl[5] = 15;
    contrl[6] = handle;
    vdi();

    *row = intout[0];
    *column = intout[1];
}
