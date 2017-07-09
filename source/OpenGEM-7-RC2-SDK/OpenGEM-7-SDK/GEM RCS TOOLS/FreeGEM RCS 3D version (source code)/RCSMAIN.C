 /*	RCSMAIN.C	10/08/84 - 1/25/85  	Tim Oren		*/
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/

#include "ppdrcs.h"
 
VOID set_filemenu(VOID)
{
	if ( rcs_state == NOFILE_STATE )
    {
		enab_obj( ad_menu, NEWITEM);
		enab_obj( ad_menu, SVASITEM);
		enab_obj( ad_menu, RVRTITEM);
	}
	else if ( rcs_state == FILE_STATE)
	{
		enab_obj( ad_menu, SAVEITEM);
		enab_obj( ad_menu, RVRTITEM);
	}
}



VOID do_redraw(WORD w_handle, GRECT *dirty, LPTREE tree, GRECT *area)
	{
	GRECT		p, r;
	WORD		i;

	gsx_moff();

	wind_get(w_handle, WF_FIRSTXYWH, &p.g_x, &p.g_y, &p.g_w, &p.g_h);
	while ( p.g_w && p.g_h )
		{
		if ( rc_intersect(area, &p) )
	  	if ( rc_intersect(dirty, &p) )
			{
 			gsx_sclip(&p);	
			if (tree ==  ad_view)	/* trivial accelerator */
				{   	 			
				objc_xywh(ad_view, ROOT, &r);
				rc_intersect(&p, &r);
				if (!rc_equal(&p, &r) || 
					rcs_state == MENU_STATE)
					gr_rect(BLUE, IP_4PATT, &p);
				}
	    		objc_draw(tree, ROOT, MAX_DEPTH, 
				p.g_x, p.g_y, p.g_w, p.g_h);
			if (tree == ad_view && tree_view() )
			for (i = rcs_nsel; i; )
				{
				objc_xywh(ad_view, rcs_sel[--i], &r);
				gsx_xbox(&r);
				if (rcs_state != ALRT_STATE)
					{
					obj_handle(ad_view, rcs_sel[i], &r);
					gr_rect(BLUE, IP_SOLID, &r);
					}
				}
			}
	  	wind_get(w_handle, WF_NEXTXYWH, 
			&p.g_x, &p.g_y, &p.g_w, &p.g_h);
		}
	gsx_mon();
	}

	VOID
hndl_redraw(w_handle, c)
	WORD		w_handle;
	GRECT		*c;
	{
	if(iconedit_flag)
	{
		do_redraw(w_handle, c, ad_itool, &itool);
		draw_clipb(TRUE, c);	/*draw the clipping*/
	}
	else
	{
		do_redraw(w_handle, c, ad_tools, &tools); 
	}
	do_redraw(w_handle, c, ad_pbx, &pbx);
	if(iconedit_flag) outl_img();	/*draw the outline of the icon/data*/
	do_redraw(w_handle, c, ad_view, &view);
	}

VOID obj_nowdraw(LPTREE tree, WORD obj)
	{
	GRECT	p;

	objc_xywh(tree, obj, &p);
	hndl_redraw(rcs_view, &p);
	}

VOID cut_part(WORD sobj, WORD dup)
	{
	LPTREE	tree;

	if (file_view())
		cut_tree(trpan_f(sobj), dup);
	else
		cut_obj(sobj, dup);
	tree = ad_tools;
	tree[CLIPBORD].ob_spec = (LPVOID)image_addr(CLIPFULL);
	if( toolp)  obj_nowdraw(ad_tools, CLIPBORD);
	else        enab_obj( ad_menu, PASTITEM);
	set_filemenu(); 		
	rcs_edited = TRUE;
	}

VOID paste_part(WORD dobj, WORD dx, WORD dy, WORD dup)
	{
	if (!ad_clip)
		return;
	if (file_view())
		paste_tree(trpan_f(dobj), dx, dy, dup);
	else
		paste_obj(dobj, dx, dy, dup);
	set_filemenu();		
	rcs_edited = TRUE;
	}

	
VOID del_part(WORD sobj)
	{
	if (file_view())  del_tree(trpan_f(sobj));
	else		      del_ob(sobj);
	set_filemenu();  	
	rcs_edited = TRUE;
	}

VOID new_part(WORD sobj, WORD dobj, WORD dx, WORD dy)
	{
	if (file_view()) new_tree(sobj, trpan_f(dobj), dx, dy);
	else		     new_obj(ad_pbx, sobj, dobj, dx, dy); 
	rcs_edited = TRUE;
	set_filemenu();
	}

VOID dup_part(WORD sobj, WORD dobj, WORD dx, WORD dy)
	{
	if (file_view()) dup_tree(trpan_f(sobj), trpan_f(dobj), dx, dy);
	else		     new_obj(ad_view, sobj, dobj, dx, dy);
	rcs_edited = TRUE;
	set_filemenu();
	}

VOID mov_part(WORD sobj, WORD dobj, WORD dx, WORD dy)
	{
	if (file_view()) mov_tree(trpan_f(sobj), trpan_f(dobj), dx, dy);
	else             mov_obj(sobj, dobj, dx, dy);
	rcs_edited = TRUE;
	set_filemenu();
	}

	
VOID type_part(WORD sobj)
	{
	if (file_view()) type_tree(trpan_f(sobj));
	else 		     type_obj(sobj);
	}


	
WORD hndl_mouse(WORD mx, WORD my)
	{
	hot_off();
	rcs_hot = objc_find(ad_tools, ROOT, MAX_DEPTH, mx, my);
	if (rcs_hot == NIL)
		{
		wait_tools();
		return (FALSE);
		}
	wait_obj(ad_tools, rcs_hot);
	if (ad_tools[rcs_hot].ob_flags & SELECTABLE)
		invert_obj(ad_tools, rcs_hot, &tools);
	else
		rcs_hot = NIL;
	return (FALSE);
	}
 
VOID drop_part(LPTREE tree, WORD sobj, GRECT *d, WORD mx, WORD my, WORD dup)
	{
	WORD	dobj;
	LPTREE	dtree;

	if (!pane_find(&dtree, &dobj, d->g_x, d->g_y))
		return;
	if (tree == ad_tools)		
		{
		if (dtree == ad_view)
			paste_part(dobj, d->g_x, d->g_y, dup);
		return;
		}

	if (tree == ad_view)
		{
		if (dtree == ad_tools)
			{
			dobj = objc_find(ad_tools, ROOT, NIL, mx, my);
			if (dobj == CLIPBORD)
				cut_part(sobj, dup);
			else if (dobj == TRASHCAN)
				del_part(sobj);
			return;
			}
		if (dtree == ad_view)
			{
			if (dup)
				dup_part(sobj, dobj, d->g_x, d->g_y);
			else
				mov_part(sobj, dobj, d->g_x, d->g_y);
			return;
			}
		return;
		}

	if (tree == ad_pbx)
		{
		if (dtree == ad_view)
			new_part(sobj, dobj, d->g_x, d->g_y);
		return;
		}
	}

VOID drag_part(LPTREE tree, WORD obj, GRECT *d, WORD *mx, WORD *my)
	{

	hot_off();	 
	if (tree == ad_pbx)        invert_obj(tree,obj,&pbx);
	else if (tree == ad_tools) invert_obj(tree,obj,&tools);
	else                       invert_obj(tree, obj, &view);
	graf_mouse(HAND, 0x0L);
	objc_xywh(tree, obj, d);

	if (tree == ad_tools && obj == CLIPBORD && ad_clip 
		&& rcs_clipkind == OBJKIND)
		{
		d->g_w = ad_clip[ROOT].ob_width;
		d->g_h = ad_clip[ROOT].ob_height;
		}
	hot_dragbox(d, &full, mx, my, tree == ad_view);
	graf_mouse(ARROW, 0x0L);
	if (tree == ad_pbx)          invert_obj(tree, obj, &pbx);
	else if (tree == ad_tools)   invert_obj(tree, obj, &tools);
	else 	                     invert_obj(tree, obj, &view);
	wait_tools();
	}


	
WORD handle_hit(LPTREE tree, WORD obj, WORD x, WORD y)
	{
	GRECT	d;

	if (tree_view() && rcs_state != ALRT_STATE)
		{
		obj_handle(tree, obj, &d);
		return inside(x, y, &d);
		}
	return (FALSE);
	}


 
VOID hndl_down(WORD mx, WORD my, LPTREE tree, WORD obj, WORD dup)
	{
	GRECT	d;

	if (tree == ad_view)
		{
		if (handle_hit(tree, obj, mx, my))
			{
			size_obj(tree, obj);
			set_filemenu();
			rcs_edited = TRUE;
			return;
			}
		if (rcs_state == MENU_STATE)	
		if (obj == THEBAR || tree[obj].ob_next == ROOT)
			return;		/* kludgy */
		if (obj == ROOT)
			return;		
		}

	if (tree == ad_tools)
	if (obj != CLIPBORD || ad_clip == 0)
		return;

	drag_part(tree, obj, &d, &mx, &my);
	drop_part(tree, obj, &d, mx, my, dup);
	return;
	}


	
WORD hndl_dblklik(LPTREE tree, WORD obj)
	{
	if (tree != ad_view)   return(FALSE);
	if (tree_view()) 	   open_obj(obj);
	else if (obj != ROOT)  open_tree(trpan_f(obj));
	if( rcs_edited )       set_filemenu();
	return(FALSE);
	}

	
VOID hndl_dsel()
	{
	if (!rcs_nsel)    return;
	if (file_view())  dselct_tree(ad_view, rcs_sel[0]);
	else              dslct_obj();
	}	     

	
VOID hndl_sel(LPTREE tree, WORD obj, WORD keystate)
	{
	WORD	shift, alt, aobj;

	if (tree != ad_view)
		{
		hndl_dsel();
		return;
		}
	if (file_view())
		{
		hndl_dsel();
		select_tree(tree, obj);
		return;
		}

	shift = keystate & (K_LSHIFT | K_RSHIFT); 
	alt = keystate & K_ALT;

	if (!shift || rcs_state == ALRT_STATE)	
		hndl_dsel();	/* Prevent delete hassles in alerts */

	if (alt && (aobj = tree[obj].ob_head) != NIL)
		for (; aobj != obj; aobj = tree[aobj].ob_next) 
			slct_obj(tree, aobj);
	else
		slct_obj(tree, obj);
	}

VOID hndl_del()
	{
	WORD	ii;
	for ( ii = rcs_nsel; ii ;del_part(rcs_sel[--ii]));
	}

	  
VOID hndl_cut()  
	{
	if ( rcs_nsel ) cut_part(rcs_sel[0], FALSE);
	}

				
VOID hndl_copy()
	{
	if ( rcs_nsel ) 
		{
		cut_part(rcs_sel[0], TRUE);
		hndl_dsel();
		}
	}

	     
VOID hndl_paste()
	{
	GRECT	d;
	WORD	mx, my, keystate, dup,button;

	if ( ad_clip )
		{
       	        graf_mkstate(&mx, &my, &button, &keystate);
       	 	dup = keystate & (K_LSHIFT | K_RSHIFT);
		drag_part(ad_tools, CLIPBORD, &d, &mx, &my);
		drop_part(ad_tools,CLIPBORD, &d, mx, my, dup);
		if ( !ad_clip ) disab_obj( ad_menu, PASTITEM);
		}
	}


				
WORD hndl_sclck(WORD mx, WORD my, WORD keystate, LPTREE *tree, WORD *obj, WORD *shifted)
	{
	WORD	ctrled;

	*shifted = keystate & (K_LSHIFT | K_RSHIFT); 
	ctrled = keystate & K_CTRL; 

	if (!pane_find(tree, obj, mx, my))
		return (FALSE);
	if (*tree == ad_view)
		{
		if (*obj != ROOT && ctrled)
			*obj = get_parent(*tree, *obj);
		return (TRUE);
		}
	else if (*obj == ROOT)
		return (FALSE);
	else
		return (TRUE);
	}


	
VOID send_redraw(WORD w_handle, GRECT *p)
	{
	rcs_rmsg[0] = WM_REDRAW;
	rcs_rmsg[1] = gl_apid;
	rcs_rmsg[2] = 0;
	rcs_rmsg[3] = w_handle;
	rcs_rmsg[4] = p->g_x;
	rcs_rmsg[5] = p->g_y;
	rcs_rmsg[6] = p->g_w;
	rcs_rmsg[7] = p->g_h;
	appl_write(gl_apid, 16, ad_rmsg);
	} 


	
VOID obj_redraw(LPTREE tree, WORD obj)
	{
	GRECT	p;

	objc_xywh(tree, obj, &p);
	rc_intersect(&view, &p);
	p.g_x -= 3; p.g_y -= 3; p.g_w += 6; p.g_h += 6;
	send_redraw(rcs_view, &p);
	}

VOID hndl_pmenu(WORD which, WORD item)
	{
	WORD	i;
	LPTREE    tree;	 
		     
	tree = ad_view;
	for (i = 0; i < rcs_nsel; i++)
		{ 
		switch (which) {
			case HOTBGCOL:
				do_bgcol(tree, rcs_sel[i], item);
				break;
			case HOTPATRN:
				do_patrn(tree, rcs_sel[i], item);
				break;
			case HOTBDCOL:
				do_bdcol(tree, rcs_sel[i], item);
				break;
			case HOTTHICK:
				do_thick(tree, rcs_sel[i], item);
				break;
			case HOTFGCOL:
				do_fgcol(tree, rcs_sel[i], item);
				break;
			case HOTRULE:
				do_rule(tree, rcs_sel[i], item);
				break;
			case HOTPOSN:
				do_posn(tree, rcs_sel[i], item);
				break;
			case HOTSWTCH:
				do_swtch(tree, rcs_sel[i], item);
				break;
			default:
				return;
			}		     
		obj_redraw(ad_view, rcs_sel[i]);
		}

	if (which == HOTSWTCH)
		{
		if (item == HDDNPOP)
			dslct_obj();
		set_switch();
		}

	rcs_edited = TRUE;
	set_filemenu();
	}


	
VOID hndl_tlbx(LPTREE tree, WORD obj, WORD dup)
	{
	WORD	pmenu, i;

	if ( !(tree[obj].ob_flags & SELECTABLE))
		return;
	if (!rcs_nsel)
		return;
	hot_off();
	if (obj == TRASHCAN )
		for (i = rcs_nsel; i; del_part(rcs_sel[--i]));
	else if (obj == CLIPBORD )
		cut_part(rcs_sel[0], dup);
	else 
		if ( (pmenu = hndl_pop(obj, FALSE)) != NIL)
			hndl_pmenu(obj, pmenu); 
	wait_tools();
	}

WORD hndl_button(WORD mx, WORD my, WORD button, WORD keystate, WORD clicks)
	{
	WORD	obj_sel, dup_sel;
	LPTREE	tree_sel;

	graf_mkstate(&mx, &my, &button, &keystate);
	if ( !hndl_sclck( mx, my, keystate, &tree_sel, &obj_sel, &dup_sel) )
		{
		hndl_dsel();
		return(FALSE);
		}
	if (tree_sel == ad_tools && clicks == 1 && !(button & 0x1))
		{
		hndl_tlbx(tree_sel, obj_sel, dup_sel);
		return (FALSE);
		}

	if ( clicks == 1 )
		{
		if ( button & 0x0001 )
			hndl_down( mx, my, tree_sel, obj_sel, dup_sel );
		else
			hndl_sel(tree_sel, obj_sel, keystate);
		}
	else
		hndl_dblklik(tree_sel, obj_sel );
	return (FALSE);
	}


	
VOID hndl_open()
	{
	if (tree_view())
	    	open_obj(rcs_sel[0]);
    	else if (!rcs_nsel)	      
		{
			if(open_rsc())
				rcs_edited = FALSE;
		}
	else	 
		open_tree(trpan_f(rcs_sel[0]));
	}

	
VOID hndl_clos()
	{
	if (tree_view())
	 	{
		clos_tree();
		}
	else if (rcs_state == FILE_STATE)
       		if(clos_rsc())	 
			rcs_edited = FALSE;
	}
	
	
WORD hndl_filemenu(WORD item)
	{
	WORD	done;

	done = FALSE;

	switch (item) {
		case NEWITEM:		  
			if ( iconedit_flag)
				{
				if (new_img() )
					icn_edited = FALSE;
				}
			else
				if(new_rsc())
					rcs_edited = FALSE;
			break;
		case OPENITEM:	 
			if ( iconedit_flag)
				{
	 			if(icnopen())
					icn_edited = FALSE;	    
				}
			else
				hndl_open();
			break; 
		case MERGITEM:
			merge_rsc();
			rcs_edited = TRUE;
			break;
		case CLOSITEM:	 
			if (iconedit_flag)
				done = TRUE;
			else
				hndl_clos();
			break;
		case SAVEITEM:
			if (iconedit_flag)		
				{
				if(save_icn())
					icn_edited = FALSE;	       
				}
			else		 
				{
				save_rsc();
				rcs_edited = FALSE;
				}
			break;
		case SVASITEM:	 
			if( iconedit_flag)
				{
				if(svas_icn())
					icn_edited = FALSE;
				}
			else
				if(svas_rsc())
					rcs_edited = FALSE;
			break;
		case RVRTITEM:
			if (iconedit_flag)	       
				{
				if(rvrt_icn())
					icn_edited = FALSE;
				}
			else
				if(rvrt_rsc())
					rcs_edited = FALSE;
			break;
		case QUITITEM:				      
			done =	quit_rsc();
 		}
	if( rcs_edited )
		set_filemenu();
	if(iconedit_flag)
		set_icnmenus();
	return (done);													
	}



WORD hndl_editmenu(WORD item)
	{
	WORD	done, obmsb, new_h, new_w;
	LPTREE	tree;

	done = FALSE;

	switch (item) {
		case DELITEM:
			if(iconedit_flag) del_img(TRUE, 3);
			else hndl_del();
			break;
		case CUTITEM:
			if(iconedit_flag) cut_img();
			else hndl_cut();
			break; 
		case COPYITEM:
			if(iconedit_flag) copy_img(TRUE);
			else hndl_copy();
			break;
		case PASTITEM:
			if(iconedit_flag){
			    paste_img = TRUE;
			    rubrec_off();
			    mflags |= MU_M2;
			}
			else hndl_paste();
			break;
		case INVITEM:
			graf_mouse(M_OFF,0x0L);
			invrt_img();	       
			graf_mouse(M_ON,0x0L);
			break;
		case SOLIDIMG:		      
			graf_mouse(M_OFF,0x0L);
			solid_img();       
			fb_redraw();
			graf_mouse(M_ON,0x0L);
			break;		 
/*		case CLEARITM:
			clear_img();
			break;			
		case DATTOMSK:
			if (gl_isicon)	      
				{
				graf_mouse(M_OFF,0x0L);
				data_to_mask(TRUE);    
				graf_mouse(M_ON,0x0L);
				}
			break;
		case MSKTODAT:
			if (gl_isicon)
				{
				graf_mouse(M_OFF,0x0L);
				data_to_mask(FALSE);   
				graf_mouse(M_ON,0x0L);
				}
			break;
*/		case SIZEITEM:			
			icon_size(&new_w, &new_h);
			if( new_w != gl_wimage || new_h != gl_himage)
			{
			     tree = save_tree;			
			     obmsb = 1;	  
			     if (gl_isicon) tree[save_obj].ob_type = (obmsb<< 8)| G_ICON;
			     else           tree[save_obj].ob_type = (obmsb<< 8)| G_IMAGE;
			     if (gl_isicon)
				 { 
					gl_icnspec->ib_wicon = new_w;
					gl_icnspec->ib_hicon = new_h;
				 }
			     else
				 {     
					((LPBIT)gl_icnspec)->bi_wb = new_w >> 3;
					((LPBIT)gl_icnspec)->bi_hl = new_h;
				 }
			     icn_init(TRUE);
			     icn_edited = FALSE;
			}
			else if(scale == CLEARSZ) /*same size, but clear it*/
				clear_img();
			break;
		default:
			break;
		      }
     	return( done );
	}

	
	WORD
hndl_omenu(item)
	WORD	item;
	{
	WORD	done;

	done = FALSE;

	switch (item) {
		case RNAMITEM:
			name_tree(trpan_f(rcs_sel[0]));
			break;
		case TYPEITEM:
			type_part(rcs_sel[0]);
			break;
		case INFITEM:
			info_dial(rcs_nsel? rcs_sel[0]: NIL);
			break;
		case LOADITEM:
			load_part(rcs_sel[0]);
			break;
		case ICNEDITM:
			icon_edit();
			break;
	  	default:
			break;
		}
       if(rcs_edited )
	    set_filemenu();
	return (done);
	} 

	WORD
hndl_hmenu(item)
	WORD	item;
	{
	WORD	done;

	done = FALSE;

	switch (item) {
		case SRTITEM:
			sort_part(rcs_sel[0]);
			break;
		case UNHDITEM:
			unhid_part(rcs_sel[0]);
			break;
		case FLTITEM:
			flatten_part(rcs_sel[0]);
			break;
		default:
			break;
		}
	rcs_edited = TRUE;
	return (done);
	} 


	
VOID hndl_hidsho(WORD *flag, WORD item, WORD hide, WORD show)
	{
	WORD	str;


	*flag = !(*flag);
	str = (*flag)? hide: show;
	menu_text(ad_menu, item, string_addr(str));
	ini_panes();
	if (file_view())
		redo_trees();
	else
		{
		dslct_obj();
		view_objs();
		}
	if ( !toolp && ad_clip)
		enab_obj( ad_menu, PASTITEM);
	send_redraw(rcs_view, &full);
	} 


	
WORD hndl_globmenu(WORD item)
	{
	WORD	done;

	done = FALSE;

	switch (item) {
		case OUTPITEM:
			outp_dial();
			break;
		case SAFEITEM:
			safe_dial();
			break;
		case SVOSITEM:
			wrte_inf();
			break;
		case PARTITEM:
		        if( iconedit_flag)
			     save_fat(FALSE);
			hndl_hidsho(&partp, PARTITEM, HIDEPART, SHOWPART);
			break;
		case TOOLITEM:		
			if (iconedit_flag)
			     save_fat(FALSE);	
			hndl_hidsho(&toolp, TOOLITEM, HIDETOOL, SHOWTOOL);
	 		break;
		default:
			break;
		}
	return (done);
	} 



WORD hndl_menu(WORD title, WORD item)
	{
	WORD	done;

	done = FALSE;
	paste_img = FALSE;
	switch (title) {
		case DESKMENU:
			if (item == ABOUITEM)
				about_dial();
			break;
		case FILEMENU:
			done = hndl_filemenu(item);
			break;
		case GLOBMENU:
			done = hndl_globmenu(item);
			break;		
		case EDITMENU:
			done = hndl_editmenu(item);
			break;
		case OPTNMENU:
			done = hndl_omenu(item);
			break;
		case ARRMENU:
			done = hndl_hmenu(item);
			break;
		}
	menu_tnormal(ad_menu,title,TRUE);
	if ( iconedit_flag)
		set_icnmenus();

	return (done);
	}


	
VOID hndl_arrowed(WORD w_handle, WORD kind)
	{
	if (file_view())
		{
		switch (kind) {
			case WA_LFPAGE:
			case WA_RTPAGE:
			case WA_LFLINE:
			case WA_RTLINE:
				return;
			case WA_UPPAGE:
				do_trsinc(-fit_vtrees() * fit_htrees());
				break;
			case WA_DNPAGE:
				do_trsinc(fit_vtrees() * fit_htrees());
				break;
			case WA_UPLINE:
				do_trsinc(-fit_htrees());
				break;
			case WA_DNLINE:
				do_trsinc(fit_htrees());
				break;
			}
		redo_trees();
		}
	else
		{
		switch (kind) {
			case WA_LFPAGE:
				do_hsinc(-view.g_w);
				break;
			case WA_RTPAGE:
				do_hsinc(view.g_w);
				break;
			case WA_LFLINE:
				do_hsinc(-2 * gl_wchar);
			 	break;
			case WA_RTLINE:
				do_hsinc(2 * gl_wchar);
				break;
			case WA_UPPAGE:
				do_vsinc(-view.g_h);
				break;
			case WA_DNPAGE:
				do_vsinc(view.g_h);
				break;
			case WA_UPLINE:
				do_vsinc(-gl_hchar);
				break;
			case WA_DNLINE:
				do_vsinc(gl_hchar);
				break;
			}
		dslct_obj();
		view_objs();
		}
	}


	
VOID hndl_hslid(WORD w_handle, WORD h_set)
	{
	WORD	x;

	if (file_view())
		return;
	x = max(0, ad_view[ROOT].ob_width - view.g_w); 
	x = (WORD) umul_div(x, h_set, 1000);
	rcs_xpan = clamp_xs(ad_view, snap_xs(x));
	dslct_obj();
	view_objs();
	}


	
VOID hndl_vslid(WORD w_handle, WORD v_set)
	{
	WORD	y;

	if (file_view())
		{
		y = max(0, fit_htrees() * (need_vtrees() - fit_vtrees()));
		y = mul_div(y, v_set, 1000);
		rcs_trpan = clamp_trs(snap_trs(y));
		redo_trees();
		}
	else
		{
		y = max(0, ad_view[ROOT].ob_height - view.g_h); 
		y = (WORD) umul_div(y, v_set, 1000);
		rcs_ypan = clamp_ys(ad_view, snap_ys(y));
		dslct_obj();
		view_objs();
		}
	} 		

WORD hndl_msg(VOID)
	{
	WORD 	done;

	done = FALSE;

	switch ( rcs_rmsg[0] )	{
		case MN_SELECTED:
			done = hndl_menu(rcs_rmsg[3], rcs_rmsg[4]);
			break;
		case WM_REDRAW:
			hndl_redraw(rcs_rmsg[3], (GRECT *) &rcs_rmsg[4]); 
			break;
		case WM_CLOSED:
			hndl_clos();
			if( rcs_edited )
				set_filemenu();
     			break;
		case WM_TOPPED:
			wind_set(rcs_rmsg[3], WF_TOP, 0, 0, 0, 0);
			break;
		case WM_ARROWED:
			hndl_arrowed(rcs_rmsg[3], rcs_rmsg[4]);
			break;
		case WM_HSLID:
			hndl_hslid(rcs_rmsg[3], rcs_rmsg[4]);
			break;
		case WM_VSLID:
			hndl_vslid(rcs_rmsg[3], rcs_rmsg[4]);
			break;
		default:
			break;
		}
	rcs_rmsg[0] = 0;
	return(done);
	} 


BOOLEAN hndl_keybd( WORD key )
{
	WORD title, item, done ;

    done = FALSE ;
    title = FILEMENU ;
    switch ( key )
	{
	case ALT_O :title = GLOBMENU ; 
		    item = OUTPITEM ;
		    break ;
	case ALT_S :title = GLOBMENU ; 
		    item = SAFEITEM ;
		    break ;
	case ALT_R :title = GLOBMENU;
		    item = SVOSITEM;
		    break;	
	case ALT_P :title = GLOBMENU ; 
		    item = PARTITEM ;
		    break ;
	case ALT_H :title = GLOBMENU;
		    item = TOOLITEM ;
		    break ;
	case ALT_I :title = OPTNMENU;
		    item = INFITEM ;
		    break ;
	case ALT_N :title = OPTNMENU;
		    item = RNAMITEM ;
		    break ;    
	case ALT_T :title = OPTNMENU;
		    item = TYPEITEM;
		    break;
	case ALT_L :title = OPTNMENU;
		    item = LOADITEM;
		    break;
	case ALT_E :title = OPTNMENU;
		    item = ICNEDITM;
		    break;
	case ALT_F :title = ARRMENU;
		    item = SRTITEM;
		    break;
	case ALT_U :title = ARRMENU;
		    item = UNHDITEM;
		    break;
	case CTL_W :item = NEWITEM;
		    break;
	case CTL_O :item = OPENITEM;
		    break;
	case CTL_N :item = MERGITEM;
		    break;
	case CTL_C :item = CLOSITEM;
		    break;
	case CTL_V :item = SAVEITEM;
		    break;
	case CTL_M :item = SVASITEM;
		    break;
	case CTL_A :item = RVRTITEM;
		    break;
	case ALT_C :title = EDITMENU;
		    item = CUTITEM;
		    break;
	case ALT_Y :title = EDITMENU;
		    item = COPYITEM;
		    break;
	case ALT_A :title = EDITMENU;
		    item = PASTITEM;
		    break;
	case ALT_D :title = EDITMENU;
		    item = DELITEM;
		    break;
	case CTL_Q :item = QUITITEM;
		    break;	
	default :   return( done ) ;
	} /* switch */
    if ( ! ( DISABLED & ad_menu[item].ob_state) ) 
	done = hndl_menu( title, item ) ;
    return( done ) ;
} /* hndl_keybd */



VOID rcs_main(VOID)
	{
	WORD flags, done; 
	WORD ev_which;
	UWORD rets[6];
	/* initialize for event loop */
	flags = MU_BUTTON | MU_MESAG | MU_M1 | MU_KEYBD;  
	ad_rmsg = ADDR(rcs_rmsg);
	done = FALSE;
	do	
	{
	   ev_which = evnt_multi(flags, 0x02, 0xff, 0x01,
	   	      wait_io, wait.g_x, wait.g_y, wait.g_w, wait.g_h,
		      0, 0, 0, 0, 0,
		      ad_rmsg, 0, 0,
		      &rets[0], &rets[1], &rets[2],
		      &rets[3], &rets[4], &rets[5]);

	   wind_update(TRUE);
	   if (ev_which & MU_M1)
		done = (hndl_mouse(rets[0], rets[1]));
           if (ev_which & MU_BUTTON)
		done = (hndl_button(rets[0], rets[1], rets[2], rets[3], rets[5]));
	   if (ev_which & MU_MESAG)
		done =  (hndl_msg());		       
           if ( ev_which & MU_KEYBD )
	        done = hndl_keybd( rets[ 4 ] ) ;
           wind_update(FALSE);
	}
	while (!done) ;
	}


	
WORD GEMAIN(WORD argc, BYTE **ARGV)
	{
	WORD	rcs_term;

	if ( !(rcs_term = rcs_init()) )
		rcs_main();
	rcs_exit(rcs_term);

	return 0;
	}
	
