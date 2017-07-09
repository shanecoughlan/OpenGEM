

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vq_tabstatus( handle )
WORD handle;
{
    contrl[0] = 5;
    contrl[1] = contrl[3] = 0;
    contrl[5] = 16;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
