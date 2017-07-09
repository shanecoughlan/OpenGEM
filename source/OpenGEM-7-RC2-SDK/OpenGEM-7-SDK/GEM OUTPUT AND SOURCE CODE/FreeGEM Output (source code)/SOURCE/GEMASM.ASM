;/*******************************************************************/
;/*                                                                 */
;/*      Copyright 1999, Caldera Thin Clients, Inc.                 */
;/*      This software is licenced under the GNU Public License.    */
;/*      Please see LICENSE.TXT for further information.            */
;/*                                                                 */
;/*                 Historical Copyright                            */
;/*******************************************************************/
;/* Copyright (c) 1987 Digital Research Inc.		            */
;/* The software contained in this listing is proprietary to        */
;/* Digital Research Inc., Pacific Grove, California and is         */
;/* covered by U.S. and other copyright protection.  Unauthorized   */
;/* copying, adaptation, distribution, use or display is prohibited */
;/* and may be subject to civil and criminal penalties.  Disclosure */
;/* to others is prohibited.  For the terms and conditions of soft- */
;/* ware code use refer to the appropriate Digital Research         */
;/* license agreement.						    */
;/*******************************************************************/

;/*	GEMASM.ASM	5/03/84 -  01/05/85	Lee Lorenzen		*/
;/*	modified for MASM	    01/31/85	Tom Rolander		*/
;
PGROUP	GROUP	PROG

PROG	SEGMENT	BYTE PUBLIC 'PROG'
	ASSUME	CS:PGROUP
;
GEMENTRY 	equ	0efh
;
	PUBLIC	gem
	PUBLIC  crystal

crystal:
gem:	
	push	bp
	mov	bp,sp
	mov	cx,200
	mov	dx,0
	mov	bx,4[bp]
	mov	ax,6[bp]
	mov	es,ax
	int	GEMENTRY
	pop	bp
	ret
;
PROG	ENDS

	end