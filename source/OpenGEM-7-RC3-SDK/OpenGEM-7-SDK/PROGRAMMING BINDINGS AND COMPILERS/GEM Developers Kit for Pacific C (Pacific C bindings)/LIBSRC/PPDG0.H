/*
*	-------------------------------------------------------------
*	GEM Desktop					  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1985 - 1987		Digital Research Inc.
*	-------------------------------------------------------------
*/

#define CTRL_CNT	4		/*mike's version is 3*/

extern BYTE		ctrl_cnts[];
extern GEMBLK		gb;
extern UWORD		control[C_SIZE];
extern UWORD		global[G_SIZE];
extern UWORD		int_in[I_SIZE];
extern UWORD		int_out[O_SIZE];
extern LPVOID		addr_in[AI_SIZE];
extern LPVOID		addr_out[AO_SIZE];

extern LPGEMBLK 	ad_g;

extern WORD gem_if(WORD opcode);
