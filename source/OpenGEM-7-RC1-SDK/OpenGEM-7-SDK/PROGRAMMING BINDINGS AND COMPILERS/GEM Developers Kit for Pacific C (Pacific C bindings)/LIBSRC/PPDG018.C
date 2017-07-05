

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
menu_icheck(tree, itemnum, checkit)
	LPTREE		tree;
	WORD		itemnum, checkit;
{
	MM_ITREE = tree;
	ITEM_NUM = itemnum;
	CHECK_IT = checkit;
	return( gem_if(MENU_ICHECK) );
}
