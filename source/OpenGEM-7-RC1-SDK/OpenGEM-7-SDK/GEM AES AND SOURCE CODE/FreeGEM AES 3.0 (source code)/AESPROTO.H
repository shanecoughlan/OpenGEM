/*
*       AESPROTO.H: Prototypes for AES functions, so Pacific C can 
*                  link them correctly
*
*       Copyright 1999, John Elliott
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*/

#ifndef __AES_H_INCLUDED__
#error Must include aes.h before aesproto.h
#endif

#ifndef __AESPROTO_H_INCLUDED__

#define __AESPROTO_H_INCLUDED__ 27-04-1999



/* GEMASM.AS */
EXTERN VOID psetup(PD *p, BYTE *codevalue);
EXTERN VOID usergo(LONG addr);
EXTERN VOID gotopgm(VOID);
EXTERN VOID savestat(UDA *uda); 
EXTERN VOID switchto(UDA *uda);
EXTERN VOID dsptch(VOID);

/* GEMASYNC.C */

EXTERN VOID signal(EVB *e);
EXTERN VOID azombie(EVB *e, UWORD ret);
EXTERN VOID evinsert(EVB *e, EVB **root);
EXTERN VOID takeoff(EVB *p);
EXTERN EVSPEC mwait(EVSPEC mask);
EXTERN EVSPEC iasync(WORD afunc, LONG aparm);
EXTERN UWORD apret(EVSPEC mask);
EXTERN EVSPEC acancel(EVSPEC m);

/* GEMGRAF.C */

EXTERN VOID gsx_setclr( WORD clr, WORD fgclr, WORD bkclr, 
                        WORD style, WORD pattern );

EXTERN VOID gsx_sclip(GRECT *pt);
EXTERN VOID gsx_gclip(GRECT *pt);
EXTERN WORD gsx_chkclip(GRECT *pt);
EXTERN VOID gsx_pline(WORD offx, WORD offy, WORD cnt, WORD *pts);
EXTERN VOID gsx_cline(UWORD x1, UWORD y1, UWORD x2, UWORD y2);
EXTERN VOID gsx_attr(UWORD text, UWORD mode, UWORD color);
EXTERN VOID gsx_bxpts(GRECT *pt);
EXTERN VOID gsx_box(GRECT *pt);
EXTERN VOID gsx_xbox(GRECT *pt);
EXTERN VOID gsx_xcbox(GRECT *pt);
EXTERN VOID gsx_fix(FDB *pfd, LPBYTE theaddr, WORD wb, WORD h);
EXTERN VOID gsx_blt(LPBYTE saddr, UWORD sx, UWORD sy, UWORD swb, 
                    LPBYTE daddr, UWORD dx, UWORD dy, UWORD dwb, 
                    UWORD w, UWORD h, UWORD rule, WORD fgcolor, WORD bgcolor);
EXTERN VOID bb_screen(WORD scrule, WORD scsx, WORD scsy, WORD scdx, 
                      WORD scdy, WORD scw, WORD sch);
EXTERN VOID gsx_trans(LPBYTE saddr, UWORD swb, LPBYTE daddr, UWORD dwb, 
                      UWORD h);
EXTERN VOID gsx_start(VOID);
EXTERN VOID bb_fill(WORD mode, WORD fis, WORD patt, 
                    WORD hx, WORD hy, WORD hw, WORD hh);
EXTERN UWORD ch_width(WORD fn);
EXTERN UWORD ch_height(WORD fn);
EXTERN VOID gsx_tcalc(WORD font, LPBYTE ptext, WORD *ptextw, 
                      WORD *ptexth, WORD *pnumchs);
EXTERN VOID gsx_tblt(WORD tb_f, WORD x, WORD y, WORD tb_nc);
EXTERN VOID gsx_xline( WORD ptscount, WORD *ppoints );
EXTERN VOID gr_inside(GRECT *pt, WORD th);
EXTERN VOID gr_rect(UWORD icolor, UWORD ipattern, GRECT *pt);
EXTERN WORD gr_just(WORD just, WORD font, LPBYTE ptext, WORD w, WORD h,
                    GRECT *pt);
EXTERN VOID gr_gtext(WORD just, WORD font, LPBYTE ptext, GRECT *pt);
EXTERN VOID gr_crack(UWORD color, WORD *pbc, WORD *ptc, WORD *pip, 
                     WORD *pic, WORD *pmd);
EXTERN VOID gr_gblt(LPBYTE pimage, GRECT *pi, WORD col1, WORD col2);
EXTERN VOID gr_gicon(WORD state, LPBYTE pmask, LPBYTE pdata, LPBYTE ptext, 
              WORD ch, WORD chx, WORD chy, GRECT *pi, GRECT *pt);
EXTERN VOID gr_box(WORD x, WORD y, WORD w, WORD h, WORD th);
EXTERN VOID gr_3dbox( GRECT *tt, WORD th, WORD doborder, WORD reverse);
EXTERN VOID gr_3dpress( GRECT *tt, WORD th, WORD doborder );
EXTERN VOID inner_dots( GRECT * pt, WORD clrlite, WORD clrdark );
EXTERN VOID grcc_rect( WORD clr, GRECT *pt );
EXTERN VOID gr_clricon(WORD state, LPBYTE pmask, LPBYTE pdata, LPBYTE ptext, 
                       WORD ch, WORD chx, WORD chy, GRECT *pi, GRECT *pt);
EXTERN VOID gr_dtmfdb(FDB FAR *spec, GRECT *t);

/* GEMFLAG.C */
EXTERN WORD	tchange(WORD l, WORD h);
EXTERN VOID unsync(REG SPB *sy);

/* GEMINPUT.C */
EXTERN VOID set_ctrl(GRECT *pt);
EXTERN WORD kchange(WORD ch, WORD kstat);
EXTERN WORD mchange(WORD rx, WORD ry);
EXTERN WORD bchange(WORD rx, WORD ry);
EXTERN VOID b_click(REG WORD state);
EXTERN VOID m_forkq(FPD_CODE fcode, WORD ratx, WORD raty);
EXTERN VOID b_delay(WORD amount);
EXTERN VOID post_keybd(REG CDA *c, UWORD ch);

/* GEMINIT.C */

EXTERN VOID ini_dlongs(VOID);
#if I8086
	EXTERN WORD sizeglo(VOID);
#endif
#if MC68K
	EXTERN LONG size_theglo(VOID);
#endif
EXTERN WORD sizeuda(VOID);
EXTERN VOID ev_init(EVB *evblist, WORD cnt);
EXTERN PD *iprocess(BYTE *pname, VOID (*routine)(VOID));
EXTERN VOID fs_start(VOID);
#if SINGLACC
	EXTERN VOID sndcli(BYTE *pfilespec, WORD *paccroom);
	EXTERN VOID ldaccs(VOID);
#endif

EXTERN VOID sh_addpath(VOID);
EXTERN VOID sh_init(VOID);
EXTERN VOID gem_main(VOID);
EXTERN VOID all_run(VOID);
EXTERN VOID sh_desk(WORD obj, LPLONG plong);
EXTERN WORD hex_dig(BYTE achar);
EXTERN VOID scan_2(LPBYTE pcurr, UWORD *pwd);
EXTERN VOID sh_rdinf(VOID);

/* GEMFMALT.C */
EXTERN VOID fm_strbrk(LPTREE tree, LPBYTE palstr, WORD stroff, 
                      WORD *pcurr_id, WORD *pnitem, WORD *pmaxlen);
EXTERN VOID fm_parse(LPTREE tree, LPBYTE palstr, WORD *picnum, WORD *pnummsg, 
                     WORD *plenmsg, WORD *pnumbut, WORD *plenbut);
EXTERN VOID fm_build(LPTREE tree, WORD haveicon, WORD nummsg, WORD mlenmsg, 
                     WORD numbut, WORD mlenbut);
WORD fm_alert(WORD defbut, LPBYTE palstr);


/* GEMDOS.C */
EXTERN VOID dos_func(UWORD ax, ULONG dsdx);
EXTERN VOID dos_chdir(LPBYTE pdrvpath);
EXTERN WORD dos_gdir(WORD drive, LPBYTE pdrvpath);
EXTERN WORD dos_gdrv(VOID);
EXTERN WORD dos_sdrv(WORD newdrv);
EXTERN VOID dos_term(VOID);
EXTERN VOID dos_sdta(LPVOID ldta);
EXTERN LPVOID dos_gdta(VOID);
EXTERN WORD dos_gpsp(VOID);
EXTERN WORD dos_spsp(WORD newpsp);
EXTERN WORD dos_sfirst(LPBYTE pspec, WORD attr);
EXTERN WORD dos_snext(VOID);
EXTERN WORD dos_create(LPBYTE pname, WORD attr);
EXTERN WORD dos_open(LPBYTE pname, WORD access);
EXTERN WORD dos_close(WORD handle);
EXTERN WORD dos_delete(LPBYTE ppath);
EXTERN WORD dos_read(WORD handle, WORD cnt, LPVOID pbuffer);
EXTERN WORD dos_write(WORD handle, WORD cnt, LPVOID pbuffer);
EXTERN LONG dos_lseek(WORD handle, WORD smode, LONG sofst);
EXTERN VOID dos_exec(LPBYTE pcspec, WORD segenv, LPBYTE pcmdln, LPVOID pfcb1, LPVOID pfcb2);
EXTERN WORD dos_wait(VOID);
EXTERN LPVOID dos_alloc(LONG nbytes);
EXTERN LONG dos_avail(VOID);
EXTERN WORD dos_free(LPVOID maddr);
EXTERN WORD dos_rename(LPBYTE pname, LPBYTE newname);
EXTERN WORD dos_getticks(VOID);	/* Get seconds & centiseconds from clock */

#if MULTIAPP
	EXTERN WORD dos_stblk(UWORD blockseg, UWORD newsize);
#endif


/* GEMAPLIB.C */
EXTERN WORD     ap_init(LPVOID x_info);
EXTERN VOID     ap_rdwr(WORD code, REG PD *p, WORD length, LPVOID pbuff);
EXTERN WORD     ap_find(LPBYTE pname);
EXTERN VOID     ap_tplay(REG LPFPD pbuff, WORD len, WORD scale);
EXTERN WORD     ap_trecd(REG LPFPD pbuff, WORD len);
EXTERN VOID     ap_exit(WORD isgem);
EXTERN WORD		xa_getinfo(WORD code, WORD *rets);

/* GEMEVLIB.C */
EXTERN WORD 	ev_block(WORD code, LPVOID lvalue);
EXTERN UWORD 	ev_button(WORD bflgclks, UWORD bmask, UWORD bstate, WORD *rets);
EXTERN UWORD 	ev_mouse(REG MOBLK *pmo, WORD *rets);
EXTERN VOID     ev_timer(LONG tcount);
EXTERN WORD     ev_multi(REG WORD flags, REG MOBLK *pmo1, MOBLK *pmo2, 
                         LONG tmcount, LONG buparm, LPVOID mebuff, WORD *prets);
EXTERN WORD     ev_dclick(WORD rate, WORD setit);

/* GEMMNLIB.C */
EXTERN UWORD 	do_chg(REG LPTREE tree, WORD iitem, REG UWORD chgvalue, 
                       WORD dochg, WORD dodraw, WORD chkdisabled);
EXTERN WORD find_shortcut( WORD key );
EXTERN VOID	menu_keys(LPTREE tree, WORD parent);

/* GEMOBJOP.C */

/* Type of routine to pass to everyobj() */
typedef VOID (*EVERYOP)(LPTREE, WORD, WORD, WORD);

EXTERN LPBYTE obaddr(LPTREE tree, WORD obj, WORD fld_off);
EXTERN BYTE ob_sst(LPTREE tree, WORD obj, LONG *pspec, WORD *pstate, 
            WORD *ptype, WORD *pflags, GRECT *pt, WORD *pth);
EXTERN VOID everyobj(LPTREE tree, WORD this, WORD last, 
              EVERYOP routine, WORD startx, WORD starty, WORD maxdep);
EXTERN WORD get_par(LPTREE tree, WORD obj, WORD *pnobj);


/* GEMOBLIB.C */
EXTERN VOID 	ob_draw(LPTREE tree, WORD obj, WORD depth);
EXTERN WORD		ob_find(LPTREE tree, WORD currobj, WORD depth, WORD mx, WORD my);
EXTERN VOID		ob_add(LPTREE tree, WORD parent, WORD child);
EXTERN VOID		ob_delete(LPTREE tree, WORD obj);
EXTERN VOID		ob_order(LPTREE tree, WORD mov_obj, WORD new_pos);
EXTERN VOID		ob_change(LPTREE tree, WORD obj, UWORD new_state, WORD redraw);
EXTERN UWORD	ob_fs(LPTREE tree, WORD ob, WORD *pflag);
EXTERN VOID		ob_offset(LPTREE tree, WORD obj, WORD *pxoff, WORD *pyoff);
EXTERN VOID		just_draw(LPTREE tree, WORD obj, WORD sx, WORD sy);
EXTERN VOID		ob_highlight( LPTREE tree, WORD obj );

/* XSTATE.C (overflow from GEMOBLIB) */
EXTERN VOID  xstate(WORD obtype, WORD state, WORD doit3d, WORD th, GRECT *t, WORD flags);
EXTERN VOID  draw_hilite(WORD obtype, GRECT *t);
EXTERN VOID ystate(WORD obtype, LONG spec, GRECT *t, LPTREE tree, WORD obj);

/* GEMOBED.C */
EXTERN VOID ob_getsp(LPTREE tree, WORD obj, TEDINFO *pted);
EXTERN VOID ob_center(LPTREE tree, GRECT *pt);
EXTERN WORD scan_to_end(BYTE *pstr, WORD idx, BYTE chr);
EXTERN WORD find_pos(BYTE *str, WORD pos);
EXTERN VOID pxl_rect(LPTREE tree, WORD obj, WORD ch_pos, GRECT *pt);
EXTERN VOID curfld(LPTREE tree, WORD obj, WORD new_pos, WORD dist);
EXTERN WORD instr(BYTE chr, BYTE *str);
EXTERN WORD check(BYTE *in_char, BYTE valchar);
EXTERN VOID ob_stfn(WORD idx, WORD *pstart, WORD *pfinish);
EXTERN WORD ob_delit(WORD idx);
EXTERN WORD ob_edit(LPTREE tree, WORD obj, WORD in_char, WORD *idx, WORD kind);
EXTERN VOID ob_actxywh(LPTREE tree, WORD obj, GRECT *pt);
EXTERN VOID ob_relxywh(LPTREE tree, WORD obj, GRECT *pt);

/* GEMFMLIB.C */
EXTERN WORD		fm_show(WORD string, UWORD *pwd, WORD level);
EXTERN VOID 	fm_own(WORD beg_ownit);
EXTERN WORD		fm_inifld(LPTREE tree, WORD start_fld);
EXTERN WORD		fm_keybd(LPTREE tree, WORD obj, WORD *pchar, WORD *pnew_obj);
EXTERN WORD 	fm_button(LPTREE tree, WORD new_obj, WORD clks, WORD *pnew_obj);
EXTERN WORD		fm_do(LPTREE tree, WORD start_fld);
EXTERN WORD 	fm_dial(WORD fmd_type, GRECT *pt, GRECT *ipt);
EXTERN WORD 	fm_show(WORD string, UWORD *pwd, WORD level);
EXTERN WORD 	eralert(WORD n, WORD d);	
EXTERN WORD		fm_error(WORD n);
EXTERN WORD 	fm_alert(WORD defbut, LPBYTE palstr);

/* GEMGRLIB.C */
EXTERN WORD gr_stilldn(WORD out, WORD x, WORD y, WORD w, WORD h);
EXTERN VOID gr_setup(WORD color);
EXTERN VOID gr_clamp(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin, 
                     WORD *pneww, WORD *pnewh);
EXTERN VOID gr_scale(WORD xdist, WORD ydist, WORD *pcnt, WORD *pxstep, 
                     WORD *pystep);
EXTERN VOID gr_stepcalc(WORD orgw, WORD orgh, GRECT *pt, 
				        WORD *pcx, WORD *pcy, WORD *pcnt, WORD *pxstep, 
                        WORD *pystep);
EXTERN VOID gr_xor(WORD clipped, WORD cnt, WORD cx, WORD cy, WORD cw, 
                   WORD ch, WORD xstep, WORD ystep, WORD dowdht);
EXTERN VOID gr_draw(WORD have2box, GRECT *po, GRECT *poff);
EXTERN WORD gr_wait(GRECT *po, GRECT *poff);
EXTERN VOID gr_rubwind(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin, 
                       GRECT *poff, WORD *pwend, WORD *phend);
EXTERN VOID gr_rubbox(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin, 
                      WORD *pwend, WORD *phend);
EXTERN VOID gr_dragbox(WORD w, WORD h, WORD sx, WORD sy, GRECT *pc, 
                       WORD *pdx, WORD *pdy);
EXTERN VOID gr_2box(WORD flag1, WORD cnt, GRECT *pt, WORD xstep, WORD ystep, 
                    WORD flag2);
EXTERN VOID gr_movebox(WORD w, WORD h, WORD srcx, WORD srcy, 
                       WORD dstx, WORD dsty);
#if GROWBOX
	EXTERN VOID gr_growbox(GRECT *po, GRECT *pt);
	EXTERN VOID gr_shrinkbox(GRECT *po, GRECT *pt);
#endif
EXTERN WORD gr_watchbox(LPTREE tree, WORD obj, WORD instate, WORD outstate);
EXTERN WORD gr_slidebox(LPTREE tree, WORD parent, WORD obj, WORD isvert);
EXTERN WORD gr_mkstate(WORD *pmx, WORD *pmy, WORD *pmstat, WORD *pkstat);


/* GEMSCLIB.C */
EXTERN WORD sc_clrd(WORD isread);
EXTERN WORD sc_read(LPBYTE pscrap);
EXTERN WORD sc_write(LPBYTE pscrap);
EXTERN WORD sc_clear(VOID);


/* GEMFSLIB.C */
EXTERN WORD		fs_exinput(LPBYTE pipath, LPBYTE pisel, WORD *pbutton, 
                LPBYTE pname);

/* GEMWMLIB.C */
VOID w_nilit(WORD num, OBJECT *olist);
VOID w_obadd(OBJECT *olist, WORD parent, WORD child);
VOID w_setup(PD *ppd, WORD w_handle, WORD kind);
WORD *w_getxptr(WORD which, WORD w_handle);
VOID w_getsize(WORD which, WORD w_handle, GRECT *pt);
VOID w_setsize(WORD which, WORD w_handle, GRECT *pt);
VOID w_adjust(WORD parent, WORD obj, WORD x, WORD y, WORD w, WORD h);
VOID w_hvassign3d(WORD isvert, REG WORD parent, WORD obj, WORD vx, WORD vy, 
	                WORD hx, WORD hy, WORD w, WORD h, WORD xa, WORD ya);
VOID w_hvassign(WORD isvert, WORD parent, WORD obj, WORD vx, 
   	            WORD vy, WORD hx, WORD hy, WORD w, WORD h);
WORD do_walk(WORD wh, LPTREE tree, WORD obj, WORD depth, GRECT *pc);
VOID w_drawdesk(GRECT *pc);
VOID w_cpwalk(WORD wh, WORD obj, WORD depth, WORD usetrue);
VOID w_clipdraw(WORD wh, WORD obj, WORD depth, WORD usetrue);
VOID w_strchg(WORD w_handle, WORD obj, LPBYTE pstring);
VOID w_barcalc(WORD isvert, WORD space, WORD sl_value, WORD sl_size, 
               WORD min_sld, GRECT *ptv, GRECT *pth);
VOID w_bldbar(UWORD kind, WORD istop, WORD w_bar, WORD sl_value, 
              WORD sl_size, WORD x, WORD y, WORD w, WORD h);
WORD w_top(VOID);
VOID w_setactive(VOID);
WORD w_bldactive(WORD w_handle);
VOID ap_sendmsg(WORD ap_msg[], WORD type, PD *towhom, WORD w3, 
                WORD w4, WORD w5, WORD w6, WORD w7);
VOID w_redraw(WORD w_handle, GRECT *pt);
WORD w_mvfix(GRECT *ps, GRECT *pd);
WORD w_move(WORD w_handle, WORD *pstop, GRECT *prc);
VOID w_update(WORD bottom, GRECT *pt, WORD top, WORD moved, WORD usetrue);
VOID w_setmen(WORD pid);
VOID w_newmenu(PD *owner);
#if SINGLAPP
	VOID w_menufix(VOID);
#endif
#if MULTIAPP
	VOID w_menufix(PD *rlr);
#endif
WORD w_clswin(VOID);
VOID oldwfix(PD *npd, WORD isdelete);
VOID newwfix(PD *npd);
WORD w_windfix(PD *npd);
WORD draw_change(WORD w_handle, GRECT *pt);
VOID w_owns(WORD w_handle, ORECT *po, GRECT *pt, WORD *poutwds);
WORD w_union(ORECT *po, GRECT *pt);
VOID wm_start(VOID);
WORD wm_create(WORD kind, GRECT *pt);
VOID wm_opcl(WORD wh, GRECT *pt, WORD isadd);
VOID wm_open(WORD w_handle, GRECT *pt);
VOID wm_close(WORD w_handle);
VOID wm_delete(WORD w_handle);
VOID wm_get(WORD w_handle, WORD w_field, WORD *poutwds);
WORD wm_gsizes(WORD w_field, WORD *psl, WORD *psz);
VOID wm_mktop(WORD w_handle);
VOID wm_set(WORD w_handle, WORD w_field, WORD *pinwds);
WORD wm_find(WORD x, WORD y);
VOID wm_update(WORD beg_update);
VOID wm_calc(WORD wtype, UWORD kind, WORD x, WORD y, WORD w, WORD h,
             WORD *px, WORD *py, WORD *pw, WORD *ph);
WORD change_desktop( FDB FAR * bmp );



/* GEMRSLIB.C */
EXTERN WORD rs_gaddr(LPWORD pglobal, UWORD rtype, UWORD rindex, 
                     REG LPVOID *rsaddr);
EXTERN WORD rs_saddr(LPWORD pglobal, UWORD rtype, UWORD rindex, 
                     LPVOID rsaddr);
EXTERN WORD		rs_load(REG LPWORD pglobal, LPBYTE rsfname);
EXTERN WORD     rs_free(LPWORD pglobal);
EXTERN WORD 	rs_obfix(LPTREE tree, WORD curob);
EXTERN BYTE *   rs_str(WORD num);

/* GEMSHLIB.C */
EXTERN WORD 	sh_read(LPBYTE pcmd, LPBYTE ptail);
EXTERN WORD     sh_write(WORD does, WORD isgem, WORD idover, LPBYTE pcmd, LPBYTE ptail);
EXTERN WORD     sh_get(LPVOID pbuffer, WORD len);
EXTERN WORD     sh_put(LPVOID pbuffer, WORD len);
EXTERN WORD     sh_find(LPBYTE pspec);
EXTERN WORD		sh_envrn(LPLPTR ppath, REG LPBYTE psrch);
EXTERN VOID     sh_rdef(LPBYTE, LPBYTE);
EXTERN VOID     sh_wdef(LPBYTE, LPBYTE);

EXTERN WORD		xs_getshell(LPBYTE buf);
EXTERN WORD		xs_setshell(LPBYTE buf);

/* GSX2.AS */
EXTERN VOID     gsx2(VOID);
EXTERN WORD		mul_div(WORD m1, UWORD m2, WORD d1);
EXTERN UWORD 	umul_div(UWORD m1, UWORD m2, UWORD d1);
EXTERN VOID		i_ptsin(WORD *ptsin);
EXTERN VOID 	i_intin(WORD *intin);
EXTERN VOID 	i_ptsout(WORD *ptsout);
EXTERN VOID 	i_intout(WORD *intout);
EXTERN VOID	 	i_ptr(VOID *ptr);
EXTERN VOID		i_ptr2(VOID *ptr);

/* Pacific doesn't let you "overload" (WORD, WORD) and (LONG) */

EXTERN VOID		i_lptr1(VOID *off, WORD seg);
EXTERN VOID		i_lptr1l(LPVOID address);
EXTERN VOID		m_lptr2(LPVOID *ptr);


/* OPTIMIZE.C */
EXTERN WORD		sound(WORD isfreq, WORD freq, WORD dura);
EXTERN WORD 	bit_num(UWORD flag);
EXTERN VOID		rc_constrain(GRECT *pc, GRECT *pt);
EXTERN VOID		rc_union(GRECT *p1, GRECT *p2);
EXTERN WORD		rc_intersect(GRECT *p1, GRECT *p2);
EXTERN WORD 	mid(WORD lo, WORD val, WORD hi);
EXTERN BYTE		*strscn(BYTE *ps, BYTE *pd, BYTE stop);
EXTERN BYTE 	*strcat(BYTE *ps, BYTE *pd);
EXTERN VOID		fmt_str(BYTE *instr, BYTE *outstr);
EXTERN VOID		unfmt_str(BYTE *instr, BYTE *outstr);
EXTERN VOID		fs_sset(LPTREE tree, WORD obj, LPBYTE pstr, LPBYTE *ptext, WORD *ptxtlen);
EXTERN VOID		inf_sset(LPTREE tree, WORD obj, BYTE *pstr);
EXTERN VOID		fs_sget(LPTREE tree, WORD obj, LPBYTE pstr);
EXTERN VOID		inf_sget(LPTREE tree, WORD obj, BYTE *pstr);
EXTERN VOID		inf_fldset(LPTREE tree, WORD obj, UWORD testfld, 
                           UWORD testbit, UWORD truestate, UWORD falsestate);
EXTERN WORD		inf_gindex(LPTREE tree, WORD baseobj, WORD numobj);
EXTERN WORD		inf_what(LPTREE tree, WORD ok, WORD cncl);
EXTERN VOID 	merge_str(BYTE *pdst, BYTE *ptmp, UWORD parms[]);
EXTERN WORD		wildcmp(BYTE *pwld, BYTE *ptst);
EXTERN VOID		ins_char(BYTE *str, WORD pos, BYTE chr, WORD tot_len);
EXTERN BYTE		*op_gname(WORD index);


/* GEMDOSIF.AS */
EXTERN WORD 	drawrat(WORD x, WORD y);
EXTERN VOID 	__DOS(VOID);
EXTERN WORD     __EXEC(VOID);
EXTERN VOID		cli(VOID);	
EXTERN VOID		sti(VOID);
EXTERN VOID 	supret(WORD ret);
EXTERN VOID		takeerr(VOID);
EXTERN VOID		giveerr(VOID);
EXTERN VOID		takecpm(VOID);
EXTERN VOID		givecpm(VOID);
EXTERN VOID 	retake(VOID);
EXTERN VOID	    cpmcod(VOID);
EXTERN VOID 	setdsss(UDA *);
EXTERN UWORD	far_call(LONG code, LONG data);
EXTERN WORD		far_bcha(VOID);
EXTERN WORD 	far_mcha(VOID);
EXTERN VOID		justretf(VOID);
EXTERN WORD 	ini_idle(VOID);
EXTERN VOID		idle(VOID);

/* OPTIMOPT.AS */

EXTERN BYTE toupper(BYTE ch);
EXTERN VOID r_get(GRECT *pxywh, WORD *px, WORD *py, WORD *pw, WORD *ph);
EXTERN VOID r_set(GRECT *pxywh, WORD x,   WORD y,   WORD w,   WORD h);
EXTERN WORD rc_equal(GRECT *prc1, GRECT *prc2);
EXTERN VOID rc_copy(GRECT *psrc, GRECT *pdst);
EXTERN UWORD inside(WORD x, WORD y, GRECT *pt);
EXTERN WORD min(WORD a, WORD b);
EXTERN WORD max(WORD a, WORD b);
EXTERN BYTE toupper(BYTE ch);
EXTERN BYTE *scasb(BYTE *p, BYTE b);
EXTERN VOID movs(WORD num, VOID *ps, VOID *pd);
EXTERN VOID bfill(WORD num, unsigned char bval, VOID *addr);
EXTERN WORD strlen(BYTE *p1);
EXTERN WORD strcmp(BYTE *p1, BYTE *p2);
EXTERN WORD strchk(BYTE *s,  BYTE *t);
EXTERN WORD strcpy(BYTE *ps, BYTE *pd);

/* LARGE.AS */

EXTERN LONG HW(UWORD x);
EXTERN UWORD LHIWD(LONG x);
EXTERN WORD LSTRLEN(LPBYTE addr);
EXTERN LONG LLCS(VOID);
EXTERN LONG LLDS(VOID);
EXTERN LONG LLSS(VOID);
EXTERN WORD LWCOPY(LPVOID pdst, LPVOID psrc, WORD count);
EXTERN BYTE LBCOPY(LPVOID pdst, LPVOID psrc, WORD count);
EXTERN WORD LBWMOV(LPWORD pdst, LPBYTE psrc);
EXTERN WORD LSTCPY(LPBYTE pdst, LPBYTE psrc);

/* GEMSTART.AS */

EXTERN WORD pgmld(WORD handle, BYTE *ploadname, LPBYTE *pldaddr, WORD *paccroom);


/* GEMDISP.C */
EXTERN VOID forkq(FPD_CODE fcode, WORD lodata, WORD hidata);
EXTERN VOID disp_act(PD *p);
EXTERN VOID mwait_act(PD *p);
EXTERN VOID forker(VOID);
EXTERN VOID chkkbd(VOID);
EXTERN VOID schedule(VOID);


/* GEMPD.C */
EXTERN PD *fpdnm(BYTE *pname, UWORD pid);
EXTERN PD *pstart(BYTE *pcode, BYTE *pfilespec, LPBYTE ldaddr);
EXTERN PD *pd_index(WORD i);
EXTERN PD *getpd(VOID);
EXTERN VOID p_nameit(PD *p, BYTE *pname);
EXTERN VOID insert_process(PD *pi, PD **root);

/* GEMQUEUE.C */

EXTERN VOID doq(WORD donq, PD *p, QPB *m);
EXTERN VOID aqueue(WORD isqwrite, EVB *e, LONG lm);


/* GEMWRECT.C */
EXTERN VOID  or_start(VOID);
EXTERN ORECT *get_orect(VOID);
EXTERN ORECT *mkpiece(WORD tlrb, ORECT *new, ORECT *old);
EXTERN ORECT *brkrct(ORECT *new, ORECT *r, ORECT *p);
EXTERN VOID mkrect(LPTREE tree, WORD wh, WORD d1, WORD d2);
EXTERN VOID newrect(LPTREE tree, WORD wh, WORD d1, WORD d2);

/* GEMMNLIB.C */
EXTERN VOID rect_change(LPTREE tree, MOBLK *prmob, WORD iob, WORD x);
EXTERN VOID mn_init(VOID);
EXTERN WORD mn_getda(PD *ppd);
EXTERN VOID menu_fixup(BYTE *pname);
EXTERN UWORD do_chg(LPTREE tree, WORD iitem, UWORD chgvalue, 
                    WORD dochg, WORD dodraw, WORD chkdisabled);
EXTERN WORD menu_set(LPTREE tree, WORD last_item, WORD cur_item, WORD setit);
EXTERN VOID menu_sr(WORD saveit, LPTREE tree, WORD imenu);
EXTERN WORD menu_sub(LPTREE *ptree, WORD ititle);
EXTERN WORD menu_down(WORD ititle);
EXTERN WORD mn_do(WORD *ptitle, WORD *pitem, WORD key);
EXTERN VOID mn_bar(LPTREE tree, WORD showit, WORD pid);

#if SINGLAPP
	VOID mn_clsda(VOID);
#endif

WORD mn_register(WORD pid, LPBYTE pstr);
VOID mn_unregister(WORD da_id);
#if MULTIAPP
	WORD mn_ppdtoid(PD *p);
#endif

/* GEMCTRL.C */

EXTERN VOID ct_msgup(WORD message, PD *owner, WORD wh, WORD m1, WORD m2, 
              WORD m3, WORD m4);
EXTERN VOID hctl_window(WORD w_handle, WORD mx, WORD my);
EXTERN VOID hctl_rect(WORD key);
EXTERN VOID ct_chgown(PD *mpd, GRECT *pr);
EXTERN VOID ct_mouse(WORD grabit);
#if MULTIAPP
	EXTERN VOID hctl_mesag(WORD pmbuf[]);
#endif
EXTERN VOID ctlmgr(VOID);

/* GEMGSXIF.C */

typedef WORD (*MBFUNC)(VOID);

EXTERN ULONG gsx_mcalc(VOID);
EXTERN VOID gsx_malloc(VOID);
EXTERN VOID gsx_mfree(VOID);
EXTERN VOID gsx_mret(LPBYTE *pmaddr, LONG *pmlen);
EXTERN VOID gsx_ncode(WORD code, WORD n, WORD m);
EXTERN VOID gsx_1code(WORD code, UWORD value);
EXTERN VOID gsx_init(VOID);
EXTERN VOID gsx_exec(LPBYTE pcspec, WORD segenv, LPBYTE pcmdln, LPVOID pfcb1, 
              LPVOID pfcb2);
EXTERN VOID gsx_graphic(WORD tographic);
EXTERN VOID gsx_wsopen(VOID);
EXTERN VOID gsx_wsclose(VOID);
EXTERN VOID ratinit(VOID);
EXTERN VOID ratexit(VOID);
EXTERN VOID bb_set(WORD sx, WORD sy, WORD sw, WORD sh, 
                   WORD *pts1, WORD *pts2, FDB *pfd, FDB *psrc, FDB *pdst);
EXTERN VOID bb_save(GRECT *ps);
EXTERN VOID bb_restore(GRECT *pr);
EXTERN VOID gsx_setmb(MBFUNC boff, MBFUNC moff, LPVOID *pdrwaddr);
EXTERN VOID gsx_resetmb(VOID);
EXTERN WORD gsx_tick(LPVOID tcode, LPVOID *ptsave);
EXTERN VOID gsx_mfset(LPMFORM pmfnew);
EXTERN VOID gsx_mxmy(WORD *pmx, WORD *pmy);
EXTERN WORD gsx_button(VOID);
EXTERN WORD gsx_kstate(VOID);
EXTERN VOID gsx_moff(VOID);
EXTERN VOID gsx_mon(VOID);
EXTERN WORD gsx_char(VOID);
EXTERN ULONG gsx_gbufflen(VOID);
EXTERN VOID v_opnwk( WORD *pwork_in, WORD *phandle, WORD *pwork_out );
EXTERN VOID v_pline( WORD count, WORD *pxyarray );
EXTERN VOID vst_clip( WORD clip_flag, WORD *pxyarray );
EXTERN VOID vst_height( WORD height, WORD *pchr_width, WORD *pchr_height, 
                        WORD *pcell_width, WORD *pcell_height );
EXTERN VOID vr_recfl( WORD *pxyarray, WORD *pdesMFDB );
EXTERN VOID vro_cpyfm( WORD wr_mode, WORD *pxyarray, 
                       FDB *psrcMFDB, FDB *pdesMFDB );
EXTERN VOID vrt_cpyfm( WORD wr_mode, WORD *pxyarray, 
                       FDB *psrcMFDB, FDB *pdesMFDB, WORD fgcolor, WORD bgcolor );
EXTERN VOID vrn_trnfm( FDB *psrcMFDB, FDB *pdesMFDB );
EXTERN VOID vsl_width( WORD width );

#define vsf_interior( x )	gsx_1code(S_FILL_STYLE, x)
#define vsl_type( x )		gsx_1code(S_LINE_TYPE, x)
#define vsf_style( x )		gsx_1code(S_FILL_INDEX, x)
#define vsf_color( x )		gsx_1code(S_FILL_COLOR, x)
#define vsl_udsty( x )		gsx_1code(ST_UD_LINE_STYLE, x)


/* GEMCICON.C */
VOID	render_bmp(FDB FAR* fdb, WORD x, WORD y);
VOID	render_ico(FDB FAR *mask, FDB FAR *data, WORD x, WORD y, WORD selected);

/* GEMPROP.C */
WORD prop_get(LPBYTE program, LPBYTE section, LPBYTE buf, WORD buflen, WORD options);
WORD prop_put(LPBYTE program, LPBYTE section, LPBYTE buf, WORD options);
WORD prop_gui_set(WORD property, WORD value);
WORD prop_gui_get(WORD property);


#if DEBUG

	EXTERN VOID TRACE(BYTE *S);	/* Output $-terminated string */
	EXTERN VOID ophex(WORD N);	/* Output hex integer */
	EXTERN BYTE TRCHAR(BYTE B);	/* Output character */
	EXTERN BYTE TRKEY(VOID);	/* Get keypress */
	EXTERN VOID border(BYTE B);	/* Set border */

#endif /* DEBUG */

#if EMSDESK

/* GEMEMSIF.AS */
VOID	EMS_Free(WORD handle);
BYTE	EMS_Map(WORD handle, BYTE ppage, WORD lpage);
WORD	EMS_Alloc(WORD num_pages);
BYTE	EMS_Init(BYTE *emm_vers, WORD *pf_addr, WORD *ppgs, WORD *lpgs);
WORD	EMS_Check(VOID);
WORD	EMS_Avail(VOID);

/* "Special" ob_spec which puts the desktop image in EMS
 *
 * We can't use FFFFFFFFL because an ob_spec of 0xFFFFFFFFL is
 * exactly equivalent to setting the HIDDEN flag on an object.
 */ 
#define DTMFDB_EMS_SPEC	0xFFFFFFFEL

#endif

/*

GEMSUPER.C
GEMGSXIF.C
GEMDOS.C
MULL.C
UNTITLED.C
DEBUG.C
GEMBASE.C
*/
#endif /* ndef __AESPROTO_H_INCLUDED__ */



