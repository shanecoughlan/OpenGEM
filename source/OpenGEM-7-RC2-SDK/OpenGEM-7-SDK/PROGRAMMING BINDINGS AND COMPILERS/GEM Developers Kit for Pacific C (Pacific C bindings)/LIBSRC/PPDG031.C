

#include "ppdgem.h"
#include "ppdg0.h"



	WORD
objc_change(tree, drawob, depth, xc, yc, wc, hc, newstate, redraw)
	LPTREE		tree;
	WORD		drawob, depth;
	WORD		xc, yc, wc, hc;
	WORD		newstate, redraw;
{
	OB_TREE = tree;
	OB_DRAWOB = drawob;
	OB_DEPTH = depth;
	OB_XCLIP = xc;
	OB_YCLIP = yc;
	OB_WCLIP = wc;
	OB_HCLIP = hc;
	OB_NEWSTATE = newstate;
	OB_REDRAW = redraw;
	return( gem_if( OBJC_CHANGE ) );
}
