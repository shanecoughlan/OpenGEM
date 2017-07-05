

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
objc_order(tree, mov_obj, newpos)
	LPTREE		tree;
	WORD		mov_obj, newpos;
{
	OB_TREE = tree;
	OB_OBJ = mov_obj;
	OB_NEWPOS = newpos;
	return( gem_if( OBJC_ORDER ) );
}
