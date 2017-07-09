#include "ppdgem.h"
#include <dos.h>

WORD LSTRLEN (LPBYTE addr)
{
	REG WORD n = 0;	/* [JCE 20-10-1999] Give me a brown paper bag to wear */
			/* for missing the "=0" */
	while (*addr)
	{
		++addr;
		++n;
	}
	return n;
}

MLOCAL BYTE b;


LPVOID LLDS(VOID)
{
	return MK_FP(FP_SEG(&b), 0);
}

LPVOID LLCS(VOID)
{
	return MK_FP(FP_SEG(LLCS), 0);
}

LPVOID LLSS(VOID)
{
	BYTE lv;
	return MK_FP(FP_SEG(&lv), 0);
}


WORD LWCOPY(LPWORD pdst, LPWORD psrc, WORD numwds)
{
	WORD n;
	
	if (FP_SEG(pdst) == FP_SEG(psrc) && FP_OFF(pdst) > FP_OFF(psrc))
	{
		for (n = numwds - 1; n >= 0; n--)
		{
			pdst[n] = psrc[n];
		}	

	}
	else for (n = 0; n < numwds; n++)
	{
		pdst[n] = psrc[n];
	}
	
	return 0;
}


BYTE LBCOPY(LPBYTE pdst, LPBYTE psrc, WORD numbytes)
{
	WORD n;
	
	if (FP_SEG(pdst) == FP_SEG(psrc) && FP_OFF(pdst) > FP_OFF(psrc))
	{
		for (n = numbytes - 1; n >= 0; n--)
		{
			pdst[n] = psrc[n];
		}	

	}
	else for (n = 0; n < numbytes; n++)
	{
		pdst[n] = psrc[n];
	}
	
	return 0;
}

WORD LBWMOV(LPWORD pdst, LPBYTE psrc)
{
	register WORD n;

	while (*psrc)
	{
		*(pdst++) = *(psrc++);
		++n;
	}
	return n;
}


