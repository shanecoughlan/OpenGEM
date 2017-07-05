

#include "ppdgem.h"
#include "ppdv0.h"




    WORD
vqin_mode( handle, dev_type, mode )
WORD handle, dev_type, *mode;
{
    intin[0] = dev_type;

    contrl[0] = 115;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();

    *mode = intout[0];
}
