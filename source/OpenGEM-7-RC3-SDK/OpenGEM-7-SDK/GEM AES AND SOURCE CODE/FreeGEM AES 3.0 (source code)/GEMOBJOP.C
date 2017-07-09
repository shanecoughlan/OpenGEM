/*	GEMOBJOP.C	03/15/84 - 05/27/85	Gregg Morris		*/
/*	merge High C vers. w. 2.2 		8/21/87		mdf	*/ 
/*	fix get_par				11/12/87	mdf	*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "aes.h"

	LPBYTE 
obaddr(tree, obj, fld_off)
	LPTREE		tree;
	WORD		obj;
	WORD		fld_off;
{
	return( ((LPBYTE)(&tree[obj])) + fld_off );
}
/*
	BYTE
ob_sst(tree, obj, pspec, pstate, ptype, pflags, pt, pth)
	LPTREE		tree;
	WORD		obj;
	REG LONG	*pspec;
	WORD		*pstate, *ptype;
	REG WORD	*pflags;
	REG GRECT	*pt;
	WORD		*pth;
{
	WORD		th;
	OBJECT		tmp;

	LWCOPY(ADDR(&tmp), &tree[obj].ob_next, sizeof(OBJECT)/2);
	pt->g_w = tmp.ob_width;
	pt->g_h = tmp.ob_height;
	*pflags = tmp.ob_flags;
	*pspec = tmp.ob_spec;

	if (*pflags & INDIRECT)
	  *pspec = *(LPLONG)(tmp.ob_spec);

	*pstate = tmp.ob_state;
	*ptype = tmp.ob_type & 0x00ff;
	th = 0;
	switch( *ptype )
	{
	  case G_TITLE:
		th = 1;
		break;
	  case G_TEXT:
	  case G_BOXTEXT:
	  case G_FTEXT:
	  case G_FBOXTEXT:
		th = ((LPTEDI)(*pspec))->te_thickness;
		break;
	  case G_BOX:
	  case G_BOXCHAR:
	  case G_IBOX:
		th = LBYTE2(((BYTE *)pspec));
		break;
	  case G_BUTTON:
		th--;
		if ( *pflags & EXIT)
		  th--;
		if ( *pflags & DEFAULT)
		  th--;
		break;
	}
	if (th > 128)
	  th -= 256;
	*pth = th;
	return(LBYTE3(((BYTE *)pspec)));
}
*/

	BYTE
ob_sst(tree, obj, pspec, pstate, ptype, pflags, pt, pth)
	LPTREE		tree;
	WORD		obj;
	REG LONG	*pspec;
	WORD		*pstate, *ptype;
	REG WORD	*pflags;
	REG GRECT	*pt;
	WORD		*pth;
{
	WORD		th;
	OBJECT far *	tmp;

	tmp = tree + obj ;
	pt->g_w = tmp->ob_width;
	pt->g_h = tmp->ob_height;
	*pflags = tmp->ob_flags;
	*pspec = tmp->ob_spec;

	if (*pflags & INDIRECT)
	  *pspec = *(LONG far *)tmp->ob_spec ;

	*pstate = tmp->ob_state;
	*ptype = tmp->ob_type & 0x00ff;
	th = 0;
	switch( *ptype )
	{
	  case G_TITLE:
		th = 1;
		break;
	  case G_TEXT:
	  case G_BOXTEXT:
	  case G_FTEXT:
	  case G_FBOXTEXT:
		th = (WORD)*(LONG far *)(*pspec+22);
		break;
	  case G_BOX:
	  case G_BOXCHAR:
	  case G_IBOX:
		th = ((*pspec) >> 16 ) & 0xFF ;
		break;
	  case G_BUTTON:
		th--;
		if ( *pflags & EXIT ){
#if 0000
			/* slim down buttons (for nicer 3D effects */
		  th--;
#endif
		  if( *pflags & DEFAULT )
		    th--;
		}
		break;
	}
	if (th > 128)
	  th -= 256;
	*pth = th;
	return( ( *pspec ) >> 24 ) ;
}


int	bpv, obpv;
long lptr;


	VOID
everyobj(tree, this, last, routine, startx, starty, maxdep)
	REG LPTREE	tree;
	REG WORD	this, last;
	EVERYOP		routine;
	WORD		startx, starty;
	WORD		maxdep;
{
	REG WORD	tmp1;
	REG WORD	depth;
	WORD		x[9], y[9];

	
	x[0] = startx;
	y[0] = starty;
	depth = 1;
						/* non-recursive tree	*/
						/*   traversal		*/
child:
						/* see if we need to	*/
						/*   to stop		*/
	if ( this == last)
	  return;
						/* do this object	*/

	x[depth] = x[depth-1] + tree[this].ob_x;
	y[depth] = y[depth-1] + tree[this].ob_y;

	(*routine)(tree, this, x[depth], y[depth]);

						/* if this guy has kids	*/
						/*   then do them	*/
	tmp1 = tree[this].ob_head;

	if ( tmp1 != NIL )
	{
	  if ( !( tree[this].ob_flags & HIDETREE ) && 
		( depth <= maxdep ) )
	  {
	    depth++;
	    this = tmp1;
	    goto child;
	  }
	}

sibling:

						/* if this is the root	*/
						/*   which has no parent*/
						/*   or it is the last	*/
						/*   then stop else... 	*/
	tmp1 = tree[this].ob_next;
	if ( (tmp1 == last) ||
	     (this == ROOT) )	
	  {
	  return;
	  }
						/* if this obj. has a	*/
						/*   sibling that is not*/
						/*   his parent, then	*/
						/*   move to him and do	*/
						/*   him and his kids	*/
	if ( tree[tmp1].ob_tail != this )
	{
	  this = tmp1;
	  goto child;
	}
						/* else move up to the	*/
						/*   parent and finish	*/
						/*   off his siblings	*/ 
	depth--;
	this = tmp1;
	goto sibling;
}


/*
*	Routine that will find the parent of a given object.  The
*	idea is to walk to the end of our siblings and return
*	our parent.  If object is the root then return NIL as parent.
*	Also have this routine return the immediate next object of
*	this object.
*
*

*/
	WORD
get_par(tree, obj, pnobj)
	REG LPTREE	tree;
	REG WORD	obj;
	WORD		*pnobj;
{
	REG WORD	pobj;
	REG WORD	nobj;

	pobj = obj;
	nobj = NIL;
	if (obj == ROOT)
	  pobj = NIL;
	else
	{
	  do
	  {
	    obj = pobj;
	    pobj = tree[obj].ob_next;
	    if (nobj == NIL)
	      nobj = pobj;
	  } while ( (pobj >= ROOT) && (tree[pobj].ob_tail != obj) );
	}
	*pnobj = nobj;
	return(pobj);
} /* get_par */



