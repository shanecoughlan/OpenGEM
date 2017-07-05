
;/*	GEMASM.ASM	5/03/84 -  01/05/85	Lee Lorenzen		*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;/*     modified for Large model    09/16/86    C. Shaw			*/
;
;
	INCLUDE LARGE.MAC
;
PSEG
	PUBLIC	gem
X	EQU	2		; extra offset for caller's segment
;
GEMENTRY 	equ	0efh
;
BEGIN	PROC	FAR
gem:	push	bp
	mov	bp,sp
	mov	cx,200
	mov	dx,0
	mov	bx,04 + X[bp]
	mov	ax,06 + X[bp]
	mov	es,ax
	int	GEMENTRY
	pop	bp    
	ret
;
BEGIN	ENDP
ENDPS
	end
