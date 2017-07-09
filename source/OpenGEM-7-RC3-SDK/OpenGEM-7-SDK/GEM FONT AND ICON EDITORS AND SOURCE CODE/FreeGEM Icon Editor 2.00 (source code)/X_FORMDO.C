/************************************************************************

    EDICON 2.00 - icon editor for GEM

    Copyright (C) 1998,1999,2000  John Elliott <jce@seasip.demon.co.uk>

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



#include "ppdgem.h"
#include "edicon.h"
#include "ediconf.h"
#include "iconfile.h"
#include "farmem.h"
#include "editor.h"

#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>


UWORD x_mx, x_my, x_mb;
extern WORD xedit, yedit, wedit, hedit;


	WORD
find_obj(tree, start_obj, which)       /* routine to find the next editable */
	REG LPTREE	tree;          /* text field, or a field that is as */
	WORD		start_obj;     /* marked as a default return field. */
	WORD		which;
{
	REG WORD	obj, flag, theflag, inc;

	obj = 0;
	flag = EDITABLE;
	inc = 1;
	switch(which)
	{
	  case FMD_BACKWARD:
		inc = -1;
						/* fall thru		*/
	  case FMD_FORWARD:
		obj = start_obj + inc;
		break;
	  case FMD_DEFLT:
		flag = DEFAULT;
		break;
	}

	while ( obj >= 0 )
	{
	  theflag = tree[obj].ob_flags;
	  if (theflag & flag)
	    return(obj);
	  if (theflag & LASTOB)
	    obj = -1;
	  else
	    obj += inc;
	}
	return(start_obj);
}


	WORD
fm_inifld(tree, start_fld)
	LONG		tree;
	WORD		start_fld;
{
						/* position cursor on	*/
						/*   the starting field	*/
	if (start_fld == 0)
	  start_fld = find_obj(tree, 0, FMD_FORWARD);
	return( start_fld );
}


WORD x_form_do(REG LPTREE tree, WORD start_fld, LPOBJ pOb, EDITOR_DOC *pEd)
{
	REG WORD	edit_obj;
	WORD		next_obj;
	WORD		which, cont;
	WORD		idx;
	UWORD		ks, kr, br;

	wind_update(1);
	wind_update(3);
						/* set starting field	*/
						/*   to edit, if want	*/
						/*   first editing field*/
						/*   then walk tree	*/
	next_obj = fm_inifld(tree, start_fld);
	edit_obj = 0;
						/* interact with user	*/
	cont = TRUE;
	while(cont)
	{
						/* position cursor on	*/
						/*   the selected 	*/
						/*   editting field	*/
	  if ( (next_obj != 0) &&
	       (edit_obj != next_obj) )
	  {
	    edit_obj = next_obj;
	    next_obj = 0;
	    objc_edit(tree, edit_obj, 0, &idx, EDINIT);
	  }
						/* wait for mouse or key */
	  which = evnt_multi(MU_KEYBD | MU_BUTTON, 
			0x02, 0x01, 0x01, 
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			0x0L,
			0, 0,
			&x_mx, &x_my, &x_mb, 
			&ks,   &kr,   &br);
						/* handle keyboard event*/
	  if (which & MU_KEYBD)
	  {
	    cont = form_keybd(tree, edit_obj, next_obj, kr, &next_obj, (WORD *)&kr);
	    if (kr)
	      objc_edit(tree, edit_obj, kr, &idx, EDCHAR);
	  }
						/* handle button event	*/
	  if (which & MU_BUTTON)
	  {
	    next_obj = objc_find(tree, ROOT, MAX_DEPTH, x_mx, x_my);
 	    if (next_obj == NIL)
	    {
	      /* sound(TRUE, 440, 2); */
	      next_obj = 0;
	    }
	    else
	    {
		/* [JCE] handle clicks on the edit panel entirely ourselves */
		  if (next_obj == EDITOR) 
		  {
			  //frob_bitmap(pEd, pOb, 1);
			  /* It crashes if I don't dispatch the event with form_button()
			   * so I'll send it to something harmless */
			  cont = form_button(tree, PREVWICN, br, &next_obj); 
		  }   
	      else cont = form_button(tree, next_obj, br, &next_obj);
	    }
	  }
						/* handle end of field	*/
						/*   clean up		*/
	  if ( (!cont) ||
	       ((next_obj != 0) && 
		(next_obj != edit_obj)) )

	  {
	    objc_edit(tree, edit_obj, 0, &idx, EDEND);
	  }
	}

	wind_update(2);
	wind_update(0);
						/* return exit object	*/
						/*   hi bit may be set	*/
						/*   if exit obj. was	*/
						/*   double-clicked	*/
	return(next_obj);
}




