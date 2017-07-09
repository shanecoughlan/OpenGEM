

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_updwk( handle )
WORD handle;
{
    contrl[0] = 4;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();
}
