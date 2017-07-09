#include "ppdgem.h"
#include "ppdg0.h"

WORD prop_del(pprogram, psection, options)
BYTE FAR *psection;
BYTE FAR *pprogram;
WORD options;
{
    PROP_PROG  = pprogram;
    PROP_SECT  = psection;
    PROP_OPT   = options;

    return( gem_if( PROP_DEL ) );
}

