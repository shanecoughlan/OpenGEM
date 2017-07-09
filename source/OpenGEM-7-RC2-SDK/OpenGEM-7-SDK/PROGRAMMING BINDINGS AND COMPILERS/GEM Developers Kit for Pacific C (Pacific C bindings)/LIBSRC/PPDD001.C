

#include "ppdgem.h"
#include "ppdd0.h"



	VOID
dos_func(ax, lodsdx, hidsdx)  
	UWORD		ax;
	UWORD		lodsdx;
	UWORD		hidsdx;
{
	DOS_AX = ax;
	DOS_DX = lodsdx;	/* low  DS:DX   */
	DOS_DS = hidsdx;	/* high DS:DX   */

	__DOS();
}
