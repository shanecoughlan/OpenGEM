

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vex_motv( handle, usercode, savecode )
WORD handle;
LPVOID usercode, *savecode;
{
    i_lptr1( usercode );

    contrl[0] = 126;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    m_lptr2( savecode );
}
