

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vst_load_fonts( handle, select )
WORD handle, select;
{
    contrl[0] = 119;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
