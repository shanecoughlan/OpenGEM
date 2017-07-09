

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vsm_height( handle, height )
WORD handle, height;
{
    WORD t;
    ptsin[0] = 0;
    ptsin[1] = height;

    contrl[0] = 19;
    contrl[1] = 1;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();
    t = ptsout[1];
    return( t );
}
