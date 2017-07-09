DGROUP  GROUP   DATA

DATA 	SEGMENT PARA PUBLIC 'DATA'
	vec_len_high	dw	0 
	vec_len_low 	dw	0

DATA	ENDS

PGROUP  GROUP   PROG

PROG	SEGMENT BYTE PUBLIC 'PROG'
ASSUME	CS:PGROUP
ASSUME	DS:DGROUP
	     
PUBLIC SMUL_DIV
PUBLIC vec_len


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
		mov	vec_len_high, dx		; save high word of square
		mov	vec_len_low, ax			; save low word of square

; Calculate delta_y squared and add to delta_x squared.
		mov	dx, 6[bp]		; delta_y parameter
		mov	ax, dx
		imul	dx

		add	vec_len_low, ax
		adc	vec_len_high, dx		; high/low = sum of squares

; Get the initial binary search boundaries.  This is done by taking an
;   approximate square root based on the highest order set bit in the
;   high/low bit string.
		cmp	vec_len_high, 0
		je	no_high_byte

		mov	ax, vec_len_high		; check on high order byte
		mov	cl, 16
		jmp	bounds_loop

no_high_byte:
		mov	ax, vec_len_low			; check on low order byte
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

		pop	bp
		ret


;
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

PROG	ENDS
	END
