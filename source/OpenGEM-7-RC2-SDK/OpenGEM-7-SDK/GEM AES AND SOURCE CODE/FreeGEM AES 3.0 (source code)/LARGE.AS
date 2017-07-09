;/*	LARGE.A86	2/3/84 - 06/10/85	Lee Jay Lorenzen	*/
;/*     for Lattice C ==>  AX=HI, BX=LO					*/
;
;
;       Copyright 1999, Caldera Thin Clients, Inc.                      
;       This software is licenced under the GNU Public License.         
;       Please see LICENSE.TXT for further information.                 
;                                                                       
;                  Historical Copyright                                 
;
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



	.psect	_TEXT,class=CODE
;
	.globl	_HW
	.globl  _LHIWD
	.globl	_LSTRLEN
	.globl	_LSTCPY
;	.globl	_ADDR
	.globl	_LLDS
	.globl	_LLCS
	.globl	_LLSS 
	.globl	_LWCOPY
	.globl	_LBCOPY
;	.globl	_LBGET
;	.globl	_LBSET
	.globl	_LWGET
;	.globl	_LWSET
;	.globl	_LLGET
;	.globl	_LLSET
	.globl	_LBWMOV
;
;		LONG
;	HW(WORD)	
;						/* coerce short word to	*/
;						/*   high word  of long	*/
;#define HW(x) ((LONG)((UWORD)(x)) << 16)
;
_HW:
;;	mov		dx,dx	;High word
	xor		ax,ax	;Low word
	ret
;
;		WORD
;	LHIWD(aLONG)
;						/* return high word of	*/
;						/*   a long value	*/
;#define LHIWD(x) ((UWORD)(x >> 16))
;
_LHIWD:
	push	bp			
    mov		bp,sp
;;	mov		ax,4[bp]		; move offset
	mov		ax,6[bp]		; move segment
	pop		bp
	ret		#4
;
;		WORD
;	LSTRLEN(long_addr)
;
_LSTRLEN:
	push	bp
	mov		bp,sp
	push	di
	cld
	les		di,4[bp]		; destination string offset
	mov		bx,di 			; remember start of string
	xor		ax,ax			; look for a null
	mov		cx,#7fffh
	repnz	scasb
	mov		ax,di
	sub		ax,bx
	dec		ax
	pop		di
	pop		bp
	ret		#4
;
;		LONG
;	ADDR(shrt_addr)
;
;ADDR:
;	push	bp
;	mov	bp,sp
;	mov	bx,4[bp]		; move low word of addr
;	mov	ax,ds			; set hi word of addr
;	pop	bp
;	ret				; return
;
;		LONG
;	LLDS()
;
;
_LLDS:
	xor 	ax,ax			; clear bx
	mov 	dx,ds
	ret
;
;		LONG
;	LLCS()
;
;
_LLCS:
	xor		ax,ax
	mov		dx,cs
	ret
;
;		LONG
;	LLSS()
;
;
_LLSS:
	xor 	ax,ax
	mov 	dx,ss
	ret
;
;	LWCOPY(pdstoff, pdstseg, psrcoff, psrcseg, numwds)
;
;
_LWCOPY:
	push	bp
	mov		bp,sp
	push	di
	push	si
	push	ds

	cld
	les		di,4[bp]		; dest off
	lds		si,8[bp]		; src off
	mov		cx,12[bp]		; count
	rep		movsw

	pop		ds
	pop		si
	pop		di

	pop		bp
	ret		#10
;
;
;	LBCOPY(pdstoff, pdstseg, psrcoff, psrcseg, numbytes)
;
;
_LBCOPY:
	push	bp
	mov		bp,sp

	push	ds
	push	di
	push	si

	cld
	les		di,4[bp]		; dest off
	lds		si,8[bp]		; src off
	mov		cx,12[bp]		; count
	rep		movsb

	pop		si
	pop		di
	pop		ds

	pop		bp
	ret		#10
;
;	expand from bytes to words until you hit a null also return
;	the number of characters moved
;
;	LBWMOV(pdstoff, pdstseg, psrcoff, psrcseg)
;
;
_LBWMOV:
	push 	bp
	mov		bp,sp

	push	ds
	push	di
	push	si

	cld			; assume forward
	les		di,4[bp]	; dest off
	lds		si,8[bp]	; src off
	mov		cx,#0		; count
	mov		ah,#0
lbw_loop:
	lodsb
	cmp		al,#0
	je		lbw_done
	inc		cx
	stosw
	jmp		lbw_loop
lbw_done:
	mov		ax,cx

	pop		si
	pop		di
	pop		ds

	pop		bp
	ret		#8
;
;	copy from src to dest until and including a null in the 
;	source string
;
;	len = LSTCPY(pdstoff, pdstseg, psrcoff, psrcseg)
;
;
_LSTCPY:
	push	bp
	mov		bp,sp
	push	ds
	push	di
	push	si
	cld			; assume forward
	les		di,4[bp]	; dest off & seg
	lds		si,8[bp]	; src off & seg
	mov		cx,#0		; count
	mov		ah,#0
lsc_loop:
	lodsb
	cmp		al,#0
	je		lsc_done
	inc		cx
	stosb
	jmp		lsc_loop
lsc_done:
	stosb
	pop		si
	pop		di
	mov		ax,cx
	pop		ds
	pop		bp
	ret		#8
;
;		UBYTE
;	LBGET(lp)
;		LONG		p;
;
;LBGET:
;	push	bp
;	mov		bp,sp
;	push	es	
;	les	bx,4[bp]	;load offset
;	sub		ax,ax
;	mov		al,es:[bx]
;	pop		es
;	pop		bp
;	ret
;
;		VOID
;	LBSET(lp, abyte)
;		LONG		p;
;		BYTE		abyte;
;
;LBSET:
;	push	bp
;	mov	bp,sp
;	push	es
;	les	bx,4[bp]	; load offset
;	mov	ax,8[bp]	; load the value
;	mov	es:[bx],al
;	xor	ah,ah
;	pop	es
;	pop	bp
;	ret
;
;		UWORD
;	LWGET(lp)
;		LONG		p;
;
_LWGET:
	push	bp
	mov		bp,sp
	push	es		
	les		bx,4[bp]
	mov		ax,es:[bx]
	pop		es
	pop		bp
	ret		#4
;
;		VOID
;	LWSET(lp, aword)
;		LONG		p;
;		WORD		aword;
;
;LWSET:
;	push	bp
;	mov	bp,sp
;	push	es
;	les	bx,4[bp]
;	mov	ax,8[bp]
;	mov	es:[bx],ax
;	pop	es
;	pop	bp
;	ret
;
;		ULONG
;	LLGET(lp)
;		LONG		p;
;
;LLGET:
;	push	bp
;	mov	bp,sp
;	push	es
;	les	bx,4[bp]
;	mov	ax,es:[bx]
;	inc	bx
;	inc	bx
;	mov	bx,es:[bx]
;	xchg	ax,bx
;	pop	es
;	pop	bp
;	ret
;
;		VOID
;	LLSET(lp, along)
;		LONG		p;
;		LONG		along;
;
;LLSET:
;	push	bp
;	mov	bp,sp
;	push    es
;	les	bx,4[bp]	; load offset
;	mov	ax,8[bp]
;	mov	es:[bx],ax	; store lo value
;	inc	bx
;	inc	bx
;	mov	cx,10[bp]
;	mov	es:[bx],cx	; store hi value
;	pop	es
;	pop	bp
;	ret



