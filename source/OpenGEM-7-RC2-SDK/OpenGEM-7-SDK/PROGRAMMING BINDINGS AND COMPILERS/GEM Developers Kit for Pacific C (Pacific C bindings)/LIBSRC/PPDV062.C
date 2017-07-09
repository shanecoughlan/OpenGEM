

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vst_rotation( handle, angle )
WORD handle, angle;
{
    intin[0] = angle;

    contrl[0] = 13;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
