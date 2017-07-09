

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
v_clear_disp_list( handle )
WORD handle;
{
    contrl[0] = 5;
    contrl[1] = contrl[3] = 0;
    contrl[5] = 22;
    contrl[6] = handle;
    vdi();
}
