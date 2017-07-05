
;
;/*	VDIASM.ASM	6/26/84 - 10/16/84	Don Heiskell	*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;
;	modified for Large model    9/12/85 C. Shaw

	INCLUDE LARGE.MAC

	DSEG
	extrn	contrl:word
	extrn	ptsin:word
	extrn	intin:word
	extrn	intout:word
	extrn	ptsout:word
	extrn	pblock:word
	ENDDS

	PSEG

	public	vdi, gemcheck

GEM_INT 	equ	0efh
GEM_VECTOR	equ	4*GEM_INT

BEGIN	PROC	FAR
vdi:
	push	ds
	mov	dx, seg		pblock
	mov	ds, dx
	mov	dx,offset	pblock		;ds:dx points at the parm block
	mov	cx,473h
	int	GEM_INT
	pop	ds
	ret
;
gemcheck:
	push	es
	xor	ax, ax
	mov	es, ax
	mov	bx, es:[GEM_VECTOR]
	mov	ax, es:[GEM_VECTOR + 2]
	mov	es, ax
	cmp	es:byte ptr 2[bx], 'G'
	jne	not_loaded
	cmp	es:byte ptr 3[bx], 'E'
	jne	not_loaded
	cmp	es:byte ptr 4[bx], 'M'
	jne	not_loaded
	mov	ax, 1
	jmp	check_done
not_loaded:
	xor	ax, ax
check_done:
	pop	es
	ret

BEGIN	ENDP
	ENDPS

	end
