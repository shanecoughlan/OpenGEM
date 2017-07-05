

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vq_extnd( handle, owflag, work_out )
WORD handle, owflag, work_out[];
{
    i_intout( work_out );
    i_ptsout( work_out + 45 );
    intin[0] = owflag;

    contrl[0] = 102;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();

    i_intout( intout );
    i_ptsout( ptsout );
}
