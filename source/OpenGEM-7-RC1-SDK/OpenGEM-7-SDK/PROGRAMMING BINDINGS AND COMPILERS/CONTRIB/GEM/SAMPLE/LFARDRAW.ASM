
;
;/*	FARDRAW.ASM	3/22/85 	Tim Oren	*/
;
;	modified for Large model    9/18/84  C. Shaw     */             
	INCLUDE LARGE.MAC

	EXTRN	DR_CODE:FAR

	PSEG
;

BEGIN	PROC	FAR		;" procname proc far" makes ret = retf
;
drawsp	dw 0
drawss	dw 0
;
;	far_draw()
;		ax = hi part of long pointer to PARMBLK
;		bx = lo part of long pointer to PARMBLK
;
;	need to save the regs, get on a local stack, and call
;	the dr_code() routine with the parameters pointed at
;	in PARMBLK
;
far_draw:
	push	bp
	mov	bp,sp
	push	ds
	push	es
	push	si
	push	di
	mov	cx,ax		; remember hi ptr to parm blk
	mov	ax,ss
	mov	drawss,ax
	mov	ax,sp
	mov	drawsp,ax
	cli
	mov	ax,seg drawstk
	mov	ss,ax
	mov	ds,ax
	mov	es,ax
	mov	sp,offset drawstk
	sti
	push	cx		; push hi ptr to parmblk
	push	bx		; push lo ptr to parmblk
	call	dr_code 	; state = dr_code((LONG)pparmblk)
	add	sp,4
	mov	bx,ax		; remember state
	cli
	mov	ax,drawss
	mov	ss,ax
	mov	ax,drawsp
	mov	sp,ax
	sti
	pop	di
	pop	si
	pop	es
	pop	ds
	mov	ax,bx		; restore state
	pop	bp
	ret

BEGIN	ENDP
	ENDPS
;
	DSEG
	PUBLIC	DRAWADDR

DRAWADDR   dw	offset far_draw
drawseg    dw	seg far_draw
	   dw	256 dup(?)	; dup(?) = uninitialized
drawstk    dw	0
;
;
	ENDDS
	END
