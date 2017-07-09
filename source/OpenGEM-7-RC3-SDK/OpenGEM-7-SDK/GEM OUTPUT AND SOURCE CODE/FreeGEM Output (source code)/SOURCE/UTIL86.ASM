;/*******************************************************************/
;/*                                                                 */
;/*      Copyright 1999, Caldera Thin Clients, Inc.                 */
;/*      This software is licenced under the GNU Public License.    */
;/*      Please see LICENSE.TXT for further information.            */
;/*                                                                 */
;/*                 Historical Copyright                            */
;/*******************************************************************/
;/* Copyright (c) 1987 Digital Research Inc.		           */
;/* The software contained in this listing is proprietary to        */
;/* Digital Research Inc., Pacific Grove, California and is         */
;/* covered by U.S. and other copyright protection.  Unauthorized   */
;/* copying, adaptation, distribution, use or display is prohibited */
;/* and may be subject to civil and criminal penalties.  Disclosure */
;/* to others is prohibited.  For the terms and conditions of soft- */
;/* ware code use refer to the appropriate Digital Research         */
;/* license agreement.						   */
;/*******************************************************************/

; UTIL86.ASM	misc assembly utilities for GEM OUTPUT.	
; 06-04-85	Susan Bancroft
;
PGROUP	GROUP	PROG
DGROUP	GROUP	DATA

DATA	SEGMENT PARA PUBLIC 'DATA'

	public	brk_addr
	public  brk_seg 

brk_addr dw 	offset brk_hndler
brk_seg	 dw 	seg    brk_hndler
rtn_addr dw 	0
rtn_seg	 dw 	0
data_seg dw	0
vlen_hi	 dw	0
vlen_lo	 dw	0

DATA	ENDS


PROG	SEGMENT	BYTE PUBLIC 'PROG'
	ASSUME	CS:PGROUP
	ASSUME	DS:DGROUP

	extrn	dr_code:near

	public	SMUL_DIV
	public  RMUL_DIV
	public  UMUL_DIV
	public  brk_hndler 
	public  seg_off
	public	b_move
	public  b_stuff
	public  swapbyte
	public	cxc33, cxm33, cxd33
	public  vec_len
	public	l_neg


;***************************************************************************
;
; SMUL_DIV (m1,m2,d1)
;
;	 ( ( ( m1 * m2 ) / d1 ) + 1 / 2	
;	m1 = signed 16 bit integer
;	m2 = snsigned 15 bit integer
;	d1 = signed 16 bit integer
;
;***************************************************************************
SMUL_DIV:
	push	bp		;save the callers bp
	mov	bp,sp
	mov	ax,06[bp]
	mov	bx,04[bp]
	imul	bx		; m2 * m1
	mov	si, 1
	and	dx, dx
	jns	smul_div_1
	neg	si
smul_div_1:
	mov	bx,08[bp]
	idiv	bx		; m2 * m1 / d1
	pop	bp
	and	bx, bx		; test if divisor is negative
	jns	smul_div_2
	neg	si
	neg	bx		; make it positive
smul_div_2:
	and	dx, dx		; test if remainder is negative
	jns	smul_div_3
	neg	dx		; make remainder positive
smul_div_3:
	shl	dx, 1		; see if 2 * remainder is > divisor
	cmp	dx, bx
	jl	smul_div_4
	add	ax, si
smul_div_4:
	ret

;***************************************************************************
;
; RMUL_DIV (m1,m2,d1,r1)
;
;	 ( ( ( m1 * m2 ) / d1 ) with remainder r1	
;	m1 = signed 16 bit integer
;	m2 = snsigned 15 bit integer
;	d1 = signed 16 bit integer
;	r1 = address of remainder
;
;***************************************************************************
RMUL_DIV:
	push	bp		;save the callers bp
	mov	bp,sp
	mov	ax,06[bp]
	mov	bx,04[bp]
	imul	bx		; m2 * m1
	mov	bx,08[bp]
	idiv	bx		; m2 * m1 / d1
	mov	bx,10[bp]	; return remainder
	mov	[bx],dx
	pop	bp
	ret

;***************************************************************************
;
; UMUL_DIV (m1,m2,d1)
;
;	 ( ( m1 * m2 ) / d1 ) rounded if remainder*2 >= d1
;	m1 = usigned 16 bit integer
;	m2 = unsigned 15 bit integer
;	d1 = usigned 16 bit integer
;
;***************************************************************************
UMUL_DIV:
	push	bp		;save the callers bp
	mov	bp,sp
	mov	ax,06[bp]
	mov	bx,04[bp]
	mul	bx		; m2 * m1
	mov	bx,08[bp]
	div	bx		; m2 * m1 / d1
	shl	dx,1		; remainder * 2
	cmp	dx,bx		; round if remainder*2 greater than
	jb	no_round	; divisor
	inc	ax
no_round:
	pop	bp
	ret
;
;***************************************************************************
; BRK_HNDLER
;
; Routine for handling control break interupts
;
;***************************************************************************
brk_hndler:
	pop rtn_addr
	pop rtn_addr
	pop rtn_addr
	pop rtn_addr
	pop rtn_seg
	popf
	stc
	pushf
	push rtn_seg
	push rtn_addr
	mov ax, 200
	iret


;
;************************************************************************
;		LONG							*
;	address = seg_off(WORD)						*
;		returns bx = offset					*
;			ax = segment					*
;************************************************************************
seg_off:
		push	bp
		mov	bp, sp
		mov	bx, 4[bp]
		mov	ax, bx
		and	bx, 000fh		;make it an offset
		mov	cl, 4
		shr	ax, cl			;make it a segment
		pop	bp
		ret

;
;************************************************************************
;	b_move(&from, count, &to);					*
;		&from:		ADDR	4[bp]				*
;		count:		WORD	8[bp]				*
;		&to:		ADDR	10[bp]				*
;************************************************************************
b_move:
		push	bp
		mov	bp, sp

; Move from the source to the destination.
		mov	cx, 8[bp]
		les	di, 10[bp]		; es:di = address of destination
		lds	si, 4[bp]		; ds:si = address of source
	rep	movsb				; move bytes

; Restore and exit.
		mov	ax, seg data_seg
		mov	ds, ax
		pop	bp
		ret

;************************************************************************
;	b_stuff(pointer, count, value);					*
;		pointer:	ADDR	4[bp]				*
;		count:		WORD	8[bp]				*
;		value:		BYTE	10[bp]				*
;************************************************************************
b_stuff:
		push	bp
		mov	bp, sp

; Set the requested number of bytes to the requested value.
		les	di, 4[bp]		; es:di = destination address
		mov	cx, 8[bp]		; cx = count
		mov	al, 10[bp]		; al = value to set
	rep	stosb				; stuff

; Restore and exit.
		pop	bp
		ret

;************************************************************************
;	swapbyte(&buffer, count);					*
;		&buffer		ADDR	4[bp]				*
;************************************************************************
swapbyte:
		push	bp
		mov	bp, sp

; Get the address of the buffer and the count of words to be swapped.
		les	di, 4[bp]		; es:di = buffer to swap
		mov	cx, 8[bp]

; Do the dirty deed.
swapbyte_loop:
		mov	ax, es:[di]
		xchg	ah, al			; swap 'em
		stosw
		loop	swapbyte_loop

		pop	bp
		ret
;
;************************************************************************
;
;	LONG COMPARE
;
;************************************************************************
cxc33:
	push	bx
	push	dx
	cmp	ax,cx
	jnz	cxc1
	shr	bx,1
	shr	dx,1
	cmp	bx,dx
	jnz	cxc1
	pop	dx
	pop	bx
	cmp	bx,dx
	jmp	cxc2
cxc1:
	pop	dx
	pop	bx
cxc2:
	ret


;************************************************************************
;
;	LONG DIVIDE
;
;************************************************************************
cxd33:
	xchg	ax,bx
	mov	cx,dx
	jcxz	div0
	cmp	cx,1
	jne	not_div1
	xchg	ax,bx
	xor	cx,cx
	mov	dx,cx
	ret
not_div1:
	xor	dx,dx
	xchg	ax,bx
	push	ax
	div	cx
	mov	di,ax
	mul	cx
	pop	dx
	sub	dx,ax
	mov	ax,bx
	div	cx
	sub	cx,cx
	mov	bx,ax
	mov	ax,di
	ret
div0:
	mov	ax,-1
	mov	bx,ax
	ret


;************************************************************************
;
;	LONG MULTIPLY
;
;************************************************************************
cxm33:
	push	dx
	mul	dx
	pop	dx
	push	ax
	mov	ax,bx
	push	dx
	mul	cx
	mov	cx,ax
	mov	ax,bx
	pop	dx
	mul	dx
	add	dx,cx
	pop	cx
	add	dx,cx
	mov	bx,ax
	mov	ax,dx
	ret

;***************************************************************************
;	n = vec_len(delta_x, delta_y);
;
;	NOTE:	delta_x and delta_y must both be
;		greater than or equal to zero
;***************************************************************************
vec_len:
		push	bp
		mov	bp, sp

; Check for zeroes.
		cmp	word ptr 4[bp], 0
		jne	x_squared
		cmp	word ptr 6[bp], 0
		jne	x_squared
		xor	bx, bx				; return value
		jmp	search_loop_end

; Calculate delta_x squared.
x_squared:
		mov	dx, 4[bp]		; delta_x parameter
		mov	ax, dx
		imul	dx
		mov	vlen_hi, dx		; save high word of square
		mov	vlen_lo, ax		; save low word of square

; Calculate delta_y squared and add to delta_x squared.
		mov	dx, 6[bp]		; delta_y parameter
		mov	ax, dx
		imul	dx

		add	vlen_lo, ax
		adc	vlen_hi, dx		; high/low = sum of squares

; Get the initial binary search boundaries.  This is done by taking an
;   approximate square root based on the highest order set bit in the
;   high/low bit string.
		cmp	vlen_hi, 0
		je	no_high_byte

		mov	ax, vlen_hi		; check on high order byte
		mov	cl, 16
		jmp	bounds_loop

no_high_byte:
		mov	ax, vlen_lo			; check on low order byte
		sub	cl, cl

bounds_loop:
		cmp	ax, 1			; done yet?
		je	bounds_loop_end
		inc	cl
		shr	ax, 1
		jmp	bounds_loop

bounds_loop_end:
		shr	cl, 1
		mov	bx, 1
		shl	bx, cl			; bx = initial low bound
		mov	cx, bx
		shl	cx, 1			; cx = initial high bound

; Perform a binary search for a square root (somewhat brutishly).
search_loop:
		mov	ax, cx
		sub	ax, bx
		cmp	ax, 1			; done with the search?
		jle	search_loop_end

		shr	ax, 1
		add	ax, bx			; candidate = (high+low)/2
		mov	si, ax			; save a copy for next pass
		mul	ax			; dx/ax = candidate square

		cmp	dx, vlen_hi		; check against high word
		ja	high_adjust
		jb	low_adjust

		cmp	ax, vlen_lo			; check against low word
		ja	high_adjust
		jb	low_adjust

		mov	bx, si			; exact root found!
		jmp	search_loop_end

high_adjust:
		mov	cx, si			; adjust high value down
		jmp	search_loop

low_adjust:
		mov	bx, si			; adjust low value up
		jmp	search_loop

search_loop_end:
		mov	ax, bx			; ax = solution

		pop	bp
		ret

;***************************************************************************
;returns negative of a long value. 
;      LONG
; l_neg( long_val )
; LONG	long_val ;
; {
;	return( (-1L) * long_val ) ;
; }
;
;************************************************************************
;
l_neg:		
		push 	bp
		mov	bp, sp
		mov 	bx, 04[bp]
		mov	ax, 06[bp]
		sub	cx, cx
		neg	bx
		sbb	cx, ax
		mov	ax, cx
		pop	bp
		ret
		
PROG	ENDS


	end