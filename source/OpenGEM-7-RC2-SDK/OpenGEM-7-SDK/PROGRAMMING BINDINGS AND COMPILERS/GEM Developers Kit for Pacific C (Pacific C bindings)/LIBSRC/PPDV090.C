

#include "ppdgem.h"
#include "ppdv0.h"



    BOOLEAN 
vqt_attributes( handle, attributes )
WORD handle, attributes[];
{
    i_intout( attributes );
    i_ptsout( attributes + 6 );

    contrl[0] = 38;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    i_intout( intout );
    i_ptsout( ptsout );
    if (contrl[4]) return 1;
    return 0;
}
