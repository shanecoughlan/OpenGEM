

#include "ppdgem.h"
#include "ppdg0.h"




	WORD
objc_edit(tree, obj, inchar, idx, kind)
	LPTREE		tree;
	WORD		obj;
	WORD		inchar, *idx, kind;
{
	OB_TREE = tree;
	OB_OBJ = obj;
	OB_CHAR = inchar;
	OB_IDX = *idx;
	OB_KIND = kind;
	gem_if( OBJC_EDIT );
	*idx = OB_ODX;
	return((WORD) RET_CODE );
}
