

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vsm_choice( handle, choice )
WORD    handle, *choice;
{
    WORD t;
    contrl[0] = 30;
    contrl[1] = contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    *choice = intout[0];
    t = contrl[4];	/*this is a fix for bug in Lattice v3.2, it will*/
			/*give warning 85, if return(contrl[4]) directly*/
    return( t );
}
