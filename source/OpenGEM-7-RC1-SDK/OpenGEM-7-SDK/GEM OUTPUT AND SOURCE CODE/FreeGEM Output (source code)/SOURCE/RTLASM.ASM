;/*******************************************************************/
;/*                                                                 */
;/*      Copyright 1999, Caldera Thin Clients, Inc.                 */
;/*      This software is licenced under the GNU Public License.    */
;/*      Please see LICENSE.TXT for further information.            */
;/*                                                                 */
;/*                 Historical Copyright                            */
;/*******************************************************************/
;/* Copyright (c) 1987 Digital Research Inc.		           */
;/* The software contained in this listing is proprietary to        */
;/* Digital Research Inc., Pacific Grove, California and is         */
;/* covered by U.S. and other copyright protection.  Unauthorized   */
;/* copying, adaptation, distribution, use or display is prohibited */
;/* and may be subject to civil and criminal penalties.  Disclosure */
;/* to others is prohibited.  For the terms and conditions of soft- */
;/* ware code use refer to the appropriate Digital Research         */
;/* license agreement.						   */
;/*******************************************************************/

;/* Extracted from High-C SMALL model library HCSE.LIB			*/

PGROUP	GROUP	PROG

PROG	SEGMENT	BYTE PUBLIC 'PROG'
	ASSUME	CS:PGROUP

	PUBLIC		_mwmpyl
	PUBLIC		_mwdivl
	PUBLIC		_mwudivl

_mwmpyl:
		push	bp
		mov	bp, sp
		mov	ax, 4[bp]
		mul	word ptr 08[bp]
		mov	bx, ax
		mov	cx, dx
		mov	ax, 4[bp]
		mul	word ptr 0Ah[bp]
		add	cx, ax
		mov	ax, 6[bp]
		mul	word ptr 8[bp]
		add	cx, ax
		mov	ax, bx
		mov	dx, cx
		pop	bp
		ret	8

_mwdivl:
		push	bp
		mov	bp, sp
		sub	sp, 2
		mov	dx, 0Ah[bp]
		mov	-2[bp], dx
		and	dx, dx
		jnl	pos_arg2
		sub	dx, dx
		mov	ax, 8[bp]
		neg	ax
		sbb	dx, 0Ah[bp]
		push	dx
		push	ax
		jmp	chk_arg1

pos_arg2:	push	dx
		push	word ptr 8[bp]

chk_arg1:	mov	dx, 6[bp]
		xor	-2[bp], dx
		and	dx, dx
		jnl	pos_arg1
		sub	dx, dx
		mov	ax, 4[bp]
		neg	ax
		sbb	dx, 6[bp]
		push	dx
		push	ax
		jmp	do_divide

pos_arg1:	push	dx
		push	word ptr 4[bp]

do_divide:	call	_mwudivl
		test	word ptr 0FFFEh[bp], 08000H
		jz	adj_sign
		sub	cx, cx
		neg	ax
		sbb	cx, dx
		mov	dx, cx

adj_sign:	test	word ptr 6[bp], 08000H
		jz	divl_exit
		sub	cx, cx
		neg	bx
		sbb	cx, si
		mov	si, cx

divl_exit:	mov	sp, bp
		pop	bp
		ret	8

_mwudivl:
		push	bp
		mov	bp, sp
		mov	ax, 4[bp]
		mov	dx, 6[bp]
		sub	si, si
		mov	bx, si
		mov	cx, 32

udiv_loop:	jcxz	udiv_done
		dec	cx
		clc	
		rcl	ax, 1
		rcl	dx, 1
		rcl	bx, 1
		rcl	si, 1
		cmp	si, 0Ah[bp]
		jb	udiv_loop
		ja	udiv_adj_down
		cmp	bx, 8[bp]
		jb	udiv_loop

udiv_adj_down:	sub	bx, 8[bp]
		sbb	si, 0Ah[bp]
		inc	ax
		jmp	udiv_loop

udiv_done:	pop	bp
		ret	8

PROG	ENDS

end