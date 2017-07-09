
#include "aes.h"

#if DEBUG

int	TRACEFLAG;

#asm
;
; Debugging code
;
		.psect	_TEXT,class=CODE
		.globl	_TRACE,_ophex,_rlr,_TRKEY,_TRCHAR,_border

_TRKEY:	mov		ah,#1
		int		#0x21
		cmp		al,#'!'
		jnz		trkr
		int		#3
trkr:	ret

_TRACE:	push	dx		;String parameter
		mov		dx,_rlr
		call	_ophex
		mov		ah,#2
		mov		dl,#':'
		int		#0x21
		pop		dx
		mov		ah,#9
		int		#0x21
;;		call	_TRKEY
		ret
;
_TRCHAR:
		mov		ah,#2
		int		#0x21
		ret
;
_ophex:	mov		al,dh
		call	hexa
		mov		al,dl
hexa:	push	ax
		mov		cl,#4
		rcr		al,cl
		call	hexn
		pop		ax
hexn:	and		al,#0x0F
		cmp		al,#10
		jc		hexn1
		add		al,#7
hexn1:	add		al,#48
		push	dx
		mov		dl,al
		mov		ah,#2
		int		#0x21
		pop		dx
		ret
;
_border:			;Set border & background colour
	push	ax
	push	bx
	push	di
	mov		bx,dx
	mov		ah,#0x0B
	int		#0x10
	pop		di
	pop		bx
	pop		ax
	ret
;
#endasm

#endif /* DEBUG */u
