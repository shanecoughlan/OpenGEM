

#include "ppdgem.h"
#include "ppdv0.h"

/* The following functions are GEM/3 only*/
	VOID
v_copies(handle, count)
WORD handle, count;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[5] = 28;
    contrl[6] = handle;
    intin[0] = count;
    vdi();
}
