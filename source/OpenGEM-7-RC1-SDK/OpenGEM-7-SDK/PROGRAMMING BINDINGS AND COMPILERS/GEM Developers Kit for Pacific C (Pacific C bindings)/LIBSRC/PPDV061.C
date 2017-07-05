

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vst_height( handle, height, char_width, char_height, cell_width, cell_height )
WORD handle, height, *char_width, *char_height, *cell_width, *cell_height;
{
    ptsin[0] = 0;
    ptsin[1] = height;

    contrl[0] = 12;
    contrl[1] = 1;
    contrl[3] = 0;
    contrl[6] = handle;

    vdi();

    *char_width = ptsout[0];
    *char_height = ptsout[1];
    *cell_width = ptsout[2];
    *cell_height = ptsout[3];
}
