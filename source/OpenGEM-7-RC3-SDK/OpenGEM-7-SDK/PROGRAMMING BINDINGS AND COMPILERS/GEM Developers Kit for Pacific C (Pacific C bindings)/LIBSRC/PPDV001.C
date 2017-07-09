

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_clswk( handle )
WORD handle;
{
    contrl[0] = 2;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();
}
