/*	GEMSUPER.C	7/09/84 - 08/14/85	Lee Jay Lorenzen	*/
/*	2.0		11/06/85 - 12/12/85	Lowell Webster		*/
/*	APPLLIB.C	4/11/84 - 10/22/84	Lee Jay Lorenzen	*/
/*	EVNTLIB.C	4/11/84 - 5/16/84	Lee Jay Lorenzen	*/
/*	MENULIB.C	04/26/84 - 09/29/84	Lowell Webster		*/
/*	OBJCLIB.C	03/15/84 - 09/10/84	Gregg Morris		*/
/*	FORMLIB.C	03/15/84 - 06/16/85	Gregg Morris		*/
/*	GRAFLIB.C	05/05/84 - 09/10/84	Lee Lorenzen		*/
/*	SCRPLIB.C	05/05/84 - 02/02/85	Lee Lorenzen		*/
/*	FSELLIB.C	05/05/84 - 09/09/84	Lee Lorenzen		*/
/*	WINDLIB.C 	4/23/84 - 8/18/84	Lee Lorenzen		*/
/*	RSRCLIB.C	05/05/84 - 10/23/84	Lowell Webster		*/
/*	SHELLIB.C	4/11/84 - 01/29/85	Lee Lorenzen		*/
/*	merge High C vers. w. 2.2 		8/24/87		mdf	*/ 

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "aes.h"

#define CONTROL  pcrys_blk[0]
#define GGLOBAL  pcrys_blk[1]
#define INT_IN   pcrys_blk[2]
#define INT_OUT  pcrys_blk[3]
#define ADDR_IN  pcrys_blk[4]
#define ADDR_OUT pcrys_blk[5]


MLOCAL LPVOID	ad_rso;



#if SINGLAPP
	UWORD
crysbind1(opcode, pglobal, int_in, int_out, addr_in)
#endif
#if MULTIAPP
	UWORD
crysbind1(opcode, pglobal, int_in, int_out, addr_in, addr_out)
#endif
	WORD		opcode;
	REG LPWORD	pglobal;
	REG UWORD	int_in[];
	REG UWORD	int_out[];
	REG LPVOID	addr_in[];
#if MULTIAPP
	LONG		addr_out[];
#endif
{
	LONG		maddr, mbv;
	LPTREE		tree;
	REG WORD	ret;

	ret = TRUE;

// [JCE] This function is too big for PPD's optimiser, so it's split in two:

	switch(opcode)
	{	
				/* Application Manager			*/
	  case APPL_INIT:
#if SINGLAPP
	  	pglobal[0] = 0x0300;		/* version number	*/
	  	pglobal[1] = 0x0001;		/* num of concurrent procs*/
#endif
#if MULTIAPP
	  	pglobal[0] = 0x0110;		/* version number	*/
	  	pglobal[1] = NUM_DESKACC-1;	/* num of concurrent procs*/
#endif
/*		LLSET(pglobal, 0x00010200L);
*/
		pglobal[2]  = rlr->p_pid;
		sh_desk(0, (LPLONG)(&pglobal[3]));
		pglobal[10] = gl_nplanes;
		pglobal[11] = LLOWD((LONG)ADDR(&D));
		pglobal[12] = LHIWD((LONG)ADDR(&D));
		pglobal[13] = gl_bvdisk >> 16;
		pglobal[14] = gl_bvhard >> 16;
						/* reset dispatcher 	*/
						/*  count to let the app*/
						/*  run a while.	*/
		gl_dspcnt = NULL;
		ret = ap_init(addr_in[0]);
		break;
	  case APPL_READ:
	  case APPL_WRITE:
		ap_rdwr(opcode == APPL_READ ? MU_MESAG : MU_SDMSG, 
			fpdnm(NULLPTR, AP_RWID), AP_LENGTH, AP_PBUFF);
		break;
	  case APPL_FIND:
		ret = ap_find( AP_PNAME );
		break;
	  case APPL_TPLAY:
		ap_tplay(AP_TBUFFER, AP_TLENGTH, AP_TSCALE);
		break;
	  case APPL_TRECORD:
		ret = ap_trecd(AP_TBUFFER, AP_TLENGTH);
		break;
	  case APPL_BVSET:
		gl_bvdisk = ((ULONG)AP_BVDISK) << 16;
		gl_bvhard = ((ULONG)AP_BVHARD) << 16;
		break;
	  case APPL_YIELD:
		dsptch();
		break;
	  case APPL_BVEXT:
	    switch (AP_XBVMODE)
		{ 
			case 0: AP_XBVDISKL = LLOWD(gl_bvdisk);
					AP_XBVDISKH = LHIWD(gl_bvdisk);
					AP_XBVHARDL = LLOWD(gl_bvhard);
					AP_XBVHARDH = LHIWD(gl_bvhard);
					break;
			case 1: gl_bvdisk  = (LONG)AP_XBVDISK;
					gl_bvhard  = (LONG)AP_XBVHARD;
					break;
		}
		break;
	  case APPL_EXIT:
		ap_exit( TRUE );
		break;
				/* Event Manager			*/
	  case EVNT_KEYBD:
		  ret = ev_block(MU_KEYBD, 0x0L);
		break;
	  case EVNT_BUTTON:
		ret = ev_button(B_CLICKS, B_MASK, B_STATE, (WORD *)&EV_MX);
		break;
	  case EVNT_MOUSE:
		ret = ev_mouse((MOBLK *)&MO_FLAGS, (WORD *)&EV_MX);
		break;
	  case EVNT_MESAG:
#if MULTIAPP
						/* standard 16 byte read */
		ev_mesag(MU_MESAG, rlr, ME_PBUFF);
#endif
#if SINGLAPP
		ap_rdwr(MU_MESAG, rlr, 16, ME_PBUFF);
#endif
		break;
	  case EVNT_TIMER:
		ev_timer( HW(T_HICOUNT) + LW(T_LOCOUNT) );
		break;
	  case EVNT_MULTI:
		  if (MU_FLAGS & MU_TIMER)
		  maddr = HW(MT_HICOUNT) + LW(MT_LOCOUNT);
if ((MB_MASK == 3) && (MB_STATE == 1))
{
  MB_STATE = 0;

}
	  mbv = HW(MB_CLICKS) | LW((MB_MASK << 8) | MB_STATE);
		ret = ev_multi(MU_FLAGS, (MOBLK *)&MMO1_FLAGS, (MOBLK *)&MMO2_FLAGS, 
			maddr, mbv, MME_PBUFF, (WORD *)&EV_MX);
		break;
	  case EVNT_DCLICK:
		ret = ev_dclick(EV_DCRATE, EV_DCSETIT);
		break;
				/* Menu Manager				*/
	  case MENU_BAR:
	  /* GEM/2 and GEM/3 only draw the menu bar if it's for the 
	   * app that owns the menu, as defined by gl_mnppd. However,
	   * gl_mnppd is only set when a new top window is set. 
	   *
       * This, in turn, stops the desktop's menu bar appearing 
       * until it opens one or more windows. The call to w_menufix() below
       * forces a recheck of gl_mnppd and a redraw of the menu bar.
	   */
        if (gl_mnppd == rlr)
		  mn_bar(MM_ITREE, SHOW_IT, rlr->p_pid);
		else
		{
		  menu_tree[rlr->p_pid] = (SHOW_IT) ? MM_ITREE : ((LPTREE)0x0L);
	  	  /* Fix for GEM/1 and FreeGEM desktops */
		  w_menufix();
		}
		break;
	  case MENU_ICHECK:
		do_chg(MM_ITREE, ITEM_NUM, CHECKED, CHECK_IT, FALSE, FALSE);
		break;
	  case MENU_IENABLE:
		do_chg(MM_ITREE, (ITEM_NUM & 0x7fff), DISABLED, 
			!ENABLE_IT, ((ITEM_NUM & 0x8000) != 0x0), FALSE);
		break;
	  case MENU_TNORMAL:
		if (gl_mntree == menu_tree[rlr->p_pid])
		  do_chg(MM_ITREE, TITLE_NUM, SELECTED, !NORMAL_IT, 
				TRUE, TRUE);
		break;
	  case MENU_TEXT:
		tree = (LPTREE)MM_ITREE;
		if (LHIWD((LONG)tree))
		  LSTCPY((LPBYTE)tree[ITEM_NUM].ob_spec, (LPBYTE)MM_PTEXT);	
		else
		  LSTCPY(desk_acc[ gl_mnpds[ LLOWD(tree) ] ], MM_PTEXT);
		break;
	  case MENU_REGISTER:
		ret = mn_register(MM_PID, MM_PSTR);
		break;
	  case MENU_UNREGISTER:
		if (MM_MID == -1) 
		  MM_MID = gl_mnpds[rlr->p_pid];
		mn_unregister( MM_MID );
		break;
				/* Object Manager			*/
	  case MENU_CLICK:
		if (MN_SETIT)
		  gl_mnclick = MN_CLICK;
		ret = gl_mnclick;
		break;

	  case OBJC_ADD:
		ob_add(OB_TREE, OB_PARENT, OB_CHILD);
		break;
	  case OBJC_DELETE:
		ob_delete(OB_TREE, OB_DELOB);
		break;
	  case OBJC_DRAW:
		gsx_sclip((GRECT *)&OB_XCLIP);
		ob_draw(OB_TREE, OB_DRAWOB, OB_DEPTH);
		break;
	  case OBJC_FIND:
		ret = ob_find(OB_TREE, OB_STARTOB, OB_DEPTH, 
				OB_MX, OB_MY);
		break;
	  case OBJC_OFFSET:
		ob_offset(OB_TREE, OB_OBJ, (WORD *)&OB_XOFF, (WORD *)&OB_YOFF);
		break;
	  case OBJC_ORDER:
		ob_order(OB_TREE, OB_OBJ, OB_NEWPOS);
		break;
	  case OBJC_EDIT:
		gsx_sclip(&gl_rfull);
		OB_ODX = OB_IDX;
		ret = ob_edit(OB_TREE, OB_OBJ, OB_CHAR, (WORD *)&OB_ODX, OB_KIND);
		break;
	  case OBJC_CHANGE:
		gsx_sclip((GRECT *)&OB_XCLIP);
		ob_change(OB_TREE, OB_DRAWOB, OB_NEWSTATE, OB_REDRAW);
		break;
	}


	return (ret);
}


#if SINGLAPP
	UWORD
crysbind(opcode, pglobal, int_in, int_out, addr_in)
#endif
#if MULTIAPP
	UWORD
crysbind(opcode, pglobal, int_in, int_out, addr_in, addr_out)
#endif

	WORD		opcode;
	REG LPWORD	pglobal;
	REG UWORD	int_in[];
	REG UWORD	int_out[];
	REG LPVOID	addr_in[];
#if MULTIAPP
	LONG		addr_out[];
#endif
{
	LPVOID		maddr;
	REG WORD	ret;

	ret = TRUE;

	if (opcode < FORM_DO) return crysbind1(opcode,pglobal,int_in,int_out,addr_in
#if MULTIAPP
	,addr_out
#endif
	);
	
	switch(opcode)
	{
				/* Form Manager				*/
	  case FORM_DO:
		ret = fm_do(FM_FORM, FM_START);
		break;
	  case FORM_DIAL:
		ret = fm_dial(FM_TYPE, (GRECT *)&FM_X, (GRECT *)&FM_IX);
		break;
	  case FORM_ALERT:
		ret = fm_alert(FM_DEFBUT, FM_ASTRING);
		break;
	  case FORM_ERROR:
		ret = fm_error(FM_ERRNUM);
		break;
	  case FORM_CENTER:
		ob_center(FM_FORM, (GRECT *)&FM_XC);
		break;
	  case FORM_KEYBD:
		gsx_sclip(&gl_rfull);
		FM_OCHAR = FM_ICHAR;
		FM_ONXTOB = FM_INXTOB;
		ret = fm_keybd(FM_FORM, FM_OBJ, (WORD *)&FM_OCHAR, (WORD *)&FM_ONXTOB);
		break;
	  case FORM_BUTTON:
		gsx_sclip(&gl_rfull);
		ret = fm_button(FM_FORM, FM_OBJ, FM_CLKS, (WORD *)&FM_ONXTOB);
		break;
				/* Graphics Manager			*/
#if MULTIAPP
	  case PROC_CREATE:
	  	ret = prc_create(PR_IBEGADDR, PR_ISIZE, PR_ISSWAP, PR_ISGEM,
				 &PR_ONUM );
	  	break;
	  case PROC_RUN:
		ret = pr_run(PR_NUM, PR_ISGRAF, PR_ISOVER, PR_PCMD, PR_PTAIL);
		break;
	  case PROC_DELETE:
	  	ret = pr_abort(PR_NUM);
	  	break;
	  case PROC_INFO:
	  	ret = pr_info(PR_NUM, &PR_OISSWAP, &PR_OISGEM, &PR_OBEGADDR,
			&PR_OCSIZE, &PR_OENDMEM, &PR_OSSIZE, &PR_OINTADDR);
	  	break;
	  case PROC_MALLOC:
	  	ret = pr_malloc(PR_IBEGADDR, PR_ISIZE);
	  	break;
	  case PROC_MFREE:
	  	ret = pr_mfree(PR_NUM);
	  	break;
	  case PROC_SWITCH:
	  	ret = pr_switch(PR_NUM);
	  	break;
	  case PROC_SETBLOCK:
		ret = pr_setblock(PR_NUM);
		break;
#endif
				/* Graphics Manager			*/
	  case GRAF_RUBBOX:
		  gr_rubbox(GR_I1, GR_I2, GR_I3, GR_I4, 
					(WORD *)&GR_O1, (WORD *)&GR_O2);
		  break;
	  case GRAF_DRAGBOX:
		  gr_dragbox(GR_I1, GR_I2, GR_I3, GR_I4, (GRECT *)&GR_I5, 
					(WORD *)&GR_O1, (WORD *)&GR_O2);
		  break;
	  case GRAF_MBOX:
		  gr_movebox(GR_I1, GR_I2, GR_I3, GR_I4, GR_I5, GR_I6);
		  break;
#if GROWBOX
	  case GRAF_GROWBOX:
		gr_growbox((GRECT *)&GR_I1, (GRECT *)&GR_I5);
		break;
	  case GRAF_SHRINKBOX:
		gr_shrinkbox((GRECT *)&GR_I1, (GRECT *)&GR_I5);
		break;
#endif
	  case GRAF_WATCHBOX:
		ret = gr_watchbox(GR_TREE, GR_OBJ, GR_INSTATE, GR_OUTSTATE);
		break;
	  case GRAF_SLIDEBOX:
		ret = gr_slidebox(GR_TREE, GR_PARENT, GR_OBJ, GR_ISVERT);
		break;
	  case GRAF_HANDLE:
		GR_WCHAR = gl_wchar;
		GR_HCHAR = gl_hchar;
		GR_WBOX = gl_wbox;
		GR_HBOX = gl_hbox;
		ret = gl_handle;
		break;
	  case GRAF_MOUSE:
		if (GR_MNUMBER > 255)
		{
		  if (GR_MNUMBER == M_OFF)
		    gsx_moff();
		  if (GR_MNUMBER == M_ON)
		    gsx_mon();
		}
		else
		{
		  if (GR_MNUMBER != 255)		
		  {
		    rs_gaddr(ad_sysglo, R_BIPDATA, 3 + GR_MNUMBER, &maddr);
		    maddr = *(LPLPTR)maddr;
		  }
		  else
		    maddr = (LPVOID)GR_MADDR;
		  gsx_mfset(maddr);
		}
		break;
	  case GRAF_MKSTATE:
		ret = gr_mkstate((WORD *)&GR_MX, (WORD *)&GR_MY, (WORD *)&GR_MSTATE, (WORD *)&GR_KSTATE);
		break;
				/* Scrap Manager			*/
	  case SCRP_READ:
		ret = sc_read(SC_PATH);
		break;
	  case SCRP_WRITE:
		ret = sc_write(SC_PATH);
		break;
	  case SCRP_CLEAR:
	  	ret = sc_clear();
	  	break;
				/* File Selector Manager		*/
	  case FSEL_INPUT:
		ret = fs_exinput(FS_IPATH, FS_ISEL, (WORD *)&FS_BUTTON, ADDR(rs_str(ISELNAME)));
		break;
	  case FSEL_EXINPUT:
		ret = fs_exinput(FS_IPATH, FS_ISEL, (WORD *)&FS_BUTTON, FS_INAME);
		break;
				/* Window Manager			*/
	  case WIND_CREATE:
		ret = wm_create(WM_KIND, (GRECT *)&WM_WX);
		break;
	  case WIND_OPEN:
		wm_open(WM_HANDLE, (GRECT *)&WM_WX);
		break;
	  case WIND_CLOSE:
		wm_close(WM_HANDLE);
		break;
	  case WIND_DELETE:
		wm_delete(WM_HANDLE);
		break;
	  case WIND_GET:
		wm_get(WM_HANDLE, WM_WFIELD, (WORD *)&WM_OX);
		break;
	  case WIND_SET:
		  wm_set(WM_HANDLE, WM_WFIELD, (WORD *)&WM_IX);
		  break;
	  case WIND_FIND:
		ret = wm_find(WM_MX, WM_MY);
		break;
	  case WIND_UPDATE:
		wm_update(WM_BEGUP);
		break;
	  case WIND_CALC:
		wm_calc(WM_WCTYPE, WM_WCKIND, WM_WCIX, WM_WCIY, 
			WM_WCIW, WM_WCIH, (WORD *)&WM_WCOX, (WORD *)&WM_WCOY, 
			(WORD *)&WM_WCOW, (WORD *)&WM_WCOH);
		break;
				/* Resource Manager			*/
	  case RSRC_LOAD:
		ret = rs_load(pglobal, (LPBYTE)RS_PFNAME);
		break;
	  case RSRC_FREE:
		ret = rs_free(pglobal);
		break;
	  case RSRC_GADDR:
		ret = rs_gaddr(pglobal, RS_TYPE, RS_INDEX, &ad_rso);
		break;
	  case RSRC_SADDR:
		ret = rs_saddr(pglobal, RS_TYPE, RS_INDEX, RS_INADDR);
		break;
	  case RSRC_OBFIX:
		ret = rs_obfix(RS_TREE, RS_OBJ);
		break;
				/* Shell Manager			*/
	  case SHEL_READ:
		ret = sh_read((LPBYTE)SH_PCMD, (LPBYTE)SH_PTAIL);
		break;
	  case SHEL_WRITE:
		ret = sh_write(SH_DOEX, SH_ISGR, SH_ISCR, SH_PCMD, SH_PTAIL);
		break;
	  case SHEL_GET:
		ret = sh_get(SH_PBUFFER, SH_LEN);
		break;
	  case SHEL_PUT:
		ret = sh_put(SH_PDATA, SH_LEN);
		break;
	  case SHEL_FIND:
		ret = sh_find(SH_PATH);
		break;
	  case SHEL_ENVRN:
		ret = sh_envrn(SH_PATH, SH_SRCH);
		break;
	  case SHEL_RDEF:
		sh_rdef(SH_LPCMD, SH_LPDIR);
		break;
	  case SHEL_WDEF:
		sh_wdef(SH_LPCMD, SH_LPDIR);
		break;
	  case XGRF_STEPCALC:
	  	gr_stepcalc( XGR_I1, XGR_I2, (GRECT *)&XGR_I3, (WORD *)&XGR_O1,
		   (WORD *)&XGR_O2, (WORD *)&XGR_O3, (WORD *)&XGR_O4, (WORD *)&XGR_O5 );
		break;
	  case XGRF_2BOX:
	  	gr_2box(XGR_I4, XGR_I1, (GRECT *)&XGR_I6, XGR_I2,
			XGR_I3,	XGR_I5 );
		break;

	  /* ViewMAX calls */

	  case XGRF_COLOR:
		gsx_setclr(XGR_I1, XGR_I2, XGR_I3, XGR_I4, XGR_I5);
		break;
	  case XGRF_DTIMAGE:
		ret = change_desktop((FDB FAR *)XGR_IMAGE); 
		break;

	  /* Property get/set calls */
#if PROPLIB
	  case PROP_GET:
       ret = prop_get(PROP_PROG, PROP_SECT, PROP_BUF, PROP_BUFL, PROP_OPT);
       break;

	  case PROP_PUT:
       ret = prop_put(PROP_PROG, PROP_SECT, PROP_BUF, PROP_OPT);
       break;

	  case PROP_DEL:
       ret = prop_put(PROP_PROG, PROP_SECT, NULL, PROP_OPT);
	   break;

	  case PROP_GUI_GET:
	   ret = prop_gui_get(PROP_NUM);
	   break;

	  case PROP_GUI_SET:
	   ret = prop_gui_set(PROP_NUM, PROP_VALUE);
	   break;
	   
	   
#endif		
#if XAPPLIB
	  case XAPP_GETINFO:
        ret = xa_getinfo(XAPP_WHAT, (WORD *)&XAPP_OUT);
        break;
#endif
#if XSHELL
	  case XSHL_GETSHELL:
	  	ret = xs_getshell(XSHL_PROGRAM);
	  	break;
	  case XSHL_SETSHELL:
	  	ret = xs_setshell(XSHL_PROGRAM);
	  	break;
#endif
	  default:
		fm_show(ALNOFUNC, (UWORD *)NULLPTR, 1);
		ret = -1;
		break;
	}

	return(ret);
}

/*
*	Routine that copies input parameters into local buffers,
*	calls the appropriate routine via a case statement, copies
*	return parameters from local buffers, and returns to the
*	routine.
*/

	VOID
xif(pcrys_blk)
	LPLPTR		pcrys_blk;
{
	UWORD		control[C_SIZE];
	UWORD		int_in[I_SIZE];
	UWORD		int_out[O_SIZE];
	LPVOID		addr_in[AI_SIZE];
#if MULTIAPP
	LONG		addr_out[AO_SIZE];
#endif

	LWCOPY(ADDR(&control[0]), CONTROL, C_SIZE);
	if (IN_LEN)
	  LWCOPY(ADDR(&int_in[0]), INT_IN, IN_LEN);
	if (AIN_LEN)
	  LWCOPY(ADDR(&addr_in[0]), ADDR_IN, AIN_LEN*2);
	else
	  addr_in[0] = (LPVOID)0L; // [JCE] Conditional parameter to appl_init()
	  
#if SINGLAPP
	int_out[0] = crysbind(OP_CODE, GGLOBAL, &int_in[0], &int_out[0], 
				&addr_in[0]);
#endif
#if MULTIAPP
	int_out[0] = crysbind(OP_CODE, GGLOBAL, &int_in[0], &int_out[0], 
				&addr_in[0], &addr_out[0]);
#endif
	if (OUT_LEN)
	  LWCOPY(INT_OUT, ADDR(&int_out[0]), OUT_LEN);
	if (OP_CODE == RSRC_GADDR)
	 *((LPVOID FAR *)(ADDR_OUT)) = ad_rso;
#if MULTIAPP
	if (OP_CODE == PROC_INFO)
	  LWCOPY(ADDR_OUT, ADDR(&addr_out[0]), AOUT_LEN * 2);
#endif
}


/*
*	Supervisor entry point.  Stack frame must be exactly like
*	this if supret is to work.
*/
	VOID
super(pcrys_blk)
	LONG		pcrys_blk;
{
	xif(pcrys_blk);
	
	if ( (gl_dspcnt++ % 10) == 0 )
	  dsptch();
	supret(0);
}

	VOID
nsuper(pcrys_blk)
	LONG		pcrys_blk;
{
#if MULTIAPP
	supret(0);
#endif
}


