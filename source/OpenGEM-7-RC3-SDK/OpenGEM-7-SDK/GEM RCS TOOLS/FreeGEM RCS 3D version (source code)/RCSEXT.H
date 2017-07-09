   /* external function declarations for rcs2 */
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
EXTERN	VOID	init_img();
EXTERN	VOID	rast_op();
EXTERN	LONG	SEG_OFF();
EXTERN	VOID	B_MOVE();
EXTERN	VOID	MAGNIFY();
EXTERN	WORD	ok_rvrt_files();
EXTERN  WORD    cont_rcsinit();
EXTERN  VOID    icon_edit();
EXTERN	VOID	undo_menufix();
EXTERN	VOID	desk_menufix();
EXTERN	VOID	ob_relxywh();
EXTERN  WORD	shel_find();
EXTERN	WORD	quit_rsc();
EXTERN  VOID	slid_objs();
EXTERN  VOID    slid_trees();
EXTERN	VOID	read_inf();
EXTERN	VOID	set_rootxy();
EXTERN	VOID	wrte_inf();
EXTERN  VOID    movup();
EXTERN  VOID    del_objindex();
EXTERN	VOID    gsx_1code();
EXTERN	WORD	gl_apid;
EXTERN	VOID	movs();
EXTERN	WORD	strcmp();
EXTERN	WORD	hndl_menu();
EXTERN	VOID 	set_sliders();
EXTERN	VOID	LLSTRCPY();
EXTERN	WORD	LLSTRCMP();
EXTERN VOID    count_free();
EXTERN VOID    dcomp_tree();
 /*	RCSINIT.C	6/21/85		Tim Oren		*/


#if PCDOS
EXTERN	WORD	DOS_BX;
#endif

EXTERN  LONG    dos_avail();
EXTERN	WORD	ini_tree();		/* in RCSLIB.C */
EXTERN	LONG	image_addr();
EXTERN	VOID	trans_bitblk();
EXTERN	VOID	trans_obj();
EXTERN	VOID	rc_copy();
EXTERN	VOID	set_slsize();
EXTERN	UWORD	umul_div();
EXTERN	VOID	view_tools();		/* in RCSMAIN.C */
EXTERN	VOID	view_parts();
EXTERN	VOID	clr_title();
EXTERN	VOID	send_redraw();
EXTERN	WORD	rsrc_load();
EXTERN	WORD	wind_get();
EXTERN	WORD	wind_create();
EXTERN	WORD	wind_open();
EXTERN	WORD	appl_init();
EXTERN	WORD	shel_read();
EXTERN	WORD	wind_update();
EXTERN	WORD	graf_handle();
EXTERN	WORD	v_pline();
EXTERN  WORD    v_arc();
EXTERN	WORD	vsf_color();
EXTERN	VOID	gsx_vopen();
EXTERN	WORD	gsx_start();
EXTERN	VOID	mouse_form();
EXTERN	WORD	menu_bar();
EXTERN	VOID	new_state();
EXTERN	VOID	ini_buff();
EXTERN	VOID	get_path();
EXTERN	VOID	rsc_title();
EXTERN	WORD	rvrt_files();
EXTERN	VOID	view_trees();
EXTERN	WORD	dos_free();
EXTERN	WORD	wind_close();
EXTERN	WORD	wind_delete();
EXTERN	WORD	rsrc_free();
EXTERN	VOID	gsx_vclose();
EXTERN	WORD	appl_exit();
EXTERN	VOID	wait_tools();
/*	RCSMAIN.C	10/08/84 - 1/25/85  	Tim Oren		*/

EXTERN 	WORD	mul_div();
EXTERN	WORD	get_parent();
EXTERN  WORD	rc_intersect();
EXTERN	WORD	open_rsc();		/* in RCSFILES.C */
EXTERN	VOID	save_rsc();
EXTERN	WORD	svas_rsc();
EXTERN	WORD	rvrt_rsc();
EXTERN	WORD	new_rsc();
EXTERN	VOID	del_tree();		/* in RCSTREES.C */
EXTERN	VOID	new_tree();
EXTERN  WORD	mov_tree();
EXTERN	VOID	open_tree();
EXTERN	VOID	clos_tree();
EXTERN	VOID	name_tree();
EXTERN  VOID	select_tree();
EXTERN	VOID	dselct_tree();
EXTERN	VOID	dup_tree();
EXTERN	VOID	cut_tree();
EXTERN	VOID	paste_tree();
EXTERN	WORD	fit_htrees();
EXTERN	WORD	need_vtrees();
EXTERN	WORD	fit_vtrees();
EXTERN	VOID	do_trsinc();
EXTERN	WORD	trpan_f();
EXTERN	VOID	del_ob();		/* in RCSOBJS.C */
EXTERN	WORD	new_obj();
EXTERN	VOID	mov_obj();
EXTERN	VOID	open_obj();
EXTERN	VOID	type_obj();
EXTERN  VOID	size_obj();
EXTERN	VOID	unhid_part();
EXTERN	VOID	flatten_part();
EXTERN	VOID	sort_part();
EXTERN	VOID	slct_obj();
EXTERN	VOID	dslct_obj();
EXTERN	VOID	cut_obj();
EXTERN	VOID	paste_obj();
EXTERN	WORD	snap_trs();
EXTERN	WORD	clamp_trs();
EXTERN	WORD	snap_xs();
EXTERN	WORD	clamp_xs();
EXTERN	WORD	snap_ys();
EXTERN	WORD	clamp_ys();
EXTERN	VOID	load_part();		/* in RCSEDIT.C */
EXTERN	WORD	file_view();
EXTERN	VOID	type_tree();
EXTERN	VOID	hot_off();
EXTERN	WORD	objc_find();
EXTERN	VOID	wait_obj();
EXTERN	VOID	invert_obj();
EXTERN	WORD	pane_find();
EXTERN	WORD	graf_mouse();
EXTERN	VOID	objc_xywh();
EXTERN	VOID	hot_dragbox();
EXTERN	WORD	tree_view();
EXTERN	VOID	obj_handle();
EXTERN	UWORD	inside();
EXTERN	VOID	do_bgcol();
EXTERN	VOID	do_patrn();
EXTERN	VOID	do_bdcol();
EXTERN	VOID	do_thick();
EXTERN	VOID	do_fgcol();
EXTERN	VOID	do_rule();
EXTERN	VOID	do_posn();
EXTERN	VOID	do_swtch();
EXTERN	VOID	set_switch();
EXTERN	WORD	hndl_pop();
EXTERN	WORD	clos_rsc();
EXTERN	VOID	merge_rsc();
EXTERN	VOID	info_dial();
EXTERN	WORD	menu_text();
EXTERN	VOID	ini_panes();
EXTERN	VOID	redo_trees();
EXTERN	VOID	view_objs();
EXTERN	VOID	outp_dial();
EXTERN	VOID	safe_dial();
EXTERN	VOID	about_dial();
EXTERN	WORD	menu_tnormal();
EXTERN	WORD	appl_write();
EXTERN	VOID	gsx_moff();
EXTERN	VOID	gsx_sclip();
EXTERN	WORD	rc_equal();
EXTERN	VOID	gr_rect();
EXTERN	WORD	objc_draw();
EXTERN	VOID	gsx_xbox();
EXTERN	VOID	gsx_mon();
EXTERN	VOID	do_hsinc();
EXTERN	VOID	do_vsinc();
EXTERN	WORD	max();
EXTERN	WORD	wind_set();
EXTERN	WORD	evnt_multi();
EXTERN  WORD    evnt_button();
EXTERN  WORD    graf_rubbox();
EXTERN	WORD	evnt_mouse();
EXTERN	WORD	rcs_init();
EXTERN	VOID	rcs_exit();
EXTERN	VOID	hndl_dsel();
EXTERN	VOID	obj_redraw();
EXTERN	VOID	obj_nowdraw();
EXTERN	VOID	do_redraw();
EXTERN	VOID	hndl_redraw();

/*	RCSINTF.C	06/25/85  	Tim Oren		*/

EXTERN	WORD	gl_nplanes;


EXTERN	VOID	enab_obj();
EXTERN	VOID	disab_obj();
EXTERN	VOID	sble_obj();
EXTERN	VOID	unsble_obj();
EXTERN	VOID	chek_obj();
EXTERN	VOID	unchek_obj();
EXTERN	LONG	avail_mem();
EXTERN	VOID 	menu_opts();
EXTERN	WORD	form_center();
EXTERN	WORD	form_dial();

EXTERN	WORD	form_do();
EXTERN	WORD	form_alert();
EXTERN	VOID	ob_setxywh();
EXTERN	VOID	arrange_tree();
EXTERN	WORD	lock_menus();
EXTERN	WORD	objc_offset();
EXTERN	VOID	bb_save();
EXTERN	VOID	outl_obj();
EXTERN	VOID	bb_restore();
EXTERN	VOID	gsx_attr();
EXTERN	WORD	min();
EXTERN	VOID	rc_constrain();
EXTERN	VOID	r_set();
EXTERN	VOID	desel_obj();
EXTERN	VOID	hide_obj();
EXTERN	VOID	SET_X();
EXTERN	VOID	unhide_obj();
EXTERN	VOID	sel_obj();
EXTERN  VOID	SET_WIDTH();
/*	RCSFILES.C	10/08/84 - 1/25/85	Tim Oren		*/


		/* in RCSLIB.C	*/
EXTERN	WORD	read_files();		/* in RCSREAD.C */
EXTERN	WORD	write_files();		/* in RCSWRITE.C */
EXTERN	WORD	dos_gdrv();
EXTERN	WORD	dos_gdir();
EXTERN	WORD	fsel_input();
EXTERN	WORD	dos_chdir();
EXTERN	WORD	open_files();
EXTERN	VOID	set_title();
EXTERN	WORD	merge_files();

 /*	RCSREAD.C	1/28/85 - 1/28/85	Tim Oren		*/

EXTERN  UWORD   swap_bytes();
EXTERN  WORD	dmcopy();		/* in RCSLIB.C	*/
EXTERN	VOID	clr_clip();
EXTERN  WORD	new_index();
EXTERN	LONG	get_mem();
EXTERN	LONG	copy_tree();
EXTERN	WORD	copy_objs();
EXTERN	VOID	add_trindex();
EXTERN	VOID	comp_alerts();		/* in RCSALERT.C */
EXTERN	LONG	dos_lseek();
EXTERN	WORD	read_piece();
EXTERN	WORD	get_file();
EXTERN	WORD	dos_open();
EXTERN	WORD	form_error();
EXTERN	WORD	dos_close();
EXTERN	WORD	objc_add();
EXTERN  VOID	set_kind();
EXTERN	VOID	SET_HEIGHT();
EXTERN	WORD	find_name();
EXTERN	WORD	tree_kind();
EXTERN	VOID	rs_obfix();
EXTERN	VOID	unique_name();

 /*	RCSWRITE.C	1/28/85 - 1/28/85	Tim Oren		*/

EXTERN	WORD	dos_space();
EXTERN  WORD	write_piece();
EXTERN	VOID	LWINC();
EXTERN	VOID	SET_FLAGS();
EXTERN	WORD	dos_create();

/*	RCSTREES.C	11/6/84 -  1/25/85 	Tim Oren		*/


EXTERN  WORD	hndl_dial();
		/* in RCSDATA.C */
EXTERN	VOID	zap_objindex();
EXTERN	WORD	name_ok();
		/* in RCSLIB.C */
EXTERN	VOID	snap_wh();
EXTERN	WORD	encode();
EXTERN	VOID	set_slpos();
EXTERN	VOID	hndl_locked();
EXTERN	VOID	fix_alert();
EXTERN	WORD	graf_growbox();
EXTERN	WORD	graf_shrinkbox();
EXTERN	WORD	set_obname();
EXTERN	VOID	get_obname();
EXTERN	VOID	set_menus();
EXTERN	VOID	clr_hot();
/*	RCSOBJS.C	11/20/84 - 1/25/85 	Tim Oren		*/


EXTERN	WORD	nth_child();
EXTERN	VOID	snap_xy();
EXTERN	WORD	in_bar();		/* in RCSMENU.C */
EXTERN	WORD	in_menu();
EXTERN	WORD	is_menu();
EXTERN	WORD	menu_n();
EXTERN	WORD	bar_n();
EXTERN	WORD	get_active();
EXTERN	WORD	blank_menu();
EXTERN	VOID	fix_menu_bar();
EXTERN	VOID	dslct_1obj();
EXTERN	VOID	mslct_obj();
EXTERN	VOID	map_dslct();
EXTERN	VOID	table_code();
EXTERN	WORD	make_num();
EXTERN	VOID	set_hot();
EXTERN	WORD	objc_delete();
EXTERN	WORD	menu_ok();
EXTERN	VOID	SET_STATE();
EXTERN	WORD	objc_order();
EXTERN	WORD	pt_roomp();
EXTERN	VOID	clamp_rubbox();
/*	RCSALERT.C	 1/17/85 - 1/17/85	Tim Oren		*/


EXTERN	LONG	mak_trindex();
EXTERN	LONG	mak_frstr();
		
/*	RCSMENU.C	 1/27/85 - 1/25/85 	Tim Oren		*/


EXTERN	LONG	get_obmem();		/* in RCSDATA.C */
EXTERN	WORD	child_of();
/*	RCSEDIT.C	12/26/84 - 1/25/85  	Tim Oren		*/


EXTERN  WORD	newsize_obj();
EXTERN  VOID	update_if();
EXTERN  VOID	text_wh();
EXTERN  VOID	icon_wh();
EXTERN  WORD	in_which_menu();
EXTERN  WORD	posn_obj();
/*	RCSLOAD.C	7/3/85		Tim Oren		*/

EXTERN	VOID	icon_tfix();
/*	RCSLIB.C	10/11/84 - 1/25/85 	Tim Oren		*/


EXTERN  WORD	DOS_ERR;
EXTERN	WORD	gl_wchar;
EXTERN	WORD	gl_hchar;
EXTERN	VOID	rc_union();
EXTERN	VOID	gsx_outline();
EXTERN	VOID	gsx_invert ();

EXTERN	WORD	rsrc_gaddr();
EXTERN	WORD	ch_height();
EXTERN	WORD	ch_width();
/*	RCSVDI.C	04/09/85	Tim Oren converted from:	*/
/*	APGSXIF.C	05/06/84 - 12/21/84	Lee Lorenzen		*/


#if	I8086
EXTERN VOID	i_ptsin();
EXTERN VOID	i_intin();
EXTERN VOID	i_ptsout();
EXTERN VOID	i_intout();
#endif

EXTERN VOID	i_ptr();
EXTERN VOID	i_ptr2();


EXTERN	VOID	r_get();
EXTERN	VOID	bb_fill();
EXTERN	VOID	gsx_xline();
EXTERN	VOID	gsx_ncode();
EXTERN	VOID	gsx_1code();
EXTERN	VOID	vst_clip();
EXTERN	VOID	vrn_trnfm();
EXTERN  WORD    vro_cpyfm();
EXTERN	WORD	vr_recfl();
