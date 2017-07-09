;/*	VDIASM.ASM	6/26/84 - 10/16/84	Don Heiskell	*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;
PGROUP	GROUP	PROG

DGROUP	GROUP	DATA

DATA	SEGMENT PARA PUBLIC 'DATA'
	extrn	contrl:word
	extrn	ptsin:word
	extrn	intin:word
	extrn	intout:word
	extrn	ptsout:word

pblock	dw	offset	contrl
	dw	seg	contrl
intioff	dw	offset	intin
	dw	seg	contrl
ptsioff	dw	offset	ptsin
	dw	seg	contrl
intooff	dw	offset	intout
	dw	seg	contrl
ptsooff	dw	offset	ptsout
	dw	seg	contrl
DATA	ENDS

PROG	SEGMENT	BYTE PUBLIC 'PROG'
	ASSUME	CS:PGROUP
	ASSUME	DS:DGROUP

	public	vdi, gemcheck
	public	i_ptsin, i_intin, i_intout, i_ptsout
	public	i_ptr, i_ptr2, i_lptr1, m_lptr2

VDI_INT		equ	0efh

GEM_INT		equ	0efh
GEM_VECTOR	equ	4*GEM_INT

vdi:
	mov	cx,473h
	mov	dx,offset	pblock		;point at the parm block
	int	VDI_INT
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

i_ptsin:
	push	bp
	mov	bp,sp
	mov	ax,04[bp]
	mov	ptsioff,ax
	pop	bp
	ret

i_intin:
	push	bp
	mov	bp,sp
	mov	ax,04[bp]
	mov	intioff,ax
	pop	bp
	ret

i_ptsout:
	push	bp
	mov	bp,sp
	mov	ax,04[bp]
	mov	ptsooff,ax
	pop	bp
	ret

i_intout:
	push	bp
	mov	bp,sp
	mov	ax,04[bp]
	mov	intooff,ax
	pop	bp
	ret

i_ptr:
	push	bp
	mov	bp,sp
	mov	ax,04[bp]
	mov	contrl+14,ax
	mov	ax,ds
	mov	contrl+16,ax
	pop	bp
	ret

i_ptr2:
	push	bp
	mov	bp,sp
	mov	ax,04[bp]
	mov	contrl+18,ax
	mov	ax,ds
	mov	contrl+20,ax
	pop	bp
	ret

i_lptr1:
	push	bp
	mov	bp,sp
	mov	ax,04[bp]
	mov	contrl+14,ax
	mov	ax,06[bp]
	and	ax,ax
	jnz	i_lptr11
	mov	ax,cs

i_lptr11:
	mov	contrl+16,ax
	pop	bp
	ret

m_lptr2:
	push	bp
	mov	bp,sp
	mov	bx,04[bp]
	mov	ax,contrl+18
	mov	[bx],ax
	inc	bx
	inc	bx
	mov	ax,contrl+20
	mov	[bx],ax
	pop	bp
	ret

PROG	ENDS

	end
