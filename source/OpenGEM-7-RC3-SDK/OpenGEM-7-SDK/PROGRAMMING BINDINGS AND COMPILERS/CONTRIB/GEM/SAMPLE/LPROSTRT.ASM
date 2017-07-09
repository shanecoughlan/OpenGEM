;/*	PROSTART.ASM	05/03/84 -  01/05/85	Lee Lorenzen		*/
;/*	added check for "GEMAES"    01/29/85	Tom Rolander		*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;/*     modified of Large model     09/12/85	C. Shaw                 */
;/*     modified for Lattice v3.0   04/22/86    M. Smith                */
;/*	modified for Lattice v3.2   09/04/87    Mei Chung		*/
;/*	to free memory correctly beyond bss instead of data		*/

	INCLUDE LARGE.MAC

	DSEG
SGROUP	GROUP	STACK
STACK	SEGMENT STACK 'STACK'
STACK	ENDS
	EXTRN	_VER:BYTE
	EXTRN	_PSP:WORD

PARABEG	dw	0
PARAEND	dw	0
gemaesmsg db	"GEMAES not present in memory."
	db	"$"
memerrmsg db	"Unable to free memory."
	db	"$"
terminmsg db	0dh,0ah
	db	"Execution terminated."
	db	"$"
	ENDDS


	EXTRN	GEMAIN:FAR
	EXTRN	stk   :WORD

	PSEG

	ASSUME	DS:DGROUP
	ASSUME  SS:SGROUP


	PUBLIC	MAIN
BEGIN   PROC    FAR
MAIN:
;
;	check for Lattice C 2.1x which already frees up memory

	mov	al,_VER+10
	cmp	al,'2'
	jg	afterfree
	jl	freemem
chknxtdig:
	mov	al,_VER+12
	cmp	al,'0'
	jg	afterfree
;
;	free up any memory we're not using
;		es points at our psp
;
freemem:
	mov	ax,_PSP+2
	mov	PARABEG,ax
	mov	ax, offset sgroup:stk
	add	ax,15
	mov	cl,4
	shr	ax,cl
	mov	cx,ax			; cx = our data size in paragraphs
	mov	ax, seg sgroup:stk
	mov	bx,ax
	add	bx,cx
	mov	PARAEND,bx
;	     
;
free_ex:
	mov	ax,PARABEG
	mov	es,ax
	mov	bx,PARAEND
	sub	bx,ax
	mov	ah,4ah
	int	21h
	mov	dx,offset memerrmsg
	jc	errormsg		;jump if set block fails
;
;
afterfree:
;
; check for presence of GEMAES before calling the application
	xor	ax,ax
	mov	es,ax
	mov	bx,es:[3bch]
	mov	ax,es:[3beh]
	mov	es,ax
	mov	di,2
	add	di,bx
	mov	si,offset gemaesmsg
	mov	cx,6
	rep	cmpsb
	jz	runapplication
	mov	dx,offset gemaesmsg
errormsg:
	mov	ah,9
	int	21h
	mov	dx,offset terminmsg
	mov	ah,9
	int	21h
	mov	ax,-1
	jmp	terminate

runapplication:
	mov	ax,ds
	mov	es,ax
	call	GEMAIN

terminate:
	ret			;return to ltcl
BEGIN	ENDP
	ENDPS
;
	END
