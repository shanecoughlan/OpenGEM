#include "ppdgem.h"
#include "ppdg0.h"

WORD prop_get(pprogram, psection, pbuffer, len, options)
BYTE FAR *pbuffer;
BYTE FAR *psection;
BYTE FAR *pprogram;
WORD len;
WORD options;
{
    PROP_PROG  = pprogram;
    PROP_SECT  = psection;
    PROP_BUF   = pbuffer;
    PROP_BUFL  = len;
    PROP_OPT   = options;

    return( gem_if( PROP_GET ) );
}

