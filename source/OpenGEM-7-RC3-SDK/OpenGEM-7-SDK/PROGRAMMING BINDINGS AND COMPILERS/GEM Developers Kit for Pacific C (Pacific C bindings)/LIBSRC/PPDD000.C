/*	DOSBIND.C	2/15/85			Lee Lorenzen		*/

#include "ppdgem.h"
#include "dosbind.h"  				/* file i/o defines 	*/ 
#include <dos.h>

union  REGS  DR;
struct SREGS SR;

#define DOS_AX DR.x.ax
#define DOS_BX DR.x.bx
#define DOS_CX DR.x.cx
#define DOS_DX DR.x.dx
#define DOS_SI DR.x.si
#define DOS_DI DR.x.di

#define DOS_DS SR.ds
#define DOS_ES SR.es

GLOBAL UWORD	DOS_ERR;

WORD __DOS()
{
	intdosx(&DR, &DR, &SR);

	DOS_ERR = DR.x.cflag;
}
