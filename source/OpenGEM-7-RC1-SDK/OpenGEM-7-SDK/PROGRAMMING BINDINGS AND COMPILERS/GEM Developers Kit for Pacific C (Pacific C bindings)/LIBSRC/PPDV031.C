

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vs_mute( handle, action )
WORD handle;
WORD action;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[5] = 62;
    contrl[6] = handle;

    intin[0] = action;
    vdi();
    return( intout[0] );
}
