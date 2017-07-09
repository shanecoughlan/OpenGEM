
;
;/*	LONGASM.ASM	2/3/84 - 10/09/84	Lee Jay Lorenzen	*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;/*	for Lattice C ==>  AX=HI, BX=LO
;
;	modified for Large model  9/12/85       C. Shaw			*/
;   
	INCLUDE LARGE.MAC

X	EQU	2		; extra offset for caller's segment

	DSEG
	ENDDS

	PSEG
;
	PUBLIC	LSTRLEN

	PUBLIC	LLDS
	PUBLIC	LLCS
	PUBLIC	LLSS
	PUBLIC	LWCOPY
	PUBLIC	LBCOPY
	PUBLIC	LBWMOV

BEGIN	PROC	FAR
;
;		WORD
;	LSTRLEN(long_addr)
;
LSTRLEN:
	push	bp
	mov	bp,sp
	push	ds
	mov	bx,4+X[bp]
	mov	ax,6+X[bp]
	mov	ds,ax
	sub	cx,cx
slagain:
	cmp	byte ptr [bx],0
	jz	sldone
	inc	bx
	jmp	slagain
sldone:
	mov	ax,bx
	mov	bx,4+X[bp]
	sub	ax,bx
	pop	ds
	pop	bp
	ret
;
;		LONG
;	ADDR(shrt_addr)
;
;
;		LONG
;	LLDS()
;
;
LLDS:
	push bp
	mov bp,sp
	sub bx,bx
	mov ax,ds
	pop bp
	ret
;
;		LONG
;	LLCS()
;
;
LLCS:
	push bp
	mov bp,sp
	sub bx,bx
	mov ax,cs
	pop bp
	ret
;
;		LONG
;	LLSS()
;
;
LLSS:
	push bp
	mov bp,sp
	sub bx,bx
	mov ax,ss
	pop bp
	ret
;
;	LWCOPY(pdstoff, pdstseg, psrcoff, psrcseg, numwds)
;
;
LWCOPY:
	push bp
	mov bp,sp
	push di
	push si
	mov di,4+X[bp]		  ; dest off
	mov bx,6+X[bp]		  ; dest seg
	mov si,8+X[bp]		  ; src off
	mov dx,10+X[bp] 	  ; src seg
	mov cx,12+X[bp] 	  ; count
	cld			; assume forward
	cmp bx,dx		; if !same seg
	jne lwcopy_mov		;     then dont worry about overlap
	cmp si,di		; if src offs < dest offs
				;     then start from end and go back
				; else normal
	ja lwcopy_mov		;  jg changed to ja  (JJN)
	  mov ax,cx
	  shl ax,1
	  add si,ax
	  sub si,2
	  add di,ax
	  sub di,2
	  std			;  go backwards
lwcopy_mov:
	push ds
	push es
	mov ds,dx		; src
	mov es,bx		; dest
	rep movsw
	pop es
	pop ds


	lea sp,-4[bp]		; this seems like a no-op since the sp
				; is two pushes (si and di) away from bp
	pop si
	pop di
	pop bp
	cld
	ret
;
;
;	LBCOPY(pdstoff, pdstseg, psrcoff, psrcseg, numbytes)
;
;
LBCOPY:
	push bp
	mov bp,sp
	push di
	push si
	mov di,4+X[bp]		  ; dest off
	mov bx,6+X[bp]		  ; dest seg
	mov si,8+X[bp]		  ; src off
	mov dx,10+X[bp] 	  ; src seg
	mov cx,12+X[bp] 	  ; count
	cld			; assume forward
	cmp bx,dx		; if !same seg
	jne lbcopy_mov		;     then dont worry about overlap
	cmp si,di		; if src offs < dest offs
				;     then start from end and go back
				; else normal
	ja lbcopy_mov		;   jg changed to ja  (JJN)
	  add si,cx
	  dec si
	  add di,cx
	  dec di
	  std			;  go backwards
lbcopy_mov:
	push ds
	push es
	mov ds,dx		; src
	mov es,bx		; dest
	rep movsb
	pop es
	pop ds


	lea sp,-4[bp]	      ; see comment in lwcopy
	pop si
	pop di
	pop bp
	cld
	ret
;
;	expand from bytes to words until you hit a null also return
;	the number of characters moved
;
;	LBWMOV(pdstoff, pdstseg, psrcoff, psrcseg)
;
;
LBWMOV:
	push bp
	mov bp,sp
	push di
	push si
	mov di,4+X[bp]		  ; dest off
	mov bx,6+X[bp]		  ; dest seg
	mov si,8+X[bp]		  ; src off
	mov dx,10+X[bp] 	  ; src seg
	mov cx,0		; count
	cld			; assume forward
lbw_mov:
	push ds
	push es
	mov ds,dx		; src
	mov es,bx		; dest
	mov ah,0
lbw_loop:
	lodsb
	cmp al,0
	je lbw_done
	inc cx
	stosw
	jmp lbw_loop
lbw_done:
	pop es
	pop ds


	lea sp,-4[bp]
	pop si
	pop di
	pop bp
	cld
	mov	ax,cx
	ret
;
BEGIN	ENDP
	ENDPS
	end
