

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
objc_delete(tree, delob)
	LPTREE		tree;
	WORD		delob;
{
	OB_TREE = tree;
	OB_DELOB = delob;
	return( gem_if( OBJC_DELETE ) );
}
