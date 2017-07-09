/*	GEMBIND.C	2/15/85				Lee Lorenzen	*/

#include "portab.h"
#include "machine.h"
#include "treeaddr.h"
#include "obdefs.h"
#include "gembind.h"


GLOBAL BYTE		ctrl_cnts[] =
{
/* Application Manager		*/
	0, 1, 0, 			/* func 010		*/
	2, 1, 1, 			/* func 011		*/
	2, 1, 1, 			/* func 012		*/
	0, 1, 1, 			/* func 013		*/
	2, 1, 1, 			/* func 014		*/
	1, 1, 1, 			/* func 015		*/
	0, 0, 0, 			/* func 016		*/
	0, 0, 0, 			/* func 017		*/
	0, 0, 0, 			/* func 008		*/
	0, 1, 0, 			/* func 019		*/
/* Event Manager		*/
	0, 1, 0, 			/* func 020		*/
	3, 5, 0, 			/* func 021		*/
	5, 5, 0, 			/* func 022		*/
	0, 1, 1, 			/* func 023		*/
	2, 1, 0, 			/* func 024		*/
	16, 7, 1,	 		/* func 025		*/
	2, 1, 0, 			/* func 026		*/
	0, 0, 0, 			/* func 027		*/
	0, 0, 0, 			/* func 028		*/
	0, 0, 0, 			/* func 009		*/
/* Menu Manager			*/
	1, 1, 1, 			/* func 030		*/
	2, 1, 1, 			/* func 031		*/
	2, 1, 1, 			/* func 032		*/
	2, 1, 1, 			/* func 033		*/
	1, 1, 2, 			/* func 034		*/
	1, 1, 1, 			/* func 005		*/
	0, 0, 0, 			/* func 006		*/
	0, 0, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* Object Manager		*/
	2, 1, 1, 			/* func 040		*/
	1, 1, 1, 			/* func 041		*/
	6, 1, 1, 			/* func 042		*/
	4, 1, 1, 			/* func 043		*/
	1, 3, 1, 			/* func 044		*/
	2, 1, 1, 			/* func 045		*/
	4, 2, 1, 			/* func 046		*/
	8, 1, 1, 			/* func 047		*/
	0, 0, 0, 			/* func 048		*/
	0, 0, 0, 			/* func 049		*/
/* Form Manager			*/
	1, 1, 1, 			/* func 050		*/
	9, 1, 1, 			/* func 051		*/
	1, 1, 1, 			/* func 002		*/
	1, 1, 0, 			/* func 003		*/
	0, 5, 1, 			/* func 004		*/
	0, 0, 0, 			/* func 005		*/
	0, 0, 0, 			/* func 006		*/
	0, 0, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* Dialog Manager		*/
	0, 0, 0, 			/* func 060		*/
	0, 0, 0, 			/* func 061		*/
	0, 0, 0, 			/* func 062		*/
	0, 0, 0, 			/* func 003		*/
	0, 0, 0, 			/* func 004		*/
	0, 0, 0, 			/* func 005		*/
	0, 0, 0, 			/* func 006		*/
	0, 0, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* Graphics Manager	*/
	4, 3, 0, 			/* func 070		*/
	8, 3, 0, 			/* func 071		*/
	6, 1, 0, 			/* func 072		*/
	8, 1, 0, 			/* func 073		*/
	8, 1, 0, 			/* func 074		*/
	4, 1, 1, 			/* func 075		*/
	3, 1, 1, 			/* func 076		*/
	0, 5, 0, 			/* func 077		*/
	1, 1, 1, 			/* func 078		*/
	0, 5, 0, 			/* func 009		*/
/* Scrap Manager		*/
	0, 1, 1, 			/* func 080		*/
	0, 1, 1, 			/* func 081		*/
	0, 0, 0, 			/* func 082		*/
	0, 0, 0, 			/* func 083		*/
	0, 0, 0, 			/* func 084		*/
	0, 0, 0, 			/* func 005		*/
	0, 0, 0, 			/* func 006		*/
	0, 0, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* fseler Manager		*/
	0, 2, 2, 			/* func 090		*/
	0, 0, 0, 			/* func 091		*/
	0, 0, 0, 			/* func 092		*/
	0, 0, 0, 			/* func 003		*/
	0, 0, 0, 			/* func 004		*/
	0, 0, 0, 			/* func 005		*/
	0, 0, 0, 			/* func 006		*/
	0, 0, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* Window Manager		*/
	5, 1, 0, 			/* func 100		*/
	5, 1, 0, 			/* func 101		*/
	1, 1, 0, 			/* func 102		*/
	1, 1, 0, 			/* func 103		*/
	2, 5, 0, 			/* func 104		*/
	6, 1, 0, 			/* func 105		*/
	2, 1, 0, 			/* func 106		*/
	1, 1, 0, 			/* func 107		*/
	6, 5, 0, 			/* func 108		*/
	0, 0, 0, 			/* func 009		*/
/* Resource Manager		*/
	0, 1, 1, 			/* func 110		*/
	0, 1, 0, 			/* func 111		*/
	2, 1, 0, 			/* func 112		*/
	2, 1, 1, 			/* func 113		*/
	1, 1, 1, 			/* func 114		*/
	0, 0, 0, 			/* func 115		*/
	0, 0, 0, 			/* func 006		*/
	0, 0, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* Shell Manager		*/
	0, 1, 2, 			/* func 120		*/
	3, 1, 2, 			/* func 121		*/
	1, 1, 1, 			/* func 122		*/
	1, 1, 1, 			/* func 123		*/
	0, 1, 1, 			/* func 124		*/
	0, 1, 2 			/* func 125		*/
};



typedef struct gemblkstr
{
	LONG		gb_pcontrol;
	LONG		gb_pglobal;
	LONG		gb_pintin;
	LONG		gb_pintout;
	LONG		gb_padrin;
	LONG		gb_padrout;		
} GEMBLK;
						/* in STARTUP.A86	*/
EXTERN WORD		gem();

GLOBAL GEMBLK		gb;
GLOBAL UWORD		control[C_SIZE];
GLOBAL UWORD		global[G_SIZE];
GLOBAL UWORD		int_in[I_SIZE];
GLOBAL UWORD		int_out[O_SIZE];
GLOBAL LONG		addr_in[AI_SIZE];
GLOBAL LONG		addr_out[AO_SIZE];

GLOBAL LONG		ad_g;

	WORD
gem_if(opcode)
	WORD		opcode;
{
	WORD		i;
	BYTE		*pctrl;

	control[0] = opcode;

	pctrl = &ctrl_cnts[(opcode - 10) * 3];
	for(i=1; i<C_SIZE; i++)
	  control[i] = *pctrl++;

	gem(ad_g);
	return((WORD) RET_CODE );
}


	WORD
appl_init()
{
	gb.gb_pcontrol = ADDR((BYTE *) &control[0]); 
	gb.gb_pglobal = ADDR((BYTE *) &global[0]);
	gb.gb_pintin = ADDR((BYTE *) &int_in[0]);
	gb.gb_pintout = ADDR((BYTE *) &int_out[0]);
	gb.gb_padrin = ADDR((BYTE *) &addr_in[0]);
	gb.gb_padrout = ADDR((BYTE *) &addr_out[0]);

	ad_g = ADDR((BYTE *) &gb);
	gem_if(APPL_INIT);
	return((WORD) RET_CODE );
}


	WORD
appl_exit()
{
	gem_if(APPL_EXIT);
	return( TRUE );
}

					/* Application Manager		*/
	WORD
appl_write(rwid, length, pbuff)
	WORD		rwid;
	WORD		length;
	LONG		pbuff;
{
	AP_RWID = rwid;
	AP_LENGTH = length;
	AP_PBUFF = pbuff;
	return( gem_if(APPL_WRITE) );
}


	WORD
appl_read(rwid, length, pbuff)
	WORD		rwid;
	WORD		length;
	LONG		pbuff;
{
	AP_RWID = rwid;
	AP_LENGTH = length;
	AP_PBUFF = pbuff;
	return( gem_if(APPL_READ) );
}


	WORD
appl_find(pname)
	LONG		pname;
{
	AP_PNAME = pname;
	return( gem_if(APPL_FIND) );
}


	WORD
appl_tplay(tbuffer, tlength, tscale)
	LONG		tbuffer;
	WORD		tlength;
	WORD		tscale;
{
	AP_TBUFFER = tbuffer;
	AP_TLENGTH = tlength;
	AP_TSCALE = tscale;
	return( gem_if(APPL_TPLAY) );
}



	WORD
appl_trecord(tbuffer, tlength)
	LONG		tbuffer;
	WORD		tlength;
{
	AP_TBUFFER = tbuffer;
	AP_TLENGTH = tlength;
	return( gem_if(APPL_TRECORD) );
}

					/* Event Manager		*/
	UWORD
evnt_keybd()
{
	return((UWORD) gem_if(EVNT_KEYBD) );
}


	WORD
evnt_button(clicks, mask, state, pmx, pmy, pmb, pks)
	WORD		clicks;
	UWORD		mask;
	UWORD		state;
	WORD		*pmx, *pmy, *pmb, *pks;
{
	B_CLICKS = clicks;
	B_MASK = mask;
	B_STATE = state;
	gem_if(EVNT_BUTTON);
	*pmx = EV_MX;
	*pmy = EV_MY;
	*pmb = EV_MB;
	*pks = EV_KS;
	return((WORD) RET_CODE);
}


	WORD
evnt_mouse(flags, x, y, width, height, pmx, pmy, pmb, pks)
	WORD		flags, x, y, width, height;
	WORD		*pmx, *pmy, *pmb, *pks;
{
	MO_FLAGS = flags;
	MO_X = x;
	MO_Y = y;
	MO_WIDTH = width;
	MO_HEIGHT = height;
	gem_if(EVNT_MOUSE);
	*pmx = EV_MX;
	*pmy = EV_MY;
	*pmb = EV_MB;
	*pks = EV_KS;
	return((WORD) RET_CODE);
}


	WORD
evnt_mesag(pbuff)
	LONG		pbuff;
{
	ME_PBUFF = pbuff;
	return( gem_if(EVNT_MESAG) );
}


	WORD
evnt_timer(locnt, hicnt)
	UWORD		locnt, hicnt;
{
	T_LOCOUNT = locnt;
	T_HICOUNT = hicnt;
	return( gem_if(EVNT_TIMER) );
}


	WORD
evnt_multi(flags, bclk, bmsk, bst, m1flags, m1x, m1y, m1w, m1h, 
		m2flags, m2x, m2y, m2w, m2h, mepbuff,
		tlc, thc, pmx, pmy, pmb, pks, pkr, pbr )
	UWORD		flags, bclk, bmsk, bst;
	UWORD		m1flags, m1x, m1y, m1w, m1h;
	UWORD		m2flags, m2x, m2y, m2w, m2h;
	LONG		mepbuff;
	UWORD		tlc, thc;
	UWORD		*pmx, *pmy, *pmb, *pks, *pkr, *pbr;
{
	MU_FLAGS = flags;

	MB_CLICKS = bclk;
	MB_MASK = bmsk;
	MB_STATE = bst;

	MMO1_FLAGS = m1flags;
	MMO1_X = m1x;
	MMO1_Y = m1y;
	MMO1_WIDTH = m1w;
	MMO1_HEIGHT = m1h;

	MMO2_FLAGS = m2flags;
	MMO2_X = m2x;
	MMO2_Y = m2y;
	MMO2_WIDTH = m2w;
	MMO2_HEIGHT = m2h;

	MME_PBUFF = mepbuff;

	MT_LOCOUNT = tlc;
	MT_HICOUNT = thc;

	gem_if(EVNT_MULTI);

	*pmx = EV_MX;
	*pmy = EV_MY;
	*pmb = EV_MB;
	*pks = EV_KS;
	*pkr = EV_KRET;
	*pbr = EV_BRET;
	return((WORD) RET_CODE );
}


	WORD
evnt_dclick(rate, setit)
	WORD		rate, setit;
{
	EV_DCRATE = rate;
	EV_DCSETIT = setit;
	return( gem_if(EVNT_DCLICK) );
}


					/* Menu Manager			*/
	WORD
menu_bar(tree, showit)
	LONG		tree;
	WORD		showit;
{
	MM_ITREE = tree;
	SHOW_IT = showit;
	return( gem_if(MENU_BAR) );
}


	WORD
menu_icheck(tree, itemnum, checkit)
	LONG		tree;
	WORD		itemnum, checkit;
{
	MM_ITREE = tree;
	ITEM_NUM = itemnum;
	CHECK_IT = checkit;
	return( gem_if(MENU_ICHECK) );
}


	WORD
menu_ienable(tree, itemnum, enableit)
	LONG		tree;
	WORD		itemnum, enableit;
{
	MM_ITREE = tree;
	ITEM_NUM = itemnum;
	ENABLE_IT = enableit;
	return( gem_if(MENU_IENABLE) );
}


	WORD
menu_tnormal(tree, titlenum, normalit)
	LONG		tree;
	WORD		titlenum, normalit;
{
	MM_ITREE = tree;
	TITLE_NUM = titlenum;
	NORMAL_IT = normalit;
	return( gem_if( MENU_TNORMAL ) );
}


	WORD
menu_text(tree, inum, ptext)
	LONG		tree;
	WORD		inum;
	LONG		ptext;
{
	MM_ITREE = tree;
	ITEM_NUM = inum;
	MM_PTEXT = ptext;
	return( gem_if( MENU_TEXT ) );
}

	WORD
menu_register(pid, pstr)
	WORD		pid;
	LONG		pstr;
{
	MM_PID = pid;
	MM_PSTR = pstr;
	return( gem_if( MENU_REGISTER ) );
}


					/* Object Manager		*/
	WORD
objc_add(tree, parent, child)
	LONG		tree;
	WORD		parent, child;
{
	OB_TREE = tree;
	OB_PARENT = parent;
	OB_CHILD = child;
	return( gem_if( OBJC_ADD ) );
}


	WORD
objc_delete(tree, delob)
	LONG		tree;
	WORD		delob;
{
	OB_TREE = tree;
	OB_DELOB = delob;
	return( gem_if( OBJC_DELETE ) );
}


	WORD
objc_draw(tree, drawob, depth, xc, yc, wc, hc)
	LONG		tree;
	WORD		drawob, depth;
	WORD		xc, yc, wc, hc;
{
	OB_TREE = tree;
	OB_DRAWOB = drawob;
	OB_DEPTH = depth;
	OB_XCLIP = xc;
	OB_YCLIP = yc;
	OB_WCLIP = wc;
	OB_HCLIP = hc;
	return( gem_if( OBJC_DRAW ) );
}


	WORD
objc_find(tree, startob, depth, mx, my)
	LONG		tree;
	WORD		startob, depth, mx, my;
{
	OB_TREE = tree;
	OB_STARTOB = startob;
	OB_DEPTH = depth;
	OB_MX = mx;
	OB_MY = my;
	return( gem_if( OBJC_FIND ) );
}


	WORD
objc_order(tree, mov_obj, newpos)
	LONG		tree;
	WORD		mov_obj, newpos;
{
	OB_TREE = tree;
	OB_OBJ = mov_obj;
	OB_NEWPOS = newpos;
	return( gem_if( OBJC_ORDER ) );
}


	WORD
objc_offset(tree, obj, poffx, poffy)
	LONG		tree;
	WORD		obj;
	WORD		*poffx, *poffy;
{
	OB_TREE = tree;
	OB_OBJ = obj;
	gem_if(OBJC_OFFSET);
	*poffx = OB_XOFF;
	*poffy = OB_YOFF;
	return((WORD) RET_CODE );
}



	WORD
objc_edit(tree, obj, inchar, idx, kind)
	LONG		tree;
	WORD		obj;
	WORD		inchar, *idx, kind;
{
	OB_TREE = tree;
	OB_OBJ = obj;
	OB_CHAR = inchar;
	OB_IDX = *idx;
	OB_KIND = kind;
	gem_if( OBJC_EDIT );
	*idx = OB_ODX;
	return((WORD) RET_CODE );
}


	WORD
objc_change(tree, drawob, depth, xc, yc, wc, hc, newstate, redraw)
	LONG		tree;
	WORD		drawob, depth;
	WORD		xc, yc, wc, hc;
	WORD		newstate, redraw;
{
	OB_TREE = tree;
	OB_DRAWOB = drawob;
	OB_DEPTH = depth;
	OB_XCLIP = xc;
	OB_YCLIP = yc;
	OB_WCLIP = wc;
	OB_HCLIP = hc;
	OB_NEWSTATE = newstate;
	OB_REDRAW = redraw;
	return( gem_if( OBJC_CHANGE ) );
}



					/* Form Manager			*/
	WORD
form_do(form, start)
	LONG		form;
	WORD		start;
{
	FM_FORM = form;
	FM_START = start;
	return( gem_if( FORM_DO ) );
}


	WORD
form_dial(dtype, ix, iy, iw, ih, x, y, w, h)
	WORD		dtype;
	WORD		ix, iy, iw, ih;
	WORD		x, y, w, h;
{
	FM_TYPE = dtype;
	FM_IX = ix;
	FM_IY = iy;
	FM_IW = iw;
	FM_IH = ih;
	FM_X = x;
	FM_Y = y;
	FM_W = w;
	FM_H = h;
	return( gem_if( FORM_DIAL ) );
}


	WORD
form_alert(defbut, astring)
	WORD		defbut;
	LONG		astring;
{
	FM_DEFBUT = defbut;
	FM_ASTRING = astring;
	return( gem_if( FORM_ALERT ) );
}

	WORD
form_error(errnum)
	WORD		errnum;
{
	FM_ERRNUM = errnum;
	return( gem_if( FORM_ERROR ) );
}

	WORD
form_center(tree, pcx, pcy, pcw, pch)
	LONG		tree;
	WORD		*pcx, *pcy, *pcw, *pch;
{
	FM_FORM = tree;
	gem_if(FORM_CENTER);
	*pcx = FM_XC;
	*pcy = FM_YC;
	*pcw = FM_WC;
	*pch = FM_HC;
	return((WORD) RET_CODE );
}
					/* Graphics Manager		*/

	VOID
graf_rubbox(xorigin, yorigin, wmin, hmin, pwend, phend)
	WORD		xorigin, yorigin;
	WORD		wmin, hmin;
	WORD		*pwend, *phend;
{
	GR_I1 = xorigin;
	GR_I2 = yorigin;
	GR_I3 = wmin;
	GR_I4 = hmin;
	gem_if( GRAF_RUBBOX );
	*pwend = GR_O1;
	*phend = GR_O2;
	return((WORD) RET_CODE );
}


	VOID
graf_dragbox(w, h, sx, sy, xc, yc, wc, hc, pdx, pdy)
	WORD		w, h;
	WORD		sx, sy;
	WORD		xc, yc, wc, hc;
	WORD		*pdx, *pdy;
{
	GR_I1 = w;
	GR_I2 = h;
	GR_I3 = sx;
	GR_I4 = sy;
	GR_I5 = xc;
	GR_I6 = yc;
	GR_I7 = wc;
	GR_I8 = hc;
	gem_if( GRAF_DRAGBOX );
	*pdx = GR_O1;
	*pdy = GR_O2;
	return((WORD) RET_CODE );
}


	VOID
graf_mbox(w, h, srcx, srcy, dstx, dsty)
	WORD		w, h;
	WORD		srcx, srcy, dstx, dsty;
{
	GR_I1 = w;
	GR_I2 = h;
	GR_I3 = srcx;
	GR_I4 = srcy;
	GR_I5 = dstx;
	GR_I6 = dsty;
	return( gem_if( GRAF_MBOX ) );
}


	VOID
graf_growbox(orgx, orgy, orgw, orgh, x, y, w, h)
	WORD		orgx, orgy, orgw, orgh;
	WORD		x, y, w, h;
{
	GR_I1 = orgx;
	GR_I2 = orgy;
	GR_I3 = orgw;
	GR_I4 = orgh;
	GR_I5 = x;
	GR_I6 = y;
	GR_I7 = w;
	GR_I8 = h;
	return( gem_if( GRAF_GROWBOX ) );
}


	VOID
graf_shrinkbox(orgx, orgy, orgw, orgh, x, y, w, h)
	WORD		orgx, orgy, orgw, orgh;
	WORD		x, y, w, h;
{
	GR_I1 = orgx;
	GR_I2 = orgy;
	GR_I3 = orgw;
	GR_I4 = orgh;
	GR_I5 = x;
	GR_I6 = y;
	GR_I7 = w;
	GR_I8 = h;
	return( gem_if( GRAF_SHRINKBOX ) );
}


	VOID
graf_watchbox(tree, obj, instate, outstate)
	LONG		tree;
	WORD		obj;
	UWORD		instate, outstate;
{
	GR_TREE = tree;
	GR_OBJ = obj;
	GR_INSTATE = instate;
	GR_OUTSTATE = outstate;
	return( gem_if( GRAF_WATCHBOX ) );
}


	VOID
graf_slidebox(tree, parent, obj, isvert)
	LONG		tree;
	WORD		parent;
	WORD		obj;
	WORD		isvert;
{
	GR_TREE = tree;
	GR_PARENT = parent;
	GR_OBJ = obj;
	GR_ISVERT = isvert;
	return( gem_if( GRAF_SLIDEBOX ) );
}


	WORD
graf_handle(pwchar, phchar, pwbox, phbox)
	WORD		*pwchar, *phchar;
	WORD		*pwbox, *phbox;
{
	gem_if(GRAF_HANDLE);
	*pwchar = GR_WCHAR ;
	*phchar = GR_HCHAR;
	*pwbox = GR_WBOX;
	*phbox = GR_HBOX;
	return((WORD) RET_CODE);
}

	WORD
graf_mouse(m_number, m_addr)
	WORD		m_number;
	LONG		m_addr;
{
	GR_MNUMBER = m_number;
	GR_MADDR = m_addr;
	return( gem_if( GRAF_MOUSE ) );
}


	WORD
graf_mkstate(pmx, pmy, pmstate, pkstate)
	WORD		*pmx, *pmy, *pmstate, *pkstate;
{
	gem_if( GRAF_MKSTATE );
	*pmx = GR_MX;
	*pmy = GR_MY;
	*pmstate = GR_MSTATE;
	*pkstate = GR_KSTATE;
}


					/* Scrap Manager		*/
	WORD
scrp_read(pscrap)
	LONG		pscrap;
{
	SC_PATH = pscrap;
	return( gem_if( SCRP_READ ) );
}


	WORD
scrp_write(pscrap)
	LONG		pscrap;
{
	SC_PATH = pscrap;
	return( gem_if( SCRP_WRITE ) );
}


					/* fseler Manager		*/
	WORD
fsel_input(pipath, pisel, pbutton)
	LONG		pipath, pisel;
	WORD		*pbutton;
{
	FS_IPATH = pipath;
	FS_ISEL = pisel;
	gem_if( FSEL_INPUT );
	*pbutton = FS_BUTTON;
	return((WORD) RET_CODE );
}


					/* Window Manager		*/
	WORD
wind_create(kind, wx, wy, ww, wh)
	UWORD		kind;
	WORD		wx, wy, ww, wh;
{
	WM_KIND = kind;
	WM_WX = wx;
	WM_WY = wy;
	WM_WW = ww;
	WM_WH = wh;
	return( gem_if( WIND_CREATE ) );
}


	WORD
wind_open(handle, wx, wy, ww, wh)
	WORD		handle;
	WORD		wx, wy, ww, wh;
{
	WM_HANDLE = handle;
	WM_WX = wx;
	WM_WY = wy;
	WM_WW = ww;
	WM_WH = wh;
	return( gem_if( WIND_OPEN ) );
}


	WORD
wind_close(handle)
	WORD		handle;
{
	WM_HANDLE = handle;
	return( gem_if( WIND_CLOSE ) );
}


	WORD
wind_delete(handle)
	WORD		handle;
{
	WM_HANDLE = handle;
	return( gem_if( WIND_DELETE ) );
}


	WORD
wind_get(w_handle, w_field, pw1, pw2, pw3, pw4)
	WORD		w_handle;
	WORD		w_field;
	WORD		*pw1, *pw2, *pw3, *pw4;
{
	WM_HANDLE = w_handle;
	WM_WFIELD = w_field;
	gem_if( WIND_GET );
	*pw1 = WM_OX;
	*pw2 = WM_OY;
	*pw3 = WM_OW;
	*pw4 = WM_OH;
	return((WORD) RET_CODE );
}


	WORD
wind_set(w_handle, w_field, w2, w3, w4, w5)
	WORD		w_handle;	
	WORD		w_field;
	WORD		w2, w3, w4, w5;
{
	WM_HANDLE = w_handle;
	WM_WFIELD = w_field;
	WM_IX = w2;
	WM_IY = w3;
	WM_IW = w4;
	WM_IH = w5;
	return( gem_if( WIND_SET ) );
}


	WORD
wind_find(mx, my)
	WORD		mx, my;
{
	WM_MX = mx;
	WM_MY = my;
	return( gem_if( WIND_FIND ) );
}


	WORD
wind_update(beg_update)
	WORD		beg_update;
{
	WM_BEGUP = beg_update;
	return( gem_if( WIND_UPDATE ) );
}

	WORD
wind_calc(wctype, kind, x, y, w, h, px, py, pw, ph)
	WORD		wctype;
	UWORD		kind;
	WORD		x, y, w, h;
	WORD		*px, *py, *pw, *ph;
{
	WM_WCTYPE = wctype;
	WM_WCKIND = kind;
	WM_WCIX = x;
	WM_WCIY = y;
	WM_WCIW = w;
	WM_WCIH = h;
	gem_if( WIND_CALC );
	*px = WM_WCOX;
	*py = WM_WCOY;
	*pw = WM_WCOW;
	*ph = WM_WCOH;
	return((WORD) RET_CODE );
}

					/* Resource Manager		*/
	WORD
rsrc_load(rsname)
	LONG	rsname;
{
	RS_PFNAME = rsname;
	return( gem_if(RSRC_LOAD) );
}


	WORD
rsrc_free()
{
	return( gem_if( RSRC_FREE ) );
}


	WORD
rsrc_gaddr(rstype, rsid, paddr)
	WORD		rstype;
	WORD		rsid;
	LONG		*paddr;
{
	RS_TYPE = rstype;
	RS_INDEX = rsid;
	gem_if(RSRC_GADDR);
	*paddr = RS_OUTADDR;
	return((WORD) RET_CODE );
}


	WORD
rsrc_saddr(rstype, rsid, lngval)
	WORD		rstype;
	WORD		rsid;
	LONG		lngval;
{
	RS_TYPE = rstype;
	RS_INDEX = rsid;
	RS_INADDR = lngval;
	return( gem_if(RSRC_SADDR) );
}


	WORD
rsrc_obfix(tree, obj)
	LONG		tree;
	WORD		obj;
{
	RS_TREE = tree;
	RS_OBJ = obj;
	return( gem_if(RSRC_OBFIX) );
}


					/* Shell Manager		*/
	WORD
shel_read(pcmd, ptail)
	LONG		pcmd, ptail;
{
	SH_PCMD = pcmd;
	SH_PTAIL = ptail;
	return( gem_if( SHEL_READ ) );
}


	WORD
shel_write(doex, isgr, iscr, pcmd, ptail)
	WORD		doex, isgr, iscr;
	LONG		pcmd, ptail;
{
	SH_DOEX = doex;
	SH_ISGR = isgr;
	SH_ISCR = iscr;
	SH_PCMD = pcmd;
	SH_PTAIL = ptail;
	return( gem_if( SHEL_WRITE ) );
}


	WORD
shel_get(pbuffer, len)
	LONG		pbuffer;
	WORD		len;
{
	SH_PBUFFER = pbuffer;
	SH_LEN = len;
	return( gem_if( SHEL_GET ) );
}


	WORD
shel_put(pdata, len)
	LONG		pdata;
	WORD		len;
{
	SH_PDATA = pdata;
	SH_LEN = len;
	return( gem_if( SHEL_PUT ) );
}



	WORD
shel_find(ppath)
	LONG		ppath;
{
	SH_PATH = ppath;
	return( gem_if( SHEL_FIND ) );
}


	WORD
shel_envrn(ppath, psrch)
	LONG		ppath;
	LONG		psrch;
{
	SH_PATH = ppath;
	SH_SRCH = psrch;
	return( gem_if( SHEL_ENVRN ) );
}


