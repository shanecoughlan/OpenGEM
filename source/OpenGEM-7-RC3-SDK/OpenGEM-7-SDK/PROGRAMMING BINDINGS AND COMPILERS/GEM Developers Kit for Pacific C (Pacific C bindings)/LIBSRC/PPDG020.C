

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
menu_tnormal(tree, titlenum, normalit)
	LPTREE		tree;
	WORD		titlenum, normalit;
{
	MM_ITREE = tree;
	TITLE_NUM = titlenum;
	NORMAL_IT = normalit;
	return( gem_if( MENU_TNORMAL ) );
}
