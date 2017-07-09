

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
objc_find(tree, startob, depth, mx, my)
	LPTREE		tree;
	WORD		startob, depth, mx, my;
{
	OB_TREE = tree;
	OB_STARTOB = startob;
	OB_DEPTH = depth;
	OB_MX = mx;
	OB_MY = my;
	return( gem_if( OBJC_FIND ) );
}
