/*	MACHINE.H		09/29/84-02/08/85	Lee Lorenzen	*/
/*	GEM DEVELOPER KIT 	06/07/86		Lowell Webster	*/
/*      Modified for Pacific C  4-1-1998                John Elliott */

#define	PCDOS	1	/* IBM PC DOS		*/
#define GEMDOS	0	/* DRI GEM DOS		*/

#define HILO 0		/* how bytes are stored */

#ifdef i8086
#define I8086   1
#endif

#ifndef I8086
#define I8086 	1 	/* Intel 8086/8088 */
#endif

#define	MC68K	0	/* Motorola 68000 */

#define ALCYON	0	/* Alcyon C Compiler */

						/* in LONGASM.ASM	*/
						/* return length of 	*/
						/*   string pointed at	*/
						/*   by long pointer	*/
EXTERN WORD	LSTRLEN(LPBYTE);
						/* copy n words from	*/
						/*   src long ptr to	*/
						/*   dst long ptr i.e.,	*/
						/*   LWCOPY(dlp, slp, n)*/
EXTERN WORD	LWCOPY(LPWORD, LPWORD, WORD);
						/* copy n words from	*/
						/*   src long ptr to	*/
						/*   dst long ptr i.e.,	*/
						/*   LBCOPY(dlp, slp, n)*/
EXTERN BYTE	LBCOPY(LPBYTE, LPBYTE, WORD);
						/* move bytes into wds*/
						/*   from src long ptr to*/
						/*   dst long ptr i.e.,	*/
						/*   until a null is	*/
						/*   encountered, then	*/
						/*   num moved is returned*/
						/*   LBWMOV(dwlp, sblp)*/
EXTERN WORD	LBWMOV(LPWORD, LPBYTE);

EXTERN WORD	LSTCPY(LPBYTE, LPBYTE);
						/* coerce short ptr to	*/
						/*   low word  of long	*/
#define LW(x) ( (LONG)((UWORD)(x)) )
						/* coerce short ptr to	*/
						/*   high word  of long	*/

#define HW(x) ((LONG)((UWORD)(x)) << 16)
						/* return low word of	*/
						/*   a long value	*/
#define LLOWD(x) ((UWORD)(x))
						/* return high word of	*/
						/*   a long value	*/
#define LHIWD(x) ((UWORD)(x >> 16))
						/* return low byte of	*/
						/*   a word value	*/
#define LLOBT(x) ((BYTE)(x & 0x00ff))
						/* return high byte of	*/
						/*   a word value	*/
#define LHIBT(x) ((BYTE)( (x >> 8) & 0x00ff))

/************************************************************************/

#if I8086

/* Since Pacific C allows far pointers in the small model, these can be done
 * as macros rather than functions */

						/* return long address	*/
						/*   of short ptr	*/
/*EXTERN LONG	ADDR();*/
#define ADDR (LPVOID)

						/* return long address	*/
						/*   of the data seg	*/
EXTERN LPVOID LLDS(VOID);

						/* return long address	*/
						/*   of the code seg	*/
EXTERN LPVOID LLCS(VOID);
						/* return a single byte	*/
						/*   pointed at by long	*/
						/*   ptr		*/
#define LBGET(x) ( (UBYTE) *((LPBYTE)(x)) )
						/* set a single byte	*/
						/*   pointed at by long	*/
						/*   ptr, LBSET(lp, bt)	*/
#define LBSET(x, y)  ( *((LPBYTE)(x)) = y)
						/* return a single word	*/
						/*   pointed at by long	*/
						/*   ptr		*/
#define LWGET(x) ( (WORD) *((LPWORD)(x)) )
						/* set a single word	*/
						/*   pointed at by long	*/
						/*   ptr, LWSET(lp, bt)	*/
#define LWSET(x, y)  ( *((LPWORD)(x)) = y)

						/* return a single long	*/
						/*   pointed at by long	*/
						/*   ptr		*/
#define LLGET(x) ( *((LPLONG)(x)))
						/* set a single long	*/
						/*   pointed at by long	*/
						/*   ptr, LLSET(lp, bt)	*/
#define LLSET(x, y) ( *((LPLONG)(x)) = y)

						/* return 0th byte of	*/
						/*   a long value given	*/
						/*   a short pointer to	*/
						/*   the long value	*/
#define LVGET(x) ( *((LPLPTR)(x)))
						/* set a single pointer	*/
						/*   pointed at by long	*/
						/*   ptr, LVSET(lp, bt)	*/
#define LVSET(x, y) ( *((LPLPTR)(x)) = y)

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
