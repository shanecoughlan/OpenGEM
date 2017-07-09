/*	GEMBIND.C	9/11/85				Lee Lorenzen	*/
/*	for 3.0		3/11/86 - 8/26/86		MDF		*/
/*	merge source	5/28/87				mdf		*/
/*	menu_click	9/25/87				mdf		*/

/*
*	-------------------------------------------------------------
*	GEM Desktop					  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1985 - 1987		Digital Research Inc.
*	-------------------------------------------------------------
*/

#include <limits.h>	/* for PATH_MAX */
#include <stdlib.h>	/* for malloc */
#include "djgppgem.h"

#include "debug.h"

extern LPWORD pControl, pGlobal, pInt_in, pInt_out, pAddr_in, pAddr_out;
extern LPLPTR pParam;
extern WORD gem_if(WORD);

static LPTREE menubar_16 = 0L;


/* Call with NULL to free the menu bar resource */
WORD dj_menu_bar(OBJECT *tree, WORD showit)
{
	if (menubar_16) dos_free(menubar_16);

	if (tree)
	{
		menubar_16 = dos_alloc(dj_tree_size32(tree));
		if (!menubar_16) return 0;
		dj_tree_32to16(tree, menubar_16);
	}
	else menubar_16 = 0L;

	LSSET(MM_ITREE, menubar_16);
	LWSET(SHOW_IT, showit);
	return( gem_if(MENU_BAR) );
}


WORD dj_menu_icheck(WORD itemnum, WORD checkit)
{
	if (!menubar_16) return 0;

	LSSET(MM_ITREE, menubar_16);
	LWSET(ITEM_NUM, itemnum);
	LWSET(CHECK_IT, checkit);
	return( gem_if(MENU_ICHECK) );
}


WORD dj_menu_ienable(WORD itemnum, WORD enableit)
{
	if (!menubar_16) return 0;

	LSSET(MM_ITREE, menubar_16);
	LWSET(ITEM_NUM, itemnum);
	LWSET(ENABLE_IT, enableit);
	return( gem_if(MENU_IENABLE) );
}


WORD dj_menu_tnormal(WORD titlenum, WORD normalit)
{
	if (!menubar_16) return 0;

	LSSET(MM_ITREE, menubar_16);
	LWSET(TITLE_NUM, titlenum);
	LWSET(NORMAL_IT, normalit);
	return( gem_if( MENU_TNORMAL ) );
}


WORD dj_menu_text(WORD inum, LPBYTE ptext)
{
	if (!menubar_16) return 0;

	LSSET(MM_ITREE, menubar_16);
	LWSET(ITEM_NUM, inum);
	LSSET(MM_PTEXT, ptext);
	return( gem_if( MENU_TEXT ) );
}

#define TREEIN \
	LPTREE tr = dos_alloc(dj_tree_size32(tree)); \
	WORD rv;                                     \
                                                     \
	if (!tr) return 0;                           \
	dj_tree_32to16(tree, tr)                    


#define TREEOUT \
	dj_tree_16to32s(tr, tree); \
	dos_free(tr);              \
	return rv                 

					/* Object Manager		*/
WORD dj_objc_add(OBJECT *tree, WORD parent, WORD child)
{
	TREEIN;

	LSSET(OB_TREE,  tr);
	LWSET(OB_PARENT, parent);
	LWSET(OB_CHILD, child);
	rv = gem_if( OBJC_ADD );

	TREEOUT;
}


WORD dj_objc_delete(OBJECT *tree, WORD delob)
{
	TREEIN;

	LSSET(OB_TREE, tr);
	LWSET(OB_DELOB, delob);
	rv = gem_if( OBJC_DELETE ) ;

	TREEOUT;
}

WORD dj_objc_draw(OBJECT *tree, WORD drawob, WORD depth, 
		WORD xc, WORD yc, WORD wc, WORD hc)
{
	TREEIN;

	LSSET(OB_TREE, tr);
	LWSET(OB_DRAWOB, drawob);
	LWSET(OB_DEPTH, depth);
	LWSET(OB_XCLIP, xc);
	LWSET(OB_YCLIP, yc);
	LWSET(OB_WCLIP, wc);
	LWSET(OB_HCLIP, hc);
	rv = gem_if( OBJC_DRAW ) ;

	TREEOUT;
}


WORD dj_objc_find(OBJECT *tree, WORD startob, WORD depth, WORD mx, WORD my)
{
	TREEIN;

	LSSET(OB_TREE, tr);
	LWSET(OB_STARTOB, startob);
	LWSET(OB_DEPTH, depth);
	LWSET(OB_MX, mx);
	LWSET(OB_MY, my);
	rv = gem_if( OBJC_FIND );

	TREEOUT;
}




WORD dj_objc_offset(OBJECT *tree, WORD obj, WORD *poffx, WORD *poffy)
{
	TREEIN;

	LSSET(OB_TREE, tr);
	LWSET(OB_OBJ,  obj);
	gem_if(OBJC_OFFSET);
	*poffx = OB_XOFF;
	*poffy = OB_YOFF;

	rv = (WORD)RET_CODE;
	TREEOUT;
}


WORD dj_objc_edit(OBJECT *tree, WORD obj, WORD inchar, WORD *idx, WORD kind)
{
	TREEIN;

	LSSET(OB_TREE, tr);
	LWSET(OB_OBJ,  obj);
	LWSET(OB_CHAR, inchar);
	LWSET(OB_IDX, *idx);
	LWSET(OB_KIND, kind);
	gem_if( OBJC_EDIT );
	*idx = OB_ODX;

	rv = (WORD)RET_CODE;
	TREEOUT;
}


WORD dj_objc_change(OBJECT *tree, WORD drawob, WORD depth, WORD xc, WORD yc, 
		 WORD wc, WORD hc, WORD newstate, WORD redraw)
{
	TREEIN;
	
	LSSET(OB_TREE, tr);
	LWSET(OB_DRAWOB, drawob);
	LWSET(OB_DEPTH, depth);
	LWSET(OB_XCLIP, xc);
	LWSET(OB_YCLIP, yc);
	LWSET(OB_WCLIP, wc);
	LWSET(OB_HCLIP, hc);
	LWSET(OB_NEWSTATE, newstate);
	LWSET(OB_REDRAW, redraw);

	rv = gem_if( OBJC_CHANGE ) ;

	TREEOUT;
}



					/* Form Manager			*/
WORD dj_form_do(OBJECT *tree, WORD start)
{
	TREEIN;

	LSSET(FM_FORM, tr);
	LWSET(FM_START, start);
	rv = gem_if( FORM_DO );
	TREEOUT;
}


WORD dj_form_center(OBJECT *tree, WORD *pcx, WORD *pcy, WORD *pcw, WORD *pch)
{
	TREEIN;

	LSSET(FM_FORM, tr);
	gem_if(FORM_CENTER);
	*pcx = FM_XC;
	*pcy = FM_YC;
	*pcw = FM_WC;
	*pch = FM_HC;
	rv = (WORD) RET_CODE;

	TREEOUT;
}



WORD dj_form_keybd(OBJECT *tree, WORD obj, WORD nxt_obj, WORD thechar, 
		WORD *pnxt_obj, WORD *pchar)
{
	TREEIN;

	LSSET(FM_FORM,   tr);
	LWSET(FM_OBJ,    obj);
	LWSET(FM_INXTOB, nxt_obj);
	LWSET(FM_ICHAR,  thechar);
	gem_if( FORM_KEYBD );
	*pnxt_obj = FM_ONXTOB;
	*pchar = FM_OCHAR;
	rv = (WORD) RET_CODE;

	TREEOUT;
}


WORD dj_form_button(OBJECT *tree, WORD obj, WORD clks, WORD *pnxt_obj)
{
	TREEIN;

	LSSET(FM_FORM, tr);
	LWSET(FM_OBJ,  obj);
	LWSET(FM_CLKS, clks);
	gem_if( FORM_BUTTON );
	*pnxt_obj = FM_ONXTOB;
	rv = (WORD) RET_CODE;

	TREEOUT;
}



WORD dj_rsrc_treeaddr(WORD rsid, OBJECT **paddr)
{
	LPTREE tr16;

	LWSET(RS_TYPE, R_TREE);
	LWSET(RS_INDEX, rsid);
	gem_if(RSRC_GADDR);
	tr16 = RS_OUTADDR;

	if (tr16)
	{
		int tz;

		*paddr = malloc(tz = dj_tree_size16(tr16));

		dj_tree_16to32(tr16, *paddr);
	}

	return((WORD) RET_CODE );
}



WORD dj_rsrc_obfix(OBJECT *tree, WORD obj)
{
	TREEIN;

	LSSET(RS_TREE, tr);
	LWSET(RS_OBJ,  obj);
	rv = gem_if(RSRC_OBFIX);
	
	TREEOUT;
}
