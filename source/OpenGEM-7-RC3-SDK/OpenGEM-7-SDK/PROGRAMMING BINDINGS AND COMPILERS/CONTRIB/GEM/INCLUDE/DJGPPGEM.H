/* JCE 3-1-1998: The single 'include' file necessary for DJGPP to compile 
   GEM applications.

   This file contains prototypes for all AES and VDI functions. 
*/

#ifndef __DJGPPGEM_H__

#define __DJGPPGEM_H__ 5/10/2002

#include "portab.h"	/* BYTE, WORD, LONG etc. */
#include "machine.h"	/* LWGET, LWSET etc. */
#include "dosalloc.h"	/* Allocate & free DOS memory */
#include "obdefs.h"	/* GEM structures */
#include "djdefs.h"	/* DJGPP-specific functions */
#include "djgbind.h"	/* GEM binding #defines */
#include "treeaddr.h"	/* Object field macros */
#include "djtree.h"	/* Tree 16/32bit conversions */

/* Utility functions */

LPBYTE dj_string_addr(char *s);	/* Copy a string into DOS memory. To free
                                 * it afterwards, use dos_free() on it */

WORD dj_form_alert(WORD opt, char *s, ...); /* Do a form_alert using a string in */
                                       /* linear memory */
WORD dj_lstrlen(LPBYTE s);	       /* Do a strlen() on a string in DOS memory */
WORD dj_putb2w(BYTE *s, LPBYTE d);     /* Convert string to list of integers */
WORD dj_getw2b(LPBYTE s, WORD cnt, BYTE *d);     /* Convert list of integers to string */
WORD dj_wind_setl(WORD w_handle, WORD w_field, LPVOID l1, LPVOID l2); /* pass pointers to wind_set() */

/* Object painting callbacks */
WORD dj_userdef(LPTREE tree, WORD object, DJUSERBLK *djb);
void dj_unuserdef(DJUSERBLK *djb);
WORD dj_userdef_prealloc(WORD count);

/* Miscellaneous prototypes */

WORD GEMAIN(WORD ARGC, BYTE *ARGV[]);
WORD aes(VOID);
WORD vdi(VOID);
WORD gemcheck(VOID);
WORD aescheck(VOID);
WORD vdi_alloc_arrays(VOID);
WORD aes_alloc_arrays(VOID);
AESFUNC divert_aes(AESFUNC newf);
VDIFUNC divert_vdi(VDIFUNC newf);

/* Prototypes for the AES functions */

WORD appl_init   (X_BUF_V2 *xbuf);
WORD appl_exit   (void);
WORD appl_write  (WORD  rwid,    WORD length, WORD *pbuff);
WORD appl_read   (WORD  rwid,    WORD length, WORD *pbuff);
WORD appl_find   (BYTE *pname);
WORD appl_tplay  (LPVOID  tbuffer, WORD  tlength, WORD tscale);
WORD appl_trecord(LPVOID  tbuffer, WORD  tlength);
WORD appl_bvset  (UWORD bvdisk,  UWORD bvhard);
VOID appl_yield  (void);
WORD appl_xbvget(ULONG *bvdisk, ULONG *bvhard);
WORD appl_xbvset(ULONG bvdisk, ULONG bvhard);

UWORD evnt_keybd(void);
WORD  evnt_button(WORD clicks, UWORD mask, UWORD state, WORD *pmx, 
		  WORD *pmy, WORD *pmb, WORD *pks);
WORD  evnt_mouse(WORD flags, WORD x, WORD y, WORD width, WORD height,
		  WORD *pmx, WORD *pmy, WORD *pmb, WORD *pks);
WORD  evnt_mesag(WORD buff[8]);
WORD  evnt_timer(UWORD locnt, UWORD hicnt);
WORD  evnt_multi(UWORD flags, UWORD bclk, UWORD bmsk, UWORD bst, 
		 UWORD m1flags, UWORD m1x, UWORD m1y, UWORD m1w, UWORD m1h, 
		 UWORD m2flags, UWORD m2x, UWORD m2y, UWORD m2w, UWORD m2h, 
                 WORD mepbuff[8], UWORD tlc, UWORD thc, UWORD *pmx, UWORD *pmy,
		 UWORD *pmb, UWORD *pks, UWORD *pkr, UWORD *pbr);
WORD evnt_dclick(WORD rate, WORD setit);
					
WORD menu_bar(     LPTREE tree, WORD showit);
WORD menu_icheck(  LPTREE tree, WORD itemnum,  WORD checkit);
WORD menu_ienable( LPTREE tree, WORD itemnum,  WORD enableit);
WORD menu_tnormal( LPTREE tree, WORD titlenum, WORD normalit);
WORD menu_text(    LPTREE tree, WORD inum,     LPBYTE ptext);
WORD menu_register(WORD pid, LPVOID pstr);
WORD menu_unregister(WORD mid);

WORD objc_add(LPTREE tree, WORD parent, WORD child);
WORD objc_delete(LPTREE tree, WORD delob);
WORD objc_draw(LPTREE tree, WORD drawob, WORD depth, WORD xc, WORD yc, 
                                                   WORD wc, WORD hc);
WORD objc_find(LPTREE tree, WORD startob, WORD depth, WORD mx, WORD my);
WORD objc_order(LPTREE tree, WORD mov_obj, WORD newpos);
WORD objc_offset(LPTREE tree, WORD obj, WORD *poffx, WORD *poffy);
WORD objc_edit(LPTREE tree, WORD obj, WORD inchar, WORD *idx, WORD kind);
WORD objc_change(LPTREE tree, WORD drawob, WORD depth, 
                 WORD xc, WORD yc, WORD wc, WORD hc, 
                 WORD newstate, WORD redraw);
					
WORD form_do(LPVOID form, WORD start);
WORD form_dial(WORD dtype, WORD ix, WORD iy, WORD iw, WORD ih, 
                           WORD x,  WORD y,  WORD w,  WORD h);
WORD form_alert(WORD defbut, LPBYTE astring);
WORD form_error(WORD errnum);
WORD form_center(LPTREE tree, WORD *pcx, WORD *pcy, WORD *pcw, WORD *pch);
WORD form_keybd(LPVOID form, WORD obj, WORD nxt_obj, WORD thechar, 
		WORD *pnxt_obj, WORD *pchar);
WORD form_button(LPVOID form, WORD obj, WORD clks, WORD *pnxt_obj);

WORD graf_rubbox(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin, 
		 WORD *pwend, WORD *phend);
WORD graf_dragbox(WORD w,  WORD h,  WORD sx,   WORD sy, WORD xc, WORD yc, 
                  WORD wc, WORD hc, WORD *pdx, WORD *pdy);
WORD graf_mbox(WORD w, WORD h, WORD srcx, WORD srcy, WORD dstx, WORD dsty);
WORD graf_watchbox(LPTREE tree, WORD obj, UWORD instate, UWORD outstate);
WORD graf_slidebox(LPTREE tree, WORD parent, WORD obj, WORD isvert);
WORD graf_handle(WORD *pwchar, WORD *phchar, WORD *pwbox, WORD *phbox);
WORD graf_mouse(WORD m_number, LPUWORD m_addr);
VOID graf_mkstate(WORD *pmx, WORD *pmy, WORD *pmstate, WORD *pkstate);
/* Obsolete, for compatibility only */
WORD graf_growbox(WORD,WORD,WORD,WORD,WORD,WORD,WORD,WORD);
WORD graf_shrinkbox(WORD,WORD,WORD,WORD,WORD,WORD,WORD,WORD);

WORD scrp_read(char *pscrap);
WORD scrp_write(char *pscrap);
WORD scrp_clear(void);

WORD fsel_input(BYTE * pipath, BYTE * pisel, WORD *pbutton);
WORD fsel_exinput(BYTE * pipath, BYTE * pisel, WORD *pbutton, BYTE *title);

WORD  wind_create(UWORD kind,   WORD wx, WORD wy, WORD ww, WORD wh);
WORD  wind_open  ( WORD handle, WORD wx, WORD wy, WORD ww, WORD wh);
WORD  wind_close ( WORD handle);
WORD  wind_delete( WORD handle);
WORD  wind_get(    WORD w_handle, WORD w_field, WORD *pw1, WORD *pw2, 
						WORD *pw3, WORD *pw4);
WORD  wind_set(    WORD w_handle, WORD w_field, WORD w2, WORD w3, 
                                                WORD w4, WORD w5);
WORD  wind_find(   WORD mx, WORD my);
WORD  wind_update( WORD beg_update);
WORD  wind_calc(   WORD wctype, UWORD kind, 
		   WORD x,   WORD y,   WORD w,   WORD h,
                   WORD *px, WORD *py, WORD *pw, WORD *ph);
					

WORD  rsrc_load(BYTE *rsname);
WORD  rsrc_free(void);
WORD  rsrc_gaddr(WORD rstype, WORD rsid, LPVOID *paddr);
WORD  rsrc_saddr(WORD rstype, WORD rsid, LPVOID lngval);
WORD  rsrc_obfix(LPTREE tree, WORD obj);

WORD shel_read(LPVOID pcmd, LPVOID ptail);
WORD shel_write(WORD doex, WORD isgr, WORD iscr, LPVOID pcmd, LPVOID ptail);
WORD shel_find(LPVOID ppath);
WORD shel_envrn(LPVOID ppath, LPVOID psrch);
WORD shel_rdef(LPVOID lpcmd, LPVOID lpdir);
WORD shel_wdef(LPVOID lpcmd, LPVOID lpdir);
WORD shel_get(LPVOID buf, WORD len);
WORD shel_put(LPVOID buf, WORD len);

WORD xgrf_stepcalc(WORD orgw,  WORD orgh,    WORD xc,    WORD yc, 
                   WORD w,     WORD h,       WORD *pcx,  WORD *pcy, 
                   WORD *pcnt, WORD *pxstep, WORD *pystep);
WORD xgrf_2box(    WORD xc,    WORD yc,      WORD w,     WORD h,
                   WORD corners, WORD cnt,   WORD xstep, WORD ystep,
                   WORD doubled);

/* ViewMAX/2 "set colours" call */

WORD xgrf_colour(WORD type, WORD fg, WORD bg, WORD style, WORD index);
#define xgrf_color xgrf_colour

/* ViewMAX/3 "set desktop image" call */

WORD xgrf_dtimage(LPMFDB deskMFDB);

/* JCE 24-7-1999 FreeGEM "property" calls */

WORD prop_get(char *program, char *section, char *buf, WORD buflen, WORD options);
WORD prop_put(char *program, char *section, char *buf, WORD options);
WORD prop_del(char *program, char *section, WORD options);
WORD prop_gui_get(WORD property);
WORD prop_gui_set(WORD property, WORD value);

WORD xapp_getinfo(WORD ap_gtype, WORD *ap_gout1, WORD *ap_gout2,
				 WORD *ap_gout3, WORD *ap_gout4);

#define appl_getinfo xapp_getinfo /* for Atari compatibility */

WORD xshl_getshell(char *shell);
WORD xshl_setshell(char *shell);

WORD menu_click(WORD click, WORD setit);	/* GEM/3 only */

/* Prototypes for the VDI functions */

VOID vro_cpyfm( WORD handle, WORD wr_mode, WORD xy[], MFDB *srcMFDB, MFDB *desMFDB );
VOID vr_trnfm(  WORD handle, MFDB *srcMFDB, MFDB *desMFDB );

WORD v_opnwk( WORD work_in[11], WORD *handle, WORD work_out[57] );
VOID v_clswk( WORD handle );
VOID v_clrwk( WORD handle );
VOID v_updwk( WORD handle );
VOID vq_chcells(  WORD handle, WORD *rows, WORD *columns );
VOID v_exit_cur(  WORD handle );
VOID v_enter_cur( WORD handle );
VOID v_curup(     WORD handle );
VOID v_curdown(   WORD handle );
VOID v_curright(  WORD handle );
VOID v_curleft(   WORD handle );
VOID v_curhome(   WORD handle );
VOID v_eeos(      WORD handle );
VOID v_eeol(      WORD handle );
VOID vs_curaddress( WORD handle, WORD row, WORD column );
VOID v_curtext(     WORD handle, BYTE *string );
VOID v_rvon(        WORD handle );
VOID v_rvoff(       WORD handle );
VOID vq_curaddress( WORD handle, WORD *row, WORD *column );
WORD vq_tabstatus(  WORD handle );
VOID v_hardcopy(    WORD handle );
VOID v_dspcur(      WORD handle, WORD x, WORD y );
VOID v_rmcur(       WORD handle );
VOID v_form_adv(    WORD handle );
VOID v_output_window( WORD handle, WORD xy[] );
VOID v_clear_disp_list( WORD handle );
VOID v_bit_image( WORD handle,  BYTE *filename, WORD aspect, 
                  WORD x_scale, WORD y_scale,   WORD h_align, WORD v_align, 
		  WORD xy[] );
VOID vq_scan(     WORD handle, WORD *g_height, WORD *g_slice, 
                               WORD *a_height, WORD *a_slice, WORD *factor );
VOID v_alpha_text( WORD handle, BYTE *string );
WORD vs_palette( WORD handle, WORD palette );
VOID v_sound( WORD handle, WORD frequency, WORD duration );
WORD vs_mute( WORD handle, WORD action );
VOID vsp_film( WORD handle, WORD index, WORD lightness );
BOOLEAN vqp_filmname( WORD handle, WORD index, BYTE *name );
VOID vsc_expose( WORD handle, WORD state );
VOID v_meta_extents( WORD handle, WORD min_x, WORD min_y, 
                                  WORD max_x, WORD max_y );
VOID v_write_meta( WORD handle, WORD num_ints, LPWORD ints, 
                                WORD num_pts,  LPWORD pts );
VOID vm_filename( WORD handle, BYTE *filename );
VOID vm_pagesize( WORD handle, WORD pgwidth, WORD pgheight );
VOID vm_coords( WORD handle, WORD llx, WORD lly, WORD urx, WORD ury );
VOID vt_resolution ( WORD handle, WORD xres, WORD yres, WORD *xset, WORD *yset );
VOID vt_axis ( WORD handle, WORD xres, WORD yres, WORD *xset, WORD *yset );
VOID vt_origin ( WORD handle, WORD xorigin, WORD yorigin );
VOID vq_tdimensions( WORD handle, WORD *xdimension, WORD *ydimension );
VOID vt_alignment( WORD handle, WORD dx, WORD dy );
VOID v_pline(   WORD handle, WORD count, WORD xy[] );
VOID v_pmarker( WORD handle, WORD count, WORD xy[] );
VOID v_gtext(   WORD handle, WORD x, WORD y, BYTE *string);
VOID v_fillarea( WORD handle, WORD count, WORD xy[]);
VOID v_cellarray( WORD handle, WORD xy[4], WORD row_length, 
                  WORD el_per_row, WORD num_rows, WORD wr_mode, LPWORD colors );
VOID v_bar( WORD handle, WORD xy[] );
VOID v_arc( WORD handle, WORD xc, WORD yc, WORD rad, WORD sang, WORD eang );
VOID v_pieslice( WORD handle, WORD xc, WORD yc, WORD rad, WORD sang, WORD eang );
VOID v_circle(  WORD handle, WORD xc, WORD yc, WORD rad );
VOID v_ellipse( WORD handle, WORD xc, WORD yc, WORD xrad, WORD yrad );
VOID v_ellarc(  WORD handle, WORD xc, WORD yc, WORD xrad, WORD yrad, 
                WORD sang,   WORD eang );
VOID v_ellpie( WORD handle, WORD xc, WORD yc, WORD xrad, WORD yrad, 
	       WORD sang,   WORD eang);
VOID v_rbox(   WORD handle, WORD xy[] );
VOID v_rfbox(  WORD handle, WORD xy[] );
VOID v_justified( WORD handle, WORD x, WORD y, BYTE string[], WORD length, 
                  WORD word_space, WORD char_space);
WORD vqt_justified( WORD handle, WORD x, WORD y, BYTE string[], 
		    WORD length, WORD word_space, WORD char_space, 
                    LPWORD offsets);
VOID vst_height( WORD handle, WORD height, 
                 WORD *char_width, WORD *char_height, 
                 WORD *cell_width, WORD *cell_height );
WORD vst_rotation( WORD handle, WORD angle );
VOID vs_color(   WORD handle, WORD index, WORD *rgb );
WORD vsl_type(   WORD handle, WORD style );
WORD vsl_width(  WORD handle, WORD width );
WORD vsl_color(  WORD handle, WORD index );
WORD vsm_type(   WORD handle, WORD symbol );
WORD vsm_height( WORD handle, WORD height );
WORD vsm_color(  WORD handle, WORD index );
WORD vst_font(   WORD handle, WORD font );
WORD vst_color(  WORD handle, WORD index );
WORD vsf_interior( WORD handle, WORD style );
WORD vsf_style(   WORD handle, WORD index );
WORD vsf_color(   WORD handle, WORD index );
VOID vq_color( WORD handle, WORD index, WORD set_flag, WORD rgb[] );
VOID vq_cellarray( WORD handle,   WORD xy[4], WORD row_len, WORD num_rows, 
		   WORD *el_used, WORD *rows_used, WORD *stat, LPVOID colors );
VOID vrq_locator( WORD handle, WORD initx, WORD inity, 
                  WORD *xout, WORD *yout, WORD *term );
WORD vsm_locator( WORD handle, WORD initx, WORD inity, 
                  WORD *xout, WORD *yout, WORD *term );
VOID vrq_valuator( WORD handle, WORD val_in, WORD *val_out, WORD *term );
VOID vsm_valuator( WORD handle, WORD val_in, WORD *val_out, WORD *term, 
                   WORD *status );
VOID vrq_choice( WORD handle, WORD in_choice, WORD *out_choice );
WORD vsm_choice( WORD handle, WORD *choice );
VOID vrq_string( WORD handle, WORD length, WORD echo_mode, WORD echo_xy[],
                 BYTE *string);
WORD vsm_string( WORD handle, WORD length, WORD echo_mode, WORD echo_xy[],
                 BYTE *string );
WORD vswr_mode( WORD handle, WORD mode );
VOID vsin_mode( WORD handle, WORD dev_type, WORD mode );
VOID vql_attributes( WORD handle, WORD attributes[] );
VOID vqm_attributes( WORD handle, WORD attributes[] );
VOID vqf_attributes( WORD handle, WORD attributes[] );

/* [JCE] Returns FALSE if the VDI does not support this call */
BOOLEAN vqt_attributes( WORD handle, WORD attributes[10] );

VOID vst_alignment( WORD handle, WORD hor_in,   WORD vert_in, 
                                 WORD *hor_out, WORD *vert_out );
VOID v_opnvwk( WORD work_in[11], WORD *handle, WORD work_out[57] );
VOID v_clsvwk( WORD handle );
VOID vq_extnd( WORD handle, WORD owflag, WORD work_out[] );
VOID v_contourfill( WORD handle, WORD x, WORD y, WORD index );
WORD vsf_perimeter( WORD handle, WORD per_vis );
BOOLEAN v_get_pixel( WORD handle, WORD x, WORD y, WORD *pel, WORD *index );
WORD vst_effects( WORD handle, WORD effect );
WORD vst_point( WORD handle, WORD point, WORD *char_width, WORD *char_height, 
                WORD *cell_width, WORD *cell_height );
VOID vsl_ends( WORD handle, WORD beg_style, WORD end_style);
VOID vsc_form( WORD handle, WORD *cur_form );
VOID vsf_udpat( WORD handle, WORD fill_pat[], WORD planes );
VOID vsl_udsty( WORD handle, WORD pattern );
VOID vr_recfl( WORD handle, WORD *xy );
VOID vqin_mode( WORD handle, WORD dev_type, WORD *mode );
VOID vqt_extent( WORD handle, BYTE string[], WORD *extent );
WORD vqt_width( WORD handle, BYTE character, WORD *cell_width, 
                WORD *left_delta, WORD *right_delta );
VOID vex_timv( WORD handle, LPVOID tim_addr, LPVOID *old_addr, WORD *scale );
WORD vst_load_fonts( WORD handle, WORD select );
VOID vst_unload_fonts( WORD handle, WORD select );
VOID vrt_cpyfm( WORD handle, WORD wr_mode, WORD xy[],
                MFDB *srcMFDB, MFDB *desMFDB, WORD *index );
VOID v_show_c( WORD handle, WORD reset );
VOID v_hide_c( WORD handle );
VOID vq_mouse( WORD handle, WORD *status, WORD *px, WORD *py );
VOID vex_butv( WORD handle, LPVOID usercode, LPVOID *savecode );
VOID vex_motv( WORD handle, LPVOID usercode, LPVOID *savecode );
VOID vex_curv( WORD handle, LPVOID usercode, LPVOID *savecode );
VOID vq_key_s( WORD handle, WORD *status );
VOID vs_clip( WORD handle, WORD clip_flag, WORD xy[] );
WORD vqt_name( WORD handle, WORD element_num, BYTE name[] );
BOOLEAN vqt_font_info( WORD handle, WORD *minADE, WORD *maxADE, 
                    WORD distances[], WORD *maxwidth, WORD effects[] );

/* The following functions are GEM/3 only*/

VOID v_copies( WORD handle, WORD count);
VOID v_etext(  WORD handle, WORD x, WORD y, UBYTE string[], WORD offsets[]);
VOID v_orient( WORD handle, WORD orientation );
VOID v_tray(   WORD handle, WORD tray );

WORD v_xbit_image( WORD handle,     BYTE *filename,  WORD aspect, WORD x_scale, 
                   WORD y_scale,    WORD h_align,    WORD v_align, WORD rotate, 
                   WORD background, WORD foreground, WORD xy[] );
WORD vst_ex_load_fonts( WORD handle, WORD select, WORD font_max, WORD font_free );

/* 3.1 VDI */
VOID v_ps_halftone(WORD handle, WORD index, WORD angle, WORD frequency);
VOID v_setrgbi(WORD handle, WORD primtype, WORD r, WORD g, WORD b, WORD i);
VOID v_topbot(WORD handle, WORD height, WORD *char_width, WORD *char_height, 
                                        WORD *cell_width, WORD *cell_height);
VOID vs_bkcolor(WORD handle, WORD color);
VOID v_set_app_buff(LPVOID address, WORD nparagraphs);
WORD v_bez_on(WORD handle);
WORD v_bez_off(WORD handle);
WORD v_bez_qual(WORD handle, WORD prcnt);
VOID vs_grayoverride(WORD handle, WORD grayval);
VOID v_bez(WORD handle, WORD count, LPWORD xyarr, LPVOID bezarr, WORD *minmax, WORD *npts, WORD *nmove);
VOID v_bezfill(WORD handle, WORD count, LPWORD xyarr, LPVOID bezarr, WORD *minmax, WORD *npts, WORD *nmove);


/*** DOS functions ***/

/* [JCE] Note: These dos_* functions bypass the DOS extender and 
   directly call real-mode DOS. If it is at all possible, use DJGPP's own
   libraries or _dos_* functions instead. */

VOID dos_func(UWORD ax, UWORD lodsdx, UWORD hidsdx);
VOID dos_chdir(LPBYTE pdrvpath);
WORD dos_gdir(WORD drive, LPBYTE pdrvpath);
WORD dos_gdrv(VOID);
WORD dos_sdrv(WORD newdrv);
VOID dos_sdta(LPVOID ldta);
WORD dos_sfirst(LPBYTE pspec, WORD attr);
WORD dos_snext(VOID);
WORD dos_open(LPBYTE pname, WORD access);
WORD dos_close(WORD handle);
WORD read_piece(WORD handle, UWORD cnt, LPVOID pbuffer);
LONG dos_read(WORD handle, LONG cnt, LPVOID pbuffer);
LONG dos_lseek(WORD handle, WORD smode, LONG sofst);
WORD dos_wait();
LONG dos_avail(VOID);
VOID dos_space(WORD drv, LONG *ptotal, LONG *pavail);
WORD dos_rmdir(LPBYTE ppath);
WORD dos_create(LPBYTE pname, WORD attr);
WORD dos_mkdir(LPBYTE ppath);
WORD dos_delete(LPBYTE pname);
WORD dos_rename(LPBYTE poname, LPBYTE pnname);
WORD write_piece(WORD handle, UWORD cnt, LPVOID pbuffer);
LONG dos_write(WORD handle, LONG cnt, LPVOID pbuffer);
WORD dos_chmod(LPBYTE pname, WORD func, WORD attr);
VOID dos_setdt(WORD handle, WORD time, WORD date);

/*** GEM 3.1 PTK extra functions ***/

WORD rc_equal(GRECT *p1, GRECT *p2);
VOID rc_copy(GRECT *psbox, GRECT *pdbox);
WORD rc_intersect(GRECT *p1, GRECT *p2);
WORD rc_inside(WORD x, WORD y, GRECT *pt);
VOID rc_grect_to_array(GRECT *area, WORD *array);

VOID ob_undostate(LPTREE tree, WORD index, WORD state);
WORD ob_isstate(LPTREE tree, WORD index, WORD state);
VOID ob_undoflag(LPTREE tree, WORD index, WORD flag);
WORD ob_isflag(LPTREE tree, WORD index, WORD flag);
LPBYTE ob_get_text(LPTREE tree, WORD index, WORD clear);
VOID ob_draw_dialog(LPTREE tree, WORD xl, WORD yl, WORD wl, WORD hl);
VOID ob_undraw_dialog(LPTREE tree, WORD xl, WORD yl, WORD wl, WORD hl);
VOID ob_dostate(LPTREE tree, WORD index, WORD state);
VOID ob_doflag(LPTREE tree, WORD index, WORD flag);
VOID ob_set_text(LPTREE tree, WORD index, LPBYTE p);

WORD x_ytox(WORD y);
WORD x_udx_xform(WORD user_x);
WORD x_udy_xform(WORD user_y);
WORD x_dux_xform(WORD x);
WORD x_duy_xform(WORD y);
WORD x_udx_scale(WORD user_dx);
WORD x_udy_scale(WORD user_dy);
WORD x_dux_scale(WORD dev_x);
WORD x_duy_scale(WORD dev_y);
WORD x_sxform(WORD user_x, WORD user_y, WORD user_w, WORD user_h, 
              WORD dev_x,  WORD dev_y,  WORD dev_w, WORD dev_h);
VOID x_saspect(WORD user_w, WORD user_h, WORD *dev_w, WORD *dev_h, 
               WORD w_microns, WORD h_microns);
WORD x_mul_div(WORD m1, WORD m2, WORD d1);


VOID fix_icon       (WORD vdi_handle, LPTREE tree);
VOID trans_gimage   (WORD vdi_handle, LPTREE tree, WORD obj);
WORD form_exdo(LPTREE tree, WORD start_fld);
WORD evnt_event(MEVENT *mevent);

#include "djgutils.h"

#endif /* ndef __DJGPPGEM_H__ */
