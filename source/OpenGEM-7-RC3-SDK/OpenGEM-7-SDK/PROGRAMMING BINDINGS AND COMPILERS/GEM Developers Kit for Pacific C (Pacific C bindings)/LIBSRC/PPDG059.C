

#include "ppdgem.h"
#include "ppdg0.h"


	WORD
wind_calc(wctype, kind, x, y, w, h, px, py, pw, ph)
	WORD		wctype;
	UWORD		kind;
	WORD		x, y, w, h;
	WORD		*px, *py, *pw, *ph;
{
	WM_WCTYPE = wctype;
	WM_WCKIND = kind;
	WM_WCIX = x;
	WM_WCIY = y;
	WM_WCIW = w;
	WM_WCIH = h;
	gem_if( WIND_CALC );
	*px = WM_WCOX;
	*py = WM_WCOY;
	*pw = WM_WCOW;
	*ph = WM_WCOH;
	return((WORD) RET_CODE );
}
