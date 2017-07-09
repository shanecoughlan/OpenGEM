;/*	GEMASM.ASM	5/03/84 -  01/05/85	Lee Lorenzen		*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;
PGROUP	GROUP	PROG

PROG	SEGMENT	BYTE PUBLIC 'PROG'
	ASSUME	CS:PGROUP
;
GEMENTRY 	equ	0efh
;
	PUBLIC	gem
gem:	push	bp
	mov	bp,sp
	mov	cx,200
	mov	dx,0
	mov	bx,4[bp]
	mov	ax,6[bp]
	mov	es,ax
	int	GEMENTRY
	pop	bp
	ret
;
PROG	ENDS

	end

