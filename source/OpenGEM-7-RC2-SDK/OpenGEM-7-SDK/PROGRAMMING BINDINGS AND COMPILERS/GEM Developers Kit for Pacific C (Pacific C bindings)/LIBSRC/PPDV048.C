

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_fillarea( handle, count, xy)
WORD handle, count, xy[];
{
    i_ptsin( xy );

    contrl[0] = 9;
    contrl[1] = count;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    i_ptsin( ptsin );
}
