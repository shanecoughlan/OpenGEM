

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vrq_locator( handle, initx, inity, xout, yout, term )
WORD handle, initx, inity, *xout, *yout, *term;
{
    ptsin[0] = initx;
    ptsin[1] = inity;

    contrl[0] = 28;
    contrl[1] = 1;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    *xout = ptsout[0];
    *yout = ptsout[1];
    *term = intout[0];
}
