

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vq_key_s( handle, status )
WORD handle, *status;
{
    contrl[0] = 128;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    *status = intout[0];
}
