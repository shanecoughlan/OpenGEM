
/*
 * JCE 13-4-1998 This replaces VDIASM.ASM
 * 
 */

#include "djgppgem.h"

#define GEMENTRY 0xEF
#define GEM_VECTOR (4 * GEMENTRY)
 

static VDIFUNC pVdi = NULL;
static int allocated = 0;

GLOBAL LPWORD pContrl, pIntin, pPtsin, pIntout, pPtsout;
GLOBAL LPLPTR pPblock = 0; 
GLOBAL LPBYTE pGeneral;		/* General transfer buffer */
extern WORD lgl_apid;


WORD vdi_alloc_arrays(VOID)
{
	LPBYTE 	pData;

	if (allocated) return 1;

	pData = dos_alloc(830 + 4096);	
	if (!pData) return 0;

	pContrl = (LPWORD)pData;
	pIntin  = (LPWORD)(pData + 24);		/* NOTE: This relies on     */
	pPtsin  = (LPWORD)(pData + 184);	/* sizeof(*pData) == 1      */
	pIntout = (LPWORD)(pData + 696);	/* Since these LP* types    */
	pPtsout = (LPWORD)(pData + 786);	/* are not true pointers,   */
	pPblock = (LPLPTR)(pData + 810);	/* this is always true. But */
                  /* just to be safe, pData is an LPBYTE and not an LPWORD. */

	pGeneral = pData + 830;

	/* Create the parameter block */

	LSSET(pPblock,     pContrl);
	LSSET(pPblock + 4, pIntin);
	LSSET(pPblock + 8, pPtsin);
	LSSET(pPblock +12, pIntout);
	LSSET(pPblock +16, pPtsout);

	allocated = 1;
	return 1;
}


VDIFUNC divert_vdi(VDIFUNC f)
{	
	VDIFUNC p1 = pVdi;

	if (f == (VDIFUNC)vdi) f = NULL;

	pVdi = f;
	return p1;
}



WORD vdi(VOID)
{
	__dpmi_regs rs;

	if (!allocated) return 0;	/* No arrays set up */

	if (pVdi) return (*pVdi)(pPblock);
	
	rs.x.ds = LP_SEG(pPblock);
	rs.x.dx = LP_OFF(pPblock);
	rs.x.cx = 0x0473;

	__dpmi_int(GEMENTRY, &rs);
	return rs.x.ax;
}


WORD gemcheck(VOID)
{
	LPBYTE pSig = LSGET(GEM_VECTOR);
	
	if (LBGET(pSig + 2) == 'G' &&
            LBGET(pSig + 3) == 'E' &&
            LBGET(pSig + 4) == 'M')    return 1;
	
	return 0;
}

