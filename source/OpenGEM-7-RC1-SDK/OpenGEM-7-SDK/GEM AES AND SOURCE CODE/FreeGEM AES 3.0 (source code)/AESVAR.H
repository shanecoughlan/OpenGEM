
#ifdef VARS_HERE
	#define EXT GLOBAL
#else
	#define EXT EXTERN
#endif


#if DEBUG
EXTERN WORD TRACEFLAG;
#endif

EXT LPVOID   ad_sysglo;
EXT LPMFORM  ad_mouse;
EXT LPBYTE	ad_scmd;
EXT LPVOID   ad_dta;
EXT LPBYTE   ad_path;
EXT LPBYTE	ad_stail;
EXT LPBYTE	ad_ssave;
EXT LPBYTE	ad_scrap;
EXT LPSPB	ad_windspb;
EXT LPBYTE	ad_tmp1;
EXT BYTE		gl_tmp1[LEN_FSNAME];
EXT LPBYTE	ad_tmp2;
EXT BYTE		gl_tmp2[LEN_FSNAME];
EXT LPVOID	ad_fsdta;
EXT WORD		hdr_buff[HDR_LENGTH/2];
EXT LPUWORD  ad_hdrbuff;

EXT LPBYTE	ad_g2loc;

EXT LPBYTE	ad_tmpstr;
EXT LPBYTE	ad_rawstr;
EXT LPBYTE	ad_fmtstr;
EXT LPBYTE	ad_valstr;
EXT LPTEDI	ad_edblk;
EXT LPBBLK	ad_bi;
EXT LPICON	ad_ib;

EXT LPTREE	ad_fstree;
EXT LPBYTE	ad_pfile;
EXT LPBYTE	ad_scdir;
EXT LPBYTE   ad_g1loc;
EXT LPBYTE	desk_acc[NUM_DESKACC];

EXT LPMFORM	ad_armice;
EXT LPMFORM	ad_hgmice;
EXT LPBYTE 	ad_envrn;		/* initialized in GEMSTART	*/
EXT LPTREE	ad_stdesk;
EXT LPTREE	gl_awind;
EXT LPFPD   gl_rbuf;
EXT LPTREE	menu_tree[NUM_PDS];
EXT LPTREE	gl_mntree;

EXT WORD	gl_xclip;
EXT WORD	gl_yclip;
EXT WORD	gl_wclip;
EXT WORD	gl_hclip;
EXT WORD	gl_width;
EXT WORD	gl_wchar;
EXT WORD	gl_hchar;
EXT WORD	gl_wbox;
EXT WORD	gl_hbox;
EXT WORD	gl_handle;
EXT WORD	gl_nplanes;

EXT PD	*gl_mnppd;


EXT GRECT	gl_rfull;

EXTERN THEGLO	D;

EXT ULONG	gl_bvdisk;
EXT ULONG	gl_bvhard;
EXT WORD	gl_dspcnt;
EXT WORD	gl_mnpds[NUM_PDS];
EXT WORD	gl_mnclick;
EXT WORD	gl_dacnt;
EXT WORD	gl_dabox;
EXT LPTREE	gl_datree;

EXT LPWORD	ad_intin;
EXT WORD	intin[128];
EXT UBYTE FAR *ad_nils;
EXT LPBYTE	ad_fsnames;

EXT LPVOID	ad_s1fcb;
EXT LPVOID	ad_s2fcb;

EXT BOOLEAN gl_domono;
EXT CLRCAT cc_s[]; 
EXT OBJECT	W_ACTIVE[];

EXT WORD gl_fmactive;
EXT BOOLEAN root_on_top;
EXT PD		*ctl_pd;	/* Menu manager pd */

EXT LONG    gl_boxtick;	/* Value to slow down graf_*box() animations */

EXTERN LONG NUM_TICK;	/* Incremented by the tick interrupt */

/* Raw materials for the object tree that draws windows */

EXTERN WORD	gl_watype[NUM_ELEM];
EXTERN WORD gl_waflag[NUM_ELEM];
EXTERN LONG	gl_waspec[NUM_ELEM];
EXTERN WORD gl_waflag3d[NUM_ELEM];
EXTERN LONG	gl_waspec3d[NUM_ELEM];
EXTERN WORD gl_waflag2d[NUM_ELEM];
EXTERN LONG	gl_waspec2d[NUM_ELEM];

/* The W_SIZER element's spec is changed dynamically. These variables 
 * keep a copy of the original values */

EXT LONG sizer_spec;
EXT WORD sizer_flag;

/* These "UI" trees hold the graphics for GEM5-style checkboxes & radio 
 * buttons */

EXT LPTREE gl_uimono, gl_uicolour;

/* The name of the shell */

EXT BYTE shname_buf[16];

/* System foreground colour & foreground transparency (for titlebars) */

EXT WORD sys_fg, sys_tr, syn_fg, syn_tr, syi_fg, syi_tr;

/* Miscellaneous GUI options */

EXT GUI_OPTS gl_opts;

/* EMS variables */

EXT	BYTE	gl_ems_avail;
EXT	BYTE	gl_emm_vers;
EXT	WORD	gl_emm_seg;
EXT	WORD	gl_emm_ppgs;
EXT	WORD	gl_emm_lpgs;
EXT WORD	gl_emm_inuse;
EXT WORD	gl_emm_handle;

