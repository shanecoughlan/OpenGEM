/* EXTERNAL declarations of variables for GEM OUTPUT.	*/
/* Susan Bancroft 		5/6/85			*/


EXTERN BYTE	btch_hdr[ 10 ] ;
EXTERN BYTE 	names[ MAX_ENTRIES ][ FNAME_LENGTH ] ;
EXTERN BYTE	cur_file[ FNAME_LENGTH ] ;
EXTERN BYTE	list_name[ FNAME_LENGTH ] ;
EXTERN BYTE	lis_dir[ PATH_LENGTH ] ;
EXTERN BYTE	opt_dir[ PATH_LENGTH ] ;
EXTERN BYTE	lis_wc[ PATH_LENGTH ] ;
EXTERN BYTE	gsx_wc[ PATH_LENGTH ] ;
EXTERN BYTE	inv_fname[ NAME_LENGTH ] ;
EXTERN BYTE	invoker[ 129 ] ;
EXTERN BYTE	cmd_tail[ 129 ] ;
EXTERN BYTE	invkr_file[ 129 ] ;
EXTERN BYTE	_dta[ DTA_LEN ] ;     /* disk transfer area for dos_sfirst */

EXTERN BOOLEAN	is_out_file ;
EXTERN BOOLEAN	is_1st_word ;
EXTERN BOOLEAN	is_img_file ;
EXTERN BOOLEAN	is_gmp_file ;
EXTERN BOOLEAN	is_spool ;
EXTERN BOOLEAN	expand_tabs ;
EXTERN BOOLEAN	let_rot ;
EXTERN BOOLEAN  bkgnd_ok ;
EXTERN BOOLEAN	fonts_loaded ;
EXTERN BOOLEAN  prnt_open ;
EXTERN BOOLEAN  init_ff ;
EXTERN BOOLEAN  film_names ;
EXTERN BOOLEAN  edit_copies ;

EXTERN WORD	copies_idx ;
EXTERN WORD 	selected[ MAX_ENTRIES ] ;
EXTERN WORD	strt_name ;
EXTERN WORD	strt_col ;
EXTERN WORD	longest_name ;
EXTERN WORD	nfonts ;
EXTERN WORD	font_eps ;
EXTERN WORD	cur_font ;
EXTERN WORD	scr_hndl ;
EXTERN WORD	gem_hndl ;
EXTERN WORD	gl_wcell ;
EXTERN WORD	gl_hcell ;
EXTERN WORD	gl_wbox ;
EXTERN WORD	gl_hbox ;
EXTERN WORD	gl_baseoff ;
EXTERN WORD	contrl[ CONTRL_SIZE ] ;
EXTERN WORD	intin[ INTIN_SIZE ] ;
EXTERN WORD	ptsin[ PTSIN_SIZE ] ;
EXTERN WORD	intout[ INTOUT_SIZE ] ;
EXTERN WORD	ptsout[ PTSOUT_SIZE ] ;
EXTERN WORD	gl_rmsg[ 8 ] ;
EXTERN WORD	gl_smsg[ 8 ] ;
EXTERN WORD	tmhi ;
EXTERN WORD	tmlo ;
EXTERN WORD	flags ;
EXTERN WORD	w_handle ;
EXTERN WORD	this_pgnum ;		/* current page number being printed*/
EXTERN WORD	s_slot ;
EXTERN WORD	e_slot ;
EXTERN WORD	next_slot ;
EXTERN WORD	num_empty ;
EXTERN WORD	num_slct ;
EXTERN WORD	modified ;
EXTERN WORD	gl_ob ;
EXTERN WORD	gl_xob ;
EXTERN WORD	gl_yob ;
EXTERN WORD	gl_wob ;
EXTERN WORD	gl_hob ;
EXTERN WORD	b_indx ;	/* index into current buffer */
EXTERN WORD	b_length ;	/* length in bytes of current buffer */

EXTERN WORD	nn ;		/* length of gem file in printer line widths */
EXTERN WORD	hdr_size ;
EXTERN WORD	f_open ;
EXTERN WORD	f_handle ;
EXTERN WORD	*buf_pos ;
EXTERN WORD	*buf_end ;
EXTERN WORD	buf_lgth ;
EXTERN WORD	fst_buf ;
EXTERN WORD	file_type ;
EXTERN WORD	grf_mode ;	/* Flag for screen or alpha mode*/
EXTERN WORD	make_fit ;	/* Best fit flag for printer or plotter*/
EXTERN WORD	horz_just ;	/* Horizontal justification */
EXTERN WORD	vert_just ;	/* Vertical justification	*/
EXTERN WORD	cur_hndl ;	/* Handle for selected device*/
EXTERN WORD	nxpixel ;	/* Number of x pixels across physical page*/
EXTERN WORD	nypixel ;	/* Number of y pixels across physical page*/
EXTERN WORD	x_size ;	/* Number of x pixels across virtual page*/
EXTERN WORD	y_size ;	/* Number of y pixels across virtual page*/
EXTERN WORD	dvc_nxpixel ;	/* Number of x pixels across device */
EXTERN WORD	dvc_nypixel ;	/* Number of y pixels across device */
EXTERN WORD	dvc_wpixel ;	/* pixel width after rotation */
EXTERN WORD	dvc_hpixel ;	/* pixel height after rotation */
EXTERN WORD	char_width ;
EXTERN WORD	char_hgt ;
EXTERN WORD	cell_width ;
EXTERN WORD	cell_hgt ;
EXTERN WORD	org_height ;
EXTERN WORD	bf_height ;
EXTERN WORD	page_width ;	/* Page size in tenths of mm */
EXTERN WORD	page_height ;
EXTERN WORD	mf_xtrans ;	/* transformation parameters */
EXTERN WORD	mf_ytrans ;
EXTERN WORD	mf_xlength ;
EXTERN WORD	mf_ylength ;
EXTERN WORD	rotate ;		/* Flag indicating picture rotation */
EXTERN WORD	nxoffset ;	/* justification offsets	*/
EXTERN WORD	nyoffset ;
EXTERN WORD	dvc_nxoff ;	/* justification offsets in device units */
EXTERN WORD	dvc_nyoff ;
EXTERN WORD	brk_out ;	/* flag to breakout of display loop */
EXTERN WORD	menu_width ;

EXTERN WORD	p_col1[ 10 ] ;  /* the main color in primary colors */
EXTERN WORD	p_col2[ 10 ] ;  /* the other color in primary colors */
EXTERN WORD	s_col1[ 10 ] ;  /* the main color in secondary colors */
EXTERN WORD	s_col2[ 10 ] ;  /* the other color in secondary colors */
EXTERN WORD	n_col[ 10 ] ;  /* the main color in neutral colors */

EXTERN WORD	font_ids[ MX_FONTS ] ;	/* font ids for screen */

EXTERN LONG	ad_break ;
EXTERN LONG	tray_tr ;
EXTERN LONG	dev_tree ;
EXTERN LONG	gl_menu ;
EXTERN LONG	pcnt_tree ;	/* Address of print count tree	*/
EXTERN LONG	cntl_tr ;
EXTERN LONG	sav_byte ;	/* current byte number in file */
EXTERN LONG	fr_str[ SINFNAME - SGEMEXT + 1 ] ;
EXTERN RECTANGLE page_wdw ;
EXTERN RECTANGLE out_extent ;

EXTERN BUFFR 	buffer ;

EXTERN OPTIONS	cur_opts ;
EXTERN UWORD	key_map[ LASTITEM - FRSTITEM + 1 ] ; 
EXTERN GRECT	gl_desk ;
EXTERN GRECT	gl_win ;
EXTERN GRECT	gl_work ;

EXTERN USERBLK	gl_udtray ;
EXTERN PARMBLK	parm_tr ;

EXTERN OBJECT 	desk[ 1 ] ; 
EXTERN ICN_SPC	icon[ 50 ] ;

EXTERN DVC_STRUCT dvc[ MX_DVCS ] ;

EXTERN WKSTN	dev ;

EXTERN SDPREFS  sd_defaults ;
			      
EXTERN PDPREFS	pd_defaults ;

EXTERN CDPREFS	cd_defaults ;

EXTERN LONG	brk_addr ;		/* util86.asm */
EXTERN LONG	drawaddr ;		/* util86.asm */

EXTERN GM_GLOB	global ;		/* somewhere in the bowels of the AES */

EXTERN BOOLEAN  FreeGEM;
/* end of o_var.h */
