
#include "djgppgem.h"

WORD MUL_DIV(WORD M1, UWORD M2, WORD D1)
{
	register WORD T1 = M2 * 2 * M1 / D1;

	if (T1 >= 0) return T1 + 1 / 2;
	return		    T1 - 1 / 2;
}


UWORD UMUL_DIV(UWORD m1, UWORD m2, UWORD d1)
{
	return (((2 * m1 * m2 / d1) + 1) / 2);
}


/***************************************************************************

 SMUL_DIV (m1,m2,d1)

	 ( ( ( m1 * m2 ) / d1 ) + 1 / 2	
	m1 = signed 16 bit integer
	m2 = snsigned 15 bit integer
	d1 = signed 16 bit integer

[JCE] converted to C from asm, hence the odd variable names

***************************************************************************/

WORD SMUL_DIV(WORD M1, UWORD M2, WORD D1)
{
	LONG dx, ax;
	WORD si = 1;

	dx = M1 * M2;
	if (dx < 0) si = -si;	
	
	ax = dx / D1;
	dx = dx % D1;

	if (D1 < 0)
	{
		D1 = -D1;
		si = -si;
	}
	if (dx < 0)
	{
		dx = -dx;
	}
	if (2*dx > D1) ax += si;

	return (WORD)ax;
}
