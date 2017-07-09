/*	RCSTREES.C	11/6/84 -  1/25/85 	Tim Oren		*/
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
#include "ppdrcs.h"


	WORD
trpan_f(obj)
	WORD	obj;
	{
	if (obj == ROOT)
		return (ROOT);
	else
		return (obj + rcs_trpan);
	}

	WORD
trunpan_f(trnum)
	WORD	trnum;
	{
	if (trnum == 0)
		return (0);
	else
		return (trnum - rcs_trpan);
	} 
	
	WORD
fit_vtrees()
	{
	return view.g_h / (ICON_H + MIN_HINT);
	}

	WORD
fit_htrees()
	{
	return view.g_w / (ICON_W + MIN_WINT);
	}

	WORD
need_vtrees()
	{
	return (1 + (head->rsh_ntree - 1) / max(1, fit_htrees()));
	}

	WORD
snap_trs(tr)
	WORD	tr;
	{
	WORD	fh;

	tr += (fh = fit_htrees()) / 2;
	return (tr - tr % fh);
	}

	WORD
clamp_trs(tr)
	WORD	tr;
	{
	tr = min(tr, fit_htrees() * (need_vtrees() - fit_vtrees()));
	return max(0, tr);
	}

	VOID
do_trsinc(inc)
	WORD	inc;
	{
	rcs_trpan = clamp_trs(rcs_trpan + inc);
	}

	VOID
view_trees()
	{
	WORD	nobj, iobj;
	WORD	kind, where, nh; 
	LPTREE	tree;
	LPICON  iconad;

	tree = ad_pbx;
	rcs_work[0].ob_next = NIL;	/* construct tree root */
	rcs_work[0].ob_type = G_BOX;
	rcs_work[0].ob_flags = NONE;
	rcs_work[0].ob_state = NORMAL;
	rcs_work[0].ob_spec  = (LPVOID)0x00000007L;
	rcs_work[0].ob_x = view.g_x;
	rcs_work[0].ob_y = view.g_y;
	rcs_work[0].ob_width = view.g_w;
	rcs_work[0].ob_height = view.g_h;

	if (!head->rsh_ntree)
		{
		rcs_work[0].ob_head = NIL;
		rcs_work[0].ob_tail = NIL;
		return;
		}

	nobj = min(VIEWSIZE, head->rsh_ntree - rcs_trpan); 
	nobj = min(nobj, fit_vtrees() * (nh = fit_htrees()));

	rcs_work[0].ob_head = 1;	/* root pointers */
	rcs_work[0].ob_tail = nobj;

	for (iobj = 0; iobj++ < nobj; )
		{
		if (iobj < nobj)
			rcs_work[iobj].ob_next = iobj + 1;
		else
			rcs_work[iobj].ob_next = ROOT;
		rcs_work[iobj].ob_head = NIL;
		rcs_work[iobj].ob_tail = NIL;
		rcs_work[iobj].ob_flags = NONE;
		rcs_work[iobj].ob_state = NORMAL;
		rcs_work[iobj].ob_type = G_ICON;
		rcs_work[iobj].ob_spec = ADDR(&rcs_icons[iobj-1]);
		rcs_work[iobj].ob_width = ICON_W;
		rcs_work[iobj].ob_height = ICON_H + gl_hschar;
		rcs_work[iobj].ob_x = ((iobj - 1) % nh) * (ICON_W + MIN_WINT);
		rcs_work[iobj].ob_y = ((iobj - 1) / nh) * (ICON_H + MIN_HINT);

		where = find_tree(iobj + rcs_trpan - 1);
		kind = get_kind(where);
		iconad = (LPICON)tree[rcs_typ2icn[kind]].ob_spec;
		LBCOPY(ADDR(&rcs_icons[iobj-1]),	
			ADDR(iconad), sizeof(ICONBLK));	
		rcs_icons[iobj-1].ib_ptext = 		/* address of tree */
			ADDR( get_name(where) );
		rcs_icons[iobj-1].ib_ytext = ICON_H;
		}
	}

	VOID
slid_trees()
	{
	WORD	v_size; 

	v_size = min(1000, (WORD) umul_div(fit_vtrees(), 1000, need_vtrees()));
	set_slsize(rcs_view, 1000, v_size);
	v_size = max(1, fit_htrees() * (need_vtrees() - fit_vtrees()));
	v_size = mul_div(rcs_trpan, 1000, v_size);
	set_slpos(rcs_view, 0, v_size); 
	}

	VOID
redo_trees()
	{
	dselct_tree(ad_view, rcs_sel[0]);
	rcs_trpan = clamp_trs(rcs_trpan); 	/* in case of resize */
	slid_trees();
	view_trees();
	send_redraw(rcs_view, &view);
	}

	VOID
del_tree(sobj)
	WORD	sobj;
	{
	WORD	nobj, iobj; 

	if (rcs_lock)
		{
		hndl_locked();
		return;
		}
	if (sobj == ROOT)
		return;

	nobj = head->rsh_ntree;
	head->rsh_ntree = --nobj;
	
	zap_objindex( tree_addr(sobj - 1), ROOT);
	for (iobj = sobj; iobj <= nobj; iobj++)
		{
		LPLTREE tp = tree_ptr(iobj - 1);

		*tp = tree_addr(iobj);
		}
	redo_trees();
	}

VOID cut_tree(WORD sobj, WORD dup)
	{
	WORD	where;
	LPTREE	tree;

	if (rcs_lock)
		{
		hndl_locked();
		return;
		}
	tree = tree_addr(sobj - 1);
	where = find_value((LPBYTE) tree);
	rcs_clipkind = get_kind(where);
	ad_clip = copy_tree(tree, ROOT, TRUE);
	if (!dup)
		del_tree(sobj);
	}

	WORD
posn_tree(dobj, dx, dy)
	WORD	dobj, dx, dy;
	{
	WORD	iobj, iy, nh, ix, nobj;

	nobj = head->rsh_ntree;
	if (dobj == ROOT)
		{
		ix = 1 + (dx - view.g_x + MIN_WINT) / (ICON_W + MIN_WINT);
		iy = (dy - view.g_y) / (ICON_H + MIN_HINT);
		nh = view.g_w / (ICON_W + MIN_WINT);
		iobj = iy * nh + ix + rcs_trpan;
		dobj = min( iobj, nobj);
		}
	return (dobj);
	}

LPTREE ins_tree(LPTREE stree, WORD sobj, WORD kind, WORD dobj, WORD dx, WORD dy)
	{
	LPTREE	dtree;
	BYTE	name[9];

	dtree = copy_tree(stree, sobj, TRUE);
	unique_name(&name[0], "TREE%W", head->rsh_ntree + 1);
	if (new_index( (LPBYTE) dtree, kind, &name[0]) == NIL)
		{
		hndl_alert(1, string_addr(STNFULL));
		return (LPTREE)(-1);
		}
	add_trindex(dtree);
	dobj = mov_tree(head->rsh_ntree, dobj, dx, dy);
	select_tree(ad_view, trunpan_f(dobj));
	name_tree(dobj);
	dselct_tree(ad_view, trunpan_f(dobj));
	return (dtree);
	}

	VOID
paste_tree(dobj, dx, dy, dup)
	WORD	dobj, dx, dy, dup;
	{
	if (!tree_kind(rcs_clipkind))
		return;
	if (ins_tree(ad_clip, ROOT, rcs_clipkind, dobj, dx, dy) == NOTREE)
		return;
	if (!dup)
		clr_clip();
	}

	VOID
new_tree(sobj, dobj, dx, dy)
	WORD	sobj, dobj, dx, dy;
	{
	WORD	w, h;
	LPTREE	maddr, tree;
	LPVOID obspec;

	if (rcs_lock)
		{
		hndl_locked();
		return;
		}
	switch (sobj) {
		case MENUICON:
			ini_tree(&maddr, NEWMENU);
			ins_tree(maddr, ROOT, MENU, dobj, dx, dy);
			break;
		case ALRTICON:
			ini_tree(&tree, ALRTPBX);
			obspec = tree[APBXIMG0].ob_spec;
			ini_tree(&maddr, NEWALRT);
			tree = ins_tree(maddr, ROOT, ALRT, dobj, dx, dy);
			if (tree == NOTREE)
				break;
			tree[ALRTIMG].ob_spec = obspec;
			fix_alert(tree);
			break;
		case PANLICON:
		case FREEICON:
			ini_tree(&maddr, NEWPANEL);
			tree = ins_tree(maddr, ROOT, 
				(sobj == PANLICON)? PANL: FREE, dobj, dx, dy);
			if (tree == NOTREE)
				break;
			tree[ROOT].ob_height = view.g_h;
			tree[ROOT].ob_width  = view.g_w;
			break;
		case DIALICON:
			ini_tree(&maddr, NEWDIAL);
			tree = ins_tree(maddr, ROOT, DIAL, dobj, dx, dy);
			if (tree == NOTREE)
				break;
			w = view.g_w;
			h = view.g_h;
			snap_wh(&w, &h);
			tree[ROOT].ob_height = h;
			tree[ROOT].ob_width  = w;
			break;
		default:
			break;
		}	
	}

	VOID
dup_tree(sobj, dobj, dx, dy)
	WORD	sobj, dobj, dx, dy;
	{
	WORD	where, kind;
	LPTREE	stree; 

	if (rcs_lock)
		{
		hndl_locked();
		return;
		}
	stree = tree_addr(sobj - 1);
	where = find_value((LPBYTE) stree);
	kind = get_kind(where);
	ins_tree(stree, ROOT, kind, dobj, dx, dy);
	}

	WORD
mov_tree(sobj, dobj, dx, dy)
	WORD	sobj, dobj, dx, dy;
	{
	WORD	iobj; 
	LPTREE	sv_link;

	if (rcs_lock)
		{
		hndl_locked();
		return 0;
		}
	dobj = posn_tree(dobj, dx, dy);
	if (sobj == dobj)
		{
		redo_trees();
		return (dobj);
		}
	
	sv_link = tree_addr(sobj - 1);

	if (sobj < dobj)
		for (iobj = sobj; iobj < dobj; iobj++ )
			*tree_ptr(iobj - 1) =  tree_addr(iobj);
	else
		for (iobj = sobj ; iobj > dobj ; iobj-- )
			*tree_ptr(iobj - 1) =  tree_addr(iobj - 2);
	
	*tree_ptr(dobj - 1) = sv_link; 

	redo_trees();
	return (dobj);
	}

	
	VOID
open_tree(obj)
	WORD	obj;
	{
	WORD	kind, new, where; 
	GRECT	o;
	LPTREE	tree;
	WORD	themenu, menu;
 
	tree = ad_view;
	select_tree(tree, trunpan_f(obj));
	where = find_tree(obj - 1);
	kind = get_kind(where);

	switch (kind) { 
		case UNKN:
			type_tree(obj);
			dselct_tree(tree, trunpan_f(obj));
			return;
		case PANL:
			new = PANL_STATE;
			break;
		case DIAL:
			new = DIAL_STATE;
			break;
		case ALRT:
			new = ALRT_STATE;
			break;
		case MENU:
			new = MENU_STATE;
			break;
		case FREE:
			new = FREE_STATE;
			break;
		default:
			break;
		}

	rcs_svfstat = rcs_state;
	new_state(new);
	objc_xywh(tree, trunpan_f(obj), &o);
	if (!rcs_xpert)
		graf_growbox(o.g_x, o.g_y, o.g_w, o.g_h, 
			view.g_x, view.g_y, view.g_w, view.g_h);
	set_title( get_name(where) );
	
	tree = tree_addr(obj - 1);

	if (rcs_state == MENU_STATE)
		{			 
		desk_menufix( tree);
		themenu = tree[ROOT].ob_tail;
		for (menu = tree[themenu].ob_head; menu != themenu;
			menu = tree[menu].ob_next)
			hide_obj(tree, menu);
		rcs_menusel = FALSE;
		}
	dselct_tree(ad_view, rcs_sel[0]);

	rcs_xtsave = tree[ROOT].ob_x;
	rcs_ytsave = tree[ROOT].ob_y;
	rcs_xpan = rcs_ypan = 0;
	ad_view = tree;
	view_objs();
	}	

	VOID
clos_tree()
	{
	WORD	themenu, menu;

	if (rcs_state == MENU_STATE)
		{
		themenu = ad_view[ROOT].ob_tail;
		for (menu = ad_view[themenu].ob_head; menu != themenu;
			menu = ad_view[menu].ob_next)
			unhide_obj(ad_view, menu);
		undo_menufix(ad_view);
		}

	dslct_obj();

	ad_view[ROOT].ob_x = rcs_xtsave;
	ad_view[ROOT].ob_y = rcs_ytsave;
	new_state(rcs_svfstat);

	if (!rcs_xpert)
		graf_shrinkbox(view.g_x + view.g_x / 2, 
			view.g_y + view.g_h / 2, 0, 0,
			view.g_x, view.g_y, view.g_w, view.g_h);
	if (rcs_state == FILE_STATE)
		rsc_title();
	else
		clr_title();
	ad_view = ADDR(&rcs_work[0]);
	redo_trees();
	}

	VOID
name_tree(obj)
	WORD	obj;
	{
	WORD	where ,exitobj, ok;
	BYTE	name[9];
	LPTREE	tree;
	GRECT	p;

	objc_xywh(ad_view, trunpan_f(obj), &p); 

	ini_tree(&tree, NAMETREE);
	where = set_obname(tree, NAMEITEM, name, tree_addr(obj - 1), 0);

	do {
		exitobj = hndl_dial(tree, NAMEITEM, &p);
		desel_obj(tree, exitobj);
		ok = DEFAULT & tree[exitobj].ob_flags;
		} while (ok && !name_ok(name, where, FALSE));

	if (ok)
		{
		rcs_edited = TRUE;
		get_obname(name, tree_addr(obj - 1), 0);
		redo_trees();
		}
	else
		dselct_tree(ad_view, trunpan_f(obj));
	}  

VOID type_tree(WORD obj)
	{
	WORD	where, kind, exitobj;
	LPTREE	tree;
	GRECT	p;

	objc_xywh(ad_view, trunpan_f(obj), &p); 

	ini_tree(&tree, TYPETREE);
	where = find_tree(obj - 1);
	kind = get_kind(where);
	if (!tree_kind(kind))
		return;
	sel_obj(tree, UNKNITEM + kind);

	exitobj = hndl_dial(tree, 0, &p);

	if (DEFAULT & tree[exitobj].ob_flags)
		{
		rcs_edited = TRUE;
		set_kind(where, encode(tree, UNKNITEM, 6));
		redo_trees();
		}
	else
		dselct_tree(ad_view, trunpan_f(obj));

	map_tree(tree, ROOT, NIL, desel_obj);	/* clear radio buttons */
	}  

	VOID
select_tree(tree, obj)
	WORD	obj;
	LPTREE	tree;
	{
	if (obj == ROOT)
		return;
	rcs_nsel = 1;
	rcs_sel[0] = obj;
	enab_obj(ad_menu, OPENITEM);
	enab_obj(ad_menu, DELITEM);
	enab_obj(ad_menu, COPYITEM);
	enab_obj(ad_menu, CUTITEM);
	if (!rcs_lock)
		{
		enab_obj(ad_menu, RNAMITEM);
		enab_obj(ad_menu, TYPEITEM);
		}
	sel_obj(tree, obj);
	obj_nowdraw(tree, obj);
	if (!rcs_lock)
		{
		sble_obj(ad_tools, CLIPBORD);
		sble_obj(ad_tools, TRASHCAN);
		}
	}

VOID dselct_tree(LPTREE tree, WORD obj)
	{
	if (rcs_nsel)
		{
		rcs_nsel = 0;
		desel_obj(tree, obj);
		obj_redraw(tree, obj);
		}
	set_menus();
	clr_hot();
	}

