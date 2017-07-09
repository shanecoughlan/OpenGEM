

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vst_ex_load_fonts( handle, select, font_max, font_free )
WORD handle, select, font_max, font_free;
{
    contrl[0] = 119;
    contrl[1] = 0;
    contrl[3] = 3;
    contrl[6] = handle;
    intin[0] = select;
    intin[1] = font_max;
    intin[2] = font_free;
    vdi();
    return( intout[0] );
}
