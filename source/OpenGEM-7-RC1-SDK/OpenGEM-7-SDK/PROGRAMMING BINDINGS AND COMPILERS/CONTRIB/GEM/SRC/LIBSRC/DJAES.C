
/*
 * JCE 26-9-1998 This replaces GEMASM.ASM
 * 
 */

#include "djgppgem.h"

#define GEMENTRY 0xEF
#define GEM_VECTOR (4 * GEMENTRY)
 

static AESFUNC pAes = NULL;
static int allocated = 0;

GLOBAL LPWORD pControl, pGlobal, pInt_in, pInt_out, pAddr_in, pAddr_out;
GLOBAL LPLPTR pParam = 0; 

extern WORD vdi_alloc_arrays(VOID);

WORD lgl_apid;

WORD aes_alloc_arrays(VOID)
{
	LPBYTE 	pData;

	if (allocated) return 1;
        if (!vdi_alloc_arrays()) return 0;

//	pData = dos_alloc(138);	
	pData = dos_alloc(144);	
	if (!pData) return 0;

	pControl = (LPWORD)pData;
	pGlobal  = (LPWORD)(pData + 10);
        pInt_in  = (LPWORD)(pData + 40);
        pInt_out = (LPWORD)(pData + 72);
        pAddr_in = (LPWORD)(pData + 86);
        pAddr_out= (LPWORD)(pData + 98);
	pParam   = (LPLPTR)(pData +118);

	/* Create the parameter block */

	LSSET(pParam,     pControl);
	LSSET(pParam + 4, pGlobal);
	LSSET(pParam + 8, pInt_in);
	LSSET(pParam +12, pInt_out);
	LSSET(pParam +16, pAddr_in);
	LSSET(pParam +20, pAddr_out);
	allocated = 1;
	return 1;
}


AESFUNC divert_aes(AESFUNC f)
{	
	AESFUNC p1 = pAes;

	if (f == (AESFUNC)aes) f = NULL;

	pAes = f;
	return p1;
}



WORD aes(VOID)
{
	__dpmi_regs rs;

	if (!allocated) return 0;	/* No arrays set up */

	if (pAes) return (*pAes)(pParam);
	
	rs.x.es = LP_SEG(pParam);
	rs.x.bx = LP_OFF(pParam);
	rs.x.cx = 200;
	rs.x.dx = 0;

	LWSET(pParam + 24, lgl_apid);
	__dpmi_int(GEMENTRY, &rs);

	return rs.x.ax;
}


WORD aescheck(VOID)
{
	LPBYTE pSig = LSGET(GEM_VECTOR);
	
	if (LBGET(pSig + 2) == 'G' &&
            LBGET(pSig + 3) == 'E' &&
            LBGET(pSig + 4) == 'M' &&
            LBGET(pSig + 5) == 'A' &&
            LBGET(pSig + 6) == 'E' &&
            LBGET(pSig + 7) == 'S')    return 1;
	
	return 0;
}

