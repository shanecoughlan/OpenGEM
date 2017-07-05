/*******************************************************************/
/*                                                                 */
/*      Copyright 1999, Caldera Thin Clients, Inc.                 */
/*      This software is licenced under the GNU Public License.    */
/*      Please see LICENSE.TXT for further information.            */
/*                                                                 */
/*                 Historical Copyright                            */
/*******************************************************************/
/* Copyright (c) 1987 Digital Research Inc.		           */
/* The software contained in this listing is proprietary to        */
/* Digital Research Inc., Pacific Grove, California and is         */
/* covered by U.S. and other copyright protection.  Unauthorized   */
/* copying, adaptation, distribution, use or display is prohibited */
/* and may be subject to civil and criminal penalties.  Disclosure */
/* to others is prohibited.  For the terms and conditions of soft- */
/* ware code use refer to the appropriate Digital Research         */
/* license agreement.						   */
/*******************************************************************/

;/*	PROSTART.ASM	05/03/84 -  01/05/85	Lee Lorenzen		*/
;/*	added check for "GEMAES"    01/29/85	Tom Rolander		*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;/*                                                                     */
;/*     Copyright updated for FreeGEM  16/4/2000   Owen Rudge           */
;/*     Added Windows title support    18/11/2000  Owen Rudge           */

PGROUP	GROUP	PROG

DGROUP	GROUP	DATA,PROEND

DATA	SEGMENT PARA PUBLIC 'DATA'
	EXTRN	_VER:BYTE
	EXTRN	_PSP:WORD
PARABEG	dw	0
PARAEND	dw	0

wtitle  db      'FreeGEM Output',0

copyrt	db      0dh,0ah
        db	'*******************************************************',0dh,0ah
	db	'* FreeGEM Output                       Build 20010101 *',0dh,0ah
	db	'* Serial No.  XXXX-0000-654321    All Rights Reserved *',0dh,0ah
	db	'*                                                     *',0dh,0ah
	db	'* Copyright (c) 1987            Digital Research Inc. *',0dh,0ah
	db	'* Copyright (c) 1999       Caldera Thin Clients, Inc. *',0dh,0ah
	db	'* Copyright (c) 2000-2001                  Owen Rudge *',0dh,0ah
	db	'*******************************************************',0dh,0ah

gemaesmsg db	"GEMAES not present in memory."
	db	"$"
memerrmsg db	"Unable to free memory."
	db	"$"
terminmsg db	0dh,0ah
	db	"Execution terminated."
	db	"$"
DATA	ENDS

PROEND	SEGMENT PARA PUBLIC 'DATA'
;
	EXTRN	PROGEND:WORD
;
PROEND	ENDS
;

PROG	SEGMENT	BYTE PUBLIC 'PROG'

	ASSUME	CS:PGROUP
	ASSUME	DS:DGROUP

GLOBAMNT	equ	0
;
;
	EXTRN	GEMAIN:NEAR

	PUBLIC	PROMAIN
PROMAIN:
;
;	check for Lattice C 2.1x which already frees up memory

	mov	al,_VER+10
	cmp	al,'2'
	jg	afterfree
	jl	freemem
chknxtdig:
	mov	al,_VER+12
	cmp	al,'0'
	jg	afterfree
;
;	free up any memory we're not using
;		es points at our psp
;
freemem:
	mov	ax,_PSP+2
	mov	PARABEG,ax
	mov	ax,offset dgroup:PROGEND
	add	ax, GLOBAMNT
	add	ax,15
	mov	cl,4
	shr	ax,cl
	mov	cx,ax			; cx = our data size in paragraphs
	mov	bx,ds
	add	bx,cx
	mov	PARAEND,bx
;
;
free_ex:
	mov	ax,PARABEG
	mov	es,ax
	mov	bx,PARAEND
	sub	bx,ax
	mov	ah,4ah
	int	21h
	mov	dx,offset memerrmsg
	jc	errormsg		;jump if set block fails
;
;
afterfree:
;
; check for presence of GEMAES before calling the application
	xor	ax,ax
	mov	es,ax
	mov	bx,es:[3bch]
	mov	ax,es:[3beh]
	mov	es,ax
	mov	di,2
	add	di,bx
	mov	si,offset gemaesmsg
	mov	cx,6
	rep	cmpsb
	jz	runapplication
	mov	dx,offset gemaesmsg
errormsg:
	mov	ah,9
	int	21h
	mov	dx,offset terminmsg
	mov	ah,9
	int	21h
	mov	ax,-1
	jmp	terminate

runapplication:
        push    es                          ; code to set title if running under Windows
        mov     di,offset wtitle            ; to 'FreeGEM Output'
        mov     ax,seg wtitle
        mov     es,ax
        xor     dx,dx
        mov     ax,168Eh
        int     2Fh
        pop     es

	mov	ax,ds
	mov	es,ax
	call	GEMAIN

terminate:
	ret			;return to cs

PROG	ENDS
;
	END