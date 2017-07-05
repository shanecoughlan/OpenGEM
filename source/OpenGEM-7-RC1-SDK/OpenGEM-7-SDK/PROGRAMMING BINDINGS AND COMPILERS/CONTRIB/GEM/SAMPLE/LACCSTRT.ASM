;/*	ACCSTART.ASM	05/03/84 -  01/05/85	Lee Lorenzen		*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;/*	modified for Large model    09/17/85	C. Shaw			*/
;
	INCLUDE	LARGE.MAC


	EXTRN	GEMAIN:FAR
	
	PSEG
	PUBLIC	MAIN	  
BEGIN	PROC	FAR
MAIN:
	call	GEMAIN

	ret			;return to tcs
BEGIN	ENDP
	ENDPS
;
	END
