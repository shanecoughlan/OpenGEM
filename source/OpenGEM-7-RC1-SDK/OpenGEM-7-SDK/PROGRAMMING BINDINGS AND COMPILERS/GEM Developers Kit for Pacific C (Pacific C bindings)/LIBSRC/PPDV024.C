

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_output_window( handle, xy )
WORD handle, xy[];
{
    i_ptsin( xy );

    contrl[0] = 5;
    contrl[1] = 2;
    contrl[3] = 0;
    contrl[5] = 21;
    contrl[6] = handle;
    vdi();

    i_ptsin( ptsin );
}
