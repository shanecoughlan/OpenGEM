
#include <dos.h>
#include "ppddbind.h"

extern union  REGS  DR;
extern struct SREGS SR;

#define DOS_AX DR.x.ax
#define DOS_BX DR.x.bx
#define DOS_CX DR.x.cx
#define DOS_DX DR.x.dx
#define DOS_SI DR.x.si
#define DOS_DI DR.x.di

#define DOS_DS SR.ds
#define DOS_ES SR.es

extern UWORD	DOS_ERR;

extern WORD __DOS();
extern VOID dos_func(UWORD ax, UWORD lodsdx, UWORD hidsdx);
extern VOID dos_lpvoid(UWORD ax, LPVOID fp);
