

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vsc_form( handle, cur_form )
WORD handle, *cur_form;
{
    i_intin( cur_form );

    contrl[0] = 111;
    contrl[1] = 0;
    contrl[3] = 37;
    contrl[6] = handle;
    vdi();

    i_intin( intin );
}
