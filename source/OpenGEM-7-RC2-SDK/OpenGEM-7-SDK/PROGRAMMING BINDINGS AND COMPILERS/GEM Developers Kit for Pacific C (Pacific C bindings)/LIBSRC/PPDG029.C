

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
objc_offset(tree, obj, poffx, poffy)
	LPTREE		tree;
	WORD		obj;
	WORD		*poffx, *poffy;
{
	OB_TREE = tree;
	OB_OBJ = obj;
	gem_if(OBJC_OFFSET);
	*poffx = OB_XOFF;
	*poffy = OB_YOFF;
	return((WORD) RET_CODE );
}
