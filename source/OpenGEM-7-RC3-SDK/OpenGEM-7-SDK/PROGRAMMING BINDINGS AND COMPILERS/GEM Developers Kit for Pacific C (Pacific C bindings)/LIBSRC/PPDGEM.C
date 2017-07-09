/*
 * JCE 1-3-1998 This replaces GEMASM.ASM
 * 
 */


#include <dos.h>
#include "ppdgem.h"

#define GEMENTRY 0xEF
#define GEM_VECTOR 4 * GEMENTRY


static AESFUNC pAes = NULL;

 
AESFUNC divert_aes(AESFUNC f)
{
	AESFUNC p1 = pAes;
	
	if (f == gem) f = NULL;
	pAes = f;
	return p1;
}
 
 
WORD gem(LPGEMBLK gb)
{
	union  REGS ir, or;
	struct SREGS sr;

	if (pAes) return (*pAes)(gb);

	ir.x.cx = 200;
	ir.x.dx = 0;
	ir.x.bx = FP_OFF(gb);
	sr.es   = FP_SEG(gb);
	
	int86x(GEMENTRY, &ir, &or, &sr);

	return or.x.ax;
}

MLOCAL BYTE SIG[] = "GEMAES";

WORD aescheck(VOID)
{
	LPWORD pGem = MK_FP(0, GEM_VECTOR);
	LPBYTE pSig = MK_FP(pGem[0], pGem[1]);
	WORD n;

	for (n = 0; n < 6; n++)
	{
		if (pSig[2 + n] != SIG[n]) return 0;
	}
	return 1;
}

