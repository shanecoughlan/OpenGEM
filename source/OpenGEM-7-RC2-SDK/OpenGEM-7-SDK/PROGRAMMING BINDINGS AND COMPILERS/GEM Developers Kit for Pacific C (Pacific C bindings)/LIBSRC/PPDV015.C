

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_curtext( handle, string )
WORD handle;
BYTE *string; 
{
    WORD *intstr;

    intstr = intin;
    while (*intstr++ = *string++)
      ;

    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = intstr - intin - 1;
    contrl[5] = 12;
    contrl[6] = handle;
    vdi();
}
