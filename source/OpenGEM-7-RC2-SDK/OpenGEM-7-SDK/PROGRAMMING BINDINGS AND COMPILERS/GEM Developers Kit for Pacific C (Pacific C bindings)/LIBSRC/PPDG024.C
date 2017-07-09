

#include "ppdgem.h"
#include "ppdg0.h"

					/* Object Manager		*/
	WORD
objc_add(tree, parent, child)
	LPTREE		tree;
	WORD		parent, child;
{
	OB_TREE = tree;
	OB_PARENT = parent;
	OB_CHILD = child;
	return( gem_if( OBJC_ADD ) );
}
