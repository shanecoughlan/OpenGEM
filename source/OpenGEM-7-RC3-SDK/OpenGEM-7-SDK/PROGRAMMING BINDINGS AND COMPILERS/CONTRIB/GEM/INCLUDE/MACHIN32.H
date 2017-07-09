/*	MACHINE.H		09/29/84-02/08/85	Lee Lorenzen	*/
/*	GEM DEVELOPER KIT 	06/07/86		Lowell Webster	*/
/*      DJGPP                   21 Jan 1998             John Elliott    */

#include <pc.h>
#include <dpmi.h>
#include <go32.h>
#include <sys/farptr.h>
#include <sys/movedata.h>
#include "dosalloc.h"

#define	PCDOS	1	/* IBM PC DOS		*/
#define GEMDOS	0	/* DRI GEM DOS		*/

#define HILO 0		/* how bytes are stored */

#ifndef I8086
#define I8086 	1 	/* Intel 8086/8088 */
#endif

#define	MC68K	0	/* Motorola 68000 */

#define ALCYON	0	/* Alcyon C Compiler */

						/* in LONGASM.ASM	*/
						/* return length of 	*/
						/*   string pointed at	*/
						/*   by long pointer	*/
#define LSTRLEN              dj_lstrlen
#define LWCOPY(src, dest, n) movedata(_dos_ds, src, _dos_ds, dest, n*2)
#define LBCOPY(src, dest, n) movedata(_dos_ds, src, _dos_ds, dest, n)

						/* move bytes into wds*/
						/*   from src long ptr to*/
						/*   dst long ptr i.e.,	*/
						/*   until a null is	*/
						/*   encountered, then	*/
						/*   num moved is returned*/
						/*   LBWMOV(dwlp, sblp)*/
EXTERN WORD	LBWMOV(LPWORD dst, LPBYTE src);

EXTERN WORD	LSTCPY();

#define LP_SEG(x)  ((x >> 4) & 0xFFFF)		/* [JCE] Convert an absolute */
#define LP_OFF(x)  (x & 0x0F)			/* address in 16-bit area to */
						/* a suitable segment+offset */

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

/* This is DJGPP, there is no need to use hideous macros when 
 *  we've got proper inline functions. This also saves any 
 *  problems with multiple evaluation of x.
 *
 * #define LINEAR(x) (LW(x)  + (((LONG)LHIWD(x)) << 4)) 
 * #define SEGOFF(x) (((x) & 0x0f)  | (((LONG)((x) & 0xFFFF0)) << 12)) 
 */

__inline__ extern LONG LINEAR(LONG x)	/* seg:off -> linear */
{
	return (x & 0xFFFF) + ( (x & 0xFFFF0000) >> 12);
}

__inline__ extern LONG  SEGOFF(LONG x)	/* linear -> seg:off */
{
	return (x & 0x0f) | ((x & 0xFFFF0) << 12);
}

/* NOTE: linear->seg:off is NOT unique. It tries to put the pointer as
 *       near to the beginning of the segment as possible. */

/************************************************************************/

#if I8086

/* ADDR cannot be used because local objects are not visible to GEM
 * at all.
 *
EXTERN LONG	ADDR(VOID *p);
*/

/* LLDS and LLCS removed because they have no meaning - our program is 
 * entirely outside GEM's address space.
 * 
 * XXX best way of managing callbacks from 16-bit to 32-bit code?
 *
 */

#define LBGET(p)    _farpeekb(_dos_ds, p)	/* Far peeks and pokes */
#define LBSET(p,b)  _farpokeb(_dos_ds, p, b)
#define LWGET(p)    _farpeekw(_dos_ds, p)
#define LWSET(p,w)  _farpokew(_dos_ds, p, w)

/* LLSET and LLGET are not included, to force necessary portability 
  changes:

  - LLSET / LLGET on far pointers (in segment:offset form) should be
   replaced by LSSET / LSGET.
  - LLSET / LLGET on 32-bit words should be replaced by LDSET / LDGET.

  This is because the system of storing addresses is:

  Data in GEM's address space stores pointers as segment:offset
  Data in the 32-bit address space stores pointers as 32-bit offsets

  LSSET / LSGET do the conversion between the two; LDSET / LDGET don't.

#define LLGET(p)    _farpeekl(_dos_ds, p)
#define LLSET(p,l)  _farpokel(_dos_ds, p, l)
*/

#define LDGET(p)    _farpeekl(_dos_ds, p)
#define LDSET(p,l)  _farpokel(_dos_ds, p, l)

/* As LLGET/LLSET, but converts between DJGPP's linear pointers and
 * GEM's seg:off pointers */
#define LSGET(p)    LINEAR(_farpeekl(_dos_ds, p))
#define LSSET(p,l)  _farpokel(_dos_ds, p, SEGOFF(l))

#define LBYTE0(x) (*x)
						/* return 1st byte of	*/
						/*   a long value given	*/
						/*   a short pointer to	*/
						/*   the long value	*/
#define LBYTE1(x) (*(x+1))
						/* return 2nd byte of	*/
						/*   a long value given	*/
						/*   a short pointer to	*/
						/*   the long value	*/
#define LBYTE2(x) (*(x+2))
						/* return 3rd byte of	*/
						/*   a long value given	*/
						/*   a short pointer to	*/
						/*   the long value	*/
#define LBYTE3(x) (*(x+3))

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
