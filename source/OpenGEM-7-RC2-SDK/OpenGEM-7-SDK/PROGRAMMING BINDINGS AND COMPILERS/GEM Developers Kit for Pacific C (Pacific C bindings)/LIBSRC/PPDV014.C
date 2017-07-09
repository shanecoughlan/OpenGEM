

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vs_curaddress( handle, row, column )
WORD handle, row, column;
{
    intin[0] = row;
    intin[1] = column;

    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 2;
    contrl[5] = 11;
    contrl[6] = handle;
    vdi();
}
