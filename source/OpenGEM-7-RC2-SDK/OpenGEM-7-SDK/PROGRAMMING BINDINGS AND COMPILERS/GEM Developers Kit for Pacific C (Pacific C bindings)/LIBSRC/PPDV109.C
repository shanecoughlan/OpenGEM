

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vqt_width( handle, character, cell_width, left_delta, right_delta )
WORD handle, *cell_width, *left_delta, *right_delta;
BYTE character;
{
    intin[0] = character;

    contrl[0] = 117;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();

    *cell_width = ptsout[0];
    *left_delta = ptsout[2];
    *right_delta = ptsout[4];
    return( intout[0] );
}
