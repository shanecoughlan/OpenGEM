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

;/*	FARDRAW.ASM	3/22/85		Tim Oren	*/
;
PGROUP	GROUP	PROG
;
DGROUP	GROUP	DATA
;
PROG	SEGMENT	BYTE PUBLIC 'PROG'
BEGIN   PROC    FAR             ;" procname proc far" makes ret = retf
	ASSUME	CS:PGROUP
	EXTRN	DR_CODE:NEAR
;
drawsp  dw 0
drawss  dw 0
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
	call	dr_code		; state = dr_code((LONG)pparmblk)
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

BEGIN   ENDP
PROG	ENDS	
;
DATA	SEGMENT	PARA PUBLIC 'DATA'
	PUBLIC	DRAWADDR

DRAWADDR   dw	offset pgroup:far_draw
drawseg    dw	seg pgroup:far_draw
	   dw	256 dup(?)      ; dup(?) = uninitialized 
drawstk	   dw	0
;
;
DATA	ENDS
	END