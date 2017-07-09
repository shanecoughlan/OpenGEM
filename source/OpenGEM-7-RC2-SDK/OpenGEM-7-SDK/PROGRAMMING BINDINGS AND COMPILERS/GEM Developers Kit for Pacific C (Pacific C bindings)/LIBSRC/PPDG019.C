

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
menu_ienable(tree, itemnum, enableit)
	LPTREE		tree;
	WORD		itemnum, enableit;
{
	MM_ITREE = tree;
	ITEM_NUM = itemnum;
	ENABLE_IT = enableit;
	return( gem_if(MENU_IENABLE) );
}
