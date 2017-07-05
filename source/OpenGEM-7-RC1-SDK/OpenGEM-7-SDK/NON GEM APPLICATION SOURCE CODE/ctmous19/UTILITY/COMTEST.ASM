; COMTEST - Find COM port types and associated IRQ line
; Copyright (c) 2000-2002 Arkady Belousov <ark@mos.ru>
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
;
; History:
;
; 2.5 -	Added external assembler library
;	Added detection of mouse type
;
; 2.0 - New IRQ detection agorithm
;
; 1.0 - First public release
;

%pagesize 255
%noincl
;%macs
%nosyms
;%depth 0
%linum 0
%pcnt 0
;%bin 0

WARN
LOCALS

.model use16 tiny

dataref equ <offset @data>	; offset relative data group

include asm.mac
include hll.mac
include macro.mac
include BIOS/area0.def
include convert/digit.mac
include convert/count2x.mac
include DOS/io.mac
include DOS/mem.mac
include hard/PIC8259A.def
include hard/UART.def

nl		equ <13,10>
eos		equ <'$'>


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€ DATA SEGMENTS €€€€€€€€€€€€€€€€€€€€€€€€€€€€€

.const

S_header	db 'COMTEST v2.5 (020501) - Utility to detect serial ports',nl
		db 'Copyright (c) 2000-2002 by Arkady V.Belousov, licensed under GPL2',nl
		db nl
		db 'COM# Addr IRQ# Type                      Attached',nl
		db '---- ---- ---- ------------------------- ---------------------------'
CRLF		db nl,eos

.data

S_port		db '  '
S_COMno		db   '   '
S_IOaddr	db	'      '
S_IRQno		db	      '    ',eos

S_note		db eos,10,'* multiple IRQ detected',nl,eos

.const

S_noUART	db		'not found',eos
S_8250		db		'8250 (no FIFO)            ',eos
S_8250A		db		'8250A/16450 (no FIFO)     ',eos
S_16550noSCR	db		'16550 (buggy FIFO/no SCR) ',eos
S_16550		db		'16550 (buggy FIFO)        ',eos
S_16550AnoSCR	db		'16550A (with FIFO/no SCR) ',eos
S_16550A	db		'16550A (with FIFO)        ',eos

S_MS		db 'Microsoft mode mouse',eos
S_WM		db 'Mouse with wheel',eos
S_LT		db 'Logitech 3-button mode mouse',eos

UARTtype	dw dataref:S_8250
		dw dataref:S_8250A
		dw dataref:S_16550noSCR
		dw dataref:S_16550
		dw dataref:S_16550AnoSCR
		dw dataref:S_16550A

micetype	dw dataref:S_MS
		dw dataref:S_WM
		dw dataref:S_LT


;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€ CODE SEGMENT €€€€€€€€€€€€€€€€€€€€€€€€€€€€€

.code
.startup
		cld
		mov	ax,1Fh			; disable mouse
		call	mousedrv

		DOSWriteS ,,@data:S_header
		mov	bx,'1'
	loop_
		push	bx

;---------- get IO address for COM port

		mov	[S_COMno],bl
		MOVSEG	es,0,ax,BIOS
		shl	bx,1
		mov	si,COM_base[bx-'1'-'1']
;----------
		call	processbase
		pop	bx
		inc	bx
		cmp	bx,'4'
	until_ above

		DOSWriteS ,,@data:S_note	; final note

;---------- reset mouse and exit through RET

		xor	ax,ax			; reset mouse
		;j	mousedrv

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

mousedrv	proc
		push	ax bx es
		DOSGetIntr 33h
		mov	ax,es
		test	ax,ax
		pop	es bx ax
	if_ nz
		int	33h
	end_
		ret
mousedrv	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
; In:	SI			(I/O address)
; Modf:	AX, CX, DX, BX, DI, ES
; Call:	detectUART, detectmouse
;
processbase	proc

;---------- convert IO address into string

		mov	di,dataref:S_IOaddr	; string for 4 digits
		MOVSEG	es,ds,,@data
		_word_hex si

;---------- detect UART type and IRQ line

		call	detectUART
		mov	ax,'  '
		mov	byte ptr S_IRQno[2],al
		mov	di,dataref:S_noUART
	if_ nc
		shl	bx,1
		mov	di,UARTtype[bx]
		call	detectIRQ
		mov	ax,203Fh		; =' ?'
		test	bx,bx
	andif_ nz				; if IRQ detected
		movsub	ax,bx,1
		and	ax,bx
	 if_ nz
		mov	byte ptr [S_note],13	; turn on final note if
		mov	byte ptr S_IRQno[2],'*'	;  more than one IRQ detected
		xor	bx,ax			; remain lowest nonzero bit
	 end_
		xor	cx,cx
@@mask2num:	shr	bx,1
		 loopnz	@@mask2num

		movsub	ax,'10'-11,cx		; ='1x' (CX=-IRQ-1)
		cmp	al,'0'
	 if_ below
		add	ax,(' '-'1') shl 8 + 10	; =' x'
	 end_
	end_ if

;---------- print UART info

		xchg	al,ah
		mov	word ptr S_IRQno[0],ax
		DOSWriteS ,,@data:S_port
		DOSWriteS ,,di

;---------- check attached devices

		cmp	di,dataref:S_noUART
	if_ ne
		call	detectmouse
	andif_ nc
		shl	bx,1
		DOSWriteS ,,micetype[bx]
	end_
		DOSWriteS ,,@data:CRLF
		ret
processbase	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Detect UART presence and type
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	SI			(I/O address)
; Out:	Carry flag		(no UART detected)
;	BX			(UART type: 0=8250, 1=8250A/16450,
;				 2=16550/no SCR, 3=16550,
;				 4=16550A/no SCR, 5=16550A)
; Use:	none
; Modf:	AX, DX
; Call:	none
;
detectUART	proc
		cli
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

		movidx	dx,LCR_index,si,LSR_index ; {3FBh} LCR (line ctrl reg)
		 in	ax,dx			; {3FCh} MCR (modem ctrl reg)
		 xchg	bx,ax			; OPTIMIZE: instead MOV BX,AX
		out_	dx,%LCR<1,0,-1,-1,3>	; {3FBh} LCR: DLAB on, 8S2
		 inb	ah,dx
		out_	dx,%LCR<0,0,0,0,2>	; {3FBh} LCR: DLAB off, 7N1
		 in	al,dx
		cmp	ax,(LCR<1,0,-1,-1,3> shl 8)+LCR<0,0,0,0,2>

	if_ eq					; equal if LCR conforms

;---------- check IER for reserved bits and clear UART interrupts

		movidx	dx,IER_index,si,LCR_index
		 in	al,dx			; {3F9h} IER (int enable reg)
		and	al,mask IER_reserved	; reserved bits should be clear
		 jz	@@loopback

		movidx	dx,LCR_index,si,IER_index
	end_ if

		xchg	ax,bx			; OPTIMIZE: instead MOV AL,BL
		 out	dx,al			; {3FBh} LCR: restore contents
@@noUART:	sti
		stc
		ret

;---------- check loopback mode

@@loopback:	;mov	al,%IER<>
		 out	dx,al			; {3F9h} IER: interrupts off
		movidx	dx,MCR_index,si,IER_index
		 out_	dx,%MCR<,,1>		; {3FCh} MCR: enable loopback
		movidx	dx,MSR_index,si,MCR_index
		 inb	ah,dx			; {3FEh} MSR (modem stat reg)
		movidx	dx,MCR_index,si,MSR_index ; MSR.4-7=MCR.1,0,2,3
		 out_ 	dx,%MCR<,,1,1,1,1,1>	; {3FCh} MCR: enable loopback
		movidx	dx,MSR_index,si,MCR_index
		 in	al,dx			; {3FEh} MSR (modem stat reg)
		movidx	dx,LCR_index,si,MSR_index
		not	al
		testflag ax,(MSR<1,1,1,1> shl 8)+MSR<1,1,1,1>
		xchg	ax,bx			; OPTIMIZE: instead MOV AX,BX
		 out	dx,ax			; {3FBh} LCR: restore contents
		jnz	@@noUART		; {3FCh} MCR: restore contents

;---------- check if SCRatch register present

		movidx	dx,SCR_index,si,LCR_index
		 in	al,dx			; {3FFh} SCR (scratch reg)
		 xchg	bx,ax			; OPTIMIZE: instead MOV BL,AL
		out_	dx,055h			; {3FFh} SCR (scratch reg)
		 inb	ah,dx			; 1: check if present
		out_	dx,0AAh			; {3FFh} SCR (scratch reg)
		 in	al,dx			; 2: check if present
		sub	ax,055AAh
		 neg	ax			; nonzero makes carry flag
		 sbb	ax,ax			; UART=8250 (no SCR)
		 inc	ax			;  or 16450 (with SCR)
		xchg	ax,bx			; OPTIMIZE: instead MOV AL,BL
		 out	dx,al			; {3FFh} SCR: restore contents

;---------- check FIFO

		movidx	dx,FCR_index,si,SCR_index
		 out_	dx,%FCR<-1,,,1,1,1>	; {3FAh} FCR: enable FIFO
		movidx	dx,IIR_index,si,FCR_index
		 in	al,dx			; {3FAh} IIR (intr id reg)
		testflag al,IIR{IIR_FIFO=10b}
	if_ nz
		movadd	bx,,2			; UART=16550
		testflag al,IIR{IIR_FIFO=01b}
	andif_ nz
		movadd	bx,,2			; UART=16550A
	end_
		movidx	dx,FCR_index,si,IIR_index
		out_	dx,%FCR{FCR_enable=0}	; {3FAh} FCR: disable FIFO
		sti
		;clc
		ret
detectUART	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Detect IRQ assigned to UART
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	SI			(I/O address)
; Out:	BX			(mask of detected IRQs)
; Use:	none
; Modf:	AX, CX, DX
; Call:	none
;
detectIRQ	proc
		cli
		mov	al,OCW3<,,,OCW3_IRR>
		 out	PIC2_OCW3,al		; {0A0h} select IRR read mode
		 out	PIC1_OCW3,al		; {20h} select IRR read mode

;---------- save current LCR/MCR and reset UART

		movidx	dx,LCR_index,si		; {3FBh} LCR (line ctrl reg)
		 in	ax,dx			; {3FCh} MCR (modem ctrl reg)
		 push	ax			; keep old LCR and MCR values
		 out_	dx,%LCR<0,,,,3>,%MCR<,,0,1,1,0,0>
						; {3FBh} LCR: DLAB off
						; {3FCh} MCR: DTR/RTS/loop off, OUTx on
		movidx	dx,FCR_index,si,LCR_index
		 out_	dx,%FCR<>		; {3FAh} FCR: disable FIFO

;---------- test THRE interrupt generation

		movidx	dx,IER_index,si,FCR_index
		mov	cx,3
@@detIRQloop:	push	cx
		out_	dx,%IER{IER_THRE=1}	; {3F9h} IER: enable THRE intr
		mov	bx,1			; mask of detected IRQ
						;  (except timer IRQ0)
		mov	ch,1			; OPTIMIZE: instead MOV CX,1xxh
@@waitIRQon:	inb	ah,PIC2_IRR		; {0A0h} get IRR
		in	al,PIC1_IRR		; {20h} get IRR
		or	ax,bx
		xor	ax,bx			; detect raised IRQ
		loopz	@@waitIRQon
		xchg	bx,ax			; OPTIMIZE: instead MOV BX,AX

		out_	dx,%IER<>		; {3F9h} IER: interrupts off
						;!!! under W4WG first THRE
						;  sometime ignored (and CX=0)
		inc	cx			; loop remained iterations
@@waitIRQoff:	inb	ah,PIC2_IRR		; {0A0h} get IRR
		in	al,PIC1_IRR		; {20h} get IRR
		xor	ax,bx
		and	ax,bx			; detect dropped IRQ
		loopz	@@waitIRQoff

		pop	cx
		loopz	@@detIRQloop

;---------- exclude IRQ2 if IRQs from slave PIC present

		test	ah,ah
	if_ nz					; if IRQ8-15 detected then
		 maskflag ax,not (1 SHL 2)	; exclude IRQ2 (PIC2 attach point)
	end_
		xchg	bx,ax			; OPTIMIZE: instead MOV BX,AX

;---------- restore LCR and MCR state

		movidx	dx,LCR_index,si,IER_index
		 pop	ax			; {3FBh} LCR: restore contents
		 out	dx,ax			; {3FCh} MCR: restore contents
		sti
		ret
detectIRQ	endp

;ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ
;			Detect mouse type if present
;‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
;
; In:	SI			(I/O address)
; Out:	Carry flag		(no mouse found)
;	BX			(mouse type: 0=MS,1=MS+wheel,2=Logitech)
; Use:	0:46Ch
; Modf:	AX, CX, DX, ES
; Call:	none
;
detectmouse	proc

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
		 out_	dx,%LCR<0,,0,0,2>	; {3FBh} LCR: DLAB off, 7N1
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

		mov	bx,0100h		; bl=mouse type, bh=no `M'
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
		mov	bl,1			; ...MS mouse+wheel found
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
		ret
detectmouse	endp

;€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€

end
