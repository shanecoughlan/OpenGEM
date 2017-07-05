

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vst_unload_fonts( handle, select )
WORD handle, select;
{
    contrl[0] = 120;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();
}
