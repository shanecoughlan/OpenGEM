

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vqt_font_info( handle, minADE, maxADE, distances, maxwidth, effects )
WORD handle, *minADE, *maxADE, distances[], *maxwidth, effects[];
{
    contrl[0] = 131;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    *minADE = intout[0];
    *maxADE = intout[1];
    *maxwidth = ptsout[0];
    distances[0] = ptsout[1];
    distances[1] = ptsout[3];
    distances[2] = ptsout[5];
    distances[3] = ptsout[7];
    distances[4] = ptsout[9];
    effects[0] = ptsout[2];
    effects[1] = ptsout[4];
    effects[2] = ptsout[6];
}
