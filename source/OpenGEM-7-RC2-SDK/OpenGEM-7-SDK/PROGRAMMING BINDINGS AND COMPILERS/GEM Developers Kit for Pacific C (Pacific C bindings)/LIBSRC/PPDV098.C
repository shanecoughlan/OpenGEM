

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vst_effects( handle, effect )
WORD handle, effect;
{
    intin[0] = effect;

    contrl[0] = 106;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
