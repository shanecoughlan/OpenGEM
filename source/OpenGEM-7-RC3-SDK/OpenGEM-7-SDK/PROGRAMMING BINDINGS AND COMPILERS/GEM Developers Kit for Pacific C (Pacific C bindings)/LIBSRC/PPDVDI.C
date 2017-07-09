
/*
 * JCE 1-3-1998 This replaces VDIASM.ASM
 * 
 */


#include <dos.h>
#include "ppdgem.h"

#define GEMENTRY 0xEF
#define GEM_VECTOR (4 * GEMENTRY)
 
extern GSXPAR pblock;


static VDIFUNC pVdi = NULL;


VDIFUNC divert_vdi(VDIFUNC f)
{	
	VDIFUNC p1 = pVdi;

	if (f == (VDIFUNC)vdi) f = NULL;

	pVdi = f;
	return p1;
}



WORD vdi(VOID)
{
	union REGS ir, or;
	struct SREGS sr;
	register far void *ppblock = &pblock;

	if (pVdi) return (*pVdi)(ppblock);
	
	sr.ds   = FP_SEG(ppblock);
	ir.x.dx = FP_OFF(ppblock);
	ir.x.cx = 0x0473;
	int86x(GEMENTRY,&ir,&or,&sr);
	return or.x.ax;
}


WORD gemcheck(VOID)
{
	LPWORD pGem = MK_FP(0, GEM_VECTOR);
	LPBYTE pSig = MK_FP(pGem[0], pGem[1]);

	if (pSig[2] == 'G' && pSig[3] == 'E' && pSig[4] == 'M') return 1; 
	
	return 0;
}

#ifndef USER_INTIN

GLOBAL WORD	contrl[11];		/* control inputs		*/
GLOBAL WORD	intin[80];		/* max string length		*/
GLOBAL WORD	ptsin[256];		/* polygon fill points		*/
GLOBAL WORD	intout[45];		/* open workstation output	*/
GLOBAL WORD	ptsout[12];

#endif
