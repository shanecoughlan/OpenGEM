/*	GEMBIND.C	9/11/85				Lee Lorenzen	*/
/*	for 3.0		3/11/86 - 8/26/86		MDF		*/
/*	merge source	5/28/87				mdf		*/
/*	menu_click	9/25/87				mdf		*/
/*  Pacific C   4 Jan 1998          John Elliott    */

/*
*	-------------------------------------------------------------
*	GEM Desktop					  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1985 - 1987		Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "ppdgem.h" 
#include "ppdgbind.h"
#include "ppdg0.h"

#define CTRL_CNT	4		/*mike's version is 3*/


GLOBAL BYTE		ctrl_cnts[] =
{
/* Application Manager		*/
	0, 1, 1, 			/* func 010		[JCE] Extra parameter in addr_in */
	2, 1, 1, 			/* func 011		*/
	2, 1, 1, 			/* func 012		*/
	0, 1, 1, 			/* func 013		*/
	2, 1, 1, 			/* func 014		*/
	1, 1, 1, 			/* func 015		*/
	2, 1, 0, 			/* func 016		*/
	0, 0, 0, 			/* func 017		*/
	1, 5, 2, 			/* func 018		*/
	0, 1, 0, 			/* func 019		*/
/* Event Manager		*/
	0, 1, 0, 			/* func 020		*/
	3, 5, 0, 			/* func 021		*/
	5, 5, 0, 			/* func 022		*/
	0, 1, 1, 			/* func 023		*/
	2, 1, 0, 			/* func 024		*/
	16, 7, 1,	 		/* func 025		*/
	2, 1, 0, 			/* func 026		*/
	0, 0, 0, 			/* func 027		*/
	0, 0, 0, 			/* func 028		*/
	0, 0, 0, 			/* func 009		*/
/* Menu Manager			*/
	1, 1, 1, 			/* func 030		*/
	2, 1, 1, 			/* func 031		*/
	2, 1, 1, 			/* func 032		*/
	2, 1, 1, 			/* func 033		*/
	1, 1, 2, 			/* func 034		*/
	1, 1, 1, 			/* func 005		*/
	1, 1, 0, 			/* func 006		*/
	2, 1, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* Object Manager		*/
	2, 1, 1, 			/* func 040		*/
	1, 1, 1, 			/* func 041		*/
	6, 1, 1, 			/* func 042		*/
	4, 1, 1, 			/* func 043		*/
	1, 3, 1, 			/* func 044		*/
	2, 1, 1, 			/* func 045		*/
	4, 2, 1, 			/* func 046		*/
	8, 1, 1, 			/* func 047		*/
	0, 0, 0, 			/* func 048		*/
	0, 0, 0, 			/* func 049		*/
/* Form Manager			*/
	1, 1, 1, 			/* func 050		*/
	9, 1, 0, 			/* func 051 		*/
	1, 1, 1, 			/* func 002		*/
	1, 1, 0, 			/* func 003		*/
	0, 5, 1, 			/* func 004		*/
	3, 3, 1, 			/* func 005		*/
	2, 2, 1, 			/* func 006		*/
	0, 0, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* Process Manager		*/
	0, 0, 0, 			/* func 060		*/
	0, 0, 0, 			/* func 061		*/
	0, 0, 0, 			/* func 062		*/
	0, 0, 0, 			/* func 063		*/
	0, 0, 0, 			/* func 064		*/
	0, 0, 0, 			/* func 065		*/
	0, 0, 0, 			/* func 066		*/
	0, 0, 0, 			/* func 067		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* Graphics Manager	*/
	4, 3, 0, 			/* func 070		*/
	8, 3, 0, 			/* func 071		*/
	6, 1, 0, 			/* func 072		*/
	8, 1, 0, 			/* func 073		*/
	8, 1, 0, 			/* func 074		*/
	4, 1, 1, 			/* func 075		*/
	3, 1, 1, 			/* func 076		*/
	0, 5, 0, 			/* func 077		*/
	1, 1, 1, 			/* func 078		*/
	0, 5, 0, 			/* func 009		*/
/* Scrap Manager		*/
	0, 1, 1, 			/* func 080		*/
	0, 1, 1, 			/* func 081		*/
	0, 1, 0, 			/* func 082		*/
	0, 0, 0, 			/* func 083		*/
	0, 0, 0, 			/* func 084		*/
	0, 0, 0, 			/* func 005		*/
	0, 0, 0, 			/* func 006		*/
	0, 0, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* fseler Manager		*/
	0, 2, 2, 			/* func 090		*/
	0, 2, 3, 			/* func 091		*/
	0, 0, 0, 			/* func 092		*/
	0, 0, 0, 			/* func 003		*/
	0, 0, 0, 			/* func 004		*/
	0, 0, 0, 			/* func 005		*/
	0, 0, 0, 			/* func 006		*/
	0, 0, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* Window Manager		*/
	5, 1, 0, 			/* func 100		*/
	5, 1, 0, 			/* func 101		*/
	1, 1, 0, 			/* func 102		*/
	1, 1, 0, 			/* func 103		*/
	2, 5, 0, 			/* func 104		*/
	6, 1, 0, 			/* func 105		*/
	2, 1, 0, 			/* func 106		*/
	1, 1, 0, 			/* func 107		*/
	6, 5, 0, 			/* func 108		*/
	0, 0, 0, 			/* func 009		*/
/* Resource Manager		*/
	0, 1, 1, 			/* func 110		*/
	0, 1, 0, 			/* func 111		*/
	2, 1, 0, 			/* func 112		*/
	2, 1, 1, 			/* func 113		*/
	1, 1, 1, 			/* func 114		*/
	0, 0, 0, 			/* func 115		*/
	0, 0, 0, 			/* func 006		*/
	0, 0, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* Shell Manager		*/
	0, 1, 2, 			/* func 120		*/
	3, 1, 2, 			/* func 121		*/
	1, 1, 1, 			/* func 122		*/
	1, 1, 1, 			/* func 123		*/
	0, 1, 1, 			/* func 124		*/
	0, 1, 2, 			/* func 125		*/
	0, 1, 2, 			/* func 126		*/
	0, 1, 2, 			/* func 127		*/
	1, 0, 1, 			/* func 128		*/
	1, 0, 1, 			/* func 129		*/
/* Extended Graphics Manager	*/
	6, 6, 0,			/* func 130		*/
	9, 1, 0,			/* func 131		*/
	5, 0, 0,			/* func 132	(xgrf_colour JCE 7 Jan 1998)     */
	0, 1, 1,			/* func 133	(xgrf_dtmfdb now returns a word) */
	0, 0, 0,			/* func 134		*/
	0, 0, 0,			/* func 135		*/
	0, 0, 0,			/* func 136		*/
	0, 0, 0,			/* func 137		*/
	0, 0, 0,			/* func 138		*/
	0, 0, 0 			/* func 139		*/
};

/* [JCE] new functions added by me start at 1000 */

GLOBAL BYTE		ctrl_ncnts[] =
{
/* Property Manager		*/
	2, 1, 3, 			/* func 1010		*/
	1, 1, 3, 			/* func 1011		*/
	1, 1, 2, 			/* func 1012		*/
	1, 1, 0, 			/* func 1013		*/
	2, 1, 0, 			/* func 1014		*/
	0, 0, 0, 			/* func 0005		*/
	0, 0, 0, 			/* func 0006		*/
	0, 0, 0, 			/* func 0007		*/
	0, 0, 0, 			/* func 0008		*/
	0, 0, 0, 			/* func 0009		*/
/* Extended application manager */
	1, 4, 0, 			/* func 1020		*/
	0, 0, 0, 			/* func 0001		*/
	0, 0, 0, 			/* func 0002		*/
	0, 0, 0, 			/* func 0003		*/
	0, 0, 0, 			/* func 0004		*/
	0, 0, 0, 			/* func 0005		*/
	0, 0, 0, 			/* func 0006		*/
	0, 0, 0, 			/* func 0007		*/
	0, 0, 0, 			/* func 0008		*/
	0, 0, 0, 			/* func 0009		*/
/* Extended shell manager */
	0, 0, 1, 			/* func 1030		*/
	0, 0, 1, 			/* func 1031		*/
	0, 0, 0, 			/* func 0002		*/
	0, 0, 0, 			/* func 0003		*/
	0, 0, 0, 			/* func 0004		*/
	0, 0, 0, 			/* func 0005		*/
	0, 0, 0, 			/* func 0006		*/
	0, 0, 0, 			/* func 0007		*/
	0, 0, 0, 			/* func 0008		*/
	0, 0, 0, 			/* func 0009		*/

};

					/* in STARTUP.A86	*/
					
GLOBAL GEMBLK		gb;
GLOBAL UWORD		control[C_SIZE];
GLOBAL UWORD		global[G_SIZE];
GLOBAL UWORD		int_in[I_SIZE];
GLOBAL UWORD		int_out[O_SIZE];
GLOBAL LPVOID		addr_in[AI_SIZE];
GLOBAL LPVOID		addr_out[AO_SIZE];

GLOBAL LPGEMBLK 	ad_g;

	WORD
gem_if(opcode)
	WORD		opcode;
{
	WORD		i;
	BYTE		*pctrl;

	control[0] = opcode;

	if (opcode < 1000 ) pctrl = &ctrl_cnts [(opcode - 10) * 3];
	else                pctrl = &ctrl_ncnts[(opcode - 1010) * 3];
	for(i=1; i<=CTRL_CNT; i++)
	  control[i] = *pctrl++;

	gem(ad_g);
	return((WORD) RET_CODE );
}
