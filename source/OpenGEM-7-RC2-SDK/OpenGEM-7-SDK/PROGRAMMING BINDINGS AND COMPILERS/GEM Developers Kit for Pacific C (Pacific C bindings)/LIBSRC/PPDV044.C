

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vt_alignment( handle, dx, dy )
WORD    handle, dx, dy;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 2;
    contrl[5] = 85;
    contrl[6] = handle;
    intin[0] = dx;
    intin[1] = dy;
    vdi();
}
