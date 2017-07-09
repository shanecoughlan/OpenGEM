/*	RCSLIB.C	10/11/84 - 1/25/85 	Tim Oren		*/
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
#include "ppdrcs.h"

EXTERN	MFDB	gl_dst;

	VOID
set_slsize(w_handle, h_size, v_size)
	WORD	w_handle, h_size, v_size;
	{
	WORD	cur_size, foo;

	wind_get(w_handle, WF_HSLSIZ, &cur_size, &foo, &foo, &foo);
	if (cur_size != h_size)
		wind_set(w_handle, WF_HSLSIZ, h_size, 0, 0, 0);
	wind_get(w_handle, WF_VSLSIZ, &cur_size, &foo, &foo, &foo);
	if (cur_size != v_size)
		wind_set(w_handle, WF_VSLSIZ, v_size, 0, 0, 0);
	}	

	VOID
set_slpos(w_handle, h_pos, v_pos)
	WORD	w_handle, h_pos, v_pos;
	{
	WORD	cur_pos, foo;

	wind_get(w_handle, WF_HSLIDE, &cur_pos, &foo, &foo, &foo);
	if (cur_pos != h_pos)
		wind_set(w_handle, WF_HSLIDE, h_pos, 0, 0, 0);
	wind_get(w_handle, WF_VSLIDE, &cur_pos, &foo, &foo, &foo);
	if (cur_pos != v_pos)
		wind_set(w_handle, WF_VSLIDE, v_pos, 0, 0, 0);
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

	
MLOCAL WORD gem5 = -1;
	
WORD make_cbox(LPTREE tr, WORD obj)
{	
	if ((tr[obj].ob_state & EXT3D) == EXT3D)
	{
		if (tr[obj].ob_type == G_BUTTON) 
		{
			tr[obj].ob_type = G_STRING;
		}

		/* The little pseudo-buttons on the "output" page are not 
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
	

	
WORD ini_tree(LPTREE *tree, WORD which)		/* find tree address */
{
	WORD w;

	w = rsrc_gaddr(R_TREE, which, (LPVOID *)tree) ;
	
	if (!*tree) return w;

	if (gem5 == -1)
	{
		char name[40];
		vqt_name(gl_handle, 1, name);
		if (name[0]) gem5 = 0; else gem5 = 1;
	}
	
	if (!gem5 && !(gl_xbuf.abilities & ABLE_X3D)) return w;
	if (which == DIALPBX || which == ALRTPBX) return w;

	map_tree(*tree, ROOT, NIL, make_cbox);

	return w;
}


	

LPVOID image_addr(WORD which)
	{
	LPVOID	where;

	rsrc_gaddr(R_IMAGEDATA, which, &where);
	return (where);
	} 

LPBYTE string_addr(WORD which)
	{
	LPVOID	where;

	rsrc_gaddr(R_STRING, which, &where);
	return (LPBYTE)(where);
	} 

VOID arrange_tree(LPTREE tree, WORD wint, WORD hint, WORD *maxw, WORD *maxh)
	{
	WORD	obj, x, y, rowh, wroot;

	if ( !(wroot = tree[ROOT].ob_width)) 
		return;
	x = wint;
	y = hint;
	rowh = 0;
	*maxw = 0;

	for (obj = tree[ROOT].ob_head; obj != ROOT; 
		obj = tree[obj].ob_next)
		{
		/* [JCE] The brackets seemed to be wrong here; they were round the 
		 *       &&, not the > */
		if (rowh && (x + tree[obj].ob_width > wroot))
			{
			x = wint;
			y += (rowh + hint);
			rowh = 0;
			}
		tree[obj].ob_x = x;
		tree[obj].ob_y = y;
		if ( !(tree[obj].ob_flags & HIDETREE) )
			{
			x += (tree[obj].ob_width + wint); 
			*maxw = max(*maxw, x);
			rowh = max(rowh, tree[obj].ob_height);
			}
		}
	*maxh = y + rowh + hint;
	}

/*
*	Routine that will find the parent of a given object.  The
*	idea is to walk to the end of our siblings and return
*	our parent.  If object is the root then return NIL as parent.
*/
WORD get_parent(LPTREE tree, WORD obj)
{
	WORD		pobj;
	
	if (obj == NIL) return (NIL);
	pobj = tree[obj].ob_next;

	if (pobj != NIL)
	{	
	  while( tree[pobj].ob_tail != obj ) 
	  {
	    obj  = pobj;
	    pobj = tree[obj].ob_next;
	  }
	}
	return(pobj);
} /* get_parent */


WORD nth_child(LPTREE tree, WORD nobj, WORD n)
	{
	for (nobj = tree[nobj].ob_head; --n; nobj = tree[nobj].ob_next);
	return (nobj);
	}


	
WORD child_of(LPTREE tree, WORD nobj, WORD obj)
	{
	WORD	nbar;

	nobj = tree[nobj].ob_head;
	for (nbar = 1; nobj != obj; nbar++)
		nobj = tree[nobj].ob_next;
	return (nbar);
	}

VOID table_code(LPTREE tree, WORD obj0, WORD *types, WORD ntypes, WORD type)
{
	WORD itype;
	
	for (itype = 0; itype < ntypes; itype++)
		if (type == types[itype])
			{
			sel_obj(tree, obj0 + itype);
			return;
			}
	}

WORD encode(LPTREE tree, WORD ob1st, WORD num)
	{
	for (; num--; )
		if (tree[ob1st+num].ob_state & SELECTED)
			return(num);
	return (0);
	}

#include <stdio.h>
	
	
VOID map_tree(LPTREE tree, WORD first, WORD last, MAPROUTINE routine)
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

VOID snap_xy(WORD *x, WORD *y)
	{
	*x += gl_wchar / 2;	
	*y += gl_hchar / 2;
	*x -= *x % gl_wchar;
	*y -= *y % gl_hchar;
	}

VOID snap_wh(WORD *w, WORD *h)
	{
	*w += gl_wchar / 2;
	*h += gl_hchar / 2;
	*w = max(*w - *w % gl_wchar, gl_wchar);
	*h = max(*h - *h % gl_hchar, gl_hchar);
	}

VOID text_wh(LPTEDI taddr, WORD type, WORD *w, WORD *h)
	{
	WORD	font;
	LPBYTE  text;

	font = taddr->te_font;
	text = (type == G_TEXT || type == G_BOXTEXT) ? 
				taddr->te_ptext  : 
				taddr->te_ptmplt;
	*h = ch_height(font);
	*w = ch_width(font) * LSTRLEN(text);
	}

VOID icon_wh(LPICON taddr, WORD *w, WORD *h)
	{
	ICONBLK	here;
	GRECT	p;

	LBCOPY(ADDR(&here), (LPVOID)taddr, sizeof(ICONBLK));
	rc_copy((GRECT *) &here.ib_xchar, &p);
	rc_union((GRECT *) &here.ib_xicon, &p);
	rc_union((GRECT *) &here.ib_xtext, &p);
	*w = p.g_x + p.g_w;
	*h = p.g_y + p.g_h;
	}
       

VOID icon_tfix(LPICON taddr)
	{
	WORD	dw;

	dw = (taddr->ib_wicon - taddr->ib_wtext) / 2;
	taddr->ib_xicon = (dw<0)? -dw: 0;
	taddr->ib_xtext = (dw<0)? 0: dw;
	}


VOID trans_bitblk(LPBIT obspec)
	{
	LPVOID	taddr;
	WORD	wb, hl;

	if ( (taddr = obspec->bi_pdata) == (LPVOID)-1)
		return;
	wb = obspec->bi_wb;
	hl = obspec->bi_hl;
	gsx_trans(taddr, wb, taddr, wb, hl);
	}

VOID grect_to_array(GRECT *area, WORD array[])
{
    array[0] = area->g_x;
    array[1] = area->g_y;
    array[2] = area->g_x + area->g_w - 1;
    array[3] = area->g_y + area->g_h - 1;
}


    VOID
rast_op(mode, s_area, s_mfdb, d_area, d_mfdb)
    WORD	mode;
    GRECT	*s_area, *d_area;
    MFDB	*s_mfdb, *d_mfdb;
{
    WORD	pxy[8];

    grect_to_array(s_area, pxy);
    grect_to_array(d_area, &pxy[4]);
    vro_cpyfm( gl_handle, mode, pxy, s_mfdb, d_mfdb);
}


VOID outl_obj(LPTREE tree, WORD obj, GRECT *clprect)
	{
	GRECT	p;

	objc_xywh(tree, obj, &p);
	rc_intersect( clprect, &p);
	gsx_moff();
	gsx_outline(&p);
	gsx_mon();
	}

	
VOID invert_obj(LPTREE tree, WORD obj, GRECT *clprect)
	{
	GRECT	hot;

	objc_xywh(tree, obj, &hot);
	rc_intersect( clprect, &hot );
	gsx_moff();
	gsx_invert(&hot);
	gsx_mon();
	if(iconedit_flag)
		{
		if (tree[obj].ob_state == SELECTED) tree[obj].ob_state = NORMAL;
		else tree[obj].ob_state = SELECTED;
		}
	}

	
WORD trans_obj(LPTREE tree, WORD obj)
	{
	WORD	type, wb, hl;
	LPVOID	taddr;
	LPVOID	obspec;
	LPICON  ib;

	type = tree[obj].ob_type;
	if ( (obspec = tree[obj].ob_spec) == (LPVOID)-1)
		return;
	switch (type) {
		case G_IMAGE:
			trans_bitblk((LPBIT)obspec);
			return;
		case G_ICON:
			ib = (LPICON)obspec;
			hl = ib->ib_hicon;
			wb = (ib->ib_wicon + 7) >> 3;
			if ( (taddr = ib->ib_pdata ) != (LPVOID)-1) gsx_trans(taddr, wb, taddr, wb, hl);
			if ( (taddr = ib->ib_pmask ) != (LPVOID)-1)	gsx_trans(taddr, wb, taddr, wb, hl);
			return;
		default:
			return;
		}
	}

VOID unfix_chpos(LPUWORD where, WORD x_or_y)
	{
	UWORD	cpos, coff, cbits;

	cpos = *where;
	coff = cpos / (x_or_y? gl_hchar: gl_wchar);
	cbits = cpos % (x_or_y? gl_hchar: gl_wchar);
	*where = coff | (cbits << 8);
	}

/************************************************************************/
/* fix_chpos								*/
/************************************************************************/
VOID fix_chpos(LPUWORD pfix, WORD ifx)
{
	WORD	coffset;
	WORD	cpos;

	cpos = *pfix;
	coffset = (WORD) LHIBT(cpos) & 0xff;	/* Alcyon bug ! */
	cpos = (WORD) LLOBT(cpos) & 0xff;
	if (ifx)
	  cpos *= gl_wchar;
	else
	  cpos *= gl_hchar;
	if ( coffset > 128 )			/* crude	*/
	  cpos -= (256 - coffset);
	else
	  cpos += coffset;
	*pfix = cpos;
} /* fix_chpos() */


/************************************************************************/
/* rs_obfix								*/
/************************************************************************/
WORD rs_obfix(LPTREE tree, WORD curob)
{
	WORD		i, val;
	LPUWORD		p;
						/* set X,Y,W,H with	*/
						/*   fixch, use val	*/
						/*   to alternate TRUEs	*/
						/*   and FALSEs		*/
	p = &(tree[curob].ob_x);

	val = TRUE;
	for (i=0; i<4; i++)
	{
	  fix_chpos(&p[i], val);
	  val = !val;
	}
}


VOID undo_obj(LPTREE tree, WORD which, WORD bit)
	{
	tree[which].ob_state &= ~bit;
	}

	
VOID do_obj(LPTREE tree, WORD which, WORD bit)
	{
	tree[which].ob_state |= bit;
	}

	
WORD enab_obj(LPTREE tree, WORD which)
	{
	undo_obj(tree, which, DISABLED);
	return 1;
	}

	
WORD disab_obj(LPTREE tree, WORD which)
	{
	do_obj(tree, which, DISABLED);
	return 1;
	}


	
VOID sel_obj(LPTREE tree, WORD which)
	{
	do_obj(tree, which, SELECTED);
	}

	
WORD desel_obj(LPTREE tree, WORD which)
	{
	undo_obj(tree, which, SELECTED);
	return 1;
	}

	
VOID chek_obj(LPTREE tree, WORD which)
	{
	do_obj(tree, which, CHECKED);
	}

	
WORD unchek_obj(LPTREE tree, WORD which)
	{
	undo_obj(tree, which, CHECKED);
	return 1;
	}

	
VOID unflag_obj(LPTREE tree, WORD which, WORD bit)
	{
	tree[which].ob_flags &= ~bit;
	}

	
VOID flag_obj(LPTREE tree, WORD which, WORD bit)
	{
	tree[which].ob_flags |= bit;
	}

	
VOID hide_obj(LPTREE tree, WORD which)
	{
	flag_obj(tree, which, HIDETREE);
	}

	
VOID unhide_obj(LPTREE tree, WORD which)
	{
	unflag_obj(tree, which, HIDETREE);
	}

	
VOID indir_obj(LPTREE tree, WORD which)
	{
	flag_obj(tree, which, INDIRECT);
	}

	
VOID dir_obj(LPTREE tree, WORD which)
	{
	unflag_obj(tree, which, INDIRECT);
	}

	
VOID sble_obj(LPTREE tree, WORD which)
	{
	flag_obj(tree, which, SELECTABLE);
	}

	
WORD unsble_obj(LPTREE tree, WORD which)
	{
	unflag_obj(tree, which, SELECTABLE);
	return 1;
	}

	
VOID objc_xywh(LPTREE tree, WORD obj, GRECT *p)
	{
	objc_offset(tree, obj, &p->g_x, &p->g_y);
	p->g_w = tree[obj].ob_width;
	p->g_h = tree[obj].ob_height;
	}

	
VOID ob_setxywh(LPTREE tree, WORD obj, GRECT *pt)
{
	LWCOPY((LPVOID)&tree[obj].ob_x, ADDR(pt), sizeof(GRECT)/2);
}


/************************************************************************/
/* o b _ r e l x y w h							*/
/************************************************************************/
VOID ob_relxywh(LPTREE tree, WORD obj, GRECT *pt)
{
	LWCOPY(ADDR(pt), (LPVOID)&tree[obj].ob_x, sizeof(GRECT)/2);
} /* ob_relxywh */


VOID r_get(GRECT *pxywh, WORD *px, WORD *py, WORD *pw, WORD *ph)
{
	*px = pxywh->g_x;
	*py = pxywh->g_y;
	*pw = pxywh->g_w;
	*ph = pxywh->g_h;
}



VOID r_set(GRECT *pxywh, WORD x, WORD y, WORD w, WORD h)
{
	pxywh->g_x = x;
	pxywh->g_y = y;
	pxywh->g_w = w;
	pxywh->g_h = h;
}

	UWORD
inside(x, y, pt)		/* determine if x,y is in rectangle	*/
	WORD		x, y;
	GRECT		*pt;
	{
	if ( (x >= pt->g_x) && (y >= pt->g_y) &&
	    (x < pt->g_x + pt->g_w) && (y < pt->g_y + pt->g_h) )
		return(TRUE);
	else
		return(FALSE);
	} /* inside */

	VOID
rc_constrain(pc, pt)
	GRECT		*pc;
	GRECT		*pt;
	{
	if (pt->g_x < pc->g_x)
	    pt->g_x = pc->g_x;
	if (pt->g_y < pc->g_y)
	    pt->g_y = pc->g_y;
	if ((pt->g_x + pt->g_w) > (pc->g_x + pc->g_w))
	    pt->g_x = (pc->g_x + pc->g_w) - pt->g_w;
	if ((pt->g_y + pt->g_h) > (pc->g_y + pc->g_h))
	    pt->g_y = (pc->g_y + pc->g_h) - pt->g_h;
	}

	VOID
rc_union(p1, p2)
	GRECT		*p1, *p2;
	{
	WORD		tx, ty, tw, th;

	tw = max(p1->g_x + p1->g_w, p2->g_x + p2->g_w);
	th = max(p1->g_y + p1->g_h, p2->g_y + p2->g_h);
	tx = min(p1->g_x, p2->g_x);
	ty = min(p1->g_y, p2->g_y);
	p2->g_x = tx;
	p2->g_y = ty;
	p2->g_w = tw - tx;
	p2->g_h = th - ty;
	}

WORD min(WORD a, WORD b)
{
	return( (a < b) ? a : b );
}


WORD max(WORD a, WORD b)
{
	return( (a > b) ? a : b );
}

	VOID
movs(num, ps, pd)
	WORD		num;
	BYTE		*ps, *pd;
{
	do
	  *pd++ = *ps++;
	while (--num);
}
     
	VOID
movup( num, ps, pd)
	WORD	num;
	BYTE	*ps, *pd;
{
	do
	  *pd-- = *ps--;
	while (num--);
}

	BYTE
toupper(c)
	BYTE		c;
{
	return (BYTE) (c > 0x5f ? c & 0x5f : c);
}

VOID strup(BYTE *p1)
	{
	for(; *p1; p1++)
		*p1 = toupper(*p1);
	}


VOID LLSTRCPY(LPBYTE src, LPBYTE dest)
{
	BYTE	b;

	do 
	{
		b = *(src++);
		*(dest++) = b ;
	} while (b);
}

WORD LLSTRCMP(LPBYTE l1, LPBYTE l2)
	{
	BYTE	b;

	while (b = *(l1++))
		if (b != *(l2++))
			return (FALSE);
	return (!*(l2));
	}


BYTE uhex_dig(WORD wd)
	{
	if ( (wd >= 0) && (wd <= 9) )
		return (BYTE) (wd + '0');	
	if ( (wd >= 0x0a) && (wd <= 0x0f) )
		return (BYTE) (wd + 'A' - 0x0a);
	return(' ');
	}

WORD make_num(BYTE *pstr)
	{
/* take the given string & return a number: 0-999			*/
	WORD		num, factor;
	BYTE		*ptmp;

	ptmp = pstr;
	num = 0;
	if (!ptmp)
	  	return(num);
					/* skip to end of number	*/
	while (*ptmp)
	  	ptmp++;
	ptmp--;
					/* pick off digits		*/
	factor = 1;
	while (ptmp >= pstr)
		{
	  	num += (*ptmp - '0') * factor;
	  	factor *= 10;
	  	ptmp--;
		} /* while */
	return(num);
	} 


typedef BYTE *PBYTE;
	
VOID merge_str(BYTE *pdst, BYTE *ptmp, ...)
{
	WORD		do_value;
	BYTE		lholder[12];
	BYTE		*pnum, *psrc;
	LONG		lvalue, divten;
	WORD		digit, base;
	va_list		va_parms;

	va_start(va_parms, ptmp);

	
	while(*ptmp)
	{
	  if (*ptmp != '%')
	    *pdst++ = *ptmp++;
	  else
	  {
	    ptmp++;
	    do_value = FALSE;
	    switch(*ptmp++)
	    {
	      case '%':
		*pdst++ = '%';
		break;
	      case 'L':
		lvalue   = va_arg(va_parms, LONG);
		do_value = TRUE;
		base = 10;
		break;
	      case 'W':
		lvalue   = va_arg(va_parms, WORD);
		do_value = TRUE;
		base = 10;
		break;
	      case 'B':
		lvalue   = va_arg(va_parms, WORD) & 0xFF;
		do_value = TRUE;
		base = 10;
		break;
	      case 'S':
		psrc = va_arg(va_parms, PBYTE);
		while(*psrc)
		  *pdst++ = *psrc++;
		break;
	      case 'H':
		lvalue = va_arg(va_parms, WORD) & 0xffff;
		do_value = TRUE;
		base = 16;
		break;
	      case 'X':
		lvalue = va_arg(va_parms, LONG);
		do_value = TRUE;
		base = 16;
		break;
	    }
	    if (do_value)
	    {
	      pnum = &lholder[0];
	      if (lvalue < 0 && base == 10)
			{
			*pdst++ = '-';
			lvalue = -lvalue;
			}
	      while(lvalue)
	      {
			divten = lvalue / base;
			digit = (WORD) lvalue - (divten * base);
			*pnum++ = uhex_dig(digit);
			lvalue = divten;
	      }
	      if (pnum == &lholder[0])
			*pdst++ = '0';
	      else
	      {
		while(pnum != &lholder[0])
		  *pdst++ = *--pnum;
	      }
	    }
	  }
	}
	*pdst = 0;
	va_end(va_parms);
}


WORD dmcopy(FILE *fp, LONG msrc, LPVOID mdest, WORD mln)
		/* disk to memory copy */
{
	if (fseek(fp, msrc, SEEK_SET) == EOF) return FALSE;
	if ( lfread( mdest, 1, mln, fp) < mln) return FALSE;
	return(mln);

} /* dmcopy */




LPBYTE obj_addr(LPTREE tree, WORD obj, WORD offset)
	{
	return  ((LPBYTE)(&tree[obj])) + offset;
	}

