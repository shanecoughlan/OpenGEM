/************************************************************************

    SYSFONT v1.02 - system font editor for GEM

    Copyright (C) 2002  John Elliott <jce@seasip.demon.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*************************************************************************/

#include "sysfonti.h"
#define R_STRING 5


/* The adaptive resource beautifier (which makes checkboxes look right on
 * all GEM versions, removes underlines when not required, etc.) */

typedef WORD (*MAPROUTINE)(LPTREE tr, WORD obj);

MLOCAL WORD ini_tree(WORD which);
MLOCAL VOID map_tree(LPTREE tree, WORD first, WORD last, MAPROUTINE routine);
MLOCAL WORD menu_cleanup(LPTREE tr, WORD obj);

WORD rsrc_init(LPBYTE filename, WORD first_tree, WORD last_tree, WORD menu)
{
	WORD n;
	LPTREE tree;
	
	if (!rsrc_load(filename)) return 0;

	for (n = first_tree; n <= last_tree; n++) ini_tree(n);

	if (!(g_xbuf.abilities & ABLE_X3D)) 
	{
		rsrc_gaddr(R_TREE, menu, (LPVOID *)&tree) ;
		if (tree) map_tree(tree, ROOT, NIL, menu_cleanup);
	}
	return 1;
}



/*  If the AES supports checkboxes and radio buttons, implement them. 
 * 
 *  This is done simply by changing buttons with the DRAW3D and WHITEBAK
 *  flags to strings. That way, the controls look like buttons in older
 *  GEMs and like checkboxes in new ones. 
 * 
 */
	
#define EXT3D (DRAW3D | WHITEBAK)

/* The FreeGEM checkboxes and radio buttons are based on those in GEM/5.
 * This program will detect GEM/5 or FreeGEM and act accordingly.
 */

	
	
WORD make_cbox(LPTREE tr, WORD obj)
{	
	if ((tr[obj].ob_state & EXT3D) == EXT3D)
	{
		if (tr[obj].ob_type == G_BUTTON) 
		{
			tr[obj].ob_type = G_STRING;
		}

		/* The little pseudo-buttons on the "file info" page are not 
		 * buttons, but boxes. So rather than make them strings, just
		 * take their borders and 3D away. */
		if (tr[obj].ob_flags & FLAG3D)
		{
			tr[obj].ob_flags &= ~FLAG3D;
			tr[obj].ob_flags &= ~USECOLORCAT;
			tr[obj].ob_spec = (LPVOID) (((LONG)tr[obj].ob_spec) & 0xFF00FFFFL);
			if (gem5) 
			{
				tr[obj].ob_state |= CROSSED;
				tr[obj].ob_type   = G_STRING;
				tr[obj].ob_spec   = ADDR(" ");
			}
		}
	}
	else if (tr[obj].ob_type == G_BUTTON)
	{
		tr[obj].ob_state |= DRAW3D;
	}

	/* Do the GEM/5 3D bits */
	if (gem5) 
	{
		if (tr[obj].ob_flags & FLAG3D)
		{
			tr[obj].ob_state |= CROSSED;
	
			if (tr[obj].ob_type == G_BOX  || tr[obj].ob_type == G_BOXCHAR)
			{
				tr[obj].ob_spec = (LPVOID)((((LONG)tr[obj].ob_spec) & 0xFFFFFF80L) | 0x7E);
				tr[obj].ob_flags |= OUTLINED;
			}
		}
		if (tr[obj].ob_state & EXT3D)
		{
			tr[obj].ob_state |= CROSSED;
		}

		if (tr[obj].ob_type == G_BOX && tr[obj].ob_state & SHADOWED)
		{
			tr[obj].ob_state |= CROSSED | CHECKED;
			tr[obj].ob_state &= ~SHADOWED;	
			tr[obj].ob_spec = (LPVOID)((((LONG)tr[obj].ob_spec) & 0xFFFFFF80L) | 0x7E);
		}

		if (tr[obj].ob_type == G_FTEXT || tr[obj].ob_type == G_FBOXTEXT)
		{
			tr[obj].ob_type = G_FBOXTEXT;
			tr[obj].ob_state |= CROSSED | OUTLINED | CHECKED;
		}
/* Title bars */
		if (tr[obj].ob_type == G_BOXTEXT && ((LPTEDI)tr[obj].ob_spec)->te_color == 0x1191  )
		{
		 ((LPTEDI)tr[obj].ob_spec)->te_color = 0x1A7B;
			
		}
/* Entry fields */
		if (tr[obj].ob_type == G_TEXT || tr[obj].ob_type == G_BOXTEXT)
		{
			tr[obj].ob_type = G_BOXTEXT;
			tr[obj].ob_state |= CROSSED | OUTLINED | CHECKED;
		}
	}
	
}


/* Remove underlines from a string, moving all the other characters up
 * and putting spaces at the end */
MLOCAL VOID chop_underline(LPBYTE txt)
{
	LPBYTE txt2;
	while (*txt)
	{
		if (*txt == '_')
		{
			txt2 = txt;
			while (txt2[1])
			{
				txt2[0] = txt2[1];
				++txt2;	
			}
			txt2[0] = ' ';
		}
		++txt;
	}
}



MLOCAL WORD gem3_cleanup(LPTREE tr, WORD obj)
{	
	if (tr[obj].ob_type == G_BUTTON)
	{
		chop_underline(tr[obj].ob_spec);		
	}

	/* Shrink "extended 3D" buttons (checkboxes / radio buttons) a bit */
	if ((tr[obj].ob_state & EXT3D) == EXT3D)
	{
		tr[obj].ob_y += 2;
		tr[obj].ob_x += 2;
		tr[obj].ob_height -= 4;
		tr[obj].ob_width  -= 4;
	}
}


MLOCAL WORD menu_cleanup(LPTREE tr, WORD obj)
{	
	if (tr[obj].ob_type == G_STRING || tr[obj].ob_type == G_TITLE)
	{
		chop_underline(tr[obj].ob_spec);		
	}
}


	

	
static WORD ini_tree(WORD which)		/* find tree address */
{
	WORD w;
	LPTREE tree;

	w = rsrc_gaddr(R_TREE, which, (LPVOID *)&tree) ;
	
	if (!tree) return w;

	if (gem5 == -1)
	{
		char name[40];
		WORD attrib[10];

		/* First check for the ViewMAX/2 driver, which also fails on 
		 * vqt_name() */
		if (vqt_attributes(vdi_handle, attrib))
		{	
			vqt_name(vdi_handle, 1, name);
			if (name[0]) gem5 = 0; else gem5 = 1;
		}
		else gem5 = 0;
	}

	/* Remove underlines from buttons if not in FreeGEM */
	if (!(g_xbuf.abilities & ABLE_X3D)) 
	{
		map_tree(tree, ROOT, NIL, gem3_cleanup);
	}
	if ((g_xbuf.abilities & ABLE_X3D) || gem5)
	{
		map_tree(tree, ROOT, NIL, make_cbox);
	}
	return w;
}


	

	
static VOID map_tree(LPTREE tree, WORD first, WORD last, MAPROUTINE routine)
{
	REG WORD tmp1;
	REG WORD this = first;
	
						/* non-recursive tree	*/
						/*   traversal		*/
child:
						/* see if we need to	*/
						/*   to stop		*/
	if ( this == last)
	  return;

						/* do this object	*/
	(*routine)(tree, this);
						/* if this guy has kids	*/
						/*   then do them	*/
	tmp1 = tree[this].ob_head;
	
	if ( tmp1 != NIL )
	{
	  this = tmp1;
	  goto child;
	}
sibling:
						/* if this obj. has a	*/
						/*   sibling that is not*/
						/*   his parent, then	*/
						/*   move to him and do	*/
						/*   him and his kids	*/
	tmp1 = tree[this].ob_next;
	if ( (tmp1 == last) ||
	     (tmp1 == NIL) )
	  return;

	if ( tree[tmp1].ob_tail != this )
	{
	  this = tmp1;
	  goto child;
	}
						/* if this is the root	*/
						/*   which has no parent*/
						/*   then stop else 	*/
						/*   move up to the	*/
						/*   parent and finish	*/
						/*   off his siblings	*/ 
	this = tmp1;
	goto sibling;
}



