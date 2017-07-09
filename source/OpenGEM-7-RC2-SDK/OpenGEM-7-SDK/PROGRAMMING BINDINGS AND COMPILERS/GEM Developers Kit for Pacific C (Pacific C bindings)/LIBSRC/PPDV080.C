

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vsm_valuator( handle, val_in, val_out, term, status )
WORD    handle, val_in, *val_out, *term, *status;
{
    intin[0] = val_in;

    contrl[0] = 29;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();

    *val_out = intout[0];
    *term = intout[1];
    *status = contrl[4];
}
