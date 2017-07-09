;/*******************************************************************/
;/*                                                                 */
;/*      Copyright 1999, Caldera Thin Clients, Inc.                 */
;/*      This software is licenced under the GNU Public License.    */
;/*      Please see LICENSE.TXT for further information.            */
;/*                                                                 */
;/*                 Historical Copyright                            */
;/*******************************************************************/
;/* Copyright (c) 1987 Digital Research Inc.		            */
;/* The software contained in this listing is proprietary to        */
;/* Digital Research Inc., Pacific Grove, California and is         */
;/* covered by U.S. and other copyright protection.  Unauthorized   */
;/* copying, adaptation, distribution, use or display is prohibited */
;/* and may be subject to civil and criminal penalties.  Disclosure */
;/* to others is prohibited.  For the terms and conditions of soft- */
;/* ware code use refer to the appropriate Digital Research         */
;/* license agreement.						    */
;/*******************************************************************/

;/*	DOSASM.ASM	4/18/84 - 10/24/84	Lee Jay Lorenzen	*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;
;
PGROUP	GROUP	PROG

DGROUP	GROUP	DATA

DATA	SEGMENT PARA PUBLIC 'DATA'
	EXTRN	DOS_AX:WORD, DOS_BX:WORD, DOS_CX:WORD, DOS_DX:WORD
	EXTRN	DOS_DS:WORD, DOS_ES:WORD, DOS_SI:WORD, DOS_DI:WORD
	EXTRN	DOS_ERR:WORD
DATA	ENDS

PROG	SEGMENT	BYTE PUBLIC 'PROG'
	ASSUME	CS:PGROUP
	ASSUME	DS:DGROUP
;
	PUBLIC	__DOS
;
savax	dw	0
savds	dw	0
savss	dw	0
savsp	dw	0
;
;
__DOS:
	push	bp
	mov	bp,sp
	push	es
	push	si
	push	di
	mov	ax,DOS_DI
	mov	di,ax
	mov	ax,DOS_SI
	mov	si,ax
	mov	ax,DOS_ES
	mov	es,ax
	mov	ax,DOS_AX
	mov	bx,DOS_BX
	mov	cx,DOS_CX
	mov	dx,DOS_DX
	mov	savax,ax
	push	ds
	mov	ax,DOS_DS
	mov	ds,ax
	mov	ax,savax
	int	21h
	mov	savax,ax
	mov	ax,ds
	mov	savds,ax
	pop	ds
	mov	ax,savax
	jc	d_error
	mov	DOS_ERR,0
	jmp	d_exit
d_error:
	mov	DOS_ERR,1
d_exit:
	mov	DOS_AX,ax
	mov	DOS_BX,bx
	mov	DOS_CX,cx
	mov	DOS_DX,dx
	mov	ax,savds
	mov	DOS_DS,ax
	mov	ax,es
	mov	DOS_ES,ax
	mov	ax,si
	mov	DOS_SI,ax
	mov	ax,di
	mov	DOS_DI,ax
	pop	di
	pop	si
	pop	es
	pop	bp
	ret
;
PROG	ENDS

	end