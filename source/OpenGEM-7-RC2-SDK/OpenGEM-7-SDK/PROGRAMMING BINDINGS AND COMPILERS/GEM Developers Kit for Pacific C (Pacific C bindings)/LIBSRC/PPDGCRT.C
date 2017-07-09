
#include "ppdgem.h"

WORD MUL_DIV(WORD M1, UWORD M2, WORD D1)
{
	register WORD T1 = M2 * 2 * M1 / D1;

	if (T1 >= 0) return T1 + 1 / 2;
	return			    T1 - 1 / 2;
}


UWORD UMUL_DIV(UWORD m1, UWORD m2, UWORD d1)
{
	return (((2 * m1 * m2 / d1) + 1) / 2);
}
