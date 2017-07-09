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

; GEMOVL_A.ASM	misc overlay tools for use with PLINK86 and GEM.
; 06-14-85	Susan Bancroft
;
PGROUP	GROUP	PROG
DGROUP	GROUP	DATA

DATA	SEGMENT PARA PUBLIC 'DATA'

	EXTRN	_VER:BYTE

DATA	ENDS

PROG	SEGMENT	BYTE PUBLIC 'PROG'
	ASSUME	CS:PGROUP
	ASSUME	DS:DGROUP

	extrn	_ovlerr:near

	public  OvlErr

;***************************************************************************
;
; Disply dialog "can't find overlay... please insert disk yuk yuk.."
;
;***************************************************************************
OvlErr	proc	far
	push	bp		;save the callers bp
	mov	bp, sp
	push	ds
	push	es
	mov	ax, seg dgroup:_VER
	mov	ds, ax
	mov	es, ax
	call	_ovlerr
	pop	es
	pop	ds
	pop	bp
	ret
OvlErr	endp

;************************************************************************
;
PROG	ENDS


	end