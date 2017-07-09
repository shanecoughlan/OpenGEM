

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_write_meta( handle, num_ints, ints, num_pts, pts )
WORD handle, num_ints, ints[], num_pts, pts[];
{
    i_intin( ints );
    i_ptsin( pts );

    contrl[0] = 5;
    contrl[1] = num_pts;
    contrl[3] = num_ints;
    contrl[5] = 99;
    contrl[6] = handle;
    vdi();

    i_intin( intin );
    i_ptsin( ptsin );
}
