

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vq_chcells( handle, rows, columns )
WORD handle, *rows, *columns;
{
    contrl[0] = 5;
    contrl[1] = contrl[3] = 0;
    contrl[5] = 1;
    contrl[6] = handle;
    vdi();

    *rows = intout[0];
    *columns = intout[1];
}
