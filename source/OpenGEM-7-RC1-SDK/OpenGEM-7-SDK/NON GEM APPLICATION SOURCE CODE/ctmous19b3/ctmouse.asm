; Cute Mouse Driver - a tiny mouse driver
; Copyright (c) 1997-2002 Nagy Daniel <nagyd@users.sourceforge.net>
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;

CTMVER		equ	"1.9"
CTMRELEASE	equ	"1.9 beta 3"
driverversion	equ	705h	; Microsoft driver version

WARN
%NOINCL
LOCALS
.model tiny
		assume	ss:nothing
;.286
;.386

FASTER_CODE	 = 0		; Optimize by speed instead size
OVERFLOW_PROTECT = 0		; Prevent internal variables overflow
FOOLPROOF	 = 1		; Check passed to driver arguments correctness

USE_286 equ (@CPU and 4)
USE_386 equ (@CPU and 8)

_ARG_DI_	equ	word ptr [bp]
_ARG_SI_	equ	word ptr [bp+2]
_ARG_BP_	equ	word ptr [bp+4]

IF USE_286

_ARG_BX_	equ	word ptr [bp+8]
_ARG_DX_	equ	word ptr [bp+10]
_ARG_CX_	equ	word ptr [bp+12]
_ARG_AX_	equ	word ptr [bp+14]
_ARG_ES_	equ	word ptr [bp+16]
_ARG_DS_	equ	word ptr [bp+18]
_ARG_OFFS_	equ	24

PUSHALL		equ	pusha
POPALL		equ	popa

ELSE ;---------- USE_286

_ARG_BX_	equ	word ptr [bp+6]
_ARG_DX_	equ	word ptr [bp+8]
_ARG_CX_	equ	word ptr [bp+10]
_ARG_AX_	equ	word ptr [bp+12]
_ARG_ES_	equ	word ptr [bp+14]
_ARG_DS_	equ	word ptr [bp+16]
_ARG_OFFS_	equ	22

PUSHALL		equ	push	ax cx dx bx bp si di
POPALL		equ	pop	di si bp bx dx cx ax

ENDIF ;---------- USE_286

INCLUDE asm.mac
INCLUDE macro.mac
INCLUDE BIOS/area0.def
INCLUDE convert/digit.mac
INCLUDE DOS/MCB.def
INCLUDE DOS/PSP.def
INCLUDE DOS/file.mac
INCLUDE DOS/io.mac
INCLUDE DOS/mem.mac
INCLUDE hard/PIC8259A.def
INCLUDE hard/UART.def

PS2serv		macro	serv,errlabel
		mov	ax,serv
		int	15h
	IFNB <errlabel>
		jc	errlabel
		test	ah,ah
		jnz	errlabel
	ENDIF
endm

POINT		struc
  X		dw	0
  Y		dw	0
ends

.code
		org	0
TSRstart	label
		org	100h
start:		jmp	real_start


;€€€€€€€€€€€€€€€€€€€€€€€€€€€ UNINITIALIZED DATA €€€€€€€€€€€€€€€€€€€€€€€€€€€

;!!! WARNING: don't init variables in uninitialized data section because
;		it will not be present in the executable image

		org	PSP_TSR		; reuse part of PSP
TSRdata?	label

;----- application state begins here -----

;!!! WARNING: variables order between StartSaveArea and LenDefArea must be
;		syncronized with variables order after StartDefArea

		evendata
StartSaveArea = $

mickey8		POINT	?		; mickeys per 8 pixel ratios
;;+*doublespeed	dw	?		; double-speed threshold (mickeys/sec)
startscan	dw	?		; screen mask/cursor start scanline
endscan		dw	?		; cursor mask/cursor end scanline

;---------- hotspot, screenmask and cursormask must follow as is

hotspot		POINT	?		; cursor bitmap hot spot
screenmask	db	2*16 dup (?)	; user defined screen mask
cursormask	db	2*16 dup (?)	; user defined cursor mask
cursorhidden	db	?		; 0 - cursor visible, else hidden
;;+*nolightpen?	db	?		; 0 - emulate light pen
		evendata
LenDefArea = $ - StartSaveArea		; initialized by softreset_21

rangemax	POINT	?		; horizontal/vertical range max
upleft		POINT	?		; upper left of update region
lowright	POINT	?		; lower right of update region
pos		POINT	?		; virtual cursor position
granpos		POINT	?		; granulated virtual cursor position
UIR@		dd	?		; user interrupt routine address

		evendata
StartClearArea = $

rounderr	POINT	?		; rounding error after mickeys->pixels
		evendata
LenClearArea1 = $ - StartClearArea	; cleared by setpos_04

rangemin	POINT	?		; horizontal/vertical range min
		evendata
LenClearArea2 = $ - StartClearArea	; cleared by setupvideo

cursortype	db	?		; 0 - software, else hardware
callmask	db	?		; user interrupt routine call mask
mickeys		POINT	?		; mouse move since last access
BUTTLASTSTATE	struc
  counter	dw	?
  lastrow	dw	?
  lastcol	dw	?
	ends
buttpress	BUTTLASTSTATE	?,?,?
buttrelease	BUTTLASTSTATE	?,?,?
		evendata
LenClearArea3 = $ - StartClearArea	; cleared by softreset_21
LenSaveArea = $ - StartSaveArea

;----- registers values for RIL -----

;!!! WARNING: registers order and RGROUPDEF contents must be fixed

		evendata
StartVRegsArea = $

regs_SEQC	db	5 dup (?)
reg_MISC	db	?
regs_CRTC	db	25 dup (?)
regs_ATC	db	21 dup (?)
regs_GRC	db	9 dup (?)
reg_FC		db	?
reg_GPOS1	db	?
reg_GPOS2	db	?
		evendata
LenVRegsArea = $ - StartVRegsArea

		evendata
StartDefVRegsArea = $

def_SEQC	db	5 dup (?)
def_MISC	db	?
def_CRTC	db	25 dup (?)
def_ATC		db	21 dup (?)
def_GRC		db	9 dup (?)
def_FC		db	?
def_GPOS1	db	?
def_GPOS2	db	?
		evendata
ERRIF ($-StartDefVRegsArea ne LenVRegsArea) "VRegs area contents corrupted!"

;----- old interrupt vectors -----

oldint33	dd	?		; old INT 33 handler address
oldIRQaddr	dd	?		; old IRQ handler address


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€ INITIALIZED DATA €€€€€€€€€€€€€€€€€€€€€€€€€€€€

		evendata
TSRdata		label

StartDefArea = $
		POINT	<8,16>			; mickey8
;;+*		dw	64			; doublespeed
		dw	77FFh,7700h		; startscan, endscan
		POINT	<0,0>			; hotspot
		dw	0011111111111111b	; screenmask
		dw	0001111111111111b
		dw	0000111111111111b
		dw	0000011111111111b
		dw	0000001111111111b
		dw	0000000111111111b
		dw	0000000011111111b
		dw	0000000001111111b
		dw	0000000000111111b
		dw	0000000000011111b
		dw	0000000111111111b
		dw	0000000011111111b
		dw	0011000011111111b
		dw	1111100001111111b
		dw	1111100001111111b
		dw	1111110011111111b
		dw	0000000000000000b	; cursormask
		dw	0100000000000000b
		dw	0110000000000000b
		dw	0111000000000000b
		dw	0111100000000000b
		dw	0111110000000000b
		dw	0111111000000000b
		dw	0111111100000000b
		dw	0111111110000000b
		dw	0111110000000000b
		dw	0110110000000000b
		dw	0100011000000000b
		dw	0000011000000000b
		dw	0000001100000000b
		dw	0000001100000000b
		dw	0000000000000000b
		db	1			; cursorhidden
;;+*		db	0			; nolightpen?
		evendata
ERRIF ($-StartDefArea ne LenDefArea) "Defaults area contents corrupted!"

;----- driver and video state begins here -----

		evendata
UIRunlock	db	1		; 0 - user intr routine is in progress
videolock	db	1		; drawing: 1=ready,0=busy,-1=busy+queue

cursorptr	dd	0		; video memory pointer
cursordrawn	db	0		; 1 - restore screen data

videomode	db	?		; video mode number
granumask	POINT	<-1,-1>
textbuf		label	word
buffer@		dd	?		; pointer to screen sprite copy

vdata1		dw	10h,1, 10h,3, 10h,4, 10h,5, 10h,8, 08h,2
VDATA1cnt	equ	($-vdata1)/4
vdata2		dw	10h,1, 10h,4, 10h,0105h, 10h,0FF08h, 08h,0F02h
VDATA2cnt	equ	($-vdata2)/4

;----- table of pointers to registers values for RIL -----

RGROUPDEF	struc
  port@		dw	?
  regs@		dw	?
  def@		dw	?
  regscnt	db	1
  rmodify?	db	0
RGROUPDEF	ends

		evendata
videoregs@	label
	RGROUPDEF <3D4h,regs_CRTC,def_CRTC,25>	; CRTC
	RGROUPDEF <3C4h,regs_SEQC,def_SEQC,5>	; Sequencer
	RGROUPDEF <3CEh,regs_GRC, def_GRC, 9>	; Graphics controller
	RGROUPDEF <3C0h,regs_ATC, def_ATC, 20>	; VGA attrib controller
	RGROUPDEF <3C2h,reg_MISC, def_MISC>	; VGA misc output and input
	RGROUPDEF <3DAh,reg_FC,   def_FC>	; Feature Control
	RGROUPDEF <3CCh,reg_GPOS1,def_GPOS1>	; Graphics 1 Position
	RGROUPDEF <3CAh,reg_GPOS2,def_GPOS2>	; Graphics 2 Position


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€ IRQ HANDLERS €€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

IRQhandler	proc
		assume	ds:nothing,es:nothing
		;cld
		push	ds es
		PUSHALL
		MOVSEG	ds,cs,,DGROUP
IRQproc		label	byte			; "mov al,OCW2<OCW2_EOI>"
		j	PS2proc			;  if serial mode
		out	PIC1_OCW2,al		; {20h} end of interrupt

		db	0BAh			; MOV DX,word
IO_address	dw	?			; UART IO address
		push	dx
		movidx	dx,LSR_index
		 in	al,dx			; {3FDh} LSR: get status
		xchg	bx,ax			; OPTIMIZE: instead MOV BL,AL
		pop	dx
		movidx	dx,RBR_index
		 in	al,dx			; {3F8h} flush receive buffer
		db	0B9h			; MOV CX,word
IOdone		db	?,0			; processed bytes counter

		testflag bl,mask LSR_break+mask LSR_FE+mask LSR_OE
		jz	@@checkdata		; if break/framing/overrun
		xor	cx,cx			;  errors then restart
		mov	[IOdone],cl		;  sequence: clear counter

@@checkdata:	shr	bl,LSR_RBF+1
		jnc	@@exitIRQ		; jump if data not ready
		db	0E8h			; CALL NEAR to mouseproc
mouseproc	dw	MSMproc-$-2

@@exitIRQ:	jmp	@rethandler
IRQhandler	endp
		assume	ds:DGROUP

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;!!! WARNING: below buffer for copy of screen sprite when serial protocols

bufferSERIAL	label				; requires 3*16 bytes

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;				Enable PS/2
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	none
; Modf:	BX, ES
; Call:	disablePS2, INT 15/C2xx
;
enablePS2	proc
		call	disablePS2
		MOVSEG	es,cs,,DGROUP
		mov	bx,offset DGROUP:IRQhandler
		PS2serv	0C207h			; es:bx=ptr to handler
		mov	bh,1			; set mouse on
		PS2serv	0C200h
		ret
enablePS2	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;				Disable PS/2
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	none
; Modf:	BX, ES
; Call:	INT 15/C2xx
;
disablePS2	proc
		xor	bx,bx			; set mouse off
		PS2serv	0C200h
		MOVSEG	es,bx,,nothing		; clear PS/2 mouse handler
		PS2serv	0C207h			; es:bx=ptr to handler
		ret
disablePS2	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

PS2proc		proc
		mov	bp,sp
		mov	ax,[bp+_ARG_OFFS_+6]	; Status
		mov	bx,[bp+_ARG_OFFS_+4]	; X
		mov	cx,[bp+_ARG_OFFS_+2]	; Y
		testflag al,00100000b	; =20h	; check Y sign bit
		jz	@@checkxsign
		mov	ch,-1
@@checkxsign:	testflag al,00010000b	; =10h	; check X sign bit
		jz	@@yreverse
		mov	bh,-1
@@yreverse:	neg	cx			; reverse Y movement
		call	swapbuttons
		POPALL
		pop	es ds
		retf
PS2proc		endp
ERRIF ($-bufferSERIAL lt 3*16) "PS/2 handler too small for buffer!"

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;!!! WARNING: below buffer for copy of screen sprite when PS2 protocol

bufferPS2	label				; requires 3*16 bytes

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Enable serial interrupt in PIC
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	IO_address
; Modf:	AX, DX, SI, IOdone, MSLTbuttons
; Call:	INT 21/25
;
enableUART	proc
;---------- set new IRQ handler
		db	0B8h			; MOV AX,word
IRQintnum	dw	2500h			; INT number of selected IRQ
		mov	dx,offset DGROUP:IRQhandler
		int	21h			; set INT in DS:DX

;---------- set communication parameters
		mov	si,[IO_address]
		movidx	dx,LCR_index,si
		 out_	dx,%LCR{LCR_DLAB=1}	; {3FBh} LCR: DLAB on
		xchg	dx,si			; 1200 baud rate
		 outw	dx,96			; {3F8h},{3F9h} divisor latch
		xchg	dx,si
		 db	0B8h			; MOV AX,word
LCRset		 LCR	<0,,LCR_noparity,0,2>	; {3FBh} LCR: DLAB off, 7/8N1
		 MCR	<,,,1,1,1,1>		; {3FCh} MCR: DTR/RTS/OUTx on
		 out	dx,ax

;---------- prepare UART for interrupts
		movidx	dx,RBR_index,si,LCR_index
		 in	al,dx			; {3F8h} flush receive buffer
		movidx	dx,IER_index,si,RBR_index
		 out_	dx,%IER{IER_DR=1},%FCR<>; {3F9h} IER: enable DR intr
						; {3FAh} FCR: disable FIFO
		dec	ax			; OPTIMIZE: instead MOV AL,0
		mov	[IOdone],al
		mov	[MSLTbuttons],al
;----------
		in	al,PIC1_IMR		; {21h} get IMR
		db	24h			; AND AL,byte
notPIC1state	db	?			; clear bit to enable interrupt
		out	PIC1_IMR,al		; {21h} enable serial interrupts
		ret
enableUART	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Disable serial interrupt of PIC
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	IO_address, oldIRQaddr
; Modf:	AX, DX
; Call:	INT 21/25
;
disableUART	proc
		in	al,PIC1_IMR		; {21h} get IMR
		db	0Ch			; OR AL,byte
PIC1state	db	?			; set bit to disable interrupt
		out	PIC1_IMR,al		; {21h} disable serial interrupts
;----------
		movidx	dx,LCR_index,[IO_address] ; {3FBh} LCR: DLAB off
		 out_	dx,%LCR<>,%MCR<>	; {3FCh} MCR: DTR/RTS/OUT2 off
		movidx	dx,IER_index,,LCR_index
		 ;mov	al,IER<>
		 out	dx,al			; {3F9h} IER: interrupts off

;---------- restore old IRQ handler
		mov	ax,[IRQintnum]		; AH=25h
		push	ds
		lds	dx,[oldIRQaddr]
		int	21h			; set INT in DS:DX
		pop	ds
		ret
disableUART	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Process mouse bytes the Microsoft/Logitech way
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

MSLTproc	proc
		testflag al,01000000b	; =40h	; synchro check
		jz	@@MSLT_2		; jump if non first byte

		mov	[IOdone],1		; request next 2/3 bytes
		mov	[MSLT_1],al
MSLTCODE1	label	byte			; "ret" if not LT/WM
		xchg	ax,cx			; OPTIMIZE: instead MOV AL,CL
		sub	al,3			; first byte after 3 bytes?
		jnz	@@MSLTret
		mov	[MSLTbuttons],al	; release middle button if yes
@@MSLTret:	ret

@@MSLT_2:	jcxz	@@MSLTret		; skip nonfirst byte at start
		inc	[IOdone]		; request next byte
		loop	@@MSLT_3
		mov	[MSLT_X],al		; keep X movement LO
		ret

@@MSLT_3:	loop	@@LTWM_4
		xchg	cx,ax			; OPTIMIZE: instead MOV CL,AL
		db	0B4h			; MOV AH,byte
MSLT_1		db	?			; first byte of MS/LT protocol

		;mov	al,0			; CX was 0
		shr	ax,2			; bits 1-0 - X movement HI
		db	0Ch			; OR AL,byte
MSLT_X		db	?
		mov	bh,ah
		cbw
		xchg	bx,ax			; BX=X movement

		mov	al,0
		shr	ax,2			; bits 3-2 - Y movement HI
		or	al,cl
		mov	cl,ah			; bits 5-4 - L/R buttons
		cbw
		xchg	cx,ax			; CX=Y movement

		flipflag al,[MSLTbuttons]
		maskflag al,00000011b	; =3	; L/R buttons change mask
		mov	dl,al
		or	dl,bl			; nonzero if L/R buttons state
		or	dl,cl			;  changed or mouse moved
MSLTCODE2	label	byte
		j	@@MSLTupdate		; "jnz" if MS3
		setflag	al,00000100b	; =4	; empty event toggles button
		j	@@MSLTupdate

@@LTWM_4:	;mov	ch,0
		mov	[IOdone],ch		; request next 3/4 bytes
MSLTCODE3	label	byte			; if LT "mov cl,3" else
		mov	cl,3			; if WM "mov cl,2" else "ret"
		shr	al,cl			; (MS defines only 3 bytes)
		flipflag al,[MSLTbuttons]
		maskflag al,00000100b	; =4
		jz	@@MSLTret		; exit if button 3 not changed
		xor	cx,cx
		xor	bx,bx
@@MSLTupdate:	db	034h			; XOR AL,byte
MSLTbuttons	db	?			; buttons state for MS3/LT
		mov	[MSLTbuttons],al
		j	swapbuttons
MSLTproc	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Process mouse bytes the Mouse Systems way
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

MSMproc		proc
		jcxz	@@MSM_1
		cbw
		dec	cx
		jz	@@MSM_2
		dec	cx
		jz	@@MSM_3
		loop	@@MSM_5

@@MSM_4:	add	ax,[MSM_X]
@@MSM_2:	mov	[MSM_X],ax
		j	@@MSMnext

@@MSM_1:	flipflag al,10000111b	; =87h	; sync check: AL should
		testflag al,11111000b	; =0F8h	;  be equal to 10000lmr
		jnz	@@MSMret
		mov	[MSM_buttons],al	; bits 2-0 - M/L/R buttons
		;j	@@MSMnext

@@MSM_3:	mov	[MSM_Y],ax
@@MSMnext:	inc	[IOdone]		; request next byte
@@MSMret:	ret

@@MSM_5:	;mov	ch,0
		mov	[IOdone],ch		; request next 5 bytes
		db	0B1h			; MOV CL,byte
MSM_buttons	db	?
		db	05h			; ADD AX,word
MSM_Y		dw	?
		db	0BBh			; MOV BX,word
MSM_X		dw	?
		xchg	cx,ax
		neg	cx
		testflag al,00000110b	; =6	; check the L and M buttons
		jpe	swapbuttons		; skip if the same
		flipflag al,00000110b	; =6	; swap them
		;j	swapbuttons
MSMproc		endp
ERRIF ($-bufferPS2 lt 3*16) "Serial handler too small for buffer!"

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Update mouse status
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	AL			(new buttons state)
;	BX			(X mouse movement)
;	CX			(Y mouse movement)
; Out:	none
; Use:	callmask, granpos, mickeys, UIR@
; Modf:	AX, CX, DX, BX, SI, DI, UIRunlock
; Call:	updateposition, updatebutton, refreshcursor
;
swapbuttons	proc
		testflag al,00000011b	; =3	; check the L and R buttons
		jpe	mouseupdate		; skip if the same
		db	034h			; XOR AL,byte
SWAPMASK	db	00000011b	; =3	; 0 if (PS2 xor LEFTHAND)
		;j	mouseupdate
swapbuttons	endp

mouseupdate	proc
		db	024h			; AND AL,byte
buttonsmask	db	00000111b
		xchg	di,ax			; OPTIMIZE: instead MOV DI,AX

;---------- recalculate mickey counters and screen position
		db	0B2h			; MOV DL,byte
mresolutionX	db	0
		xchg	ax,bx			; OPTIMIZE: instead MOV AX,BX
		MOVREG_	bx,<offset X>
		call	updateposition

		db	0B2h			; MOV DL,byte
mresolutionY	db	0
		xchg	ax,cx
		MOVREG_	bl,<offset Y>		; OPTIMIZE: BL instead BX
		call	updateposition
		or	cl,al			; bit 0=mickeys change flag

;---------- recalculate buttons state
		mov	ax,[mickeys.Y]
		xchg	ax,di
		xchg	dx,ax			; OPTIMIZE: instead MOV DX,AX
		mov	dh,dl			; DH=buttons new state
		xchg	dl,[buttstatus]
		xor	dl,dh			; DL=buttons change state
IF FASTER_CODE
		jz	@@callUIR		; CX=events mask
ENDIF
		xor	bx,bx			; buttpress array index
		mov	al,00000010b		; indicate that 1 is pressed
		call	updatebutton
		mov	al,00001000b		; indicate that 2 is pressed
		call	updatebutton
		mov	al,00100000b		; indicate that 3 is pressed
		call	updatebutton

;---------- call User Interrupt Routine
@@callUIR:	dec	[UIRunlock]
		jnz	@@updatedone		; exit if user proc running

		and	cl,[callmask]		; is there a user call mask?
		jz	@@updateshow		; skip if not
		db	0BBh			; MOV BX,word
buttstatus	db	0,0			; buttons status
		mov	ax,[granpos.X]
		mov	dx,[granpos.Y]
		xchg	ax,cx
		mov	si,[mickeys.X]
		;mov	di,[mickeys.Y]
		push	ds
		sti
		call	[UIR@]
		pop	ds
@@updateshow:	call	refreshcursor
;----------
@@updatedone:	inc	[UIRunlock]
		ret
mouseupdate	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; In:	AX			(mouse movement)
;	BX			(offset X/offset Y)
;	DL			(resolution)
; Out:	AX			(1 - mickey counter changed)
; Use:	mickey8, rangemax, rangemin, granumask
; Modf:	DX, SI, mickeys, rounderr, pos, granpos
;
updateposition	proc
		test	ax,ax
		jz	@@uposret
		mov	si,ax
		jns	@@resolute
		neg	si

;---------- apply resolution (AX=movement, SI=abs(AX), DL=reslevel)
@@resolute:	cmp	si,2			; movement not changed if...
		jbe	@@newmickeys		; ...[-2..+2] movement
		mov	dh,0
		dec	dx
		jz	@@newmickeys		; ...or resolution=1
		cmp	si,7
		ja	@@rescheck		; [-7..-2,+2..+7] movement...
@@resmul2:	shl	ax,1			; ...multiplied by 2
		j	@@newmickeys

@@rescheck:	inc	dx
		jnz	@@resmul

		mov	dl,10
		shr	si,2
		cmp	si,dx
		jae	@@resmul		; auto resolution=
		mov	dx,si			;  min(10,abs(AX)/4)
@@resmul:
IF FASTER_CODE
		cmp	dl,2
		je	@@resmul2
ENDIF
		imul	dx			; multiply on resolution

;---------- apply mickeys per 8 pixels ratio to calculate cursor position
@@newmickeys:	add	word ptr mickeys[bx],ax
IF FASTER_CODE
		mov	si,word ptr mickey8[bx]
		cmp	si,8
		je	@@newpos
		shl	ax,3
		dec	si
		jle	@@newpos
		add	ax,word ptr rounderr[bx]
		inc	si
		cwd
		idiv	si
		mov	word ptr rounderr[bx],dx
		test	ax,ax
		jz	@@uposdone
ELSE
		shl	ax,3
		add	ax,word ptr rounderr[bx]
		cwd
		idiv	word ptr mickey8[bx]
		mov	word ptr rounderr[bx],dx
ENDIF
@@newpos:	add	ax,word ptr pos[bx]

;---------- cut new position by virtual ranges and save
@savecutpos:	mov	dx,word ptr rangemax[bx]
		cmp	ax,dx
		jge	@@cutpos
		mov	dx,word ptr rangemin[bx]
		cmp	ax,dx
		jg	@@savepos
@@cutpos:	xchg	ax,dx			; OPTIMIZE: instead MOV AX,DX

@@savepos:	mov	word ptr pos[bx],ax	; new position
		and	al,byte ptr granumask[bx]
		mov	word ptr granpos[bx],ax	; new granulated position
@@uposdone:	mov	ax,1
@@uposret:	ret
updateposition	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; In:	AL			(unrolled press bit mask)
;	CL			(unrolled buttons change state)
;	DL			(buttons change state)
;	DH			(buttons new state)
;	BX			(buttpress array index)
; Out:	CL
;	DX			(shifted state)
;	BX			(next index)
; Use:	granpos
; Modf:	AX, SI, buttpress, buttrelease
;
updatebutton	proc
		shr	dx,1
		jnc	@@nextbutton		; jump if button unchanged
		mov	si,offset DGROUP:buttpress
		test	dl,dl
		js	@@updbutton		; jump if button pressed
		shl	al,1			; indicate that it released
		mov	si,offset DGROUP:buttrelease
@@updbutton:	or	cl,al
		inc	[si+bx].counter
		mov	ax,[granpos.Y]
		mov	[si+bx].lastrow,ax
		mov	ax,[granpos.X]
		mov	[si+bx].lastcol,ax
@@nextbutton:	add	bx,SIZE BUTTLASTSTATE
		ret
updatebutton	endp

;€€€€€€€€€€€€€€€€€€€€€€€€€€ END OF IRQ HANDLERS €€€€€€€€€€€€€€€€€€€€€€€€€€€


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€ INT 10 HANDLER €€€€€€€€€€€€€€€€€€€€€€€€€€€€€

		evendata
RILtable	dw	offset DGROUP:RIL_F0	; RIL functions
		dw	offset DGROUP:RIL_F1
		dw	offset DGROUP:RIL_F2
		dw	offset DGROUP:RIL_F3
		dw	offset DGROUP:RIL_F4
		dw	offset DGROUP:RIL_F5
		dw	offset DGROUP:RIL_F6
		dw	offset DGROUP:RIL_F7

int10handler	proc
		assume	ds:nothing,es:nothing
		cld
		test	ah,ah			; set video mode?
		jz	@@setmodreq
		cmp	ah,11h			; font manipulation function
		je	@@setnewfont
		cmp	ax,4F02h		; VESA set video mode?
		je	@@setmodreq
		cmp	ah,0F0h			; RIL func requested?
		jb	@@jmpold10
		cmp	ah,0F7h
		jbe	@@RIL
		cmp	ah,0FAh
		je	@@RIL_FA
@@jmpold10:	db	0EAh			; JMP FAR to old INT 10 handler
oldint10	dd	?

@@setnewfont:	cmp	al,10h
		jb	@@jmpold10
		cmp	al,20h
		jae	@@jmpold10
		;j	@@setmodreq

;========== set video mode
@@setmodreq:	push	ax
		mov	al,2			; OPTIMIZE: AL instead AX
		pushf				;!!! Logitech MouseWare
		push	cs			;  Windows driver workaround
		call	handler33		; hide mouse cursor
		pop	ax
		pushf
		call	cs:[oldint10]
		push	ds es
		PUSHALL
		MOVSEG	ds,cs,,DGROUP
		mov	[cursorhidden],1	; normalize flag
		call	setupvideo
@@exitINT10:	jmp	@rethandler

;========== RIL
@@RIL:		push	ds es
		PUSHALL
		MOVSEG	ds,cs,,DGROUP
		mov	bp,sp
		mov	al,ah
		maskflag ax,0Fh			;!!! AH must be 0 for RIL_*
		mov	si,ax
		shl	si,1
		call	RILtable[si]
		j	@@exitINT10
;----------
@@RIL_FA:	MOVSEG	es,cs,,DGROUP		; RIL FA - Interrogate driver
		mov	bx,offset DGROUP:RILversion
		iret
int10handler	endp
		assume	ds:DGROUP

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F0 - Read one register
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	DX			(group index)
;	BX			(register #)
; Out:	BL			(value)
; Use:	videoregs@
; Modf:	AL, SI
; Call:	none
;
RIL_F0		proc
		mov	si,dx
		mov	si,videoregs@[si].regs@
		cmp	dx,20h
		jae	@@retBL			; jump if single register
		add	si,bx
@@retBL:	lodsb
		mov	byte ptr [_ARG_BX_],al
		ret
RIL_F0		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F1 - Write one register
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	DX			(group index)
;	BL			(value for single reg)
;	BL			(register # otherwise)
;	BH			(value otherwise)
; Out:	BL			(value)
; Use:	none
; Modf:	AX
; Call:	RILwrite
;
RIL_F1		proc
		mov	ah,bl
		cmp	dx,20h
		jae	RILwrite		; jump if single registers
		xchg	ax,bx			; OPTIMIZE: instead MOV AX,BX
		mov	byte ptr [_ARG_BX_],ah
		;j	RILwrite
RIL_F1		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; In:	DX			(group index)
;	AL			(register # for regs group)
;	AH			(value to write)
; Out:	none
; Use:	videoregs@
; Modf:	AL, DX, BX, DI
; Call:	RILoutAH, RILgroupwrite
;
RILwrite	proc
		xor	bx,bx
		mov	di,dx
		cmp	dx,20h
		mov	dx,videoregs@[di].port@
		mov	videoregs@[di].rmodify?,dl ; OPTIMIZE: DL instead 1
		mov	di,videoregs@[di].regs@
		jae	@@RWsingle		; jump if single register
		mov	bl,al
@@RWsingle:	mov	[di+bx],ah
		jae	RILoutAH
		;j	RILgroupwrite
RILwrite	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; In:	DX			(IO port)
;	AL			(register #)
;	AH			(value to write)
; Out:	none
; Use:	videoregs@
; Modf:	none
; Call:	none
;
RILgroupwrite	proc
		cmp	dl,0C0h
		je	@@writeATC		; jump if ATTR controller
		out	dx,ax
		ret
@@writeATC:	push	ax dx
		mov	dx,videoregs@[(SIZE RGROUPDEF)*5].port@
		in	al,dx			; {3DAh} force address mode
		pop	dx ax
		out	dx,al			; {3C0h} select ATC register
RILoutAH:	xchg	al,ah
		out	dx,al			; {3C0h} modify ATC register
		xchg	al,ah
		ret
RILgroupwrite	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F2 - Read register range
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CH			(starting register #)
;	CL			(# of registers)
;	DX			(group index: 0,8,10h,18h)
;	ES:BX			(buffer, CL bytes size)
; Out:	none
; Use:	videoregs@
; Modf:	AX, CX, SI, DI
; Call:	none
;
RIL_F2		proc
		assume	es:nothing
		mov	di,bx
		mov	si,dx
		mov	si,videoregs@[si].regs@
		mov	al,ch
		;mov	ah,0
		add	si,ax
RILmemcopy:	sti
		mov	ch,0
		shr	cx,1
		rep	movsw
		adc	cx,cx
		rep	movsb
		ret
RIL_F2		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F3 - Write register range
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CH			(starting register #)
;	CL			(# of registers, >0)
;	DX			(group index: 0,8,10h,18h)
;	ES:BX			(buffer, CL bytes size)
; Out:	none
; Use:	videoregs@
; Modf:	AX, CX, DX, BX, DI
; Call:	RILgroupwrite
;
RIL_F3		proc
		assume	es:nothing
		mov	di,dx
		mov	dx,videoregs@[di].port@
		mov	videoregs@[di].rmodify?,dl ; OPTIMIZE: DL instead 1
		mov	di,videoregs@[di].regs@
RILgrouploop:	xor	ax,ax
		xchg	al,ch
		add	di,ax
@@R3loop:	mov	ah,es:[bx]
		mov	[di],ah
		inc	bx
		inc	di
		call	RILgroupwrite
		inc	ax			; OPTIMIZE: AX instead AL
		loop	@@R3loop
		ret
RIL_F3		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F4 - Read register set
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(# of registers, >0)
;	ES:BX			(table of registers records)
; Out:	none
; Use:	videoregs@
; Modf:	AL, CX, BX, DI
; Call:	none
;
RIL_F4		proc
		assume	es:nothing
		sti
		mov	di,bx
@@R4loop:	mov	bx,es:[di]
		movadd	di,,2
		mov	bx,videoregs@[bx].regs@
		mov	al,es:[di]
		inc	di
		xlat
		stosb
		loop	@@R4loop
		ret
RIL_F4		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F5 - Write register set
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(# of registers, >0)
;	ES:BX			(table of registers records)
; Out:	none
; Use:	none
; Modf:	AX, CX, DX, SI
; Call:	RILwrite
;
RIL_F5		proc
		assume	es:nothing
		mov	si,bx
@@R5loop:	lods	word ptr es:[si]
		xchg	dx,ax			; OPTIMIZE: instead MOV DX,AX
		lods	word ptr es:[si]
		call	RILwrite
		loop	@@R5loop
		ret
RIL_F5		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F7 - Define registers default
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	DX			(group index)
;	ES:BX			(table of one-byte entries)
; Out:	none
; Use:	videoregs@
; Modf:	CL, SI, DI, ES, DS
; Call:	RILmemcopy
;
RIL_F7		proc
		assume	es:nothing
		mov	si,bx
		mov	di,dx
		mov	cl,videoregs@[di].regscnt
		mov	videoregs@[di].rmodify?,cl ; OPTIMIZE: CL instead 1
		mov	di,videoregs@[di].def@
		push	es ds
		pop	es ds
		j	RILmemcopy
RIL_F7		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F6 - Revert registers to default
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	videoregs@
; Modf:	AX, CX, DX, BX, SI, DI, ES
; Call:	RILgrouploop
;
RIL_F6		proc
		MOVSEG	es,ds,,DGROUP
		mov	si,offset DGROUP:videoregs@+(SIZE RGROUPDEF)*8

@@R6loop:	sub	si,SIZE RGROUPDEF
		xor	cx,cx
		xchg	cl,[si].rmodify?
		jcxz	@@R6next

		mov	bx,[si].def@
		mov	di,[si].regs@
		mov	dx,[si].port@
		mov	cl,[si].regscnt
		;mov	ch,0
		loop	@@R6group
		mov	al,[bx]
		stosb
		out	dx,al
		;j	@@R6next		; OPTIMIZE: single regs
		j	@@R6loop		;  handled first

@@R6group:	inc	cx			; OPTIMIZE: CX instead CL
		;mov	ch,0
		call	RILgrouploop

@@R6next:	cmp	si,offset DGROUP:videoregs@
		ja	@@R6loop
		ret
RIL_F6		endp

;€€€€€€€€€€€€€€€€€€€€€€€€€ END OF INT 10 HANDLER €€€€€€€€€€€€€€€€€€€€€€€€€€


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Draw mouse cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

drawcursor	proc
		jz	@graphcursor		; jump if graphics mode
		call	gettxtoffset
		cmp	[cursortype],ch		; OPTIMIZE: CH instead 0
		jz	@@softcursor		; jump if software cursor

;========== draw hardware text mode cursor
		shr	di,1
		mov	dx,videoregs@[0].port@	; CRTC port
		mov	ax,di
		out_	dx,0Fh,al		; cursor position lo
		xchg	ax,di			; OPTIMIZE: instead MOV AX,DI
		out_	dx,0Eh,ah		; cursor position hi
		ret

;========== draw software text mode cursor
@@softcursor:	jcxz	@@drawtext		; jump if cursor not drawn
		cmp	di,word ptr cursorptr[0]
		je	@@drawtextdone		; exit if position not changed
		push	di
		call	restoretext
		pop	di

@@drawtext:	mov	ax,[granpos.Y]		; get cursor position
		mov	bx,[granpos.X]
		mov	si,8			; OPTIMIZE: instead -[granumask.Y]
		call	checkifseen
		jc	@@drawret		; exit if not seen

		mov	word ptr cursorptr[0],di
		MOVSEG	es,<word ptr cursorptr[2]>,,nothing
		mov	ax,es:[di]		; save char under cursor
		mov	textbuf[0],ax
		and	ax,[startscan]
		xor	ax,[endscan]
		stosw				; draw to new position
		mov	textbuf[2],ax

@@drawtextdone:	inc	[cursordrawn]		; we have to restore later
@@drawret:	ret

;========== draw graphics mode cursor
@graphcursor:	; if [videomode] not in [4-6, 0Dh-13h] "ret" else "push cx"
		db	?
		call	updatevregs
		mov	bx,[granpos.X]
		mov	ax,[granpos.Y]
		sub	bx,[hotspot.X]
		sub	ax,[hotspot.Y]		; virtual Y
		and	bx,[granumask.X]	; virtual granulated X

		push	ax
		call	getgroffset
		pop	ax cx

; cx=old cursordrawn, bx=X, ax=Y, di=video row offset, si=nextrow

		jcxz	@@drawgraph		; jump if cursor not drawn
		cmp	bx,[cursorX]
		jne	@@restsprite
		db	081h,0FFh		; CMP DI,word
cursorrow	dw	?
		je	@@drawgrdone		; exit if position not changed

; bx=X, ax=Y, di=video row offset, si=nextrow
@@restsprite:	push	ax bx
		call	restoresprite
		pop	bx ax

;---------- check if cursor seen and not in update region
; bx=X, ax=Y, di=video row offset, si=nextrow
@@drawgraph:	mov	[cursorX],bx
		mov	[cursorY],ax
		mov	[cursorrow],di
		mov	[nextrow],si

		db	0B1h			; MOV CL,byte
bitmapshift	db	?			; mode 13h=1, 0Dh=4, other=3
		shr	bx,cl
		shl	bx,cl			; virtual screen aligned X
		mov	si,16			; cursor height
		call	checkifseen
		jc	@@drawgrret		; exit if not seen

;---------- precompute some sprite parameters
		push	ax			; push [cursorY]
		shr	dx,cl			; right X byte offset
		mov	ax,[scanline]
		cmp	dx,ax
		jbe	@@spritesize
		xchg	dx,ax			; DX=min(DX,scanline)
@@spritesize:	sar	bx,cl			; left X byte offset (signed)
		db	0B8h			; MOV AX,word
cursorX		dw	?
		sub	cl,3			; mode 0Dh=1
		sar	ax,cl			; sprite shift for non 13h modes
		neg	bx
		jge	@@savespritesz
		add	dx,bx
		sub	di,bx
		xor	bx,bx
		and	al,7			; shift in byte (X%8)
@@savespritesz:	mov	word ptr cursorptr[0],di
		mov	[spritewidth],dx
		neg	al
		add	al,8			; 8+|X| or 8-X%8
		cbw				; OPTIMIZE: instead MOV AH,0
		push	ax bx

;---------- save sprite and draw cursor at new cursor position
		mov	al,0D6h			; screen source
		call	copysprite		; save new sprite

		pop	si cx ax		; spriteshift,,[cursorY]
		les	di,[cursorptr]
		assume	es:nothing
		;mov	dx,[nextrow]
		;xor	bx,bx			; mask offset (2*16 bytes)

@@makeloop:	cmp	ax,[maxcoordY]
		jae	@@makenexty		; jump if Y > max || Y < 0

		push	ax dx bx di
		call	makerow
		pop	di bx dx ax

@@makenexty:	inc	ax
		add	di,dx
		xor	dx,[nextxor]
		movadd	bx,,2
		cmp	bx,2*16
		jb	@@makeloop
;----------
@@drawgrdone:	inc	[cursordrawn]
@@drawgrret:	;j	restorevregs
drawcursor	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Restore graphics card video registers
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

restorevregs	proc
		; if [planar videomode 0Dh-12h] "push ds" else "ret"
		db	?
		POPSEG	es,DGROUP
		mov	bx,offset DGROUP:vdata1
		mov	cx,VDATA1cnt
		j	@writevregs
restorevregs	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Save & update graphics card video registers
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

updatevregs	proc
		; if [planar videomode 0Dh-12h] "push ds" else "ret"
		db	?
		POPSEG	es,DGROUP
		mov	bx,offset DGROUP:vdata1
		mov	cx,VDATA1cnt
		mov	ah,0F4h			; read register set
		int	10h

		mov	bx,offset DGROUP:vdata2
		mov	cx,VDATA2cnt
@writevregs:	mov	ah,0F5h			; write register set
		int	10h
		ret
updatevregs	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Restore old screen content
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

restorescreen	proc
		jcxz	@restret		; exit if cursor was not drawn
		jnz	restoretext		; jump if text mode
		call	updatevregs
		call	restoresprite
		j	restorevregs
restorescreen	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

restoretext	proc
		les	di,[cursorptr]
		assume	es:nothing
		mov	ax,es:[di]
		cmp	ax,textbuf[2]
		jne	@restret
		mov	ax,textbuf[0]
		stosw				; restore old text char attrib
@restret:	ret
restoretext	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

restoresprite	proc
		mov	al,0D7h			; screen destination
		;j	copysprite		; restore old sprite
restoresprite	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Copy screen sprite back and forth
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	AL			(0D6h-scr src./0D7h-scr dst.)
; Out:	DX = [nextrow]
;	BX = 0
; Use:	buffer@, cursorptr
; Modf:	AX, CX, ES
; Call:	none
;
copysprite	proc	C uses si di ds
		cmp	al,0D6h
		mov	NEXTOFFSCODE[1],al
		db	0B8h			; MOV AX,word
cursorY		dw	?			; cursor screen Y pos
		db	0BAh			; MOV DX,word
nextrow		dw	?			; next row offset
		mov	bx,16			; sprite height in rows
		les	di,[buffer@]
		lds	si,[cursorptr]
		assume	ds:nothing,es:nothing
		je	@@copyloop
		push	ds es
		pop	ds es
		xchg	si,di

@@copyloop:	push	dx
		db	03Dh			; CMP AX,word
maxcoordY	dw	?			; screen height
		jae	@@copynexty		; jump if Y > max || Y < 0

		db	0B9h			; MOV CX,word
spritewidth	dw	?			; seen part of sprite in bytes
		sub	dx,cx
		rep	movsb

@@copynexty:	inc	ax
NEXTOFFSCODE	db	01h,?			; ADD SI,DX/ADD DI,DX
		pop	dx
		db	081h,0F2h		; XOR DX,word
nextxor		dw	?			; even times remain unchanged
		dec	bx
		jnz	@@copyloop
		ret
copysprite	endp
		assume	ds:DGROUP

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Transform the cursor mask row to screen
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	BX			(mask offset)
;	CX			(sprite shift when non 13h modes)
;	SI			(sprite shift when mode 13h)
;	ES:DI			(video memory pointer)
; Out:	none
; Use:	screenmask, cursormask
; Modf:	AX, CX, DX, BX, SI, DI
; Call:	none
;
makerow		proc
		assume	es:nothing
		mov	dx,word ptr screenmask[bx]
		mov	bx,word ptr cursormask[bx]
		mov	ax,[spritewidth]
		cmp	[videomode],13h
		jne	makerowno13
;----------
		mov	cx,si
		shl	dx,cl
		shl	bx,cl
		xchg	cx,ax			; OPTIMIZE: instead MOV CX,AX

@@loopx13:	mov	al,0
		shl	dx,1
		jnc	@@chkcmask		; jump if most sign bit zero
		mov	al,es:[di]
@@chkcmask:	shl	bx,1
		jnc	@@putbyte13		; jump if most sign bit zero
		xor	al,0Fh
@@putbyte13:	stosb
		loop	@@loopx13
		ret

;---------- display cursor row in modes other than 13h
makerowno13:	push	cx
		xchg	si,ax			; OPTIMIZE: instead MOV SI,AX
		mov	ax,0FFh
@@maskshift:	stc
		rcl	dx,1
		rcl	al,1			; al:dh:dl shifted screenmask
		shl	bx,1
		rcl	ah,1			; ah:bh:bl shifted cursormask
		loop	@@maskshift
		xchg	dh,bl			; al:bl:dl - ah:bh:dh

@@loopxno13:	push	dx
		cmp	[videomode],0Dh		; is videomode 0Dh-12h?
		jae	@@planar		; jump if multiplanar mode
		and	al,es:[di]
		xor	al,ah
		stosb
		j	@@nextxno13

@@planar:	xchg	cx,ax			; OPTIMIZE: instead MOV CX,AX
		out_	3CEh,5,0		; set write mode
		out_	,3,08h			; data ANDed with latched data
		xchg	es:[di],cl
		out_	,,18h			; data XORed with latched data
		xchg	es:[di],ch
		inc	di

@@nextxno13:	xchg	ax,bx			; OPTIMIZE: instead MOV AX,BX
		pop	bx
		dec	si
		jnz	@@loopxno13
		pop	cx
		ret
makerow		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Return graphic mode video memory offset
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	AX			(Y coordinate in pixels)
; Out:	DI			(video memory offset)
;	SI			(offset to next row)
; Use:	videomode
; Modf:	AX, DX, ES
; Call:	@getoffsret
;
getgroffset	proc
;
;4/5 (320x200x4)   byte offset = (y/2)*80 + (y%2)*2000h + (x*2)/8
;  6 (640x200x2)   byte offset = (y/2)*80 + (y%2)*2000h + x/8
;0Dh (320x200x16)  byte offset = y*40 + x/8, bit offset = 7 - (x % 8)
;0Eh (640x200x16)  byte offset = y*80 + x/8, bit offset = 7 - (x % 8)
;0Fh (640x350x4)   byte offset = y*80 + x/8, bit offset = 7 - (x % 8)
;10h (640x350x16)  byte offset = y*80 + x/8, bit offset = 7 - (x % 8)
;11h (640x480x2)   byte offset = y*80 + x/8, bit offset = 7 - (x % 8)
;12h (640x480x16)  byte offset = y*80 + x/8, bit offset = 7 - (x % 8)
;13h (320x200x256) byte offset = y*320 + x
;HGC (720x348x2)   byte offset = (y%4)*2000h + (y/4)*90 + x/8
;			bit offset = 7 - (x % 8)
;
		MOVSEG	es,0,di,BIOS
		db	0BAh			; MOV DX,word
scanline	dw	?
		mov	si,dx			; [nextrow]
		cmp	byte ptr cursorptr[3],0A0h
		je	@getoffsret		; jump if not videomode 4-6
		mov	si,2000h
		sar	ax,1			; AX=Y/2
		jnc	@getoffsret
		mov	di,si			; DI=(Y%2)*2000h
		mov	si,-(2000h-80)
		j	@getoffsret
getgroffset	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Return text mode video memory offset
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	DI			(video memory offset=row*[0:44Ah]*2+column*2)
; Use:	0:44Ah, 0:44Eh, granpos
; Modf:	AX, DX, ES
; Call:	getpageoffset
;
gettxtoffset	proc
		MOVSEG	es,0,dx,BIOS
		mov	di,[granpos.X]
		mov	al,[bitmapshift]
		dec	ax			; OPTIMIZE: AX instead AL
		xchg	cx,ax
		sar	di,cl			; DI=column*2
		xchg	cx,ax			; OPTIMIZE: instead MOV CX,AX
		mov	ax,[granpos.Y]
		sar	ax,2			; AX=row*2=Y/4
		mov	dx,[VIDEO_width]	; screen width

@getoffsret:	imul	dx			; AX=row*screen width
		add	ax,[VIDEO_pageoff]	; add video page offset
		add	di,ax
		ret
gettxtoffset	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Check if cursor in update region
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

checkifseen	proc
		db	0BAh			; MOV DX,word
cursorwidth	db	?,0

;---------- check if cursor shape seen on the screen
		add	si,ax
		jle	@@retunseen
		add	dx,bx
		jle	@@retunseen
		cmp	ax,[maxcoordY]
		jge	@@retunseen
		cmp	bx,[maxcoordX]
		jge	@@retunseen

;---------- check if cursor shape intersects with update region
		cmp	ax,[lowright.Y]
		jg	@@retseen
		cmp	bx,[lowright.X]
		jg	@@retseen
		cmp	si,[upleft.Y]
		jle	@@retseen
		cmp	dx,[upleft.X]
		jle	@@retseen

@@retunseen:	stc
		ret
@@retseen:	clc
		ret
checkifseen	endp


;€€€€€€€€€€€€€€€€€€€€€€€€ INT 33 HANDLER SERVICES €€€€€€€€€€€€€€€€€€€€€€€€€

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

setupvideo	proc
		mov	si,LenClearArea2/2	; clear area 2
ERRIF (LenClearArea2 mod 2 ne 0) "LenClearArea2 must be even!"
		j	@setvideo
setupvideo	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 21 - Software reset
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	[AX] = 21h/FFFFh	(not installed/installed)
;	[BX] = 2/3/FFFFh	(number of buttons)
; Use:	0:449h, 0:44Ah, 0:463h, 0:484h, 0:487h, 0:488h, 0:4A8h, shape
; Modf:	StartSaveArea, videomode, nextxor, buffer@, restorevregs, updatevregs,
;	@graphcursor, scanline, maxcoordX, maxcoordY, granumask, cursorptr,
;	bitmapshift, cursorwidth, rangemax, StartClearArea
; Call:	hidecursor, @savecutpos
;
softreset_21	proc
		mov	[_ARG_BX_],3
buttonscnt	equ	byte ptr [$-2]		; buttons count (2/3)
		mov	[_ARG_AX_],0FFFFh
						; restore default area
		memcopy	LenDefArea,ds,DGROUP,DGROUP:StartSaveArea,ds,,DGROUP:StartDefArea
		call	hidecursor
		mov	si,LenClearArea3/2	; clear area 3
ERRIF (LenClearArea3 mod 2 ne 0) "LenClearArea3 must be even!"

;---------- setup video regs values for current video mode
@setvideo:	push	si
		MOVSEG	es,ds,,DGROUP
		MOVSEG	ds,0,ax,BIOS
		mov	ax,[CRTC_base]		; base IO address of CRTC
		mov	videoregs@[0].port@,ax	; 3D4h/3B4h
		add	ax,6			; Feature Control register
		mov	videoregs@[(SIZE RGROUPDEF)*5].port@,ax
		mov	al,[VIDEO_mode]		; current video mode
		push	ax
;----------
		mov	ah,9
		cmp	al,11h			; VGA videomodes?
		jae	@@getshift

		cbw				; OPTIMIZE: instead MOV AH,0
		cmp	al,0Fh			; 0F-10 videomodes?
		jb	@@checkcga
		testflag [VIDEO_control],mask VCTRL_RAM_64K
		jz	@@getshift		; jump if only 64K of VRAM
		mov	ah,2
		j	@@getshift

@@checkcga:	cmp	al,4			; not color text modes?
		jae	@@getshift
		mov	cl,[VIDEO_switches]	; get display combination
		maskflag cl,mask VIDSW_feature0+mask VIDSW_display
		cmp	cl,9			; EGA+ECD/MDA?
		je	@@lines350
		cmp	cl,3			; MDA/EGA+ECD?
		jne	@@getshift
@@lines350:	mov	ah,13h
;----------
@@getshift:	lds	si,[VIDEO_ptrtable@]
		assume	ds:nothing
		lds	si,[si].VIDEO_paramtbl@
		add	ah,al
		mov	al,(offset VPARAM_SEQC) shl 2
		shr	ax,2
		add	si,ax			; SI += (AL+AH)*64+5

		mov	di,offset DGROUP:StartDefVRegsArea
		push	di
		mov	al,3
		stosb				; def_SEQ[0]=3
		memcopy	50			; copy default registers value
		mov	al,0
		stosb				; def_ATC[20]=0; VGA only
		memcopy	9			; def_GRC
		;mov	ah,0
		stosw				; def_FC=0, def_GPOS1=0
		inc	ax			; OPTIMIZE: instead MOV AL,1
		stosb				; def_GPOS2=1

		pop	si			; copy current registers value
		memcopy	LenVRegsArea,,,DGROUP:StartVRegsArea,es,DGROUP

		dec	ax			; OPTIMIZE: instead MOV AL,0
		mov	cx,8
		mov	di,offset DGROUP:videoregs@[0].rmodify?
@@setmodify:	stosb
		add	di,(SIZE RGROUPDEF)-1
		loop	@@setmodify

;---------- set parameters for current video mode
; mode	 seg   screen  cell scan planar  VX/
;			    line	byte
;  0	B800h  640x200 16x8   -    -	  -
;  1	B800h  640x200 16x8   -    -	  -
;  2	B800h  640x200	8x8   -    -	  -
;  3	B800h  640x200	8x8   -    -	  -
;  4	B800h  640x200	2x1  80   no	  8
;  5	B800h  640x200	2x1  80   no	  8
;  6	B800h  640x200	1x1  80   no	  8
;  7	B000h  640x200	8x8   -    -	  -
; 0Dh	A000h  320x200	2x1  40    +	 16
; 0Eh	A000h  640x200	1x1  80    +	  8
; 0Fh	A000h  640x350	1x1  80    +	  8
; 10h	A000h  640x350	1x1  80    +	  8
; 11h	A000h  640x480	1x1  80    +	  8
; 12h	A000h  640x480	1x1  80    +	  8
; 13h	A000h  320x200	2x1 320   no	  2
;
		pop	ax			; current video mode
; mode 0-3
		mov	bx,0B8C3h		; B800h: 0-3/RET opcode
		mov	cx,0304h		; 16x8: 0-1
		mov	dx,0FFFFh
		mov	di,200			; x200: 4-6, 0Dh-0Eh, 13h
		cmp	al,2
		jb	@@settext
		dec	cx			; 8x8: 2-3, 7
		cmp	al,4
		jb	@@settext
; mode 7
		cmp	al,7
		jne	@@checkgraph
		mov	bh,0B0h			; B000h: 7

@@settext:	mov	ch,1
		mov	dh,-8
		shl	dl,cl

		MOVSEG	es,0,ax,BIOS
		add	al,[VIDEO_height]	; screen height-1
		jz	@@screenw		; zero on old machines
		inc	ax			; OPTIMIZE: AX instead AL
IF USE_286
		shl	ax,3
ELSE
		mov	ah,8
		mul	ah
ENDIF
		xchg	di,ax			; OPTIMIZE: instead MOV DI,AX

@@screenw:	mov	ax,[VIDEO_width]	; screen width
		j	@@setcommon

; mode 4-6
@@checkgraph:	;mov	cx,0303h		; sprite: 3 bytes/row
		;mov	dx,0FFFFh		; 1x1: 6, 0Eh-12h
		;mov	bx,0B8C3h		; B800h: 4-6/RET opcode
		push	bx
		;mov	di,200			; x200: 4-6, 0Dh-0Eh, 13h
		mov	si,2000h xor -(2000h-80) ; [nextxor] for modes 4-6
		;MOVSEG	es,ds,,DGROUP
		db	0BBh			; MOV BX,word
BUFFADDR	dw	offset DGROUP:bufferPS2
		cmp	al,6
		je	@@setgraphics
		jb	@@set2x1
; mode 8-
		pop	bx
		mov	bh,0A0h			; A000h: 0Dh-

; for modes 0Dh-13h we'll be storing the cursor shape and the hided
; screen contents at free space in video memory to save driver memory

		MOVSEG	es,0A000h,si,nothing
		xor	si,si			; [nextxor] for modes >6
		cmp	al,13h
		ja	@@nonstandard
		jb	@@mode0812
; mode 13h
		push	bx
		mov	bx,0FA00h		; =320*200
		mov	cx,1000h		; sprite: 16 bytes/row
@@set320:	inc	cx			; OPTIMIZE: instead INC CL
@@set2x1:	dec	dx			; OPTIMIZE: instead MOV DL,-2
		j	@@setgraphics
; mode 8-0Dh
@@mode0812:	cmp	al,0Dh
		jb	@@nonstandard
		mov	bl,1Eh			; PUSH DS opcode
		push	bx
		mov	bx,3E82h		; 16002: 0Dh-0Eh
		je	@@set320
; mode 0Eh-12h
		cmp	al,0Fh
		jb	@@setgraphics
		mov	di,350			; x350: 0Fh-10h
		mov	bh,7Eh			; 32386: 0Fh-10h
		cmp	al,11h
		jb	@@setgraphics
		mov	di,480			; x480: 11h-12h
		mov	bh,9Eh			; 40578: 11h-12h

@@setgraphics:	mov	[videomode],al
		mov	[nextxor],si
		saveFAR	[buffer@],es,bx
		pop	ax			; multiplanar modes opcodes
		mov	byte ptr [restorevregs],al
		mov	byte ptr [updatevregs],al
		xchg	bx,ax			; OPTIMIZE: instead MOV BH,AH
		mov	al,51h			; PUSH CX opcode
		j	@@setgcommon

@@nonstandard:

;;+++++ for text modes: bh := 0B8h, dl := 0FFh, cl := 3, j @@settext

		;mov	dx,0FFFFh
		;mov	di,200
		mov	al,0C3h			; RET opcode

@@setgcommon:	mov	byte ptr [@graphcursor],al
		mov	ax,640			; [maxcoordX]
		shr	ax,cl
		mov	[scanline],ax		; screen line width in bytes

@@setcommon:	shl	ax,cl
		mov	[maxcoordX],ax
		mov	[maxcoordY],di
		mov	byte ptr [granumask.X],dl
		mov	byte ptr [granumask.Y],dh
		mov	byte ptr cursorptr[3],bh
		mov	[bitmapshift],cl	; log2(screen/memory ratio)
						;  (mode 13h=1, 0-1/0Dh=4, other=3)
		shl	ch,cl
		mov	[cursorwidth],ch
		pop	si

;---------- set ranges and center cursor (AX=maxcoordX, DI=maxcoordY)
		mov	cx,ax
		dec	ax
		mov	[rangemax.X],ax		; set right X range
		shr	cx,1			; X middle

		mov	dx,di
		dec	di
		mov	[rangemax.Y],di		; set lower Y range
		shr	dx,1			; Y middle

;---------- set cursor position (CX=X, DX=Y, SI=area size to clear)
@setpos:	;cli
		MOVSEG	es,ds,,DGROUP
		mov	di,offset DGROUP:StartClearArea
		xchg	cx,si
		xor	ax,ax
		rep	stosw

		xchg	ax,dx			; OPTIMIZE: instead MOV AX,DX
		MOVREG_	bx,<offset Y>
		call	@savecutpos
		xchg	ax,si			; OPTIMIZE: instead MOV AX,SI
		MOVREG_	bl,<offset X>		; OPTIMIZE: BL instead BX
		jmp	@savecutpos
softreset_21	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 1F - Disable mouse driver
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	[AX] = 1Fh/FFFFh	(success/unsuccess)
;	[ES:BX]			(old int33 handler)
; Use:	oldint33, oldint10
; Modf:	AX, CX, DX, BX, DS, ES, disabled?, cursorhidden
; Call:	INT 21/35, INT 21/25, disablePS2/disableUART, hidecursor
;
disabledrv_1F	proc
		les	ax,[oldint33]
		assume	es:nothing
		mov	[_ARG_ES_],es
		mov	[_ARG_BX_],ax

		db	0E8h			; CALL NEAR to disableproc
disableproc	dw	disablePS2-$-2

		mov	al,0
		mov	[buttstatus],al
		inc	ax			; OPTIMIZE: instead MOV AL,1
		cmp	[disabled?],al
		je	@@disabled
		mov	[cursorhidden],al	; normalize flag
		call	hidecursor

;---------- check if INT 33 or INT 10 were intercepted
;	    (i.e. handlers segments not equal to CS)
		mov	cx,cs
		DOSGetIntr 33h
		mov	dx,es
		cmp	dx,cx
		jne	althandler_18

		;mov	ah,35h
		mov	al,10h
		int	21h
		movsub	ax,es,cx
		jne	althandler_18

		inc	ax			; OPTIMIZE: instead MOV AL,1
		mov	[disabled?],al
		lds	dx,[oldint10]
		assume	ds:nothing
		DOSSetIntr 10h			; restore old INT 10 handler
@@disabled:	ret
disabledrv_1F	endp
		assume	ds:DGROUP

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 18 - Set alternate user interrupt handler
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(call mask)
;	ES:DX			(FAR routine)
; Out:	[AX] = 18h/FFFFh	(success/unsuccess)
;
althandler_18	proc
		assume	es:nothing
		mov	[_ARG_AX_],0FFFFh
		ret
althandler_18	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 19 - Get alternate user interrupt handler
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(call mask)
; Out:	[CX]			(0=not found)
;	[BX:DX]			(FAR routine)
;
althandler_19	proc
		mov	[_ARG_CX_],0
		ret
althandler_19	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 00 - Reset driver and read status
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	[AX] = 0/FFFFh		(not installed/installed)
;	[BX] = 2/3/FFFFh	(number of buttons)
; Use:	none
; Modf:	none
; Call:	softreset_21, enabledriver_20
;
resetdriver_00	proc
		call	softreset_21
		;j	enabledriver_20
resetdriver_00	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 20 - Enable mouse driver
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	[AX] = 20h/FFFFh	(success/unsuccess)
; Use:	none
; Modf:	AX, CX, DX, BX, ES, disabled?, oldint10
; Call:	INT 21/35, INT 21/25, setupvideo, enablePS2/enableUART
;
enabledriver_20	proc
		xor	cx,cx
		xchg	cl,[disabled?]
		jcxz	@@enabled

;---------- set new INT 10 handler
		DOSGetIntr 10h
		saveFAR	[oldint10],es,bx
		;mov	al,10h
		DOSSetIntr ,,,DGROUP:int10handler
;----------
@@enabled:	call	setupvideo
		db	0E9h			; JMP NEAR to enableproc
enableproc	dw	enablePS2-$-2
enabledriver_20	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 03 - Get cursor position and buttons status
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	[BX]			(buttons status)
;	[CX]			(X - column)
;	[DX]			(Y - row)
; Use:	buttstatus, granpos
; Modf:	AX, CX, DX
; Call:	@retBCDX
;
status_03	proc
		;mov	ah,0
		mov	al,[buttstatus]
		mov	cx,[granpos.X]
		mov	dx,[granpos.Y]
		j	@retBCDX
status_03	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 05 - Get button press data
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	BX			(button number)
; Out:	[AX]			(buttons status)
;	[BX]			(press times)
;	[CX]			(last press X)
;	[DX]			(last press Y)
; Use:	none
; Modf:	SI, buttpress
; Call:	@retbuttstat
;
pressdata_05	proc
		mov	si,offset DGROUP:buttpress
		j	@retbuttstat
pressdata_05	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 06 - Get button release data
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	BX			(button number)
; Out:	[AX]			(buttons status)
;	[BX]			(release times)
;	[CX]			(last release X)
;	[DX]			(last release Y)
; Use:	buttstatus
; Modf:	AX, CX, DX, BX, SI, buttrelease
; Call:	none
;
releasedata_06	proc
		mov	si,offset DGROUP:buttrelease
@retbuttstat:	;mov	ah,0
		mov	al,[buttstatus]
		mov	[_ARG_AX_],ax
IF FOOLPROOF
		cmp	bx,2
		ja	@@ret56
ENDIF
ERRIF (6 ne SIZE BUTTLASTSTATE) "BUTTLASTSTATE structure size changed!"
		shl	bx,1
		add	si,bx			; SI+BX=buttrelease
		shl	bx,1			;  +button*SIZE BUTTLASTSTATE
		xor	ax,ax
		xchg	[si+bx.counter],ax
		mov	cx,[si+bx.lastcol]
		mov	dx,[si+bx.lastrow]
@retBCDX:	mov	[_ARG_DX_],dx
		mov	[_ARG_CX_],cx
@retBX:		mov	[_ARG_BX_],ax
@@ret56:	ret
releasedata_06	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0B - Get motion counters
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	[CX]			(number of mickeys mouse moved
;	[DX]			 horizontally/vertically since last call)
; Use:	none
; Modf:	mickeys
; Call:	@retBCDX
;
mickeys_0B	proc
		xchg	ax,bx			; OPTIMIZE: instead MOV AX,BX
		xor	cx,cx
		xor	dx,dx
		xchg	[mickeys.X],cx
		xchg	[mickeys.Y],dx
		j	@retBCDX
mickeys_0B	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 15 - Get driver storage requirements
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	[BX]			(buffer size)
; Use:	LenSaveArea
; Modf:	AX
; Call:	@retBX
;
storagereq_15	proc
		mov	ax,LenSaveArea
		j	@retBX
storagereq_15	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 1B - Get mouse sensitivity
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	[BX]			()
;	[CX]			()
;	[DX]			(speed threshold in mickeys/second)
; Use:	/doublespeed/
; Modf:	/AX/, /CX/, /DX/
; Call:	@retBCDX
;
sensitivity_1B	proc
;;+*		mov	ax,
;;+*		mov	cx,
;;+*		mov	dx,[doublespeed]
		j	@retBCDX
sensitivity_1B	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 1E - Get display page
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	[BX]			(display page number)
; Use:	0:462h
; Modf:	AX, DS
; Call:	@retBX
;
videopage_1E	proc
		MOVSEG	ds,0,ax,BIOS
		mov	al,[VIDEO_pageno]
		j	@retBX
videopage_1E	endp
		assume	ds:DGROUP

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 01 - Show mouse cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	none
; Modf:	AX, lowright.Y, cursorhidden
; Call:	@showcursor, @showcheck
;
showcursor_01	proc
		mov	byte ptr lowright.Y[1],-1 ; "disable" update region
		cmp	[cursorhidden],ah	; OPTIMIZE: AH instead 0
		jz	@showcursor		; jump if show already enabled
		dec	[cursorhidden]
		j	@showcheck
showcursor_01	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 02 - Hide mouse cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	none
; Modf:	cursorhidden
; Call:	hidecursor
;
hidecursor_02	proc
		inc	[cursorhidden]
IF FOOLPROOF
		jnz	hidecursor		; jump if counter not overflow
		dec	[cursorhidden]
ENDIF
		;j	hidecursor
hidecursor_02	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

hidecursor	proc
		mov	ax,restorescreen-videoqueue-2
		j	@cursorproc
hidecursor	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 07 - Set horizontal cursor range
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(min X)
;	DX			(max X)
; Out:	none
; Use:	none
; Modf:	BX
; Call:	@setnewrange
;
hrange_07	proc
		MOVREG_	bx,<offset X>
		j	@setnewrange
hrange_07	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 08 - Set vertical cursor range
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(min Y)
;	DX			(max Y)
; Out:	none
; Use:	pos
; Modf:	CX, DX, BX, rangemin, rangemax
; Call:	setpos_04
;
vrange_08	proc
		MOVREG_	bx,<offset Y>
IF FOOLPROOF
@setnewrange:	xchg	ax,cx			; OPTIMIZE: instead MOV AX,CX
		cmp	ax,dx
		jle	@@saverange
		xchg	ax,dx
@@saverange:	mov	word ptr rangemin[bx],ax
ELSE
@setnewrange:	mov	word ptr rangemin[bx],cx
ENDIF
		mov	word ptr rangemax[bx],dx
		mov	cx,[pos.X]
		mov	dx,[pos.Y]
		;j	setpos_04
vrange_08	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 04 - Position mouse cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(X - column)
;	DX			(Y - row)
; Out:	none
; Use:	none
; Modf:	SI
; Call:	@setpos, refreshcursor
;
setpos_04	proc
		mov	si,LenClearArea1/2	; clear area 1
ERRIF (LenClearArea1 mod 2 ne 0) "LenClearArea1 must be even!"
		call	@setpos
		;j	refreshcursor
setpos_04	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

refreshcursor	proc
		cmp	[cursorhidden],0
@showcheck:	jnz	@showret		; exit if cursor hidden

@showcursor:	mov	ax,drawcursor-videoqueue-2
@cursorproc:	mov	[videoqueue],ax		; place request to queue
		sub	[videolock],1
		jc	@showret		; -1 drawing in progress
		js	@@showdone		; <-1 queue already used

@@vqueueloop:	sti
		xor	cx,cx
		mov	ax,[granumask.Y]
		xchg	cl,[cursordrawn]	; mark cursor as not drawn
		inc	ax			; Zero flag if graphics
						; CX=old cursordrawn
		db	0E8h			; CALL NEAR to videoqueue
videoqueue	dw	?			; last cursor request

@@showdone:	inc	[videolock]		; drawing stopped
		jz	@@vqueueloop		; loop if queue was not empty
@showret:	ret
refreshcursor	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 09 - Define graphics cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	BX			(hot spot X)
;	CX			(hot spot Y)
;	ES:DX			(pointer to bitmaps)
; Out:	none
; Use:	none
; Modf:	AX, CX, BX, SI, DI, ES, hotspot, screenmask, cursormask
; Call:	@showret, hidecursor, refreshcursor
;
graphcursor_09	proc
		assume	es:nothing
;---------- compare user shape with internal area
		mov	si,offset DGROUP:hotspot
		lodsw
		cmp	ax,bx
		jne	@@redrawgraph
		lodsw
		xor	ax,cx
		jne	@@redrawgraph
		mov	di,dx
		;mov	ah,0
		mov	al,16+16
		xchg	ax,cx
		repe	cmpsw
		je	@showret
		xchg	cx,ax			; OPTIMIZE: instead MOV CX,AX

;---------- copy user shape to internal area
@@redrawgraph:	push	ds ds es
		pop	ds es
		mov	di,offset DGROUP:hotspot
		xchg	ax,bx			; OPTIMIZE: instead MOV AX,BX
		stosw
		xchg	ax,cx			; OPTIMIZE: instead MOV AX,CX
		stosw
		memcopy	2*(16+16),,,,,,dx
		pop	ds

redrawscreen:	call	hidecursor
		j	refreshcursor
graphcursor_09	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0A - Define text cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	BX			(0 - SW, else HW text cursor)
;	CX			(screen mask/start scanline)
;	DX			(cursor mask/end scanline)
; Out:	none
; Use:	none
; Modf:	AX, CX, BX, cursortype, startscan, endscan
; Call:	INT 10/01, @showret, redrawscreen
;
textcursor_0A	proc
		xchg	cx,bx
		jcxz	@@difftext		; jump if software cursor

		mov	ch,bl
		mov	cl,dl
		mov	ah,1
		int	10h			; set cursor shape & size
		mov	cl,1

@@difftext:	cmp	cl,[cursortype]
		jne	@@redrawtext
		cmp	bx,[startscan]
		jne	@@redrawtext
		cmp	dx,[endscan]
		je	@showret		; exit if cursor not changed

@@redrawtext:	mov	[cursortype],cl
		mov	[startscan],bx
		mov	[endscan],dx
		j	redrawscreen
textcursor_0A	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 10 - Define screen region for updating
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX, DX			(X/Y of upper left corner)
;	SI, DI			(X/Y of lower right corner)
; Out:	none
; Use:	none
; Modf:	AX, CX, DX, DI, upleft, lowright
; Call:	redrawscreen
;
updateregion_10	proc
		mov	ax,[_ARG_SI_]
IF FOOLPROOF
		cmp	cx,ax
		jle	@@regionX
		xchg	cx,ax
@@regionX:	mov	[upleft.X],cx
		mov	[lowright.X],ax
		xchg	ax,di			; OPTIMIZE: instead MOV AX,DI
		cmp	dx,ax
		jle	@@regionY
		xchg	dx,ax
@@regionY:	mov	[upleft.Y],dx
		mov	[lowright.Y],ax
ELSE
		mov	[upleft.X],cx
		mov	[upleft.Y],dx
		mov	[lowright.X],ax
		mov	[lowright.Y],di
ENDIF
		j	redrawscreen
updateregion_10	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 16 - Save driver state
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	BX			(buffer size)
;	ES:DX			(buffer)
; Out:	none
; Use:	StartSaveArea
; Modf:	CX, SI, DI
; Call:	none
;
savestate_16	proc
		assume	es:nothing
IF FOOLPROOF
;-		cmp	bx,LenSaveArea		;!!! TurboPascal IDE
;-		jb	@stateret		;  workaround: it not init BX
ENDIF
		memcopy	LenSaveArea,,,dx,,,DGROUP:StartSaveArea
@stateret:	ret
savestate_16	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 17 - Restore driver state
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	BX			(buffer size)
;	ES:DX			(saved state buffer)
; Out:	none
; Use:	none
; Modf:	SI, DI, DS, ES, StartSaveArea
; Call:	@stateret, hidecursor, refreshcursor
;
restorestate_17	proc
		assume	es:nothing
IF FOOLPROOF
;-		cmp	bx,LenSaveArea		;!!! TurboPascal IDE
;-		jb	@stateret		;  workaround: it not init BX
ENDIF
;---------- do nothing if SaveArea is not changed
;;+*		mov	si,offset DGROUP:StartSaveArea
;;+*		mov	di,dx
;;+*		mov	cx,LenSaveArea/2
ERRIF (LenSaveArea mod 2 ne 0) "LenSaveArea must be even!"
;;+*		repe	cmpsw
;;+*		je	@stateret

;---------- change SaveArea
		push	es dx
		call	hidecursor
		MOVSEG	es,ds,,DGROUP
		pop	si ds
		assume	ds:nothing
		memcopy	LenSaveArea,,,DGROUP:StartSaveArea
		jmp	refreshcursor
restorestate_17	endp
		assume	ds:DGROUP

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0D - Light pen emulation On
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	none
; Modf:	none
; Call:	lightpenoff_0E
;
;;+*lightpenon_0D	proc
;;+*		mov	al,0
;;+*		;j	lightpenoff_0E
;;+*lightpenon_0D	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0E - Light pen emulation Off
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	none
; Modf:	nolightpen?
; Call:	none
;
;;+*lightpenoff_0E	proc
;;+*		mov	[nolightpen?],al	; OPTIMIZE: AL instead nonzero
;;+*		ret
;;+*lightpenoff_0E	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 14 - Exchange user interrupt routines
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(new call mask)
;	ES:DX			(new FAR routine)
; Out:	[CX]			(old call mask)
;	[ES:DX]			(old FAR routine)
; Use:	callmask, UIR@
; Modf:	AX
; Call:	UIR_0C
;
exchangeUIR_14	proc
		assume	es:nothing
		;mov	ah,0
		mov	al,[callmask]
		mov	[_ARG_CX_],ax
		mov	ax,word ptr UIR@[0]
		mov	[_ARG_DX_],ax
		mov	ax,word ptr UIR@[2]
		mov	[_ARG_ES_],ax
		;j	UIR_0C
exchangeUIR_14	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0C - Define user interrupt routine
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(call mask)
;	ES:DX			(FAR routine)
; Out:	none
; Use:	none
; Modf:	UIR@, callmask
; Call:	none
;
UIR_0C		proc
		assume	es:nothing
		saveFAR [UIR@],es,dx
		mov	[callmask],cl
		ret
UIR_0C		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0F - Set mickeys/pixels ratios
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(number of mickeys per 8 pix
;	DX			 horizontally/vertically)
; Out:	none
; Use:	none
; Modf:	mickey8
; Call:	none
;
sensitivity_0F	proc
IF FOOLPROOF
		test	dx,dx
		jz	@@ret0F			; ignore wrong ratio
		jcxz	@@ret0F			; ignore wrong ratio
ENDIF
		mov	[mickey8.X],cx
		mov	[mickey8.Y],dx
@@ret0F:	ret
sensitivity_0F	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 1A - Set mouse sensitivity
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	BX			(ignored)
;	CX			(ignored)
;	DX			(speed threshold in mickeys/second, ignored)
; Out:	none
; Use:	none
; Modf:	none
; Call:	doublespeed_13
;
sensitivity_1A	proc
		;j	doublespeed_13
sensitivity_1A	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 13 - Define double-speed threshold
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	DX			(speed threshold in mickeys/second)
; Out:	none
; Use:	none
; Modf:	/DX/, /doublespeed/
; Call:	none
;
doublespeed_13	proc
;;+*		test	dx,dx
;;+*		jnz	@@savespeed
;;+*		mov	dl,64
;;+*@@setspeed:	mov	[doublespeed],dx
		;ret
doublespeed_13	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0D 0E 11 12 18 19 1C 1D - Null function for not implemented calls
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

nullfunc	proc
		ret
nullfunc	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;				INT 33 handler
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

		evendata
handler33table	dw	offset DGROUP:resetdriver_00
		dw	offset DGROUP:showcursor_01
		dw	offset DGROUP:hidecursor_02
		dw	offset DGROUP:status_03
		dw	offset DGROUP:setpos_04
		dw	offset DGROUP:pressdata_05
		dw	offset DGROUP:releasedata_06
		dw	offset DGROUP:hrange_07
		dw	offset DGROUP:vrange_08
		dw	offset DGROUP:graphcursor_09
		dw	offset DGROUP:textcursor_0A
		dw	offset DGROUP:mickeys_0B
		dw	offset DGROUP:UIR_0C
		dw	offset DGROUP:nullfunc	;lightpenon_0D
		dw	offset DGROUP:nullfunc	;lightpenoff_0E
		dw	offset DGROUP:sensitivity_0F
		dw	offset DGROUP:updateregion_10
		dw	offset DGROUP:nullfunc	;11 - genius driver only
		dw	offset DGROUP:nullfunc	;12 - large graphics cursor
		dw	offset DGROUP:doublespeed_13
		dw	offset DGROUP:exchangeUIR_14
		dw	offset DGROUP:storagereq_15
		dw	offset DGROUP:savestate_16
		dw	offset DGROUP:restorestate_17
		dw	offset DGROUP:althandler_18
		dw	offset DGROUP:althandler_19
		dw	offset DGROUP:sensitivity_1A
		dw	offset DGROUP:sensitivity_1B
		dw	offset DGROUP:nullfunc	;1C - InPort mouse only
		dw	offset DGROUP:nullfunc	;1D - define display page #
		dw	offset DGROUP:videopage_1E
		dw	offset DGROUP:disabledrv_1F
		dw	offset DGROUP:enabledriver_20
		dw	offset DGROUP:softreset_21

handler33	proc
		assume	ds:nothing,es:nothing
		cld
		test	ah,ah
		jnz	@@iret
		push	ds
		MOVSEG	ds,cs,,DGROUP
		cmp	al,21h
		ja	language_23
		push	es
		PUSHALL
		mov	si,ax			;!!! AX must be unchanged
		mov	bp,sp
		shl	si,1
		call	handler33table[si]	; call by calculated offset
@rethandler:	POPALL
		pop	es ds
@@iret:		iret
		assume	ds:DGROUP

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 23 - Get language for messages
; Out:	[BX]			(language code: 0 - English)
;
language_23:	cmp	al,23h
		je	@iretBX0

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 24 - Get software version, mouse type and IRQ
; Out:	[AX] = 24h/FFFFh	(installed/error)
;	[BX]			(version)
;	[CL]			(IRQ #/0=PS/2)
;	[CH] = 1=bus/2=serial/3=InPort/4=PS2/5=HP (mouse type)
; Use:	driverversion
;
version_24:	cmp	al,24h
		jne	no_version_24
		mov	bx,driverversion
		db	0B9h			; MOV CX,word
mouseinfo	db	?,4
no_version_24:

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 26 - Get maximum virtual screen coordinates
; Out:	[BX]			(mouse disabled flag)
;	[CX]			(max virtual screen X)
;	[DX]			(max virtual screen Y)
; Use:	maxcoordY
;
maxscreen_26:	cmp	al,26h
		jne	no_maxscreen_26
		db	0BBh			; MOV BX,word
disabled?	db	1,0			; 1 - driver disabled
		db	0B9h			; MOV CX,word
maxcoordX	dw	?			; screen width
		mov	dx,[maxcoordY]
		dec	cx
		dec	dx
no_maxscreen_26:

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 27 - Get screen/cursor masks and mickey counters
; Out:	[AX]			(screen mask/start scanline)
;	[BX]			(cursor mask/end scanline)
;	[CX]			(number of mickeys mouse moved
;	[DX]			 horizontally/vertically since last call)
; Modf:	mickeys
;
;;+*cursor_27:	cmp	al,27h
;;+*		jne	no_cursor_27
;;+*		mov	ax,[startscan]
;;+*		mov	bx,[endscan]
;;+*		xor	cx,cx
;;+*		xor	dx,dx
;;+*		xchg	[mickeys.X],cx
;;+*		xchg	[mickeys.Y],dx
;;+*		pop	ds
;;+*		iret
;;+*no_cursor_27:

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 31 - Get current virtual cursor coordinates
; Out:	[AX]			(min virtual cursor X)
;	[BX]			(min virtual cursor Y)
;	[CX]			(max virtual cursor X)
;	[DX]			(max virtual cursor Y)
; Use:	rangemin, rangemax
;
cursrange_31:	cmp	al,31h
		jne	no_cursrange_31
		mov	ax,[rangemin.X]
		mov	bx,[rangemin.Y]
		lds	cx,[rangemax]
		mov	dx,ds
		pop	ds
		iret
no_cursrange_31:

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 32 - Get active advanced functions
; Out:	[AX]			(active functions flag)
;	[CX] = 0
;	[DX] = 0
;	[BX] = 0
;
active_32:	cmp	al,32h
		jne	no_active_32
		mov	ax,0100010000001100b	; active: 26 2A 31 32
		xor	cx,cx
		xor	dx,dx
@iretBX0:	xor	bx,bx
		pop	ds
		iret
no_active_32:

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 4D - Get pointer to copyright string
; Out:	[ES:DI]			(copyright string)
; Use:	IDstring
;
copyright_4D:	cmp	al,4Dh
		jne	no_copyright_4D
		MOVSEG	es,ds,,DGROUP
		mov	di,offset DGROUP:IDstring
no_copyright_4D:

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 6D - Get pointer to version
; Out:	[ES:DI]			(version string)
; Use:	msversion
;
version_6D:	cmp	al,6Dh
		jne	no_version_6D
		MOVSEG	es,ds,,DGROUP
		mov	di,offset DGROUP:msversion
no_version_6D:

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 2A - Get cursor hot spot
; Out:	[AX]			(cursor visibility counter)
;	[BX]			(hot spot X)
;	[CX]			(hot spot Y)
;	[DX] = 1=bus/2=serial/3=InPort/4=PS2/5=HP (mouse type)
; Use:	cursorhidden, hotspot
;
hotspot_2A:	cmp	al,2Ah
		jne	@@iretDS
		;mov	ah,0
		mov	al,[cursorhidden]
		mov	bx,[hotspot.X]
		mov	cx,[hotspot.Y]
		db	0BAh			; MOV DX,word
mouseinfo1	db	4,0
@@iretDS:	pop	ds
		iret
handler33	endp

;€€€€€€€€€€€€€€€€€€€€€€€€€ END OF INT 33 SERVICES €€€€€€€€€€€€€€€€€€€€€€€€€


RILversion	label
msversion	db	driverversion / 100h,driverversion mod 100h
IDstring	db	'CuteMouse ',CTMVER,0
IDstringlen = $ - IDstring

TSRend		label


;€€€€€€€€€€€€€€€€€€€€€€€€ INITIALIZATION PART DATA €€€€€€€€€€€€€€€€€€€€€€€€

.const

messages segment virtual ; place at the end of current segment
INCLUDE ctmouse.msg
messages ends

S_mousetype	dw	DGROUP:S_atPS2
		dw	DGROUP:S_inMSYS
		dw	DGROUP:S_inLT
		dw	DGROUP:S_inMS

.data

options		db	0
OPT_PS2		equ	00000001b
OPT_SERIAL	equ	00000010b
OPT_COMforced	equ	00000100b
OPT_PS2after	equ	00001000b
OPT_3button	equ	00010000b
OPT_NOMSYS	equ	00100000b
OPT_LEFTHAND	equ	01000000b
OPT_NOUMB	equ	10000000b


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€ Real Start €€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

.code

real_start:	cld
		DOSGetIntr 33h
		saveFAR [oldint33],es,bx	; save old INT 33h handler

;---------- parse command line and find mouse
@@start:	DOSWriteS ,,DGROUP:Copyright	; 'Cute Mouse Driver'
		mov	si,offset PSP:cmdline_len
		lodsb
		cbw				; OPTIMIZE: instead MOV AH,0
		mov	bx,ax
		mov	[si+bx],ah		; OPTIMIZE: AH instead 0
		call	commandline		; examine command line

		mov	cx,word ptr oldint33[2]
		jcxz	@@startfind
		mov	ax,1Fh			; disable old driver
		pushf				;!!! Logitech MouseWare
		call	[oldint33]		;  Windows driver workaround
;----------
@@startfind:	mov	al,[options]
		testflag al,OPT_PS2+OPT_SERIAL
		jnz	@@find1COM
		setflag	al,OPT_PS2+OPT_SERIAL

@@find1COM:	testflag al,OPT_PS2after
		jz	@@findPS2
		call	searchCOM
		jnc	@@serialfound

@@findPS2:	testflag al,OPT_PS2+OPT_PS2after
		jz	@@find2COM
		push	ax
		call	checkPS2
		pop	ax
		jc	@@find2COM
		mov	mouseinfo[0],bh
		j	@@mousefound

@@find2COM:	testflag al,OPT_PS2after
		jnz	@@devnotfound
		testflag al,OPT_SERIAL+OPT_NOMSYS
		jz	@@devnotfound
		call	searchCOM		; call if no /V and serial
		jnc	@@serialfound

@@devnotfound:	mov	dx,offset DGROUP:E_notfound ; 'Error: device not found'
		jmp	EXITENABLE

@@serialfound:	mov	al,2
		mov	mouseinfo[1],al
		mov	[mouseinfo1],al
@@mousefound:	mov	[mousetype],bl

;---------- check if CuteMouse driver already installed
		call	getCuteMouse
		mov	dx,offset DGROUP:S_reset ; 'Resident part reset to'
		mov	cx,4C02h		; terminate, al=return code
		je	@@setupdriver

;---------- allocate UMB memory, if possible, and set INT 33 handler
		mov	bx,(TSRend-TSRstart+15)/16
		push	bx
		call	prepareTSR		; new memory segment in ES
		memcopy	<SIZE oldint33>,es,,DGROUP:oldint33,ds,,DGROUP:oldint33
		push	ds
		MOVSEG	ds,es,,DGROUP
		mov	[disabled?],1		; copied back in setupdriver
		DOSSetIntr 33h,,,DGROUP:handler33
		pop	ds ax
		mov	dx,offset DGROUP:S_installed ; 'Installed at'
		mov	cl,0			; errorlevel
;----------
@@setupdriver:	push	ax			; size of TSR for INT 21/31
		DOSWriteS
		mov	al,[mousetype]

		mov	bx,offset DGROUP:S_CRLF
		shl	al,1
		jnc	@@printmode1		; jump if no wheel (=8xh)
		mov	bx,offset DGROUP:S_wheel

@@printmode1:	cbw				; OPTIMIZE: instead MOV AH,0
		cmp	al,1 shl 1
		xchg	si,ax			; OPTIMIZE: instead MOV SI,AX
		jb	@@printmode3		; jump if PS/2 mode (=0)
		ja	@@printmode2		; jump if not Mouse Systems (=1)
		inc	cx			; OPTIMIZE: CX instead CL

@@printmode2:	DOSWriteS ,,DGROUP:S_atCOM
@@printmode3:	push	cx			; exit function and errorlevel
		DOSWriteS ,,S_mousetype[si]
		DOSWriteS ,,bx
		call	setupdriver

;---------- close all handles (20 pieces) to prevent decreasing system
;	    pool of handles if INT 21/31 used
		mov	bx,19
@@closeall:	DOSCloseFile
		dec	bx
		jns	@@closeall
;----------
		pop	ax dx			; AH=31h (TSR) or 4Ch (EXIT)
		int	21h

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Setup resident driver code and parameters
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

setupdriver	proc
;---------- detect VGA card (VGA ATC have one more register)
		mov	ax,1A00h
		int	10h			; get display type in BX
		cmp	al,1Ah
		jne	@@lhand
		xchg	ax,bx			; OPTIMIZE: instead MOV AL,BL
		cmp	al,7			; monochrome VGA?
		je	@@setATCVGA
		cmp	al,8			; color VGA?
		jne	@@lhand
@@setATCVGA:	inc	videoregs@[(SIZE RGROUPDEF)*3].regscnt

;---------- setup left hand mode handling
@@lhand:	db	0B9h			; MOV CX,word
mousetype	db	?,0			; 0=PS/2,1=MSys,2=LT,3=MS,
						; 83h=MS+wheel
		mov	al,00000000b	; =0
		jcxz	@@checklhand		; jump if PS/2 mode (=0)
		mov	al,00000011b	; =3
@@checklhand:	testflag [options],OPT_LEFTHAND
		jz	@@setlhand
		flipflag al,00000011b	; =3
@@setlhand:	mov	[SWAPMASK],al

;---------- setup buttons count and mask
		mov	al,3
		testflag [options],OPT_3button
		jnz	@@sethandler
		jcxz	@@setbuttons		; jump if PS/2 mode (=0)
		cmp	cl,al			; OPTIMIZE: AL instead 3
		jne	@@sethandler		; jump if not MS mode (=3)
@@setbuttons:	mov	[buttonsmask],al	; OPTIMIZE: AL instead 0011b
		dec	ax
		mov	[buttonscnt],al		; OPTIMIZE: AL instead 2

;---------- setup mouse handlers code
@@sethandler:	jcxz	@@setother		; jump if PS/2 mode (=0)

		mov	word ptr [IRQproc],((OCW2<OCW2_EOI>) shl 8)+0B0h
						; MOV AL,OCW2<OCW2_EOI>
		mov	[BUFFADDR],offset DGROUP:bufferSERIAL
		mov	[enableproc],enableUART-enableproc-2
		mov	[disableproc],disableUART-disableproc-2
		dec	cx
		jz	@@setother		; jump if Mouse Systems mode (=1)
		mov	[mouseproc],MSLTproc-mouseproc-2
		dec	cx
		jz	@@setother		; jump if Logitech mode (=2)
		mov	MSLTCODE3[1],2
		loop	@@setother		; jump if wheel mode (=83h)

		cmp	al,2			; OPTIMIZE: AL instead [buttonscnt]
		je	@@setMSproc		; jump if MS2
		mov	[MSLTCODE2],075h	; JNZ
@@setMSproc:	mov	al,0C3h			; RET
		mov	[MSLTCODE1],al
		mov	[MSLTCODE3],al

;---------- setup, if required, other parameters
@@setother:	push	es ds es ds
		pop	es ds			; get back [oldint10]...
		memcopy	<SIZE oldint10>,es,,DGROUP:oldint10,ds,,DGROUP:oldint10
		mov	al,[disabled?]
		pop	ds
		mov	[disabled?],al		; ...and [disabled?]

		DOSGetIntr <byte ptr [IRQintnum]>
		mov	ax,es
		pop	es
		mov	di,offset DGROUP:oldIRQaddr
		xchg	ax,bx
		stosw				; save old IRQ handler
		xchg	ax,bx			; OPTIMIZE: instead MOV AX,BX
		stosw

;---------- copy TSR image (even if ES=DS - this is admissible)
		memcopy	((TSRend-TSRdata+1)/2)*2,es,,DGROUP:TSRdata,ds,,DGROUP:TSRdata

;---------- call INT 33/0000 (Reset driver)
		pop	ax
		pushf				;!!! Logitech MouseWare
		push	cs ax			;  Windows driver workaround
		mov	ax,offset DGROUP:handler33
		push	es ax
		xor	ax,ax			; reset driver
		retf
setupdriver	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Check given or all COM-ports for mouse connection
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

searchCOM	proc
		;mov	[LCRset],LCR<0,,LCR_noparity,0,2>
		mov	di,offset DGROUP:detectmouse
		call	COMloop
		jnc	@searchret

		testflag [options],OPT_NOMSYS
		stc
		jnz	@searchret

		mov	[LCRset],LCR<0,,LCR_noparity,0,3>
		mov	bl,1			; =Mouse Systems mode
		mov	di,offset DGROUP:checkUART
		;j	COMloop
searchCOM	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

COMloop		proc
		push	ax
		mov	ax,'11'			; scan only current COM port
		testflag [options],OPT_COMforced
		jnz	@@checkCOM
		mov	ah,'4'			; scan all COM ports

@@COMloop:	push	ax
		call	setCOMport
		pop	ax
@@checkCOM:	push	ax
		mov	si,[IO_address]
		call	di
		pop	ax
		jnc	@@searchbreak
		inc	ax			; OPTIMIZE: AX instead AL
		cmp	ah,al
		jae	@@COMloop
		;stc				; JB mean CF=1

@@searchbreak:	pop	ax
@searchret:	ret
COMloop		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Check if UART available
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	SI = [IO_address]
; Out:	Carry flag		(no UART detected)
; Use:	none
; Modf:	AX, DX
; Call:	none
;
checkUART	proc
		test	si,si
		 jz	@@noUART		; no UART if base=0

;---------- check UART registers for reserved bits
		movidx	dx,MCR_index,si		; {3FCh} MCR (modem ctrl reg)
		 in	ax,dx			; {3FDh} LSR (line status reg)
		testflag al,mask MCR_reserved+mask MCR_AFE
		 jnz	@@noUART
		movidx	dx,LSR_index,si,MCR_index
		 in	al,dx			; {3FDh} LSR (line status reg)
		inc	ax
		 jz	@@noUART		; no UART if AX was 0FFFFh

;---------- check LCR function
		cli
		movidx	dx,LCR_index,si,LSR_index
		 in	al,dx			; {3FBh} LCR (line ctrl reg)
		 push	ax
		out_	dx,%LCR<1,0,-1,-1,3>	; {3FBh} LCR: DLAB on, 8S2
		 inb	ah,dx
		out_	dx,%LCR<0,0,0,0,2>	; {3FBh} LCR: DLAB off, 7N1
		 in	al,dx
		sti
		sub	ax,(LCR<1,0,-1,-1,3> shl 8)+LCR<0,0,0,0,2>
		 jnz	@@restoreLCR		; nonzero if LCR not conforms

;---------- check IER for reserved bits
		movidx	dx,IER_index,si,LCR_index
		 in	al,dx			; {3F9h} IER (int enable reg)
		movidx	dx,LCR_index,si,IER_index
		;mov	ah,0
		and	al,mask IER_reserved	; reserved bits should be clear

@@restoreLCR:	neg	ax			; nonzero makes carry flag
		pop	ax
		 out	dx,al			; {3FBh} LCR: restore contents
		ret

@@noUART:	stc
		ret
checkUART	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Detect mouse type if present
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	SI = [IO_address]
; Out:	Carry flag		(no UART or mouse found)
;	BX			(mouse type: 2=Logitech,3=MS,83h=MS+wheel)
; Use:	0:46Ch, LCRset
; Modf:	AX, CX, DX, ES
; Call:	checkUART
;
detectmouse	proc
		call	checkUART
		jc	@@detmret

;---------- save current LCR/MCR
		movidx	dx,LCR_index,si		; {3FBh} LCR (line ctrl reg)
		 in	ax,dx			; {3FCh} MCR (modem ctrl reg)
		 push	ax			; keep old LCR and MCR values

;---------- reset UART: drop RTS line, interrupts and disable FIFO
		;movidx	dx,LCR_index,si		; {3FBh} LCR: DLAB off
		 out_	dx,%LCR<>,%MCR<>	; {3FCh} MCR: DTR/RTS/OUT2 off
		movidx	dx,IER_index,si,LCR_index
		 ;mov	ax,(FCR<> shl 8)+IER<>	; {3F9h} IER: interrupts off
		 out	dx,ax			; {3FAh} FCR: disable FIFO

;---------- set communication parameters and flush receive buffer
		movidx	dx,LCR_index,si,IER_index
		 out_	dx,%LCR{LCR_DLAB=1}	; {3FBh} LCR: DLAB on
		xchg	dx,si
		 ;mov	ah,0			; 1200 baud rate
		 out_	dx,96,ah		; {3F8h},{3F9h} divisor latch
		xchg	dx,si
		 out_	dx,[LCRset]		; {3FBh} LCR: DLAB off, 7/8N1
		movidx	dx,RBR_index,si,LCR_index
		 in	al,dx			; {3F8h} flush receive buffer

;---------- wait current+next timer tick and then raise RTS line
		MOVSEG	es,0,ax,BIOS
@@RTSloop:	mov	ah,byte ptr [BIOS_timer]
@@RTSwait:	cmp	ah,byte ptr [BIOS_timer]
		 je	@@RTSwait		; loop until next timer tick
		xor	al,1
		 jnz	@@RTSloop		; loop until end of 2nd tick

		movidx	dx,MCR_index,si,RBR_index
		 out_	dx,%MCR<,,,0,,1,1>	; {3FCh} MCR: DTR/RTS on, OUT2 off

;---------- detect if Microsoft or Logitech mouse present
		mov	bx,0103h		; bl=mouse type, bh=no `M'
		mov	cl,4			; scan 4 first bytes
@@detmloop:	mov	ch,2+1			; length of silence in ticks
						; (include rest of curr tick)
@@readloop:	mov	ah,byte ptr [BIOS_timer]
@@readwait:	movidx	dx,LSR_index,si
		 in	al,dx			; {3FDh} LSR (line status reg)
		testflag al,mask LSR_RBF
		 jnz	@@parse			; jump if data ready
		cmp	ah,byte ptr [BIOS_timer]
		 je	@@readwait		; loop until next timer tick
		dec	ch
		 jnz	@@readloop		; loop until end of 2nd tick
		j	@@detmdone		; exit if no more data

@@parse:	movidx	dx,RBR_index,si
		 in	al,dx			; {3F8h} receive byte
		cmp	al,'('-20h
		 je	@@detmdone		; exit if PNP data starts
		cmp	al,'M'
		 jne	@@checkWM
		 mov	bh,0			; MS compatible mouse found...
@@checkWM:	cmp	al,'Z'
		 jne	@@checkLT
		 mov	bl,83h			; ...MS mouse+wheel found
@@checkLT:	cmp	al,'3'
		 jne	@@detmnext
		 mov	bl,2			; ...Logitech mouse found
@@detmnext:	dec	cl
		 jnz	@@detmloop

@@detmdone:	movidx	dx,LCR_index,si
		 pop	ax			; {3FBh} LCR: restore contents
		 out	dx,ax			; {3FCh} MCR: restore contents

		shr	bh,1			; 1 makes carry flag
@@detmret:	ret
detectmouse	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;				Check for PS/2
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	Carry flag		(no PS/2 device found)
;	BL			(mouse type: 0=PS/2)
;	BH			(interrupt #/0=PS/2)
; Use:	none
; Modf:	AX, CX, BX
; Call:	INT 11, INT 15/C2xx
;
checkPS2	proc
		int	11h			; get equipment list
		testflag al,mask HW_PS2
		jz	@@noPS2			; jump if PS/2 not indicated
		mov	bh,3
		PS2serv 0C205h,@@noPS2		; initialize mouse, bh=datasize
		mov	bh,3
		PS2serv 0C203h,@@noPS2		; set mouse resolution bh
		MOVSEG	es,cs,,DGROUP
		mov	bx,offset DGROUP:PS2dummy
		PS2serv	0C207h,@@noPS2		; mouse, es:bx=ptr to handler
		MOVSEG	es,0,bx,nothing
		PS2serv	0C207h			; mouse, es:bx=ptr to handler
		;xor	bx,bx			; =PS/2 mouse
		;clc
		ret
@@noPS2:	stc
		ret
PS2dummy:	retf
checkPS2	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;				Set COM port
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	AL			(COM port letter)
; Out:	none
; Use:	0:400h
; Modf:	AL, CL, ES, com_port, IO_address, S_atIO
; Call:	setIRQ
;
setCOMport	proc
		push	ax di
		mov	[com_port],al

		cbw				; OPTIMIZE: instead MOV AH,0
		xchg	di,ax			; OPTIMIZE: instead MOV DI,AX
		MOVSEG	es,0,ax,BIOS
		shl	di,1
		mov	ax,COM_base[di-'1'-'1']
		mov	[IO_address],ax

		MOVSEG	es,ds,,DGROUP
		mov	di,offset DGROUP:S_atIO	; string for 4 digits
		mov	cl,12
@@loophex:	push	ax
		shr	ax,cl
		and	al,0Fh
		digit2x
		stosb
		pop	ax
		sub	cl,4
		jae	@@loophex

		pop	di ax
		and	al,1			; 1=COM1/3, 0=COM2/4
		add	al,'3'			; IRQ4 for COM1/3
		;j	setIRQ			; IRQ3 for COM2/4
setCOMport	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;				Set IRQ number
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	AL			(IRQ number letter)
; Out:	none
; Use:	none
; Modf:	AL, CL, IRQno, mouseinfo, IRQintnum, PIC1state, notPIC1state
; Call:	none
;
setIRQ		proc
		mov	[IRQno],al
		sub	al,'0'
		mov	mouseinfo[0],al
		mov	cl,al
		add	al,8			; INT=IRQ+8
		mov	byte ptr [IRQintnum],al
		mov	al,1
		shl	al,cl			; convert IRQ into bit mask
		mov	[PIC1state],al		; PIC interrupt disabler
		not	al
		mov	[notPIC1state],al	; PIC interrupt enabler
		ret
setIRQ		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Check if CuteMouse driver is installed
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	Zero flag		(ZF=1 if installed)
;	ES			(driver segment)
; Use:	oldint33, IDstring
; Modf:	AX, CX, SI, DI
; Call:	INT 33/004D
;
getCuteMouse	proc
		cmp	word ptr oldint33[2],1
		jb	@ret			; jump if handler seg is zero
		mov	ax,4Dh
		pushf				;!!! Logitech MouseWare
		call	[oldint33]		;  Windows driver workaround
		cmp	di,offset DGROUP:IDstring
		jne	@ret
		mov	si,di
		mov	cx,IDstringlen
		repe	cmpsb
@ret:		ret
getCuteMouse	endp


;€€€€€€€€€€€€€€€€€€€€€€€€€€ COMMAND LINE PARSING €€€€€€€€€€€€€€€€€€€€€€€€€€

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Parse Serial option
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

_serialopt	proc
		lodsb
		cmp	al,'1'
		jb	@@fixret
		cmp	al,'4'
		ja	@@fixret
		setflag	[options],OPT_COMforced
		call	setCOMport		; '/Sc' -> set COM port
		lodsb
		cmp	al,'2'
		jb	@@fixret
		cmp	al,'7'
		jbe	setIRQ			; '/Sci' -> set IRQ line
@@fixret:	dec	si			; fixup for command line ptr
		ret
_serialopt	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Parse Resolution option
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

_resolution	proc
		;mov	ah,0
		lodsb				; first argument
		sub	al,'0'
		cmp	al,9
		ja	@@fixupres		; jump if AL < 0 || AL > 9

		mov	ah,al
		lodsb				; second argument
		sub	al,'0'
		cmp	al,9
		jbe	@@setres		; jump if AL >= 0 && AL <= 9

@@fixupres:	dec	si			; fixup for command line ptr
		mov	al,ah			; drop invalid argument
@@setres:	mov	[mresolutionX],ah
		mov	[mresolutionY],al
		ret
_resolution	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Check if mouse services already present
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

_checkdriver	proc
		mov	cx,word ptr oldint33[2]
		jcxz	@ret
		;mov	ah,0
		mov	al,21h			; OPTIMIZE: AL instead AX
		int	33h
		inc	ax
		jnz	@ret
		mov	dx,offset DGROUP:E_mousepresent ; 'Mouse service already...'
		j	EXITMSG
_checkdriver	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
.const

OPTION		struc
  C		db	?
  M		db	0
  PROC@		dw	?
	ends

OPTABLE		OPTION	<'P',OPT_PS2,			@ret>
		OPTION	<'S',OPT_SERIAL,		_serialopt>
;;+		OPTION	<'A',OPT_SERIAL+OPT_COMforced,	_addropt>
		OPTION	<'Y',OPT_NOMSYS,		@ret>
		OPTION	<'V',OPT_PS2after,		@ret>
		OPTION	<'3' and not 20h,OPT_3button,	@ret>
		OPTION	<'R',,				_resolution>
		OPTION	<'L',OPT_LEFTHAND,		@ret>
		OPTION	<'B',,				_checkdriver>
		OPTION	<'W',OPT_NOUMB,			@ret>
		OPTION	<'U',,				unloadTSR>
		OPTION	<'?' and not 20h,,		EXITMSG>
OPTABLEend	label

.code

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; In:	DS:SI			(null terminated command line)
;
commandline	proc
		lodsb
		test	al,al
		jz	@ret			; exit if end of command line
		cmp	al,' '
		jbe	commandline		; skips spaces and controls
		cmp	al,'/'			; option character?
		jne	@@badoption
		lodsb
		and	al,not 20h		; uppercase

		mov	dx,offset DGROUP:Syntax	; 'Options:'
		mov	bx,offset DGROUP:OPTABLE
@@optloop:	cmp	al,[bx].C
		jne	@@nextopt
		mov	ah,[bx].M
		or	[options],ah
		call	[bx].PROC@
		j	commandline
@@nextopt:	add	bx,SIZE OPTION
		cmp	bx,offset DGROUP:OPTABLEend
		jb	@@optloop

@@badoption:	mov	dx,offset DGROUP:E_option ; 'Error: Invalid option'

EXITMSG:	mov	bx,dx
		inc	dx
		DOSWriteS
		mov	al,[bx]
		mov	ah,4Ch			; terminate, al=return code
		int	21h
commandline	endp


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€ TSR MANAGEMENT €€€€€€€€€€€€€€€€€€€€€€€€€€€€€

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Unload driver and quit
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

unloadTSR	proc
		call	getCuteMouse		; check if CTMOUSE installed
		mov	dx,offset DGROUP:E_nocute ; 'CuteMouse driver is not installed!'
		jne	EXITMSG

		push	es
		mov	ax,1Fh			; disable CuteMouse driver
		pushf				;!!! Logitech MouseWare
		call	[oldint33]		;  Windows driver workaround
		mov	cx,es
		pop	es
		cmp	al,1Fh
		mov	dx,offset DGROUP:E_notunload ; 'Driver unload failed...'
		jne	@@exitenable

		saveFAR	[oldint33],cx,bx
		push	ds
		DOSSetIntr 33h,cx,,bx		; restore old int33 handler
		pop	ds
		call	FreeMem
		mov	dx,offset DGROUP:S_unloaded ; 'Driver successfully unloaded...'
EXITENABLE:	mov	cx,word ptr oldint33[2]
		jcxz	EXITMSG			; exit if no old driver
@@exitenable:	mov	ax,20h			; enable old driver
		pushf				;!!! Logitech MouseWare
		call	[oldint33]		;  Windows driver workaround
		j	EXITMSG
unloadTSR	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Prepare memory for TSR
;
; In:	BX			(TSR size)
;	DS			(PSP segment)
; Out:	ES			(memory segment to be TSR)
;	CH			(exit code for INT 21)
; Use:	PSP:2Ch, MCB:8
; Modf:	AX, CL, DX, SI, DI
; Call:	INT 21/49, AllocUMB
;
prepareTSR	proc
		assume	ds:PSP
		mov	cx,[env_seg]
		jcxz	@@allocmem		; suggested by Matthias Paul
		DOSFreeMem cx			; release environment
		mov	[env_seg],0		; suggested by Matthias Paul
		assume	ds:DGROUP

@@allocmem:	call	AllocUMB
		mov	ax,ds
		mov	es,dx
		mov	ch,31h			; TSR exit, al=return code
		cmp	dx,ax
		je	@@prepareret		; exit if TSR "in place"

		push	ds
		dec	ax			; current MCB
		dec	dx			; target MCB...
						; ...copy process name
		memcopy	8,dx,MCB,MCB:ownername,ax,MCB,MCB:ownername
		POPSEG	ds,DGROUP

		inc	dx
		mov	[MCB:ownerID],dx	; ...and set owner to itself

		mov	ah,26h
		int	21h			; create PSP at segment in DX
		movadd	ax,[MCB_size],dx	; =target (PSP seg+MCB size)
		MOVSEG	es,dx,,PSP		; target PSP...
		mov	es:[PSP:next_seg],ax	; ...fix field

		mov	ch,4Ch			; terminate, al=return code
@@prepareret:	ret
prepareTSR	endp


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€ MEMORY HANDLING €€€€€€€€€€€€€€€€€€€€€€€€€€€€€

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Get XMS handler address
;
; In:	none
; Out:	Carry flag		(set if no XMS support)
; Use:	none
; Modf:	AX, BX, XMSentry
; Call:	INT 2F/4300, INT 2F/4310
;
getXMSaddr	proc
		mov	ax,4300h
		int	2fh			; XMS: installation check
		cmp	al,80h
		stc
		jne	@@getXMSret		; return if XMS not present
		push	es
		mov	ax,4310h		; XMS: Get Driver Address
		int	2Fh
		saveFAR [XMSentry],es,bx
		pop	es
		clc
@@getXMSret:	ret
getXMSaddr	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Save allocation strategy
;
; In:	none
; Out:	Carry flag		(no UMB link supported)
; Use:	none
; Modf:	AX, SaveMemStrat, SaveUMBLink
; Call:	INT 21/5800, INT 21/5802
;
SaveStrategy	proc
		mov	ax,5800h
		int	21h			; get DOS memalloc strategy
		mov	[SaveMemStrat],ax
		mov	ax,5802h
		int	21h			; get UMB link state
		mov	[SaveUMBLink],al
		ret
SaveStrategy	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Restore allocation strategy
;
; In:	none
; Out:	none
; Use:	SaveMemStrat, SaveUMBLink
; Modf:	AX, BX
; Call:	INT 21/5801, INT 21/5803
;
RestoreStrategy	proc
		db	0BBh			; MOV BX,word
SaveMemStrat	dw	?
		mov	ax,5801h
		int	21h			; set DOS memalloc strategy
		db	0BBh			; MOV BX,word
SaveUMBLink	db	?,0
		mov	ax,5803h
		int	21h			; set UMB link state
		ret
RestoreStrategy	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Allocate high memory
;
; In:	BX			(required memory size in para)
;	DS			(current memory segment)
; Out:	DX			(seg of new memory or DS)
; Use:	XMSentry
; Modf:	AX, ES
; Call:	INT 21/48, INT 21/49, INT 21/58,
;	SaveStrategy, RestoreStrategy, getXMSaddr
;
AllocUMB	proc
		push	bx
		testflag [options],OPT_NOUMB
		jnz	@@allocasis		; jump if UMB prohibited
		mov	ax,ds
		cmp	ah,0A0h
		jae	@@allocasis		; jump if already loaded hi

;---------- check if UMB is DOS type
		call	SaveStrategy
		mov	bx,1			; add UMB to MCB chain
		mov	ax,5803h
		int	21h			; set UMB link state

; try to set a best strategy to allocate DOS supported UMBs
		mov	bl,41h			; OPTIMIZE: BL instead BX
		mov	ax,5801h		; hi mem, best fit
		int	21h			; set alloc strategy
		jnc	@@allocDOSUMB

; try a worse one then
		mov	bl,81h			; OPTIMIZE: BL instead BX
		mov	ax,5801h		; hi mem then low mem, best fit
		int	21h			; set alloc strategy

@@allocDOSUMB:	pop	bx
		push	bx
		mov	ah,48h
		int	21h			; allocate UMB memory (size=BX)
		pushf
		xchg	dx,ax			; OPTIMIZE: instead MOV DX,AX
		call	RestoreStrategy		; restore allocation strategy
		popf
		jc	@@allocXMSUMB
		cmp	dh,0A0h			; exit if allocated mem is
		jae	@@allocret		;  is above 640k,
		DOSFreeMem dx			;  else free it

;---------- try XMS driver to allocate UMB
@@allocXMSUMB:	call	getXMSaddr
		jc	@@allocasis
		pop	dx
		push	dx
		mov	ah,10h			; XMS: Request UMB (size=DX)
		call	[XMSentry]		; ...AX=1 -> BX=seg, DX=size
		dec	ax
		jnz	@@allocasis
		pop	ax
		push	ax
		cmp	bx,ax
		mov	dx,bx
		jae	@@allocret
		mov	ah,11h			; XMS: Release UMB (seg=DX)
		call	[XMSentry]

;---------- use current memory segment
@@allocasis:	mov	dx,ds

@@allocret:	pop	bx
		ret
AllocUMB	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; In:	ES			(segment to free)
; Out:	none
; Use:	XMSentry
; Modf:	AH, DX
; Call:	INT 21/49, getXMSaddr
;
FreeMem		proc
		assume	es:nothing
		call	getXMSaddr
		jc	@@DOSfree

		mov	dx,es
		mov	ah,11h			; XMS: Release UMB
		db	09Ah			; CALL FAR dword
XMSentry	dd	?

@@DOSfree:	DOSFreeMem			; free allocated memory
		ret
FreeMem		endp

;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

		end	start
