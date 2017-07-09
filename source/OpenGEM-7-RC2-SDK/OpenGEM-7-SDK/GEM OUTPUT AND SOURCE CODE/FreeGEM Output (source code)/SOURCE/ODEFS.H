#define		DEBUG		FALSE

#define		K_RSHIFT	0x0001
#define		K_LSHIFT	0x0002

					/* masks for flag word in metafile */
#define 	IMG_FLAG	0x0001	/* mfile contains image escape cmd */

#define		GEM_FILE 	1
#define		TXT_FILE 	2
#define		OUT_FILE 	3
#define		IMG_FILE 	4
#define		GMP_FILE 	5
#define		SYS_FILE	6

#define		PRT_MSG 	200
#define		STRT_MSG 	201
#define		QUIT_MSG 	202

#define		EDITMENU	300
#define		PANEL_OFF	301
#define		FRSTITEM	STRTITEM
#define		LASTITEM	COLRITEM
#define		FIRST_MENU	SPRFMENU
#define 	LAST_MENU	CPRFMENU
#define		FIRST_ITEM	SHRTITM0	/* first item in pref menus */
#define		MX_DVCS		(DEV20ICN-DEV01ICN+1) 

#define		OK 		1
#define		CANCEL 		0
#define 	ERROR            1

#define		MO_ARROW	0 
#define		MO_HGLASS	2
#define		MO_FTHAND	4
#define		MOUSE_OFF 	256
#define		MOUSE_ON 	257

#define		MAX_ENTRIES	36
#define		INDX_SZ		5

#define		SCRN_TYPE	1		/* 0000 0001 */
#define		PLOT_TYPE	2		/* 0000 0010 */
#define		PRNT_TYPE	4		/* 0000 0100 */
#define		CMRA_TYPE	16		/* 0001 0000 */

#define		PHYSICAL_PAGE	0
#define		META_WINDOW	1

#define		TOPLEFT		0
#define		CENTERED	1
#define		BOTTOMRIGHT	2
#define		PORTRAIT	1
#define		LANDSCAPE  	2
#define		TRUE_SZ		0 

#define		MAX_COLORS	16
#define		CONTRL_SIZE	16
#define		INTIN_SIZE	132
#define		PTSIN_SIZE	256
#define		INTOUT_SIZE	128
#define		PTSOUT_SIZE	128
#define		DTA_LEN		128
#define		MAX_INT		32767
#define		CM_PER_INx100	254

#define		FILEPORT	0x0000
#define		DEFAPORT	0x00FF		/* use whatever port is set */
#define		CUSTOMSZ	0xFF00
#define		MULT_CPYS	0x0001	      /* mask for printer cap. flag */

#define		GEM_EXT		fr_str[0] 
#define		INF_EXT		fr_str[1] 
#define		LIS_EXT		fr_str[2] 
#define		OUT_EXT		fr_str[3] 
#define		GMP_EXT		fr_str[4] 
#define		IMG_EXT		fr_str[5] 
#define		UNTITLED	fr_str[6] 
#define		INFNAME		fr_str[7] 
#define		APPNAME		fr_str[8] 
#define		PFILE_NAME	fr_str[9]

		/*	033 = octal for	27d, escape character */
#define		GEM_CMD		"\033\033GEM"
#define		IMG_CMD		"\033\033IMG"

#undef		EOF		/* defined in portab.h as (-1) */
#undef		NULL		/* defined in portab.h as 0 */

#define		NULL		0x00	
#define		HTAB		0x09
#define		LF		0x0a
#define		FF		0x0c
#define		CR		0x0d
#define		EOF 		0x1a
#define		SP		0x20
#define		BEG_PAREN	0x28
#define		END_PAREN	0x29
#define		ASTERISK	0x2a
#define		COMMA		0x2c
#define		PERIOD		0x2e
#define		SLASH		0x2f
#define		ZERO		0x30
#define		NINE		0x39
#define		COLON		0x3a
#define		SEMI_COLON	0x3b
#define		UCASE_A		'A'
#define		UCASE_X		0x58
#define		UCASE_Z		0x5a
#define		BSLASH		0x5c
#define		LCASE_X		0x78
#define		TILDE		0x7e
  
/* IBM keyboard character codes */
#define		ESCAPE		0x011B
#define		CRETURN		0x1c0d
#define		SPACE		0x3920
#define		BSPACE		0x0e08
#define		TAB		0x0f09
#define		BTAB		0x0f00
#define		PGUP		0x4900
#define		PGDN		0x5100
#define		UARROW		0x4800
#define		LARROW		0x4b00
#define		RARROW		0x4d00
#define		DARROW		0x5000

#define		BUF_SIZE	64

#define		FNAME_LENGTH	70 
#define		PATH_LENGTH	67
#define		NAME_LENGTH	13

#define 	MX_FONTS	20
#define 	MX_FSIZES	20
#define		FONT_EPS	1	/* tolerance for display to screen */

#define 	OPCODE           contrl[0]
#define 	NUM_PTSIN        contrl[1]
#define 	NUM_PTSOUT       contrl[2]
#define 	NUM_INTIN        contrl[3]
#define 	NUM_INTOUT       contrl[4]
#define 	SUBCODE          contrl[5]
#define 	DEV_HNDL	 contrl[6]

#define		WORK_X		(gl_work.g_x + menu_width)
#define		WORK_W		(gl_work.g_w - menu_width)

#define		OB_PTR(a,b)	a+b*sizeof(OBJECT)

typedef struct color_bundle {
	WORD red ;
	WORD green ;
	WORD blue ;
	WORD bas_col ;
	WORD lgtns ;
} CLBNDL ;

typedef struct	_sdprefs
{
    	BOOLEAN		wait_for_key ;		
	BOOLEAN 	cycle_until_esc ;
	WORD 		time ; /* time to pause between screens, in seconds */
} SDPREFS ;

typedef struct	_pdprefs
{
    	WORD 	horzjust ;
	WORD 	vertjust ;
	WORD 	scale ;
	BOOLEAN final_ff ;
	BOOLEAN bkgrnd ;
	BOOLEAN	delete ;
	BOOLEAN pause ;
	WORD	port ;
	BOOLEAN	to_file ;	
	WORD	paper_units ;
	WORD	paper_item ;
	WORD	paper_width ;
	WORD	paper_height  ;
	WORD	paper_tray ;
	BYTE	file[ FNAME_LENGTH ] ;
} PDPREFS ;

typedef struct	_cdprefs
{
	WORD 	film;
	WORD 	lgt;
	CLBNDL 	cmclr[ MAX_COLORS ] ;
} CDPREFS ;

typedef union _dvc_union
{
    SDPREFS	sd ;
    PDPREFS	pd ;
    CDPREFS	cd ;
} DVC_PREFS ;

typedef struct dvc_strct
{
    BYTE	id[ 13 ] ;	/* 12 chars plus null */
    WORD	num ;
    WORD	type ;
    DVC_PREFS	prefs ;
} DVC_STRUCT ;

typedef struct icn_spec
{
    WORD	tree1 ;
    WORD	obj1 ;
    WORD	tree2 ;
    WORD	obj2 ;
} ICN_SPC ;

typedef struct mfdb 
{
    FBYTE	*mp;
    WORD	fwp;
    WORD	fh; 
    WORD	fww; 
    WORD	ff;
    WORD	np;
    WORD	r1; 
    WORD	r2;
    WORD	r3;
} MFDB;

typedef struct btfl {
	BYTE head[ 10 ] ;
	WORD num ;
	BYTE names[ MAX_ENTRIES ][ FNAME_LENGTH ] ;
} BTCHFL ;

typedef struct opts {
	BYTE 	version[ 20 ] ;
	WORD 	cur_dvc ;
	WORD 	numcopies ;
	WORD	start_pgnum ;		/* page number to begin printing on */
	WORD	end_pgnum ;		/* page number to end printing on */
	WORD 	confirm ;
	WORD 	auto_disp ;
	BYTE 	gsx_wc[ FNAME_LENGTH ] ;
	GRECT	window ;
} OPTIONS ;

typedef struct rtangle {
	WORD	left ;
	WORD	top ;
	WORD	right ;
	WORD 	bottom ;
} RECTANGLE ;

typedef struct m_scratch {
	OPTIONS	opts ;					/* 352 bytes */
	BYTE 	s1[ FNAME_LENGTH ] ;			/* 70  bytes */
	BYTE 	s2[ FNAME_LENGTH ] ;			/* 70  bytes */
	BYTE 	s3[ FNAME_LENGTH ] ;			/* 70  bytes */
	BYTE 	s4[ FNAME_LENGTH ] ;			/* 70  bytes */
} MISC_SCRATCH ;					/* 632 bytes total */

typedef union mfbuffer {
	WORD	w[ BUF_SIZE ] ;		
 	BYTE	b[ 2*BUF_SIZE ] ;
} MFBUF ;						/* 128 bytes total */

typedef struct d_scratch {
	MFBUF	mfbuf ;					/* 128 bytes */
	WORD	avl_pt[ MX_FONTS ][ MX_FSIZES ] ;	/* 800 bytes */
	WORD	avl_px[ MX_FONTS ][ MX_FSIZES ] ;	/* 800 bytes */
	BYTE 	s1[ FNAME_LENGTH ] ;			/* 70 bytes */
	BYTE 	s2[ FNAME_LENGTH ] ;			/* 70 bytes */
	BYTE 	s3[ FNAME_LENGTH ] ;			/* 70 bytes */
} DSPLY_SCRATCH ;					/* 1938 bytes total */

typedef union bffr {
	BTCHFL		btch ;				/* 2532 bytes */
	DSPLY_SCRATCH	d ;				/* 498 bytes */
	MISC_SCRATCH	m ;				/* 632 bytes */
} BUFFR ;

typedef struct wsstr
{
	WORD		ws_xres;
	WORD		ws_yres;
	WORD		ws_noscale;
	WORD		ws_wpixel;
	WORD		ws_hpixel;
	WORD		ws_ncheights;
	WORD		ws_nlntypes;
	WORD		ws_nlnwidths;
	WORD		ws_nmktypes;
	WORD		ws_nmksizes;
	WORD		ws_nfaces;
	WORD		ws_npatts;
	WORD		ws_nhatchs;
	WORD		ws_ncolors;
	WORD		ws_ngdps;
	WORD		ws_supgdps[10];
	WORD		ws_attgdps[10];
	WORD		ws_color;
	WORD		ws_rotate;
	WORD		ws_fill;
	WORD		ws_cell;
	WORD		ws_npals;
	WORD		ws_nloc;
	WORD		ws_nval;
	WORD		ws_nchoice;
	WORD		ws_nstring;
	WORD		ws_type;
	WORD		ws_minwch;
	WORD		ws_minhch;
	WORD		ws_maxwch;	
	WORD		ws_maxhch;
	WORD		ws_lnminw;
	WORD		ws_pts5;
	WORD		ws_lnmaxw;
	WORD		ws_pts7;
	WORD		ws_pts8;
	WORD		ws_mkminw;
	WORD		ws_pts10;
	WORD		ws_mkmaxw;
} WS;

typedef struct exstr
{
/* [0] */	WORD		ws_scrntype;
/* [1] */	WORD		ws_bkcolr;
/* [2] */	WORD		ws_supeff;
/* [3] */	WORD		ws_scraster;
/* [4] */	WORD		ws_nplanes;
/* [5] */	WORD		ws_ctable;
/* [6] */	WORD		ws_perform;
/* [7] */	WORD		ws_fcontour;
/* [8] */	WORD		ws_rotchar;
/* [9] */	WORD		ws_numwmodes;
/* [10] */	WORD		ws_inpmode;
/* [11] */	WORD		ws_talign;
/* [12] */	WORD		ws_ink;
/* [13] */	WORD		ws_rubber;
/* [14] */	WORD		ws_maxptsin;
/* [15] */	WORD		ws_maxintin;
/* [16] */	WORD		ws_mousbtns;
/* [17] */	WORD		ws_stywide;
/* [18] */	WORD		ws_modwide;
/* [19] */	WORD		ws_clip;
/* [20] */	WORD		ws_dotunits;
/* [21] */	WORD		ws_xdotsz ;
/* [22] */	WORD		ws_ydotsz ;
/* [23] */	WORD		ws_xdpi;
/* [24] */	WORD		ws_ydpi;
/* [25] */	WORD		ws_imgrot;
/* [26] */	WORD		ws_xqsize ;	/*size of quarter screen buf*/
/* [27]	*/	WORD		ws_yqsize ;
/* [28] */	WORD		ws_flags ;	/* misc capability flags */
/* [29->44] */	WORD		ws_1reserved[44-29+1];
/* [45] */	WORD		ws_ulxclip;
/* [46] */	WORD		ws_ulyclip;
/* [47] */	WORD		ws_lrxclip;
/* [48] */	WORD		ws_lryclip;
/* [49->56] */	WORD		ws_2reserved[56-49+1];
} EXTND ;

typedef struct wk_stn
{
	WORD		xres ;
	WORD		yres ;
	WORD		wpixel ;
	WORD		hpixel ;
	LONG		xlen ;
	LONG		ylen ;
	WORD		xdpi ;
	WORD		ydpi ;
	WORD		nplanes ;
	WORD		maxptsin ;
	WORD		maxintin ;
	WORD		img_rot ;
	WORD		minwch ;
	WORD		minhch ;
	WORD		maxwch ;
	WORD		maxhch ;
	WORD		flags ;
} WKSTN ;

#define		CUR_FNAME	d.s3
#define		AVL_POINT	d.avl_pt
#define		AVL_PIXEL	d.avl_px
#define		MF		d.mfbuf.w
#define		TXT		d.mfbuf.b
#define		MF_MRKR		d.mfbuf.w[0]
#define		HDR_SIZE 	d.mfbuf.w[1]
#define		MF_VERS		d.mfbuf.w[2]
#define		TRNSFM_KIND	d.mfbuf.w[3]
#define		XMIN		d.mfbuf.w[4]
#define		YMIN		d.mfbuf.w[5]
#define		XMAX		d.mfbuf.w[6]
#define		YMAX		d.mfbuf.w[7]
#define		MF_WIDTH	d.mfbuf.w[8]
#define		MF_HEIGHT	d.mfbuf.w[9]
#define		LL_X		d.mfbuf.w[10]
#define		LL_Y		d.mfbuf.w[11]
#define		UR_X		d.mfbuf.w[12]
#define		UR_Y		d.mfbuf.w[13]
#define		MF_KIND		d.mfbuf.w[14]

#define		SCRN_PREFS	dvc[cur_opts.cur_dvc].prefs.sd
#define		PRNT_PREFS	dvc[cur_opts.cur_dvc].prefs.pd
#define		PLOT_PREFS	dvc[cur_opts.cur_dvc].prefs.pd
#define		CMRA_PREFS	dvc[cur_opts.cur_dvc].prefs.cd
#define		CUR_DEV		dvc[cur_opts.cur_dvc]	
#define		DEV_TYPE	dvc[cur_opts.cur_dvc].type	
#define		DEV_ID		dvc[cur_opts.cur_dvc].id
#define		DEV_NUM		dvc[cur_opts.cur_dvc].num


void ShowHelp(WORD);

/* end of odefs.h */

