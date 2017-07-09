#include "ppdgem.h"
#include "ppdg0.h"

WORD prop_put(pprogram, psection, pbuffer, options)
BYTE FAR *pbuffer;
BYTE FAR *psection;
BYTE FAR *pprogram;
WORD options;
{
    PROP_PROG  = pprogram;
    PROP_SECT  = psection;
    PROP_BUF   = pbuffer;
    PROP_OPT   = options;

    return( gem_if( PROP_PUT ) );
}

