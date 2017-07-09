;/*	OPTIMOPT.A86	3/20/85 - 06/10/85	Lee Jay Lorenzen	*/
;
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
	.psect	_TEXT,class=CODE
;
; [JCE] Rewritten for Pacific calling conventions. These functions 
; MUST be prototyped before use.
;
	.globl	_r_get
	.globl	_r_set
	.globl	_rc_equal
	.globl	_rc_copy
	.globl	_inside
	.globl	_min
	.globl	_max
	.globl	_toupper
	.globl	_scasb
	.globl	_movs
	.globl	_bfill
	.globl	_strlen
	.globl	_strcmp
	.globl	_strchk
	.globl	_strcpy

	.signat	_bfill,12344

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
;	VOID
;r_get(pxywh, px, py, pw, ph)
;	WORD		*pxywh;
;	WORD		*px, *py, *pw, *ph;

_r_get:
	push	bp
	mov		bp,sp

	push	es
	push	si
	push	di

	mov		cx,ds	; Converted to Pacific calling conventions
	mov		es,cx	; (1st 2 parameters in DX and AX)
	mov		si,dx	; SI = parameter 1, pxywh
	mov		di,ax	; DI = parameter 2, px
	lodsw
	stosw
	lodsw
	mov		di,4[bp]
	stosw
	lodsw
	mov		di,6[bp]
	stosw
	lodsw
	mov		di,8[bp]
	stosw

	pop		di
	pop		si
	pop		es

	pop		bp
	ret		#6
;
;
;	VOID
;r_set(pxywh, x, y, w, h)
;	WORD		*pxywh;
;	WORD		x, y, w, h;
;
_r_set:
	push	bp
	mov		bp,sp

	push	es
	push	di
	push	si		;DX = pxywh, AX = x

	mov		cx,ds
	mov		es,cx
	mov		di,dx
	stosw
	mov		ax,4[bp]
	stosw
	mov		ax,6[bp]
	stosw
	mov		ax,8[bp]
	stosw

	pop		si
	pop		di
	pop		es

	pop		bp
	ret		#6
;
;	WORD
;rc_equal(p1, p2)
;	WORD		*p1, *p2;
;
_rc_equal:
	push	bp
	mov		bp,sp

	push	es
	push	si
	push	di

	cld
	mov		cx,#4		; count
	mov		si,dx		; src off
	mov		di,ax		; dest off
	mov		dx,ds
	mov		es,dx
rcloop:
	lodsw
	scasw
	jne		rcfalse
	loop	rcloop
	mov		ax,#1
	jmp		1f
rcfalse:
	mov		ax,#0

1:	pop		di
	pop		si
	pop		es

	pop		bp
	ret

;
;
;	VOID
;rc_copy(psxywh, pdxywh)
;	WORD		*psxywh;
;	WORD		*pdxywh;
;
_rc_copy:
	push	bp
	mov		bp,sp

	push	es
	push	di
	push	si

	cld
	mov		cx,#4		; count
	mov		si,dx		; src off
	mov     di,ax		; dest off
	mov		dx,ds		; src
	mov		es,dx		; dest
	rep		movsw

	pop		si
	pop		di
	pop		es

	pop		bp
	ret
;
;
;
;	UWORD
;inside(x, y, pt)
;	WORD		x, y;
;	GRECT		*pt;
;
_inside:
	push	bp
	mov		bp,sp
	push	si
	push	di

	mov		cx,ax		; cx = y
	mov		ax,dx		; ax = x
	mov		si,4[bp]	; si = *pt
	cmp		ax,[si]
	jl		infalse
	cmp		cx,2[si]
	jl		infalse
	mov		dx,4[si]
	add		dx,[si]			; dx = x + w
	cmp		ax,dx
	jge		infalse
	mov		dx,6[si]
	add		dx,2[si]		; dx = y + h
	cmp		cx,dx
	jge		infalse
	mov		ax,#1
	jmp		inret

infalse:
	mov		ax,#0
inret:
	pop		di
	pop		si
	pop		bp
	ret		#2
;
;
;	WORD
;min(a, b)
;	WORD		a, b;
;
_min:
	cmp		ax,dx	
	jle		minret
	mov		ax,dx
minret:
	ret
;
;
;	WORD
;max(a, b)
;	WORD		a, b;
;
_max:
	cmp		ax,dx
	jge		minret
	mov		ax,dx
	ret
;
;
;	BYTE
;toupper(ch)
;	BYTE		ch;
;
_toupper:
	push	bp
	mov		bp,sp
	mov		ax,dx
	and		ax,#00ffh
	cmp		al,#'a'
	jl		toupret
	cmp		al,#'z'
	jg		toupret
	sub		ax,#32
toupret:
	pop		bp
	ret
;
;
;	BYTE
;*scasb(p, b)
;	BYTE		*p;
;	BYTE		b;
;
_scasb:
	push	bp
	mov		bp,sp
	push	si
	cld
	mov		si,dx	;Source
	mov		bx,ax	;Byte
	and		bx,#00ffh
lpscas:
	lodsb
	cmp		al,bh
	jz		scasret
	cmp		al,bl
	jnz		lpscas
scasret:
	mov		ax,si
	dec		ax
	pop		si
	pop		bp
	ret
;
;
;	VOID
;movs(num, ps, pd)
;	WORD		num;
;	BYTE		*ps, *pd;
;
_movs:
	push	bp
	mov		bp,sp

	push	es
	push	si
	push	di

	cld
	mov		cx,dx		; Count
	mov		si,ax		; Source offset
	mov		di,4[bp]	; dest off
	mov		dx,ds		; src
	mov		es,dx		; dest
	rep		movsb

	pop		di
	pop		si
	pop		es

	pop		bp
	ret		#2
;
;
;	VOID
;bfill(num, bval, addr)
;	REG WORD	num;
;	BYTE		bval;
;	REG BYTE	*addr;
;
_bfill:
	push	bp
	mov		bp,sp

	push	es
	push	di

	cld
	mov     cx,dx		; count
;;	mov		ax,ax		; byte value
	mov		di,4[bp]	; dest off
	mov		dx,ds
	mov		es,dx		; dest
	rep		stosb

	pop		di
	pop		es

	pop		bp
	ret		#2
;
;
;	WORD
;strlen(p1)
;	BYTE		*p1;
;
_strlen:
	push	bp
	mov		bp,sp

	push	es
	push	di
	push	si

	cld
	mov		bx,dx		; remember start of string
	mov		di,bx		; destination string offset
	mov		ax,ds
	mov		es,ax		; destination string segment
	xor		ax,ax		; look for a null
	mov		cx,#7fffh
	repnz	scasb
	mov		ax,di
	sub		ax,bx
	dec		ax

	pop		si
	pop		di
	pop		es

	pop		bp
	ret
;
;
;	WORD
;strcmp(p1, p2)
;	BYTE		*p1, *p2;
;
_strcmp:
	push	bp
	mov		bp,sp

	push	es
	push	si
	push	di

	cld
	mov		si,dx		;p1
	mov		di,ax		;p2
	mov		dx,ds
	mov		es,dx
	xor		ax,ax		; clear ah,al
lpsc:
	lodsb				; pick up source
	scasb				; compare with destination
	jne		scfalse		; if not same then false
	cmp		al,ah		; if no more string
	jnz		lpsc		;   then exit true
sctrue:
	mov		ax,#1
scret:

	pop		di
	pop		si
	pop		es

	pop		bp
	ret
scfalse:
	mov	ax,#0
	jmp	scret
;
;
;/*
;*	Return <0 if s<t, 0 if s==t, >0 if s>t
;*/
;
;	WORD
;strchk(s, t)
;	BYTE		s[], t[];
;
_strchk:
	push	bp
	mov	bp,sp

	push	es
	push	si
	push	di

	cld
	mov		si,dx		;si -> s
	mov		di,ax		;di -> t
	mov		ax,ds
	mov		es,ax
	xor		ax,ax		; clear ah,al
lpschk:
	lodsb				; pick up source
	scasb				; compare with destination
	jne		scplmn		; if not same then false
	cmp		al,ah		; if no more string
	jnz		lpschk		;   then exit true
scplmn:
	mov		bl,-1[di]
	mov		bh,#0
	sub		ax,bx

	pop		di
	pop		si
	pop		es

	pop		bp
	ret
;
;
;	BYTE
;*strcpy(ps, pd)
;	BYTE		*ps, *pd;
;
_strcpy:
	push	bp
	mov		bp,sp

	push	es
	push	si
	push	di

	cld
	mov		si,dx	;ps
	mov		di,ax	;pd
	mov		ax,ds
	mov		es,ax
	xor		ax,ax		; clear ah,al
lpcpy:
	lodsb				; pick up source
	stosb				; put in destination
	cmp		al,ah		; if no more string
	jnz		lpcpy		;   then copy done
cpydne:
	mov		ax,di

	pop		di
	pop		si
	pop		es

	pop		bp
	ret



