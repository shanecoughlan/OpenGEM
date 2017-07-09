

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vsl_udsty( handle, pattern )
WORD handle, pattern;
{
    intin[0] = pattern;

    contrl[0] = 113;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
}
