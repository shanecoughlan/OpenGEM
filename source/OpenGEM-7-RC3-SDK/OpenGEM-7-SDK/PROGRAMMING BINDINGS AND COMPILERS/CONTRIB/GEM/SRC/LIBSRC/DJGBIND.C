/*	GEMBIND.C	9/11/85				Lee Lorenzen	*/
/*	for 3.0		3/11/86 - 8/26/86		MDF		*/
/*	merge source	5/28/87				mdf		*/
/*	menu_click	9/25/87				mdf		*/

/*
*	-------------------------------------------------------------
*	GEM Desktop					  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1985 - 1987		Digital Research Inc.
*	-------------------------------------------------------------
*/

#include <limits.h>	/* for PATH_MAX */
#include "djgppgem.h"

extern LPBYTE pGeneral;
extern WORD lgl_apid;

#define CTRL_CNT	4		/*mike's version is 3*/

GLOBAL BYTE		ctrl_cnts[] =
{
/* Application Manager		*/
	0, 1, 1, 			/* func 010		*/
	2, 1, 1, 			/* func 011		*/
	2, 1, 1, 			/* func 012		*/
	0, 1, 1, 			/* func 013		*/
	2, 1, 1, 			/* func 014		*/
	1, 1, 1, 			/* func 015		*/
	2, 1, 0, 			/* func 016		*/
	0, 0, 0, 			/* func 017		*/
	1, 5, 2, 			/* func 018		*/
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
	1, 1, 0, 			/* func 006		*/
	2, 1, 0, 			/* func 007		*/
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
	9, 1, 0, 			/* func 051 		*/
	1, 1, 1, 			/* func 002		*/
	1, 1, 0, 			/* func 003		*/
	0, 5, 1, 			/* func 004		*/
	3, 3, 1, 			/* func 005		*/
	2, 2, 1, 			/* func 006		*/
	0, 0, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* Process Manager		*/
	0, 0, 0, 			/* func 060		*/
	0, 0, 0, 			/* func 061		*/
	0, 0, 0, 			/* func 062		*/
	0, 0, 0, 			/* func 063		*/
	0, 0, 0, 			/* func 064		*/
	0, 0, 0, 			/* func 065		*/
	0, 0, 0, 			/* func 066		*/
	0, 0, 0, 			/* func 067		*/
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
	0, 1, 0, 			/* func 082		*/
	0, 0, 0, 			/* func 083		*/
	0, 0, 0, 			/* func 084		*/
	0, 0, 0, 			/* func 005		*/
	0, 0, 0, 			/* func 006		*/
	0, 0, 0, 			/* func 007		*/
	0, 0, 0, 			/* func 008		*/
	0, 0, 0, 			/* func 009		*/
/* fseler Manager		*/
	0, 2, 2, 			/* func 090		*/
	0, 2, 3, 			/* func 091		*/
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
	0, 1, 2, 			/* func 125		*/
	0, 1, 2, 			/* func 126		*/
	0, 1, 2, 			/* func 127		*/
	1, 0, 1, 			/* func 128		*/
	1, 0, 1, 			/* func 129		*/
/* Extended Graphics Manager	*/
	6, 6, 0,			/* func 130		*/
	9, 1, 0,			/* func 131		*/
	5, 0, 0,			/* func 132		*/
	0, 0, 1,			/* func 133		*/
	0, 0, 0,			/* func 134		*/
	0, 0, 0,			/* func 135		*/
	0, 0, 0,			/* func 136		*/
	0, 0, 0,			/* func 137		*/
	0, 0, 0,			/* func 138		*/
	0, 0, 0 			/* func 139		*/
};

/* [JCE] new functions added by me start at 1000 */

GLOBAL BYTE		ctrl_ncnts[] =
{
/* Property Manager		*/
	2, 1, 3, 			/* func 1010		*/
	1, 1, 3, 			/* func 1011		*/
	1, 1, 2, 			/* func 1012		*/
	1, 1, 0, 			/* func 1013		*/
	2, 1, 0, 			/* func 1014		*/
	0, 0, 0, 			/* func 0005		*/
	0, 0, 0, 			/* func 0006		*/
	0, 0, 0, 			/* func 0007		*/
	0, 0, 0, 			/* func 0008		*/
	0, 0, 0, 			/* func 0009		*/
/* Extended Application Manager */
	1, 4, 0, 			/* func 1020		*/
	0, 0, 0, 			/* func 0001		*/
	0, 0, 0, 			/* func 0002		*/
	0, 0, 0, 			/* func 0003		*/
	0, 0, 0, 			/* func 0004		*/
	0, 0, 0, 			/* func 0005		*/
	0, 0, 0, 			/* func 0006		*/
	0, 0, 0, 			/* func 0007		*/
	0, 0, 0, 			/* func 0008		*/
	0, 0, 0, 			/* func 0009		*/
/* Extended Shell Manager */
	0, 0, 1, 			/* func 1030		*/
	0, 0, 1, 			/* func 1031		*/
	0, 0, 0, 			/* func 0002		*/
	0, 0, 0, 			/* func 0003		*/
	0, 0, 0, 			/* func 0004		*/
	0, 0, 0, 			/* func 0005		*/
	0, 0, 0, 			/* func 0006		*/
	0, 0, 0, 			/* func 0007		*/
	0, 0, 0, 			/* func 0008		*/
	0, 0, 0, 			/* func 0009		*/
};

						/* in STARTUP.A86	*/
EXTERN WORD		aes(VOID);

extern LPWORD pControl, pGlobal, pInt_in, pInt_out, pAddr_in, pAddr_out;
extern LPLPTR pParam;

WORD gem_if(WORD opcode)
{
	WORD		i;
	BYTE		*pctrl;

	LWSET(pControl, opcode);

	if (opcode >= 1000) pctrl = &ctrl_ncnts[(opcode - 1010) * 3];
	else                pctrl = &ctrl_cnts [(opcode - 10) * 3];
	for(i=1; i<=CTRL_CNT; i++)
	  LWSET(pControl + 2*i, *pctrl++);

	aes();
	return((WORD) RET_CODE );
}


WORD appl_init(X_BUF_V2 *buf)
{
	if (!aes_alloc_arrays()) return 0;

	if (buf)	/* Copy in the zeroised X_BUF */
	{
		dosmemput(buf, buf->buf_len, pGeneral);
		ctrl_cnts[2] = 1;
	}
	else 	ctrl_cnts[2] = 0;
	LSSET(AP_PBUFF, pGeneral);
	gem_if(APPL_INIT);
	
	if (buf)
	{
		/* Fix up pointers; bear in mind the binding only knows */
		/* about X_BUF up to V2 */
		/* No fixup for arch */
		LDSET(pGeneral+4,  LSGET(pGeneral+4)); /* cc */
		LDSET(pGeneral+8,  LSGET(pGeneral+8)); /* w_active */
		LDSET(pGeneral+12, LSGET(pGeneral+12)); /* info */
		/* No fixup for abilities */
		dosmemget(pGeneral, buf->buf_len, buf);
	}

	return((WORD) RET_CODE );
}


WORD appl_exit()
{
	gem_if(APPL_EXIT);
	return( TRUE );
}
					/* Application Manager		*/
WORD appl_write(WORD rwid, WORD length, WORD pbuff[])
{
	WORD    ret;
	LPWORD  pData = pGeneral;

	if (length > 4096) pData = dos_alloc(length);
	if (!pData) return -1;

	LWSET(AP_RWID,   rwid);
	LWSET(AP_LENGTH, length);
	LSSET(AP_PBUFF,  pData);

	dosmemput(pbuff, length, pData);
	ret = gem_if(APPL_WRITE);

	if (length > 4096) dos_free(pData);
	return ret;
}


WORD appl_read(WORD rwid, WORD length, WORD pbuff[])
{
	WORD    ret;
	LPWORD  pData = pGeneral;

	if (length > 4096) pData = dos_alloc(length);
	if (!pData) return -1;

	LWSET(AP_RWID,   rwid);
	LWSET(AP_LENGTH, length);
	LSSET(AP_PBUFF,  pData);

	dosmemput(pbuff, length, pData);
	ret = gem_if(APPL_READ);
	dosmemget(pData, length, pbuff);

	return ret;
}


WORD appl_find(char *pname)
{
	/* Note: An application name should never be more than 9 characters,
         *       including the terminating NULL. So the code below should
         *       never be necessary */

	WORD    ret;
	WORD	length = 1 + strlen(pname);
	LPWORD  pData = pGeneral;

	if (length > 4096) pData = dos_alloc(length);
	if (!pData) return -1;

	LSSET(AP_PNAME, pGeneral);

	dosmemput(pname, length, pData);
	
	ret = gem_if(APPL_FIND);

	if (length > 4096) dos_free(pData);

	return ret;
}


WORD appl_tplay(LPVOID tbuffer, WORD tlength, WORD tscale)
{
	LSSET(AP_TBUFFER, tbuffer);
	LWSET(AP_TLENGTH, tlength);
	LWSET(AP_TSCALE,  tscale);
	return( gem_if(APPL_TPLAY) );
}



WORD appl_trecord(LPVOID tbuffer, WORD tlength)
{
	LSSET(AP_TBUFFER, tbuffer);
	LWSET(AP_TLENGTH, tlength);
	return( gem_if(APPL_TRECORD) );
}


WORD appl_bvset(UWORD bvdisk, UWORD bvhard)
{
	LWSET(AP_BVDISK, bvdisk);
	LWSET(AP_BVHARD, bvhard);
	return( gem_if(APPL_BVSET) );
}



VOID appl_yield()
{
	gem_if(APPL_YIELD);
}



WORD appl_xbvset(ULONG bvdisk, ULONG bvhard)
{
	LDSET(AP_XBVDISK, bvdisk);
	LDSET(AP_XBVHARD, bvhard);
	LWSET(AP_XBVMODE, 1);
	return( gem_if(APPL_XBVSET) );
}



WORD appl_xbvget(ULONG *bvdisk, ULONG *bvhard)
{
	WORD rv;
	LWSET(AP_XBVMODE, 0);
	rv = gem_if(APPL_XBVSET);
	if (bvdisk) *bvdisk = AP_XBVDISK1;
	if (bvhard) *bvhard = AP_XBVHARD1;
	return rv;
}



					/* Event Manager		*/
UWORD evnt_keybd()
{
	return((UWORD) gem_if(EVNT_KEYBD) );
}


WORD evnt_button(WORD clicks, UWORD mask, UWORD state, 
		 WORD *pmx,   WORD *pmy, WORD *pmb, WORD *pks)
{
	LWSET(B_CLICKS, clicks);
	LWSET(B_MASK, mask);
	LWSET(B_STATE, state);
	gem_if(EVNT_BUTTON);
	*pmx = EV_MX;
	*pmy = EV_MY;
	*pmb = EV_MB;
	*pks = EV_KS;
	return((WORD) RET_CODE);
}


WORD evnt_mouse(WORD flags, WORD x, WORD y, WORD width, WORD height, 
		WORD *pmx, WORD *pmy, WORD *pmb, WORD *pks)
{
	LWSET(MO_FLAGS, flags);
	LWSET(MO_X, x);
	LWSET(MO_Y, y);
	LWSET(MO_WIDTH, width);
	LWSET(MO_HEIGHT, height);
	gem_if(EVNT_MOUSE);
	*pmx = EV_MX;
	*pmy = EV_MY;
	*pmb = EV_MB;
	*pks = EV_KS;
	return((WORD) RET_CODE);
}



WORD evnt_mesag(WORD buff[8])
{
	WORD n;

	dosmemput(buff, 16, pGeneral);
	LSSET(ME_PBUFF,     pGeneral);

	n = gem_if(EVNT_MESAG);

	dosmemget(pGeneral, 16, buff);
	return n;
}


WORD evnt_timer(UWORD locnt, UWORD hicnt)
{
	LWSET(T_LOCOUNT, locnt);
	LWSET(T_HICOUNT, hicnt);
	return( gem_if(EVNT_TIMER) );
}



WORD evnt_multi(UWORD flags,   UWORD bclk, UWORD bmsk, UWORD bst, 
	        UWORD m1flags, UWORD m1x,  UWORD m1y,  UWORD m1w, UWORD m1h, 
		UWORD m2flags, UWORD m2x,  UWORD m2y,  UWORD m2w, UWORD m2h, 
		WORD mepbuff[8],
		UWORD tlc,  UWORD thc, 
		UWORD *pmx, UWORD *pmy, UWORD *pmb, 
                UWORD *pks, UWORD *pkr, UWORD *pbr )
{
	LWSET(MU_FLAGS, flags);
	LWSET(MB_CLICKS, bclk);
	LWSET(MB_MASK, bmsk);
	LWSET(MB_STATE, bst);

	LWSET(MMO1_FLAGS, m1flags);
	LWSET(MMO1_X, m1x);
	LWSET(MMO1_Y, m1y);
	LWSET(MMO1_WIDTH, m1w);
	LWSET(MMO1_HEIGHT, m1h);

	LWSET(MMO2_FLAGS, m2flags);
	LWSET(MMO2_X, m2x);
	LWSET(MMO2_Y, m2y);
	LWSET(MMO2_WIDTH, m2w);
	LWSET(MMO2_HEIGHT, m2h);

	dosmemput(mepbuff, 16, pGeneral);
 	LSSET(MME_PBUFF, pGeneral);

	LWSET(MT_LOCOUNT, tlc);
	LWSET(MT_HICOUNT, thc);

	gem_if(EVNT_MULTI);

	dosmemget(pGeneral, 16, mepbuff);
	*pmx = EV_MX;
	*pmy = EV_MY;
	*pmb = EV_MB;
	*pks = EV_KS;
	*pkr = EV_KRET;
	*pbr = EV_BRET;
	return((WORD) RET_CODE );
}


WORD evnt_dclick(WORD rate, WORD setit)
{
	LWSET(EV_DCRATE, rate);
	LWSET(EV_DCSETIT, setit);
	return( gem_if(EVNT_DCLICK) );
}


					/* Menu Manager			*/
WORD menu_bar(LPTREE tree, WORD showit)
{
	LSSET(MM_ITREE, tree);
	LWSET(SHOW_IT, showit);
	return( gem_if(MENU_BAR) );
}


WORD menu_icheck(LPTREE tree, WORD itemnum, WORD checkit)
{
	LSSET(MM_ITREE, tree);
	LWSET(ITEM_NUM, itemnum);
	LWSET(CHECK_IT, checkit);
	return( gem_if(MENU_ICHECK) );
}


WORD menu_ienable(LPTREE tree, WORD itemnum, WORD enableit)
{
	LSSET(MM_ITREE, tree);
	LWSET(ITEM_NUM, itemnum);
	LWSET(ENABLE_IT, enableit);
	return( gem_if(MENU_IENABLE) );
}


WORD menu_tnormal(LPTREE tree, WORD titlenum, WORD normalit)
{
	LSSET(MM_ITREE, tree);
	LWSET(TITLE_NUM, titlenum);
	LWSET(NORMAL_IT, normalit);
	return( gem_if( MENU_TNORMAL ) );
}


WORD menu_text(LPTREE tree, WORD inum, LPBYTE ptext)
{
	LSSET(MM_ITREE, tree);
	LWSET(ITEM_NUM, inum);
	LSSET(MM_PTEXT, ptext);
	return( gem_if( MENU_TEXT ) );
}

WORD menu_register(WORD pid, LPBYTE pstr)
{
	LWSET(MM_PID,  pid);
	LSSET(MM_PSTR, pstr);
	return( gem_if( MENU_REGISTER ) );
}


WORD menu_unregister(WORD mid)
{
	LWSET( MM_MID, mid);
	return( gem_if( MENU_UNREGISTER ) );
}
					/* Object Manager		*/
WORD objc_add(LPTREE tree, WORD parent, WORD child)
{
	LSSET(OB_TREE,  tree);
	LWSET(OB_PARENT, parent);
	LWSET(OB_CHILD, child);
	return( gem_if( OBJC_ADD ) );
}


WORD objc_delete(LPTREE tree, WORD delob)
{
	LSSET(OB_TREE, tree);
	LWSET(OB_DELOB, delob);
	return( gem_if( OBJC_DELETE ) );
}

WORD objc_draw(LPTREE tree, WORD drawob, WORD depth, 
		WORD xc, WORD yc, WORD wc, WORD hc)
{
	LSSET(OB_TREE, tree);
	LWSET(OB_DRAWOB, drawob);
	LWSET(OB_DEPTH, depth);
	LWSET(OB_XCLIP, xc);
	LWSET(OB_YCLIP, yc);
	LWSET(OB_WCLIP, wc);
	LWSET(OB_HCLIP, hc);
	return( gem_if( OBJC_DRAW ) );
}


WORD objc_find(LPTREE tree, WORD startob, WORD depth, WORD mx, WORD my)
{
	LSSET(OB_TREE, tree);
	LWSET(OB_STARTOB, startob);
	LWSET(OB_DEPTH, depth);
	LWSET(OB_MX, mx);
	LWSET(OB_MY, my);
	return( gem_if( OBJC_FIND ) );
}




WORD objc_order(LPTREE tree, WORD mov_obj, WORD newpos)
{
	LSSET(OB_TREE, tree);
	LWSET(OB_OBJ, mov_obj);
	LWSET(OB_NEWPOS, newpos);
	return( gem_if( OBJC_ORDER ) );
}



WORD objc_offset(LPTREE tree, WORD obj, WORD *poffx, WORD *poffy)
{
	LSSET(OB_TREE, tree);
	LWSET(OB_OBJ,  obj);
	gem_if(OBJC_OFFSET);
	*poffx = OB_XOFF;
	*poffy = OB_YOFF;
	return((WORD) RET_CODE );
}


WORD objc_edit(LPTREE tree, WORD obj, WORD inchar, WORD *idx, WORD kind)
{
	LSSET(OB_TREE, tree);
	LWSET(OB_OBJ,  obj);
	LWSET(OB_CHAR, inchar);
	LWSET(OB_IDX, *idx);
	LWSET(OB_KIND, kind);
	gem_if( OBJC_EDIT );
	*idx = OB_ODX;
	return((WORD) RET_CODE );
}


WORD objc_change(LPTREE tree, WORD drawob, WORD depth, WORD xc, WORD yc, 
		 WORD wc, WORD hc, WORD newstate, WORD redraw)
{
	LSSET(OB_TREE, tree);
	LWSET(OB_DRAWOB, drawob);
	LWSET(OB_DEPTH, depth);
	LWSET(OB_XCLIP, xc);
	LWSET(OB_YCLIP, yc);
	LWSET(OB_WCLIP, wc);
	LWSET(OB_HCLIP, hc);
	LWSET(OB_NEWSTATE, newstate);
	LWSET(OB_REDRAW, redraw);
	return( gem_if( OBJC_CHANGE ) );
}



					/* Form Manager			*/
WORD form_do(LPTREE form, WORD start)
{
	LSSET(FM_FORM, form);
	LWSET(FM_START, start);
	return( gem_if( FORM_DO ) );
}


WORD form_dial(WORD dtype, WORD ix, WORD iy, WORD iw, WORD ih, 
			   WORD x,  WORD y,  WORD w,  WORD h)
{
	LWSET(FM_TYPE, dtype);
	LWSET(FM_IX, ix);
	LWSET(FM_IY, iy);
	LWSET(FM_IW, iw);
	LWSET(FM_IH, ih);
	LWSET(FM_X, x);
	LWSET(FM_Y, y);
	LWSET(FM_W, w);
	LWSET(FM_H, h);
	return( gem_if( FORM_DIAL ) );
}



WORD form_alert(WORD defbut, LPBYTE astring)
{
	LWSET(FM_DEFBUT,  defbut);
	LSSET(FM_ASTRING, astring);

	return( gem_if( FORM_ALERT ) );
}

WORD form_error(WORD errnum)
{
	LWSET(FM_ERRNUM, errnum);
	return( gem_if( FORM_ERROR ) );
}

WORD form_center(LPTREE tree, WORD *pcx, WORD *pcy, WORD *pcw, WORD *pch)
{
	LSSET(FM_FORM, tree);
	gem_if(FORM_CENTER);
	*pcx = FM_XC;
	*pcy = FM_YC;
	*pcw = FM_WC;
	*pch = FM_HC;
	return((WORD) RET_CODE );
}



WORD form_keybd(LPTREE form, WORD obj, WORD nxt_obj, WORD thechar, 
		WORD *pnxt_obj, WORD *pchar)
{
	LSSET(FM_FORM,   form);
	LWSET(FM_OBJ,    obj);
	LWSET(FM_INXTOB, nxt_obj);
	LWSET(FM_ICHAR,  thechar);
	gem_if( FORM_KEYBD );
	*pnxt_obj = FM_ONXTOB;
	*pchar = FM_OCHAR;
	return((WORD) RET_CODE );
}


WORD form_button(LPTREE form, WORD obj, WORD clks, WORD *pnxt_obj)
{
	LSSET(FM_FORM, form);
	LWSET(FM_OBJ,  obj);
	LWSET(FM_CLKS, clks);
	gem_if( FORM_BUTTON );
	*pnxt_obj = FM_ONXTOB;
	return((WORD) RET_CODE );
}

					/* Graphics Manager		*/
WORD graf_rubbox(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin, 
		 WORD *pwend, WORD *phend)
{
	LWSET(GR_I1, xorigin);
	LWSET(GR_I2, yorigin);
	LWSET(GR_I3, wmin);
	LWSET(GR_I4, hmin);
	gem_if( GRAF_RUBBOX );
	*pwend = GR_O1;
	*phend = GR_O2;
	return((WORD) RET_CODE );
}


WORD graf_dragbox(WORD w,    WORD h,  WORD sx, WORD sy, 
		  WORD xc,   WORD yc, WORD wc, WORD hc, 
		  WORD *pdx, WORD *pdy)
{
	LWSET(GR_I1, w);
	LWSET(GR_I2, h);
	LWSET(GR_I3, sx);
	LWSET(GR_I4, sy);
	LWSET(GR_I5, xc);
	LWSET(GR_I6, yc);
	LWSET(GR_I7, wc);
	LWSET(GR_I8, hc);
	gem_if( GRAF_DRAGBOX );
	*pdx = GR_O1;
	*pdy = GR_O2;
	return((WORD) RET_CODE );
}


WORD graf_mbox(WORD w, WORD h, WORD srcx, WORD srcy, WORD dstx, WORD dsty)
{
	LWSET(GR_I1, w);
	LWSET(GR_I2, h);
	LWSET(GR_I3, srcx);
	LWSET(GR_I4, srcy);
	LWSET(GR_I5, dstx);
	LWSET(GR_I6, dsty);
	return( gem_if( GRAF_MBOX ) );
}


WORD graf_watchbox(LPTREE tree, WORD obj, UWORD instate, UWORD outstate)
{
	LSSET(GR_TREE,     tree);
	LWSET(GR_OBJ,      obj);
	LWSET(GR_INSTATE,  instate);
	LWSET(GR_OUTSTATE, outstate);
	return( gem_if( GRAF_WATCHBOX ) );
}


WORD graf_slidebox(LPTREE tree, WORD parent, WORD obj, WORD isvert)
{
	LSSET(GR_TREE,   tree);
	LWSET(GR_PARENT, parent);
	LWSET(GR_OBJ,    obj);
	LWSET(GR_ISVERT, isvert);
	return( gem_if( GRAF_SLIDEBOX ) );
}



WORD graf_handle(WORD *pwchar, WORD *phchar, WORD *pwbox, WORD *phbox)
{
	gem_if(GRAF_HANDLE);
	*pwchar = GR_WCHAR ;
	*phchar = GR_HCHAR;
	*pwbox = GR_WBOX;
	*phbox = GR_HBOX;

	return((WORD) RET_CODE);
}


WORD graf_mouse(WORD m_number, LPWORD m_addr)
{
	LWSET(GR_MNUMBER, m_number);
	LWSET(GR_MADDR,     LP_OFF(m_addr));
	LWSET(GR_MADDR + 2, LP_SEG(m_addr));
	return( gem_if( GRAF_MOUSE ) );
}


VOID graf_mkstate(WORD *pmx, WORD *pmy, WORD *pmstate, WORD *pkstate)
{
	gem_if( GRAF_MKSTATE );
	*pmx = GR_MX;
	*pmy = GR_MY;
	*pmstate = GR_MSTATE;
	*pkstate = GR_KSTATE;
}

					/* Scrap Manager		*/
WORD scrp_read(char *pscrap)
{
	WORD r;

	
	LBSET(pGeneral, 0);	/* In case (as in ViewMAX) the call does
				 * nothing */
	LSSET(SC_PATH, pGeneral);
	r = gem_if( SCRP_READ );
	dosmemget(pGeneral, 1 + dj_lstrlen(pGeneral), pscrap);
	return r;
}


WORD scrp_write(char *pscrap)
{
	dosmemput(pscrap, 1 + strlen(pscrap), pGeneral);
	LSSET(SC_PATH, pGeneral);
	return( gem_if( SCRP_WRITE ) );
}


WORD scrp_clear(VOID)
{
	return( gem_if( SCRP_CLEAR ) );
}


				/* fseler Manager		*/
WORD fsel_input(BYTE *pipath, BYTE *pisel, WORD *pbutton)
{
	dosmemput(pipath, 1 + strlen(pipath), pGeneral);
	dosmemput(pisel,  1 + strlen(pisel),  pGeneral + PATH_MAX);
	LSSET(FS_IPATH, pGeneral);
	LSSET(FS_ISEL,  pGeneral + PATH_MAX);
	gem_if( FSEL_INPUT );

	dosmemget(pGeneral           , 1 + dj_lstrlen(pGeneral),            pipath);
	dosmemget(pGeneral + PATH_MAX, 1 + dj_lstrlen(pGeneral + PATH_MAX), pisel);

	*pbutton = FS_BUTTON;

	return((WORD) RET_CODE );
}


WORD fsel_exinput(BYTE *pipath, BYTE *pisel, WORD *pbutton, BYTE *title)
{
	dosmemput(pipath, 1 + strlen(pipath), pGeneral);
	dosmemput(pisel,  1 + strlen(pisel),  pGeneral + PATH_MAX);
	dosmemput(title,  1 + strlen(title),  pGeneral + 2*PATH_MAX);
	LSSET(FS_IPATH,   pGeneral);
	LSSET(FS_ISEL,    pGeneral + PATH_MAX);
	LSSET(FS_ITITLE,  pGeneral + 2*PATH_MAX);
	gem_if( FSEL_EXINPUT );

	dosmemget(pGeneral           , 1 + dj_lstrlen(pGeneral),            pipath);
	dosmemget(pGeneral + PATH_MAX, 1 + dj_lstrlen(pGeneral + PATH_MAX), pisel);

	*pbutton = FS_BUTTON;

	return((WORD) RET_CODE );
}


					/* Window Manager		*/
WORD wind_create(UWORD kind, WORD wx, WORD wy, WORD ww, WORD wh)
{
	LWSET(WM_KIND, kind);
	LWSET(WM_WX, wx);
	LWSET(WM_WY, wy);
	LWSET(WM_WW, ww);
	LWSET(WM_WH, wh);
	return( gem_if( WIND_CREATE ) );
}



WORD wind_open(WORD handle, WORD wx, WORD wy, WORD ww, WORD wh)
{
	LWSET(WM_HANDLE, handle);
	LWSET(WM_WX, wx);
	LWSET(WM_WY, wy);
	LWSET(WM_WW, ww);
	LWSET(WM_WH, wh);
	return( gem_if( WIND_OPEN ) );
}


WORD wind_close(WORD handle)
{
	LWSET(WM_HANDLE, handle);
	return( gem_if( WIND_CLOSE ) );
}


WORD wind_delete(WORD handle)
{
	LWSET(WM_HANDLE, handle);
	return( gem_if( WIND_DELETE ) );
}


WORD wind_get(WORD w_handle, WORD w_field, WORD *pw1, WORD *pw2, WORD *pw3, WORD *pw4)
{
	LWSET(WM_HANDLE, w_handle);
	LWSET(WM_WFIELD, w_field);
	gem_if( WIND_GET );
	*pw1 = WM_OX;
	*pw2 = WM_OY;
	*pw3 = WM_OW;
	*pw4 = WM_OH;
	return((WORD) RET_CODE );
}


WORD wind_set(WORD w_handle, WORD w_field, WORD w2, WORD w3, WORD w4, WORD w5)
{
	LWSET(WM_HANDLE, w_handle);
	LWSET(WM_WFIELD, w_field);
	LWSET(WM_IX, w2);
	LWSET(WM_IY, w3);
	LWSET(WM_IW, w4);
	LWSET(WM_IH, w5);
	return( gem_if( WIND_SET ) );
}


WORD dj_wind_setl(WORD w_handle, WORD w_field, LPVOID lp1, LPVOID lp2)
{
	LWSET(WM_HANDLE, w_handle);
	LWSET(WM_WFIELD, w_field);
	LWSET(WM_IX, LP_OFF(lp1));
	LWSET(WM_IY, LP_SEG(lp1));
	LWSET(WM_IW, LP_OFF(lp1));
	LWSET(WM_IH, LP_SEG(lp1));
	return( gem_if( WIND_SET ) );
}




WORD wind_find(WORD mx, WORD my)
{
	LWSET(WM_MX, mx);
	LWSET(WM_MY, my);
	return( gem_if( WIND_FIND ) );
}


WORD wind_update(WORD beg_update)
{
	LWSET(WM_BEGUP, beg_update);
	return( gem_if( WIND_UPDATE ) );
}

WORD wind_calc(WORD wctype, UWORD kind, WORD x, WORD y, WORD w, WORD h, 
		WORD *px, WORD *py, WORD *pw, WORD *ph)
{
	LWSET(WM_WCTYPE, wctype);
	LWSET(WM_WCKIND, kind);
	LWSET(WM_WCIX, x);
	LWSET(WM_WCIY, y);
	LWSET(WM_WCIW, w);
	LWSET(WM_WCIH, h);
	gem_if( WIND_CALC );
	*px = WM_WCOX;
	*py = WM_WCOY;
	*pw = WM_WCOW;
	*ph = WM_WCOH;
	return((WORD) RET_CODE );
}

					/* Resource Manager		*/
WORD rsrc_load(BYTE *rsname)
{
	dosmemput(rsname, 1 + strlen(rsname), pGeneral);

	LSSET(RS_PFNAME,    pGeneral);
	return( gem_if(RSRC_LOAD) );
}


WORD rsrc_free(VOID)
{
	return( gem_if( RSRC_FREE ) );
}


WORD rsrc_gaddr(WORD rstype, WORD rsid, LPVOID *paddr)
{
	LWSET(RS_TYPE, rstype);
	LWSET(RS_INDEX, rsid);
	gem_if(RSRC_GADDR);
	*paddr = RS_OUTADDR;
	return((WORD) RET_CODE );
}


WORD rsrc_saddr(WORD rstype, WORD rsid, LPVOID lngval)
{
	LWSET(RS_TYPE,   rstype);
	LWSET(RS_INDEX,  rsid);
	LSSET(RS_INADDR, lngval);
	return( gem_if(RSRC_SADDR) );
}


WORD rsrc_obfix(LPTREE tree, WORD obj)
{
	LSSET(RS_TREE, tree);
	LWSET(RS_OBJ,  obj);
	return( gem_if(RSRC_OBFIX) );
}


					/* Shell Manager		*/
WORD shel_read(LPBYTE pcmd, LPBYTE ptail)
{
	LSSET(SH_PCMD,  pcmd);
	LSSET(SH_PTAIL, ptail);
	return( gem_if( SHEL_READ ) );
}


WORD shel_write(WORD doex, WORD isgr, WORD iscr, LPBYTE pcmd, LPBYTE ptail)
{
	LWSET(SH_DOEX,  doex);
	LWSET(SH_ISGR,  isgr);
	LWSET(SH_ISCR,  iscr);
	LSSET(SH_PCMD,  pcmd);
	LSSET(SH_PTAIL, ptail);
	return( gem_if( SHEL_WRITE ) );
}


WORD shel_find(LPBYTE ppath)
{
	LSSET(SH_PATH, ppath);
	return( gem_if( SHEL_FIND ) );
}


WORD shel_envrn(LPBYTE ppath, LPBYTE psrch)
{
	LSSET(SH_PATH, ppath);
	LSSET(SH_SRCH, psrch);
	return( gem_if( SHEL_ENVRN ) );
}


WORD shel_rdef(LPBYTE lpcmd, LPBYTE lpdir)
{
	LSSET(SH_LPDIR, lpdir);
	return( gem_if( SHEL_RDEF ) );
}


WORD shel_wdef(LPBYTE lpcmd, LPBYTE lpdir)
{
	LSSET(SH_LPCMD, lpcmd);
	LSSET(SH_LPDIR, lpdir);
	return( gem_if( SHEL_WDEF ) );
}

WORD shel_get(LPBYTE buffer, WORD len)
{
	LSSET(SH_PBUFFER, buffer);
	LWSET(SH_LEN, len);
	return( gem_if( SHEL_GET ) );
}

WORD shel_put(LPBYTE buffer, WORD len)
{
	LSSET(SH_PBUFFER, buffer);
	LWSET(SH_LEN, len);
	return( gem_if( SHEL_PUT ) );
}


WORD xgrf_stepcalc(WORD orgw, WORD orgh, WORD xc,    WORD yc, WORD w, WORD h, 
		   WORD *pcx, WORD *pcy, WORD *pcnt, WORD *pxstep, WORD *pystep)
{
	LWSET(XGR_I1, orgw);
	LWSET(XGR_I2, orgh);
	LWSET(XGR_I3, xc);
	LWSET(XGR_I4, yc);
	LWSET(XGR_I5, w);
	LWSET(XGR_I6, h);
	gem_if( XGRF_STEPCALC );
	*pcx = XGR_O1;
	*pcy = XGR_O2;
	*pcnt = XGR_O3;
	*pxstep = XGR_O4;
	*pystep = XGR_O5;
	return((WORD) RET_CODE );
}

WORD xgrf_2box( WORD xc,    WORD yc,    WORD w, WORD h, WORD corners, WORD cnt, 
		WORD xstep, WORD ystep, WORD doubled)
{
	LWSET(XGR_I1, cnt);
	LWSET(XGR_I2, xstep);
	LWSET(XGR_I3, ystep);
	LWSET(XGR_I4, doubled);
	LWSET(XGR_I5, corners);
	LWSET(XGR_I6, xc);
	LWSET(XGR_I7, yc);
	LWSET(XGR_I8, w);
	LWSET(XGR_I9, h);
	return( gem_if( XGRF_2BOX ) );
}


/*  The following two routines, graf_growbox and graf_shrinkbox are
*   placed here for compatability with previous releases since they
*   are no longer directly supported by the AES.
*
*   [JCE] but they are by GROWBOX.ACC
*/


WORD graf_growbox(WORD x1, WORD y1, WORD w1, WORD h1,
                  WORD x2, WORD y2, WORD w2, WORD h2)
{
	LWSET(GR_I1, x1);
	LWSET(GR_I2, y1);
	LWSET(GR_I3, w1);
	LWSET(GR_I4, h1);
	LWSET(GR_I5, x2);
	LWSET(GR_I6, y2);
	LWSET(GR_I7, w2);
	LWSET(GR_I8, h2);

	return (gem_if(GRAF_GROWBOX));
}

WORD graf_shrinkbox(WORD x1, WORD y1, WORD w1, WORD h1,
                  WORD x2, WORD y2, WORD w2, WORD h2)
{
	LWSET(GR_I1, x1);
	LWSET(GR_I2, y1);
	LWSET(GR_I3, w1);
	LWSET(GR_I4, h1);
	LWSET(GR_I5, x2);
	LWSET(GR_I6, y2);
	LWSET(GR_I7, w2);
	LWSET(GR_I8, h2);

	return (gem_if(GRAF_SHRINKBOX));
}


/* This is GEM/3 specific function*/

WORD menu_click(WORD click, WORD setit)
{
	LWSET( MN_CLICK, click);
	LWSET( MN_SETIT, setit);
	return( gem_if( MENU_CLICK ));
}


/* [JCE] set system colours in ViewMAX */

WORD xgrf_colour( WORD type, WORD fg, WORD bg, WORD style, WORD index )
{
	LWSET(XGR_I1, type);	/* Object type */
	LWSET(XGR_I2, fg);	/* Foreground colour */
	LWSET(XGR_I3, bg);	/* Background colour */
	LWSET(XGR_I4, style);	/* Fill style */
	LWSET(XGR_I5, index);	/* Fill index */
	return ( gem_if ( XGRF_COLOUR ) );
}


/* This can't be allocated inside the binding - it's permanent */

WORD xgrf_dtimage( LPMFDB deskMFDB )
{
    LSSET(XGR_DTIMAGE, deskMFDB);

    return gem_if(XGRF_DTIMAGE);
}

/* JCE 25-7-1999: "Property" get/set calls */

WORD prop_get(char *program, char *section, char *buf, WORD buflen, WORD options)
{
	int lpname = 1 + strlen(program);
	int lsname = 1 + strlen(section);
	WORD r;

	dosmemput(program,  lpname, pGeneral);
	dosmemput(section,  lsname, pGeneral + lpname);
	LSSET(PROP_PROG, pGeneral);
	LSSET(PROP_SECT, pGeneral + lpname);
	LSSET(PROP_BUF,  pGeneral + lpname + lsname);
	LWSET(PROP_BUFL, buflen);
	LWSET(PROP_OPT,  options);
	r = gem_if(PROP_GET);

	if (r) return r;
	dosmemget(pGeneral + lpname + lsname, buflen, buf);
	return 0;
}

WORD prop_put(char *program, char *section, char *buf, WORD options)
{
	int lpname = 1 + strlen(program);
	int lsname = 1 + strlen(section);
	int lsbuf  = 1 + strlen(buf);

	dosmemput(program,  lpname, pGeneral);
	dosmemput(section,  lsname, pGeneral + lpname);
	dosmemput(buf,      lsbuf,  pGeneral + lpname + lsname);
	LSSET(PROP_PROG, pGeneral);
	LSSET(PROP_SECT, pGeneral + lpname);
	LSSET(PROP_BUF,  pGeneral + lpname + lsname);
	LWSET(PROP_OPT,  options);
	return gem_if(PROP_PUT);
}


WORD prop_del(char *program, char *section, WORD options)
{
	int lpname = 1 + strlen(program);
	int lsname = 1 + strlen(section);

	dosmemput(program,  lpname, pGeneral);
	dosmemput(section,  lsname, pGeneral + lpname);
	LSSET(PROP_PROG, pGeneral);
	LSSET(PROP_SECT, pGeneral + lpname);
	LWSET(PROP_OPT,  options);
	return gem_if(PROP_DEL);
}

WORD prop_gui_get(WORD property)
{
	LWSET(PROP_NUM, property);
	return gem_if(PROP_GUI_GET);
}


WORD prop_gui_set(WORD property, WORD value)
{
	LWSET(PROP_NUM, property);
	LWSET(PROP_VALUE, value);
	return gem_if(PROP_GUI_SET);
}



WORD xapp_getinfo(WORD ap_gtype, WORD *ap_gout1, WORD *ap_gout2,
				 WORD *ap_gout3, WORD *ap_gout4)
{
  WORD ret;

  LWSET(XAP_GTYPE, ap_gtype);
 
  ret = gem_if( XAPP_GETINFO );

  *ap_gout1 = XAP_GOUT1;
  *ap_gout2 = XAP_GOUT2;
  *ap_gout3 = XAP_GOUT3;
  *ap_gout4 = XAP_GOUT4;

  return ret;
}

/* JCE 4-12-1999: Get/set shell calls */

WORD xshl_getshell(char *sh)
{
	WORD r;
	
	LBSET(pGeneral, 0);
	LSSET(XSH_SHNAME, pGeneral);
	r = gem_if( XSHL_GETSHELL );
	dosmemget(pGeneral, 1 + dj_lstrlen(pGeneral), sh);
	return r;
}


WORD xshl_setshell(char *sh)
{
	dosmemput(sh, 1 + strlen(sh), pGeneral);
	LSSET(XSH_SHNAME, pGeneral);
	return( gem_if( XSHL_SETSHELL ) );
}



/*  GEMBIND.C  */

