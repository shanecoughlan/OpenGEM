 /*	RCSDATA.C	12/21/84 - 1/25/85  	Tim Oren		*/
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
#include "ppdrcs.h"



/* variables used in icon edit mode */
    UWORD	loword, hiword;
    GRECT	trans_area;
    MFDB	trans_mfdb, tmp_mfdb, tmp2_mfdb, fat_mfdb;  
    WORD	junk;	

GLOBAL  X_BUF_V2 gl_xbuf;
GLOBAL	LPTREE	save_tree;
GLOBAL	LPVOID	dst_mp, src_mp;
GLOBAL	LPICON	gl_icnspec;
GLOBAL  BOOLEAN icn_edited = FALSE, clipped = FALSE, paste_img = FALSE;
GLOBAL	BOOLEAN	iconedit_flag = FALSE, flags,pen_on, grid;
GLOBAL	BOOLEAN	selecton = FALSE, inverted;
GLOBAL	WORD	save_obj, save_state, gl_wimage, gl_himage, gl_datasize;
GLOBAL	GRECT	scrn_area,hold_area,fat_area,src_img,icn_img,dat_img,mas_img;
GLOBAL	GRECT	scroll_fat, gridbx;
GLOBAL	GRECT	clip_area, selec_area, flash_area, dispc_area;
GLOBAL	MFDB	undo_mfdb,und2_mfdb,hold_mfdb,hld2_mfdb,disp_mfdb,scrn_mfdb;
GLOBAL	MFDB	clip_mfdb, clp2_mfdb, save_mfdb, sav2_mfdb;
GLOBAL	LONG	ibuff_size;			 
GLOBAL	BOOLEAN	is_mask, gl_isicon ;
GLOBAL  WORD	out1, xwait, ywait, wwait, hwait,out2;
GLOBAL  WORD	mousex, mousey, bstate, kstate,kreturn, bclicks;
GLOBAL  WORD    colour,fgcolor, bgcolor, old_fc, deltax, deltay;
GLOBAL	WORD	gridw = 8, gridh = 8;

#if	MC68K

UWORD		bit_mask[16] = { 0x8000, 0x4000, 0x2000, 0x1000,
				 0x0800, 0x0400, 0x0200, 0x0100,
				 0x0080, 0x0040, 0x0020, 0x0010,
				 0x0008, 0x0004, 0x0002, 0x0001 };
#else

UWORD		bit_mask[16] = { 0x0080, 0x0040, 0x0020, 0x0010,
				 0x0008, 0x0004, 0x0002, 0x0001,
				 0x8000, 0x4000, 0x2000, 0x1000,
				 0x0800, 0x0400, 0x0200, 0x0100 };
#endif
	     

UWORD		color_map[16] = { 0x0000, 0x000f, 0x0001, 0x0002, 0x0004,
			          0x0006, 0x0003, 0x0005, 0x0007, 0x0008,
			          0x0009, 0x000a, 0x000c, 0x000e, 0x000b,
			          0x000d};
GLOBAL  WORD    invert3[8] = {BLACK,WHITE,CYAN,MAGENTA,YELLOW,RED,BLUE,GREEN};
GLOBAL  WORD	invert4[16]= {BLACK, WHITE, DCYAN, DMAGENTA, DYELLOW, DRED,
			      DBLUE, DGREEN, DBLACK, DWHITE, CYAN, MAGENTA,
			      YELLOW, RED, BLUE, GREEN};
GLOBAL	BYTE	sav_icnpath[80];
GLOBAL	BYTE	sav_rcspath[80];
GLOBAL	BYTE	rcs_rfile[80];
GLOBAL	BYTE	rcs_dfile[80];
GLOBAL  BYTE	rcs_app[80];
GLOBAL	BYTE	rcs_infile[80];
GLOBAL  BYTE    icn_file[80];
GLOBAL	FILE   *rcs_frhndl;
GLOBAL	FILE   *rcs_fdhndl;     
GLOBAL  UWORD	hard_drive;	/* what is hard drive configuration? */
GLOBAL	WORD	partp;		/* are parts shown? */
GLOBAL	WORD	toolp;		/* are tools shown? */
GLOBAL  LPTREE	ad_menu;
GLOBAL	LPTREE	ad_tools;
GLOBAL  LPTREE	ad_view;
GLOBAL	LPTREE	ad_pbx;
GLOBAL	LPTREE	ad_clip;
GLOBAL  LPTREE  ad_itool;
GLOBAL  LPTREE  ad_idisp;
GLOBAL	WORD	rcs_clipkind;
GLOBAL	WORD	rcs_view;
GLOBAL	GRECT	full;
GLOBAL	GRECT	view;
GLOBAL	GRECT	pbx;
GLOBAL	GRECT	tools;
GLOBAL  GRECT   itool;
GLOBAL  GRECT   idisp;
GLOBAL	WORD	rcs_trpan;	/* Tree row offset in window	*/
GLOBAL	WORD	rcs_xpan;	/* (Positive) offset of root	*/
GLOBAL	WORD	rcs_ypan;
GLOBAL	WORD	rcs_xtsave;	/* Tree x,y before being opened */
GLOBAL	WORD	rcs_ytsave;
GLOBAL	WORD	rcs_mform;	/* Current mouse form */
GLOBAL	WORD	rcs_hot;	/* Currently active tools */
GLOBAL	GRECT	wait;		/* Current mouse wait rectangle */
GLOBAL	WORD	wait_io;	/* Waiting for in or out? */

GLOBAL	WORD	rcs_svfstat;
GLOBAL  WORD	rcs_state = 0;
GLOBAL  WORD	icn_state = 0;
GLOBAL	BYTE	rcs_title[80];
GLOBAL	WORD	rcs_nsel = 0;
GLOBAL	WORD	rcs_sel[MAXSEL];
GLOBAL	WORD	rcs_cflag = FALSE;	/* emit .c file or not?	   */
GLOBAL	WORD	rcs_hflag = TRUE;	/* emit .h file or not?	   */
GLOBAL	WORD	rcs_oflag = FALSE;	/* emit .o file or not?	   */
GLOBAL	WORD	rcs_cbflag = FALSE;	/* emit C-BASIC binding?   */
GLOBAL	WORD	rcs_f77flag = FALSE;	/* emit FORTRAN binding?   */
GLOBAL	WORD	rcs_fsrtflag = FALSE;   /* sort the binding file?  */
GLOBAL	WORD	rcs_lock = FALSE;	/* no tree changes? */
GLOBAL	WORD	rcs_xpert = FALSE;	/* omit warnings? */
GLOBAL	WORD	rcs_edited = FALSE;	/* quit without warning?  */
GLOBAL	WORD	rcs_low = FALSE;	/* low memory? */
GLOBAL	WORD	rcs_panic = FALSE;	/* in deep trouble! */ 
GLOBAL	WORD	rcs_menusel;		/* only used in MENU_STATE */
GLOBAL  WORD	rcs_rmsg[8];
GLOBAL  LPWORD	ad_rmsg;

GLOBAL	OBJECT	rcs_work[VIEWSIZE+1];	/* Space for workbench objects	*/
GLOBAL	ICONBLK	rcs_icons[VIEWSIZE];	/* Space for workbench iconblks */
GLOBAL	WORD	rcs_typ2icn[NUM_TYP] = {
	UNKNICON, PANLICON, MENUICON, DIALICON, ALRTICON, FREEICON};
					/* Variables used in write_file */
GLOBAL	UWORD	rcs_wraddr;		/* Current offset in output file */
GLOBAL	UWORD	wr_obnum;		/* Count of obj in tree		*/
GLOBAL	WORD	wr_obndx[TRACESIZE];	/* Tree trace index is built here */

GLOBAL  UWORD	rcs_ndxno = 0;

GLOBAL	INDEX	rcs_index[NDXSIZE];
GLOBAL	DEFLTS	deflt_options[NOPTS];
GLOBAL  BYTE	rsc_path[80];

GLOBAL	WORD	OK_NOFILE[] = { OPENITEM, MERGITEM, QUITITEM, INFITEM, 
				OUTPITEM, SAFEITEM,SVOSITEM, 
				PARTITEM, TOOLITEM, 0};

GLOBAL	WORD	OK_FILE[] = {NEWITEM, OPENITEM, MERGITEM, SVASITEM, 
			QUITITEM, INFITEM, OUTPITEM, SAFEITEM, SVOSITEM,
			PARTITEM, TOOLITEM, MERGITEM, CLOSITEM, 
			0};

GLOBAL	WORD	OK_TREES[] = {CLOSITEM, QUITITEM, INFITEM, OUTPITEM,SAFEITEM,
			      SVOSITEM, PARTITEM, TOOLITEM, 0};
		  
GLOBAL  WORD	OK_NOICN[] = {NEWITEM, OPENITEM, CLOSITEM, SVASITEM, OUTPITEM,
			      SAFEITEM, SVOSITEM,INVITEM,SOLIDIMG,SIZEITEM,0};

GLOBAL  WORD	OK_ICN[] =  {NEWITEM, OPENITEM, CLOSITEM, SVASITEM, OUTPITEM,
			     SAFEITEM, SVOSITEM,INVITEM,SOLIDIMG,SIZEITEM, 0};

GLOBAL  WORD    OK_EDITED[] = {NEWITEM, SVASITEM,RVRTITEM,0};
		
GLOBAL	WORD	ILL_LOCK[] = {MERGITEM, RNAMITEM, TYPEITEM, SRTITEM, 
			PASTITEM, CUTITEM, DELITEM, UNHDITEM, FLTITEM, 0}; 

GLOBAL	WORD	HOT_IBOX[] = {HOTBDCOL, HOTTHICK, 0};
GLOBAL	WORD	HOT_BOX[] = {HOTBDCOL, HOTTHICK, HOTBGCOL, HOTPATRN, 0};
GLOBAL	WORD	HOT_TEXT[] = {HOTFGCOL, HOTRULE, 0};
GLOBAL	WORD	HOT_IMAGE[] = {HOTFGCOL, 0};
GLOBAL	WORD	HOT_ICON[] = {HOTFGCOL, HOTBGCOL, 0};
GLOBAL	WORD	HOT_BTEXT[] = {HOTBDCOL, HOTTHICK, HOTBGCOL, HOTPATRN,
			HOTFGCOL, HOTRULE, 0};

GLOBAL	FILE   *rcs_fhhndl;	/* used in write_files and c_ routines */
GLOBAL	BYTE	rcs_hfile[100], hline[80];

GLOBAL	struct tally rcs_tally;

GLOBAL	WORD	c_obndx[MAPSIZE]; /* maps from disk address to string/image # */
GLOBAL	WORD	c_nstring;
GLOBAL	WORD	c_frstr;	/* first string referenced by freestr */
GLOBAL	WORD	c_nfrstr;
GLOBAL	WORD	c_nimage;
GLOBAL	WORD	c_frimg;	/* first image referenced by freebits */
GLOBAL	WORD	c_nbb;
GLOBAL	WORD	c_nfrbit;
GLOBAL	WORD	c_nib;
GLOBAL	WORD	c_nted;
GLOBAL	WORD	c_nobs;
GLOBAL	WORD	c_ntree;

GLOBAL	struct obitmap rcs_bit2obj[] = {
		SELECTABLE, 0, SBLEPOP,
		DEFAULT, 0, DFLTPOP,
		EXIT, 0, EXITPOP,
		RBUTTON, 0, RDIOPOP,
		0, DISABLED, DSBLPOP,
		EDITABLE, 0, EDBLPOP,
		TOUCHEXIT, 0, TCHXPOP,
		HIDETREE, 0, HDDNPOP,
		ESCCANCEL, 0, ESCCPOP,
		SCROLLER, 0, SCRCPOP,		
		FLAG3D, 0, D3DPOP,
		USECOLORCAT, 0, CCATPOP,
		BITBUTTON, 0, BTBNPOP,
		FL3DBAK, 0, B3DPOP,
		SUBMENU, 0, SBMPOP,
		0, CROSSED, CROSPOP,
		0, CHECKED, CHEKPOP,
		0, OUTLINED, OUTLPOP,
		0, SHADOWED, SHADPOP,
		0, SELECTED, SLCTPOP,
		0, WHITEBAK, WBGPOP,
		0, DRAW3D,   DR3DPOP,
		0, HIGHLIGHTED, HILIPOP,
		0, UNHIGHLIGHTED, UNHIPOP,
		0, 0, 0};

GLOBAL	struct popmap rcs_ht2pop[] = {
	HOTBGCOL, POPCOLOR, TRUE,
	HOTPATRN, POPPATRN, TRUE,
	HOTBDCOL, POPCOLOR, TRUE,
	HOTTHICK, POPTHICK, TRUE,
	HOTFGCOL, POPCOLOR, TRUE,
	HOTRULE,  POPRULE,  TRUE,
	HOTPOSN,  POPPOSN,  FALSE,
	HOTSWTCH, POPSWTCH, FALSE,
	0, 0};
GLOBAL	struct popmap icn_ht2pop[] = {
	FCLORBOX, POPCOLOR, TRUE,
	BCLORBOX, POPCOLOR, TRUE,
	0, 0};

GLOBAL	struct map rcs_it2thk[] = {
	OUT3POP, -3,
	OUT2POP, -2,
	OUT1POP, -1,
	NONEPOP,  0,
	IN1POP,   1,
	IN2POP,	  2,
	IN3POP,   3,
	0, 0};	  



GLOBAL	LOOKUP	c_types[N_TYPES] = {
	G_BOX, "G_BOX",
	G_TEXT, "G_TEXT",
	G_BOXTEXT, "G_BOXTEXT",
	G_IMAGE, "G_IMAGE",
	G_USERDEF, "G_USERDEF",
	G_IBOX, "G_IBOX",
	G_BUTTON, "G_BUTTON",
	G_BOXCHAR, "G_BOXCHAR",
	G_STRING, "G_STRING",
	G_FTEXT, "G_FTEXT",
	G_FBOXTEXT, "G_FBOXTEXT",
	G_ICON, "G_ICON",
	G_TITLE, "G_TITLE",
	G_CLRICN, "G_CLRICN"};

GLOBAL	LOOKUP	c_flags[N_FLAGS] = {
	NONE, "NONE",
	SELECTABLE, "SELECTABLE",
	DEFAULT, "DEFAULT",
	EXIT, "EXIT",
	EDITABLE, "EDITABLE",
	RBUTTON, "RBUTTON",
	LASTOB, "LASTOB",
	TOUCHEXIT, "TOUCHEXIT",
	HIDETREE, "HIDETREE",
	INDIRECT, "INDIRECT",
	ESCCANCEL, "ESCCANCEL",
	BITBUTTON, "BITBUTTON",
	SCROLLER, "SCROLLER",
	FLAG3D,   "FLAG3D",
	USECOLORCAT, "USECOLORCAT",
	FL3DBAK,  "FL3DBAK",
	SUBMENU,  "SUBMENU"
	};

GLOBAL	LOOKUP	c_states[N_STATES] = {
	NORMAL, "NORMAL",
	SELECTED, "SELECTED",
	CROSSED, "CROSSED",
	CHECKED, "CHECKED",
	DISABLED, "DISABLED",
	OUTLINED, "OUTLINED",
	SHADOWED, "SHADOWED",
	DRAW3D,   "DRAW3D",
	WHITEBAK, "WHITEBAK",
	HIGHLIGHTED, "HIGHLIGHTED",
	UNHIGHLIGHTED, "UNHIGH"};

GLOBAL	WORD	str_types[] = {G_STRING, G_BUTTON};
GLOBAL	WORD	box_types[] = {G_BOX, G_IBOX, G_BOXCHAR};
GLOBAL	WORD	txt_types[] = {G_TEXT, G_FTEXT, G_BOXTEXT, G_FBOXTEXT};

GLOBAL	OBJECT	blank_obj = {
	-1, -1, -1, G_BOX, HIDETREE, NORMAL, (LPVOID)0x00FF1100L, 0, 0, 8, 1};

GLOBAL	BYTE	*hptns[] = {
	"#define %S %W  \t/* TREE */\n", 
	"#define %S %W  \t/* STRING */\n", 
	"#define %S %B  \t/* OBJECT in TREE #%B */\n",
	"#define %S %W	\t/* FREE STRING */\n",
	"#define %S %W	\t/* FREE IMAGE */\n"};

GLOBAL	BYTE	*optns[] = {
	"      %S = %W;  \t(* TREE *)\n", 
	"      %S = %W;  \t(* STRING *)\n", 
	"      %S = %B;  \t(* OBJECT in TREE #%B *)\n",
	"      %S = %W;  \t(* FREE STRING *)\n",
	"      %S = %W;  \t(* FREE IMAGE *)\n"};

GLOBAL	BYTE	*bptns[] = {
	"      %S = %W;  REM ---TREE---\n", 
	"      %S = %W;  REM ---STRING---\n", 
	"      %S = %B;  REM ---OBJECT in TREE #%B---\n",
	"      %S = %W;  REM ---FREE STRING---\n",
	"      %S = %W;  REM ---FREE IMAGE---\n"};

GLOBAL	BYTE	*fptns[] = {
	"\tPARAMETER (%S = %W)\n", 
	"\tPARAMETER (%S = %W)\n", 
	"\tPARAMETER (%S = %B)\n",
	"\tPARAMETER (%S = %W)\n",
	"\tPARAMETER (%S = %W)\n"};

GLOBAL	LPRSHDR	rs_hdr;
GLOBAL	LPRSHDR	head;
GLOBAL	LONG	buff_size;
GLOBAL	LPBYTE	rcs_free;

MLOCAL	LPLSTR	d_frstr;		/* These are local to dcomp_free */
MLOCAL	LPLBIT	d_frimg;
MLOCAL	WORD	d_nfrstr;
MLOCAL	WORD	d_nfrimg;

VOID ini_buff()
	{
	UWORD	ii;

	for (ii = 0; ii < sizeof(RSHDR); ii++) ((LPBYTE)head)[ii] = 0;
	rcs_low = FALSE;
	rcs_panic = FALSE;
	rcs_free = ((LPBYTE)head) + sizeof(RSHDR);
	}

LPLTREE tree_ptr(WORD n)
	{
	return (LPLTREE)((LPBYTE)head + 
	        LW(head->rsh_trindex) + 
	        LW(n * sizeof(LPTREE)));
	}  

LPLSTR str_ptr(WORD n)
	{
	return (LPLSTR)((LPBYTE)head + 
	        LW(head->rsh_frstr ) + 
	        LW(n * sizeof(LPBYTE)));
	}

LPLBIT img_ptr(WORD n)
	{
	return (LPLBIT)((LPBYTE)head + 
	        LW(head->rsh_frimg)  + 
	        LW(n * sizeof(LONG)));
	}
	

LPTREE tree_addr(WORD n)
	{
	return *tree_ptr(n);
	}  

LPBYTE str_addr(WORD n)
	{
	return *str_ptr(n);
	}

LPBIT img_addr(WORD n)
	{
	return *img_ptr(n);
	}

VOID set_value(WORD key, LPBYTE val)
	{
	rcs_index[key].val = val;
	}

	
VOID set_kind(WORD key, WORD kind)
	{
	rcs_index[key].kind = kind;
	}

	
VOID set_name(WORD key, BYTE *name)
	{
	if (name == NULLPTR)  rcs_index[key].name[0] = '\0';
	else  strcpy(&(rcs_index[key].name[0]), name);
	}

	
WORD new_index(LPBYTE val, WORD kind, BYTE *name)
	{
	if (rcs_ndxno >= NDXSIZE) return (NIL);
	set_value(rcs_ndxno, val);
	set_kind(rcs_ndxno, kind);
	set_name(rcs_ndxno, name);
	return (rcs_ndxno++);
	}

VOID del_index(WORD key)
	{
	rcs_ndxno--;
	if (key < rcs_ndxno)
		movs(sizeof(INDEX) * (rcs_ndxno - key),
			(BYTE *)&rcs_index[key+1], 
			(BYTE *)&rcs_index[key]);
	}

WORD get_kind(WORD key)
	{
	return rcs_index[key].kind;
	}

LPBYTE get_value(WORD key)
	{
	return rcs_index[key].val;
	}

BYTE *get_name(WORD key)
	{
	return &(rcs_index[key].name[0]);
	}


WORD find_value(LPBYTE val)
	{
	WORD	entno;

	for (entno = 0; entno < rcs_ndxno; entno++)
		if (rcs_index[entno].val == val)
			return(entno);
	return (NIL);
	}

WORD find_tree(WORD n)
	{
	return find_value((LPBYTE) tree_addr(n));
	}

	
WORD find_obj(LPTREE tree, WORD obj)
	{
	return find_value( (LPBYTE) (&tree[obj]));
	}

	
WORD find_name(BYTE *name)
	{
	WORD	entno;

	for (entno = 0; entno < rcs_ndxno; entno++)
		if (!strcmp(name, &rcs_index[entno].name[0]) )
			return (entno);
	return (NIL);
	}

WORD tree_kind(WORD kind)
	{
	switch (kind) 
	{
		case UNKN:
		case PANL:
		case DIAL:
		case MENU:
		case ALRT:
		case FREE:
			return (TRUE);
		default:
			break;
		}
	return (FALSE);
	}

VOID unique_name(BYTE *name, BYTE *ptn, WORD n)
	{
	do {
		merge_str(name, ptn, n);
		n++;
		} while (find_name(name) != NIL);
	}
       

	WORD
blank_name( name )
	BYTE	*name;
	{
	WORD	blank, idx;

	
	blank = TRUE;	  
	for (idx = 0;name[idx] != 0; idx++)
	     if (name[idx] != ' ')
		{
		blank = FALSE;
	  	break;
		} 
	return( blank );
	}

	
WORD name_ok(BYTE *name, WORD ok, WORD nilok)
	{
	WORD	hit;

	if (name[0] == '@' || !name[0] || blank_name(name) )
	if (nilok) return (TRUE);
	else
	{
		hndl_alert(1, string_addr(STNONAM));
		return (FALSE);
	}

	hit = find_name(name);
	if (hit == NIL || hit == ok) return (TRUE);
	hndl_alert(1, string_addr(STNMDUP));
	return (FALSE);
	}


	
WORD set_obname(LPTREE tree, WORD obj, BYTE *name, LPTREE ntree, WORD nobj)
	{
	LPTEDI	taddr;
	WORD	where;

	taddr = (LPTEDI)(tree[obj].ob_spec);

	taddr->te_ptext   = ADDR(name);
	taddr->te_txtlen  = 9;
	taddr->te_tmplen  = 9;

	where = find_value((LPBYTE) (&ntree[nobj] ));
	if (where == NIL) strcpy(name, "@");
	else              strcpy(name, get_name(where));
	return (where);
	}

	
VOID get_obname(BYTE *name, LPTREE ntree, WORD nobj)
	{
	WORD	where;
	LPTREE	addr;

	addr = &ntree[nobj];
	where = find_value((LPBYTE) addr);
	if (where != NIL)
		{
		if (!name[0] || name[0] == '@') del_index(where);
		else  strcpy(get_name(where), name);
		return;
		}
	if (!name[0] || name[0] == '@') return;
	where = new_index((LPBYTE)addr, OBJKIND, name);
	if (where == NIL)
		hndl_alert(1, string_addr(STNFULL));
	}


	
WORD del_objindex(LPTREE tree, WORD obj)
	{
	WORD	where;

	where = find_obj(tree, obj);
	if (where != NIL)
		del_index(where);
	return 1;
	}

VOID zap_objindex(LPTREE tree, WORD obj)
	{
	map_tree(tree, obj, tree[obj].ob_next, del_objindex);
	}

LONG avail_mem()
	{
	return (LONG)((LPBYTE)head - (LPBYTE)rcs_free) + buff_size;
	}

LPVOID get_mem(UWORD bytes)
	{
	LPVOID	ret;
	LONG	left;

	ret = rcs_free;
#if	MC68K
	if (bytes & 0x1)
		bytes++;
#endif
	rcs_free += bytes;
	left = avail_mem();
	if (left < 1000 && !rcs_low)
		{
		hndl_alert(1, string_addr(STLOMEM));
		rcs_low = TRUE;
		}
	if (left <= 0 && !rcs_panic)
		{
		hndl_alert(1, string_addr(STPANIC));
		rcs_panic = TRUE;
		}
	return (ret);
	}

LPOBJ get_obmem()
	{
	UWORD	foo;

	foo = (UWORD) (rcs_free - ( ((LPBYTE)head) + head->rsh_object ));
	
	foo %= sizeof(OBJECT);
		 	/* synchronize to even OBJECT	*/
			/* boundary w.r.t. first OBJECT */
	if (foo)
		rcs_free += (UWORD) sizeof(OBJECT) - foo;
	return get_mem(sizeof(OBJECT));
	}

	
VOID update_if(LPLSTR taddr, BYTE *naddr)
	{
	if (!LLSTRCMP(*taddr, ADDR(naddr)))
		{
		*taddr = get_mem(strlen(naddr) + 1);
		LLSTRCPY(ADDR(naddr), *taddr);
		}
	}

LPTEDI copy_ti(LPTEDI source)
	{
	LPTEDI	dest;

	dest = get_mem(sizeof(TEDINFO));
	LBCOPY(ADDR(dest), ADDR(source), sizeof(TEDINFO) );
	return (dest);
	}

LPICON copy_ib(LPICON source)
	{
	LPICON	dest;

	dest = get_mem(sizeof(ICONBLK));
	LBCOPY(ADDR(dest), ADDR(source), sizeof(ICONBLK) );
	return (dest);
	}


LPBIT copy_bb(LPBIT source)
	{
	LPBIT	dest;

	dest = get_mem(sizeof(BITBLK));
	LBCOPY(ADDR(dest), ADDR(source), sizeof(BITBLK) );
	return (dest);
	}

	
WORD copy_spec(LPTREE tree, WORD obj)
	{
	WORD	type;
	LPVOID	obspec;

	type = LLOBT(tree[obj].ob_type);
	obspec = tree[obj].ob_spec;
	switch (type) {
		case G_TEXT:
		case G_BOXTEXT:
		case G_FTEXT:
		case G_FBOXTEXT:
			obspec = copy_ti(obspec);
			break;
		case G_ICON:
			obspec = copy_ib(obspec);
			break;
		case G_IMAGE:
			obspec = copy_bb(obspec);
			break;
		default:
			return;
		}
	tree[obj].ob_spec = obspec;
	return 1;
	}


	
WORD indx_obs(LPTREE tree, WORD which)
	{
	wr_obndx[wr_obnum++] = which;
	return 1;
	}


	
WORD fnd_ob(WORD old)
	{
	WORD	new;
	
	for (new = 0; new < wr_obnum && wr_obndx[new] != old; new++);
	return (new);
	}

WORD copy_objs(LPTREE stree, WORD obj, LPTREE dtree, WORD specopy)
	{
	WORD	iobj, root, link;
	LPOBJ	here;
	LPTREE	tree;
	UWORD	thisobj;

	tree = stree;
	wr_obnum = 0;
	map_tree(tree, obj, tree[obj].ob_next, indx_obs);

	tree = dtree;
	for (iobj = 0; iobj < wr_obnum; iobj++)
		{
		here = get_obmem();
		LBCOPY(ADDR(here), ADDR(&stree[wr_obndx[iobj]]), sizeof(OBJECT) );
		thisobj = (here - tree);
		if (!iobj)
			{
			root = thisobj;
			tree[thisobj].ob_next = NIL;
			}
		else if ( (link = tree[thisobj].ob_next) != -1L) tree[thisobj].ob_next = fnd_ob(link) + root; 
		if      ( (link = tree[thisobj].ob_head) != -1L) tree[thisobj].ob_head = fnd_ob(link) + root; 
		if      ( (link = tree[thisobj].ob_tail) != -1L) tree[thisobj].ob_tail = fnd_ob(link) + root; 
		}
	if (specopy) map_tree(tree, root, NIL, copy_spec);
	return (root);
	}

LPTREE copy_tree(LPTREE stree, WORD obj, WORD specopy)
	{
	LPTREE	dtree;

	get_obmem();
	dtree = (LPTREE)(rcs_free -= (UWORD) sizeof(OBJECT) );		/* a hack which */
	copy_objs(stree, obj, dtree, specopy);		/* saves memory */
	return (dtree);
	}


	
LPLTREE mak_trindex(WORD nentry)
	{
	LPLTREE	trindex;
	WORD	nobj, iobj;
	UWORD	indoff;

	nobj = head->rsh_ntree;
	head->rsh_ntree += nentry;

	trindex = (LPLTREE)get_mem(sizeof(LONG) * (nobj + nentry));
	indoff = (UWORD) ((LPBYTE)trindex - (LPBYTE)head);

	for (iobj = 0; iobj < nobj; iobj++)
		{
		*(trindex++) = tree_addr(iobj);
		}

	head->rsh_trindex = indoff;
	return (trindex);
	}

	
VOID add_trindex(LPTREE tree)
	{
	mak_trindex(1)[0] = tree;
	}


	
LPLSTR mak_frstr(WORD nentry)
	{
	LPLSTR	frstr;
	WORD	nobj, iobj;
	UWORD	indoff;

	nobj = head->rsh_nstring;
	head->rsh_nstring += nentry;

	frstr = (LPLSTR)get_mem(sizeof(LONG) * (nobj + nentry));
	indoff = (UWORD) ((LPBYTE)frstr - (LPBYTE)head);

	for (iobj = 0; iobj < nobj; iobj++)
		{		
		*(frstr++) = str_addr(iobj);
		}

	head->rsh_frstr = indoff;
	return (frstr);
	}

LPLBIT mak_frimg(WORD nentry)
	{
	LPLBIT	frimg;
	WORD	nobj, iobj;
	UWORD	indoff;

	nobj =head->rsh_nimages;
	head->rsh_nimages = nobj + nentry;

	frimg = (LPLBIT)get_mem(sizeof(LONG) * (nobj + nentry));
	indoff = (UWORD) ((LPBYTE)frimg - (LPBYTE)head);

	for (iobj = 0; iobj < nobj; iobj++)
		{
		*(frimg++) = img_addr(iobj);
		}

	head->rsh_frimg = indoff;
	return (frimg);
	}


	
WORD count_free(LPTREE tree, WORD obj)
	{
	switch (tree[obj].ob_type & 0xff) 
	{
		case G_STRING:
			d_nfrstr++;
			break;
		case G_IMAGE:
			d_nfrimg++;
			break;
		default:
			break;
		}
	return 1;
	}

					
WORD dcomp_tree(LPTREE tree, WORD obj)
{
	WORD	ndex;
	LPVOID	taddr;

	taddr = tree[obj].ob_spec;
	ndex = find_obj(tree, obj);

	switch (tree[obj].ob_type & 0xFF)
	{
		case G_STRING:
			*(d_frstr++) = (LPBYTE)taddr;
			d_nfrstr++;
			if (ndex == NIL)
				break;
			set_kind(ndex, FRSTR);
			set_value(ndex, (BYTE *) (d_nfrstr - 1));
			break;
		case G_IMAGE:
			*(d_frimg++) = (LPBIT)taddr;
			d_nfrimg++;
			if (ndex == NIL)
				break;
			set_kind(ndex, FRIMG);
			set_value(ndex, (BYTE *) (d_nfrimg - 1));
			break;
		default:
			break;
	}
	return 1;
}


					
VOID dcomp_free()
	{
	LPTREE	tree;
	WORD	ntrind, nfrstr, nfrimg, itree, ndex, ntree;

	ntrind = head->rsh_ntree;
	for (d_nfrimg = d_nfrstr = itree = 0; itree < ntrind; itree++ )
		{
		tree = tree_addr(itree);
		ndex = find_value( (LPBYTE) tree);
		if ( get_kind(ndex) == FREE )
			map_tree(tree, ROOT, NIL, count_free);
		}
	if (d_nfrstr)
		{
		nfrstr = head->rsh_nstring;
		d_frstr = mak_frstr(d_nfrstr);
		d_nfrstr = nfrstr;
		}
	if (d_nfrimg)
		{
		nfrimg = head->rsh_nimages;
		d_frimg = mak_frimg(d_nfrimg);
		d_nfrimg = nfrimg;
		} 
	for (ntree = itree = 0; itree < ntrind; itree++ )
		{
		tree = tree_addr(itree);
		ndex = find_value( (LPBYTE) tree);
		if ( get_kind(ndex) == FREE )
			{
			map_tree(tree, ROOT, NIL, dcomp_tree);
			del_objindex(tree, ROOT);
			}
		else
			{
			if (ntree != itree) *tree_ptr(ntree) = tree_addr(itree);
			ntree++;
			}
		}

	head->rsh_ntree = ntree;
	}

	BYTE
*c_lookup(what, where, n, punt)
	LOOKUP	*where;
	UWORD	what, n;
	BYTE	*punt;
	{
	UWORD	i;

	for (i = 0; i < n; i++)
		if (what == where->l_val)
			return (&where->l_name[0]);
		else
			where++;
	merge_str(punt, "0x%H", &what);
	return (punt);
	}

VOID map_all(MAPROUTINE routine)
	{
	WORD	ntree, itree;

	ntree = head->rsh_ntree;

	for (itree = 0; itree < ntree; itree++)
		map_tree( tree_addr(itree), 0, -1, routine);
	}

VOID clr_tally(VOID)
	{
	rcs_tally.nobj = 0;
	rcs_tally.nib = 0;
	rcs_tally.nbb = 0;
	rcs_tally.nti = 0;
	rcs_tally.nimg = 0;
	rcs_tally.nstr = 0;
	rcs_tally.nbytes = 0;
	}

	
VOID tally_str(LPBYTE addr)
	{
	if (addr == (LPBYTE)-1)
		return;
	rcs_tally.nstr++;
	rcs_tally.nbytes += (UWORD) (LSTRLEN(addr) + 1);
	}


	
VOID tally_bb(LPBIT addr)
	{
	rcs_tally.nbb++;
	rcs_tally.nbytes += (UWORD) sizeof(BITBLK);
	if (addr->bi_pdata == (LPVOID)-1)
		return;
	rcs_tally.nimg++;
	rcs_tally.nbytes += (UWORD) (addr->bi_wb * addr->bi_hl);
	}


	
VOID tally_free(VOID)
	{
	LPLSTR	free;
	LPLBIT  bfree;
	WORD	i;

	rcs_tally.nstr   += head->rsh_nstring;
	rcs_tally.nbytes += (UWORD) (head->rsh_nstring * sizeof(LONG));
	free = (LPLSTR) ( (LPBYTE)head + LW( head->rsh_frstr) );
	for (i = 0; i < head->rsh_nstring; i++) tally_str( free[i] );

	rcs_tally.nbb    += head->rsh_nimages;
	rcs_tally.nbytes += (UWORD) (head->rsh_nimages * 
		                         (sizeof(LONG) + sizeof(BITBLK)));
  	bfree = (LPLBIT) ( ((LPBYTE)head) + LW( head->rsh_frimg) );
	for (i = 0; i < head->rsh_nimages; i++) tally_bb( bfree[i] );
	}

	
WORD tally_obj(LPTREE tree, WORD obj)
	{
	LPVOID 	taddr; 
	LPTEDI  tedi;
	LPICON	icon;
	WORD	size;

	rcs_tally.nobj++;
	rcs_tally.nbytes += (UWORD) sizeof(OBJECT);
	taddr = tree[obj].ob_spec;
	tedi  = (LPTEDI)taddr;
	icon  = (LPICON)taddr;
	switch (LLOBT(tree[obj].ob_type))
	{
		case G_STRING:
		case G_BUTTON:
		case G_TITLE:
			if (taddr == (LPVOID)-1)
				return 1;
			tally_str(taddr);
			return 1;
		case G_TEXT:
		case G_BOXTEXT:
		case G_FTEXT:
		case G_FBOXTEXT:
			if (taddr == (LPVOID)-1)
				return 1;
			rcs_tally.nti++;
			rcs_tally.nbytes += (UWORD) sizeof(TEDINFO);
			tally_str(tedi->te_ptext);
			tally_str(tedi->te_pvalid);
			tally_str(tedi->te_ptmplt);
			return 1;
		case G_IMAGE:
			if (taddr == (LPVOID)-1L)
				return 1;
			tally_bb(taddr);
			return 1;
		case G_ICON:
			if (taddr == (LPVOID)-1L)
				return 1;
			rcs_tally.nib++;
			rcs_tally.nbytes += (UWORD) sizeof(ICONBLK);
			size = (icon->ib_wicon + 7) / 8;
			size *= icon->ib_hicon;
			if (icon->ib_pdata != (LPVOID)-1)
				{
				rcs_tally.nimg++;
				rcs_tally.nbytes += (UWORD) size;
				}
			if (icon->ib_pmask != (LPVOID)-1)
				{
				rcs_tally.nimg++;
				rcs_tally.nbytes += (UWORD) size;
				}
			tally_str(icon->ib_ptext);
			return 1;
		default:
			return 1;
		}
	}

