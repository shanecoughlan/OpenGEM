


	.globl	large_data
	.globl	large_code
	.psect	ltext,local,class=CODE,reloc=16,size=65535

;*************************************************************************** ;#
;						
; MUL_DIV (m1,m2,d1)
;
; ( ( ( m1 * 2 * m2 ) / d1 ) + 1 ) / 2 
; m1 = signed 16 bit integer 
; m2 = unsigned 15 bit integer 
; d1 = signed 16 bit integer
;
;*************************************************************************** ;#


	.globl	_mul_div
	.signat	_mul_div,12346
_mul_div:
			;dx = m1
			;ax = m2
			;6[bp] = d1

	push	bp		;save the callers bp
 	mov		bp,sp
	shl		ax,#1	;m2 * 2
	mov		bx,dx	;m1
	imul	bx		;m2 * 2 * m1
	mov		bx,6[bp];
	idiv	bx		;m2 * 2 * m1 / d1
	and		ax,ax	;set flags
	js		1f
	inc		ax
	shr		ax,#1
	pop		bp
	retf	#2

1:	add		ax,#-1
	neg		ax
	shr		ax,#1
	neg		ax
	pop		bp	
	retf	#2		;Prototyped, so it pops its own stack

;*************************************************************************** ;#
; 
; UMUL_DIV (m1,m2,d1) 
; 
; ( ( ( m1 * 2 * m2 ) / d1 ) + 1 ) / 2 
; m1 = usigned 16 bit integer 
; m2 = unsigned 15 bit integer 
; d1 = usigned 16 bit integer 
;
;*************************************************************************** ;#


	.globl	_umul_div
	.signat	_umul_div,12346
_umul_div:
	push	bp
	mov		bp,sp
	shl		ax,#1
	mov		bx,dx
	mul		bx
	mov		bx,6[bp]
	div		bx
	inc		ax
	shr		ax,#1
	pop		bp
	retf	#2		;Prototyped, so it pops its own stack

