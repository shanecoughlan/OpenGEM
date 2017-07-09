
/* A quick main() for GEM apps */

#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include "djgppgem.h"

static char *fail = "GEMAES not present in memory.\r\n";
static char *exte = "Execution terminated.\r\n";


int main(int argc, char **argv)
{
	LPLONG pGEM = 4 * 0xEF;
	LPBYTE pSIG = LSGET(pGEM) + 2;
	int n;

	for (n = 0; n < 6; n++)
	{
		if (LBGET(pSIG + n) != fail[n])
		{
			cputs(fail);
			cputs(exte);
			return 1;
		}
	}
	return GEMAIN(argc, argv);
}


__inline__ LONG LINEAR(LONG x)	/* seg:off -> linear */
{
	return (x & 0xFFFF) + ( (x & 0xFFFF0000) >> 12);
}

__inline__ LONG  SEGOFF(LONG x)	/* linear -> seg:off */
{
	return (x & 0x0f) | ((x & 0xFFFF0) << 12);
}



/* NOTE: This is code for Pacific C to free DOS memory beyond the end of 
  the program. Below that is the Lattice C equivalent of this file.
  I have not written any memory-freeing code for DJGPP; I don't know 
  if it's appropriate when most of the program ends up in linear memory
  rather than DOS memory.

#asm

	.globl __Hstack

_freemem:
	push es
	push bx
	push cx

;	int	#3
	
	mov	ah,#0x51
	int	#0x21				;Get PSP segment in BX

	mov	ax,# __Hstack		;Top of our memory (offset)
	add	ax,#0x0F
	mov	cl,#0x04
	shr	ax,cl
	mov	cx,ax				;CX = offset in paras
	
	mov	ax,#seg __Hstack	;Top of our memory
	add	ax,cx				;(seg+off)

	sub	ax,bx				;AX = revised segment length
	push ax

	mov	ax,bx				;Segment base
	mov	es,ax				;ES = segment base
	pop	bx					;BX = revised length
	mov	ah,#0x4a
	int	#0x21

	pop	cx
	pop	bx
	pop	es
	
	mov	ax,#1
	jc	frend
	xor	ax,ax
frend:
	ret
	
#endasm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; LATTICE C CODE FOLLOWS ;;

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


*/
