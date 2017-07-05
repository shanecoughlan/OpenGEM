

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
graf_watchbox(tree, obj, instate, outstate)
	LPTREE		tree;
	WORD		obj;
	UWORD		instate, outstate;
{
	GR_TREE = tree;
	GR_OBJ = obj;
	GR_INSTATE = instate;
	GR_OUTSTATE = outstate;
	return( gem_if( GRAF_WATCHBOX ) );
}
