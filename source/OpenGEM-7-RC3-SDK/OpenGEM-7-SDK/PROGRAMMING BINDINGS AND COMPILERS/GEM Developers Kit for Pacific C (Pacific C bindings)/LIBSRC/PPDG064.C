

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
rsrc_obfix(tree, obj)
	LPTREE		tree;
	WORD		obj;
{
	RS_TREE = tree;
	RS_OBJ = obj;
	return( gem_if(RSRC_OBFIX) );
}
