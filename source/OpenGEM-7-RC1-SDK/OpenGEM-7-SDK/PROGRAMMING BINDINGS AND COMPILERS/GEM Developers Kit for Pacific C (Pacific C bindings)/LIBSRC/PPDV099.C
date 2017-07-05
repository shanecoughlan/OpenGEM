

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vst_point( handle, point, char_width, char_height, cell_width, cell_height )
WORD handle, point, *char_width, *char_height, *cell_width, *cell_height;
{
    intin[0] = point;

    contrl[0] = 107;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();

    *char_width = ptsout[0];
    *char_height = ptsout[1];
    *cell_width = ptsout[2];
    *cell_height = ptsout[3];
    return( intout[0] );
}
