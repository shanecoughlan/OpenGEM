

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vm_coords( handle, llx, lly, urx, ury )
WORD handle, llx, lly, urx, ury;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 5;
    contrl[5] = 99;
    contrl[6] = handle;
    intin[0] = 1;
    intin[1] = llx;
    intin[2] = lly;
    intin[3] = urx;
    intin[4] = ury;
    vdi();
}
