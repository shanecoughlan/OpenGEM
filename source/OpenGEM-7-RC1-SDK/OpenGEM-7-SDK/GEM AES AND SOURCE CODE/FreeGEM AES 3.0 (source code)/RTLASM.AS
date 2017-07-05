;       Copyright 1999, Caldera Thin Clients, Inc.                      
;       This software is licenced under the GNU Public License.         
;       Please see LICENSE.TXT for further information.                 
;                                                                       

	.psect	_TEXT,class=CODE
	.globl	_mwmpyl
	.globl	_mwdivl
	.globl	_mwudivl
	.globl	amull,adivl


_mwmpyl:
		push	bp
		mov		bp, sp
		push	dx		;Parameter 2	
;;		mov		ax, 4[bp]
		push	ax		;Parameter 1
		mul		8[bp],word
		mov		bx, ax
		mov		cx, dx
;;		mov		ax, 4[bp]
		pop		ax		;Parameter 1
		mul		6[bp],word
		add		cx, ax
		pop		ax		;Parameter 2
		mul		4[bp],word
		add		cx, ax
		mov		ax, bx
		mov		dx, cx
		pop		bp
		ret		#4

_mwdivl:
		push	bp
		mov		bp, sp
		sub		sp,#6
		mov		-4[bp],dx	;Parameter 2
		mov		-6[bp],ax	;Parameter 1
		mov		dx, 0Ah[bp]
		mov		-2[bp], dx
	
		and		dx, dx
		jnl		pos_arg2
		sub		dx, dx
		mov		ax, 4[bp]
		neg		ax
		sbb		dx, 6[bp]
		push	dx
		push	ax
		jmp		chk_arg1

pos_arg2:	
		push	dx
		push	4[bp]

chk_arg1:	
		mov		dx, -4[bp]
		xor		-2[bp], dx
		and		dx, dx
		jnl		pos_arg1
		sub		dx, dx
		mov		ax, -6[bp]
		neg		ax
		sbb		dx, -4[bp]
;;		push	dx			;Use PPD calling conventions and pass in
;;		push	ax			;DX and AX
		jmp		do_divide

pos_arg1:
;;		push	dx
		mov		ax,-6[bp]

do_divide:
		call	_mwudivl
		test	0-2[bp],#0x8000,word
		jz		adj_sign
		sub		cx, cx
		neg		ax
		sbb		cx, dx
		mov		dx, cx

adj_sign:	
		test	-4[bp],#0x8000,word
		jz		divl_exit
		sub		cx, cx
		neg		bx
; ???
		sbb		cx, si
		mov		si, cx

divl_exit:	
		mov	sp, bp
		pop	bp
		ret	#8



_mwudivl:
		push	bp
		mov		bp, sp
		push	si
		push	di

;;		mov		ax, 4[bp]
;;		mov		dx, 6[bp]
		sub		si, si
		mov		bx, si
		mov		cx, #32

udiv_loop:	
		jcxz	udiv_done
		dec		cx
		clc	
		rcl		ax, #1
		rcl		dx, #1
		rcl		bx, #1
		rcl		si, #1
		cmp		si, 6[bp]
		jb		udiv_loop
		ja		udiv_adj_down
		cmp		bx, 4[bp]
		jb		udiv_loop

udiv_adj_down:	
		sub	bx, 4[bp]
		sbb	si, 6[bp]
		inc		ax
		jmp		udiv_loop

udiv_done:	
		pop		di
		pop		si
		pop		bp
		ret		#8
