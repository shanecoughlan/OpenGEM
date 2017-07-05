;****************************************************************************
;   Copyright 1999, Caldera Thin Client Systems, Inc.                       *
;   This software is licensed under the GNU Public License.                 *
;   See LICENSE.TXT for further information.                                *
;                                                                           *
;   Historical Copyright                                                    *
;                                                                           *
;   Copyright (c) 1985,1992  Digital Research Inc.  All rights reserved.    *
;   The Software Code contained in this listing is proprietary to Digital   *
;   Research Inc., Monterey, California, and is covered by U.S. and other   *
;   copyright protection.  Unauthorized copying, adaption, distribution,    *
;   use or display is prohibited and may be subject to civil and criminal   *
;   penalties.  Disclosure to others is prohibited.  For the terms and      *
;   conditions of software code use, refer to the appropriate Digital       *
;   Research License Agreement.						    *
;****************************************************************************
;		      U.S. GOVERNMENT RESTRICTED RIGHTS			    *
;                    ---------------------------------                      *
;  This software product is provided with RESTRICTED RIGHTS.  Use, 	    *
;  duplication or disclosure by the Government is subject to restrictions   *
;  as set forth in FAR 52.227-19 (c) (2) (June, 1987) when applicable or    *
;  the applicable provisions of the DOD FAR supplement 252.227-7013 	    *
;  subdivision (b)(3)(ii) (May 1981) or subdivision (c)(1)(ii) (May 1987).  *
;  Contractor/manufacturer is Digital Research Inc. / 70 Garden Court /     *
;  BOX DRI / Monterey, CA 93940.					    *
;****************************************************************************
; $Header: m:/davinci/users//groups/panther/dsk/rcs/ems.c 4.1 91/11/08 12:44:45 anderson Exp $
; $Log:	ems.c $
;  Revision 4.1  91/11/08  12:44:45  anderson
;  Interface to EMS driver.
; 
;****************************************************************************/
;
;****************************************************************
;*  See if an expanded memory manager is loaded by looking at
;*  the name field in the driver's device header.  Then check
;*  the EMS status to make sure hardware and driver are present
;*  and functional.
;****************************************************************/

	.psect	data,  class=DATA

NAME_LEN	equ		8
	
emmname:	.byte	'EMMXXXX0'
	
	.psect	_TEXT, class=CODE

GET_VECTOR	equ	0x35
EMM_INT		equ	0x67

EMM_STATUS		equ 0x40
EMM_PAGE_ADDR	equ 0x41
EMM_AVAIL		equ 0x42
EMM_ALLOC		equ 0x43
EMM_MAP			equ 0x44
EMM_FREE		equ 0x45
EMM_VERSION		equ 0x46
EMM_NUM_PAGES	equ 0x58

;
; int EMS_Check(void);
;
_EMS_Check:
	push	bp
	push	si
	push	di
	mov		ah,#GET_VECTOR
	mov		al,#EMM_INT
	int		#0x21			;Get EMS vector
	
	mov		di,#10
	mov		si,#emmname
	mov		cx,#NAME_LEN
chk1:
	mov		al,es:[di]
	cmp		al,[si]
	jnz		rfalse
	inc		di
	inc		si
	loop	chk1
	
	mov		ah,#EMM_STATUS
	int		#EMM_INT

	or		ah,ah
	jnz		rfalse

rtrue:
	mov		ax,#1
	pop		di
	pop		si   
	pop		bp
	ret 
	
rfalse:
	xor		ax,ax
	pop		di
	pop		si
	pop		bp
	ret

;
; Return the number of 16k EMS pages available
;	
; WORD	EMS_Avail(VOID)

	.globl	_EMS_Avail

_EMS_Avail:
	push	bp
	push	si
	push	di
	mov		ah,#EMM_AVAIL
	int		#EMM_INT
	or		ah,ah
	jnz		rfalse
	pop		di
	pop		si
	pop		bp
	mov		ax,bx
	ret


;/****************************************************************
; *  Make sure EMM driver is loaded & ready, and hardware is OK.
; *  Get the EMM version number, the number of logical EMS pages
; *  available, and the address of the page frame.
; ****************************************************************/

 	.globl	_EMS_Init

_EMS_Init:
	push	bp
	mov		bp,sp
	
	push	si
	push	di

	mov		si,dx	;1st parameter
	mov		di,ax	;2nd parameter

	call	_EMS_Check
	
	or		ax,ax
	jz		rfalse

	mov		ah,#EMM_VERSION
	int		#EMM_INT
	or		ah,ah
	jnz		rfalse
	mov		[si],al		;EMM version
	
	mov		ah,#EMM_PAGE_ADDR
	int		#EMM_INT
	or		ah,ah
	jnz		rfalse
	mov		[di],bx

	mov		ah,#EMM_NUM_PAGES
	mov		al,#1
 	int		#EMM_INT
 	or		ah,ah
 	jnz		rfalse
	mov		si,4[bp]	;Parameter 3
	mov		[si],cx
	cmp		cx,#0
	jz		rfalse

	call	_EMS_Avail
	mov		di,6[bp]	;Parameter 4
	mov		[di],ax
	or		ax,ax
	jz		rfalse
 	jmp		rtrue
    
;/****************************************************************
; *  Allocate the requested number of 16Kb logical pages of EMS
; *  memory.  These will be assigned to a single handle if they 
; *  can be allocated successfully.  Function returns handle #.
; ****************************************************************/

	.globl	_EMS_Alloc

_EMS_Alloc:
	push	bp
	push	si
	push	di
	mov		bx,dx	;No. of pages
	mov		ah,#EMM_ALLOC
	int		#EMM_INT
	or		ah,ah
	jz		ema1
	mov		dx,#0
ema1:
	mov		ax,dx
	pop		di
	pop		si
	pop		bp
	ret

;/****************************************************************
; *  Map the requested logical EMS pages onto physical memory.
; ****************************************************************/
	.globl	_EMS_Map

_EMS_Map:
	push	bp
	mov		bp,sp
	push	si
	push	di
;	mov		dx,dx		;DX = handle - already in parameter 1
;	mov		al,al		;AL = ppage  - already in parameter 2
	mov		bx,4[bp]	;BX = lpage
	mov		ah,#EMM_MAP
	int		#EMM_INT	
	or		ah,ah
	brz		rtrue2
rfalse2:
	xor		ax,ax
	pop		di
	pop		si   
	pop		bp
	ret 
	
rtrue2:
	mov		ax,#1
	pop		di
	pop		si
	pop		bp
	ret		#2			;9-4-2000: RET #2 rather than RET; this is
						;using Pacific calling convention so the
						;called routine tidies the stack


;/****************************************************************
; *  Deallocate the EMS pages assigned to a handle and release the
; *  handle.
; ****************************************************************/

	.globl	_EMS_Free
_EMS_Free:
;
; Parameter 1 is already in DX - how convenient
;
	push	bp
	push	si
	push	di
	mov		ah,#EMM_FREE
	int		#EMM_INT
	pop		di
	pop		si
	pop		bp
	ret


