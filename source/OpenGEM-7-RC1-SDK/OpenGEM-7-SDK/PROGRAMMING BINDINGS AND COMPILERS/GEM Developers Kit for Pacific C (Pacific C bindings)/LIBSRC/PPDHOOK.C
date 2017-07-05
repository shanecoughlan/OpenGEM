/*
 * This contains a number of functions to help a program (normally a DA) to
 * intercept calls made to GEM.
 *
 */

#include "ppdgem.h"
#include <intrpt.h>
#include <dos.h>

#define GEM_VECTOR ((isr *)MK_FP(0, 4 * 0xEF))


extern LPGEMBLK _aespar;
extern LPGSXPAR _vdipar;
extern WORD     _funcpar;
extern WORD     _gemret;
extern isr      _hookaddr;
extern isr		_old_ef;
extern BYTE 	_reentry;
extern WORD		_chain;
	
static AESFUNC AES_C8  = NULL;
static AESFUNC AES_C9  = NULL;
static VDIFUNC VDI_473 = NULL;
static AESFUNC oldaes  = NULL;
static VDIFUNC oldvdi  = NULL;	


static WORD _gem_old(void)
{
#asm
	push	ds
	push	es

;	int		#3
	mov		ax,__axpar
	push	ax
	mov		bx,__aespar
	mov		cx,__funcpar
	mov		dx,__vdipar
	
	mov		ax,__aespar + 2
	mov		es,ax
	mov		ax,__vdipar + 2
	mov		ds,ax
	pop		ax		;AX parameter
	pushf
	callf	goto_ef

	pop		es
	pop		ds
#endasm
}



static WORD newvdi(LPGSXPAR gb)
{
	_vdipar  = gb;
	_funcpar = 0x473;
	return _gem_old();
}

static WORD newaes(LPGEMBLK gb)
{
	_aespar  = gb;
	_funcpar = 0xC8;
	return _gem_old();
}

	
void interrupt _gem_hook(void)
{	
	_reentry++;
	_chain  = 1;
	_gemret = 0;
	
	switch(_funcpar)
	{
		case 0xC8:  if (AES_C8)  _chain  = (*AES_C8)(_aespar);
		 		    else		 _chain  = 1;
				    break;
		case 0xC9:  if (AES_C8)  _chain  = (*AES_C9)(_aespar);
		 		    else		 _chain  = 1;
				    break;
		case 0x473: if (VDI_473) _chain  = (*VDI_473)(_vdipar);
		 		    else	     _chain  = 1;
				    break;
		default:	break;
	}
	_reentry--;
}	


VOID ppd_setresult(WORD r)
{
	_gemret = r;
}



VOID ppd_hookon(AESFUNC pAes, AESFUNC pAes2, VDIFUNC pVdi)
{
	AES_C8  = pAes;
	AES_C9  = pAes2;
	VDI_473 = pVdi;

	if (!_old_ef)
	{
		_old_ef = set_vector(GEM_VECTOR, _hookaddr);
	}
	oldaes = divert_aes(newaes);
	oldvdi = divert_vdi(newvdi);
}


VOID ppd_hookoff(VOID)
{
	if (_old_ef) set_vector(GEM_VECTOR, _old_ef);
	_old_ef  = NULL;
	AES_C8  = NULL;
	AES_C9  = NULL;
	VDI_473 = NULL;
	divert_aes(oldaes);
	divert_vdi(oldvdi);
	oldaes = NULL;
	oldvdi = NULL;
}



#asm

#ifdef LARGE_MODEL
	.globl	large_data
	.globl	large_code
	.psect	ltext,local,class=CODE,reloc=16,size=65535
#else
	.globl	small_data
	.globl	small_code
	.psect	_TEXT,class=CODE
#endif

	.globl	__gem_call
	.globl  __gem_hook
	.globl	__gem_old
	
;
;This is our handler for INT 0xEF.
;	
__gem_call:
	jmp		gcall1
	.byte	'G','E','M','A','E','S','2','0',0

gcall1:
	push	ds				;Set DS to our data, so that we can write to
	push	ax				;local variables like _aespar. 
	mov	ax,#seg __funcpar
	mov		ds,ax
;	int		#3
	mov		al,__reentry
	or		al,al			;Re-entrant call?
	jnz		gopast
	
	pop		ax				;AX = caller AX
	mov		__axpar,ax
	pop		ax				;AX = caller DS
	push	ax

	mov	__funcpar,    cx	;0x473 for VDI, 0xC8 or 0xC9 for AES
	mov	__vdipar,     dx
	mov	__vdipar + 2, ax	;VDI parameter, far pointer in ds:dx
	mov	__aespar,     bx
	mov	ax,es	
	mov	__aespar + 2, ax	;AES parameter, far pointer in es:bx

	push	bp
	pushf			 		;gem_hook will return with an iret, so simulate 
	callf	__gem_hook 		;an interrupt call.
	pop		bp
	
	mov		ax,__chain
	or		ax,ax
	jnz		dochain
	
	mov		ax,__gemret
	pop	ds
	iret
;
dochain:
	mov		ax,__axpar
	push	ax
	mov		bx,__aespar
	mov		cx,__funcpar
	mov		dx,__vdipar
	
	mov		ax,__aespar + 2
	mov		es,ax
	mov		ax,__vdipar + 2
	mov		ds,ax

gopast:
	pop		ax
	pop		ds
	jmpf	goto_ef
	
	.psect	data,class=DATA
	.globl	__hookaddr
	.globl	__aespar
	.globl	__vdipar
	.globl	__funcpar
	.globl	__gemret
	.align	2

__hookaddr:
	.word	__gem_call,seg (__gem_call)
__aespar:
	.word	0,0
__vdipar:
	.word	0,0
__funcpar:
	.word	0
__gemret:
	.word	0
__axpar:	
	.word	0
__reentry:
	.byte	0		;Re-entrant call?

goto_ef:
	.byte	0xEA	;JMP FAR
__old_ef:
	.word	0,0
__chain:
	.word	0
	
#endasm

/* */
