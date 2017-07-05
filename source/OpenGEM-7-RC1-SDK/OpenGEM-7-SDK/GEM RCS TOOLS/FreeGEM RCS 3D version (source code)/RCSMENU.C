/*	RCSMENU.C	 1/27/85 - 1/25/85 	Tim Oren		*/
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
#include "ppdrcs.h"


WORD in_bar(LPTREE tree, WORD obj)
	{
	WORD	nobj;

	if (tree[ROOT].ob_head != get_parent(tree, nobj = get_parent(tree, obj)))
		return (FALSE);
	return (child_of(tree, nobj, obj));
	}


	
WORD in_menu(LPTREE tree, WORD obj)
	{
	WORD	nobj;

	if (tree[ROOT].ob_tail != get_parent(tree, nobj = get_parent(tree, obj)))
		return (FALSE);
	return (child_of(tree, nobj, obj));
	}

WORD is_menu(LPTREE tree, WORD obj)
	{
	WORD	nobj;

	if (tree[ROOT].ob_tail != (nobj = get_parent(tree, obj))) return (FALSE);
	return (child_of(tree, nobj, obj));
	}

WORD in_which_menu(LPTREE tree, WORD obj)
	{
	return is_menu(tree, get_parent(tree, obj));
	}


	
WORD menu_n(LPTREE tree, WORD n)
	{
	WORD	nobj;

	nobj = tree[ROOT].ob_tail;
	return (nth_child(tree, nobj, n));
	}

WORD bar_n(LPTREE tree, WORD n)
	{
	WORD	nobj;

	nobj = tree[ROOT].ob_head;
	return (nth_child(tree, tree[nobj].ob_head, n));
	}

	
WORD get_active(LPTREE tree)
	{
	return tree[tree[ROOT].ob_head].ob_head;
	}


	
WORD blank_menu(LPTREE tree, WORD sobj)
	{
	LPTREE 	dobj_ptr;

	blank_obj.ob_width = tree[sobj].ob_width;
	blank_obj.ob_height = gl_hchar;
	dobj_ptr = (LPTREE)get_obmem();
	LBCOPY((LPVOID)dobj_ptr, ADDR(&blank_obj), sizeof(OBJECT) );
	return (WORD) (dobj_ptr - tree);
	}

	
VOID fix_menu_bar(LPTREE tree)
	{
	WORD	the_active, the_menus, the_bar;
	WORD	bar_obj, menu_obj, x, x1, x2;

	the_menus = tree[ROOT].ob_tail;
	the_bar   = tree[ROOT].ob_head;
	tree[the_bar].ob_height = gl_hchar + 2;
	the_active = get_active(tree);
	tree[the_active].ob_y = 0;					/* I'll get you  */
	tree[the_active].ob_height = gl_hchar + 3;	/* for this, Lee */
										
    bar_obj = tree[tree[the_active].ob_head].ob_next; /*skip desk title*/
	menu_obj =tree[tree[the_menus ].ob_head].ob_next; /*skip acc items */
	x = 0;

	for (; bar_obj != the_active; )
		{
		tree[bar_obj].ob_x = x;
		tree[bar_obj].ob_y = 0;
		tree[bar_obj].ob_height = gl_hchar + 3;	/* be sure! */
		x1 = x + 2 * gl_wchar;
		x2 = full.g_w - tree[menu_obj].ob_width;
		tree[menu_obj].ob_x = min(x1, x2) ;
		x += tree[bar_obj].ob_width;
		bar_obj  = tree[bar_obj ].ob_next;
		menu_obj = tree[menu_obj].ob_next;
		}

	tree[the_active].ob_width = x;
	}


	
WORD menu_ok(LPTREE tree, WORD obj)
	{
	WORD	n;

	if (n = in_bar(tree, obj))   return (n == 1? FALSE: TRUE);
	if (n = in_menu(tree, obj))
		{
		if (in_which_menu(tree, obj) != 1) return (TRUE);
		return (n == 1? TRUE: FALSE);
		}
	return (FALSE);
	}
	





VOID desk_menufix(LPTREE tree)
{
	WORD		desktitle, offw, the_active,the_menus, drop_dwn1;
	GRECT		ta, dt, tmp;
      
	the_active = get_active(tree);
	desktitle = tree[the_active].ob_head;
	ob_relxywh(tree, desktitle, &dt);
	ob_relxywh(tree, the_active, &ta);
     						/* if desk title not	*/
						/*   moved then move it	*/
      
	

	if (dt.g_x == 0x0)
	{
     	  offw = 10 * gl_wchar;
	  dt.g_x = gl_width - offw  - ta.g_x -  ta.g_x;
     	  ob_setxywh(tree, desktitle, &dt);
     	  fix_menu_bar(tree);	
     	}		   
	/* move desk menu drop down */	       
	
	the_menus = tree[ROOT].ob_tail;
	drop_dwn1 = tree[the_menus].ob_head;
	ob_relxywh( tree, drop_dwn1, &tmp);
	tmp.g_x = gl_width - tmp.g_w - ta.g_x;
	ob_setxywh(tree, drop_dwn1, &tmp);

	/* make root and bar longer if needed to cover desktitle */
	if ( tree[ROOT  ].ob_width < gl_width ) tree[ROOT  ].ob_width = gl_width;
	if ( tree[THEBAR].ob_width < gl_width ) tree[THEBAR].ob_width = gl_width;
}


VOID undo_menufix(LPTREE tree)
	{
	WORD	the_active, the_menus, the_bar;
	WORD	bar_obj, menu_obj, x, x1, x2;

	the_menus = tree[ROOT].ob_tail;
	the_bar   = tree[ROOT].ob_head;
	tree[the_bar].ob_height = gl_hchar + 2;
	the_active = get_active(tree);
	tree[the_active].ob_y      =  0;			/* I'll get you  */
	tree[the_active].ob_height = gl_hchar + 3;	/* for this, Lee */
										
    bar_obj =  tree[the_active].ob_head;	/*skip desk title*/
	menu_obj = tree[the_menus].ob_head;		/*skip acc items */
	x = 0;

	for (; bar_obj != the_active; )
		{
		tree[bar_obj].ob_x = x;
		tree[bar_obj].ob_y = 0;
		tree[bar_obj].ob_height = gl_hchar + 3;	/* be sure! */
		x1 = x + 2 * gl_wchar;
		x2 = full.g_w - tree[menu_obj].ob_width;
		tree[menu_obj].ob_x = min(x1, x2) ;
		x += tree[bar_obj].ob_width;
		bar_obj  = tree[bar_obj ].ob_next;
		menu_obj = tree[menu_obj].ob_next;
		}

	tree[the_active].ob_width = x;
	}
