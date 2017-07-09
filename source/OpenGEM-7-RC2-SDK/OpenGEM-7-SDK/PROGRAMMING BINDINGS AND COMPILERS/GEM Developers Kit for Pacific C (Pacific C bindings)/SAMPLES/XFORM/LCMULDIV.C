/*
    Calculation code for XFORM.APP. 

    Doing it within #asm ... #endasm means that PPD generates the stack
    frame code.
*/

#include "ppdgem.h"

/**************************************************************************
	n = vec_len(delta_x, delta_y);

	NOTE:	delta_x and delta_y must both be
		greater than or equal to zero
**************************************************************************/

#asm

	.psect data,class=DATA
vec_len_low:	.word	0
vec_len_high:	.word	0
	

#endasm

WORD vec_len(WORD delta_x, WORD delta_y)
{
	return 1;
#asm

; Check for zeroes.
		cmp	ax, #0				; for PPD
		jne	x_squared			; modified
		cmp	dx, #0				; 8 Jan 1998 John Elliott
		jne	x_squared
		xor	bx, bx				; return value
		jmp	search_loop_end

; Calculate delta_x squared.
x_squared:
		push	dx				; dx holds delta_y
		mov		dx,ax			; dx := delta_x
		imul	dx
		mov	vec_len_high, dx		; save high word of square
		mov	vec_len_low, ax			; save low word of square

; Calculate delta_y squared and add to delta_x squared.
		pop		dx		; delta_y parameter
		mov	ax, dx
		imul	dx

		add	vec_len_low, ax
		adc	vec_len_high, dx		; high/low = sum of squares

; Get the initial binary search boundaries.  This is done by taking an
;   approximate square root based on the highest order set bit in the
;   high/low bit string.
		mov	ax,vec_len_high
		cmp	ax,#0
		je	no_high_byte

		mov	cl, #16
		jmp	bounds_loop

no_high_byte:
		mov	ax, vec_len_low			; check on low order byte
		sub	cl, cl

bounds_loop:
		cmp	ax, #1			; done yet?
		je	bounds_loop_end
		inc	cl
		shr	ax, #1
		jmp	bounds_loop

bounds_loop_end:
		shr	cl, #1
		mov	bx, #1
		shl	bx, cl			; bx = initial low bound
		mov	cx, bx
		shl	cx, #1			; cx = initial high bound

; Perform a binary search for a square root (somewhat brutishly).
search_loop:
		mov	ax, cx
		sub	ax, bx
		cmp	ax, #1			; done with the search?
		jle	search_loop_end

		shr	ax, #1
		add	ax, bx			; candidate = (high+low)/2
		mov	si, ax			; save a copy for next pass
		mul	ax			; dx/ax = candidate square

		cmp	dx, vec_len_high	; check against high word
		ja	high_adjust
		jb	low_adjust

		cmp	ax, vec_len_low		; check against low word
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
#endasm	
}	

/***************************************************************************

 SMUL_DIV (m1,m2,d1)

	 ( ( ( m1 * m2 ) / d1 ) + 1 / 2	
	m1 = signed 16 bit integer
	m2 = snsigned 15 bit integer
	d1 = signed 16 bit integer

***************************************************************************/


WORD SMUL_DIV(WORD m1, UWORD m2, WORD d1)
{
#asm
;	mov	ax,06+X[bp]	;
;	mov	bx,04+X[bp]	;
	push	si
	mov		bx,dx	; ppd passes 1st 2 parameters in AX & DX
	imul	bx		; m2 * m1
	mov	si, #1
	and	dx, dx
	jns	smul_div_1
	neg	si
smul_div_1:
	mov	bx,06[bp]	; large model modification 9/4/87
	idiv	bx		; m2 * m1 / d1
	and	bx, bx		; test if divisor is negative
	jns	smul_div_2
	neg	si
	neg	bx		; make it positive
smul_div_2:
	and	dx, dx		; test if remainder is negative
	jns	smul_div_3
	neg	dx		; make remainder positive
smul_div_3:
	shl	dx, #1		; see if 2 * remainder is > divisor
	cmp	dx, bx
	jl	smul_div_4
	add	ax, si
smul_div_4:
	pop	si
#endasm
}
