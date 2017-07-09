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

;/*	LONGASM.ASM	2/3/84 - 10/09/84	Lee Jay Lorenzen	*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;/*     for Lattice C ==>  AX=HI, BX=LO
;
include model.h

PGROUP	GROUP	PROG

DGROUP	GROUP	DATA

DATA	SEGMENT PARA PUBLIC 'DATA'
DATA	ENDS

PROG	SEGMENT	BYTE PUBLIC 'PROG'
	ASSUME	CS:PGROUP
	ASSUME	DS:DGROUP
;
	PUBLIC	LSTRLEN
	PUBLIC	ADDR
	PUBLIC	LLDS
	PUBLIC	LLCS
	PUBLIC	LLSS 
	PUBLIC	LWCOPY
	PUBLIC	LBCOPY
	PUBLIC	LBGET
	PUBLIC	LBSET
	PUBLIC	LWGET
	PUBLIC	LWSET
	PUBLIC	LLGET
	PUBLIC	LLSET
	PUBLIC	LBWMOV
;
;		WORD
;	LSTRLEN(long_addr)
;
LSTRLEN:
	push	bp
	mov	bp,sp
	push	ds
	mov	bx,4[bp]
	mov	ax,6[bp]
	mov	ds,ax
	sub	cx,cx
slagain:
	cmp	byte ptr [bx],0
	jz	sldone
	inc	bx
	jmp	slagain
sldone:
	mov	ax,bx
	mov	bx,4[bp]
	sub	ax,bx
	pop	ds
	pop	bp
	ret
;
;		LONG
;	ADDR(shrt_addr)
;
ADDR:
	push	bp
	mov	bp,sp
	mov	bx,4[bp]
ifdef SMALL_DATA
	mov	ax,ds
endif
ifdef LARGE_DATA
	mov 	ax,6[bp]
endif
	pop	bp
	ret
;
;		LONG
;	LLDS()
;
;
LLDS:
	push bp
	mov bp,sp
	sub bx,bx
	mov ax,ds
	pop bp
	ret
;
;		LONG
;	LLCS()
;
;
LLCS:
	push bp
	mov bp,sp
	sub bx,bx
	mov ax,cs
	pop bp
	ret
;
;		LONG
;	LLSS()
;
;
LLSS:
	push bp
	mov bp,sp
	sub bx,bx
	mov ax,ss
	pop bp
	ret
;
;	LWCOPY(pdstoff, pdstseg, psrcoff, psrcseg, numwds)
;
;
LWCOPY:
	push bp
	mov bp,sp
	push di
	push si
	mov di,4[bp]		; dest off
	mov bx,6[bp]		; dest seg
	mov si,8[bp]		; src off
	mov dx,10[bp]		; src seg
	mov cx,12[bp]		; count
	cld			; assume forward
	cmp bx,dx		; if !same seg
	jne lwcopy_mov		;     then dont worry about overlap
	cmp si,di		; if src offs < dest offs
				;     then start from end and go back
				; else normal
	ja lwcopy_mov
	  mov ax,cx
	  shl ax,1
	  add si,ax
	  sub si,2
	  add di,ax
	  sub di,2
	  std			;  go backwards
lwcopy_mov:
	push ds
	push es
	mov ds,dx		; src
	mov es,bx		; dest
	rep movsw
	pop es
	pop ds
	

	lea sp,-4[bp]
	pop si
	pop di
	pop bp
	cld
	ret
;
;
;	LBCOPY(pdstoff, pdstseg, psrcoff, psrcseg, numbytes)
;
;
LBCOPY:
	push bp
	mov bp,sp
	push di
	push si
	mov di,4[bp]		; dest off
	mov bx,6[bp]		; dest seg
	mov si,8[bp]		; src off
	mov dx,10[bp]		; src seg
	mov cx,12[bp]		; count
	cld			; assume forward
	cmp bx,dx		; if !same seg
	jne lbcopy_mov		;     then dont worry about overlap
	cmp si,di		; if src offs < dest offs
				;     then start from end and go back
				; else normal
	ja lbcopy_mov
	  add si,cx
	  dec si
	  add di,cx
	  dec di
	  std			;  go backwards
lbcopy_mov:
	push ds
	push es
	mov ds,dx		; src
	mov es,bx		; dest
	rep movsb
	pop es
	pop ds
	

	lea sp,-4[bp]
	pop si
	pop di
	pop bp
	cld
	ret
;
;	expand from bytes to words until you hit a null also return
;	the number of characters moved
;
;	LBWMOV(pdstoff, pdstseg, psrcoff, psrcseg)
;
;
LBWMOV:
	push bp
	mov bp,sp
	push di
	push si
	mov di,4[bp]		; dest off
	mov bx,6[bp]		; dest seg
	mov si,8[bp]		; src off
	mov dx,10[bp]		; src seg
	mov cx,0		; count
	cld			; assume forward
lbw_mov:
	push ds
	push es
	mov ds,dx		; src
	mov es,bx		; dest
	mov ah,0
lbw_loop:
	lodsb
	cmp al,0
	je lbw_done
	inc cx
	stosw
	jmp lbw_loop
lbw_done:
	pop es
	pop ds
	

	lea sp,-4[bp]
	pop si
	pop di
	pop bp
	cld
	mov	ax,cx
	ret
;
;		UBYTE
;	LBGET(lp)
;		LONG		p;
;
LBGET:
	push	bp
	mov	bp,sp
	push	es
	les	bx,DWORD PTR 4[bp]
	sub	ax,ax
	mov	al,es:[bx]
	pop	es
	pop	bp
	ret
;
;		UBYTE
;	LBSET(lp, abyte)
;		LONG		p;
;		BYTE		abyte;
;
LBSET:
	push	bp
	mov	bp,sp
	push	es
	les	bx,DWORD PTR 4[bp]
	mov	ax,8[bp]
	mov	es:[bx],al
	xor	ah,ah
	pop	es
	pop	bp
	ret
;
;		UWORD
;	LWGET(lp)
;		LONG		p;
;
LWGET:
	push	bp
	mov	bp,sp
	push	es
	les	bx,DWORD PTR 4[bp]
	mov	ax,es:[bx]
	pop	es
	pop	bp
	ret
;
;		UWORD
;	LWSET(lp, aword)
;		LONG		p;
;		WORD		aword;
;
LWSET:
	push	bp
	mov	bp,sp
	push	es
	les	bx,DWORD PTR 4[bp]
	mov	ax,8[bp]
	mov	es:[bx],ax
	pop	es
	pop	bp
	ret
;
;		ULONG
;	LLGET(lp)
;		LONG		p;
;
LLGET:
	push	bp
	mov	bp,sp
	push	es
	les	bx,DWORD PTR 4[bp]
	mov	ax,es:[bx]
	inc	bx
	inc	bx
	mov	bx,es:[bx]
	xchg	ax,bx
	pop	es
	pop	bp
	ret
;
;		ULONG
;	LLSET(lp, along)
;		LONG		p;
;		LONG		along;
;
LLSET:
	push	bp
	mov	bp,sp
	push	es
	les	bx,DWORD PTR 4[bp]
	mov	ax,8[bp]
	mov	es:[bx],ax
	inc	bx
	inc	bx
	mov	cx,10[bp]
	mov	es:[bx],cx
	xchg	ax,cx
	pop	es
	pop	bp
	ret
 
PROG	ENDS

	end