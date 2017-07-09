;/*	GEMDOSIF.A86	4/18/84 - 08/14/85	Lee Jay Lorenzen	*/
;	for 2.3		9/23/87			mdf
;
;       Copyright 1999, Caldera Thin Clients, Inc.                      
;       This software is licenced under the GNU Public License.         
;       Please see LICENSE.TXT for further information.                 
;                                                                       
;                  Historical Copyright                                 
;/*
;*	-------------------------------------------------------------
;*	GEM Application Environment Services		  Version 2.3
;*	Serial No.  XXXX-0000-654321		  All Rights Reserved
;*	Copyright (C) 1987			Digital Research Inc.
;*	-------------------------------------------------------------
;*/
;
;
	.psect	data,class=DATA
;
GEMENTRY equ	0efh
;
	.globl	_CMP_TICK, _NUM_TICK

indos:		.byte	0
achar:		.byte	0
save_ss:	.word	0
save_sp:	.word	0
_CMP_TICK:	.word	0
ti_hicmp:	.word	0
_NUM_TICK: 	.word	0
ti_hinum: 	.word	0

	.psect	_TEXT,class=CODE
;
;
	.globl	_cpmcod
	.globl	___DOS		;VOID DOS(VOID);
	.globl	___EXEC		;WORD EXEC(VOID);
	.globl	_cli, _sti	;VOID cli(VOID); VOID sti(VOID);
	.globl	_takeerr, _giveerr	;VOID (VOID);
	.globl	_takecpm, _givecpm	;VOID (VOID);
	.globl	_retake		;VOID retake(VOID);
	.globl	_setdsss	;VOID setdsss(UDA *);
	.globl	_supret		;VOID supret(WORD);
	.globl	_far_call	;UWORD far_call(LONG, LONG);
	.globl	_far_bcha	;WORD far_bcha(VOID);
	.globl	_far_mcha	;WORD far_mcha(VOID);
	.globl	_drawrat	;WORD drawrat(WORD, WORD);
	.globl  _justretf	;VOID justretf(VOID);
	.globl	_dsptch		;VOID dsptch(VOID);
	.globl	_forkq		;VOID forkq(WORD, WORD, WORD);
	.globl	_m_forkq	;VOID m_forkq(WORD, WORD, WORD);
	.globl	_super		;not prototyped
	.globl	_nsuper		;not prototyped
	.globl	_tchange	;WORD tchange(WORD, WORD);
	.globl	_bchange	;WORD bchange(WORD, WORD);
	.globl	_mchange	;WORD mchange(WORD, WORD);
	.globl	_b_delay	;VOID b_delay(WORD);
	.globl	_b_click	;VOID b_click(WORD);
	.globl	_eralert	;WORD eralert(WORD, WORD);
	.globl	_ini_idle	;WORD ini_idle(VOID);
	.globl	_idle		;VOID idle(VOID);
;
;
	.psect	data,class=DATA

	.globl	_eintstk
	.globl	_CMP_TICK, _NUM_TICK
	.globl	_drwaddr
	.globl	_tikaddr
	.globl	_tiksav
	.globl	_xrat
	.globl	_yrat
	.globl	_rlr
	.globl	_gl_bdely
	.globl	_gl_play
	.globl	_DOS_AX
	.globl	_DOS_BX
	.globl	_DOS_CX
	.globl	_DOS_DX
	.globl	_DOS_DS
	.globl	_DOS_ES
	.globl	_DOS_SI
	.globl	_DOS_ERR
	.globl	_DOS_DI
	.globl	_gintstk
	.globl	_gl_idle
;
;
_rparmblk:
	.word	0,0
_rhdrbuf:
	.word	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
;
;
	.psect	_TEXT,class=CODE
;
;
inxif:	.byte	0
blank:	.byte	0
errsav:	.word	0	; TRO 9/18/84  Add DOS error trapping
		.word 	0
cpmsav:	.word	0	
		.word	0	
savax:	.word	0
savds:	.word	0
axtmp:	.word	0
sstmp:	.word	0
sptmp:	.word	0
;
;
;
;	for all dos calls except exec
;
;
___DOS:
	push	bp
	mov		bp,sp
	push	es
	push	si
	push	di
	mov		ax,_DOS_DI
	mov		di,ax
	mov		ax,_DOS_SI
	mov		si,ax
	mov		ax,_DOS_ES
	mov		es,ax
	mov		ax,_DOS_AX
	mov		bx,_DOS_BX
	mov		cx,_DOS_CX
	mov		dx,_DOS_DX
	mov		cs:savax,ax
	push	ds
	mov		ax,_DOS_DS
	mov		ds,ax
	mov		ax,cs:savax
	int		#21h
	mov		cs:savax,ax
	mov		ax,ds
	mov		cs:savds,ax
	pop		ds
	mov		ax,cs:savax
	jc		d_error
	mov		_DOS_ERR,#0,word
	jmp		d_exit
d_error:
	mov		_DOS_ERR,#1,word
d_exit:
	mov		_DOS_AX,ax
	mov		_DOS_BX,bx
	mov		_DOS_CX,cx
	mov		_DOS_DX,dx
	mov		ax,savds
	mov		_DOS_DS,ax
	mov		ax,es
	mov		_DOS_ES,ax
	mov		ax,si
	mov		_DOS_SI,ax
	mov		ax,di
	mov		_DOS_DI,ax
	pop		di
	pop		si
	pop		es
	pop		bp
	ret
;
;
;
___EXEC:
	push	bp
	mov		bp,sp
	push	si
	push	di

	push	es
	push	ds
					; update ss,sp in uda in case this
					;   is execing a crystal application
	mov		bx,_rlr			; bx = rlr
	mov		bx,4[bx]		; bx = uda address
	mov		ax,sp
	mov		2[bx],ax		; new saved sp = sp
	mov		ax,ss
	mov		4[bx],ax		; new saved ss = ss
	mov		ax, 28[bx]		; save away ss, sp for restoring later
	inc		ax			; increment exec level
	mov		28[bx], ax		; store back next exec level
	dec		ax
	shl		ax, #1
	shl		ax, #1
	add		bx, ax			; index past other restoring ss,sp's
	mov		ax,sp
	mov		30[bx],ax		; restore sp = sp
	mov		ax,ss
	mov		32[bx],ax		; restore ss = ss
	mov		ax,_DOS_ES		; establish calling registers
	mov		es,ax
	mov		ax,_DOS_AX
	mov		bx,_DOS_BX
	mov		cx,_DOS_CX
	mov		dx,_DOS_DX
	mov		cs:savax,ax
	mov		ax,_DOS_DS
	mov		ds,ax
	mov		ax,cs:savax
	int		#21h
	cli
	mov		cs:savax,ax
	mov		ax,#seg _rlr
	mov		ds, ax
	mov		bx,_rlr			; bx = rlr
	mov		bx,4[bx]		; bx = uda address
	mov		ax, 28[bx]		; ax = exec level
	dec		ax			; decrement exec level
	mov		28[bx], ax		; save it back
	shl		ax, #1			; index to proper restore ss,sp
	shl		ax, #1			; index to proper restore ss,sp
	add		bx, ax
	mov		ax, 30[bx]		; sp = restore sp
	mov		sp, ax
	mov		ax, 32[bx]		; ss = restore ss
	mov		ss, ax
	mov		ax, cs:savax
	sti
	pop		ds
	jc		de_error
	mov		_DOS_ERR,#0,word
	jmp	 	de_exit
de_error:
	mov		_DOS_ERR,#1,word
de_exit:
	mov		_DOS_AX,ax
	pop		es

	pop		di
	pop		si
	pop		bp
	ret
;
;
_cli:
	cli
	ret
;
;
_sti:
	sti
	ret
;
; DOS error trapping code. TRO 9/18/84
;
_takeerr:
	push	bp
	mov		al,#024h
	mov		bp,#errsav
	mov		dx,cs:#err_trap
	call	inttake
	pop		bp
	ret

_giveerr:
	push	bp
	mov		al,#024h
	mov		bp,#errsav
	call	intgive
	pop		bp
	ret

inttake:
	push	es
	mov		ah,#035h	; set fn # (int# passed in al)
	int 	#021h		; get current vector
	mov		cs:2[bp],es	; saving current values
	mov		cs:0[bp],bx
	pop		es

intset:
	push	ds
	mov		bx,cs		; new vector seg is current cseg
	mov		ds,bx	
	mov		ah,#025h	; new vector offset already in dx
	int		#021h
	pop		ds
	ret

intgive:
	push	ds
	mov		dx,cs:0[bp]	; set up old offset
	mov		ds,cs:2[bp]	; and segbase
	mov		ah,#025h	; int # already in al
	int		#021h
	pop		ds
	ret

; A DOS error will trap to here (err_trap) after takeerr is called.
; For a description of the register state, see PCDOS 2.1 Technical
; Ref. Manual, pp. 5-5 to 5-10

err_trap:
	push 	es	; save the DOS environment
	push	ds
	push	bp
	push	di
	push	si
	push	dx
	push	cx
	push	bx
	push	ax
	mov		bx,ss
	mov		cs:eintss,bx
	mov		bx,sp
	mov		cs:eintsp,bx
	mov		bx,#seg _eintstk
	mov		ss,bx
	mov		ds,bx
	mov		es,bx
	mov		sp,#_eintstk
	sti
	or		ah,ah		; device or disk?
	js		dev_trap	; trap device
	xor		ah,ah		; clear MSB
	push 	ax
	and		di,#0FFH	; isolate err#
	mov		al,cs:err_tbl[di] ; translate to alert #
	push	ax
	jmp		err_x

err_tbl:
	.byte	0,5,1,5,2,5,2,2,2,4,3,3,3 ; 13 entries

dev_trap:
	mov		ax,#0FFFFh	; drive := -1 for device
	push	ax
	mov		ax,#4
	push	ax

err_x:
	mov		bx,_rlr		; get the REAL Crystal 
	mov		bx,4[bx]	; sp & ss out of the way
	mov		ax,2[bx]
	mov		cs:envsp,ax
	mov		ax,4[bx]
	mov		cs:envss,ax
	pop		dx			; Error no.
	pop		ax			; Drive
	push	ax
	push	dx			; These get popped later 
	call	_eralert	; return Z (abort) or not-Z (retry)
	mov		bx,_rlr
	mov		bx,4[bx]	; and remember, Lee, when this finally
	mov		cx,cs:envsp	; blows up - I TOLD YOU SO!
	mov		2[bx],cx	; TRO - 9/24/84
	mov		cx,cs:envss
	mov		4[bx],cx
	cli
	or		ax,ax		; make sure z flag is set up
	pop		ax			; get back err #
	mov		bx,cs:eintss
	mov		ss,bx		; reload saved stack regs 
	mov		bx,	cs:eintsp
	mov		sp,bx
	jz		err_abort

err_retry:
	pop		ax		; restore DOS environment
	pop		bx
	pop		cx
	pop		dx
	pop		si
	pop		di
	pop		bp
	pop		ds
	pop		es
	mov		al,#1		; set retry flag for DOS
	iret

err_abort:
	add		ax,#64		; make into pseudo-DOS error #
	mov		cx,#14
poploop:
	pop		bx		; toss 13: 12 DOSregs, 1 userreg (ax)
	loop	poploop		; keep #14 (bx)
	pop		cx
	pop		dx
	pop		si
	pop		di
	mov		bp,sp
	or		ss:10[bp],#1,byte	; set stacked carry flag
	pop		bp
	pop		ds
	pop		es
	stc				; set error flag
	iret			; return to caller

;
;	use DOS 2.0's tick
;
tikcod:
	push 	ds
	push 	ax
	push 	bx
 	push 	cx
	push 	dx
	mov 	ax, #seg indos		; get this ds
	mov 	ds,ax
chk_time:	
	mov		ax,_CMP_TICK		; sub off 1 ticks worth
	mov		bx,ti_hicmp
	or 		ax,bx			; do we have time to tick off
	jz		tpollmb			; no time to do skip over
	mov		cx,_NUM_TICK		; add 1 to # of ticks we've waited
	mov		dx,ti_hinum
	add		cx,#1			; add 1 tick to NUM_TICK	
	adc		dx,#0
	mov		_NUM_TICK,cx
	mov		ti_hinum,dx
	sub		ax,#1			; sub off 1 ticks worth from CMP_TICK
	sbb		bx,#0
	mov		_CMP_TICK,ax
	mov		ti_hicmp,bx
	or		ax,bx
	jnz		tpollmb			; more to do so go on
updt_time:
	mov 	save_ss, ss
	mov 	save_sp, sp
	mov 	sp, #(_gintstk - 0x80)
	mov 	ax, ds
	mov 	ss, ax
	push 	si 
	push 	di
	push 	bp
	push 	es
	push	dx				; push high number of ticks
	mov		ax,cx			; Pass low number of ticks
	mov		dx,#_tchange	; Pass address of tchange
	call	_forkq			; set up forkq
chk_delay:
	cmp		_gl_bdely,#0,word
	je		offstk
	jmp		do_delay
tpollmb:
	cmp		_gl_bdely,#0,word
	je		all_done
	mov 	save_ss, ss
	mov 	save_sp, sp
	mov 	sp,#(_gintstk - 0x80)
	mov 	ax, ds
	mov 	ss, ax
	push 	si
	push 	di
	push 	bp
	push 	es
do_delay:
	mov		dx,#1
	call	_b_delay			; decrement button delay time
offstk:					; get off our stack
 	pop 	es
	pop 	bp
	pop 	di
	pop 	si
	mov 	sp, save_sp
	mov 	ss, save_ss
all_done:
	pop 	dx
	pop 	cx
	pop 	bx
	pop 	ax
nxt_int:				; chain to next interrupt
	pushf
	callf	[_tiksav]
	pop ds
	iret
;
;
;
;	interrupts should be off
;
_takecpm:
	push	bp
	mov		al,#GEMENTRY
	mov		bp,#cpmsav
	mov		dx,cs:#_cpmcod
	call	inttake
	pop		bp
	ret
;
;
;
_retake:
	push	bp
	mov		al,#GEMENTRY
	mov		dx,cs:#_cpmcod
	call 	intset
	mov		al,#024h
	mov		dx,cs:#err_trap
	call 	intset
	pop		bp
	ret
;
;	interrupts should be off
;
_givecpm:
	push	bp
	mov		al,#GEMENTRY
	mov		bp,#cpmsav
	call	intgive
	pop		bp
	ret
;
;
_cpmcod:
	jmp		gemchk
	.byte	'GEMAES20', 0			; id message

gemchk:
	cmp		cx,#200
	jz		gemcod
	cmp		cx,#201
	jz		gemcod
	push	bp
	mov		bp, cs:#invdi
	mov		cs:[bp],#1,word
	mov		bp,sp
	mov		bp,6[bp]
	push	bp
	callf	cs:[cpmsav]		; chain to VDI

	mov		bp, cs:#invdi
	mov		cs:[bp],#0,word
	pop		bp
	iret

invdi:
	.word	0
;
; gem filter entry
; for external gem calls
;
gemcod:
	sti					; turn interrupts on
	push	bx
	push	cx
	push	dx
	push	si
	push	di
	push	bp
	push	es
	push	ds
	mov		di,ds
	mov		ax,#seg _rlr
	mov		ds,ax
	mov		ax,bx			; save bx
	mov		bx,_rlr
	mov		bx,4[bx]		; uda address
	cli
	mov		[bx],#1,word		; in super
	mov		6[bx],sp
	mov		8[bx],ss
	mov		sp,2[bx]		; load super stack
	mov		ss,4[bx]	
	sti
	mov		bx,ax			; restore bx
; whew !
	push	es			; user es
	push	bx			; user bx
	cmp		cx,#200
	jne		newsuper
	call	_super			; super( es:bx)
newsuper:
	call	_nsuper
;
; return from gementry by calling supret
;
; stack is:
;	temp space (word)
;	bp (word)
;	retadd to gementry (above)
;	es:bx parm (long)
; a6->  olda6		(super's link instruction)
;	super automatic
;	result (long) from function (to go in d0, axbx,esbx,or ax)
;	p->err (word) from function (to go to d1, cx)
; sp-> super retadd
;
; In Pacific:
;
;      caller es
;      caller bx
;      return address for super()
; bp-> saved bp for super()
; sp-> return addrsss for supret()
;      dx = parameter value for supret()
; 
;
_supret: 
	mov	di,bp			; play that shell game
;
;di -> super() stack frame
;
	mov	bp,sp
	mov	ax,dx			; AX = parameter

	mov	sp,di			; SP -> saved bp for super(), retaddr,bx,es

	add	sp,#8			; skip retadd and parms
;
	mov	bx,_rlr
	mov	bx,04h[bx]		; uda
	cli
	mov	[bx],#0,word		; not in super
	mov	2[bx],sp
	mov	4[bx],ss
	mov	sp,6[bx]
	mov	ss,8[bx]
	sti
	pop	ds
	pop	es
	pop	bp
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	iret
;
;
;
;
	.signat	_setdsss,4152

_setdsss:
	push	bp
	mov		bp,sp
	mov		bx,dx				; DX = parameter 1
	mov		ax,ds
	mov		10[bx],ax
	mov		4[bx],ax
	mov		[bx],#1,word		; insuper = true
	pop		bp
	ret
;
;
;	routine to call far into a user supplied routine with
;	the parameters from an object.  The objects new state
;	should be returned for any remaining state changes that
;	need to occurr
;	
;	far_call(code, data)
;		LONG		code
;		LONG		data
;
;	data points at the following block:
;		PARMBLK
;		{
;			LONG	pb_tree;
;			WORD	pb_obj;
;			WORD	pb_prevstate;
;			WORD	pb_currstate;
;			WORD	pb_x, pb_y, pb_w, pb_h;
;			WORD	pb_xc, pb_yc, pb_wc, pb_hc;
;			LONG	pb_parm;
;		}
;
;
	.signat	_far_call,8250


_far_call:
	push	bp
	mov		bp,sp
	push	si
	push	di
	mov		ax,4[bp]
	mov		calloff,ax
	mov		ax,6[bp]
	mov		callseg,ax
	mov		bx,8[bp]
	mov		ax,10[bp]

	callf	[calloff]

	pop		di
	pop		si
	pop		bp
	ret		#8		;2 longs = 8 parameter bytes
;
;	called far with ax = button state
;	interrups should be off
;
_far_bcha:
	push	bp

	pushf
	cli
	push	ds
	push	es
	push	ax

	mov		dx,ax			; Parameter		
	mov		ax,ss
	mov		cs:gintss,ax
	mov		ax,sp
	mov		cs:gintsp,ax
	mov		ax,#seg _gintstk
	mov		ss,ax
	mov		ds,ax
	mov		es,ax
	mov		sp,#_gintstk
	cmp		_gl_play, #0,word
	jne		nobchg

	call	_b_click			; call the button click code
nobchg:
	mov		ax,cs:gintss
	mov		ss,ax
	mov		ax,cs:gintsp
	mov		sp,ax

	pop		ax
	pop		es
	pop		ds
	popf

	pop		bp
	retf
;
;	bx = xrat, cx = yrat
;	interrupts should be off

_far_mcha:
	push	bp
	pushf
	cli
	push	ds
	push	es
	push	bx
	push	cx
	mov		ax,ss
	mov		cs:gintss,ax
	mov		ax,sp
	mov		cs:gintsp,ax
	mov		ax,#seg _gintstk
	mov		ss,ax
	mov		ds,ax
	mov		es,ax
	mov		sp,#_gintstk
	cmp		_gl_play, #0,word
	jne		nomchg
	push	cx				;yrat
	mov		ax,bx			;xrat
	mov		dx,#_mchange	;function
	call	_m_forkq
nomchg:
	mov		ax,cs:gintss
	mov		ss,ax
	mov		ax,cs:gintsp
	mov		sp,ax

	pop		cx
	pop		bx
	pop		es
	pop		ds
	popf
	pop		bp
	retf
;
;	drawrat([dx] newx, [ax] newy)
;
	.globl	_gsx_graphic

_drawrat:
	push	ax		;newy
	mov		bx, #invdi
	mov		ax, cs:[bx]
	cmp		ax,#1
	jnz		drawok
	pop		ax
	ret
	
drawok:
	pop		ax		;DX = newx AX = newy
	push	bp
	mov		bp,sp
	mov		bx,dx	;newx
	mov		cx,ax	;newy
	push	si
	push	di
	callf	[_drwaddr]
	pop		di
	pop		si
	pop		bp
	ret
;
;
_justretf:
	retf
;
_ini_idle:
	push	bp
	push	si
	push	di
	mov		ax,#0x352f
	int		#0x21
	mov		ax,es
	or		ax,bx
	mov		_gl_idle,ax
	pop		di
	pop		si
	pop		bp
	ret
;
_idle:
	mov		ax,_gl_idle
	test	ax,ax
	jnz		1f
	ret

1:	push	bp
	push	si
	push	di
	mov		ax,#0x1680
	int		#0x2f
	pop		di
	pop		si
	pop		bp
	ret
;
		.psect	data,class=DATA

_gl_idle:
		.word	0
calloff:
		.word	_justretf
callseg:
		.word	seg _justretf
_drwaddr:
		.word	_justretf
drwseg:
		.word	seg _justretf
_tikaddr:
		.word	tikcod
tikseg:
		.word	seg tikcod
_tiksav:
		.word	0,0

		.blkb	768		;[JCE] boosted from 512 to avoid trouble
_eintstk:
		.word	0
;
		.psect	_TEXT,class=CODE
gintsp:	.word	0
gintss:	.word	0
eintsp:	.word	0
eintss:	.word	0
envsp:	.word	0
envss:	.word	0
;
;

