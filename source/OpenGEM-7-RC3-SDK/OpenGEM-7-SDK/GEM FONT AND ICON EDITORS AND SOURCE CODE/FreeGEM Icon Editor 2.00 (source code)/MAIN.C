/************************************************************************

    EDICON 2.00 - icon editor for GEM

    Copyright (C) 1998,1999,2000  John Elliott <jce@seasip.demon.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*************************************************************************/


/* A quick main() for GEM apps */

#include "ppdgem.h"
#include <dos.h>
#include <conio.h>

static char *fail = "GEMAES not present in memory.\n";
/*static char *meme = "Unable to free memory.\n";*/
static char *exte = "Execution terminated.\n";

int freemem(void);


int main(int argc, char **argv)
{
	LPLONG pGEM = MK_FP(0, 4 * 0xEF);
	LPBYTE pSIG = (LPBYTE)*pGEM;
	int n;

	for (n = 0; n < 6; n++)
	{
		if (pSIG[n+2] != fail[n])
		{
			cputs(fail);
			cputs(exte);
			return 1;
		}
	}

/* I don't tend to find this necessary, but perhaps that's just me.

   If you do need it, just uncomment it.

 */

/*	if (freemem())
	{
		cputs(meme);
		return 2;
	}
*/	return GEMAIN(argc, argv);
}

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
/*
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
