

#include "ppdgem.h"
#include "ppdg0.h"



/* This is GEM/3 specific function*/

	WORD
menu_click(click, setit)
	WORD	click, setit;
{
	MN_CLICK = click;
	MN_SETIT = setit;
	return( gem_if( MENU_CLICK ));
}
