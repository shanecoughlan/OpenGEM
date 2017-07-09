

#include "ppdgem.h"
#include "ppdv0.h"


    WORD
vm_pagesize( handle, pgwidth, pgheight )
WORD handle, pgwidth, pgheight;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 3;
    contrl[5] = 99;
    contrl[6] = handle;
    intin[0] = 0;
    intin[1] = pgwidth;
    intin[2] = pgheight;
    vdi();
}
