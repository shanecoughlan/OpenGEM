

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vqm_attributes( handle, attributes )
WORD handle, attributes[];
{
    i_intout( attributes );

    contrl[0] = 36;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    i_intout( intout );
    attributes[3] = ptsout[1];
}
