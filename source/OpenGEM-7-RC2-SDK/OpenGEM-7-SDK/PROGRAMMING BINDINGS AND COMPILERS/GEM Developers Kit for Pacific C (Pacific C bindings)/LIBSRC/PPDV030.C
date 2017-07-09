

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_sound( handle, frequency, duration )
WORD handle;
WORD frequency;
WORD duration;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 2;
    contrl[5] = 61;
    contrl[6] = handle;

    intin[0] = frequency;
    intin[1] = duration;
    vdi();
}
