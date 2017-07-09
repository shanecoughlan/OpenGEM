;/*	ACCSTART.ASM	05/03/84 -  01/05/85	Lee Lorenzen		*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;
PGROUP	GROUP	PROG

PROG	SEGMENT	BYTE PUBLIC 'PROG'

	ASSUME	CS:PGROUP

	EXTRN	GEMAIN:NEAR

	PUBLIC	MAIN
MAIN:
	call	GEMAIN

	ret			;return to tcs

PROG	ENDS
;
	END
