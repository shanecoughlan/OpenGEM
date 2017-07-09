/*	MACHINE.H		09/29/84-02/08/85	Lee Lorenzen	*/
/*	GEM20			12/17/85		Lowell Webster	*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1986			Digital Research Inc.
*	-------------------------------------------------------------
*/

#define	PCDOS	1	/* IBM PC DOS */
#define	CPM	0	/* CP/M version 2.2 */
#define GEMDOS	0	/* GEM DOS		*/

#define HILO 0		/* how bytes are stored */

#define I8086	1	/* Intel 8086/8088 */
#define	MC68K	0	/* Motorola 68000 */
#define ALCYON	0	/* Alcyon C Compiler */

#define MULTIAPP 0
#define SINGLAPP 1

#if SINGLAPP
#define NUM_WIN 8		/* 8 for main app and 3 desk accs	*/

#define NUM_ACCS 3		/* 3 for number of desk accs	*/

#define NUM_DESKACC 9		/* at least 9 slots for		*/
				/*   3 desk accessories		*/
				/* each desk acc can take 3 slots*/
				/* requires new string array	*/
				/*   in gemmnlib.c if num != 9	*/
#endif

#if MULTIAPP
#define NUM_WIN 12		/* 12 for 11 process entries		*/

#define NUM_ACCS 10		/* 10 for multi-process version	*/

#define NUM_DESKACC 17		/* at least 17 slots for	*/
				/*   3 desk accessories and	*/
				/*   11 process entries		*/
				/* each desk acc can take 3 slots*/
				/* requires new string array	*/
				/*   in gemmnlib.c if num != 17	*/
#endif
						/* in LARGE.A86		*/
						/* return length of 	*/
						/*   string pointed at	*/
						/*   by long pointer	*/
/*EXTERN WORD	LSTRLEN();*/
						/* copy n words from	*/
						/*   src long ptr to	*/
						/*   dst long ptr i.e.,	*/
						/*   LWCOPY(dlp, slp, n)*/
//EXTERN WORD	LWCOPY();
						/* copy n words from	*/
						/*   src long ptr to	*/
						/*   dst long ptr i.e.,	*/
						/*   LBCOPY(dlp, slp, n)*/
//EXTERN BYTE	LBCOPY();
						/* move bytes into wds*/
						/*   from src long ptr to*/
						/*   dst long ptr i.e.,	*/
						/*   until a null is	*/
						/*   encountered, then	*/
						/*   num moved is returned*/
						/*   LBWMOV(dwlp, sblp)*/
//EXTERN WORD	LBWMOV();

//EXTERN WORD	LSTCPY();
						/* coerce short ptr to	*/
						/*   low word  of long	*/
#define LW(x) ( (LONG)((UWORD)(x)) )
						/* coerce short ptr to	*/
						/*   high word  of long	*/
/*
#define HW(x) ((LONG)((UWORD)(x)) << 16)
*/
EXTERN LONG	HW(UWORD x);
						/* return low word of	*/
						/*   a long value	*/
#define LLOWD(x) ((UWORD)(x))
						/* return high word of	*/
						/*   a long value	*/
/*
#define LHIWD(x) ((UWORD)(x >> 16))

EXTERN WORD	LHIWD(UWORD x);
*/						/* return low byte of	*/
						/*   a word value	*/
#define LLOBT(x) ((BYTE)(x & 0x00ff))
						/* return high byte of	*/
						/*   a word value	*/
#define LHIBT(x) ((BYTE)( (x >> 8) & 0x00ff))

/* [JCE] I'm more comfortable with these macros */

#define FP_SEG(x)	   (((LONG)(FAR VOID *)(x)) >> 16)

#define FP_OFF(x)      ((UWORD)(x))
#define MK_FP(seg,off) ((VOID FAR *) ((((ULONG)seg)<< 16) | ((UWORD)off)))

/************************************************************************/

#if I8086


						/* return long address	*/
						/*   of short ptr	*/
/*EXTERN LONG	ADDR();*/
#define ADDR(x) ((FAR BYTE *) (x))

						/* return long address	*/
						/*   of the data seg	*/
/*EXTERN LONG	LLDS();*/

						/* return long address	*/
						/*   of the code seg	*/
/*EXTERN LONG	LLCS();*/
						/* return a single byte	*/
						/*   pointed at by long	*/
						/*   ptr		*/
//#define LBGET(x) ( (UBYTE) *((FAR BYTE * )(x)) )
/*EXTERN BYTE	LBGET();*/
						/* set a single byte	*/
						/*   pointed at by long	*/
						/*   ptr, LBSET(lp, bt)	*/
//#define LBSET(x, y)  ( *((FAR BYTE *)(x)) = y)
/*EXTERN BYTE	LBSET();*/
						/* return a single word	*/
						/*   pointed at by long	*/
						/*   ptr		*/
//#define LWGET(x) ( (WORD) *((FAR WORD *)(x)) )
/*EXTERN WORD	LWGET();*/
						/* set a single word	*/
						/*   pointed at by long	*/
						/*   ptr, LWSET(lp, bt)	*/
//#define LWSET(x, y)  ( *((FAR WORD *)(x)) = y)
/*EXTERN WORD	LWSET();*/
						/* return a single long	*/
						/*   pointed at by long	*/
						/*   ptr		*/
//#define LLGET(x) ( *((FAR LONG *)(x)))
/*EXTERN LONG	LLGET();*/
						/* set a single long	*/
						/*   pointed at by long	*/
						/*   ptr, LLSET(lp, bt)	*/
//#define LLSET(x, y) ( *((FAR LONG *)(x)) = y)
/*EXTERN LONG	LLSET();*/
						/* return 0th byte of	*/
						/*   a long value given	*/
						/*   a short pointer to	*/
						/*   the long value	*/
#define LBYTE0(x) (*(BYTE *)x)
						/* return 1st byte of	*/
						/*   a long value given	*/
						/*   a short pointer to	*/
						/*   the long value	*/
#define LBYTE1(x) (*(((BYTE *)x)+1))
						/* return 2nd byte of	*/
						/*   a long value given	*/
						/*   a short pointer to	*/
						/*   the long value	*/
#define LBYTE2(x) (*(((BYTE *)x)+2))
						/* return 3rd byte of	*/
						/*   a long value given	*/
						/*   a short pointer to	*/
						/*   the long value	*/
#define LBYTE3(x) (*(((BYTE *)x)+3))

#endif


/************************************************************************/

#if MC68K

						/* return a long address*/
						/*   of a short pointer */
#define ADDR /**/


						/* return a single byte	*/
						/*   pointed at by long	*/
						/*   ptr		*/
#define LBGET(x) ( (UBYTE) *((BYTE * )(x)) )
						/* set a single byte	*/
						/*   pointed at by long	*/
						/*   ptr, LBSET(lp, bt)	*/
#define LBSET(x, y)  ( *((BYTE *)(x)) = y)
						/* return a single word	*/
						/*   pointed at by long	*/
						/*   ptr		*/
#define LWGET(x) ( (WORD) *((WORD *)(x)) )
						/* set a single word	*/
						/*   pointed at by long	*/
						/*   ptr, LWSET(lp, bt)	*/
#define LWSET(x, y)  ( *((WORD *)(x)) = y)

						/* return a single long	*/
						/*   pointed at by long	*/
						/*   ptr		*/
#define LLGET(x) ( *((LONG *)(x)))
						/* set a single long	*/
						/*   pointed at by long	*/
						/*   ptr, LLSET(lp, bt)	*/
#define LLSET(x, y) ( *((LONG *)(x)) = y)

						/* return 0th byte of	*/
						/*   a long value given	*/
						/*   a short pointer to	*/
						/*   the long value	*/
#define LBYTE0(x) ( *((x)+3) )
						/* return 1st byte of	*/
						/*   a long value given	*/
						/*   a short pointer to	*/
						/*   the long value	*/
#define LBYTE1(x) ( *((x)+2) )
						/* return 2nd byte of	*/
						/*   a long value given	*/
						/*   a short pointer to	*/
						/*   the long value	*/
#define LBYTE2(x) ( *((x)+1) )
						/* return 3rd byte of	*/
						/*   a long value given	*/
						/*   a short pointer to	*/
						/*   the long value	*/
#define LBYTE3(x) (*(x))



#endif

EXTERN VOID dbg();
EXTERN VOID dump();
