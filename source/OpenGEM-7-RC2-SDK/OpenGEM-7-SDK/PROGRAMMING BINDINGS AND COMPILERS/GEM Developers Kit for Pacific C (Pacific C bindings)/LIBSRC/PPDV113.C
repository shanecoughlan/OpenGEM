

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vrt_cpyfm( handle, wr_mode, xy, srcMFDB, desMFDB, index )
WORD handle, wr_mode;
LPMFDB srcMFDB, desMFDB;
WORD xy[], *index;
{
    intin[0] = wr_mode;
    intin[1] = *index++;
    intin[2] = *index;		
    i_ptr( srcMFDB );
    i_ptr2( desMFDB );
    i_ptsin( xy );

    contrl[0] = 121;
    contrl[1] = 4;
    contrl[3] = 3;
    contrl[6] = handle;
    vdi();

    i_ptsin( ptsin );
}
