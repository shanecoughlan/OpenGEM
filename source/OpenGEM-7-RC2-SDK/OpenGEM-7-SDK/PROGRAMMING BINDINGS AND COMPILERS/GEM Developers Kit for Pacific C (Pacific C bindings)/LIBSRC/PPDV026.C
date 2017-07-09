

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_bit_image( handle, filename, aspect, x_scale, y_scale, h_align, v_align, xy )
WORD handle, aspect, x_scale, y_scale, h_align, v_align;
WORD xy[];
BYTE *filename;
{
    WORD i;

    for (i = 0; i < 4; i++)
	ptsin[i] = xy[i];
    intin[0] = aspect;
    intin[1] = x_scale;
    intin[2] = y_scale;
    intin[3] = h_align;
    intin[4] = v_align;
    i = 5;
    while (intin[i++] = *filename++)
        ;

    contrl[0] = 5;
    contrl[1] = 2;
    contrl[3] = --i;
    contrl[5] = 23;
    contrl[6] = handle;
    vdi();
}
