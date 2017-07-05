

#include "ppdgem.h"
#include "ppdg0.h"


WORD graf_shrinkbox(WORD x1, WORD y1, WORD w1, WORD h1,
                    WORD x2, WORD y2, WORD w2, WORD h2)
{
	GR_I1 = x1;
	GR_I2 = y1;
	GR_I3 = w1;
	GR_I4 = h1;
	GR_I5 = x2;
	GR_I6 = y2;
	GR_I7 = w2;
	GR_I8 = h2;
	return ( gem_if ( GRAF_SHRINKBOX ) );		
}
