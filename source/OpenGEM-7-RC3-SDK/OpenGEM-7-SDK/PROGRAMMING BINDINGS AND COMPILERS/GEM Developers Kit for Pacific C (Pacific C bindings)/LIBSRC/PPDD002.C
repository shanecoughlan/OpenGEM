

#include "ppdgem.h"
#include "ppdd0.h"



	VOID
dos_lpvoid(ax, fp)
	UWORD		ax;
	LPVOID		fp;
{
	DOS_AX = ax;
	DOS_DX = FP_OFF(fp);	/* low  DS:DX   */
	DOS_DS = FP_SEG(fp);	/* high DS:DX   */

	__DOS();
}
