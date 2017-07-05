;#
;# Log of Changes:
;# ---------------
;#	HCA   9/23/87	PRO is the same driver as EHI, with the exception of
;#			alphatext attribs and devinfo file.
;#

;****************************************************************
;*			IBM Proprinter				*
;****************************************************************

; The following file defines device dependent global constants.
include externs.a86

DGROUP	GROUP	DATA
		dseg

		public	CONTRL, INTIN, PTSIN, INTOUT, PTSOUT

CONTRL		rw	CONTRL_SIZE
INTOUT		rw	INTOUT_SIZE
PTSOUT		rw	PTSOUT_SIZE
INTIN		rw	INTIN_SIZE
PTSIN		rw	0
		db	'GEMVDI 3.0 IBM Proprinter Driver 10/1/87'
		db	'Serial No. XXXX-0000-654321  All Rights Reserved'
		db	'Copyright (C) 1985-1987 Digital Research Inc.'
ptsin_end	rw	PTSIN_SIZE + 2 - (ptsin_end - PTSIN)/2


CGROUP	GROUP	CODE
		cseg

		public	adv_form
		public	alphaout
		public	clearmem
		public	concat
		public	dev_rect
		public	dini_dev
		public	enter_gr
		public	exit_gr
		public	init_dev
		public	scan_out
		public	s_orient
		public	set_page
		public	ini_page

		extrn	dinit_p:near
		extrn	init_p:near
		extrn	pout:near
		extrn	printer_out:near
		extrn	alpha_map:near

		public	ln_bytes
		public	ln_next
		public	sc_bytes
		public	sc_off

; Equates for ASCII characters used in this module:
CR		equ	0Dh		; ASCII carriage return
ESC		equ	1Bh		; ASCII escape
FORM_FEED	equ	0Ch		; ASCII form feed
LF		equ	0Ah		; ASCII line feed

; The alpha attribute escape character is a DC2:
ALPHA_ESCAPE	equ	12h

; Device dependent scan size variables:
; These all get set in set_page() (below) as per requested page size
; For Epson HiRes, bytes/"slice" = bytes/scan line * # wires to printer
ln_bytes	dw	0		; bytes per scan line
ln_next		dw	0		; bytes to next scan line
sc_bytes	dw	0		; bytes per slice
sc_off		dw	0


;************************************************************************
;*	ini_page()							*
;*		Initialize page information.				*
;************************************************************************
ini_page:
		ret

;************************************************************************
;*  set_page()								*
;*	Set variables to accomodate requested page size.  Called once	*
;*	only, from v_opnwk() in monout.c				*
;*	Set: vq_scan() outputs - a_page, g_page				*
;************************************************************************
set_page:
	      	mov	ax, dev_tab		; 
		inc	ax			; page width in pixels
		shr	ax, 1
		shr	ax, 1
		shr	ax, 1			; page width in bytes
		inc	ax
		and	ax, 0fffeh		; make word boundary
		mov	ln_bytes, ax		; bytes per line
		mov	dln_byte, ax		; bytes per line
		mov	ln_next, ax		; bytes to next line

		shl	ax, 1
		mov	sc_off, ax		; offset to next scan line

		mov	ax, ln_bytes		; restore value
		mov	cx, G_SL_HGT
		mul	cx			; graphics slice worth of
		mov	sc_bytes, ax		; 	bytes/line

; Number of slices that will fit on the page = chosen page size in y direction
; divided by height of a slice.
		mov	ax, dev_tab + 2	
		inc	ax		
		xor	dx, dx		
		mov	bx, G_SL_HGT	
		div	bx		
		mov	g_page, ax		; graphics slices per page

; Now do the same for alpha text slice height
		mov	ax, dev_tab + 2	
		inc	ax		
		xor	dx, dx		
		mov	bx, A_SL_HGT	
		div	bx		
		mov	a_page, ax		; alpha text slices per page
		ret

;************************************************************************
;*  status = dev_rect()							*
;*	Called ONCE ONLY, from v_updwk() in jmptbl.c			*
;*	Return FALSE.							*
;*  NOTE: in the hph, this routine "sets up the graphics window parms"	*
;*	EXPLORE.							*
;************************************************************************
dev_rect:
		xor	ax, ax
		ret

;************************************************************************
;*	CONCAT								*
;*		Convert the (x, y) pair passed in (BX, AX) into a	*
;*		physical address (DI) and bit offset into a byte (BX).	*
;************************************************************************
concat:
		push	ax
		push	dx

; Calculate the physical address and the bit offset.
		sub	ax, ys_min		; normalize to start of slice
		mov	dx, ln_bytes		; dx = bytes/line
		mul	dx			; ax = bytes for y position
		mov	di, bx
		shr	di, 1
		shr	di, 1
		shr	di, 1			; adjust x to byte offset
		add	di, ax			; di = byte offset to (x, y)
		and	bx, 07h			; bl = bit offset into byte

; Restore and leave.
		pop	dx
		pop	ax
		ret

;************************************************************************
;*  dini_dev()								*
;*  enter_gr() - done at beginning of each graphics line output.        *
;*  s_orient():								*
;************************************************************************
dini_dev:
enter_gr:
s_orient:
		ret

;************************************************************************
;*  init_dev()								*
;*	Called ONCE ONLY, from init_g() in monoprin.c, which is called	*
;*	from v_opnwk() in monout.c					*
;************************************************************************
init_dev:
		push	bp
		mov	ax, a_page	; alpha text lines per page
		mov	page_size, al
		mov	si, offset init_printer
		call	printer_out
		pop	bp
		ret

;**********************************************************************
;*  CLEARMEM:                                                         *
;*      Clear graphics memory.  For a printer, this means clearing    *
;*      the display list and rasterizing buffer.                      *
;**********************************************************************
Clearmem:
	Push	Es

	mov	ax, display_list_seg
	Mov	Es, Ax

	Xor	Di, Di			;Clear the rasterizing buffer
	Xor	Ax, Ax
	mov	cx, buffer_size
	shr	cx, 1

	Rep	Stosw

	Pop	Es			;Restore callers registers
	Ret

;************************************************************************
;*  ADV_FORM:                                                         	*
;*      Do a form feed on the printer.  Allowable print area = length	*
;*	  of paper, so you don't always WANT a form feed to go out.	*
;*	Called from v_clrwk() in monout.c, and c_escape() in monoprin.c	*
;*	  case 20: advance form, case 22: clear display list		*
;*	First parameter: not used by Epson (no multiple copy support).	*
;*	Second parameter: 0 = don't do form feed if vmu = 0		*
;*			  1 = do form feed unconditionally		*
;************************************************************************
adv_form:
		push	bp
		mov	bp, sp
		mov	bx, 4[bp]		; param is "do FF" flag
		cmp	bx, 1
		je	do_ff			; do form feed unconditionally
		cmp	vmu, 0
		je	dont_ff
do_ff:
		mov	dl, FORM_FEED
		call	pout
		mov	vmu, 0
dont_ff:
		pop 	bp
		ret

;**********************************************************************
;* EXIT_GR:                                                           *
;*     For the Epson printer in high res all we do is reset the line  *
;*     feed spacing.                                                  *
;**********************************************************************
exit_gr:
		mov	si, offset Exit_Graph
		call	printer_out
		ret

;************************************************************************
;*  ALPHAENV:								*
;*	Verify that the alphanumeric text environment is correct.  If	*
;*	not, output the commands to the printer to set the correct	*
;*	environment.							*
;************************************************************************
alphaenv:
		push	ax
		push	bx
		push	cx
		push	dx
		push	si
		push	bp

; Is the attribute environment correct?
		mov	dl, reqalpha
		mov	dh, curalpha
		cmp	dl, dh			; requested same as current?
		je	end_alphaenv

; The attribute environment is not correct.  Find out which attributes are
; incorrect and output the appropriate command to set that environment.
		mov	cx, 8			; cx = number of attributes
		xor	bx, bx			; bx = command index

alphaenv_loop:
		mov	bp, dx			; bp = saved attribute info
		and	dl, 1			; mask next requested attrib.
		and	dh, 1			; mask next current attribute
		cmp	dl, dh			; are they the same?
		je	next_alphaenv_loop

		push	bx			; save the command index
		add	bl, dl			; update index with begin/end
		shl	bl, 1			; bx = offset into word table
		mov	si, com_addr[bx]	; si -> command string
		cmp	byte ptr [si], 00h
		je	no_print		; attribute isn't supported 
		call	printer_out		; output command string
no_print:	pop	bx			; restore command index

next_alphaenv_loop:
		mov	dx, bp			; restore shifted attrib info
		shr	dl, 1			; next requested attribute
		shr	dh, 1			; next current attribute
		inc	bx
		inc	bx			; update command index
		loop	alphaenv_loop

; Save the new attribute set.
		mov	al, reqalpha
		mov	curalpha, al

end_alphaenv:
		pop	bp
		pop	si
		pop	dx
		pop	cx
		pop	bx
		pop	ax
		ret

;************************************************************************
;*  ALPHAOUT:								*
;*	Output the alpha text string in the INTIN array.  If any alpha	*
;*	attribute escape sequences are encountered, process them.	*
;************************************************************************
alphaout:
		push	bp

; Make sure that output occurs on a printer character line.
		call	vmu_sync

; Make sure the correct alpha text environment exists on the printer.
		call	alphaenv

; Get the length of the string and start the output loop.
		mov	cx, CONTRL + 6		; cx = number of characters
		mov	si, offset INTIN
		mov	bp, A_SL_HGT
alphaout_loop:
		lodsw				; get the next character
		cmp	al, ALPHA_ESCAPE
		jne	not_escape
		cmp	cx, 1			; any more characters?
		je	not_escape
		mov	bl, byte ptr [si]	; check the next character

		cmp	bl, '9'			; have I got a number?
		jle	number			; convert to decimal
		cmp	bl, 'A'			; have I got a letter?
		jl	bad_escape		; not a number, not a letter
		sub	bl, 7			; sub 55 to convert letter
number:		sub	bl, 48			; sub 48 to convert number

		cmp	bl, 0			; should now have a decimal
		jl	bad_escape		; number between 0 and 35
		cmp	bl, 35			
		jg	bad_escape
		
		cmp	bl, 16			; ascii G - ascii V are
		jl	got_escape		; reserved
		cmp	bl, 32
		jge	got_escape

bad_escape:
		dec	cx
		inc	si
		inc	si
		jmp	next_alphaout_loop

; An alpha attribute escape sequence has been found.  Process it.
got_escape:
		dec	cx
		inc	si
		inc	si
		xor	bh, bh
		cmp	bl, 32			; check for style setting
		jl	attrib			; (style = W - Z, or 32 - 35)
		sub	bl, 32			; calculate style table index
		shl	bl, 1

		push	si
		mov	si, styl_addr[bx]	; bx = offset into word table
		cmp	byte ptr [si], 00h	; check for unsupported style
		je	no_style
		call	printer_out
no_style:	pop 	si
		jmps	next_alphaout_loop

attrib:		mov	al, alpha_clear[bx]	; al = bit clearing mask
		and	reqalpha, al		; clear the attribute bit
		mov	al, alpha_set[bx]	; al = bit setting mask
		or	reqalpha, al		; set the bit appropriately
		call	alphaenv
		jmps	next_alphaout_loop

; Not a special character.  Just output it.  Note that a formfeed does not
; have to be replaced with anything special for the Epson printer.
not_escape:
		cmp	al, 127			; International char?
		jna	not_intl		; If so
		push	si
		push	ds			; save some regs
		push	cx
		push	ax
		call	alpha_map		; get string in local ds
		mov	si, ax			; offset in si
		pop	ax
		pop	cx
		pop	ds
		cmp	byte ptr [si], 0	; any to send?
		je	no_send			; if so then
		call	printer_out		; print them
no_send:
		pop	si
		jmps	next_alphaout_loop	; and on to next
not_intl:
		mov	dl, al
		call	pout			; output the character

; Check for a line feed or formfeed.  Either of these characters will cause
; an advance, in which case the vmu must be updated.
		cmp	al, LF
		jne	vmu_check_ff
		add	vmu, bp			; bump by one alpha line
		call	vmu_reset
		jmps	next_alphaout_loop
vmu_check_ff:
		cmp	al, FORM_FEED
		jne	next_alphaout_loop
		mov	vmu, 0

next_alphaout_loop:
		dec	cx			
		je	end_alphaout		
		jmp	alphaout_loop		

end_alphaout:
		pop	bp
		ret

;************************************************************************
;*  VMU_RESET:		(Added by Heather Anderson 9/16/87)		*
;*	Internal subroutine to reset vmu to top of new page if it's 	*
;*	beyond end of last page.  I.E. subtract (yres + 1) from it if 	*
;*	it's greater than or equal to (yres + 1).  Called each time the	*
;*	vmu is increased.						*
;************************************************************************
vmu_reset:
		push	dx
		mov	dx, vmu
		cmp	dx, dev_tab + 2		; >= yres+1, i.e. > yres
		jle	no_reset
		sub	dx, dev_tab + 2	; vmu >= (yres+1), subtract (yres+1)
		sub	dx, 1
		mov	vmu, dx
no_reset:  
		pop	dx
		ret

;************************************************************************
;*  VMU_SYNC:								*
;*	Adjust the position of the print head so that alpha text output	*
;*	will occur on a text line.  ("vmu" = vertical motion units)	*
;************************************************************************
vmu_sync:
; Is the current scan line the beginning of a character line?  If not,
; send out sufficient spacing to make it so.
		mov	ax, vmu
		xor	dx, dx
		mov	bx, A_SL_HGT		; bx = text slice height
		div	bx
		cmp	dx, 0			; check for remainder
		je	end_vmu_sync
		sub	bx, dx
		add	vmu, bx			; adjust vmu to next line
		call	vmu_reset

; The bx register now contains the number of pixels to be advanced.  However,
; the Epson command uses 1/216 inch increments.  For high resolution, the
; number of pixels needs to be multiplied by 1.5.  For low resolution, the
; number of pixels needs to be multiplied by 3.
		mov	al, bl
		shr	al, 1
		add	al, bl			; al = 1.5 * bl
if (not hires)
		shl	al, 1			; al = 3 * bl
endif
		mov	sized_lf + 3, al
		mov	si, offset sized_lf
		call	printer_out

end_vmu_sync:
		ret

;************************************************************************
;*  finish_page()							*
;*	yres has to be an even multiple of G_SL_HGT, so actual length 	*
;*	of paper may be many pixels longer.  BITMAP printers need to	*
;*	bump the physical page up by the difference to leave printer	*
;*	at the start of a new piece of paper.  We check whether or not	*
;*	it's time to do this after every GRAPHICS bump of the vmu.	*
;************************************************************************
finish_page:
		push	dx

; I've JUST bumped the vmu by G_SL_HGT.  See if it's beyond the end of the
;  graphics print area.
		mov	dx, vmu
		cmp	dx, dev_tab + 2
		jle	fp_end			; NO - not time to finish_page
		
; Calculate the number of pixels to be advanced.
		mov	bx, req_y	; true length of paper in pixels
		sub	bx, dev_tab + 2	; yres = length of graphics print area
		jz	fp_end

; The bx register now contains the number of pixels to be advanced.  However,
; the Epson command uses 1/216 inch increments.  For high resolution, the
; number of pixels needs to be multiplied by 1.5.  For low resolution, the
; number of pixels needs to be multiplied by 3.
		mov	al, bl
		shr	al, 1
		add	al, bl			; al = 1.5 * bl
if (not hires)
		shl	al, 1			; al = 3 * bl
endif
		mov	sized_lf + 3, al
		mov	si, offset sized_lf
		call	printer_out

fp_end:
		pop	dx
		ret

;**********************************************************************
;*  Scan_Out                                                          *
;*      SCAN and discard any trailing blanks in an output buffer.  If *
;*      data must be sent to the printer, convert it to printer       *
;*      dependent format and transmit it.  Then advance the page.     *
;*                                                                    *
;*      The Epson in high resolution is an interlaced printer.  Eight *
;*      even scan lines are sent first, the page is advanced one scan *
;*      line, and eight odd scan lines are printed.  The page is then *
;*      advanced to the first even scan line of the next 16 scan line *
;*      slice.  This is repeated for each 16 scan line slice in the   *
;*      buffer.                                                       *
;*                                                                    *
;*      MSB = Top wire, 8 dots per byte, Double Density,              *
;*      Interlaced bit map output.                                    *
;**********************************************************************
Scan_Out:
	Push	Es		;Save the segment register we trash

	mov	ax, yw_max
	cmp	ys_max, ax	; does this slice extend beyond the window?
	jg	calc_cnt	; yes:  calculate the number of slices
	mov	cx, slice_cnt	; no:  slice count is known
	jmps	scan_parms

calc_cnt:
	sub	ax, ys_min
	mov	cx, G_SL_HGT
	add	ax, cx		; for rounding (jg - v1.1b)
	xor	dx, dx
	div	cx
	mov	cx, ax		; cx = number of slices left

scan_parms:
	Xor	Bx, Bx		; Bx gets offset to the buffer.  The buffer is
				; always on a paragraph boundry

	Mov	Ax, Graph_Plane		; Es gets segment address of buffer
	Mov	Es, Ax

Scan_Next_Slice:
	Push	Cx			;Save Outer Loop Count

;Backscan the even numberd scan lines

	Mov     Cx, ln_bytes		;Cx contains repeat count
	Mov	Di, Bx			;Es:Di point to last byte
	Add	Di, Cx
	Dec	Di

	Call	Back_Scan

	Jcxz	Do_Odd_Scan_Line	;If blank skip output

	Mov	Di, Bx			;Es:Di point to first byte
	Call	Byte_Out

Do_Odd_Scan_Line:

if hires

	mov	si, (Offset Micro_LF)	;Advance page 1 scan line
	call	Printer_Out

	mov	Cx, ln_bytes		;Count of bytes in line to Cx
	mov	Di, Bx			;Make Es:Di point to first odd
	add	Di, Cx			;scan line
	push	Di			;Save it for output phase

	add	Di, Cx			;Adjust Di to point to last byte of
	dec	Di			;Scan line

	call	Back_Scan

	pop	Di			;Di points to first byte of scan line
	jcxz	Setup_Next_Scan_Line	;If blank skip output

	call	Byte_Out

endif

Setup_Next_Scan_Line:
	mov	Si, Offset Graph_LF	; advance the paper to start of next
	call	Printer_out		;    16 scan lines

	add	bx, sc_bytes		; advance address 16 scan lines
	mov	ax, G_SL_HGT
	add	vmu, ax			; bump vmu to next graphics line
	call	finish_page
	call	vmu_reset

	pop	cx			; restore loop count
	loop	scan_next_slice

end_scan_out:
	pop	es			; restore callers registers
	ret


;**********************************************************************
;*  Back_Scan                                                         *
;*       This routine checks for any trailing blanks in the data line.*
;*       Trailing whitespace is discarted before the line is sent to  *
;*       the printer.                                                 *
;*                                                                    *
;*  Input:                                                            *
;*       Cx = Number of bytes per line                                *
;*       Di = Address of the last byte of the first scan line         *
;*                                                                    *
;*  Output:                                                           *
;*       Cx = Number of non-blank bytes per line                      *
;*                                                                    *
;*  Registers:                                                        *
;*       Bx preserved, all others trashed                             *
;**********************************************************************
Back_Scan:
	Mov	Dx, sc_off	;Make a register for speed

Check_Next_Byte:
	Push	Cx			;Save the byte count
	Push	Di			;Save the byte address

	Mov	Cx, 8			;Number of wires

Next_Byte:
	Cmp	Es:Byte Ptr [Di], 0
	Jnz	Found_Non_Blank
	Add	Di, Dx			; Byte in next odd or even pixel line
	Loop	Next_Byte

	Pop	Di			;Get back byte address and decrement
	Dec	Di

	Pop	Cx			;Get back loop counter
	Loop	Check_Next_Byte

	Ret

Found_Non_Blank:
	Pop	Di			;Clean up the stack and return
	Pop	Cx			;Cx is count of non-blank bytes
	Ret


;**********************************************************************
;*  Byte_Out                                                          *
;*       Scan and output bytes from the bitmap to the printer.        *
;*                                                                    *
;*       Graphics byte MSB = Top Wire, LSB = Bottom Wire              *
;*                                                                    *
;*  Input:                                                            *
;*       Si = Address of first byte of first scan line to output      *
;*       Cx = number of bytes to output                               *
;*                                                                    *
;*  Registers:                                                        *
;*       Bx preserved                                                 *
;**********************************************************************
Byte_Out:
	Push	Bx		;Save Bx

; We need to calculate the number of graphics bytes we will send to the
; printer.  For the Epson, this is 8 * bytes to ship.

	Mov	Ax, Cx
	Shl	Ax, 1
	Shl	Ax, 1
	Shl	Ax, 1

	Mov	N1, Al			; put the printer into graphics mode
	Mov	N2, Ah
	Mov	Si, (Offset Graph_mode)
	Call	Printer_out

;Main output loop

Output_Next_Byte:
	Push	Cx		;Save Byte count
	Push	Di		;Save starting address

	Mov	Cx, 8		;Always extract a whole bytes worth
	
	Mov	Bx, 8001h	;Bl = Initial shift count
				;Bh = Start Bitmask

Repeat:
	Call	Mask_Out	; Get the desired bits from bitmap
				; Returns the graphics byte in <DL>

	Call	Pout		; Output byte in <Dl> to printer

	Ror	Bh, 1		;Update the bitmask to the next bit pattern
	Inc	Bl		;Update Shift counter 
	Loop	Repeat

	Pop	Di
	Pop	Cx
	Inc	Di		; Next Bitmap Byte address
	Loop	Output_Next_Byte

	Pop	Bx		;Restore Bx

	Ret


;**********************************************************************
;*  Mask_Out                                                          *
;*       Mask out data bits from bitmap                               *
;*                                                                    *
;*  Input:                                                            *
;*       Di = NBYT, start address                                     *
;*       Bh = NBIT, mask                                              *
;*       Cx = PIXL, loop count                                        *
;*       Bl = Rotate Count                                            *
;*                                                                    *
;*  Output:                                                           *
;*       Dl  = 8 bits of bitmap information                           *
;**********************************************************************
Mask_Out:
	Push	Cx                      ;Save the outer loop count
	Push	Di			;Save the starting address

	Mov	Cx, 8			;Count of bits to extract
	Xor	Dl, Dl			;Clear work register

Again:
	Rol	Dl, 1			;Make room for next bit
	Mov	Al, Es:[Di]		;Get next byte from BITMAP
	And	Al, Bh			;Leave only the desired bit
	Or	Dl, Al			;Combine with temp result

	Add	Di, sc_off		;Di Points to dots in the next 
					;even or odd pixel line

	Loop	Again

;  The resulting bit pattern still needs to be rotated into correct
;  printer wire position : MSB = Top, LSB = Bottom

	Mov	Cl, Bl		;Rotate bits until correct position
	Rol	Dl, Cl

	Pop	Di		;Restore starting address
	Pop	Cx		;Restore outer loop count
	Ret			;Return with result in Dl


;--------------------------------------------------------------------------
include	devind.a86
include devdata.a86

	dseg

	public	map_col, term_ch, dln_byte

	extrn	yw_max:word
	extrn	buffer_size:word
	extrn	display_list_seg:word
	extrn	req_y:word

;********************************************************
;* Copy of code segment variable for use in image.c	*
;********************************************************
dln_byte	dw	0

;****************************************************************
;*  Turn skip-over-perforation off, set form length in lines	*
;*	(66 can be default - 6 lines/inch, 11 inches)		*
;****************************************************************
init_printer	db	5, ESC, 'O', ESC, 'C'
page_size	db	66

;**********************************************************************
;*  Graphic Line Feed, ADVANCE PAPER 23/216 inch after 2nd pass       *
;**********************************************************************
if hires
Graph_LF	Db	5, CR, ESC, '3', 23, LF
else
Graph_LF	Db	5, CR, ESC, '3', 24, LF
endif

;**********************************************************************
;*  Micro Line Feed, ADVANCE PAPER 1/216 inch before 2nd pass         *
;**********************************************************************
if hires
Micro_LF	Db	5, CR, ESC, '3', 01, LF
endif

;**********************************************************************
;  Enter High Resolution 120 dpi graphics mode ESC, L, n1, n2         *
;**********************************************************************
if hires
Graph_Mode	Db	04, ESC, 'L'
else
Graph_Mode	Db	04, ESC, 'K'
endif
N1		Rs	01
N2		Rs	01

;**********************************************************************
;  Exit Graphics mode is reset line spacing                           *
;**********************************************************************
Exit_Graph	Db	02, ESC, '2'

;*************************************
;*
;*************************************
Term_Ch		Dw	0
Map_Col		Dw	0		;Black
		Dw	1		;White

;************************
;  Alpha text locals.	*
;************************
; alpha text attribute bit clearing/setting mask table.
alpha_clear	db	0ffh, 0feh
		db	0ffh, 0fdh
		db	0ffh, 0fbh
		db	0ffh, 0f7h
		db	0ffh, 0efh
		db	0ffh, 0dfh
		db	0ffh, 0bfh
		db	0ffh, 07fh
alpha_set	db	01h, 0
		db	02h, 0
		db	04h, 0
		db	08h, 0
		db	10h, 0
		db	20h, 0
		db	40h, 0
		db	80h, 0

; Pointer table for printer commands.
com_addr	dw	end_bold
		dw	begin_bold
		dw	end_italic
		dw	begin_italic
		dw	end_under
		dw	begin_under
		dw	end_super
		dw	begin_super
		dw	end_sub
		dw	begin_sub
		dw	end_lq
		dw	begin_lq
		dw	end_expand
		dw	begin_expand
		dw	end_light
		dw	begin_light

styl_addr	dw	set_pica
		dw	set_elite
		dw	set_cmpres
		dw	set_propor

; Printer attribute commands (length, command string).
end_bold	db	2, ESC, 'F'
begin_bold	db	2, ESC, 'E'
end_italic	db	0
begin_italic	db	0
end_under	db	3, ESC, '-', 0
begin_under	db	3, ESC, '-', 1
end_super	db	2, ESC, 'T'
begin_super	db	3, ESC, 'S', 0
end_sub		db	2, ESC, 'T'
begin_sub	db	3, ESC, 'S', 1
end_lq		db	2, ESC, 'H'
begin_lq	db	2, ESC, 'G'
end_expand	db	3, ESC, 'W', 0
begin_expand	db	3, ESC, 'W', 1
end_light	db	0
begin_light	db	0

set_pica	db	1, 18			
set_elite	db	2, ESC, ':'		
set_cmpres	db	4, ESC, 'F', 18, 15		; lq off, elite off
set_propor	db	3, ESC, 'P', 1

; Printer spacing command.
sized_lf	db	3, ESC, 'J', '?'

		end
