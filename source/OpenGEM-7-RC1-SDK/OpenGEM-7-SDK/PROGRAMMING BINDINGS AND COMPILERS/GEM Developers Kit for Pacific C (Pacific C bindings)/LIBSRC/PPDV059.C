

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_justified( handle, x, y, string, length, word_space, char_space)
WORD handle, x, y, length, word_space, char_space;
BYTE string[];
{
    WORD *intstr;

    ptsin[0] = x;
    ptsin[1] = y;
    ptsin[2] = length;
    ptsin[3] = 0;
    intin[0] = word_space;
    intin[1] = char_space;
    intstr = &intin[2];
    while (*intstr++ = *string++)
        ;

    contrl[0] = 11;
    contrl[1] = 2;
    contrl[3] = intstr - intin - 1;
    contrl[5] = 10;
    contrl[6] = handle;
    vdi();
}
