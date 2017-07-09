

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
objc_draw(tree, drawob, depth, xc, yc, wc, hc)
	LPTREE		tree;
	WORD		drawob, depth;
	WORD		xc, yc, wc, hc;
{
	OB_TREE = tree;
	OB_DRAWOB = drawob;
	OB_DEPTH = depth;
	OB_XCLIP = xc;
	OB_YCLIP = yc;
	OB_WCLIP = wc;
	OB_HCLIP = hc;
	return( gem_if( OBJC_DRAW ) );
}
