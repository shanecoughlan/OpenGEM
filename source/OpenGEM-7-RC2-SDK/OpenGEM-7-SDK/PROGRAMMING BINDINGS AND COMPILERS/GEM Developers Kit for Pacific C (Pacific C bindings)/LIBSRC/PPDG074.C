

#include "ppdgem.h"
#include "ppdg0.h"



/*   JCE 8 Feb 1998: With GROWBOX.ACC, graf_growbox() and graf_shrinkbox()
 *   will now work. They may well also become part of the AES again in
 *   OpenGEM.
 */

WORD graf_growbox(WORD x1, WORD y1, WORD w1, WORD h1,
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
	return ( gem_if ( GRAF_GROWBOX ) );
		
}
