

	.globl	large_data
	.globl	large_code
	.psect	ltext,local,class=CODE,reloc=16,size=65535
;
;************************************************
;	b_move(&from, count, &to);					*
;		&from:		ADDR	6[bp]				*
;		count:		AX             				*
;		&to:		ADDR	10[bp]				*
;************************************************
;
	.globl	_B_MOVE
	.signat	_B_MOVE,12344
;
_B_MOVE:
	push	bp
	mov		bp, sp
	push	si
	push	di

	mov		bx, ds
	mov		cx, ax
	les		di,10[bp]	; es:di = address of destination
	lds		si, 6[bp]	; ds:si = address of source
	rep 	movsb		; move bytes

; Restore and exit.
	mov		ds, bx

	pop		di
	pop		si
	pop		bp
	retf	#8	;returns 0 bytes

;
;***********************************************************
;MAGNIFY(src_mp, dst_mp, npix)
;expand bits to bytes from source to dest
;***********************************************************
;
	.globl	_MAGNIFY
	.signat	_MAGNIFY,12344
;
_MAGNIFY:
	push	bp
	mov		bp,sp
	push	si
	push	di

	mov		bx,#8000h
	mov		cx,14[bp]	;npix loaded to cx
	shr		cx,#1
	push	ds
	push	es
	lds		si, 6[bp]
	les		di,10[bp]
	mov		dx,[si]
	inc		si
	inc		si
magloop:
	xor		ax,ax
	test	dx,bx
	jz		hi0
	not		ah
;	dec		ah		;this is for white lines around fat pels
hi0:	
	ror		bx,#1
	test	dx,bx
	jz		low0
	not		al
;	dec		al
low0:
	stosw
	ror		bx,#1
	jc		next_source
	loop	magloop
	jmp		magdone
;
next_source:
	mov		dx,[si]
	inc		si
	inc		si
	loop	magloop
;
magdone:
	pop		es
	pop		ds

	pop		di
	pop		si
	pop		bp
	retf	#10
;


