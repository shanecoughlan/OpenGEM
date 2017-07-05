
;
;/*	TCRTLASM.ASM	4/18/84 - 10/24/84	Lee Jay Lorenzen	*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;
;/*	modified for Large model    09/12/85    C. Shaw                 */		

	INCLUDE LARGE.MAC

X	EQU	2		; extra offset for caller's segment

	DSEG
	ENDDS

	PSEG								  
public  cxc33    ; Comment out 
public	cxm33	 ; If you use 
public	cxd33	 ; runtime support

public	umul_div
public	mul_div

BEGIN	PROC	FAR

;******************************************************************************
;
;	LATTICE C SUPPORT ROUTINES
;
; 		cxc33 - long compare
; 		cxm33 - long multiply
; 		cxd33 - long divide
;
;******************************************************************************

cxc33:
;=====
		cmp ax,cx 
		jnz cxc2
		push bx 
		push dx
		shr bx,1
	    	shr dx,1			; Kapt. Kludge lives
	    	cmp bx,dx 		
		jnz cxc1			; need to compare unsigned
		pop dx 	
		pop bx
		cmp bx,dx
		ret				; say what ??!!!???	
cxc1:	    	pop dx 
		pop bx
cxc2:	    	ret

cxm33:
;=====

	   	push dx
	   	mul dx				; ax * dx, take low part only
 	  	pop dx
	  	push ax				; save low part
	  	mov ax,bx
          	push dx	       			; save dx
	  	mul cx				; bx * cx, low only
		mov cx,ax			; low part
	  	mov ax,bx
	  	pop dx
	  	mul dx				; whole thing
	  	add dx,cx
	  	pop cx
	  	add dx,cx
	  	mov bx,ax
	  	mov ax,dx
	  	ret


	; CXD33
	;	enter axbx long dividend, dx word divisor
	;	exit  axbx quotient cxdx remainder
	;
	;	Entry:	BX,AX = long dividend
	;		CX = word divisor
	;	Exit:	DX,AX = long result
	;		CX = remainder

cxd33:
;=====

	 	xchg ax,bx 
		mov cx,dx
	 	jcxz div0			;divide by 0
	 	cmp cx,1 
		jne not_div1			;divide by 1
	 	xchg ax,bx
	 	xor cx,cx 
		mov dx,cx
	 	ret

not_div1:   
	 	xor dx,dx			;extend AX to long value
	 	xchg ax,bx			;low in BX, high in DX,AX
	 	push ax				;save ax
	 	div cx				;divide high part
	 	mov di,ax			;save result
	 	mul cx				;AX = even divisor part
	 	pop dx				;old high in DX
	 	sub dx,ax			;eliminate even part
	 	mov ax,bx			;AX = old low part
	 	div cx				;low part in AX
	 					; rem in cxdx, quo in axbx

         	sub cx,cx 
		mov bx,ax
         	mov ax,di			;high result in DX
         	ret
;
div0:		mov ax,-1 
		mov bx,ax
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
; The point of this routine is to get floating point accuracy without
; going to the expense of floating point (sk)
;***************************************************************************

MUL_DIV:
	push	bp		;save the callers bp
	mov	bp,sp
	mov	ax,06+X[bp]
	shl	ax,1		; m2 * 2
	mov	bx,04+X[bp]
	imul	bx		; m2 * 2 * m1
	mov	bx,08+X[bp]
	idiv	bx		; m2 * 2 * m1 / d1
	and	ax,ax		; set flags
	js	mul_div_neg
	inc	ax
	shr	ax,1		; m2 * 2 * m1 / d1 +1 / 2
	pop	bp
	ret
mul_div_neg:
	add	ax,-1
	neg	ax
	shr	ax,1
	neg	ax		; m2 * 2 * m1 / d1 -1 / 2
	pop	bp
	ret
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
; The point of this routine is to get floating point accuracy without
; going to the expense of floating point (sk)
;***************************************************************************
UMUL_DIV:
	push	bp		;save the callers bp
	mov	bp,sp
	mov	ax,06+X[bp]
	shl	ax,1		; m2 * 2
	mov	bx,04+X[bp]
	mul	bx		; m2 * 2 * m1
	mov	bx,08+X[bp]
	div	bx		; m2 * 2 * m1 / d1
	inc	ax
	shr	ax,1		; m2 * 2 * m1 / d1 +1 / 2
	pop	bp
	ret


BEGIN	ENDP
	ENDPS			; "th-th-th-that's all folks!"  --Porky Pig

	end
