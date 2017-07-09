

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vqt_justified( handle, x, y, string, length, word_space, char_space, offsets)
WORD handle, x, y, length, word_space, char_space, offsets[];
BYTE string[];
{
    WORD *intstr,t;

    ptsin[0] = x;
    ptsin[1] = y;
    ptsin[2] = length;
    ptsin[3] = 0;
    intin[0] = word_space;
    intin[1] = char_space;
    intstr = &intin[2];
    while (*intstr++ = *string++)
        ;

    contrl[0] = 132;
    contrl[1] = 2;
    contrl[3] = intstr - intin - 1;
    contrl[6] = handle;
    i_ptsout( offsets );
    vdi();
    i_ptsout( ptsout );
    t = contrl[2];	/*this is a fix for bug in Lattice v3.2, it will*/
			/*give warning 85, if return(contrl[4]) directly*/
    return( t );
}
