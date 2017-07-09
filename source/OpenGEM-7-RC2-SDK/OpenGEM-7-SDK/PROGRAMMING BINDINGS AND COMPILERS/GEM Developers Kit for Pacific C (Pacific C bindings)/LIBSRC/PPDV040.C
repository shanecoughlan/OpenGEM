

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vt_resolution ( handle, xres, yres, xset, yset )
WORD    handle, xres, yres, *xset, *yset;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 2;
    contrl[5] = 81;
    contrl[6] = handle;
    intin[0] = xres;
    intin[1] = yres;
    vdi();
    *xset = intout[0];
    *yset = intout[1];
}
