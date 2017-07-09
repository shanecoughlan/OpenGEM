;/*	GEMGMAIN.A86		4/18/84 - 06/07/85	Lee Lorenzen	*/
;
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
;*	Copyright (C) 1986			Digital Research Inc.
;*	-------------------------------------------------------------
;*/
;
;JCE 24-4-1999 converted to Pacific C
;
	.psect	_TEXT,class=CODE
	.globl	gomain,terminate
	.globl	_gem_main
;
;
;	go to the application
;
gomain:
	call	_gem_main
;
;	go back to dos
;
terminate:
	mov	ax,#04c00h
	int	#21h
;

