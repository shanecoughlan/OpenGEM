

#include "ppdgem.h"
#include "ppdv0.h"

    
    WORD
vsf_interior( handle, style )
WORD handle, style;
{
    intin[0] = style;

    contrl[0] = 23;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
