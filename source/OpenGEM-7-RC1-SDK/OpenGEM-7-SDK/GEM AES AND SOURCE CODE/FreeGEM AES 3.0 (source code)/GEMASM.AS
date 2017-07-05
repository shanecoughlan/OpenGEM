;/*	LDISPA88.A86	1/28/84 - 01/18/85	Lee Jay Lorenzen	*/
;/*	for Lattice C		*/
;
;       Copyright 1999, Caldera Thin Clients, Inc.                      
;       This software is licenced under the GNU Public License.         
;       Please see LICENSE.TXT for further information.                 
;                                                                       
;                  Historical Copyright                                 
;/*
;*	-------------------------------------------------------------
;*	GEM Application Environment Services		  Version 2.3
;*	Serial No.  XXXX-0000-654321		  All Rights Reserved
;*	Copyright (C) 1986			Digital Research Inc.
;*	-------------------------------------------------------------
;*/
;
;

; Pacific C register calling conventions 
; (note: ONLY for correctly prototyped functions)
;
; WORDs returned in AX
; LONGs returned in DXAX (or DX:AX if long address)
;
; If the first parameter fits in 16 bits, it is passed in DX.
; Similarly if the second fits in 16 bits, it is passed in AX.
;
; BP, SI and DI must be preserved.
;
; Prototyped functions clear the stack themselves rather than having
; the caller do it.
;


;
; XXX to be rewritten properly for Pacific
;
	.psect	_TEXT,class=CODE

	.globl	_savestat
	.globl	_switchto
	.globl	_gotopgm
	.globl	_dsptch
	.globl	_usergo
	.globl	_psetup
	.globl  _dspstk
	.globl	_disp

	.psect	data,class=DATA
	.globl	_rlr
	.globl	_indisp
	.psect	_TEXT,class=CODE
;
;	Push flags and code pointer onto a processes stack in preparation
;	for an IRET that will start this process executing
;
;	--->	ip
;		cs
;		flags
;	       =======
;
;		VOID
;	psetup(p, codevalue)
;		PD		*p;
;		CODE		*codevalue;
;
_psetup:
	push	bp
	mov		bp,sp
	push	dx				; DX = Parameter 1
	push	ax				; AX = Parameter 2
	mov		bx,dx			; bx = *pd
	mov		bx,4[bx]		; ds:bx = pd->uda
	mov		bx,2[bx]		; ds:bx = uda->SUPSTK
	sub		bx,#2			; predecrement
	pushf					; use current flags
	pop		ax				; push flags first
	mov		[bx],ax			; store word on stack
	sub		bx,#2			; predecrement
	mov		ax,cs			; push code segment next
	mov		[bx],ax			; store hi-word on stack
	sub		bx,#2			; predecrecment
	pop		ax				; Parameter 2; push code offset last
	mov		[bx],ax			; store lo-word on stack
	mov		ax,bx
	pop		bx				; Parameter 1
	mov		bx,4[bx]
	mov		2[bx],ax 		; restore stack pointer
	pop		bp
	ret
;
_dsptch:
	test	_indisp,#0ffh,byte
	jz		dsp1
	ret
dsp1:
	sub		sp,#4		; leave 1 extra word
	push	bp
	pushf
	mov		bp,sp
	push	ax
	mov		ax,8[bp]
	mov		4[bp],ax		; ret addr
	mov		ax,cs
	mov		6[bp],ax		; cs 
	mov		ax,[bp]
	mov		8[bp],ax		; flags
	pop		ax
	popf
	pop		bp
	push	ds
	push	es
	push	bx		; dispatch code will crunch these regs
	push	dx
	push	si
	jmp		_disp
;
;
;
; usergo(addr) -- execute in user mode
;
_usergo:
	mov	bp,sp
	lds	di,2[bp]		;long parameter so on stack
	mov	bx,#seg(_rlr)
	mov	es,bx
	mov	bx,es:_rlr
	les	bx,es:4[bx]
	cli
	mov	es:[bx],#1,word
	mov	es:2[bx],sp
	mov	es:4[bx],ss
	mov	sp,es:6[bx]
	mov	ss,es:8[bx]
	sti	
	jmpf	[di]
;
;
;
;
_gotopgm:
	mov		bp,sp
	mov		bx,#seg _rlr
	mov		es,bx
	mov		bx,es:_rlr
	mov		bx,es:04h[bx]		; uda
	cli
	mov		es:[bx],#0,word	; resume user stack
	mov		es:2[bx],sp
	mov		es:4[bx],ss
	sti
	mov		bx,es:_rlr
	push	es:18[bx]	; p_ldaddr segment 
	push	es:16[bx]	; p_ldaddr offset
	retf
;
;
; this routine is entered by a call from dsptch, 
;       which is entered from an interrupt
; 	handler whose regs have all been restored after processing.
;
; therefore, the stack has the following items (after the link instruction):
;
;			+18	flags of interrupted code (short)
;	saved sp->	+14	return address of interrupted code (long)
;			+12     ds of interrupted code
;			+8	es:bx of interrupted code
;			+6	si
; 	 		+4	bp of interrupted code (push bp/sub sp,4)
;			+2	/* dispatcher local storage */
;	dbp->		+0	/* dispatcher local storage */
;
;			+4	uda address to store regs
;			+2	return address in dispatcher
; 	bp->		+0	dispatcher bp (dbp)
;
; the dispatcher will run on the current super stack,
; 	at context switch, it will call switchto which will end with an rte
;
;
; but for metaware, the stack has different arrangement:
;
;			+14	flags of interrupted code (short)
;	saved sp->	+10	return address of interrupted code (long)
;			+8      ds of interrupted code
;			+4	es:bx of interrupted code
;			+2	si
; 	dbp-> 		+0	bp of interrupted code (push bp/sub sp,4)
;
;			+6	/* dispatcher local storage */
;			+4	uda address to store regs
;			+2	return address in dispatcher
; 	bp->		+0	dispatcher bp (dbp)
;

;
; In Pacific, we have:
;
; There is no stack frame for disp().
;
;		 +12 14[bp] flags
;		 +10 12[bp] cs
;		  +E 10[bp] ip
;		  +C 0e[bp] ds
;		  +A 0c[bp] es
;		  +8 0a[bp] bx
;		  +6 08[bp] dx
;         +4 06[bp] si
;         +2 04[bp] saved di from disp(), next routine up
;	sp -> +0 02[bp] return from savestat()
;		bp+0 00[bp] saved bp
;

_savestat:
	cli							; disable ints (blech)
	test	_indisp,#0ffh,byte	; no TAS anymore
	jz		okgo				; no one's inside, go for it

;
; Pop the stack back, since there are no frames on it
;
	pop		di				;0 Return address from savestat()
	pop		di				;2 DI
	pop		si				;4 Return address from disp()
	pop		si				;6 SI
	pop		dx				;8 DX
	pop		bx				;A BX
	pop		es				;C es
	pop		ds				;E ds
	iret

;	mov		sp,bp			;Original code
;	pop		bp
;	iret
;

;
okgo:
	inc		_indisp,byte		; TAS (sortof)
	sti
	push	bp
	mov		bp,sp
	push	ds
	mov		bx,dx   		; DX = parameter1, uda address
	mov		4[bx],ss

;;;	mov		24[bx],di		; Pacific uses these
;;;	mov		20[bx],dx

	mov		22[bx],si		; Save SI, it wasn't clobbered

	mov		14[bx],ax
	mov		18[bx],cx


	mov		si,ss
	mov		es,si			; es:bp now dispatcher area

	mov		ax,es:14[bp]	;ds
	mov		10[bx],ax

	mov		ax,es:12[bp]	;es
	mov		12[bx],ax

	mov		ax,es:10[bp]	;bx
	mov		16[bx],ax

	mov		ax,es:8[bp]		;dx
	mov		20[bx],ax

	mov		ax,es:4[bp]		;di
	mov		24[bx],ax		

	mov		ax,es:0[bp]		;bp
	mov		26[bx],ax

	lea		di,16[bp]		;-> saved ip
	mov		2[bx],di

	mov		si,bp			;si -> saved bp; savestat() return

; load a big dispatcher stack (for forks, etc.)

	cli
	mov		ax,#seg _dspstk
	mov		ss,ax
	mov		sp,#_dspstk - 4 	; 4 bytes for local storage and
	mov		bp,#_dspstk 	;   uda address
	sti
	push	es:2[si]	; return address		
	ret
;
;
_switchto:
	add		sp,#2		; trash the ret. add
	cli
	dec		_indisp,byte
	mov		si,dx		; SI = parameter 1 
	mov		ax,14[si]
	mov		cx,18[si]
	mov		dx,20[si]
	mov		bx,16[si]
	mov		bp,26[si]
	mov		di,24[si]
	mov		es,12[si]
	mov		sp,2[si]
	mov		ss,4[si]
	sti
	push	10[si]		; ds
	mov		si,22[si]
	pop		ds
	iret			; no, really ???
;
	.psect	data,class=DATA
	.globl	_dspstk
	.blkb	384
_dspstk:
	.word	0
;


