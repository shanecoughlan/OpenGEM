;/*	GSX2.A86	6/26/84 - 10/16/84	Don Heiskell	*/
;
;
;       Copyright 1999, Caldera Thin Clients, Inc.                      
;       This software is licenced under the GNU Public License.         
;       Please see LICENSE.TXT for further information.                 
;                                                                       
;                  Historical Copyright                                 
;/*
;*	-------------------------------------------------------------
;*	GEM Application Environment Services		  Version 2.3
;*	Serial No.  XXXX-0000-654321		  All Rights Reserved
;*	Copyright (C) 1986			Digital Research Inc.
;*	-------------------------------------------------------------
;*/
;

; Pacific C register calling conventions 
; (note: ONLY for correctly prototyped functions)
;
; WORDs returned in AX
; LONGs returned in DXAX (or DX:AX if long address)
;
; If the first parameter fits in 16 bits, it is passed in DX.
; Similarly if the second fits in 16 bits, it is passed in AX.
;
; BP, SI and DI must be preserved.
;



;
;
GSXENTRY equ	0efh
;
	.globl	_gsx2
	.globl	_umul_div
	.globl	_mul_div
	.globl	_i_ptsin
	.globl	_i_intin
	.globl	_i_intout
	.globl	_i_ptsout
	.globl	_i_ptr
	.globl	_i_ptr2
	.globl	_i_lptr1
	.globl	_i_lptr1l
	.globl	_m_lptr2

	.psect	_TEXT,class=CODE

_gsx2:
	mov		cx,#473h
	mov		dx,#pblock		;point at the parm block
	int		#GSXENTRY
	ret
;
;***************************************************************************
;
; MUL_DIV (m1,m2,d1)
;
;	 ( ( ( m1 * 2 * m2 ) / d1 ) + 1 ) / 2	
;	m1 = signed 16 bit integer
;	m2 = unsigned 15 bit integer
;	d1 = signed 16 bit integer
;
;***************************************************************************
_mul_div:
	push	bp		;save the callers bp
	mov		bp,sp
	mov		bx,dx	; DX = Parameter 1
					; AX = Parameter 2
	shl		ax,#1	; m2 * 2
	imul	bx		; m2 * 2 * m1
	mov		bx,04[bp]
	idiv	bx		; m2 * 2 * m1 / d1
	and		ax,ax		; set flags
	js		mul_div_neg
	inc		ax
	shr		ax,#1		; m2 * 2 * m1 / d1 +1 / 2
	pop		bp
	ret		#2

mul_div_neg:
	add		ax,#-1
	neg		ax
	shr		ax,#1
	neg		ax		; m2 * 2 * m1 / d1 -1 / 2
	pop		bp
	ret		#2
;
;***************************************************************************
;
; UMUL_DIV (m1,m2,d1)
;
;	 ( ( ( m1 * 2 * m2 ) / d1 ) + 1 ) / 2	
;	m1 = usigned 16 bit integer
;	m2 = unsigned 15 bit integer
;	d1 = usigned 16 bit integer
;
;***************************************************************************
_umul_div:
	push	bp		;save the callers bp
	mov		bp,sp
	mov		bx,dx	;DX = Parameter 1
					;AX = Parameter 2
	shl		ax,#1		; m2 * 2
	mul		bx		; m2 * 2 * m1
	mov		bx,04[bp]
	div		bx		; m2 * 2 * m1 / d1
	inc		ax
	shr		ax,#1		; m2 * 2 * m1 / d1 +1 / 2
	pop		bp
	ret		#2
;
; Near pointers, all stored in DX.
;
_i_ptsin:
	mov		ptsioff,dx
	ret	


_i_intin:
	mov		intioff,dx
	ret

_i_ptsout:
	mov		ptsooff,dx
	ret


_i_intout:
	mov		intooff,dx
	ret


_i_ptr:
	mov		_contrl+14,dx
	mov		ax,ds
	mov		_contrl+16,ax
	ret


_i_ptr2:
	mov		_contrl+18,dx
	mov		ax,ds
	mov		_contrl+20,ax
	ret

_i_lptr1l:				;This takes a long pointer on the stack
	push	bp
	mov		bp,sp
	mov		dx,04[bp]	;Offset
	mov		ax,06[bp]	;Segment
	call	_i_lptr1
	pop		bp
	ret		#4

_i_lptr1:
	mov		_contrl+14,dx	;(off, seg) as parameters in DX and AX
	and		ax,ax
	jnz		i_lptr11
	mov		ax,cs
i_lptr11:
	mov		_contrl+16,ax
	ret


_m_lptr2:
	mov		bx,dx
	mov		ax,_contrl+18
	mov		[bx],ax
	inc		bx
	inc		bx
	mov		ax,_contrl+20
	mov		[bx],ax
	ret


	.psect	data,class=DATA

	.globl	_contrl
	.globl	_ptsin
	.globl	_intin
	.globl	_intout
	.globl	_ptsout

pblock:
	.word	_contrl
	.word	seg _contrl
intioff:
	.word	_intin
	.word	seg _intin
ptsioff:
	.word	_ptsin
	.word	seg _ptsin
intooff:
	.word	_intout
	.word	seg _intout
ptsooff:
	.word	_ptsout
	.word	seg _ptsout

