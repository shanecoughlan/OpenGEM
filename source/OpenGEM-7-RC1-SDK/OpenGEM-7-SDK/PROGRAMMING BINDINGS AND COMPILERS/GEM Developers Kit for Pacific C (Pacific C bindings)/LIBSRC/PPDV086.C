

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vsin_mode( handle, dev_type, mode )
WORD handle, dev_type, mode;
{
    intin[0] = dev_type;
    intin[1] = mode;

    contrl[0] = 33;
    contrl[1] = 0;
    contrl[3] = 2;
    contrl[6] = handle;
    vdi();
}
