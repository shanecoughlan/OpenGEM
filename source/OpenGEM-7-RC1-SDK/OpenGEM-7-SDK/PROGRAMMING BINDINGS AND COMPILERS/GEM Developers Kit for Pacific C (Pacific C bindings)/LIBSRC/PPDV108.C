

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vqt_extent( handle, string, extent )
WORD handle, extent[];
BYTE string[];
{
    WORD *intstr;

    intstr = intin;
    while (*intstr++ = *string++)
        ;
    i_ptsout( extent );

    contrl[0] = 116;
    contrl[1] = 0;
    contrl[3] = intstr - intin - 1;
    contrl[6] = handle;
    vdi();

    i_ptsout( ptsout );
}
