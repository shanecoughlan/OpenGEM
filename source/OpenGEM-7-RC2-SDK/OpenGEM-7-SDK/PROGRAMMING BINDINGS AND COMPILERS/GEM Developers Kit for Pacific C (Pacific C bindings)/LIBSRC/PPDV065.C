

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vsl_width( handle, width )
WORD handle, width;
{
    ptsin[0] = width;
    ptsin[1] = 0;

    contrl[0] = 16;
    contrl[1] = 1;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();
    return( ptsout[0] );
}
