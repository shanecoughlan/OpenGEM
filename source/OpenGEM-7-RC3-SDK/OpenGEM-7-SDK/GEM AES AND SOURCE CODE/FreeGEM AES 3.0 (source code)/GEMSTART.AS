;/*	GEMSTART.A86		4/18/84 - 06/07/85	Lee Lorenzen	*/
;/*	GEM20			12/17/85		Lowell Webster	*/
;
;
;       Copyright 1999, Caldera Thin Clients, Inc.                      
;       This software is licenced under the GNU Public License.         
;       Please see LICENSE.TXT for further information.                 
;                                                                       
;                  Historical Copyright                                 
;/*
;*	-------------------------------------------------------------
;*	GEM Application Environment Services		  Version 3.0
;*	Serial No.  XXXX-0000-654321		  All Rights Reserved
;*	Copyright (C) 1987			Digital Research Inc.
;*	-------------------------------------------------------------
;*/
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

;
;
NUMPDS		equ	5			; max number of 2.0 procs
;
;
GLOBLARG	equ	01f9ah			; size of THEGLO data str.
;						    in GEMGLOBE.C with all
;						    the PDs, CDAs, UDAs
GLOBSMAL	equ	0113ch			; size of THEGLO data str.
;						    in GEMGLOBE.C with just
;						    2*5 EVBs,2 PDs, CDAs, UDAs
;
;
	.psect	_TEXT,class=CODE,reloc=16,global

	.globl	start
	.globl	gomain,terminate,_sizeglo,_sizeuda,__Hbss,__Lbss


;;	PUBLIC	start
;;	extrn	gomain:near			; in GMAIN.A86
;;	extrn	terminate:near			; in GMAIN.A86
;;	extrn	sizeglo:near			; in INIT.C
;;	extrn	sizeuda:near			; in INIT.C

;
;	get a stack to use
;
small_code:
small_data:

_start:
start:
	cli
	mov		bx,es			; PSP segment
	mov		ax,#seg(__Ldata)
	mov		ds,ax
	mov		ss,ax
	mov		es,ax
	mov		sp,#stk			; temp stack
;
	call	_sizeuda		; get sizeof uda - 2
	add		ax, #_D			; add to beginning of global array
	mov		sp, ax			; real stack
;
; zero out static uninitialized data
;
	call    _sizeglo			; get size of structure
	mov		dx, ax			; save it
	inc		ax
	shr		ax, #1			; make number of words
	mov		cx, ax
	xor		ax,ax
	mov		di,#_D			; es:di points to beg of D
	cld
	rep		stosw			; clear D out
;
; The PPD code generator likes to put things (eg: _indisp)  
; in the BSS segment, just below D.
;
	mov		cx,#__Lbss
	mov		ax,#__Hbss
	sub		ax,cx
	mov		cx,ax			;CX = size of BSS
	mov		di,#__Lbss
	xor		ax,ax
	rep		stosb

	mov		es,bx			; get psp back
	sti
;
;	free up any memory we're not using
;		when we start es has our psp segment
;
	mov		ax, es			;with no prefix our PSP is es
	mov		_PARABEG,ax
	mov		bx,#0			;get our psp address
	mov		_ad_psp+2, ax
	mov		_ad_psp,#0,word
	MOV		AX, ES:0+44[bx]
	mov		_ad_envrn+2, ax
	mov		_ad_envrn,#0,word
					; get command tail from psp
	mov		ax,es
	mov		_ad_s1fcb,#5ch,word
	mov		_ad_s1fcb+2,ax
	mov		_ad_s2fcb,#6ch,word
	mov		_ad_s2fcb+2,ax
	mov		_ad_stail,#80h,word
	mov		_ad_stail+2,ax
;
;
;
assumelrg:
	mov		cx, #NUMPDS
	mov		ax, dx			;  get size of globals
	call	memshrnk
;
;	shrink down some more if no room for accessories
;
chksmall:
	mov		bx, #0FFFFh
	mov		ah, #48h
	int		#21h
	cmp		bx, #03000h		; need 192k or more
	ja		setuda
	mov		cx, #2
	mov		ax, #GLOBSMAL
	call	memshrnk
;
;	set up uda
;
setuda:	
	mov		bx,#_D
	mov		ax,ss
	mov		0+4[bx],ax
	mov		ax,sp
	mov		0+2[bx],ax
;
; check for presence of GEMVDI before calling the application
;
	push	es
	xor		ax,ax
	mov		es,ax
	mov		bx,es:0x3BC
	mov		ax,es:0x3BE
	mov		es,ax
	mov		di,#2
	add		di,bx
	mov		si,#gemvdimsg
	mov		cx,#6
	push	di		; JCE 20-9-1999: We will also allow ViewMAX VDI
	rep		cmpsb
	pop		di
	jz		cmpend
	mov		si,#vmvdimsg
	mov		cx,#7
	rep		cmpsb
	jnz		cmpend
	mov		dx,#vmvdimsg
	mov		ah,#9
	int		#0x21	
	mov		ah,#1
	int		#0x21
	xor		ax,ax
	
cmpend:
	pop		es		;End of ViewMAX checks
	jz		runapplication
	mov		dx,#gemvdimsg
errormsg:
	mov		ah,#9
	int		#0x21
	mov		dx,#terminmsg
	mov		ah,#9
	int		#0x21
	jmp		terminate
;
;	shrink down freeing extra load memory
;
memshrnk:
	mov		_totpds, cx		; set total # of pds
	add		ax,#_PROGEND	; add in end of program
	add		ax,#15
	mov		cl,#4
	shr		ax,cl
	mov		cx,ax			; cx = our data size in paragraphs
	mov		bx,ds
	add		bx,cx			; bx = last used paragraph in image
	mov		ax,_PARABEG
	mov		es,ax
	sub		bx,ax			; bx = # of paras to shrink to 
	mov		ah,#4ah
	int		#21h
	mov		dx,#memerrmsg
	jc		errormsg
	ret
;
;
;
runapplication:
	mov		ax,ds
	jmp		gomain
;
;
	.psect	_TEXT,class=CODE

;
;
;		WORD
;	pgmld(handle, ploadname, pldaddr, paccroom)
;		WORD		handle;
;		BYTE		*ploadname;
;		LONG		*pldaddr;
;		WORD		*paccroom;
;
	.signat	_pgmld,16442

_pgmld:
	push	bp
	mov		bp,sp
	push	ds
	push	es		;In Pacific, dx = param 1 (handle) & ax = param2 (ploadname)
	
	mov		bx,dx	;BX = file handle

	push	ax		;ploadname
	mov		ax,ds
	mov		es,ax
;;	mov		bx,0+4[bp]		; file handle
	mov		cx,#1ch			; file header size

	mov		dx,#hdrbuf		; buffer for header
	mov		ah,#3fh			; read file
	int		#21h
	pop		dx				; DX = ploadname
	cmp		ax,#1ch			; get 'em all ?
	jz		norderr
rderr:
					; read error so error out
	pop		es
	pop		ds
	pop		bp
	mov		ax,#0xFFFF
	ret		#4

norderr:
	push	dx				; DX = ploadname
							; calculate size of desk acc.
	mov		ax,hdrbuf+2
	add		ax,#15
	mov		cl,#4
	shr		ax,cl
	mov		bx,ax			; bx = paras in (image mod 512)
	mov		ax,hdrbuf+4
	mov		cl,#5
	shl		ax,cl			; ax = paras in (image div 512)
	add		ax,bx			; ax = paras in image
							; check accessory room here
;;	mov		bx, 0+10[bp]
	mov		bx, 6[bp]
	mov		cx, [bx]		; cx = accessory room left (paras)
	sub		cx, ax
	pop		dx
	jb		lderr			; check fit
	push	dx
	mov		[bx], cx		; cx = store back acc room

							; allocate memory here
	mov		bx,ax
	mov		ax,#4800h
	int		#21h
	pop		dx
	jc		lderr
	push	dx
							; return load address
;;	mov		bx, 8[bp]
	mov		bx, 4[bp]
	mov		[bx], #0,word	; offset of load
	mov		0+2[bx], ax		; segment of load
							; set up parm block
	mov		bx,#parmblk
	mov		[bx],ax
	mov		2[bx],ax
							; do the load overlay
;;	mov		dx,6[bp]		; ds:dx has load name
	pop		dx
	mov		bx,#parmblk	; es:bx has parm block
	mov		ax,#4b03h
	push	ds
	int		#21h
	pop		ds
	jc		lderr
	xor		ax,ax
	pop		es
	pop		ds
	pop		bp
	ret		#4

lderr:
	mov		ax,#-1
	pop		es
	pop		ds
	pop		bp
	ret		#4
;
	.psect	text,class=CODE
	.psect	stack,class=STACK,reloc=16,global
;	sseg
	.blkb	32
stk:
	.word	0
;
;
	.psect	foo,class=LDATA
	.psect	bar,class=LBSS
	.psect	baz,class=NVRAM
	.psect	data,class=DATA,reloc=16,global

	.globl	_gintstk,_totpds,_pgmld,_ad_psp,_PARABEG
	.globl	_PROGEND,_ad_s1fcb,_ad_s2fcb,_ad_stail,_ad_envrn,_D
	.globl	_start,__Ldata
	.globl	small_code,small_data
	.align	16
	.loc	0
;
;
parmblk:
		.word	0,0	; for load overlay function
hdrbuf:
		.blkb	32				;	rb	20h
_ad_psp:
		.blkb	2				; 	rw	1 
_PARABEG:
		.blkb	2				; rw	1

	.psect	data,class=DATA
copyrt:
	db	'GEM Application Environment Services     Version 3.0'
	db	'Serial No.  XXXX-0000-654321     All Rights Reserved'
	db	'Copyright (C) 1987             Digital Research Inc.'
	db	'12/01/87'
;
; ------------------------
testmsg:
	 db	'??????????',0dh,0ah
; ------------------------

vmvdimsg:
	.byte	'ViewMAX VDI detected. Press key to continue.',0dh,0ah,'$'


gemvdimsg:
	.byte	'GEMVDI not present in memory.'
	.byte	'$'
;
memerrmsg:
	.byte	'Unable to free memory.'
	.byte	'$'
;
terminmsg:
	.byte	0dh,0ah
	.byte	'Execution terminated.'
	.byte	'$'
;
;
_gintstk:
	.word	0
;
;
_totpds:
	.word	0
;

	.end	_start

