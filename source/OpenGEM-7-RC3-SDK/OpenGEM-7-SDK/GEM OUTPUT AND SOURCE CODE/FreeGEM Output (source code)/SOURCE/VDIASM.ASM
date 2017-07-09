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

;/*	VDIASM.ASM	6/26/84 - 10/16/84	Don Heiskell	*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;
PGROUP	GROUP	PROG

DGROUP	GROUP	DATA

DATA	SEGMENT PARA PUBLIC 'DATA'

	public  pblock 

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

	public	vdi, gsx1, gsx2, gemcheck

VDI_INT		equ	0efh

GEM_INT		equ	0efh
GEM_VECTOR	equ	4*GEM_INT

gsx1:
gsx2:
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


PROG	ENDS

	end