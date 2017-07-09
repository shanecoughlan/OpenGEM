
/* old-style prototypes, brute force */

EXTERN  VOID     fb_redraw();
EXTERN  VOID	 icon_size();
EXTERN  VOID     save_fat(BOOLEAN undo);
EXTERN  WORD     new_img();
EXTERN  WORD     icnopen();
EXTERN  WORD     save_icn();
EXTERN  WORD     svas_icn();
EXTERN  WORD     rvrt_icn();
EXTERN	VOID	   rubrec_off();
EXTERN  VOID	   draw_clipb();
EXTERN	VOID	   del_img();
EXTERN	VOID	   copy_img();
EXTERN	VOID	   cut_img();
EXTERN  VOID       solid_img();
EXTERN	VOID	   invrt_img();
EXTERN  VOID	   clear_img();
EXTERN   WORD      graf_mouse();
EXTERN   VOID       open_obj(WORD obj);
EXTERN	WORD	clos_rsc();
EXTERN	VOID	merge_rsc();
EXTERN	WORD	menu_text();
EXTERN	WORD	menu_tnormal();
EXTERN	WORD	appl_write();
EXTERN	VOID	gsx_moff();
EXTERN	VOID	gr_rect();
EXTERN	WORD	objc_draw();
EXTERN	VOID	gsx_xbox();
EXTERN	VOID	gsx_mon();
EXTERN	WORD	wind_set();
EXTERN	WORD	evnt_multi();
EXTERN	WORD	rcs_init();
EXTERN	VOID	rcs_exit();
EXTERN	WORD	open_rsc();		/* in RCSFILES.C */
EXTERN	VOID	save_rsc();
EXTERN	WORD	svas_rsc();
EXTERN	WORD	rvrt_rsc();
EXTERN	WORD	new_rsc();
EXTERN  WORD    quit_rsc();
EXTERN WORD      wind_get();
EXTERN WORD      wind_update();
EXTERN  BOOLEAN	 paste_img;
EXTERN 	UWORD	mflags;
EXTERN	WORD	scale;
EXTERN  WORD	gl_nplanes;


typedef WORD (*MAPROUTINE)(LPTREE tr, WORD ob);

VOID map_all(MAPROUTINE routine);

/* rcslib.c */
VOID set_slsize(WORD w_handle, WORD h_size, WORD v_size);
VOID set_slpos(WORD w_handle, WORD h_pos, WORD v_pos);
VOID merge_str(BYTE *pdst, BYTE *ptmp, ...);
VOID map_tree(LPTREE tree, WORD this, WORD last, MAPROUTINE routine);
WORD encode(LPTREE tree, WORD ob1st, WORD num);

VOID unfix_chpos(LPUWORD where, WORD x_or_y);
VOID fix_chpos(LPUWORD pfix, WORD ifx);
VOID strup(BYTE *p1);
LPBYTE string_addr(WORD str);
VOID r_get(GRECT *pxywh, WORD *px, WORD *py, WORD *pw, WORD *ph);
VOID r_set(GRECT *pxywh, WORD   x, WORD   y, WORD   w, WORD   h);
VOID gsx_sclip(GRECT *pt);
WORD min(WORD a, WORD b);
WORD ini_tree(LPTREE *tree, WORD which);		/* find tree address */
VOID snap_wh(WORD *w, WORD *h);
LPVOID image_addr(WORD which);
VOID text_wh(LPTEDI taddr, WORD type, WORD *w, WORD *h);


WORD rs_obfix(LPTREE tree, WORD curob);
VOID undo_obj(LPTREE tree, WORD which, WORD bit);
VOID do_obj  (LPTREE tree, WORD which, WORD bit);
WORD enab_obj(LPTREE tree, WORD which);
WORD disab_obj(LPTREE tree, WORD which);
VOID sel_obj(LPTREE tree, WORD which);
WORD desel_obj(LPTREE tree, WORD which);
VOID chek_obj(LPTREE tree, WORD which);
WORD unchek_obj(LPTREE tree, WORD which);
VOID unflag_obj(LPTREE tree, WORD which, WORD bit);
VOID flag_obj(LPTREE tree, WORD which, WORD bit);
VOID hide_obj(LPTREE tree, WORD which);
VOID unhide_obj(LPTREE tree, WORD which);
VOID indir_obj(LPTREE tree, WORD which);
VOID dir_obj(LPTREE tree, WORD which);
VOID sble_obj(LPTREE tree, WORD which);
WORD unsble_obj(LPTREE tree, WORD which);
VOID objc_xywh(LPTREE tree, WORD obj, GRECT *p);
VOID ob_setxywh(LPTREE tree, WORD obj, GRECT *pt);

VOID ob_relxywh(LPTREE tree, WORD obj, GRECT *pt);
VOID r_get(GRECT *pxywh, WORD *px, WORD *py, WORD *pw, WORD *ph);
VOID r_set(GRECT *pxywh, WORD x, WORD y, WORD w, WORD h);
UWORD inside(WORD x, WORD y, GRECT *pt);
VOID rc_constrain(GRECT *pc, GRECT *pt);
VOID rc_union(GRECT *p1, GRECT *p2);
WORD min(WORD a, WORD b);
WORD max(WORD a, WORD b);
VOID movs(WORD num, BYTE *ps, BYTE *pd);
VOID movup( WORD num, BYTE *ps, BYTE *pd);
BYTE toupper(BYTE c);
VOID strup(BYTE *p1);
VOID LLSTRCPY(LPBYTE src, LPBYTE dest);
WORD LLSTRCMP(LPBYTE l1, LPBYTE l2);
// to be inlined VOID LWINC(LPWORD x);				/* ? */
BYTE uhex_dig(WORD wd);
WORD make_num(BYTE *pstr);
VOID merge_str(BYTE *pdst, BYTE *ptmp, ...);
WORD dmcopy(FILE *fp, LONG msrc, LPVOID mdest, WORD mln);
LPBYTE obj_addr(LPTREE tree, WORD obj, WORD offset);
VOID arrange_tree(LPTREE tree, WORD wint, WORD hint, WORD *maxw, WORD *maxh);
WORD trans_obj(LPTREE tree, WORD obj);
WORD get_parent(LPTREE tree, WORD obj);
VOID table_code(LPTREE tree, WORD obj0, WORD *types, WORD ntypes, WORD type);
VOID snap_xy(WORD *x, WORD *y);
WORD nth_child(LPTREE tree, WORD nobj, WORD n);
WORD child_of(LPTREE tree, WORD nobj, WORD obj);
VOID invert_obj(LPTREE tree, WORD obj, GRECT *clprect);
VOID outl_obj(LPTREE tree, WORD obj, GRECT *clprect);

VOID icon_tfix(LPICON iaddr);
VOID icon_wh(LPICON taddr, WORD *w, WORD *h);
VOID trans_bitblk(LPBIT obspec);


/* rcsdata.c */

VOID ini_buff(VOID);
LPLTREE tree_ptr(WORD n);
LPLSTR str_ptr(WORD n);
LPLBIT img_ptr(WORD n);
LPTREE tree_addr(WORD n);
LPBYTE str_addr(WORD n);
LPBIT img_addr(WORD n);
VOID set_value(WORD key, LPBYTE val);
VOID set_kind(WORD key, WORD kind);
VOID set_name(WORD key, BYTE *name);
WORD new_index(LPBYTE val, WORD kind, BYTE *name);
VOID del_index(WORD key);
WORD get_kind(WORD key);
LPBYTE get_value(WORD key);
BYTE *get_name(WORD key);
WORD find_value(LPBYTE val);
WORD find_tree(WORD n);
WORD find_obj(LPTREE tree, WORD obj);
WORD find_name(BYTE *name);
WORD tree_kind(WORD kind);
VOID unique_name(BYTE *name, BYTE *ptn, WORD n);
WORD blank_name( BYTE *name );
WORD name_ok(BYTE *name, WORD ok, WORD nilok);
WORD set_obname(LPTREE tree, WORD obj, BYTE *name, LPTREE ntree, WORD nobj);
VOID get_obname(BYTE *name, LPTREE ntree, WORD nobj);
WORD del_objindex(LPTREE tree, WORD obj);
VOID zap_objindex(LPTREE tree, WORD obj);
LONG avail_mem(VOID);
LPVOID get_mem(UWORD bytes);
LPOBJ  get_obmem(VOID);
VOID update_if(LPLSTR taddr, BYTE *naddr);
LPTEDI copy_ti(LPTEDI source);
LPICON copy_ib(LPICON source);
LPBIT  copy_bb(LPBIT source);
WORD copy_spec(LPTREE tree, WORD obj);
WORD indx_obs(LPTREE tree, WORD which);
WORD fnd_ob(WORD old);
WORD copy_objs(LPTREE stree, WORD obj, LPTREE dtree, WORD specopy);
LPTREE copy_tree(LPTREE stree, WORD obj, WORD specopy);
LPLTREE mak_trindex(WORD nentry);
VOID add_trindex(LPTREE tree);
LPLSTR mak_frstr(WORD nentry);
LPLBIT mak_frimg(WORD nentry);
VOID dcomp_free(VOID);
BYTE *c_lookup(UWORD what, LOOKUP *where, UWORD n, BYTE *punt);
VOID clr_tally(VOID);
VOID tally_str(LPBYTE addr);
VOID tally_bb(LPBIT addr);
VOID tally_free(VOID);
WORD tally_obj(LPTREE tree, WORD obj);
WORD count_free(LPTREE tree, WORD obj);
WORD dcomp_tree(LPTREE tree, WORD obj);



/* rcswrite.c */
VOID   wr_str(LPLSTR where);
WORD   wr_string(LPTREE tree, WORD which);
VOID   map_frstr(VOID);
VOID   wr_imdata(LPLPTR where, WORD w, WORD h);
WORD   wr_image(LPTREE tree, WORD which);
VOID   map_frimg(VOID);
UWORD  swap_bytes(UWORD);
WORD   write_files(VOID);

VOID   c_iconhead(VOID);
VOID   c_iconblk(LPICON addr);

/* rcsvdi.c */
VOID   gsx_untrans(LPVOID saddr, WORD swb, LPVOID daddr, WORD dwb, WORD h);
VOID   gsx_trans  (LPVOID saddr, WORD swb, LPVOID daddr, WORD dwb, WORD h);
VOID   gsx_attr(WORD text, WORD mode, WORD color);
VOID   bb_fill(WORD mode, WORD fis, WORD patt, WORD hx, WORD hy, WORD hw, WORD hh);
VOID   gsx_xline( WORD ptscount, WORD *ppoints );
VOID   gsx_fix(MFDB *pfd, LPVOID theaddr, WORD wb, WORD h);
VOID   vr_xrecfl(WORD handle, WORD *pts, MFDB *mfdb);
WORD   ch_width(WORD fn);
WORD   ch_height(WORD fn);
VOID   gsx_outline(GRECT *p);
VOID   gsx_invert(GRECT *p);
VOID   bb_save(GRECT *ps);
VOID   bb_restore(GRECT *pr);
VOID   gsx_vopen(VOID);
VOID   gsx_vclose(VOID);
WORD   gsx_start(VOID);
VOID   gsx_bxpts(GRECT *g);

/* rcsfiles.c */
VOID r_to_xfile(BYTE *where, BYTE *ext);
VOID rsc_title();
WORD get_file();
VOID get_path(BYTE *tmp_path, BYTE *spec);
WORD ok_rvrt_files(VOID);

/* rcsintf.c */
WORD hndl_dial(LPTREE tree, WORD def, GRECT *p);
WORD hndl_alert(WORD b_num, LPBYTE addr);
VOID hndl_locked(VOID);
VOID mouse_form(WORD f_num);
VOID ini_panes(VOID);
VOID arrge_icntree(LPTREE tree, WORD wint, WORD hint);
VOID view_parts(VOID);
VOID lock_menu(VOID);
VOID set_menus(VOID);
VOID menu_opts(LPTREE tree, WORD obj);
VOID clr_hot(VOID);
VOID set_hot(VOID);
VOID set_switch(VOID);
VOID new_state(WORD new);
WORD file_view(VOID);
WORD tree_view(VOID);
VOID clr_clip(VOID);
VOID set_title(BYTE *new);
VOID get_defpath(BYTE *path, BYTE *spec);
VOID clr_title(VOID);
VOID hot_off(VOID);
VOID wait_tools(VOID);
VOID wait_obj(LPTREE tree, WORD obj);
WORD pane_find(LPTREE *tree, WORD *obj, WORD mx, WORD my);
WORD hndl_pop(WORD obj, BOOLEAN itool);
WORD rub_wait(GRECT *po, WORD *mx, WORD *my);
VOID hot_dragbox(GRECT *p, GRECT *pc, WORD *mx, WORD *my, WORD hot);
VOID clamp_rubbox(WORD x, WORD y, WORD wlow, WORD hlow, WORD whigh, WORD hhigh,
                  LONG ahigh, WORD *w, WORD *h, GRECT *bound);
VOID obj_handle(LPTREE tree, WORD obj, GRECT *p);
VOID about_dial(VOID);
VOID info_dial(WORD obj);
WORD find_opt( WORD opt );
BOOLEAN get_opstate( WORD index );
VOID set_opstate( WORD index );
VOID read_inf(WORD flag);
VOID init_deflts(VOID);
VOID wrte_inf(VOID);
VOID outp_dial(VOID);
VOID safe_dial(VOID);
VOID ini_prog(VOID);
VOID show_prog(WORD strno);
VOID lock_menus(VOID);
VOID wind_setl(WORD h, WORD f, LPBYTE v);


/* rcsalert.c */
VOID dcomp_alerts(VOID);
VOID fix_alert(LPTREE tree);
VOID comp_alerts(LPRSHDR hdr);
WORD pt_roomp(LPTREE tree, WORD obj0, WORD nobj);


/* rcsmain.c */
VOID obj_redraw(LPTREE tr, WORD ob);
VOID send_redraw(WORD w_handle, GRECT *p);
VOID obj_nowdraw(LPTREE tree, WORD obj);
VOID do_redraw(WORD w_handle, GRECT *dirty, LPTREE tree, GRECT *area);
WORD hndl_menu(WORD title, WORD item);
VOID hndl_arrowed(WORD w_handle, WORD kind);
VOID hndl_hslid(WORD w_handle, WORD h_set);
VOID hndl_vslid(WORD w_handle, WORD v_set);
BOOLEAN hndl_keybd( WORD key );



/* rcsmenu.c */

WORD in_bar (LPTREE tree, WORD obj);
WORD in_menu(LPTREE tree, WORD obj);
WORD is_menu(LPTREE tree, WORD obj);
WORD in_which_menu(LPTREE tree, WORD obj);
WORD menu_n(LPTREE tree, WORD n);
WORD bar_n(LPTREE tree, WORD n);
WORD get_active(LPTREE tree);
WORD blank_menu(LPTREE tree, WORD sobj);
VOID fix_menu_bar(LPTREE tree);
WORD menu_ok(LPTREE tree, WORD obj);
VOID desk_menufix(LPTREE tree);
VOID undo_menufix(LPTREE tree);

/* rcsobjs.c */
WORD snap_xs(WORD x);
WORD snap_ys(WORD y);
WORD clamp_xs(LPTREE tree, WORD x);
WORD clamp_ys(LPTREE tree, WORD y);
VOID do_hsinc(WORD inc);
VOID do_vsinc(WORD inc);
VOID set_rootxy(LPTREE tree);
WORD newsize_obj(LPTREE tree, WORD obj, WORD neww, WORD newh, WORD ok);
VOID slid_objs(VOID);
VOID view_objs(VOID);
VOID type_obj(WORD obj);
VOID del_ob(WORD sobj);
VOID constrain_obj(LPTREE tree, WORD obj, WORD *px, WORD *py);
WORD posn_obj(LPTREE tree, WORD obj, WORD *px, WORD *py, WORD w, WORD h, 
              WORD exact_ok);
VOID cut_obj(WORD sobj, WORD dup);
VOID paste_obj(WORD dobj, WORD dx, WORD dy, WORD dup);
WORD new_obj(LPTREE stree, WORD sobj, WORD dobj, WORD dx, WORD dy);
VOID mov_obj(WORD sobj, WORD dobj, WORD dx, WORD dy);
VOID size_obj(LPTREE tree, WORD obj);
VOID slct_obj(LPTREE tree, WORD obj);
VOID mslct_obj(LPTREE tree, WORD obj);
WORD dslct_1obj(LPTREE tree, WORD obj);
VOID map_dslct(LPTREE tree, WORD obj);
VOID dslct_obj(VOID); /* LPTREE tree, WORD obj); */
VOID unhid_part(WORD obj);
WORD less_ob(LPTREE tree, WORD obj1, WORD obj2, WORD mode); 
VOID sort_tree(LPTREE tree, WORD pobj, WORD mode);
VOID sort_part(WORD obj);
VOID flatten_part(WORD obj);


/* rcsload.c */
VOID    load_part(WORD obj);
WORD writ_icon(LPVOID praster, WORD icon_w, WORD icon_h, 
               WORD data_size, BOOLEAN data_field);
VOID get_icnfile(BYTE *full_path);


/* rcsinit.c */
VOID set_icnmenus(VOID);
VOID icnhot(VOID);
VOID set_gridbx(VOID);
VOID set_fatbox(VOID);
VOID init_img(VOID);
VOID iconterm(WORD termcond);
VOID wind_size(VOID);
VOID outl_img(VOID);
VOID icn_init(BOOLEAN size_changed);
WORD iconinit(VOID);
VOID icon_edit(VOID);
WORD cont_rcsinit(WORD flag);

/* rcstrees.c */

WORD trpan_f(WORD obj);
WORD trunpan_f(WORD trnum);
WORD fit_vtrees(VOID);
WORD fit_htrees(VOID);
WORD need_vtrees(VOID);
WORD snap_trs(WORD tr);
WORD clamp_trs(WORD tr);
VOID do_trsinc(WORD inc);
VOID view_trees(VOID);
VOID slid_trees(VOID);
VOID redo_trees(VOID);
VOID del_tree(WORD sobj);
VOID cut_tree(WORD sobj, WORD dup);
LPTREE ins_tree(LPTREE stree, WORD sobj, WORD kind, WORD dobj, WORD dx, WORD dy);
VOID type_tree(WORD obj);
WORD posn_tree(WORD dobj, WORD dx, WORD dy);
VOID paste_tree(WORD dobj, WORD dx, WORD dy, WORD dup);
VOID new_tree(WORD sobj, WORD dobj, WORD dx, WORD dy);
VOID dup_tree(WORD sobj, WORD dobj, WORD dx, WORD dy);
WORD mov_tree(WORD sobj, WORD dobj, WORD dx, WORD dy);
VOID open_tree(WORD obj);
VOID clos_tree(VOID);
VOID name_tree(WORD obj);
VOID select_tree(LPTREE tree, WORD obj);
VOID dselct_tree(LPTREE tree, WORD obj);

/* rcsread.c */
WORD read_68K( BOOLEAN merge );
WORD open_files(WORD def);
LPBYTE get_sub(UWORD rsindex, WORD rtype, WORD rsize);
LPBYTE get_addr(WORD rstype, WORD rsindex);
WORD fix_long(LPLPTR plong);
VOID fix_trindex(VOID);
WORD fix_ptr(WORD type, WORD index);
VOID fix_objects(VOID);
VOID fix_tedinfo(VOID);
VOID fix_iconblk(VOID);
VOID fix_bitblk(VOID);
VOID fix_frstr(VOID /* WORD type, WORD index */);
VOID fix_frimg(VOID);
VOID fix_all(VOID);
VOID comp_str(LPRSHDR hdr);
VOID comp_img(LPRSHDR hdr);
WORD read_files(VOID);
WORD merge_files(VOID);

/* rcsedit.c */
VOID do_bdcol(LPTREE tree, WORD obj, WORD item);
VOID do_bgcol(LPTREE tree, WORD obj, WORD item);
VOID do_fgcol(LPTREE tree, WORD obj, WORD item);
VOID do_patrn(LPTREE tree, WORD obj, WORD item);
VOID do_thick(LPTREE tree, WORD obj, WORD item);
VOID do_rule(LPTREE tree, WORD obj, WORD item);
VOID do_posn(LPTREE tree, WORD obj, WORD item);
VOID do_swtch(LPTREE tree, WORD obj, WORD item);

/* rcsimain.c */
VOID copy_colplanes( LPBIT longaddr, WORD color, WORD isdata);
VOID fattify(GRECT *area);

/* rcsiedit.c */
VOID resize(LPVOID src_pt, WORD src_w, WORD src_h, LPVOID des_pt);

/* rcsasm.as */
VOID MAGNIFY(LPVOID src_mp, LPVOID dst_mp, WORD npix);
void B_MOVE(LPVOID from, WORD count, LPVOID to);

/* tcrtlasm.as */
WORD mul_div (WORD m1, UWORD m2, WORD d1);
UWORD umul_div(UWORD m1, UWORD m2, WORD d1);

/* ppderror.c */
WORD ppd_form_error(WORD errno);
WORD lfwrite(LPVOID data, WORD size, WORD count, FILE *fp);
WORD lfread (LPVOID data, WORD size, WORD count, FILE *fp);

