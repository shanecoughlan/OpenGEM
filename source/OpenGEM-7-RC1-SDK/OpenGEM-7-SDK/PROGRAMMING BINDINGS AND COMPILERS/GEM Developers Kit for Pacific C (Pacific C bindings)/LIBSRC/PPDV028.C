

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_alpha_text( handle, string )
WORD handle;
BYTE *string;
{
    WORD i;

    i = 0;
    while (intin[i++] = *string++)
        ;

    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = --i;
    contrl[5] = 25;
    contrl[6] = handle;
    vdi();
}
