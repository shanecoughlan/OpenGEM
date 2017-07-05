/* GLOBAL declarations of variables for GEM OUTPUT.	*/
/* Susan Bancroft 		5/6/85			*/


GLOBAL BYTE	btch_hdr[ 10 ] = "!!!!Batch" ;
GLOBAL BYTE 	names[ MAX_ENTRIES ][ FNAME_LENGTH ] ;
GLOBAL BYTE	cur_file[ FNAME_LENGTH ] ;
GLOBAL BYTE	list_name[ FNAME_LENGTH ] ;
GLOBAL BYTE	lis_dir[ PATH_LENGTH ] ;
GLOBAL BYTE	opt_dir[ PATH_LENGTH ] ;
GLOBAL BYTE	lis_wc[ PATH_LENGTH ] ;
GLOBAL BYTE	gsx_wc[ PATH_LENGTH ] ;
GLOBAL BYTE	inv_fname[ NAME_LENGTH ] ;
GLOBAL BYTE	invoker[ 129 ] ;
GLOBAL BYTE	cmd_tail[ 129 ] ;
GLOBAL BYTE	invkr_file[ 129 ] ;
GLOBAL BYTE	_dta[ DTA_LEN ] ;	/* disk transfer area for dos_sfirst */

GLOBAL BOOLEAN	is_out_file = FALSE ;
GLOBAL BOOLEAN	is_1st_word = FALSE ;
GLOBAL BOOLEAN	is_img_file = FALSE ;
GLOBAL BOOLEAN	is_gmp_file = FALSE ;
GLOBAL BOOLEAN  is_spool = FALSE ;
GLOBAL BOOLEAN  expand_tabs = FALSE ;
GLOBAL BOOLEAN  bkgnd_ok ;
GLOBAL BOOLEAN	fonts_loaded ;
GLOBAL BOOLEAN	let_rot ;
GLOBAL BOOLEAN  prnt_open ;
GLOBAL BOOLEAN  init_ff ;
GLOBAL BOOLEAN  film_names = TRUE ;
GLOBAL BOOLEAN  edit_copies = FALSE ;

GLOBAL WORD	copies_idx = 0 ;	
GLOBAL WORD 	selected[ MAX_ENTRIES ] ; /* state flags for files in list */
GLOBAL WORD	strt_name ;		/* number of name at top of list */
GLOBAL WORD	strt_col ;		/* column number to beg. file names */
GLOBAL WORD	longest_name ;		/* length in columns of longest name*/
GLOBAL WORD	nfonts ;		/* number of fonts avail on device */
GLOBAL WORD	font_eps ;		/* tolerance delta for screens */
GLOBAL WORD	cur_font ;		/* current attribute number */
GLOBAL WORD	scr_hndl ;		/* workstn handle for screen */
GLOBAL WORD	gem_hndl ;		/* workstn handle for AES */
GLOBAL WORD	gl_wcell ;		/* width of system character */
GLOBAL WORD	gl_hcell ;		/* same, returned in graf_handle() */
GLOBAL WORD	gl_wbox ;		/* width of character 'box' */	
GLOBAL WORD	gl_hbox ;		/* height of same */
GLOBAL WORD	gl_baseoff ;		/* distance from top to baseline */
GLOBAL WORD	contrl[ CONTRL_SIZE ] ;	/* VDI arrays */
GLOBAL WORD	intin[ INTIN_SIZE ] ;
GLOBAL WORD	ptsin[ PTSIN_SIZE ] ;
GLOBAL WORD	intout[ INTOUT_SIZE ] ;
GLOBAL WORD	ptsout[ PTSOUT_SIZE ] ;
GLOBAL WORD	gl_rmsg[ 8 ] ;		/* return message area for AES */
GLOBAL WORD	gl_smsg[ 8 ] ;		/* 'send' message area for AES */
GLOBAL WORD	tmhi ;			/* high word of timer value */
GLOBAL WORD	tmlo ;			/* same, used in evnt_mult() */
GLOBAL WORD	flags ;			/* flags used in evnt_multi() */
GLOBAL WORD	w_handle ;		/* window handle */
GLOBAL WORD	this_pgnum ;		/* current page number being printed*/
GLOBAL WORD	s_slot ;		/* start and end slots, used */
GLOBAL WORD	e_slot ;		/* in tray handle routines. */
GLOBAL WORD	next_slot ;		/* index of next avail fname object*/
GLOBAL WORD	num_empty ;		/* number of available slots */
GLOBAL WORD	num_slct ;		/* number of selected slots */
GLOBAL WORD	modified ;		/* true iff existing list is changed*/
GLOBAL WORD	gl_ob ;			/* object number of current object */
GLOBAL WORD	gl_xob ;		/* x offset of current object 	*/
GLOBAL WORD	gl_yob ;		/* y				*/
GLOBAL WORD	gl_wob ;		/* width 			*/
GLOBAL WORD	gl_hob ;		/* height			*/
GLOBAL WORD	b_indx ;	/* index into current buffer */
GLOBAL WORD	b_length ;	/* length in bytes of current buffer */

GLOBAL WORD	nn ;		/* length of gem file in printer line widths */
GLOBAL WORD	hdr_size ;
GLOBAL WORD	f_open = FALSE ;
GLOBAL WORD	f_handle ;
GLOBAL WORD	*buf_pos ;
GLOBAL WORD	*buf_end ;
GLOBAL WORD	buf_lgth ;
GLOBAL WORD	fst_buf ;
GLOBAL WORD	file_type ;
GLOBAL WORD	grf_mode ;	/* Flag for screen or alpha mode*/
GLOBAL WORD	make_fit ;	/* Best fit flag for printer or plotter*/
GLOBAL WORD	horz_just ;	/* Horizontal justification */
GLOBAL WORD	vert_just ;	/* Vertical justification	*/

GLOBAL WORD	cur_hndl ;	/* Handle for selected device*/
GLOBAL WORD	nxpixel ;	/* Number of x pixels across physical page*/
GLOBAL WORD	nypixel ;	/* Number of y pixels across physical page*/
GLOBAL WORD	x_size ;	/* Number of x pixels across virtual page*/
GLOBAL WORD	y_size ;	/* Number of y pixels across virtual page*/
GLOBAL WORD	dvc_nxpixel ;	/* Number of x pixels across device */
GLOBAL WORD	dvc_nypixel ;	/* Number of y pixels across device */
GLOBAL WORD	dvc_wpixel ;	/* pixel width after rotation */
GLOBAL WORD	dvc_hpixel ;	/* pixel height after rotation */
GLOBAL WORD	char_width ;
GLOBAL WORD	char_hgt ;
GLOBAL WORD	cell_width ;
GLOBAL WORD	cell_hgt ;
GLOBAL WORD	org_height ;
GLOBAL WORD	bf_height ;
GLOBAL WORD	page_width ;	/* Page size in tenths of mm */
GLOBAL WORD	page_height ;
GLOBAL WORD	mf_xtrans ;	/* transformation parameters */
GLOBAL WORD	mf_ytrans ;
GLOBAL WORD	mf_xlength ;
GLOBAL WORD	mf_ylength ;
GLOBAL WORD	rotate ;		/* Flag indicating picture rotation */
GLOBAL WORD	nxoffset ;	/* justification offsets	*/
GLOBAL WORD	nyoffset ;
GLOBAL WORD	dvc_nxoff ;	/* justification offsets in device units */
GLOBAL WORD	dvc_nyoff ;
GLOBAL WORD	brk_out ;	/* flag to break out of display loop */
GLOBAL WORD	menu_width ;	/* width of CNTLTREE	*/

GLOBAL WORD	p_col1[ 10 ] =  /* the main color in primary colors */
			{ 100, 100, 100, 100, 100, 100, 100,  78,  64,  51 } ;
GLOBAL WORD	p_col2[ 10 ] =  /* the other colors in primary colors */
			{  85, 71,  50,  31,  19,  12,   0,   0,   0,   0  } ;
GLOBAL WORD	s_col1[ 10 ] =  /* the main colors in secondary colors */
			{ 100, 100, 100, 100, 100, 100,  78,  64,  51,  25 } ;
GLOBAL WORD	s_col2[ 10 ] =  /* the other color in secondary colors */
			{  71,  44,  31,  19,  12,   0,   0,   0,   0,   0 } ;
GLOBAL WORD	n_col[ 10 ] =  /* the main color in neutral colors */
			{  85,  71,  64,  57,  51,  44,  37,  31,  25,  12 } ;
			
GLOBAL WORD	font_ids[ MX_FONTS ] ;	/* font ids for screen */

GLOBAL LONG	ad_break ;
GLOBAL LONG	tray_tr ;	/* address of tray tree. (contains strings) */
GLOBAL LONG	dev_tree ;	/* address of device icon tree */
GLOBAL LONG	gl_menu ;	/* address of menu tree */
GLOBAL LONG	pcnt_tree ;	/* Address of print count tree	*/
GLOBAL LONG	cntl_tr ;	/* address of control side panel */
GLOBAL LONG	sav_byte ;	/* current byte number in file */
GLOBAL LONG	fr_str[ SINFNAME - SGEMEXT + 1 ] ;

GLOBAL RECTANGLE page_wdw ;
GLOBAL RECTANGLE out_extent ;

GLOBAL BUFFR 	buffer ;

GLOBAL OPTIONS	cur_opts = { "1234567890123456789", /* 20 bytes for id string */
		     	     21,	/* def_dvc = first printer */
			     1 ,	/* num copies == 1 */
			     1 ,	/* start page number */
			     999,	/* end page number */
			     TRUE,	/* confrim .LIS changes == TRUE */
			     FALSE,	/* auto_disp */
			     0,0,0,0,0,0,0,0,0,0,	/* 70 nulls for */
			     0,0,0,0,0,0,0,0,0,0,	/* default gsx_wc */
			     0,0,0,0,0,0,0,0,0,0,
			     0,0,0,0,0,0,0,0,0,0,
			     0,0,0,0,0,0,0,0,0,0,
			     0,0,0,0,0,0,0,0,0,0,
			     0,0,0,0,0,0,0,0,0,0,
			     0, 0, 0, 0			/* def. window */
			     };

GLOBAL UWORD	key_map[ LASTITEM - FRSTITEM + 1 ] = 
{
/* STRTITEM @S */	0x1f00,		/* ADDITEM  @A */	0x1e00,
/* DELEITEM @D */	0x2000,		/* DUPLITEM @C */	0x2e00,
/* ------------*/	0x0000,		/* ------------*/	0x0000,
/* ------------*/	0x0000,		/* ------------*/	0x0000,
/* NEWITEM  ^W */	0x1117,
/* OPENITEM ^O */	0x180f,		/* ----------- */	0x0000,
/* SAVEITEM ^V */	0x2f16,		/* SAVAITEM ^M */	0x320d,
/* ----------- */	0x0000,		/* TAPPITEM ^R */	0x1312,
/* QUITITEM ^Q */	0x1011,		/* ----------- */	0x0000,
/* SHRTITM0 F1 */	0x3b00,
/* ------------*/	0x0000,
/* GLOBITM0 F2 */	0x3c00,
/* ------------*/	0x0000,
/* SVPFITM0 @V */	0x2f00,
/* ------------*/	0x0000,
/* SCRNITEM F3 */	0x3d00,
/* ------------*/	0x0000,
/* PAGEITEM F4 */	0x3e00,
/* PGSIZITM F5 */	0x3f00,
/* PTRAYITM F6 */	0x4000,
/* FILERDIR F7 */	0x4100,
/* ------------*/	0x0000,
/* CMFMITEM F8 */	0x4200,
/* COLRITEM F9 */	0x4300
}; 

GLOBAL GRECT	gl_desk ;
GLOBAL GRECT	gl_win ;
GLOBAL GRECT	gl_work ;

GLOBAL USERBLK	gl_udtray ;
GLOBAL PARMBLK	parm_tr ;

GLOBAL OBJECT 	desk[ 1 ] = 
		{
		-1, -1, -1, G_BOX, NONE, NORMAL, 0x00001144L, 0, 0, 0, 0
		} ;

GLOBAL ICN_SPC	icon[ 60 ] =
{		/* original icon */    /* replacement icon */
/* 00 */	DEVCTREE, DEV01ICN,    0, 0,
/* 01 */	DEVCTREE, DEV01ICN+1,  0, 0,
/* 02 */	DEVCTREE, DEV01ICN+2,  0, 0,
/* 03 */	DEVCTREE, DEV01ICN+3,  0, 0,
/* 04 */	DEVCTREE, DEV01ICN+4,  0, 0,
/* 05 */	DEVCTREE, DEV01ICN+5,  0, 0,
/* 06 */	DEVCTREE, DEV01ICN+6,  0, 0,
/* 07 */	DEVCTREE, DEV01ICN+7,  0, 0,
/* 08 */	DEVCTREE, DEV01ICN+8,  0, 0,
/* 09 */	DEVCTREE, DEV01ICN+9,  0, 0,
/* 10 */	DEVCTREE, DEV01ICN+10, 0, 0,
/* 11 */	DEVCTREE, DEV01ICN+11, 0, 0,
/* 12 */	DEVCTREE, DEV01ICN+12, 0, 0,
/* 13 */	DEVCTREE, DEV01ICN+13, 0, 0,
/* 14 */	DEVCTREE, DEV01ICN+14, 0, 0,
/* 15 */	DEVCTREE, DEV01ICN+15, 0, 0,
/* 16 */	DEVCTREE, DEV01ICN+16, 0, 0,
/* 17 */	DEVCTREE, DEV01ICN+17, 0, 0,
/* 18 */	DEVCTREE, DEV01ICN+18, 0, 0,
/* 19 */	DEVCTREE, DEV01ICN+19, 0, 0,
/* 20 */	OUTINFO,  LOGOICON,    HRESICNS, HLOGOICN,
/* 21 */	OUTINFO,  GEMICON,     HRESICNS, HGEMICON,	
/* 22 */	OUTINFO,  OUT1ICON,    HRESICNS, HOUT1ICN,
/* 23 */	OUTINFO,  OUT2ICON,    HRESICNS, HOUT2ICN, 	
/* 24 */	SCRNPREF, SCRNICN2,    HRESICNS, HSCRNICN, 
/* 25 */	PRNTPREF, PRNTICN2,    HRESICNS, HPRNTICN,
/* 26 */	PRTCNT,   ESCICON,     HRESICNS, HESCICON,
/* 27 */	FILMPREF, CMRAICN2,    HRESICNS, HCMRAICN,
/* 28 */	COLRPREF, CMRAICN3,    HRESICNS, HCMRAICN,
/* 29 */	SCRNPREF, ESCICON2,    HRESICNS, HESCICON,
/* 30 */	CNTLTREE, STRTICON,    HRESICNS, HSTRTICN,
/* 31 */	CNTLTREE, COPYICON,    HRESICNS, HCOPYICN,
/* 32 */	SHRTCUT,  ALTICON1,    HRESICNS, HALTICON,
/* 33 */	SHRTCUT,  KEYAICON,    HRESICNS, HKEYAICN,
/* 34 */	SHRTCUT,  ALTICON2,    HRESICNS, HALTICON,
/* 35 */	SHRTCUT,  KEYCICON,    HRESICNS, HKEYCICN,
/* 36 */	SHRTCUT,  ALTICON3,    HRESICNS, HALTICON,
/* 37 */	SHRTCUT,  KEYDICON,    HRESICNS, HKEYDICN,
/* 38 */	SHRTCUT,  ALTICON4,    HRESICNS, HALTICON,
/* 39 */	SHRTCUT,  KEYSICON,    HRESICNS, HKEYSICN,
/* 40 */	PSIZPREF, PSIZICON,    HRESICNS, HPRNTICN,
/* 41 */	TRAYPREF, PTRYICON,    HRESICNS, HPRNTICN,
/* 42 */	PORTPREF, PORTICON,    HRESICNS, HPRNTICN,
/* 43 */	0xFFFF, 0, 0, 0,
/* 44 */	0, 0, 0, 0,
/* 45 */	0, 0, 0, 0,
/* 46 */	0, 0, 0, 0,
/* 47 */	0, 0, 0, 0,
/* 48 */	0, 0, 0, 0,
/* 49 */	0, 0, 0, 0,
/* 50 */	0, 0, 0, 0,
/* 51 */	0, 0, 0, 0,
/* 52 */	0, 0, 0, 0,
/* 53 */	0, 0, 0, 0,
/* 54 */	0, 0, 0, 0,
/* 55 */	0, 0, 0, 0,
/* 56 */	0, 0, 0, 0,
/* 57 */	0, 0, 0, 0,
/* 58 */	0, 0, 0, 0,
/* 59 */	0, 0, 0, 0
} ;

GLOBAL DVC_STRUCT dvc[ MX_DVCS ] = { 0 } ;

GLOBAL WKSTN	dev ;

GLOBAL SDPREFS  sd_defaults = {	/* default preferences for screen drivers */
    				TRUE, 		/* wait_for_key */
				FALSE,		/* cycle_until_esc */
				3		/* time to pause */
			      } ;
			      
GLOBAL PDPREFS	pd_defaults = { /* defalt prefs for printers and plotters */
			    	CENTERED, 	/* horzjust */
				CENTERED, 	/* vertjust */
				TRUE_SZ,	/* scale */
				TRUE, 		/* final_ff */
				FALSE,		/* bkgrnd */
				FALSE,		/* delete */
				FALSE,		/* pause */
				0,		/* port */
				FALSE,		/* to_file */	
				INCHUNIT,	/* paper_units */
				HALFSIZE+1,	/* paper_item */
				850,		/* paper_width */
				1100,		/* paper_height  */
				DFLTTRAY,	/* paper_tray */
				0, 0, 0, 0, 0,	/* file[ FNAME_LENGTH ] */
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0 
			      } ;

GLOBAL CDPREFS	cd_defaults = {	/* default preferences for cameras */
				0, 		/* film */
				0,		/* lgt	*/
				0, 0, 0, 0, 0,	/* colors */
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0,
				0, 0, 0, 0, 0
			      } ;

EXTERN LONG	brk_addr ;		/* declared GLOBAL in util86.asm */
EXTERN LONG	drawaddr ;		/* declared GLOBAL in util86.asm */

EXTERN GM_GLOB	global ;		/* somewhere in the bowels of the AES */

/* end of o_var.h */
