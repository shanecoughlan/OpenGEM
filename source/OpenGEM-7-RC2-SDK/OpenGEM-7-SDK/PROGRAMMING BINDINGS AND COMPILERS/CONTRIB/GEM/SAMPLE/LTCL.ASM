;
;/*	Tiny CL 							*/
;/*		startup module for large model GEM binding     		*/
;/*		modified 9/17/85  		C. Shaw			*/

;/*             Support for Lattice v3.0                                */  
;/*             added 4/23/86                   Mitch Smith             */
;/*		Modified for large model to put data, udata, stack 	*/
;/*		at the right position 09/04/87	Mei Chung		*/

	INCLUDE LARGE.MAC


BASE	SEGMENT WORD PUBLIC 'PROG'
BASE	ENDS

DGROUP	GROUP	DATA
SGROUP	GROUP	STACK
	
DATA	SEGMENT PARA PUBLIC 'DATA'
	PUBLIC	_PSP
_PSP	dw	0
	dw	0
	PUBLIC	_VER
_VER	db	"xxxxxxxxxx1.0"
DATA	ENDS

STACK	SEGMENT STACK 'STACK'
STACK	ENDS

	EXTRN	stk: WORD
	EXTRN	MAIN:FAR
PSEG
	ASSUME  DS: DGROUP
	ASSUME	SS: SGROUP
;
;
    
BEGIN	PROC	FAR
;
	cli
	mov	ax,dgroup
	mov	ds,ax
	mov	ax, sgroup
	mov	ss,ax
	mov	sp,offset sgroup:stk
	sti
;

	mov	ax,es			;es has our psp
	mov	_PSP+2,ax		;save for use by prostart
					;when it free's memory
	call	MAIN

	mov	ax,04c00h
	int	21h			;terminate
BEGIN	ENDP

ENDPS
;
	END
