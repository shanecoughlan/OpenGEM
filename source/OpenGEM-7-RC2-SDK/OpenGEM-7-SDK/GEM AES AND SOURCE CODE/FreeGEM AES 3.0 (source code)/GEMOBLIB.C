/*	GEMOBLIB.C	03/15/84 - 05/27/85	Gregg Morris		*/
/*	merge High C vers. w. 2.2 		8/21/87		mdf	*/ 
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


						/* in GSXBIND.C		*/
#define vsf_color( x )		gsx_1code(S_FILL_COLOR, x)
						/* in GLOBE.C		*/
						/* in GSXIF.C		*/
EXTERN VOID	gsx_moff();
EXTERN VOID	gsx_mon();
EXTERN WORD	gsx_chkclip();

/* ------------- added for metaware compiler --------- */
GLOBAL VOID	ob_actxywh();
GLOBAL VOID 	ob_relxywh();
GLOBAL VOID	ob_offset();
EXTERN VOID 	r_set();
EXTERN WORD 	strlen();
EXTERN BYTE 	ob_sst();			/* in OBJOP.C 		*/
EXTERN VOID 	everyobj();
EXTERN WORD 	get_par();
EXTERN VOID 	gsx_gclip();			/* in GRAF.C		*/
EXTERN VOID 	gsx_cline();
EXTERN VOID 	gsx_attr();
EXTERN VOID	gsx_blt();
EXTERN VOID 	gr_inside();
EXTERN VOID 	gr_crack();
EXTERN VOID	gr_box();
EXTERN VOID	gr_rect();
EXTERN VOID 	gr_gtext();
EXTERN VOID	gr_gicon();
EXTERN VOID 	gsx_tblt();
EXTERN VOID	bb_fill();
EXTERN VOID     gsx_1code();			/* in GSXIF.C		*/
/* ------------------------------------------- */

EXTERN THEGLO	D;


GLOBAL TEDINFO	edblk;
GLOBAL BITBLK	bi;
GLOBAL ICONBLK	ib;


WORD get_prev(REG LPTREE tree, WORD parent, REG WORD obj);


/*
*	Routine to take an unformatted raw string and based on a
*	template string build a formatted string.
*/
VOID ob_format(WORD just, BYTE *raw_str, BYTE *tmpl_str, BYTE *fmt_str)
{
	REG BYTE	*pfbeg, *ptbeg, *prbeg;
	BYTE		*pfend, *ptend, *prend;
	REG WORD	inc, ptlen, prlen;
/* Done to track down a bug in Desktop
	GRECT rc, rc2;
	gsx_gclip(&rc2);
	gsx_sclip(&gl_rfull);
	rc.g_x = 0; rc.g_w = 640; rc.g_h = 16;
	gsx_attr(1, MD_REPLACE, BLUE);
	rc.g_y = 16; gr_gtext(0, IBM, raw_str, &rc);
	rc.g_y = 32; gr_gtext(0, IBM, tmpl_str, &rc);
	rc.g_y = 48; gr_gtext(0, IBM, fmt_str, &rc);
#asm
	mov	ah,#0
	int	#0x16
#endasm
*/	
	if (*raw_str == '@')
	  *raw_str = NULL;

	pfbeg = fmt_str;
	ptbeg = tmpl_str;
	prbeg = raw_str;

	ptlen = strlen(tmpl_str);
	prlen = strlen(raw_str);

	inc = 1;
	pfbeg[ptlen] = NULL;
	if (just == TE_RIGHT)
	{
	  inc = -1;
	  pfbeg = pfbeg + ptlen - 1;
	  ptbeg = ptbeg + ptlen - 1;
	  prbeg = prbeg + prlen - 1;
	}

	pfend = pfbeg + (inc * ptlen);
	ptend = ptbeg + (inc * ptlen);
	prend = prbeg + (inc * prlen);

	while( ptbeg != ptend )
	{
	  if ( *ptbeg != '_')
	    *pfbeg = *ptbeg;
	  else
	  {
	    if ( prbeg != prend )
	    {
	      *pfbeg = *prbeg;
	      prbeg += inc;
	    }
	    else
	      *pfbeg = '_';
	  } /* else */
	  pfbeg += inc;
	  ptbeg += inc;
	} /* while */
/*
	gsx_attr(1, MD_REPLACE, RED);
	rc.g_x = 320;
	rc.g_y = 16; gr_gtext(0, IBM, raw_str, &rc);
	rc.g_y = 32; gr_gtext(0, IBM, tmpl_str, &rc);
	rc.g_y = 48; gr_gtext(0, IBM, fmt_str, &rc);
	gsx_sclip(&rc2);
#asm
	mov	ah,#0
	int	#0x16
#endasm
	*/
} /* ob_format */



/*
*	Routine to load up and call a user defined object draw or change 
*	routine.
*/

	WORD
ob_user(tree, obj, pt, spec, curr_state, new_state)
	LPTREE		tree;
	WORD		obj;
	GRECT		*pt;
	LPUSER		spec;
	WORD		curr_state;
	WORD		new_state;
{
	PARMBLK		pb;

	pb.pb_tree = tree;
	pb.pb_obj = obj;
	pb.pb_prevstate = curr_state;
	pb.pb_currstate = new_state;
	rc_copy(pt, (GRECT *)&pb.pb_x); 
	gsx_gclip((GRECT *)&pb.pb_xc);
	pb.pb_parm = spec->ub_parm;
	return(  far_call(spec->ub_code, (LONG)ADDR(&pb)) );
}

	VOID
ob_highlight( tree, obj )
	LPTREE	tree;
	WORD	obj;
{
	WORD	state, obtype, flags, th;
	LONG	spec;
	GRECT 	t;

	ob_sst(tree, obj, &spec, &state, &obtype, &flags, &t, &th);

	ob_actxywh(tree, obj, &t);

	if ( obtype!=G_USERDEF ) 
	{
		draw_hilite(obtype, &t);
	}
}

/*
*	Routine to draw an object from an object tree.
*/
	VOID
just_draw(tree, obj, sx, sy)
	REG LPTREE	tree;
	REG WORD	obj;
	REG WORD	sx, sy;
{
	WORD		bcol, tcol, ipat, icol, tmode, th;
	WORD		state, obtype, flags;
	LONG		spec;
	WORD		tmpth;
#if RED_UNDERLINES	
	BYTE		ch,n;
#else /* RED_UNDERLINES */
	BYTE		ch,n,sav_ch;
#endif /* RED_UNDERLINES */
	GRECT		t, c;
	WORD		doit3d;
	BYTE		sav[80];
	
	ch = ob_sst(tree, obj, &spec, &state, &obtype, &flags, &t, &th);


	if((obtype==G_BUTTON) || (flags & FLAG3D))
	{	
		doit3d = TRUE; 
		flags |= USECOLORCAT;
	} 
	else if (flags & FL3DBAK)
	{
		doit3d = -1;
		flags |= USECOLORCAT;
	}
	else	doit3d = FALSE;

	if ( (flags & HIDETREE) ||
	     (spec == -1L) )
	  return;

	t.g_x = sx;
	t.g_y = sy;
						/* do trivial reject	*/
						/*  with full extent	*/
						/*  including, outline, */
						/*  shadow, & thickness	*/
	if (gl_wclip && gl_hclip)
	{
	  rc_copy(&t, &c);
//	  memmove( (void *)&c, (void *)&t, sizeof( GRECT ) ) ;
	  if (state & OUTLINED)
	    gr_inside(&c, -5);
	  else
	    gr_inside(&c, ((th < 0) ? (5 * th) : (-5 * th)) );
	
	  if ( !(gsx_chkclip(&c)) )
	    return;
	}
						/* for all tedinfo	*/
						/*   types get copy of	*/
						/*   ted and crack the	*/
						/*   color word and set	*/
						/*   the text color	*/
	if ( obtype != G_STRING )
	{
	  tmpth = (th < 0) ? 0 : th;
	  tmode = MD_REPLACE;
	  tcol = BLACK;
	  switch( obtype )
	  {
	    case G_BOXTEXT:
	    case G_FBOXTEXT:
	    case G_TEXT:
	    case G_FTEXT:
		  LBCOPY((BYTE FAR *)ad_edblk, (BYTE FAR *)spec, 
			  sizeof(TEDINFO));
		  gr_crack(edblk.te_color, &bcol,&tcol, &ipat, &icol, &tmode);
		break;
	  }
						/* for all box types	*/
						/*   crack the color 	*/
						/*   if not ted and	*/
						/*   draw the box with	*/
						/*   border		*/
	  switch( obtype )
	  {
	    case G_BOX:
	    case G_BOXCHAR:
	    case G_IBOX:
		gr_crack(FP_OFF(spec), &bcol, &tcol, &ipat, &icol, &tmode);
	    case G_BUTTON:
		if (obtype == G_BUTTON )
		{
#if 000
		  gsx_attr(FALSE, MD_REPLACE, WHITE); /* Erase any previous box */
		  gr_box(t.g_x, t.g_y, t.g_w, t.g_h, -3);
#endif
		  icol = CC_BUTTON;
		}
	    case G_BOXTEXT:
	    case G_FBOXTEXT:

		if( doit3d )
		  gr_3dbox( &t, th, !(state & WHITEBAK), (doit3d < 0));		/*910326WHF*/
		else
		{				/* draw box's border	*/
		  if ( th != 0 )
		  {
		    gsx_attr(FALSE, MD_REPLACE, bcol);
		    gr_box(t.g_x, t.g_y, t.g_w, t.g_h, th);
		  }
		}
						/* draw filled box	*/
		if( obtype != G_IBOX )
		{
		    gr_inside(&t, tmpth); 
		    if( doit3d && !(state & WHITEBAK)) 
		    {
				/* If 3D border, shrink the interior; but if WHITEBAK is
				 * set, don't because the border moves out again. */
			    
			    gr_inside(&t, 1);	
			}
		    if( flags & USECOLORCAT )	/*910327WHF*/
		    {
				grcc_rect( icol, &t );
				if( doit3d & !gl_domono )
				{
					if (flags & FL3DBAK)
					{
						inner_dots( &t, cc_s[CC_3DSHADE].cc_background,
					    	            cc_s[CC_3DSHADE].cc_foreground);

					}
					else
					{ 
						inner_dots( &t, cc_s[CC_3DSHADE].cc_foreground,
					    	            cc_s[CC_3DSHADE].cc_background);
					}
				}
		    }
		    else gr_rect(icol, ipat, &t); 
		    if( doit3d && !(state & WHITEBAK)) gr_inside(&t, -1);	/* acct for 3d brdr */
		    gr_inside(&t, -tmpth); 
		}
		break;
	  }
	  gsx_attr(TRUE, tmode, tcol);
						/* do whats left for	*/
						/*   all the other types*/
	  switch( obtype )
	  {
	    case G_FTEXT:
	    case G_FBOXTEXT:
		LSTCPY((BYTE FAR *)ad_rawstr, (BYTE FAR *)edblk.te_ptext);
		LSTCPY((BYTE FAR *)ad_tmpstr, (BYTE FAR *)edblk.te_ptmplt);
		LSTCPY((BYTE FAR *)ad_valstr, (BYTE FAR *)edblk.te_pvalid);
		if( flags & EDITABLE ){	/* Block out a silent (no-show) field*/
		  for( n=0 ; D.g_rawstr[n] ; n++ ){
		    sav[n]=D.g_rawstr[n];
		    if( toupper(D.g_valstr[n])=='S' )
		      ((unsigned char *)D.g_rawstr)[n]=0xFE;
		  }
		}
		ob_format(edblk.te_just, &D.g_rawstr[0], &D.g_tmpstr[0], 
			&D.g_fmtstr[0]);
						/* fall thru to gr_gtext*/
	    case G_BOXCHAR:
		edblk.te_ptext = ad_fmtstr;
		if (obtype == G_BOXCHAR)
		{
		  D.g_fmtstr[0] = ch;
		  D.g_fmtstr[1] = NULL;
		  edblk.te_just = TE_CNTR;
		  edblk.te_font = IBM;
		}
						/* fall thru to gr_gtext*/
	    case G_TEXT:
	    case G_BOXTEXT:
		gr_inside(&t, tmpth);
		gr_gtext(edblk.te_just, edblk.te_font, edblk.te_ptext, 
				&t);
		gr_inside(&t, -tmpth);
		if( (flags & EDITABLE) && 
			(obtype==G_FTEXT || obtype==G_FBOXTEXT) ){
		  for( n=0 ; D.g_rawstr[n] ; n++ ){
		    edblk.te_ptext[n] = sav[n];
		  }
		}
		break;
	    case G_IMAGE:
		LBCOPY((BYTE FAR *)ad_bi, (BYTE FAR *)spec, sizeof(BITBLK));
		gsx_blt(bi.bi_pdata, bi.bi_x, bi.bi_y, bi.bi_wb,
				0x0L, t.g_x, t.g_y, gl_width/8, bi.bi_wb * 8,
				bi.bi_hl, MD_TRANS, bi.bi_color, WHITE);
		break;
	    case G_ICON:
		LBCOPY((BYTE FAR *)&ib, (BYTE FAR *)spec, sizeof(ICONBLK));
		ib.ib_xicon += t.g_x;
		ib.ib_yicon += t.g_y; 
		ib.ib_xtext += t.g_x;
		ib.ib_ytext += t.g_y; 
		gr_gicon(state, ib.ib_pmask, ib.ib_pdata, ib.ib_ptext,
		  ib.ib_char, ib.ib_xchar, ib.ib_ychar,
		  (GRECT*)&ib.ib_xicon, (GRECT*)&ib.ib_xtext);
		state &= ~SELECTED;
		break;
	    case G_CLRICN:

		LBCOPY((BYTE FAR *)&ib, (BYTE FAR *)spec, sizeof(ICONBLK));
		ib.ib_xicon += t.g_x;
		ib.ib_yicon += t.g_y; 
		ib.ib_xtext += t.g_x;
		ib.ib_ytext += t.g_y; 
		gr_clricon(state, ib.ib_pmask, ib.ib_pdata, ib.ib_ptext,
		  ib.ib_char, ib.ib_xchar, ib.ib_ychar,
		  (GRECT*)&ib.ib_xicon, (GRECT*)&ib.ib_xtext);
		state &= ~SELECTED;
		break;
	    case G_USERDEF:
		state = ob_user(tree, obj, &t, spec, state, state);
		break;
	    case G_DTMFDB:

#if EMSDESK
//#asm
//	    int	#3
//#endasm
	    /* Check for a desktop MFDB in EMS */
		if (gl_ems_avail && (spec == DTMFDB_EMS_SPEC)) 
	    {
			WORD npgs = gl_emm_inuse;
			WORD page;
			
		    for (page = 0; page < npgs; page++)
		    {
				if (!EMS_Map(gl_emm_handle, page, page)) 
				{ 
					npgs = 0;
					break;
				} 
		    }
		    if (npgs)
		    {
			    FDB FAR *fdb = MK_FP(gl_emm_seg, 0);
			    gr_dtmfdb(fdb, &t);
		    }
	    }
		else
#endif /* EMSDESK */
			gr_dtmfdb((FDB FAR *)spec, &t);
		break;
	  } /* switch type */
	}
/* Split in 3 to spare the PPD optimiser - see XSTATE.C */

	  ystate(obtype, spec, &t, tree, obj);
	
	  xstate(obtype, state, doit3d, th, &t, flags);

} /* just_draw */



/*
*	Object draw routine that walks tree and draws appropriate objects.
*/
	VOID
ob_draw(tree, obj, depth)
	REG LPTREE	tree;
	WORD		obj, depth;
{
	WORD		last, pobj;
	WORD		sx, sy;

/* Defensive code... */
#if DEBUG
	if (tree == 0) 
	{
#asm
		int	#3
#endasm
	}
#endif
	
	
	pobj = get_par(tree, obj, &last);

	if (pobj != NIL)
	  ob_offset(tree, pobj, &sx, &sy);
	else
	  sx = sy = 0;

	gsx_moff();
	everyobj(tree, obj, last, just_draw, sx, sy, depth);
	gsx_mon();
}


/*
*	Routine to find out which object a certain mx,my value is
*	over.  Since each parent object contains its children the
*	idea is to walk down the tree, limited by the depth parameter,
*	and find the last object the mx,my location was over.
*/

/************************************************************************/
/* o b _ f i n d							*/
/************************************************************************/
	WORD
ob_find(tree, currobj, depth, mx, my)
	REG LPTREE	tree;
	REG WORD	currobj;
	REG WORD	depth;
	WORD		mx, my;
{
	WORD		lastfound;
	WORD		dosibs, done, junk;
	GRECT		t, o;
	WORD		parent, childobj, flags;

	lastfound = NIL;

	if (currobj == 0)
	  r_set(&o, 0, 0, 0, 0);
	else
	{
	  parent = get_par(tree, currobj, &junk);
	  ob_actxywh(tree, parent, &o);
	}
	
	done = FALSE;
	dosibs = FALSE;

	while( !done )
	{
						/* if inside this obj,	*/
						/*   might be inside a	*/
						/*   child, so check	*/
	  ob_relxywh(tree, currobj, &t);
	  t.g_x += o.g_x;
	  t.g_y += o.g_y;

	  flags = tree[currobj].ob_flags;
	  if ( (inside(mx, my, &t)) &&
	       (!(flags & HIDETREE)) )
	  {
	    lastfound = currobj;

	    childobj = tree[currobj].ob_tail;
	    if ( (childobj != NIL) && depth)
	    {
	      currobj = childobj;
	      depth--;
	      o.g_x = t.g_x;
	      o.g_y = t.g_y;
	      dosibs = TRUE;
	    }
	    else
	      done = TRUE;
	  }
	  else
	  {
	    if ( (dosibs) &&
	         (lastfound != NIL) )
	    {
	        currobj = get_prev(tree, lastfound, currobj);
	        if (currobj == NIL)
	          done = TRUE;
	    }
	    else
	      done = TRUE;
	  }
	}
						/* if no one was found	*/
						/*   this will return	*/
						/*   NIL		*/
	return(lastfound);
} /* ob_find */


/*
*	Routine to add a child object to a parent object.  The child
*	is added at the end of the parent's current sibling list.
*	It is also initialized.
*/
	VOID
ob_add(tree, parent, child)
	REG LPTREE	tree;
	REG WORD	parent, child;
{
	REG WORD	lastkid;
	REG LPWORD	ptail;

	if ( (parent != NIL) &&
	     (child != NIL) )
	{
						/* initialize child	*/
	  tree[child].ob_next = parent;

	  ptail = &tree[parent].ob_tail;
	  lastkid = *ptail;
	  if (lastkid == NIL)
						/* this is parent's 1st	*/
						/*   kid, so both head	*/
						/*   and tail pt to it	*/
	    tree[parent].ob_head = child;
	  else
						/* add kid to end of 	*/
						/*   kid list		*/
	    tree[lastkid].ob_next = child;
	  *ptail = child;
	}
	
} /* ob_add */

/*
*	Routine to delete an object from the tree.
*/
	VOID
ob_delete(tree, obj)
	REG LPTREE	tree;
	REG WORD	obj;
{
	REG WORD	parent;
	WORD		prev, nextsib;
	REG LPWORD	ptail, phead;

	if (obj != ROOT)
	  parent = get_par(tree, obj, &nextsib);
	else
	  return;

	phead = &tree[parent].ob_head;
	if ( *phead == obj )
	{
						/* this is head child	*/
						/*   in list		*/
	  ptail = &tree[parent].ob_tail;
	  
	  if ( *ptail == obj)
	  {
						/* this is only child	*/
						/*   in list, so fix	*/
						/*   head & tail ptrs	*/
	    nextsib = NIL;
	    *ptail = NIL;
	  }
						/*   move head ptr to 	*/
						/*   next child in list	*/
	  *phead = nextsib;
	}
	else
	{
						/* it's somewhere else,	*/
						/*   so move pnext	*/
						/*   around it		*/
	  prev = get_prev(tree, parent, obj);
	  tree[prev].ob_next = nextsib;

	  ptail = &(tree[parent].ob_tail);
	  if ( *ptail == obj)
						/* this is last child	*/
						/*   in list, so move	*/
						/*   tail ptr to prev	*/
						/*   child in list	*/
	    *ptail = prev;
	}
} /* ob_delete */


/*
*	Routine to change the order of an object relative to its
*	siblings in the tree.  0 is the head of the list and NIL
*	is the tail of the list.
*/
	VOID
ob_order(tree, mov_obj, new_pos)
	REG LPTREE	tree;
	REG WORD	mov_obj;
	WORD		new_pos;
{
	REG WORD	parent;
	WORD		chg_obj, ii, junk;
	REG LPWORD	phead, pnext, pmove;

	if (mov_obj != ROOT)
	  parent = get_par(tree, mov_obj, &junk);
	else
	  return;

	ob_delete(tree, mov_obj);
	phead = &tree[parent].ob_head;
	chg_obj = *phead;
	pmove = &tree[mov_obj].ob_next;
	if (new_pos == 0)
	{
						/* put mov_obj at head	*/
						/*   of list		*/
	  *pmove = chg_obj;
	  *phead = mov_obj;
	}
	else
	{
						/* find new_pos		*/
	  if (new_pos == NIL)
	    chg_obj = tree[parent].ob_tail;
	  else
	  {
	    for (ii = 1; ii < new_pos; ii++)
	      chg_obj = tree[chg_obj].ob_next;
	  } /* else */
						/* now add mov_obj 	*/
						/*   after chg_obj	*/
	  pnext = &tree[chg_obj].ob_next;
	  *pmove = *pnext;
	  *pnext = mov_obj;
	}
	if (*pmove == parent)
	  tree[parent].ob_tail = mov_obj;
} /* ob_order */



/************************************************************************/
/* o b _ e d i t 							*/
/************************************************************************/
/* see OBED.C								*/

/*
*	Routine to change the state of an object and redraw that
*	object using the current clip rectangle.
*/
	VOID
ob_change(tree, obj, new_state, redraw)
	REG LPTREE	tree;
	REG WORD	obj;
	UWORD		new_state;
	WORD		redraw;
{
	WORD		flags, obtype, th;
	GRECT		t;
	UWORD		curr_state;
	LONG		spec;

	ob_sst(tree, obj, &spec, (WORD *)&curr_state, &obtype, &flags, &t, &th);
	
	if ( (curr_state == new_state) ||
	     (spec == -1L) )
	  return;	

	tree[obj].ob_state = new_state;

	if (redraw)
	{
	  ob_offset(tree, obj, &t.g_x, &t.g_y);

	  gsx_moff();

	  th = (th < 0) ? 0 : th;

	  if ( obtype == G_USERDEF )
	  {
	    ob_user(tree, obj, &t, spec, curr_state, new_state);
	    redraw = FALSE;
	  }
	  else
	  {
	    if ( (obtype != G_ICON)  && (obtype != G_CLRICN) &&
		 (obtype != G_BUTTON)  &&  !(flags & FLAG3D)  &&  /*910326WHF*/
		  !(new_state & DRAW3D) && !(curr_state & DRAW3D) && /* JCE 22-10-1999 */
	       ((new_state ^ curr_state) & SELECTED) )
	    {
	      bb_fill(MD_XOR, FIS_SOLID, IP_SOLID, t.g_x+th, t.g_y+th,
			t.g_w-(2*th), t.g_h-(2*th));
	      redraw = FALSE;
	    }
	  }

	  if (redraw)
	      just_draw(tree, obj, t.g_x, t.g_y);


	  gsx_mon();
	}
	return;
} /* ob_change */


	UWORD
ob_fs(tree, ob, pflag)
	LPTREE		tree;
	WORD		ob;
	WORD		*pflag;
{
	*pflag = tree[ob].ob_flags;
	return( tree[ob].ob_state );
}


/************************************************************************/
/* o b _ a c t x y w h							*/
/************************************************************************/
	VOID
ob_actxywh(tree, obj, pt)
	REG LPTREE	tree;
	REG WORD	obj;
	REG GRECT	*pt;
{
	LPWORD		pw;

	ob_offset(tree, obj, &pt->g_x, &pt->g_y);

	pw = &tree[obj].ob_width;
	pt->g_w = pw[0];
	pt->g_h = pw[1];
} /* ob_actxywh */


/************************************************************************/
/* o b _ r e l x y w h							*/
/************************************************************************/
	VOID
ob_relxywh(tree, obj, pt)
	LPTREE		tree;
	WORD		obj;
	GRECT		*pt;
{
	LWCOPY(ADDR(pt), &tree[obj].ob_x, sizeof(GRECT)/2);
} /* ob_relxywh */


	VOID
ob_setxywh(tree, obj, pt)
	LPTREE		tree;
	WORD		obj;
	GRECT		*pt;
{
	LWCOPY(&tree[obj].ob_x, ADDR(pt), sizeof(GRECT)/2);
}


/*
*	Routine to find the x,y offset of a particular object relative
*	to the physical screen.  This involves accumulating the offsets
*	of all the objects parents up to and including the root.
*/
	VOID
ob_offset(tree, obj, pxoff, pyoff)
	REG LPTREE	tree;
	REG WORD	obj;
	REG WORD	*pxoff, *pyoff;
{
	WORD		junk;
	LPWORD		px;

	*pxoff = *pyoff = 0;
	do
	{
						/* have our parent--	*/
						/*  add in his x, y	*/
	  px = &tree[obj].ob_x;
	  *pxoff += px[0];
	  *pyoff += px[1];
	  obj = get_par(tree, obj, &junk);
	} while ( obj != NIL );
}


/*
*	Routine to find the object that is previous to us in the
*	tree.  The idea is we get our parent and then walk down
*	his list of children until we find a sibling who points to
*	us.  If we are the first child or we have no parent then
*	return NIL.
*/
	WORD
get_prev(tree, parent, obj)
	REG LPTREE	tree;
	WORD		parent;
	REG WORD	obj;
{
	REG WORD	nobj, pobj;

	pobj = tree[parent].ob_head;
	if (pobj != obj)
	{
	  while (TRUE)
	  {
	    nobj = tree[pobj].ob_next;
	    if (nobj != obj)
	      pobj = nobj;
	    else
	      return(pobj);
	  }
	}
//	else	
	  return(NIL);
} /* get_prev */

/************************ VIEWMAX CODE ***********************************/
#if 0

EXTERN WORD	gl_width;

EXTERN WORD	gl_wclip;
EXTERN WORD	gl_hclip;

EXTERN WORD	gl_wchar;
EXTERN WORD	gl_hchar;

EXTERN LONG	ad_intin;
EXTERN WORD	intin[];

EXTERN BOOLEAN	gl_domono;


EXTERN THEGLO	D;

GLOBAL LONG	ad_tmpstr;
GLOBAL LONG	ad_rawstr;
GLOBAL LONG 	ad_fmtstr;
EXTERN LONG	ad_valstr;
GLOBAL LONG 	ad_edblk;
GLOBAL LONG     ad_bi;

GLOBAL TEDINFO	edblk;
GLOBAL BITBLK	bi;

MLOCAL ICONBLK	ib;


/*
*	Routine to take an unformatted raw string and based on a
*	template string build a formatted string.
*/
	VOID
ob_format(just, raw_str, tmpl_str, fmt_str)
	WORD		just;
	BYTE		*raw_str, *tmpl_str, *fmt_str;
{
	REG BYTE	*pfbeg, *ptbeg, *prbeg;
	BYTE		*ptend, *prend;
/*	BYTE		*pfend;	(RSF) unused */
	REG WORD	inc, ptlen, prlen;
//#if DBCS
	REG WORD	i;
	BYTE		addc = 0;
//#endif /* DBCS */

	if (*raw_str == '@')
//#if DBCS
	{
	  *raw_str = NULL;
	  addc = 1;
	}
//#else /* DBCS */
	  *raw_str = NULL;
//#endif /* DBCS */
	pfbeg = fmt_str;
	ptbeg = tmpl_str;
	prbeg = raw_str;

	ptlen = strlen(tmpl_str);
	prlen = strlen(raw_str);

	inc = 1;
	pfbeg[ptlen] = NULL;
	if (just == TE_RIGHT)
	{
	  inc = -1;
	  pfbeg = pfbeg + ptlen - 1;
	  ptbeg = ptbeg + ptlen - 1;
	  prbeg = prbeg + prlen - 1;
	}

/*	pfend = pfbeg + (inc * ptlen);	(RSF) unused */
	ptend = ptbeg + (inc * ptlen);
	prend = prbeg + (inc * prlen);
//#if DBCS
	i = ptbeg - tmpl_str;
//#endif /* DBCS */
	while( ptbeg != ptend )
	{
//#if DBCS
	  if ( !isequalto(tmpl_str, i, '_') )
//#else /* DBCS */
	  if ( *ptbeg != '_')
//#endif /* DBCS */	      
	    *pfbeg = *ptbeg;
	  else
	  {
	    if ( prbeg != prend )
	    {
	      *pfbeg = *prbeg;
	      prbeg += inc;
	    }
	    else
	      *pfbeg = '_';
	  } /* else */
	  pfbeg += inc;
	  ptbeg += inc;
//#if DBCS
	  i += inc;
//#endif /* DBCS */
	} /* while */
//#if DBCS
	if (addc)
	  *raw_str = '@';
//#endif /* DBCS */
} /* ob_format */


/*
*	Routine to load up and call a user defined object draw or change 
*	routine.
*/

MLOCAL	WORD
ob_user(TREE tree, WORD obj, GRECT *pt, LONG spec, 
	WORD curr_state, WORD new_state)
{
	PARMBLK		pb;

	pb.pb_tree = tree;
	pb.pb_obj = obj;
	pb.pb_prevstate = curr_state;
	pb.pb_currstate = new_state;
	memmove( (void *)&pb.pb_x, (void *)pt, sizeof( GRECT ) ) ;
	gsx_gclip((GRECT*)&pb.pb_xc);
	pb.pb_parm = LLGET(spec+4);
	return(  far_call(LLGET(spec), ADDR(&pb)) );
}




/*
*	Routine to find the object that is previous to us in the
*	tree.  The idea is we get our parent and then walk down
*	his list of children until we find a sibling who points to
*	us.  If we are the first child or we have no parent then
*	return NIL.
*/
MLOCAL	WORD
get_prev(REG TREE tree, WORD parent, REG WORD obj)
{
	REG WORD	nobj, pobj;

	pobj = (tree+parent)->ob_head ;
	if (pobj != obj)
	{
	  while (TRUE)
	  {
	    nobj = (tree+pobj)->ob_next ;
	    if (nobj != obj)
	      pobj = nobj;
	    else
	      return(pobj);
	  }
	}
	return(NIL);
} /* get_prev */



/*
*	Object draw routine that walks tree and draws appropriate objects.
*/
	VOID
ob_draw(tree, obj, depth)
	REG TREE	tree;
	WORD		obj, depth;
{
	WORD		last, pobj;
	WORD		sx, sy;

	pobj = get_par(tree, obj, &last);

	if (pobj != NIL)
	  ob_offset(tree, pobj, &sx, &sy);
	else
	  sx = sy = 0;

	gsx_moff();
	everyobj(tree, obj, last, just_draw, sx, sy, depth);
	gsx_mon();
}


/*
*	Routine to find out which object a certain mx,my value is
*	over.  Since each parent object contains its children the
*	idea is to walk down the tree, limited by the depth parameter,
*	and find the last object the mx,my location was over.
*/

/************************************************************************/
/* o b _ f i n d							*/
/************************************************************************/
	WORD
ob_find(tree, currobj, depth, mx, my)
	REG TREE	tree;
	REG WORD	currobj;
	REG WORD	depth;
	WORD		mx, my;
{
	WORD		lastfound;
	WORD		dosibs, done, junk;
	GRECT		t, o;
	WORD		parent, childobj, flags;

	lastfound = NIL;

	if (currobj == 0)
	  r_set(&o, 0, 0, 0, 0);
	else
	{
	  parent = get_par(tree, currobj, &junk);
	  ob_actxywh(tree, parent, &o);
	}
	
	done = FALSE;
	dosibs = FALSE;

	while( !done )
	{
						/* if inside this obj,	*/
						/*   might be inside a	*/
						/*   child, so check	*/
	  ob_relxywh(tree, currobj, &t);
	  t.g_x += o.g_x;
	  t.g_y += o.g_y;

	  flags = (tree+currobj)->ob_flags ;
	  if ( (inside(mx, my, &t)) &&
	       (!(flags & HIDETREE)) )
	  {
	    lastfound = currobj;

	    childobj = (tree+currobj)->ob_tail ;
	    if ( (childobj != NIL) && depth)
	    {
	      currobj = childobj;
	      depth--;
	      o.g_x = t.g_x;
	      o.g_y = t.g_y;
	      dosibs = TRUE;
	    }
	    else
	      done = TRUE;
	  }
	  else
	  {
	    if ( (dosibs) &&
	         (lastfound != NIL) )
	    {
	        currobj = get_prev(tree, lastfound, currobj);
	        if (currobj == NIL)
	          done = TRUE;
	    }
	    else
	      done = TRUE;
	  }
	}
						/* if no one was found	*/
						/*   this will return	*/
						/*   NIL		*/
	return(lastfound);
} /* ob_find */


/*
*	Routine to add a child object to a parent object.  The child
*	is added at the end of the parent's current sibling list.
*	It is also initialized.
*/
	VOID
ob_add(tree, parent, child)
	REG TREE	tree;
	REG WORD	parent, child;
{
	REG WORD	lastkid;

	if ( (parent != NIL) &&
	     (child != NIL) )
	{
						/* initialize child	*/
	  (tree+child)->ob_next = parent ;

	  lastkid = (tree+parent)->ob_tail ;
	  if (lastkid == NIL)
						/* this is parent's 1st	*/
						/*   kid, so both head	*/
						/*   and tail pt to it	*/
		  (tree+parent)->ob_head = child ;
	  else
						/* add kid to end of 	*/
						/*   kid list		*/
		  (tree+lastkid)->ob_next = child ;
	
 	  (tree+parent)->ob_tail = child ;
	}
} /* ob_add */

/*
*	Routine to delete an object from the tree.
*/
	VOID
ob_delete(tree, obj)
	REG TREE	tree;
	REG WORD	obj;
{
	REG WORD	parent;
	WORD		prev, nextsib;

	if (obj != ROOT)
	  parent = get_par(tree, obj, &nextsib);
	else
	  return;

	if ( obj == (tree+parent)->ob_head )
	{
						/* this is head child	*/
						/*   in list		*/
	  if ( obj == (tree+parent)->ob_tail )
	  {
						/* this is only child	*/
						/*   in list, so fix	*/
						/*   head & tail ptrs	*/
	    nextsib = NIL;
	    (tree+parent)->ob_tail = NIL ;
	  }
						/*   move head ptr to 	*/
						/*   next child in list	*/
	  (tree+parent)->ob_head = nextsib ;
	}
	else
	{
						/* it's somewhere else,	*/
						/*   so move pnext	*/
						/*   around it		*/
	  prev = get_prev(tree, parent, obj);
	  (tree+prev)->ob_next = nextsib ;
	  if ( obj == (tree+parent)->ob_tail )
						/* this is last child	*/
						/*   in list, so move	*/
						/*   tail ptr to prev	*/
						/*   child in list	*/
	    (tree+parent)->ob_tail = prev ;
	}
} /* ob_delete */

/*
*	Routine to change the order of an object relative to its
*	siblings in the tree.  0 is the head of the list and NIL
*	is the tail of the list.
*/
	VOID
ob_order(tree, mov_obj, new_pos)
	REG TREE	tree;
	REG WORD	mov_obj;
	WORD		new_pos;
{
	REG WORD	parent;
	WORD		chg_obj, ii, junk;

	if (mov_obj != ROOT)
	  parent = get_par(tree, mov_obj, &junk);
	else
	  return;

	ob_delete(tree, mov_obj);
	chg_obj = (tree+parent)->ob_head ;
	if (new_pos == 0)
	{
						/* put mov_obj at head	*/
						/*   of list		*/
	  (tree+mov_obj)->ob_next = chg_obj ;
	  (tree+parent)->ob_head = mov_obj ;
	}
	else
	{
						/* find new_pos		*/
	  if (new_pos == NIL)
	    chg_obj = (tree+parent)->ob_tail ;
	  else
	  {
	    for (ii = 1; ii < new_pos; ii++)
	      chg_obj = (tree+chg_obj)->ob_next ;
	  } /* else */
						/* now add mov_obj 	*/
						/*   after chg_obj	*/
	  (tree+mov_obj)->ob_next = (tree+chg_obj)->ob_next ;
	  (tree+chg_obj)->ob_next = mov_obj ;
	}
	if ( parent == (tree+mov_obj)->ob_next )
	  (tree+parent)->ob_tail = mov_obj ;
} /* ob_order */

/************************************************************************/
/* o b _ e d i t 							*/
/************************************************************************/
/* see OBED.C								*/

/*
*	Routine to change the state of an object and redraw that
*	object using the current clip rectangle.
*/
	VOID
ob_change(tree, obj, new_state, redraw)
	REG TREE	tree;
	REG WORD	obj;
	UWORD		new_state;
	WORD		redraw;
{
	WORD		flags, obtype, th;
	GRECT		t;
	UWORD		curr_state;
	LONG		spec;

	ob_sst(tree, obj, &spec, (WORD*)&curr_state, &obtype, 
		&flags, &t, &th);
	
	if ( (curr_state == new_state) || (spec == -1L) )
	  return;	

        (tree+obj)->ob_state = new_state ;

	if (redraw)
	{
	  ob_offset(tree, obj, &t.g_x, &t.g_y);

	  gsx_moff();

	  th = (th < 0) ? 0 : th;

	  if ( obtype == G_USERDEF )
	  {
	    ob_user(tree, obj, &t, spec, curr_state, new_state);
	    redraw = FALSE;
	  }
	  else
	  {
	    if ( (obtype != G_ICON)  && (obtype != G_CLRICN) &&
		 (obtype != G_BUTTON)  &&  !(flags & FLAG3D)  &&  /*910326WHF*/
	       ((new_state ^ curr_state) & SELECTED) )
	    {
	      bb_fill(MD_XOR, FIS_SOLID, IP_SOLID, t.g_x+th, t.g_y+th,
			t.g_w-(2*th), t.g_h-(2*th));
	      redraw = FALSE;
	    }
	  }

	  if (redraw)
	      just_draw(tree, obj, t.g_x, t.g_y);

	  gsx_mon();
	}
	return;
} /* ob_change */

	UWORD
ob_fs(tree, ob, pflag)
	TREE		tree;
	WORD		ob;
	WORD		*pflag;
{
	*pflag = (tree+ob)->ob_flags ;
	return( (tree+ob)->ob_state ) ;
}

/************************************************************************/
/* o b _ a c t x y w h							*/
/************************************************************************/
	VOID
ob_actxywh(tree, obj, pt)
	REG TREE	tree;
	REG WORD	obj;
	REG GRECT	*pt;
{
	ob_offset(tree, obj, &pt->g_x, &pt->g_y);
	pt->g_w = (tree+obj)->ob_width ;
	pt->g_h = (tree+obj)->ob_height ;
} /* ob_actxywh */


/************************************************************************/
/* o b _ r e l x y w h							*/
/************************************************************************/
	VOID
ob_relxywh(tree, obj, pt)
	LPTREE		tree;
	WORD		obj;
	GRECT		*pt;
{
	LBCOPY( (char far *)pt, 
	    (char far *)&((tree+obj)->ob_x), sizeof(GRECT) );
} /* ob_relxywh */

	VOID
ob_setxywh(tree, obj, pt)
	LPTREE		tree;
	WORD		obj;
	GRECT		*pt;
{
	LBCOPY( (char far *)(&(tree+obj)->ob_x), 
	    (char far *)pt, sizeof(GRECT) );
}


/*
*	Routine to find the x,y offset of a particular object relative
*	to the physical screen.  This involves accumulating the offsets
*	of all the objects parents up to and including the root.
*/
	VOID
ob_offset(tree, obj, pxoff, pyoff)
	REG TREE	tree;
	REG WORD	obj;
	REG WORD	*pxoff, *pyoff;
{
	WORD		junk;

	*pxoff = *pyoff = 0;
	do
	{
						/* have our parent--	*/
						/*  add in his x, y	*/
	  *pxoff += (tree+obj)->ob_x ;
	  *pyoff += (tree+obj)->ob_y ;
	  obj = get_par(tree, obj, &junk);
	} while ( obj != NIL );
}


/* end of gemoblib.c */

#endif

