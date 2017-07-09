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

%pagesize 255
%noincl
;%macs
%nosyms
;%depth 0
%linum 0
;%pcnt 0
;%bin 0
warn
locals

CTMVER		equ <"1.9">		; major driver version
CTMRELEASE	equ <"1.9">		; full driver version with suffixes
driverversion	equ 705h		; imitated Microsoft driver version

FASTER_CODE	 = 0		; optimize by speed instead size
OVERFLOW_PROTECT = 0		; prevent variables overflow
FOOLPROOF	 = 1		; check driver arguments validness

;.286
;.386

;------------------------------------------------------------------------

include asm.mac
include hll.mac
include code.def
include code.mac
include macro.mac
include BIOS/area0.def
include convert/digit.mac
include convert/count2x.mac
include DOS/MCB.def
include DOS/PSP.def
include DOS/file.mac
include DOS/mem.mac
include hard/PIC8259A.def
include hard/UART.def

USE_286		equ <(@CPU and 4)>
USE_386		equ <(@CPU and 8)>

_ARG_DI_	equ <word ptr [bp]>
_ARG_SI_	equ <word ptr [bp+2]>
_ARG_BP_	equ <word ptr [bp+4]>

if USE_286

_ARG_BX_	equ <word ptr [bp+8]>
_ARG_DX_	equ <word ptr [bp+10]>
_ARG_CX_	equ <word ptr [bp+12]>
_ARG_AX_	equ <word ptr [bp+14]>
_ARG_ES_	equ <word ptr [bp+16]>
_ARG_DS_	equ <word ptr [bp+18]>
_ARG_OFFS_	=   24

PUSHALL		equ <pusha>
POPALL		equ <popa>

else ; USE_286

_ARG_BX_	equ <word ptr [bp+6]>
_ARG_DX_	equ <word ptr [bp+8]>
_ARG_CX_	equ <word ptr [bp+10]>
_ARG_AX_	equ <word ptr [bp+12]>
_ARG_ES_	equ <word ptr [bp+14]>
_ARG_DS_	equ <word ptr [bp+16]>
_ARG_OFFS_	=   22

PUSHALL		equ <push	ax cx dx bx bp si di>
POPALL		equ <pop	di si bp bx dx cx ax>

endif ; USE_286

nl		equ <13,10>
eos		equ <0>

POINT		struc
  X		dw 0
  Y		dw 0
ends

PS2serv		macro	serv:req,errlabel:vararg
		mov	ax,serv
		int	15h
	ifnb <errlabel>
		jc	errlabel
		test	ah,ah
		jnz	errlabel
	endif
endm


;€€€€€€€€€€€€€€€€€€€€€€€€€€ SEGMENTS DEFINITION €€€€€€€€€€€€€€€€€€€€€€€€€€

.model use16 tiny
assume ss:nothing

@TSRcode equ <DGROUP>
@TSRdata equ <DGROUP>

TSRcref	equ <offset @TSRcode>	; offset relative TSR code group
TSRdref	equ <offset @TSRdata>	;	- " -	      data
coderef	equ <offset @code>	; offset relative main code group
dataref	equ <offset @data>	;	- " -	       data

.code
		org	0
TSRstart	label
		org	100h		; .COM style program
start:		jmp	real_start


;€€€€€€€€€€€€€€€€€€€€€€€€€€ UNINITIALIZED DATA €€€€€€€€€€€€€€€€€€€€€€€€€€

;!!! WARNING: don't init variables in uninitialized section because
;		they will not be present in the executable image

		org	PSP_TSR-3*16	; reuse part of PSP
spritebuf	db	3*16 dup(?)	; copy of screen sprite in modes 4-6

;----- application state -----

;!!! WARNING: variables order between RedefArea and szDefArea must be
;		syncronized with variables order after DefArea

		evendata
SaveArea = $
RedefArea = $

mickey8		POINT	?		; mickeys per 8 pixel ratios
;;*doublespeed	dw	?		; double-speed threshold (mickeys/sec)
startscan	dw	?		; screen mask/cursor start scanline
endscan		dw	?		; cursor mask/cursor end scanline

;----- hotspot, screenmask and cursormask must follow as is -----

hotspot		POINT	?		; cursor bitmap hot spot
screenmask	db	2*16 dup (?)	; user defined screen mask
cursormask	db	2*16 dup (?)	; user defined cursor mask
nocursorcnt	db	?		; 0=cursor enabled, else hide counter
;;*nolightpen?	db	?		; 0=emulate light pen
		evendata
szDefArea = $ - RedefArea		; initialized by softreset_21

rangemax	POINT	?		; horizontal/vertical range max
upleft		POINT	?		; upper left of update region
lowright	POINT	?		; lower right of update region
pos		POINT	?		; virtual cursor position
granpos		POINT	?		; granulated virtual cursor position
UIR@		dd	?		; user interrupt routine address

		evendata
ClearArea = $

rounderr	POINT	?		; rounding error after mickeys->pixels
		evendata
szClearArea1 = $ - ClearArea		; cleared by setpos_04

rangemin	POINT	?		; horizontal/vertical range min
		evendata
szClearArea2 = $ - ClearArea		; cleared by setupvideo

cursortype	db	?		; 0 - software, else hardware
callmask	db	?		; user interrupt routine call mask
mickeys		POINT	?		; mouse move since last access
BUTTLASTSTATE	struc
  counter	dw	?
  lastrow	dw	?
  lastcol	dw	?
ends
buttpress	BUTTLASTSTATE ?,?,?
buttrelease	BUTTLASTSTATE ?,?,?
		evendata
szClearArea3 = $ - ClearArea		; cleared by softreset_21
szSaveArea = $ - SaveArea

;----- registers values for RIL -----

;!!! WARNING: registers order and RGROUPDEF contents must be fixed

		evendata
VRegsArea = $

regs_SEQC	db	5 dup (?)
reg_MISC	db	?
regs_CRTC	db	25 dup (?)
regs_ATC	db	21 dup (?)
regs_GRC	db	9 dup (?)
reg_FC		db	?
reg_GPOS1	db	?
reg_GPOS2	db	?

DefVRegsArea = $

def_SEQC	db	5 dup (?)
def_MISC	db	?
def_CRTC	db	25 dup (?)
def_ATC		db	21 dup (?)
def_GRC		db	9 dup (?)
def_FC		db	?
def_GPOS1	db	?
def_GPOS2	db	?

szVRegsArea = $ - DefVRegsArea

ERRIF (szVRegsArea ne 64 or $-VRegsArea ne 2*64) "VRegs area contents corrupted!"

;----- old interrupt vectors -----

oldint33	dd	?		; old INT 33 handler address
oldIRQaddr	dd	?		; old IRQ handler address


;€€€€€€€€€€€€€€€€€€€€€€€€€€€ INITIALIZED DATA €€€€€€€€€€€€€€€€€€€€€€€€€€€

		evendata
TSRdata		label

DefArea = $
		POINT	<8,16>			; mickey8
;;*		dw	64			; doublespeed
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
		db	1			; nocursorcnt
;;*		db	0			; nolightpen?
		evendata
ERRIF ($-DefArea ne szDefArea) "Defaults area contents corrupted!"

;----- driver and video state begins here -----

		evendata
granumask	POINT	<-1,-1>

textbuf		label	word
buffer@		dd	?		; pointer to screen sprite copy
cursor@		dw	-1		; cursor sprite offset in videoseg;
					; -1=cursor not drawn
videoseg	dw	0		; 0=not supported video mode

UIRunlock	db	1		; 0=user intr routine is in progress
videolock	db	1		; drawing: 1=ready,0=busy,-1=busy+queue
newcursor	db	0		; 1=force cursor redraw

;----- table of pointers to registers values for RIL -----

REGSET		struc
  rgroup	dw	?
  regnum	db	?
  regval	db	?
ends
		evendata
		dw	(vdata1end-vdata1)/(size REGSET)
vdata1		REGSET	<10h,1>,<10h,3>,<10h,4>,<10h,5>,<10h,8>,<08h,2>
vdata1end	label
		dw	(vdata2end-vdata2)/(size REGSET)
vdata2		REGSET	<10h,1,0>,<10h,4,0>,<10h,5,1>,<10h,8,0FFh>,<08h,2,0Fh>
vdata2end	label

RGROUPDEF	struc
  port@		dw	?
  regs@		dw	?
  def@		dw	?
  regscnt	db	1
  rmodify?	db	0
ends
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


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€ IRQ HANDLERS €€€€€€€€€€€€€€€€€€€€€€€€€€€€€

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

IRQhandler	proc
		assume	ds:nothing,es:nothing
		cld
		push	ds es
		PUSHALL
		MOVSEG	ds,cs,,@TSRdata
IRQproc		label	byte			; "mov al,OCW2<OCW2_EOI>"
		j	PS2proc			;  if serial mode
		out	PIC1_OCW2,al		; {20h} end of interrupt

	CODE_	MOV_DX	IO_address,<dw ?>	; UART IO address
		push	dx
		movidx	dx,LSR_index
		 in	al,dx			; {3FDh} LSR: get status
		xchg	bx,ax			; OPTIMIZE: instead MOV BL,AL
		pop	dx
		movidx	dx,RBR_index
		 in	al,dx			; {3F8h} flush receive buffer
	CODE_	MOV_CX	IOdone,<db ?,0>		; processed bytes counter

		test	bl,mask LSR_break+mask LSR_FE+mask LSR_OE
	if_ nz					; if break/framing/overrun
		xor	cx,cx			;  errors then restart
		mov	[IOdone],cl		;  sequence: clear counter
	end_
		shr	bl,LSR_RBF+1
	if_ carry				; process data if data ready
		call_	mouseproc,MSMproc
	end_
		jmp	@rethandler
IRQhandler	endp
		assume	ds:@TSRdata

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;				Enable PS/2
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	none
; Modf:	BX, ES
; Call:	disablePS2, INT 15/C2xx
;
enablePS2	proc
		call	disablePS2
		MOVSEG	es,cs,,@TSRcode
		mov	bx,TSRcref:IRQhandler
		PS2serv	0C207h			; es:bx=ptr to handler
		mov	bh,1			; set mouse on
		PS2serv	0C200h
		ret
enablePS2	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;				Disable PS/2
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

PS2proc		proc
		mov	bp,sp
		mov	al,[bp+_ARG_OFFS_+6]	; buttons
if USE_286
		mov	bl,al
		shl	al,3			; CF=Y sign bit
else
		mov	cl,3
		mov	bl,al
		shl	al,cl			; CF=Y sign bit
endif
		sbb	ch,ch			; signed extension 9->16 bit
		cbw				; extend X sign bit
		mov	al,[bp+_ARG_OFFS_+4]	; AX=X movement
		mov	cl,[bp+_ARG_OFFS_+2]	; CX=Y movement
		xchg	bx,ax
		call	reverseY
		POPALL
		pop	es ds
		retf
PS2proc		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Enable serial interrupt in PIC
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	IO_address
; Modf:	AX, DX, SI, IOdone, MSLTbuttons
; Call:	INT 21/25
;
enableUART	proc

;---------- set new IRQ handler

		mov	dx,TSRcref:IRQhandler
	CODE_	MOV_AX	IRQintnum,<db 0,25h>	; INT number of selected IRQ
		int	21h			; set INT in DS:DX

;---------- set communication parameters

		mov	si,[IO_address]
		movidx	dx,LCR_index,si
		 out_	dx,%LCR{LCR_DLAB=1}	; {3FBh} LCR: DLAB on
		xchg	dx,si			; 1200 baud rate
		 outw	dx,96			; {3F8h},{3F9h} divisor latch
		xchg	dx,si
	CODE_	 MOV_AX
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
	CODE_	AND_AL	notPIC1state,<db ?>	; clear bit to enable interrupt
		out	PIC1_IMR,al		; {21h} enable serial interrupts
		ret
enableUART	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Disable serial interrupt of PIC
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	IO_address, oldIRQaddr
; Modf:	AX, DX
; Call:	INT 21/25
;
disableUART	proc
		in	al,PIC1_IMR		; {21h} get IMR
	CODE_	OR_AL	PIC1state,<db ?>	; set bit to disable interrupt
		out	PIC1_IMR,al		; {21h} disable serial interrupts

;----------
;!!! MS/Logitech ID bytes (4Dh)+PnP data looks as valid mouse packet, which
;	moves mouse; to prevent unnecessary send of PnP data after disabling
;	driver with following enabling, below DTR and RTS remained active

		movidx	dx,LCR_index,[IO_address] ; {3FBh} LCR: DLAB off
		 out_	dx,%LCR<>,%MCR<,,,0,,1,1> ; {3FCh} MCR: DTR/RTS on, OUT2 off
		movidx	dx,IER_index,,LCR_index
		 ;mov	al,IER<>
		 out	dx,al			; {3F9h} IER: interrupts off

;---------- restore old IRQ handler

		push	ds
		mov	ax,word ptr [IRQintnum]	; AH=25h
		lds	dx,[oldIRQaddr]
		assume	ds:nothing
		int	21h			; set INT in DS:DX
		pop	ds
		ret
disableUART	endp
		assume	ds:@TSRdata

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Process the Microsoft/Logitech packet bytes
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

MSLTproc	proc
	CODE_	MOV_DL	MSLTbuttons,<db ?>	; buttons state for MS3/LT/WM
		test	al,01000000b	; =40h	; synchro check
	if_ nz					; if first byte
		mov	[IOdone],1		; request next 2/3 bytes
		mov	[MSLT_1],al
MSLTCODE1	label	byte			; "ret" if not LT/WM
		xchg	ax,cx			; OPTIMIZE: instead MOV AL,CL
		sub	al,3			; if first byte after 3 bytes
		jz	@@LTWMbutton3		;  then release middle button
		ret
	end_

	if_ ncxz				; skip nonfirst byte at start
		inc	[IOdone]		; request next byte
		loop	@@MSLT_3
		mov	[MSLT_X],al		; keep X movement LO
	end_
@@LTret:	ret

@@MSLT_3:	loop	@@LTWM_4
		;mov	cl,0
	CODE_	MOV_BX	MSLT_1,<db ?,0>		; mouse packet first byte
		ror	bx,2
		xchg	cl,bh			; bits 1-0: X movement HI
		ror	bx,2			; bits 5-4: LR buttons
		or	al,bh			; bits 3-2: Y movement HI
		cbw
		 xchg	cx,ax			; CX=Y movement
	CODE_	OR_AL	MSLT_X,<db ?>
		cbw
		 xchg	bx,ax			; BX=X movement

		xor	al,dl
		 and	al,00000011b	; =3	; LR buttons change mask
		mov	dh,al
		 or	dh,bl			; nonzero if LR buttons state
		 or	dh,cl			;  changed or mouse moved
MSLTCODE2	label	byte
		j	@@MSLTupdate		; "jnz" if MS3
		or	al,00000100b	; =4	; empty event toggles button
		j	@@MSLTupdate

@@LTWM_4:	;mov	ch,0
		mov	[IOdone],ch		; request next packet
MSLTCODE3	label	byte			; if LT "mov cl,3" else
		mov	cl,3			; if WM "mov cl,2" else "ret"
		shr	al,cl
@@LTWMbutton3:	xor	al,dl
		and	al,00000100b	; =4	; M button change mask
if FASTER_CODE
		jz	@@LTret			; exit if button 3 not changed
endif
		xor	bx,bx
		xor	cx,cx

@@MSLTupdate:	xor	al,dl			; new buttons state
		mov	[MSLTbuttons],al
		j	swapbuttons
MSLTproc	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Process the Mouse Systems packet bytes
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

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

@@MSM_1:	xor	al,10000111b	; =87h	; sync check: AL should
		test	al,11111000b	; =0F8h	;  be equal to 10000lmr
	if_ zero
		test	al,00000110b	; =6	; check the L and M buttons
	 if_ odd				; if buttons not same
		xor	al,00000110b	; =6	; swap them
	 end_
		mov	[MSM_buttons],al	; bits 2-0: MLR buttons
		;j	@@MSMnext

@@MSM_3:	mov	[MSM_Y],ax
@@MSMnext:	inc	[IOdone]		; request next byte
	end_
		ret

@@MSM_5:	;mov	ch,0
		mov	[IOdone],ch		; request next packet
	CODE_	ADD_AX	MSM_Y,<dw ?>
	CODE_	MOV_BX	MSM_X,<dw ?>
		xchg	cx,ax			; OPTIMIZE: instead MOV CX,AX
	CODE_	MOV_AL	MSM_buttons,<db ?>
		;j	reverseY
MSMproc		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Update mouse status
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	AL			(new buttons state)
;	BX			(X mouse movement)
;	CX			(Y mouse movement)
; Out:	none
; Use:	callmask, granpos, mickeys, UIR@
; Modf:	AX, CX, DX, BX, SI, DI, UIRunlock
; Call:	updateposition, updatebutton, refreshcursor
;
reverseY	proc
		neg	cx			; reverse Y movement
		;j	swapbuttons
reverseY	endp

swapbuttons	proc
		test	al,00000011b	; =3	; check the L and R buttons
	if_ odd					; if buttons not same
	CODE_	XOR_AL	swapmask,<db 00000011b>	; 0 if (PS2 xor LEFTHAND)
	end_
		;j	mouseupdate
swapbuttons	endp

mouseupdate	proc
	CODE_	AND_AL	buttonsmask,<db 00000111b>
		xchg	di,ax			; OPTIMIZE: instead MOV DI,AX

;---------- update mickey counters and screen position

		xchg	ax,bx			; OPTIMIZE: instead MOV AX,BX
		MOVREG_	bx,<offset X>
	CODE_	MOV_DL	mresolutionX,<db 0>
		call	updateposition

		xchg	ax,cx
		MOVREG_	bl,<offset Y>		; OPTIMIZE: BL instead BX
	CODE_	MOV_DL	mresolutionY,<db 0>
		call	updateposition
		or	cl,al			; bit 0=mickeys change flag

;---------- update buttons state

		mov	ax,[mickeys.Y]
		xchg	ax,di
		mov	ah,al			; AH=buttons new state
		xchg	al,[buttstatus]
		xor	al,ah			; AL=buttons change state
if FASTER_CODE
	if_ nz
endif
		xchg	dx,ax			; OPTIMIZE: instead MOV DX,AX
		xor	bx,bx			; buttpress array index
		mov	al,00000010b		; mask for button 1
		call	updatebutton
		mov	al,00001000b		; mask for button 2
		call	updatebutton
		mov	al,00100000b		; mask for button 3
		call	updatebutton
if FASTER_CODE
	end_
endif

;---------- call User Interrupt Routine (CX=events mask)

		dec	[UIRunlock]
	if_ zero				; if user proc not running
		and	cl,[callmask]
	 if_ nz					; if there is a user events
	CODE_	MOV_BX	buttstatus,<db 0,0>	; buttons status
		mov	ax,[granpos.X]
		mov	dx,[granpos.Y]
		xchg	ax,cx
		mov	si,[mickeys.X]
		;mov	di,[mickeys.Y]
		push	ds
		sti
		call	[UIR@]
		pop	ds
	 end_
		call	refreshcursor
	end_

;----------

		inc	[UIRunlock]
		ret
mouseupdate	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
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
	if_ sign
		neg	si
	end_

;---------- apply resolution (AX=movement, SI=abs(AX), DL=reslevel)

		cmp	si,2			; movement not changed if...
		jbe	@@newmickeys		; ...[-2..+2] movement
		mov	dh,0
		dec	dx
		jz	@@newmickeys		; ...or resolution=1
		cmp	si,7
	if_ be					; [-7..-2,+2..+7] movement...
@@resmul2:	shl	ax,1			; ...multiplied by 2
		j	@@newmickeys
	end_

		inc	dx
	if_ zero
		mov	dl,10			; auto resolution=
		shr	si,2			;  min(10,abs(AX)/4)
		cmp	si,dx
	andif_ below
		mov	dx,si
	end_
if FASTER_CODE
		cmp	dl,2
		je	@@resmul2
endif
		imul	dx			; multiply on resolution

;---------- apply mickeys per 8 pixels ratio to calculate cursor position

@@newmickeys:	add	word ptr mickeys[bx],ax
if FASTER_CODE
		mov	si,word ptr mickey8[bx]
		cmp	si,8
	if_ ne
		shl	ax,3
		dec	si
	andif_ gt
		add	ax,word ptr rounderr[bx]
		inc	si
		cwd
		idiv	si
		mov	word ptr rounderr[bx],dx
		test	ax,ax
		jz	@@uposdone
	end_
else
		shl	ax,3
		add	ax,word ptr rounderr[bx]
		cwd
		idiv	word ptr mickey8[bx]
		mov	word ptr rounderr[bx],dx
endif
		add	ax,word ptr pos[bx]

;---------- cut new position by virtual ranges and save

@savecutpos:	mov	dx,word ptr rangemax[bx]
		cmp	ax,dx
		jge	@@cutpos
		mov	dx,word ptr rangemin[bx]
		cmp	ax,dx
	if_ le
@@cutpos:	xchg	ax,dx			; OPTIMIZE: instead MOV AX,DX
	end_
		mov	word ptr pos[bx],ax	; new position
		and	al,byte ptr granumask[bx]
		mov	word ptr granpos[bx],ax	; new granulated position
@@uposdone:	mov	ax,1
@@uposret:	ret
updateposition	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
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
	if_ carry				; if button changed
		mov	si,TSRdref:buttpress
		test	dl,dl
	 if_ ns					; if button not pressed
		shl	al,1			; indicate that it released
		mov	si,TSRdref:buttrelease
	 end_
		or	cl,al
		inc	[si+bx].counter
		mov	ax,[granpos.Y]
		mov	[si+bx].lastrow,ax
		mov	ax,[granpos.X]
		mov	[si+bx].lastcol,ax
	end_
		add	bx,size BUTTLASTSTATE	; next button
		ret
updatebutton	endp

;€€€€€€€€€€€€€€€€€€€€€€€€€ END OF IRQ HANDLERS €€€€€€€€€€€€€€€€€€€€€€€€€€


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€ INT 10 HANDLER €€€€€€€€€€€€€€€€€€€€€€€€€€€€

		evendata
RILtable	dw TSRcref:RIL_F0	; RIL functions
		dw TSRcref:RIL_F1
		dw TSRcref:RIL_F2
		dw TSRcref:RIL_F3
		dw TSRcref:RIL_F4
		dw TSRcref:RIL_F5
		dw TSRcref:RIL_F6
		dw TSRcref:RIL_F7

int10handler	proc
		assume	ds:nothing,es:nothing
		cld
		test	ah,ah			; set video mode?
		jz	@@setmode
		cmp	ah,11h			; font manipulation function
		je	@@setnewfont
		cmp	ax,4F02h		; VESA set video mode?
		je	@@setmode
		cmp	ah,0F0h			; RIL func requested?
		jb	@@jmpold10
		cmp	ah,0F7h
		jbe	@@RIL
		cmp	ah,0FAh
		je	@@RIL_FA
@@jmpold10:	jmp_far	oldint10

@@setnewfont:	cmp	al,10h
		jb	@@jmpold10
		cmp	al,20h
		jae	@@jmpold10
		;j	@@setmode

;========== set video mode or activate font

@@setmode:	push	ax
		mov	ax,2
		pushf				;!!! Logitech MouseWare
		push	cs			;  Windows driver workaround
		call	handler33		; hide mouse cursor
		pop	ax
		pushf
		call	[oldint10]
		push	ds es
		PUSHALL
		MOVSEG	ds,cs,,@TSRdata
		mov	[nocursorcnt],1		; normalize hide counter
		call	setupvideo
@@exitINT10:	jmp	@rethandler

;========== RIL

@@RIL:		push	ds es
		PUSHALL
		MOVSEG	ds,cs,,@TSRdata
		mov	bp,sp
		mov	al,ah
		and	ax,0Fh			;!!! AH must be 0 for RIL_*
		mov	si,ax
		shl	si,1
		call	RILtable[si]
		j	@@exitINT10

;----------

@@RIL_FA:	MOVSEG	es,cs,,@TSRcode		; RIL FA - Interrogate driver
		mov	bx,TSRcref:RILversion
		iret
int10handler	endp
		assume	ds:@TSRdata

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F0 - Read one register
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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
	if_ below				; if not single register
		add	si,bx
	end_
		lodsb
		mov	byte ptr [_ARG_BX_],al
		ret
RIL_F0		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F1 - Write one register
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
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
	if_ below				; if not single register
		mov	bl,al
	end_
		mov	[di+bx],ah
		jae	RILoutAH
		;j	RILgroupwrite
RILwrite	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
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
	if_ ne					; if not ATTR controller
		out	dx,ax
		ret
	end_
		push	ax dx
		mov	dx,videoregs@[(size RGROUPDEF)*5].port@
		in	al,dx			; {3DAh} force address mode
		pop	dx ax
		out	dx,al			; {3C0h} select ATC register
RILoutAH:	xchg	al,ah
		out	dx,al			; {3C0h} modify ATC register
		xchg	al,ah
		ret
RILgroupwrite	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F2 - Read register range
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F3 - Write register range
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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
	countloop_
		mov	ah,es:[bx]
		mov	[di],ah
		inc	bx
		inc	di
		call	RILgroupwrite
		inc	ax			; OPTIMIZE: AX instead AL
	end_
		ret
RIL_F3		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F4 - Read register set
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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
	countloop_
		mov	bx,es:[di]
		movadd	di,,2
		mov	bx,videoregs@[bx].regs@
		mov	al,es:[di]
		inc	di
		xlat
		stosb
	end_
		ret
RIL_F4		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F5 - Write register set
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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
	countloop_
		lods	word ptr es:[si]
		xchg	dx,ax			; OPTIMIZE: instead MOV DX,AX
		lods	word ptr es:[si]
		call	RILwrite
	end_
		ret
RIL_F5		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F7 - Define registers default
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; RIL F6 - Revert registers to default
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	videoregs@
; Modf:	AX, CX, DX, BX, SI, DI, ES
; Call:	RILgrouploop
;
RIL_F6		proc
		MOVSEG	es,ds,,@TSRdata
		mov	si,TSRdref:videoregs@+(size RGROUPDEF)*8

@@R6loop:	sub	si,size RGROUPDEF
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

@@R6next:	cmp	si,TSRdref:videoregs@
		ja	@@R6loop
		ret
RIL_F6		endp

;€€€€€€€€€€€€€€€€€€€€€€€€ END OF INT 10 HANDLER €€€€€€€€€€€€€€€€€€€€€€€€€


;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Draw mouse cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

drawcursor	proc
		mov	cx,[videoseg]
		jcxz	@@drawret		; exit if nonstandard mode

		xor	cx,cx
		cmp	[nocursorcnt],cl	; OPTIMIZE: CL instead 0
		jnz	restorescreen		; jump if cursor disabled

		mov	ax,[granumask.Y]
		xchg	cl,[newcursor]		; remove redraw request
						; CX=force cursor request
		inc	ax
		mov	bx,[granpos.Y]		; cursor position Y
		mov	ax,[granpos.X]		; cursor position X
		jz	graphcursor		; jump if graphics mode

;========== text mode cursor

		mov	si,8			; OPTIMIZE: instead -[granumask.Y]
		call	checkifseen
		jc	restorescreen		; jump if not in seen area

		call	gettxtoffset
		cmp	di,[cursor@]
	if_ eq					; exit if position not changed
		jcxz	@@drawret		;  and cursor not forced
	end_
		push	di
		call	restorescreen
		;MOVSEG	es,[videoseg],,nothing
		pop	di

		cmp	[cursortype],ch		; OPTIMIZE: CH instead 0
	if_ nz

;---------- position hardware text mode cursor

		shr	di,1
		mov	dx,videoregs@[0].port@	; CRTC port
		mov	ax,di
		out_	dx,0Fh,al		; cursor position lo
		xchg	ax,di			; OPTIMIZE: instead MOV AX,DI
		out_	dx,0Eh,ah		; cursor position hi
		ret
	end_

;---------- draw software text mode cursor

		mov	[cursor@],di
		mov	ax,es:[di]		; save char under cursor
		mov	textbuf[2],ax
		and	ax,[startscan]
		xor	ax,[endscan]
		stosw				; draw to new position
		mov	textbuf[0],ax
@@drawret:	ret
drawcursor	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Restore old screen contents
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

restorescreen	proc
		les	di,dword ptr [cursor@]
		assume	es:nothing
		inc	di
	if_ nz					; if cursor drawn
		sub	[cursor@],di		; OPTIMIZE: instead MOV -1
		mov	ax,[granumask.Y]
		dec	di
		inc	ax

	 if_ zero

;---------- graphics mode

		call	restoresprite
		jmp	restorevregs
	 end_

;---------- text mode

		mov	si,TSRdref:textbuf
		lodsw
		cmp	ax,es:[di]
	 if_ eq					; if screen not changed
		movsw				; restore old text char/attrib
	 end_
	end_
@drawret:	ret
restorescreen	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Draw graphics mode mouse cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

graphcursor	proc
		sub	ax,[hotspot.X]		; virtual X
		sub	bx,[hotspot.Y]		; virtual Y
		mov	si,16			; cursor height
		push	ax
		call	checkifseen
		pop	ax
		jc	restorescreen		; jump if not in seen area

		xchg	ax,bx
		xor	dx,dx
		neg	ax
	if_ lt
		neg	ax
		xchg	ax,dx
	end_
		mov	[spritetop],ax
		mov	ax,[screenheight]
		cmp	si,ax
	if_ ge
		xchg	si,ax			; OPTIMIZE: instead MOV SI,AX
	end_
		sub	si,dx			; =spriteheight
		push	si			;  =min(16-ax,screenheight-dx)
		call	getgroffset
		pop	dx

; cx=force request, bx=X, di=line offset, si=nextrow, dx=spriteheight

		add	di,bx
		les	ax,dword ptr [cursor@]
		assume	es:nothing
		inc	ax
	if_ nz					; if cursor drawn
	CODE_	CMP_DI	cursorpos,<dw ?>
	 if_ eq
		cmp	dx,[spriteheight]
	 andif_ eq				; exit if position not changed
		jcxz	@drawret		;  and cursor not forced
	 end_
		push	bx dx di
		dec	ax
		xchg	di,ax			; OPTIMIZE: instead MOV DI,AX
		call	restoresprite
		;MOVSEG	es,[videoseg],,nothing
		pop	di dx
	else_
		push	bx
		call	updatevregs
	end_
		pop	bx

; bx=X, di=line offset+bx, si=nextrow, dx=spriteheight, es=videoseg

		mov	[cursorpos],di
		mov	[spriteheight],dx
		mov	[nextrow],si
		sub	di,bx
		push	dx

;---------- precompute sprite parameters

		push	bx
		mov	cl,[bitmapshift]
		mov	dx,[cursorwidth]
		sar	bx,cl			; left sprite offset (signed)
		mov	ax,[scanline]
		add	dx,bx			; right sprite offset
		cmp	dx,ax
	if_ ae
		xchg	dx,ax			; DX=min(DX,scanline)
	end_

		pop	ax			; =cursorX
		sub	cl,3			; mode 0Dh=1, other=0
		and	ax,[granumask.X]	; fix for mode 4/5
		sar	ax,cl			; sprite shift for non 13h modes
		neg	bx			; sprite shift for 13h mode
	if_ lt					; if left sprite offset>0
		add	dx,bx
		sub	di,bx
		mov	bl,0
		and	al,7			; shift in byte (X%8)
	end_

		inc	bx			; OPTIMIZE: BX instead BL
		sub	al,8			; if cursorX>0
		mov	bh,al			; ...then BH=-(8-X%8)
		push	bx			; ...else BH=-(8-X)=-(8+|X|)

;---------- save screen sprite and draw cursor at new cursor position

		mov	[spritewidth],dx
		mov	[cursor@],di
		mov	al,0D6h			; screen source
		call	copysprite		; save new sprite

	CODE_	MOV_BX	spritetop,<dw ?>
		pop	cx ax			; CL/CH=sprite shift
						; AX=[spriteheight]
						; SI=[nextrow]
		shl	bx,1			; mask offset
	countloop_ ,ax
		push	ax cx bx si di
		mov	si,[spritewidth]
		mov	dx,word ptr screenmask[bx]
		mov	bx,word ptr cursormask[bx]
		call	makerow
		pop	di si bx cx ax
		add	di,si
		xor	si,[nextxor]
		movadd	bx,,2
	end_

;----------

		;j	restorevregs
graphcursor	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Restore graphics card video registers
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

restorevregs	proc
		mov	bx,TSRdref:vdata1
		j	@writevregs
restorevregs	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Save & update graphics card video registers
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

updatevregs	proc
		mov	bx,TSRdref:vdata1
		mov	ah,0F4h			; read register set
		call	@registerset

		mov	bx,TSRdref:vdata2
@writevregs:	mov	ah,0F5h			; write register set

@registerset:	; if planar videomode [0Dh-12h] then "push es" else "ret"
		db	?
		MOVSEG	es,ds,,@TSRdata
		mov	cx,[bx-2]
		int	10h
		pop	es
		ret
updatevregs	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

restoresprite	proc
		call	updatevregs
		mov	al,0D7h			; screen destination
		;j	copysprite		; restore old sprite
restoresprite	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Copy screen sprite back and forth
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	AL			(0D6h/0D7h-screen source/dest.)
;	ES:DI			(pointer to video memory)
; Out:	CX = 0
;	BX = 0
; Use:	buffer@
; Modf:	AX, DX
; Call:	none
;
copysprite	proc	C uses si di ds es
		assume	es:nothing
		cmp	al,0D6h
		mov	NEXTOFFSCODE[1],al
	CODE_	MOV_AX	nextrow,<dw ?>		; next row offset
	CODE_	MOV_BX	spriteheight,<dw ?>	; sprite height in lines
		lds	si,[buffer@]
		assume	ds:nothing
	if_ eq
		push	ds es
		pop	ds es			; DS:SI=screen
		xchg	si,di			; ES:DI=buffer
	end_

	countloop_ ,bx
	CODE_	MOV_CX	spritewidth,<dw ?>	; seen part of sprite in bytes
		movsub	dx,ax,cx
		rep	movsb
NEXTOFFSCODE	db	01h,?			; ADD SI,DX/ADD DI,DX
	CODE_	XOR_AX	nextxor,<dw ?>
	end_
		ret
copysprite	endp
		assume	ds:@TSRdata

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Transform the cursor mask row to screen
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	DX = screenmask[row]
;	BX = cursormask[row]
;	SI = [spritewidth]
;	CL			(sprite shift when mode 13h)
;	CH			(sprite shift when non 13h modes)
;	ES:DI			(video memory pointer)
; Out:	none
; Use:	bitmapshift
; Modf:	AX, CX, DX, BX, SI, DI
; Call:	none
;
makerow		proc
		assume	es:nothing
		cmp	[bitmapshift],1		; =1 for 13h mode
	if_ eq

;----------

	 countloop_ ,si
		shl	bx,cl			; if MSB=0
		sbb	al,al			; ...then AL=0
		and	al,0Fh			; ...else AL=0Fh (WHITE color)
		shl	dx,cl
	  if_ carry				; if most sign bit nonzero
		xor	al,es:[di]
	  end_
		stosb
		mov	cl,1
	 end_ countloop
		ret
	end_ if

;---------- display cursor row in modes other than 13h

makerowno13:	mov	ax,0FFh
	loop_
		stc
		rcl	dx,1
		rcl	al,1			; al:dh:dl shifted screenmask
		shl	bx,1
		rcl	ah,1			; ah:bh:bl shifted cursormask
		inc	ch
	until_ zero
		xchg	dh,bl			; al:bl:dl - ah:bh:dh

	countloop_ ,si
		push	dx
		mov	dx,es
		cmp	dh,0A0h
	 if_ ne					; if not planar mode 0Dh-12h
		and	al,es:[di]
		xor	al,ah
		stosb
	 else_
		xchg	cx,ax			; OPTIMIZE: instead MOV CX,AX
		out_	3CEh,5,0		; set write mode
		out_	,3,08h			; data ANDed with latched data
		xchg	es:[di],cl
		out_	,,18h			; data XORed with latched data
		xchg	es:[di],ch
		inc	di
	 end_
		xchg	ax,bx			; OPTIMIZE: instead MOV AX,BX
		pop	bx
	end_ countloop
		ret
makerow		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;	Return graphic mode video memory offset to line start
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	DX			(Y coordinate in pixels)
; Out:	DI			(video memory offset)
;	SI			(offset to next row)
; Use:	videoseg, scanline
; Modf:	AX, DX, ES
; Call:	@getoffsret
; Info:	4/5 (320x200x4)   byte offset = (y/2)*80 + (y%2)*2000h + (x*2)/8
;	  6 (640x200x2)   byte offset = (y/2)*80 + (y%2)*2000h + x/8
;	0Dh (320x200x16)  byte offset = y*40 + x/8, bit offset = 7 - (x % 8)
;	0Eh (640x200x16)  byte offset = y*80 + x/8, bit offset = 7 - (x % 8)
;	0Fh (640x350x4)   byte offset = y*80 + x/8, bit offset = 7 - (x % 8)
;	10h (640x350x16)  byte offset = y*80 + x/8, bit offset = 7 - (x % 8)
;	11h (640x480x2)   byte offset = y*80 + x/8, bit offset = 7 - (x % 8)
;	12h (640x480x16)  byte offset = y*80 + x/8, bit offset = 7 - (x % 8)
;	13h (320x200x256) byte offset = y*320 + x
;	HGC (720x348x2)   byte offset = (y%4)*2000h + (y/4)*90 + x/8
;						    bit offset = 7 - (x % 8)
;
getgroffset	proc
		xor	di,di
		mov	ax,[scanline]
		MOVSEG	es,di,,BIOS
		mov	si,ax			; [nextrow]
		cmp	byte ptr videoseg[1],0A0h
		je	@getoffsret		; jump if not videomode 4-6
		mov	si,2000h
		sar	dx,1			; DX=Y/2
		jnc	@getoffsret
		mov	di,si			; DI=(Y%2)*2000h
		mov	si,-(2000h-80)
		j	@getoffsret
getgroffset	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Return text mode video memory offset
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	AX/BX			(cursor position X/Y)
; Out:	DI			(video memory offset=row*[0:44Ah]*2+column*2)
; Use:	0:44Ah, 0:44Eh, bitmapshift
; Modf:	AX, DX, BX, ES
; Call:	getpageoffset
;
gettxtoffset	proc
		MOVSEG	es,0,dx,BIOS
		xchg	di,ax			; OPTIMIZE: instead MOV DI,AX
		mov	al,[bitmapshift]
		dec	ax			; OPTIMIZE: AX instead AL
		xchg	cx,ax
		sar	di,cl			; DI=column*2
		xchg	cx,ax			; OPTIMIZE: instead MOV CX,AX
		xchg	ax,bx			; OPTIMIZE: instead MOV AX,BX
		sar	ax,2			; AX=row*2=Y/4
		mov	dx,[VIDEO_width]	; screen width

@getoffsret:	imul	dx			; AX=row*screen width
		add	ax,[VIDEO_pageoff]	; add video page offset
		add	di,ax
		ret
gettxtoffset	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Check if cursor seen and not in update region
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	AX/BX			(cursor position X/Y)
;	SI			(cursor height)
; Out:	Carry flag		(cursor not seen or in update region)
;	BX			(cursor X aligned at byte in video memory)
;	SI			(cursor Y+height)
; Use:	scanline, screenheight, upleft, lowright
; Modf:	DX
; Call:	none
;
checkifseen	proc	C uses cx

;---------- check if cursor shape seen on the screen

		add	si,bx
		jle	@@retunseen		; fail if Y+height<=0
		cmp	bx,[screenheight]
		jge	@@retunseen		; fail if Y>maxY

	CODE_	MOV_CL	bitmapshift,<db ?>	; mode 13h=1, 0Dh=4, other=3
	CODE_	MOV_DX	cursorwidth,<dw ?>	; cursor width in bytes
		sar	ax,cl
		add	dx,ax
		jle	@@retunseen		; fail if X+width<=0
		cmp	ax,[scanline]
		jge	@@retunseen		; fail if X>maxX

;---------- check if cursor shape not intersects with update region

		shl	ax,cl
		cmp	bx,[lowright.Y]
		jg	@@retseen		; ok if Y below
		cmp	si,[upleft.Y]
		jle	@@retseen		; ok if Y+height above

		cmp	ax,[lowright.X]
		jg	@@retseen		; ok if X from the right
		shl	dx,cl
		cmp	dx,[upleft.X]
		jle	@@retseen		; ok if X+width from the left

@@retunseen:	stc
		ret
@@retseen:	clc
		ret
checkifseen	endp


;€€€€€€€€€€€€€€€€€€€€€€€ INT 33 HANDLER SERVICES €€€€€€€€€€€€€€€€€€€€€€€€

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

setupvideo	proc
		mov	si,szClearArea2/2	; clear area 2
ERRIF (szClearArea2 mod 2 ne 0) "szClearArea2 must be even!"
		j	@setvideo
setupvideo	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 21 - Software reset
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	[AX] = 21h/FFFFh	(not installed/installed)
;	[BX] = 2/3/FFFFh	(number of buttons)
; Use:	0:449h, 0:44Ah, 0:463h, 0:484h, 0:487h, 0:488h, 0:4A8h
; Modf:	RedefArea, screenheight, granumask, buffer@, videoseg, cursorwidth,
;	scanline, nextxor, bitmapshift, @registerset, rangemax, ClearArea
; Call:	hidecursor, @savecutpos
;
softreset_21	proc
		mov	[_ARG_BX_],3
buttonscnt	equ	byte ptr [$-2]		; buttons count (2/3)
		mov	[_ARG_AX_],0FFFFh
						; restore default area
		memcopy	szDefArea,ds,@TSRdata,@TSRdata:RedefArea,,,@TSRdata:DefArea
		call	hidecursor		; restore screen contents
		mov	si,szClearArea3/2	; clear area 3
ERRIF (szClearArea3 mod 2 ne 0) "szClearArea3 must be even!"

;---------- setup video regs values for current video mode

@setvideo:	push	si
		MOVSEG	es,ds,,@TSRdata
		MOVSEG	ds,0,ax,BIOS
		mov	ax,[CRTC_base]		; base IO address of CRTC
		mov	videoregs@[0].port@,ax	; 3D4h/3B4h
		add	ax,6			; Feature Control register
		mov	videoregs@[(size RGROUPDEF)*5].port@,ax
		mov	al,[VIDEO_mode]		; current video mode
		push	ax

;----------

	block_
		mov	ah,9
		cmp	al,11h			; VGA videomodes?
	 breakif_ ae

		cbw				; OPTIMIZE: instead MOV AH,0
		cmp	al,0Fh			; 0F-10 videomodes?
	 if_ ae
		testflag [VIDEO_control],mask VCTRL_RAM_64K
	  breakif_ zero				; break if only 64K of VRAM
		mov	ah,2
	 else_
		cmp	al,4			; not color text modes?
	 andif_ below
		xchg	cx,ax			; OPTIMIZE: instead MOV CX,AX
		mov	al,[VIDEO_switches]	; get display combination
		maskflag al,mask VIDSW_feature0+mask VIDSW_display
		cmp	al,9			; EGA+ECD/MDA?
		je	@@lines350
		cmp	al,3			; MDA/EGA+ECD?
	  if_ eq
@@lines350:	mov	ch,13h
	  end_
		xchg	ax,cx			; OPTIMIZE: instead MOV AX,CX
	 end_ if
	end_ block

;----------

		lds	si,[VIDEO_ptrtable@]
		assume	ds:nothing
		lds	si,[si].VIDEO_paramtbl@
		add	ah,al
		mov	al,(offset VPARAM_SEQC) shl 2
		shr	ax,2
		add	si,ax			; SI += (AL+AH)*64+5

		mov	di,TSRdref:VRegsArea
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

		pop	si			; initialize area of defaults
		;mov	di,TSRdref:DefVRegsArea
ERRIF (DefVRegsArea ne VRegsArea+64) "VRegs area contents corrupted!"
		memcopy	szVRegsArea,,,,es,@TSRdata

		dec	ax			; OPTIMIZE: instead MOV AL,0
		mov	cx,8
		mov	di,TSRdref:videoregs@[0].rmodify?
	countloop_
		stosb
		add	di,(size RGROUPDEF)-1
	end_

;---------- set parameters for current video mode
; mode	 seg   screen  cell scan planar  VX/
;			    line	byte
;  0	B800h  640x200 16x8   -    -	  -
;  1	B800h  640x200 16x8   -    -	  -
;  2	B800h  640x200	8x8   -    -	  -
;  3	B800h  640x200	8x8   -    -	  -
;  4	B800h  320x200	2x1   80   no	  8
;  5	B800h  320x200	2x1   80   no	  8
;  6	B800h  640x200	1x1   80   no	  8
;  7	B000h  640x200	8x8   -    -	  -
; 0Dh	A000h  320x200	2x1   40  yes	 16
; 0Eh	A000h  640x200	1x1   80  yes	  8
; 0Fh	A000h  640x350	1x1   80  yes	  8
; 10h	A000h  640x350	1x1   80  yes	  8
; 11h	A000h  640x480	1x1   80  yes	  8
; 12h	A000h  640x480	1x1   80  yes	  8
; 13h	A000h  320x200	2x1  320   no	  2
; other     0  640x200	1x1   -    -	  -
;
		pop	ax			; current video mode
; mode 0-3
		mov	dx,0B8FFh		; B800h: [0-3]
		mov	cx,0304h		; 16x8: [0-1]
		mov	di,200			; x200: [4-6,0Dh-0Eh,13h]
		cmp	al,2
	if_ ae
		dec	cx			; 8x8: [2-3,7]
		cmp	al,4
	andif_ ae
; mode 7
		cmp	al,7
		jne	@@checkgraph
		mov	dh,0B0h			; B000h: [7]
	end_

@@settext:	mov	ch,1
		mov	bh,0F8h
		shl	dl,cl

		MOVSEG	es,0,ax,BIOS
		add	al,[VIDEO_lastrow]	; screen height-1
	if_ nz					; zero on old machines
		inc	ax			; OPTIMIZE: AX instead AL
if USE_286
		shl	ax,3
else
		mov	ah,8
		mul	ah
endif
		xchg	di,ax			; OPTIMIZE: instead MOV DI,AX
	end_
		mov	ax,[VIDEO_width]	; screen width
		j	@@setcommon

; mode 4-6
@@checkgraph:	mov	ah,0C3h			; RET opcode for [4-6,13h]
		;mov	cx,0303h		; sprite: 3 bytes/row
		;mov	dx,0B8FFh		; B800h: [4-6]/1x1: [6,0Eh-12h]
		;mov	di,200			; x200: [4-6,0Dh-0Eh,13h]
		mov	si,2000h xor -(2000h-80) ; [nextxor] for [4-6]
		;MOVSEG	es,ds,,@TSRdata
		mov	bx,TSRdref:spritebuf
		cmp	al,6
		je	@@setgraphics
		jb	@@set2x1

; in modes 0Dh-13h screen contents under cursor sprite will be
; saved at free space in video memory (A000h segment)

		mov	dh,0A0h			; A000h: [0Dh-13h]
		MOVSEG	es,0A000h,bx,nothing
		xor	si,si			; [nextxor] for [0Dh-13h]
		cmp	al,13h
		ja	@@nonstandard
		je	@@mode13
; mode 8-0Dh
		cmp	al,0Dh
		jb	@@nonstandard
		mov	ah,06h			; PUSH ES opcode for [0Dh-12h]
		mov	bx,3E82h		; 16002: [0Dh-0Eh]
		je	@@set320
; mode 0Eh-12h
		cmp	al,0Fh
		jb	@@setgraphics
		mov	di,350			; x350: [0Fh-10h]
		mov	bh,7Eh			; 32386: [0Fh-10h]
		cmp	al,11h
		jb	@@setgraphics
		mov	di,480			; x480: [11h-12h]
		mov	bh,9Eh			; 40578: [11h-12h]
		j	@@setgraphics
; mode 13h
@@mode13:	;mov	bl,0
		mov	bh,0FAh			; =320*200
		mov	cx,1000h		; sprite: 16 bytes/row

@@set320:	inc	cx			; OPTIMIZE: instead INC CL
@@set2x1:	dec	dx			; OPTIMIZE: instead MOV DL,-2

@@setgraphics:	saveFAR	[buffer@],es,bx
		mov	[nextxor],si
		mov	byte ptr [@registerset],ah
		j	@@setgcommon

@@nonstandard:	;mov	cl,3
		;mov	dl,0FFh
		;mov	di,200

;;+++++ for text modes: dh := 0B8h, j @@settext

		mov	dh,0			; no video segment

@@setgcommon:	mov	ax,640			; virtual screen width
		mov	bh,0FFh			; Y granularity
		shr	ax,cl

@@setcommon:	mov	[screenheight],di
		mov	[scanline],ax		; screen line width in bytes
		mov	[bitmapshift],cl	; log2(screen/memory ratio)
						;  (mode 13h=1, 0-1/0Dh=4, other=3)
		mov	byte ptr [cursorwidth],ch ; cursor width in bytes
		mov	byte ptr [granumask.X],dl
		mov	byte ptr [granumask.Y],bh
		mov	byte ptr videoseg[1],dh
		shl	ax,cl
		pop	si

;---------- set ranges and center cursor (AX=screenwidth, DI=screenheight)

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
		MOVSEG	es,ds,,@TSRdata
		mov	di,TSRdref:ClearArea
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 1F - Disable mouse driver
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	[AX] = 1Fh/FFFFh	(success/unsuccess)
;	[ES:BX]			(old int33 handler)
; Use:	oldint33, oldint10
; Modf:	AX, CX, DX, BX, DS, ES, disabled?, nocursorcnt
; Call:	INT 21/35, INT 21/25, disablePS2/disableUART, hidecursor
;
disabledrv_1F	proc
		les	ax,[oldint33]
		assume	es:nothing
		mov	[_ARG_ES_],es
		mov	[_ARG_BX_],ax

		call_	disableproc,disablePS2

		mov	al,[disabled?]
		test	al,al
	if_ zero				; if driver not disabled
		mov	[buttstatus],al
		inc	ax			; OPTIMIZE: instead MOV AL,1
		mov	[nocursorcnt],al	; normalize hide counter
		call	hidecursor		; restore screen contents

;---------- check if INT 33 or INT 10 were intercepted
;	    (i.e. handlers segment not equal to CS)

		mov	cx,cs
		DOSGetIntr 33h
		mov	dx,es
		cmp	dx,cx
		jne	althandler_18

		;mov	ah,35h
		mov	al,10h
		int	21h
		;DOSGetIntr 10h
		movsub	ax,es,cx
		jne	althandler_18

		inc	ax			; OPTIMIZE: instead MOV AL,1
		mov	[disabled?],al
		lds	dx,[oldint10]
		assume	ds:nothing
		DOSSetIntr 10h			; restore old INT 10 handler
	end_ if
		ret
disabledrv_1F	endp
		assume	ds:@TSRdata

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 18 - Set alternate User Interrupt Routine
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 19 - Get alternate User Interrupt Routine
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(call mask)
; Out:	[CX]			(0=not found)
;	[BX:DX]			(FAR routine)
;
althandler_19	proc
		mov	[_ARG_CX_],0
		ret
althandler_19	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 00 - Reset driver and read status
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 20 - Enable mouse driver
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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
	if_ ncxz

;---------- set new INT 10 handler

		DOSGetIntr 10h
		saveFAR	[oldint10],es,bx
		;mov	al,10h
		DOSSetIntr ,,,@TSRcode:int10handler
	end_

;----------

		call	setupvideo
		jmp_	enableproc,enablePS2
enabledriver_20	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 03 - Get cursor position and buttons status
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 05 - Get button press data
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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
		mov	si,TSRdref:buttpress
		j	@retbuttstat
pressdata_05	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 06 - Get button release data
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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
		mov	si,TSRdref:buttrelease
@retbuttstat:	;mov	ah,0
		mov	al,[buttstatus]
		mov	[_ARG_AX_],ax
if FOOLPROOF
		cmp	bx,2
	if_ be
endif
ERRIF (6 ne size BUTTLASTSTATE) "BUTTLASTSTATE structure size changed!"
		shl	bx,1
		add	si,bx			; SI+BX=buttrelease
		shl	bx,1			;  +button*size BUTTLASTSTATE
		xor	ax,ax
		xchg	[si+bx.counter],ax
		mov	cx,[si+bx.lastcol]
		mov	dx,[si+bx.lastrow]
@retBCDX:	mov	[_ARG_DX_],dx
		mov	[_ARG_CX_],cx
@retBX:		mov	[_ARG_BX_],ax
if FOOLPROOF
	end_ if
endif
		ret
releasedata_06	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0B - Get motion counters
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 15 - Get driver storage requirements
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	[BX]			(buffer size)
; Use:	szSaveArea
; Modf:	AX
; Call:	@retBX
;
storagereq_15	proc
		mov	ax,szSaveArea
		j	@retBX
storagereq_15	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 1B - Get mouse sensitivity
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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
;;*		mov	ax,
;;*		mov	cx,
;;*		mov	dx,[doublespeed]
;;*		j	@retBCDX
		ret
sensitivity_1B	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 1E - Get display page
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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
		assume	ds:@TSRdata

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 01 - Show mouse cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	none
; Modf:	AX, lowright.Y
; Call:	cursorstatus
;
showcursor_01	proc
		neg	ax			; AL=AH=-1
		mov	byte ptr lowright.Y[1],al ; place update region
		j	cursorstatus		;  outside seen screen area
showcursor_01	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 02 - Hide mouse cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	none
; Modf:	AX
; Call:	cursorstatus
;
hidecursor_02	proc
		dec	ax			; AL=1,AH=0
		;j	cursorstatus
hidecursor_02	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Hint:	request to cursor redraw (instead refresh) is useful in cases when
;	interrupt handlers try to hide, then show cursor while cursor
;	drawing is in progress
;
cursorstatus	proc
		add	al,[nocursorcnt]
		sub	ah,al			; exit if "already enabled"
		jz	@showret		;   or "counter overflow"
		mov	[nocursorcnt],al
		inc	ah			; jump if cursor changed
		jz	redrawcursor		;  between enabled/disabled
		ret
cursorstatus	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 07 - Set horizontal cursor range
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 08 - Set vertical cursor range
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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
if FOOLPROOF
@setnewrange:	xchg	ax,cx			; OPTIMIZE: instead MOV AX,CX
		cmp	ax,dx
	if_ ge
		xchg	ax,dx
	end_
		mov	word ptr rangemin[bx],ax
else
@setnewrange:	mov	word ptr rangemin[bx],cx
endif
		mov	word ptr rangemax[bx],dx
		mov	cx,[pos.X]
		mov	dx,[pos.Y]
		;j	setpos_04
vrange_08	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 04 - Position mouse cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(X - column)
;	DX			(Y - row)
; Out:	none
; Use:	none
; Modf:	SI
; Call:	@setpos, refreshcursor
;
setpos_04	proc
		mov	si,szClearArea1/2	; clear area 1
ERRIF (szClearArea1 mod 2 ne 0) "szClearArea1 must be even!"
		call	@setpos
		;j	refreshcursor
setpos_04	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

refreshcursor	proc
		sub	[videolock],1
		jc	@showret		; was 0: drawing in progress
		js	@@refreshdone		; was -1: queue already used
		sti

	loop_
		call	drawcursor
@@refreshdone:	inc	[videolock]		; drawing stopped
	until_ nz				; loop until queue empty

		cli
@showret:	ret
refreshcursor	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 09 - Define graphics cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	BX			(hot spot X)
;	CX			(hot spot Y)
;	ES:DX			(pointer to bitmaps)
; Out:	none
; Use:	none
; Modf:	AX, CX, BX, SI, DI, ES, hotspot, screenmask, cursormask
; Call:	@showret, redrawcursor
;
graphcursor_09	proc
		assume	es:nothing

;---------- compare user shape with internal area

		mov	si,TSRdref:hotspot
		lodsw
		cmp	ax,bx
	if_ eq
		lodsw
		xor	ax,cx
	andif_ eq
		mov	di,dx
		;mov	ah,0
		mov	al,16+16
		xchg	ax,cx
		repe	cmpsw
		je	@showret		; exit if cursor not changed
		xchg	cx,ax			; OPTIMIZE: instead MOV CX,AX
	end_

;---------- copy user shape to internal area

		push	ds ds es
		pop	ds es
		mov	di,TSRdref:hotspot
		xchg	ax,bx			; OPTIMIZE: instead MOV AX,BX
		stosw
		xchg	ax,cx			; OPTIMIZE: instead MOV AX,CX
		stosw
		memcopy	2*(16+16),,,,,,dx
		pop	ds
		;j	redrawcursor
graphcursor_09	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

redrawcursor	proc
hidecursor:	mov	[newcursor],1		; force cursor redraw
		j	refreshcursor
redrawcursor	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0A - Define text cursor
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	BX			(0 - SW, else HW text cursor)
;	CX			(screen mask/start scanline)
;	DX			(cursor mask/end scanline)
; Out:	none
; Use:	none
; Modf:	AX, CX, BX, cursortype, startscan, endscan
; Call:	INT 10/01, @showret, redrawcursor
;
textcursor_0A	proc
		xchg	cx,bx
	if_ ncxz				; if hardware cursor
		mov	ch,bl
		mov	cl,dl
		mov	ah,1
		int	10h			; set cursor shape & size
		mov	cl,1
	end_
		cmp	cl,[cursortype]
	if_ eq
		cmp	bx,[startscan]
	andif_ eq
		cmp	dx,[endscan]
		je	@showret		; exit if cursor not changed
	end_

;----------

		mov	[cursortype],cl
		mov	[startscan],bx
		mov	[endscan],dx
		j	redrawcursor
textcursor_0A	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 10 - Define screen region for updating
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX, DX			(X/Y of upper left corner)
;	SI, DI			(X/Y of lower right corner)
; Out:	none
; Use:	none
; Modf:	AX, CX, DX, DI, upleft, lowright
; Call:	redrawcursor
;
updateregion_10	proc
		mov	ax,[_ARG_SI_]
if FOOLPROOF
		cmp	cx,ax
	if_ ge
		xchg	cx,ax
	end_
		mov	[upleft.X],cx
		mov	[lowright.X],ax
		xchg	ax,di			; OPTIMIZE: instead MOV AX,DI
		cmp	dx,ax
	if_ ge
		xchg	dx,ax
	end_
		mov	[upleft.Y],dx
		mov	[lowright.Y],ax
else
		mov	[upleft.X],cx
		mov	[upleft.Y],dx
		mov	[lowright.X],ax
		mov	[lowright.Y],di
endif
		j	redrawcursor
updateregion_10	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 16 - Save driver state
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	BX			(buffer size)
;	ES:DX			(buffer)
; Out:	none
; Use:	SaveArea
; Modf:	CX, SI, DI
; Call:	none
;
savestate_16	proc
		assume	es:nothing
if FOOLPROOF
;;-		cmp	bx,szSaveArea		;!!! TurboPascal IDE
;;-		jb	@stateret		;  workaround: it not init BX
endif
		memcopy	szSaveArea,,,dx,,,@TSRdata:SaveArea
@stateret:	ret
savestate_16	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 17 - Restore driver state
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	BX			(buffer size)
;	ES:DX			(saved state buffer)
; Out:	none
; Use:	none
; Modf:	SI, DI, DS, ES, SaveArea
; Call:	@stateret, redrawcursor
;
restorestate_17	proc
		assume	es:nothing
if FOOLPROOF
;;-		cmp	bx,szSaveArea		;!!! TurboPascal IDE
;;-		jb	@stateret		;  workaround: it not init BX
endif

;---------- do nothing if SaveArea is not changed

;;*		mov	si,TSRdref:SaveArea
;;*		mov	di,dx
;;*		mov	cx,szSaveArea/2
;;*ERRIF (szSaveArea mod 2 ne 0) "szSaveArea must be even!"
;;*		repe	cmpsw
;;*		je	@stateret

;---------- change SaveArea

		push	es dx
		MOVSEG	es,ds,,@TSRdata
		pop	si ds
		assume	ds:nothing
		memcopy	szSaveArea,,,@TSRdata:SaveArea
		j	redrawcursor
restorestate_17	endp
		assume	ds:@TSRdata

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0D - Light pen emulation ON
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	none
; Modf:	none
; Call:	lightpenoff_0E
;
;;*lightpenon_0D	proc
;;*		mov	al,0
;;*		;j	lightpenoff_0E
;;*lightpenon_0D	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0E - Light pen emulation OFF
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	none
; Use:	none
; Modf:	nolightpen?
; Call:	none
;
;;*lightpenoff_0E	proc
;;*		mov	[nolightpen?],al	; OPTIMIZE: AL instead nonzero
;;*		ret
;;*lightpenoff_0E	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 14 - Exchange User Interrupt Routines
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0C - Define User Interrupt Routine
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0F - Set mickeys/pixels ratios
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	CX			(number of mickeys per 8 pix
;	DX			 horizontally/vertically)
; Out:	none
; Use:	none
; Modf:	mickey8
; Call:	none
;
sensitivity_0F	proc
if FOOLPROOF
		test	dx,dx
	if_ nz					; ignore wrong ratio
	andif_ ncxz				; ignore wrong ratio
endif
		mov	[mickey8.X],cx
		mov	[mickey8.Y],dx
if FOOLPROOF
	end_
endif
		ret
sensitivity_0F	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 1A - Set mouse sensitivity
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 13 - Define double-speed threshold
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	DX			(speed threshold in mickeys/second)
; Out:	none
; Use:	none
; Modf:	/DX/, /doublespeed/
; Call:	none
;
doublespeed_13	proc
;;*		test	dx,dx
;;*	if_ zero
;;*		mov	dl,64
;;*	end_
;;*		mov	[doublespeed],dx
		;ret
doublespeed_13	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 0D 0E 11 12 18 19 1C 1D - Null function for not implemented calls
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

nullfunc	proc
		ret
nullfunc	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;				INT 33 handler
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

		evendata
handler33table	dw TSRcref:resetdriver_00
		dw TSRcref:showcursor_01
		dw TSRcref:hidecursor_02
		dw TSRcref:status_03
		dw TSRcref:setpos_04
		dw TSRcref:pressdata_05
		dw TSRcref:releasedata_06
		dw TSRcref:hrange_07
		dw TSRcref:vrange_08
		dw TSRcref:graphcursor_09
		dw TSRcref:textcursor_0A
		dw TSRcref:mickeys_0B
		dw TSRcref:UIR_0C
		dw TSRcref:nullfunc		;lightpenon_0D
		dw TSRcref:nullfunc		;lightpenoff_0E
		dw TSRcref:sensitivity_0F
		dw TSRcref:updateregion_10
		dw TSRcref:nullfunc		;11 - genius driver only
		dw TSRcref:nullfunc		;12 - large graphics cursor
		dw TSRcref:doublespeed_13
		dw TSRcref:exchangeUIR_14
		dw TSRcref:storagereq_15
		dw TSRcref:savestate_16
		dw TSRcref:restorestate_17
		dw TSRcref:althandler_18
		dw TSRcref:althandler_19
		dw TSRcref:sensitivity_1A
		dw TSRcref:sensitivity_1B
		dw TSRcref:nullfunc		;1C - InPort mouse only
		dw TSRcref:nullfunc		;1D - define display page #
		dw TSRcref:videopage_1E
		dw TSRcref:disabledrv_1F
		dw TSRcref:enabledriver_20
		dw TSRcref:softreset_21

handler33	proc
		assume	ds:nothing,es:nothing
		cld
		test	ah,ah
	if_ zero
		push	ds
		MOVSEG	ds,cs,,@TSRdata
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
	end_
		iret
		assume	ds:@TSRdata

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 23 - Get language for messages
; Out:	[BX]			(language code: 0 - English)
;
language_23:	cmp	al,23h
		je	@iretBX0

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 24 - Get software version, mouse type and IRQ
; Out:	[AX] = 24h/FFFFh	(installed/error)
;	[BX]			(version)
;	[CL]			(IRQ #/0=PS/2)
;	[CH] = 1=bus/2=serial/3=InPort/4=PS2/5=HP (mouse type)
; Use:	driverversion
;
version_24:	cmp	al,24h
	if_ eq
		mov	bx,driverversion
	CODE_	MOV_CX	mouseinfo,<db ?,4>
	end_

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 26 - Get maximum virtual screen coordinates
; Out:	[BX]			(mouse disabled flag)
;	[CX]			(max virtual screen X)
;	[DX]			(max virtual screen Y)
; Use:	bitmapshift
;
maxscreen_26:	cmp	al,26h
	if_ eq
		mov	cl,[bitmapshift]
	CODE_	MOV_BX	scanline,<dw ?>
	CODE_	MOV_DX	screenheight,<dw ?>
		shl	bx,cl
		dec	dx
		mov	cx,bx
		dec	cx
	CODE_	MOV_BX	disabled?,<db 1,0>	; 1=driver disabled
	end_

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 27 - Get screen/cursor masks and mickey counters
; Out:	[AX]			(screen mask/start scanline)
;	[BX]			(cursor mask/end scanline)
;	[CX]			(number of mickeys mouse moved
;	[DX]			 horizontally/vertically since last call)
; Use:	startscan, endscan
; Modf:	mickeys
;
cursor_27:	cmp	al,27h
	if_ eq
		mov	ax,[startscan]
		mov	bx,[endscan]
		xor	cx,cx
		xor	dx,dx
		xchg	cx,[mickeys.X]
		xchg	dx,[mickeys.Y]
		pop	ds
		iret
	end_

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 31 - Get current virtual cursor coordinates
; Out:	[AX]			(min virtual cursor X)
;	[BX]			(min virtual cursor Y)
;	[CX]			(max virtual cursor X)
;	[DX]			(max virtual cursor Y)
; Use:	rangemin, rangemax
;
cursrange_31:	cmp	al,31h
	if_ eq
		mov	ax,[rangemin.X]
		mov	bx,[rangemin.Y]
		lds	cx,[rangemax]
		mov	dx,ds
		pop	ds
		iret
	end_

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 32 - Get supported advanced functions flag
; Out:	[AX]			(bits 15-0=function 25h-34h supported)
;	[CX] = 0
;	[DX] = 0
;	[BX] = 0
;
active_32:	cmp	al,32h
	if_ eq
		mov	ax,0110010000001100b	; active: 26 27 2A 31 32
		xor	cx,cx
		xor	dx,dx
@iretBX0:	xor	bx,bx
		pop	ds
		iret
	end_

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 4D - Get pointer to copyright string
; Out:	[ES:DI]			(copyright string)
; Use:	IDstring
;
copyright_4D:	cmp	al,4Dh
	if_ eq
		MOVSEG	es,cs,,@TSRcode
		mov	di,TSRcref:IDstring
	end_

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 6D - Get pointer to version
; Out:	[ES:DI]			(version string)
; Use:	msversion
;
version_6D:	cmp	al,6Dh
	if_ eq
		MOVSEG	es,cs,,@TSRcode
		mov	di,TSRcref:msversion
	end_

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; 2A - Get cursor hot spot
; Out:	[AX]			(cursor visibility counter)
;	[BX]			(hot spot X)
;	[CX]			(hot spot Y)
;	[DX] = 1=bus/2=serial/3=InPort/4=PS2/5=HP (mouse type)
; Use:	nocursorcnt, hotspot
;
hotspot_2A:	cmp	al,2Ah
	if_ eq
		;mov	ah,0
		mov	al,[nocursorcnt]
		lds	bx,[hotspot]
		mov	cx,ds
	CODE_	MOV_DX	mouseinfo1,<db 4,0>
	end_

		pop	ds
		iret
handler33	endp

;€€€€€€€€€€€€€€€€€€€€€€€€ END OF INT 33 SERVICES €€€€€€€€€€€€€€€€€€€€€€€€


RILversion	label
msversion	db driverversion / 100h,driverversion mod 100h
IDstring	db 'CuteMouse ',CTMVER,0
szIDstring = $ - IDstring

TSRend		label


;€€€€€€€€€€€€€€€€€€€€€€€ INITIALIZATION PART DATA €€€€€€€€€€€€€€€€€€€€€€€

.const

messages segment virtual ; place at the end of current segment
include ctmouse.msg
messages ends

S_mousetype	dw dataref:S_atPS2
		dw dataref:S_inMSYS
		dw dataref:S_inLT
		dw dataref:S_inMS

.data

options		dw 0
OPT_PS2		equ	    1b
OPT_serial	equ	   10b
OPT_COMforced	equ	  100b
OPT_PS2after	equ	 1000b
OPT_3button	equ	10000b
OPT_noMSYS	equ    100000b
OPT_lefthand	equ   1000000b
OPT_noUMB	equ  10000000b
OPT_newTSR	equ 100000000b


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€ REAL START €€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

.code

say		macro	stroff:vararg
		MOVOFF_	di,<stroff>
		call	sayASCIIZ
endm

real_start:	cld
		DOSGetIntr 33h
		saveFAR [oldint33],es,bx	; save old INT 33h handler

;---------- parse command line and find mouse

		say	@data:Copyright		; 'Cute Mouse Driver'
		mov	si,offset PSP:cmdline_len
		lodsb
		cbw				; OPTIMIZE: instead MOV AH,0
		mov	bx,ax
		mov	[si+bx],ah		; OPTIMIZE: AH instead 0
		call	commandline		; examine command line

		mov	al,1Fh			; disable old driver
		call	mousedrv

;----------

		mov	ax,[options]
		testflag ax,OPT_PS2+OPT_serial
	if_ zero				; if no /S and /P then
		setflag	ax,OPT_PS2+OPT_serial	;  both PS2 and serial assumed
	end_
;---
		testflag ax,OPT_PS2after
	if_ nz
		call	searchCOM		; call if /V
		jnc	@@serialfound
	end_
;---
		testflag ax,OPT_PS2+OPT_PS2after
	if_ nz
		push	ax
		call	checkPS2		; call if /V or PS2
		pop	ax
	andif_ nc
		mov	mouseinfo[0],bh
		j	@@mousefound
	end_
;---
		testflag ax,OPT_PS2after
	if_ zero
		testflag ax,OPT_serial+OPT_noMSYS
	andif_ nz
		call	searchCOM		; call if no /V and serial
		jnc	@@serialfound
	end_
		mov	di,dataref:E_notfound	; 'Error: device not found'
		jmp	EXITENABLE

;----------

@@serialfound:	;push	ax			; preserve OPT_newTSR value
		mov	al,2
		mov	mouseinfo[1],al
		mov	[mouseinfo1],al
		;pop	ax
@@mousefound:	mov	[mousetype],bl

;---------- check if CuteMouse driver already installed

		testflag ax,OPT_newTSR
		jnz	@@newTSR
		call	getCuteMouse
		mov	di,dataref:S_reset	; 'Resident part reset to'
		mov	cx,4C02h		; terminate, al=return code

	if_ ne

;---------- allocate UMB memory, if possible, and set INT 33 handler

@@newTSR:	mov	bx,(TSRend-TSRstart+15)/16
		push	bx
		call	prepareTSR		; new memory segment in ES
		memcopy	<size oldint33>,es,,@TSRdata:oldint33,ds,,@TSRdata:oldint33
		push	ds
		MOVSEG	ds,es,,@TSRcode
		mov	[disabled?],1		; copied back in setupdriver
		DOSSetIntr 33h,,,@TSRcode:handler33
		POPSEG	ds,@data
		pop	ax
		mov	di,dataref:S_installed	; 'Installed at'
		mov	cl,0			; errorlevel
	end_ if

;----------

		push	ax			; size of TSR for INT 21/31
		say	di
		mov	al,[mousetype]

		mov	bx,dataref:S_CRLF
		shl	al,1
	if_ carry				; if wheel (=8xh)
		mov	bx,dataref:S_wheel
	end_

		cbw				; OPTIMIZE: instead MOV AH,0
		cmp	al,1 shl 1
		xchg	si,ax			; OPTIMIZE: instead MOV SI,AX
	if_ ae					; if not PS/2 mode (=0)
	 if_ eq					; if Mouse Systems (=1)
		inc	cx			; OPTIMIZE: CX instead CL
	 end_
		say	@data:S_atCOM
	end_
		push	cx			; exit function and errorlevel
		say	S_mousetype[si]
		say	bx
		call	setupdriver

;---------- close all handles (20 pieces) to prevent decreasing system
;	    pool of handles if INT 21/31 used

		mov	bx,19
	loop_
		DOSCloseFile
		dec	bx
	until_ sign

;----------

		pop	ax dx			; AH=31h (TSR) or 4Ch (EXIT)
		int	21h

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Setup resident driver code and parameters
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

setupdriver	proc

;---------- detect VGA card (VGA ATC have one more register)

		mov	ax,1A00h
		int	10h			; get display type in BX
		cmp	al,1Ah
	if_ eq
		xchg	ax,bx			; OPTIMIZE: instead MOV AL,BL
		sub	al,7
		cmp	al,8-7
	andif_ be					; if monochrome or color VGA
		inc	videoregs@[(size RGROUPDEF)*3].regscnt
	end_

;---------- setup left hand mode handling

	CODE_	MOV_CX	mousetype,<db ?,0>	; 0=PS/2,1=MSys,2=LT,3=MS,
						; 83h=MS+wheel
		mov	al,00000000b	; =0
	if_ ncxz				; if not PS/2 mode (=0)
		mov	al,00000011b	; =3
	end_
		testflag [options],OPT_lefthand
	if_ nz
		xor	al,00000011b	; =3
	end_
		mov	[swapmask],al

;---------- setup buttons count and mask

		mov	al,3
		testflag [options],OPT_3button
	if_ zero
		jcxz	@@setbuttons		; jump if PS/2 mode (=0)
		cmp	cl,al			; OPTIMIZE: AL instead 3
	andif_ eq				; if MS mode (=3)
@@setbuttons:	mov	[buttonsmask],al	; OPTIMIZE: AL instead 0011b
		dec	ax
		mov	[buttonscnt],al		; OPTIMIZE: AL instead 2
	end_

;---------- setup mouse handlers code

	block_
	 breakif_ cxz				; break if PS/2 mode (=0)

		fixcode	IRQproc,0B0h,%OCW2<OCW2_EOI> ; MOV AL,OCW2<OCW2_EOI>
		fixnear	enableproc,enableUART
		fixnear	disableproc,disableUART
		dec	cx
	 breakif_ zero				; break if Mouse Systems mode (=1)

		fixnear	mouseproc,MSLTproc
		dec	cx
	 breakif_ zero				; break if Logitech mode (=2)

		fixcode	MSLTCODE3,,2
		loop	@@setother		; break if wheel mode (=83h)

		cmp	al,2			; OPTIMIZE: AL instead [buttonscnt]
	 if_ ne					; if not MS2
		fixcode	MSLTCODE2,075h		; JNZ
	 end_
		mov	al,0C3h			; RET
		fixcode	MSLTCODE1,al
		fixcode	MSLTCODE3,al
	end_ block

;---------- setup, if required, other parameters

@@setother:	push	es ds es ds
		pop	es ds			; get back [oldint10]...
		memcopy	<size oldint10>,es,,@TSRdata:oldint10,ds,,@TSRdata:oldint10
		mov	al,[disabled?]
		pop	ds
		mov	[disabled?],al		; ...and [disabled?]

		DOSGetIntr [IRQintnum]
		mov	ax,es
		pop	es
		mov	di,TSRdref:oldIRQaddr
		xchg	ax,bx
		stosw				; save old IRQ handler
		xchg	ax,bx			; OPTIMIZE: instead MOV AX,BX
		stosw

;---------- copy TSR image (even if ES=DS - this is admissible)

		memcopy	((TSRend-TSRdata+1)/2)*2,es,,@TSRdata:TSRdata,ds,,@TSRdata:TSRdata

;---------- call INT 33/0000 (Reset driver)

		pop	ax
		pushf				;!!! Logitech MouseWare
		push	cs ax			;  Windows driver workaround
		mov	ax,TSRcref:handler33
		push	es ax
		xor	ax,ax			; reset driver
		retf
setupdriver	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Check given or all COM-ports for mouse connection
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

searchCOM	proc
		;mov	[LCRset],LCR<0,,LCR_noparity,0,2>
		mov	di,coderef:detectmouse
		call	COMloop
		jnc	@searchret

		testflag [options],OPT_noMSYS
		stc
		jnz	@searchret

		mov	[LCRset],LCR<0,,LCR_noparity,0,3>
		mov	bl,1			; =Mouse Systems mode
		mov	di,coderef:checkUART
		;j	COMloop
searchCOM	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

COMloop		proc
		push	ax
		xor	ax,ax			; scan only current COM port
		testflag [options],OPT_COMforced
		jnz	@@checkCOM
		mov	ah,3			; scan all COM ports

	loop_
		inc	ax			; OPTIMIZE: AX instead AL
		push	ax
		call	setCOMport
		pop	ax
@@checkCOM:	push	ax
		mov	si,[IO_address]
		call	di
		pop	ax
		jnc	@@searchbreak
		dec	ah
	until_ sign
		;stc				; preserved from prev call

@@searchbreak:	pop	ax
@searchret:	ret
COMloop		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Check if UART available
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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

	if_ zero				; zero if LCR conforms

;---------- check IER for reserved bits

		movidx	dx,IER_index,si,LCR_index
		 in	al,dx			; {3F9h} IER (int enable reg)
		movidx	dx,LCR_index,si,IER_index
		;mov	ah,0
		and	al,mask IER_reserved	; reserved bits should be clear
	end_ if

		neg	ax			; nonzero makes carry flag
		pop	ax
		 out	dx,al			; {3FBh} LCR: restore contents
		ret

@@noUART:	stc
		ret
checkUART	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Detect mouse type if present
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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
	loop_
		mov	ah,byte ptr [BIOS_timer]
	 loop_
		cmp	ah,byte ptr [BIOS_timer]
	 until_ ne				; loop until next timer tick
		xor	al,1
	until_ zero				; loop until end of 2nd tick

		movidx	dx,MCR_index,si,RBR_index
		 out_	dx,%MCR<,,,0,,1,1>	; {3FCh} MCR: DTR/RTS on, OUT2 off

;---------- detect if Microsoft or Logitech mouse present

		mov	bx,0103h		; bl=mouse type, bh=no `M'
	countloop_ 4,cl				; scan 4 first bytes
	 countloop_ 2+1,ch			; length of silence in ticks
						; (include rest of curr tick)
		mov	ah,byte ptr [BIOS_timer]
	  loop_
		movidx	dx,LSR_index,si
		 in	al,dx			; {3FDh} LSR (line status reg)
		testflag al,mask LSR_RBF
		 jnz	@@parse			; jump if data ready
		cmp	ah,byte ptr [BIOS_timer]
	  until_ ne				; loop until next timer tick
	 end_ countloop				; loop until end of 2nd tick
	 break_					; break if no more data

@@parse:	movidx	dx,RBR_index,si
		 in	al,dx			; {3F8h} receive byte
		cmp	al,'('-20h
	 breakif_ eq				; break if PnP data starts
		cmp	al,'M'
	 if_ eq
		mov	bh,0			; MS compatible mouse found...
	 end_
		cmp	al,'Z'
	 if_ eq
		mov	bl,83h			; ...MS mouse+wheel found
	 end_
		cmp	al,'3'
	 if_ eq
		mov	bl,2			; ...Logitech mouse found
	 end_
	end_ countloop

		movidx	dx,LCR_index,si
		 pop	ax			; {3FBh} LCR: restore contents
		 out	dx,ax			; {3FCh} MCR: restore contents

		shr	bh,1			; 1 makes carry flag
@@detmret:	ret
detectmouse	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;				Check for PS/2
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
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
		MOVSEG	es,cs,,@code
		mov	bx,coderef:PS2dummy
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

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;				Set COM port
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	AL			(COM port, 1-4)
; Out:	none
; Use:	0:400h
; Modf:	AL, CL, ES, com_port, IO_address, S_atIO
; Call:	setIRQ
;
setCOMport	proc
		push	ax di
		add	al,'0'
		mov	[com_port],al

		cbw				; OPTIMIZE: instead MOV AH,0
		xchg	di,ax			; OPTIMIZE: instead MOV DI,AX
		MOVSEG	es,0,ax,BIOS
		shl	di,1
		mov	ax,COM_base[di-'1'-'1']
		mov	[IO_address],ax

		mov	di,dataref:S_atIO	; string for 4 digits
		MOVSEG	es,ds,,@data
		_word_hex

		pop	di ax
		and	al,1			; 1=COM1/3, 0=COM2/4
		add	al,3			; IRQ4 for COM1/3
		;j	setIRQ			; IRQ3 for COM2/4
setCOMport	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;				Set IRQ number
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	AL			(IRQ#, 1-7)
; Out:	none
; Use:	none
; Modf:	AL, CL, IRQno, mouseinfo, IRQintnum, PIC1state, notPIC1state
; Call:	none
;
setIRQ		proc
		add	al,'0'
		mov	[IRQno],al
		sub	al,'0'
		mov	mouseinfo[0],al
		mov	cl,al
		add	al,8			; INT=IRQ+8
		mov	[IRQintnum],al
		mov	al,1
		shl	al,cl			; convert IRQ into bit mask
		mov	[PIC1state],al		; PIC interrupt disabler
		not	al
		mov	[notPIC1state],al	; PIC interrupt enabler
		ret
setIRQ		endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Check if CuteMouse driver is installed
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	none
; Out:	Zero flag		(ZF=1 if installed)
;	ES			(driver segment)
; Use:	IDstring
; Modf:	AX, CX, SI, DI
; Call:	mousedrv
;
getCuteMouse	proc
		xor	di,di
		mov	al,4Dh			; get copyright string
		call	mousedrv
		mov	si,TSRcref:IDstring
		cmp	di,si
	if_ eq
		mov	cx,szIDstring
		repe	cmpsb
	end_
		ret
getCuteMouse	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Call mouse driver if present
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	AL			(function)
; Out:	results of INT 33h, if driver installed
; Use:	oldint33
; Modf:	AH
; Call:	INT 33
;
mousedrv	proc
		mov	cx,word ptr oldint33[2]
	if_ ncxz
		mov	ah,0
		pushf				;!!! Logitech MouseWare
		call	[oldint33]		;  Windows driver workaround
	end_
		ret
mousedrv	endp


;€€€€€€€€€€€€€€€€€€€€€€€€€ COMMAND LINE PARSING €€€€€€€€€€€€€€€€€€€€€€€€€

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Parse Serial option
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

_serialopt	proc
		mov	bx,(4 shl 8)+1
		call	parsedigit
	if_ nc					; '/Sc' -> set COM port
		setflag	[options],OPT_COMforced
		call	setCOMport

		;mov	bl,1
		mov	bh,7
		call	parsedigit
		jnc	setIRQ			; '/Sci' -> set IRQ line
	end_
		ret
_serialopt	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Parse Resolution option
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

_resolution	proc
		;mov	ah,0
		mov	bx,(9 shl 8)+0
		call	parsedigit		; first argument
	if_ nc
		mov	ah,al
		;mov	bx,(9 shl 8)+0
		call	parsedigit		; second argument
		jnc	@@setres		; jump if digit present
	end_
		mov	al,ah			; replicate missing argument

@@setres:	mov	[mresolutionX],ah
		mov	[mresolutionY],al
		ret
_resolution	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; In:	DS:SI			(string pointer)
;	BL			(low bound)
;	BH			(upper bound)
; Out:	DS:SI			(pointer after digit)
;	Carry flag		(no digit)
;	AL			(digit if no carry)
; Use:	none
; Modf:	CX
; Call:	BADOPTION
;
parsedigit	proc
		lodsb
		;_ch2digit
		sub	al,'0'
		cmp	al,bh
	if_ be
		cmp	al,bl
		jae	@ret			; JAE mean CF=0
	end_
		cmp	al,10
		mov	cx,dataref:E_argument	; 'Error: Invalid argument'
		jb	BADOPTION		; error if decimal digit
		dec	si
		stc
@ret:		ret
parsedigit	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;		Check if mouse services already present
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

_checkdriver	proc
		mov	cx,word ptr oldint33[2]
		jcxz	@ret
		;mov	ah,0
		mov	al,21h			; OPTIMIZE: AL instead AX
		int	33h
		inc	ax
		jnz	@ret
		mov	di,dataref:E_mousepresent ; 'Mouse service already...'
		j	EXITMSG
_checkdriver	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
.const

OPTION		struc
  optchar	db ?
  optmask	dw 0
  optproc@	dw ?
ends

OPTABLE		OPTION <'P',OPT_PS2,			@ret>
		OPTION <'S',OPT_serial,			_serialopt>
		OPTION <'Y',OPT_noMSYS,			@ret>
		OPTION <'V',OPT_PS2after,		@ret>
		OPTION <'3' and not 20h,OPT_3button,	@ret>
		OPTION <'R',,				_resolution>
		OPTION <'L',OPT_lefthand,		@ret>
		OPTION <'B',,				_checkdriver>
		OPTION <'N',OPT_newTSR,			@ret>
		OPTION <'W',OPT_noUMB,			@ret>
		OPTION <'U',,				unloadTSR>
		OPTION <'?' and not 20h,,		EXITMSG>
OPTABLEend	label

.code

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; In:	DS:SI			(null terminated command line)
;
commandline	proc
	loop_
		lodsb
		test	al,al
		jz	@ret			; exit if end of command line
		cmp	al,' '
	until_ above				; skips spaces and controls

		cmp	al,'/'			; option character?
	if_ eq
		lodsb
		and	al,not 20h		; uppercase
		mov	di,dataref:Syntax	; 'Options:'
		mov	bx,dataref:OPTABLE
	 loop_
		cmp	al,[bx].optchar
	  if_ eq
		mov	ax,[bx].optmask
		or	[options],ax
		call	[bx].optproc@
		j	commandline
	  end_
		add	bx,size OPTION
		cmp	bx,dataref:OPTABLEend
	 until_ ae
	end_ if

		mov	cx,dataref:E_option	; 'Error: Invalid option'
BADOPTION:	say	@data:E_error		; 'Error: Invalid '
		say	cx			; 'option'/'argument'
		mov	di,dataref:E_help	; 'Enter /? on command line'

EXITMSG:	mov	bl,[di]
		inc	di
		say	di
		say	@data:S_CRLF
		xchg	ax,bx			; OPTIMIZE: instead MOV AL,BL
		.exit				; terminate, al=return code
commandline	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; In:	DS:DI			(null terminated string)
; Out:	none
; Use:	none
; Modf:	AH, DL, DI
; Call:	none
;
sayASCIIZ_	proc
	loop_
		mov	ah,2
		int	21h		; write character in DL to stdout
		inc	di
sayASCIIZ:	mov	dl,[di]
		test	dl,dl
	until_ zero
		ret
sayASCIIZ_	endp


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€ TSR MANAGEMENT €€€€€€€€€€€€€€€€€€€€€€€€€€€€

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Unload driver and quit
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹

unloadTSR	proc
		call	getCuteMouse		; check if CTMOUSE installed
		mov	di,dataref:E_nocute	; 'CuteMouse driver is not installed!'
		jne	EXITMSG

		push	es
		mov	al,1Fh			; disable CuteMouse driver
		call	mousedrv
		mov	cx,es
		pop	es

		cmp	al,1Fh
		mov	di,dataref:E_notunload	; 'Driver unload failed...'
	if_ eq
		saveFAR	[oldint33],cx,bx
		push	ds
		DOSSetIntr 33h,cx,,bx		; restore old int33 handler
		pop	ds
		call	FreeMem
		mov	di,dataref:S_unloaded	; 'Driver successfully unloaded...'
	end_

EXITENABLE:	mov	al,20h			; enable old/current driver
		call	mousedrv
		j	EXITMSG
unloadTSR	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
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
	if_ ncxz				; suggested by Matthias Paul
		DOSFreeMem cx			; release environment block
	end_
		assume	ds:@data

		call	AllocUMB
		mov	ax,ds
		mov	ch,31h			; TSR exit, al=return code
		cmp	dx,ax
	if_ ne					; if TSR not "in place"
		push	ds
		dec	ax			; current MCB
		dec	dx			; target MCB...
						; ...copy process name
		memcopy	8,dx,MCB,MCB:ownername,ax,MCB,MCB:ownername
		POPSEG	ds,@data
		inc	dx
		mov	[MCB:ownerID],dx	; ...set owner to itself

		mov	ch,4Ch			; terminate, al=return code
	end_ if
		mov	es,dx
		mov	es:[PSP:DOS_exit],cx	; memory shouldn't be
						;  interpreted as PSP
						;  (CX != 20CDh)
		ret
prepareTSR	endp


;€€€€€€€€€€€€€€€€€€€€€€€€€€€ MEMORY HANDLING €€€€€€€€€€€€€€€€€€€€€€€€€€€€

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Get XMS handler address
;
; In:	none
; Out:	Carry flag		(set if no XMS support)
; Use:	none
; Modf:	AX, CX, BX, XMSentry
; Call:	INT 2F/4300, INT 2F/4310
;
getXMSaddr	proc	C uses es
		DOSGetIntr 2Fh			; suggested by Matthias Paul
		mov	cx,es
		stc
	if_ ncxz				; if INT 2F initialized
		mov	ax,4300h
		int	2Fh			; XMS: installation check
		cmp	al,80h
		stc
	andif_ eq				; if XMS service present
		mov	ax,4310h		; XMS: Get Driver Address
		int	2Fh
		saveFAR [XMSentry],es,bx
		clc
	end_
		ret
getXMSaddr	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Save allocation strategy
;
; In:	none
; Out:	Carry flag		(no UMB link supported)
; Use:	none
; Modf:	AX, SaveMemStrat, SaveUMBLink
; Call:	INT 21/5800, INT 21/5802
;
SaveStrategy	proc
		DOSGetAlloc			; get DOS alloc strategy
		mov	[SaveMemStrat],ax
		DOSGetUMBlink			; get UMB link state
		mov	[SaveUMBLink],al
		ret
SaveStrategy	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; Restore allocation strategy
;
; In:	none
; Out:	none
; Use:	SaveMemStrat, SaveUMBLink
; Modf:	AX, BX
; Call:	INT 21/5801, INT 21/5803
;
RestoreStrategy	proc
	CODE_	MOV_BX	SaveMemStrat,<dw ?>
		DOSSetAlloc			; set DOS alloc strategy
	CODE_	MOV_BX	SaveUMBLink,<db ?,0>
		DOSSetUMBlink			; set UMB link state
		ret
RestoreStrategy	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
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
		testflag [options],OPT_noUMB
		jnz	@@allocasis		; jump if UMB prohibited
		mov	ax,ds
		cmp	ah,0A0h
		jae	@@allocasis		; jump if already loaded hi

;---------- check if UMB is a DOS type

		call	SaveStrategy
		DOSSetUMBlink UMB_LINK		; add UMB to MCB chain

		mov	bl,HI_BESTFIT		; OPTIMIZE: BL instead BX
		DOSSetAlloc			; try best strategy to
						;  allocate DOS UMBs
	if_ carry
		mov	bl,HILOW_BESTFIT	; OPTIMIZE: BL instead BX
		DOSSetAlloc			; try a worse one then
	end_

		pop	bx
		push	bx
		DOSAlloc			; allocate UMB (size in BX)
		pushf
		xchg	dx,ax			; OPTIMIZE: instead MOV DX,AX
		call	RestoreStrategy		; restore allocation strategy
		popf
	if_ nc
		cmp	dh,0A0h			; exit if allocated mem is
		jae	@@allocret		;  is above 640k (segment
		DOSFreeMem dx			;  0A000h) else free it
	end_

;---------- try a XMS manager to allocate UMB

		call	getXMSaddr
	if_ nc
		pop	dx
		push	dx
		mov	ah,10h			; XMS: Request UMB (size=DX)
		call	[XMSentry]		; ...AX=1 -> BX=seg, DX=size
		dec	ax
	andif_ zero
		pop	ax
		push	ax
		cmp	bx,ax
		mov	dx,bx
		jae	@@allocret
		mov	ah,11h			; XMS: Release UMB (seg=DX)
		call	[XMSentry]
	end_

;---------- use current memory segment

@@allocasis:	mov	dx,ds

@@allocret:	pop	bx
		ret
AllocUMB	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; In:	ES			(segment to free)
; Out:	none
; Use:	XMSentry
; Modf:	AH, DX
; Call:	INT 21/49, getXMSaddr
;
FreeMem		proc
		assume	es:nothing
		call	getXMSaddr
	if_ nc
		mov	dx,es
		mov	ah,11h			; XMS: Release UMB
		call_far XMSentry
	end_
		DOSFreeMem			; free allocated memory
		ret
FreeMem		endp

;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

end start
