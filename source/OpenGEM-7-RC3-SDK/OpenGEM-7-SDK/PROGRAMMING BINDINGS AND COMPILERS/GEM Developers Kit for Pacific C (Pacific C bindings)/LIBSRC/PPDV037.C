

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vm_filename( handle, filename )
WORD handle;
BYTE *filename;
{
    WORD *intstr;

    intstr = intin;
    while( *intstr++ = *filename++ )
        ;

    contrl[0] = 5;
    contrl[1] = 0;
    contrl[5] = 100;
    contrl[6] = handle;
    contrl[3] = intstr - intin - 1;
    vdi();
}
