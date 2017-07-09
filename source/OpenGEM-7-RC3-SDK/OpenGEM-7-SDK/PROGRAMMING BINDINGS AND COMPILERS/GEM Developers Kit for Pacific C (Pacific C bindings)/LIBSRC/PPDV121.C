

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vs_clip( handle, clip_flag, xy )
WORD handle, clip_flag, xy[];
{
    i_ptsin( xy );
    intin[0] = clip_flag;

    contrl[0] = 129;
    contrl[1] = 2;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();

    i_ptsin( ptsin );
}
