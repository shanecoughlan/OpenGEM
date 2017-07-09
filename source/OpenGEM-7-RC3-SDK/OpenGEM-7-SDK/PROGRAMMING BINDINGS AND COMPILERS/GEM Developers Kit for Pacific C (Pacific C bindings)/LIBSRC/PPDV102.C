

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vr_trnfm( handle, srcMFDB, desMFDB )
WORD handle;
LPMFDB srcMFDB, desMFDB;
{
    i_ptr( srcMFDB );
    i_ptr2( desMFDB );	

    contrl[0] = 110;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();
}
