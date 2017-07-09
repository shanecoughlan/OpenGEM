/*	RCSOBJS.C	11/20/84 - 1/25/85 	Tim Oren		*/
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
#include "ppdrcs.h"


EXTERN	BOOLEAN iconedit_flag;
EXTERN	WORD	gridw, gridh;

	WORD
snap_xs(x)
	WORD	x;
	{
	    WORD i;
	x += (i = iconedit_flag ? gridw : gl_wchar) / 2;
	return (x - x % i);
	}

	WORD
snap_ys(y)
	WORD	y;
	{
	    WORD i;
	y += (i = iconedit_flag ? gridh : gl_hchar) / 2;
	return (y - y % i);
	}

WORD clamp_xs(LPTREE tree, WORD x)
	{
	x = min(x, tree[ROOT].ob_width - view.g_w);
	return max(0, x);
	}


WORD clamp_ys(LPTREE tree, WORD y)
	{
	y = min(y, tree[ROOT].ob_height - view.g_h);
	return max(0, y);
	}

	
VOID do_hsinc(WORD inc)
	{
	rcs_xpan = clamp_xs(ad_view, snap_xs(rcs_xpan + inc));
	}

	
VOID do_vsinc(WORD inc)
	{
	rcs_ypan = clamp_ys(ad_view, snap_ys(rcs_ypan + inc));
	}

VOID set_rootxy(LPTREE tree)
	{
	tree[ROOT].ob_x = view.g_x - rcs_xpan;
	tree[ROOT].ob_y = view.g_y - rcs_ypan;
	}


WORD newsize_obj(LPTREE tree, WORD obj, WORD neww, WORD newh, WORD ok)
	{
	WORD	pobj;
	GRECT	p;

	if (!neww || !newh)
		return(TRUE);
	objc_xywh(tree, obj, &p);
	pobj = get_parent(tree, obj);

	if (!ok)
	if (pobj != posn_obj(tree, pobj, &p.g_x, &p.g_y, neww, newh, TRUE))
		{
		if (!rcs_xpert)
		if (hndl_alert(2, string_addr(STSIZEP)) == 1)
			return(FALSE);
		if (rcs_lock)
			{
			hndl_locked();
			return(FALSE);
			}
		}

	tree[obj].ob_width = neww;
	tree[obj].ob_height = newh;
	return 1;
	}


	VOID
slid_objs()
	{
	WORD	h_size, v_size;
	WORD	w, h;

	w = ad_view[ROOT].ob_width;
	h = ad_view[ROOT].ob_height;
	h_size = (WORD) umul_div(view.g_w, 1000, w);
	h_size = min(1000, h_size);
	v_size = (WORD) umul_div(view.g_h, 1000, h);
	v_size = min(1000, v_size);
	set_slsize(rcs_view, h_size, v_size); 
	h_size = max(1, w - view.g_w);
	h_size = (WORD) umul_div(rcs_xpan, 1000, h_size);
	v_size = max(1, h - view.g_h);
	v_size = (WORD) umul_div(rcs_ypan, 1000, v_size);
	set_slpos(rcs_view, h_size, v_size);
	}

	VOID
view_objs()
	{
	rcs_xpan = clamp_xs(ad_view, rcs_xpan);	
	rcs_ypan = clamp_ys(ad_view, rcs_ypan);
	set_rootxy(ad_view);
	slid_objs();
	send_redraw(rcs_view, &view);
	}

VOID type_obj(WORD obj)
	{
	WORD	exitobj, obtype, obmsb;
	LPUWORD	otaddr;
	LPTEDI	taddr;
	LPTREE	tree;
	GRECT	p;
	BYTE	xtype[4];

	tree = ad_view;
	otaddr = &tree[obj].ob_type;
	obtype = *otaddr;
	obmsb = LHIBT(obtype);
	obtype = LLOBT(obtype);
	objc_xywh(tree, obj, &p);

	ini_tree(&tree, NOBJTREE);
	hide_obj(tree, STRTYPES);
	hide_obj(tree, BOXTYPES);
	hide_obj(tree, TXTTYPES);

	merge_str(xtype, "%W", obmsb);
	taddr = (LPTEDI)(tree[XTYPE].ob_spec);

	taddr->te_ptext   = ADDR(xtype);
	taddr->te_txtlen  = 4;

	switch (obtype) {
		case G_STRING:
		case G_BUTTON:
			unhide_obj(tree, STRTYPES);
			table_code(tree, STRTYPE0, str_types, 2, obtype);
			break;
		case G_BOX:
		case G_IBOX:
		case G_BOXCHAR:
			unhide_obj(tree, BOXTYPES);
			table_code(tree, BOXTYPE0, box_types, 3, obtype);
			break;
		case G_TEXT:
		case G_FTEXT:
		case G_BOXTEXT:
		case G_FBOXTEXT:
			unhide_obj(tree, TXTTYPES);
			table_code(tree, TXTTYPE0, txt_types, 4, obtype);
			break;
		default:
			return;
		}

	exitobj = hndl_dial(tree, 0, &p);

	if (exitobj == NOOKITEM)
		{
		rcs_edited = TRUE;
		obmsb = make_num(xtype) & 0xff;
		switch (obtype) {
			case G_STRING:
			case G_BUTTON:
				obtype = str_types[encode(tree, STRTYPE0, 2)];
				break;
			case G_BOX:
			case G_IBOX:
			case G_BOXCHAR:
				obtype = box_types[encode(tree, BOXTYPE0, 3)];
				break;
			case G_TEXT:
			case G_FTEXT:
			case G_BOXTEXT:
			case G_FBOXTEXT:
				obtype = txt_types[encode(tree, TXTTYPE0, 4)];
				break;
			default:
				return;
			}

			LWSET(otaddr, (obmsb << 8) | obtype);
			set_hot();
		}
	map_tree(tree, ROOT, NIL, desel_obj);

	obj_redraw(ad_view, obj);
	}  

VOID del_ob(WORD sobj)
	{
	LPTREE	tree;
	WORD	n, menu_obj;

	if (rcs_lock)
		{
		hndl_locked();
		return;
		}
	tree = ad_view;
	switch (rcs_state) {
		case FREE_STATE:
		case PANL_STATE:
		case DIAL_STATE:
			obj_redraw(tree, sobj);
			zap_objindex(tree, sobj);
			map_dslct(tree, sobj);
			objc_delete(tree, sobj);
			return;
		case MENU_STATE:
			if (in_menu(tree, sobj))
				{
				if (is_menu(tree, get_parent(tree, sobj)) == 1)
					return;
				zap_objindex(tree, sobj);
				map_dslct(tree, sobj);
				objc_delete(tree, sobj);
				break;
				}
			if (n = is_menu(tree, sobj))
				{
				if (n == 1)
					return;
				zap_objindex(tree, sobj);
				tree[sobj].ob_head = NIL;
				tree[sobj].ob_tail = NIL;
				tree[sobj].ob_height = gl_hchar;
				tree[sobj].ob_width = menu_n(tree, n);
				break;
				}
			if (n = in_bar(tree, sobj))
				{
				if (n == 1)
					return;
				menu_obj = menu_n(tree, n);
				zap_objindex(tree, menu_obj);
				map_dslct(tree, menu_obj);
				objc_delete(tree, menu_obj);
				zap_objindex(tree, sobj);
				map_dslct(tree, sobj);
				objc_delete(tree, sobj);
				fix_menu_bar(tree);
				break;
				}
			return;
		case ALRT_STATE:
			tree[sobj].ob_spec = (LPVOID)-1;
			fix_alert(tree);
			dslct_1obj(tree, sobj);
			break;
		default:
			return;
		}
	view_objs();
	}

VOID constrain_obj(LPTREE tree, WORD obj, WORD *px, WORD *py)
	{
	WORD	tx, ty, x, y;

	switch (rcs_state) {
		case FREE_STATE:
		case DIAL_STATE:
		case MENU_STATE:
			objc_offset(tree, obj, &x, &y);
			tx = *px - x;
			ty = *py - y;
			snap_xy(&tx, &ty);
			*px = x + tx;
			*py = y + ty;
			return;
		case PANL_STATE:
			return;
		default:
			return;
		}
	}


WORD posn_obj(LPTREE tree, WORD obj, WORD *px, WORD *py, WORD w, WORD h, 
              WORD exact_ok)
	{
	GRECT	o;
	WORD	tx, ty; 

	tx = *(&obj);	/* Silly expression to stop PPD allocating obj to
	                 * a register. The PPD optimiser generates 
	                 * incorrect code if obj is allocated to a register
	                 * in this function; it loads obj to DI but doesn't
	                 * set it correctly. */
	FOREVER
		{
		tx = *px;
		ty = *py;
		constrain_obj(tree, obj, &tx, &ty);
		objc_xywh(tree, obj, &o); 
/* rectangle must fit, but not exactly, to nest under the obj */
		if (tx >= o.g_x && ty >= o.g_y &&
			tx + w <= o.g_x + o.g_w &&
			ty + h <= o.g_y + o.g_h )
			{
			if (exact_ok)
				break;	
			if (tx > o.g_x || ty > o.g_y ||
				tx + w < o.g_x + o.g_w ||
				ty + h < o.g_y + o.g_h )
				break;
			}
			
		if (obj == ROOT) return (NIL);
		obj = get_parent(tree, obj);
		}
	*px = tx - o.g_x;
	*py = ty - o.g_y;
	return (obj);
	}

VOID cut_obj(WORD sobj, WORD dup)
	{
	if (rcs_lock)
		{
		hndl_locked();
		return;
		}
	if (rcs_state == MENU_STATE)
	if (!menu_ok(ad_view, sobj) & !dup)
		return;
	rcs_clipkind = OBJKIND;
	ad_clip = copy_tree(ad_view, sobj, TRUE);
	if (!dup)
		del_ob(sobj);
	}

	VOID
paste_obj(dobj, dx, dy, dup)
	WORD	dobj, dx, dy, dup;
	{
	if (rcs_clipkind != OBJKIND)
		return;
	if (new_obj(ad_clip, ROOT, dobj, dx, dy))
	if (!dup)
		clr_clip();
	}

WORD new_obj(LPTREE stree, WORD sobj, WORD dobj, WORD dx, WORD dy)
	{
	LPTREE	tree;
	LPVOID  taddr;
	LPUWORD otaddr;
	WORD	sx, sy, dw, dh, add_at, type, obmsb;

	if (rcs_lock)
		{
		hndl_locked();
		return (FALSE);
		}
	tree = stree;
	dw = tree[sobj].ob_width;
	dh = tree[sobj].ob_height;
	type = LLOBT(tree[sobj].ob_type);

	switch (rcs_state) {
		case FREE_STATE:
			if (type != G_IMAGE && type != G_STRING)
				return (FALSE);
		case PANL_STATE:
		case DIAL_STATE:
			objc_offset(stree, sobj, &sx, &sy);
			if (sx == dx && sy == dy)	/* Don't copy */
				return (FALSE);		/* right on top */
			dobj = posn_obj(ad_view, dobj, &dx, &dy, dw, dh, FALSE);
			if (dobj == NIL)
				return (FALSE);
			sobj = copy_objs(stree, sobj, ad_view, TRUE);
			tree = ad_view;				     
			if( type == G_IMAGE || type == G_ICON)
				{
				obmsb = 1;
				otaddr = &tree[sobj].ob_type;
				*otaddr = (obmsb << 8) | type ;
				}
			if (stree == ad_pbx) tree[sobj].ob_state = NORMAL;
			
			objc_add(ad_view, dobj, sobj);
			tree[sobj].ob_x = dx;
			tree[sobj].ob_y = dy;
			obj_redraw(tree, sobj);
			return (TRUE);
		case MENU_STATE:
			if (LLOBT(tree[sobj].ob_type) == G_TITLE)
				{
				tree = ad_view;
				if (tree[ROOT].ob_head == dobj)  /* is_bar? */
					add_at = -1;
				else if ((add_at = in_bar(tree, dobj) - 1) < 1)
					return (FALSE);
				sobj = copy_objs(stree, sobj, ad_view, TRUE);
				tree[sobj].ob_y = 0;
				objc_add(tree, get_active(tree), sobj);
				objc_order(tree, sobj, add_at);
				sobj = blank_menu(tree, sobj);
				objc_add(tree, tree[ROOT].ob_tail, sobj);
				objc_order(tree, sobj, add_at);
				fix_menu_bar(tree);
				break;
				}
			if (stree == ad_view && is_menu(tree, sobj))
				return (FALSE);
			dobj = posn_obj(ad_view, dobj, &dx, &dy, dw, dh, TRUE);
			if (dobj == NIL)
				return (FALSE);
			tree = ad_view;
			if (in_menu(tree, dobj))
				{
				dx += tree[dobj].ob_x;
				dy += tree[dobj].ob_y;
				dobj = get_parent(tree, dobj);
				}
			if (is_menu(tree, dobj) < 2)
				return (FALSE);
			sobj = copy_objs(stree, sobj, ad_view, TRUE);
			objc_add(tree, dobj, sobj);
			tree[sobj].ob_x = dx;
			tree[sobj].ob_y = dy;
			break;
		case ALRT_STATE:
			taddr = tree[sobj].ob_spec;
			tree = ad_view;
			switch (type) {
				case G_IMAGE:
					if (stree == ad_clip)
						return (FALSE);
					add_at = ALRTIMG;
					break;
				case G_STRING:
					if ( !(add_at = pt_roomp(tree, ALRTSTR0, 5)))
						return (FALSE);
					break;
				case G_BUTTON:
					if ( !(add_at = pt_roomp(tree, ALRTBTN0, 3)))
						return (FALSE);
					break;
				default:
					return (FALSE);
				}
			tree[add_at].ob_spec = taddr;
			fix_alert(tree);
			break;
		default:
			return (FALSE);
		}

	view_objs();
	return (TRUE);
	}


VOID mov_obj(WORD sobj, WORD dobj, WORD dx, WORD dy)
	{
	LPTREE	tree;
	WORD	dw, dh;
	WORD	pobj, n, add_at;

	tree = ad_view;
	dw = tree[sobj].ob_width;
	dh = tree[sobj].ob_height;
	obj_redraw(tree, sobj);

	switch (rcs_state) {
		case FREE_STATE:
		case PANL_STATE:
		case DIAL_STATE:
			pobj = get_parent(tree, sobj);
			dobj = posn_obj(tree, dobj, &dx, &dy, dw, dh, FALSE);
			if (dobj == NIL)
				return;
			if (pobj != dobj)
				{
				if (rcs_lock)
					{
					hndl_locked();
				 	return;
					}
				if (!rcs_xpert)
				if (hndl_alert(2, string_addr(STMOVED)) == 1)
					return;
				objc_delete(tree, sobj);
				objc_add(tree, dobj, sobj);
				}
			tree[sobj].ob_x = dx;
			tree[sobj].ob_y = dy;
    		obj_redraw(tree, sobj);
			return;
		case MENU_STATE:
			if (rcs_lock)
				{
				hndl_locked();
				return;
				}
			if (n = in_bar(tree, sobj))
				{
				if (n == 1 || dobj == sobj)
					return;
				if (tree[ROOT].ob_head == dobj)  /* is_bar? */
					add_at = -1;
				else if ((add_at = in_bar(tree, dobj) - 1) < 1)
					return;
				objc_order(tree, sobj, add_at);
				sobj = menu_n(tree, n);
				objc_order(tree, sobj, add_at);
				fix_menu_bar(tree);
				view_objs();
				return;
				}
			if (!in_menu(tree, sobj))
				return;
			pobj = get_parent(tree, sobj);
			if (is_menu(tree, pobj) == 1)
				return;
			dobj = posn_obj(tree, dobj, &dx, &dy, dw, dh, FALSE);
			if (dobj == NIL)
				return;
			if (in_menu(tree, dobj))
				{
				dx += tree[dobj].ob_x;
				dy += tree[dobj].ob_y;
				dobj = get_parent(tree, dobj);
				}
			if (is_menu(tree, dobj) < 2)
				return;
			tree[sobj].ob_x = dx;
			tree[sobj].ob_y = dy;
			view_objs();
			return;
		default:
			return;
		}
	}


	
VOID size_obj(LPTREE tree, WORD obj)
	{
	LONG	amax;
	LPLPTR	obspec;
	GRECT	p;
	WORD	cobj, pobj, n,type;
	WORD	wlim, hlim, wmax, hmax, wtmp, htmp;

	wlim = gl_wchar;
	hlim = gl_hchar;
	amax = 0L;

	switch (rcs_state) {
		case FREE_STATE:
			if (obj != ROOT)
				return;
		case PANL_STATE:
			wlim = hlim = 2;

		case DIAL_STATE:			      
			obspec = &tree[obj].ob_spec;
			if ( (type = LLOBT(tree[obj].ob_type)) == G_BUTTON )
				wlim = LSTRLEN(*obspec) * gl_wchar;
			else if (type == G_BOXTEXT || type == G_FBOXTEXT ||
				 type == G_FTEXT)
				wlim = LSTRLEN(  ((LPTEDI)*obspec)->te_ptext  ) * gl_wchar;
			break;
		case MENU_STATE:
			if ( in_which_menu(tree, obj) == 1  && in_menu(tree, obj) == 1 )
				return;
			if (menu_ok(tree, obj))
				break;
			if ((n = is_menu(tree, obj)) > 1)
				{
				wlim = tree[bar_n(tree, n)].ob_width;
				amax = (LONG) (gl_nrows * gl_hchar) *
					(LONG) (gl_ncols * gl_wchar) / 4L;
				break;
				}
			return;
		default:
			return;
		}

	if (tree[obj].ob_head != NIL)
		for (cobj = tree[obj].ob_head; cobj != obj; cobj = tree[cobj].ob_next)
			{
			wlim = max(wlim, tree[cobj].ob_x + tree[cobj].ob_width);
			hlim = max(hlim, tree[cobj].ob_y + tree[cobj].ob_height);
			}

	obj_redraw(tree, obj);  
	objc_xywh(tree, obj, &p);

	if (obj == ROOT)
		{
		wmax = full.g_w + full.g_x;
		hmax = full.g_h + full.g_y;
		}
	else
		{
		pobj = get_parent(tree, obj);
		if (rcs_state == MENU_STATE)
		if (in_bar(tree, obj))
			pobj = get_parent(tree, pobj);
		objc_offset(tree, pobj, &wtmp, &htmp);
		wmax = min(view.g_w + view.g_x, wtmp + tree[pobj].ob_width);
		hmax = min(view.g_h + view.g_y, htmp + tree[pobj].ob_height);
		}
	wmax -= p.g_x;
	hmax -= p.g_y;

	graf_mouse(FINGER, 0x0L); 
	clamp_rubbox(p.g_x, p.g_y, wlim, hlim, wmax, hmax, amax, 
		&p.g_w, &p.g_h, &full);
	graf_mouse(ARROW, 0x0L);

	if (rcs_state != PANL_STATE)
		snap_wh(&p.g_w, &p.g_h);
	tree[obj].ob_width  = p.g_w;
	tree[obj].ob_height = p.g_h;

	if (rcs_state == MENU_STATE)	/* Realign bar if necessary */
	if (in_bar(tree, obj))
		{
		fix_menu_bar(tree);
		view_objs();
		return;
		}
	if (obj == ROOT)		/* If ROOT resized, then window */
		view_objs();		/* must be rescrolled */
	else
		obj_redraw(tree, pobj); 
		obj_redraw(tree, obj);
	}	



VOID slct_obj(LPTREE tree, WORD obj)
	{
	WORD	nbar, type;

	if (rcs_nsel)
		{
		mslct_obj(tree, obj);
		return;
		}

	type = LLOBT(tree[obj].ob_type);
	switch (rcs_state) {
		case MENU_STATE:
			if (obj == THEACTIVE)
				return;		/* just in case... */
			if (nbar = in_bar(tree, obj))
				{
				if (rcs_menusel)
					hide_obj(tree, rcs_menusel);
				obj_redraw(tree, rcs_menusel);
				unhide_obj(tree, rcs_menusel =
					menu_n(tree, nbar));
				obj_redraw(tree, rcs_menusel);
				break;
				}
			if (in_menu(tree, obj))
				break;
			if (is_menu(tree, obj) > 1)
				break;
			if (rcs_menusel)
				{
				hide_obj(tree, rcs_menusel);
				obj_redraw(tree, rcs_menusel);
				rcs_menusel = FALSE;
				}
			return;
		case ALRT_STATE:
			if (type != G_STRING && type != G_BUTTON)
				return;
			break;
		case FREE_STATE:
		case PANL_STATE:
		case DIAL_STATE:
			break;
		default:
			return;
		}

	rcs_nsel = 1;
	rcs_sel[0] = obj;

	set_menus();
	set_hot();
	set_switch();
	obj_redraw(tree, obj);
	}

VOID mslct_obj(LPTREE tree, WORD obj)
	{
	WORD	i;

	if (rcs_state == MENU_STATE)
	if (!in_menu(tree, obj))
		{
		dslct_obj();
		slct_obj(tree, obj);
		return;
		}

	for (i = rcs_nsel; i; )		/* Prevent duplicates */
	if (rcs_sel[--i] == obj)
		{
		dslct_1obj(tree, obj);
		return;
		}

	if (rcs_nsel + 1 >= MAXSEL)
		return;
	rcs_sel[rcs_nsel++] = obj;

	set_menus();
	set_hot();
	set_switch();
	obj_redraw(tree, obj);
	}


	
WORD dslct_1obj(LPTREE tree, WORD obj)
	{
	WORD	i;

	for (i = rcs_nsel; i; i--)
		if (rcs_sel[i - 1] == obj)
			break;
	if (!i)
		return;
	for (; i < rcs_nsel; i++)
		rcs_sel[i - 1] = rcs_sel[i];
	rcs_nsel--;
	set_menus();
	set_hot();
	set_switch();
	obj_redraw(tree, obj);
	return 1;
	}


	
VOID map_dslct(LPTREE tree, WORD obj)
	{
	map_tree(tree, obj, tree[obj].ob_next, dslct_1obj);
	}

/* Why the parameters? It didn't use them */
	
VOID dslct_obj(VOID) /* (LPTREE tree, WORD obj) */
	{
	if(iconedit_flag) return;
	for (; rcs_nsel; )
		obj_redraw(ad_view, rcs_sel[--rcs_nsel] );
	set_menus();
	clr_hot();
	}

	VOID
unhid_part(obj)
	WORD	obj;
	{
	WORD	iobj;

	for (iobj = ad_view[obj].ob_head; iobj != obj; iobj = ad_view[iobj].ob_next)
		unhide_obj(ad_view, iobj);
	obj_redraw(ad_view, obj);
	}

WORD less_ob(LPTREE tree, WORD obj1, WORD obj2, WORD mode)
	{
	WORD	x1, y1, x2, y2;

	x1 = tree[obj1].ob_x;
	y1 = tree[obj1].ob_y;
	x2 = tree[obj2].ob_x;
	y2 = tree[obj2].ob_y;

	switch (mode) {
		case SORTX:
			return (x2 < x1);
		case SORTY:
			return (y2 < y1);
		case SORTXY:
			if (x1 == x2)
				return (y2 < y1);
			return (x2 < x1);
		case SORTYX:
			if (y1 == y2)
				return (x2 < x1);
			return (y2 < y1);
	        default:
			return (FALSE);

		}
	} 

VOID sort_tree(LPTREE tree, WORD pobj, WORD mode)
	{
	WORD	i, n, sobj, tobj, obj;

	n = 0;
	for (obj = tree[pobj].ob_head; obj != pobj; obj = tree[obj].ob_next)
		n++;
	if (n == 1)
		return;

	for (i = 0; ++i < n; )
		{
		for (tobj = sobj = nth_child(tree, pobj, i);
			tobj != pobj; tobj = tree[tobj].ob_next)
			if (less_ob(tree, sobj, tobj, mode))
				sobj = tobj;
		if (obj != sobj)
			objc_order(tree, sobj, i - 1);
		}
	}

VOID sort_part(WORD obj)
	{
	LPTREE	tree;
	GRECT	p;
	WORD	mode;

	objc_xywh(ad_view, obj, &p);

	ini_tree(&tree, SORTTREE);
	sel_obj(tree, SRTYITEM);

	hndl_dial(tree, 0, &p);

	if (SELECTED & tree[STOKITEM].ob_state)
		{
		mode = encode(tree, SRTXITEM, 4);
		sort_tree(ad_view, obj, mode);
		}
	map_tree(tree, ROOT, NIL, desel_obj);
	}

VOID flatten_part(WORD obj)
	{
	LPTREE	tree;
	WORD	pobj, cobj, nobj;
	WORD	x, y;

	tree = ad_view;
	x = ad_view[obj].ob_x;
	y = ad_view[obj].ob_y;

	pobj = get_parent(tree, obj);
	dslct_1obj(tree, obj);
	del_objindex(tree, obj);
	objc_delete(tree, obj);

	for (cobj = tree[obj].ob_head; cobj != obj; cobj = nobj)
		{
		tree[cobj].ob_x += x;
		tree[cobj].ob_y += y;
		nobj = tree[cobj].ob_next;
		objc_add(tree, pobj, cobj);
		}
	}
