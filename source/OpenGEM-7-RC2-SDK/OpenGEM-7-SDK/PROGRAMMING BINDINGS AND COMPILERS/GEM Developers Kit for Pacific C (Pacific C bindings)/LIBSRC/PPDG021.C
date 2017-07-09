

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
menu_text(tree, inum, ptext)
	LPTREE		tree;
	WORD		inum;
	LPBYTE		ptext;
{
	MM_ITREE = tree;
	ITEM_NUM = inum;
	MM_PTEXT = ptext;
	return( gem_if( MENU_TEXT ) );
}
