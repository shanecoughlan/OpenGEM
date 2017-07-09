

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vst_font( handle, font )
WORD handle, font;
{
    intin[0] = font;

    contrl[0] = 21;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
