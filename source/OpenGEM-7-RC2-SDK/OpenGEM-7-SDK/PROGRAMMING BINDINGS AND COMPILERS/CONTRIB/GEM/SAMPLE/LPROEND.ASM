
;/*	PROEND.ASM		1/31/85 - 1/31/85	Tom Rolander	*/
;/*				9/4/87	Modified for Lattice 3.2 Mei Chung*/

;
;Lattice 3.2 generate uninitialized data segment beyond proend, to free
;memory correctly we should free it after the stack segment.


SGROUP	GROUP	STACK

STKRSV	EQU	2046
STACK	SEGMENT STACK 'STACK'
	ASSUME SS:STACK
	PUBLIC	stk
SBASE	DB	STKRSV DUP (?)
stk	DW	?
STACK	ENDS

	end
