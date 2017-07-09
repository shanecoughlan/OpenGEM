

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vex_timv( handle, tim_addr, old_addr, scale )
WORD handle, *scale;
LPVOID tim_addr, *old_addr;
{
    i_lptr1( tim_addr );

    contrl[0] = 118;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    m_lptr2( old_addr );
    *scale = intout[0];
}
