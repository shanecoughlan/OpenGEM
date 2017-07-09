

#include "ppdgem.h"
#include "ppdg0.h"



					/* Menu Manager			*/
	WORD
menu_bar(tree, showit)
	LPTREE		tree;
	WORD		showit;
{
	MM_ITREE = tree;
	SHOW_IT = showit;
	return( gem_if(MENU_BAR) );
}
