

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vt_origin ( handle, xorigin, yorigin )
WORD    handle, xorigin, yorigin;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 2;
    contrl[5] = 83;
    contrl[6] = handle;
    intin[0] = xorigin;
    intin[1] = yorigin;
    vdi();
}
