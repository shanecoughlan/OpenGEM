/*	GEMBIND.H 							*/
/*	CRYSBIND.H	05/05/84 - 02/02/85	Lee Lorenzen		*/
/*	2.0		10/4/85  - 10/28/85	Lowell Webster		*/
/*	to GEMBIND.H	3/11/86	 - 8/26/86	MDF			*/
/*	merge source	5/26/87			mdf			*/
/*	menu_click	9/25/87			mdf			*/


/*
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

				/* Application Manager			*/
#define APPL_INIT 10
#define APPL_READ 11
#define APPL_WRITE 12
#define APPL_FIND 13
#define APPL_TPLAY 14
#define APPL_TRECORD 15
#define APPL_BVSET 16
#define APPL_YIELD 17
#define APPL_XBVSET 18
#define APPL_EXIT 19
				/* Event Manager			*/
#define EVNT_KEYBD 20
#define	EVNT_BUTTON 21
#define EVNT_MOUSE 22
#define EVNT_MESAG 23
#define EVNT_TIMER 24
#define EVNT_MULTI 25
#define EVNT_DCLICK 26
				/* Menu Manager				*/
#define MENU_BAR 30
#define MENU_ICHECK 31
#define MENU_IENABLE 32
#define MENU_TNORMAL 33
#define MENU_TEXT 34
#define MENU_REGISTER 35
#define MENU_UNREGISTER 36
#define MENU_CLICK 37
				/* Object Manager			*/
#define OBJC_ADD 40
#define OBJC_DELETE 41
#define OBJC_DRAW 42
#define OBJC_FIND 43
#define OBJC_OFFSET 44
#define OBJC_ORDER 45
#define OBJC_EDIT 46
#define OBJC_CHANGE 47
				/* Form Manager				*/
#define FORM_DO 50
#define FORM_DIAL 51
#define FORM_ALERT 52
#define FORM_ERROR 53
#define FORM_CENTER 54
#define FORM_KEYBD 55
#define FORM_BUTTON 56
				/* Graphics Manager			*/
#define GRAF_RUBBOX 70
#define GRAF_DRAGBOX 71
#define GRAF_MBOX 72
#define GRAF_GROWBOX 73
#define GRAF_SHRINKBOX 74
#define GRAF_WATCHBOX 75
#define GRAF_SLIDEBOX 76
#define GRAF_HANDLE 77
#define GRAF_MOUSE 78
#define GRAF_MKSTATE 79

						/* Mouse Forms		*/
#define	ARROW	    0
#define	TEXT_CRSR   1
#define	HOURGLASS   2
#define	POINT_HAND  3
#define	FLAT_HAND   4
#define	THIN_CROSS  5
#define	THICK_CROSS 6
#define	OUTLN_CROSS 7
#define	USER_DEF  255
#define M_OFF     256
#define M_ON      257

				/* Scrap Manager			*/
#define SCRP_READ 80
#define SCRP_WRITE 81
#define SCRP_CLEAR 82
				/* File Selector Manager		*/
#define FSEL_INPUT 90
#define FSEL_EXINPUT 91
				/* Window Manager			*/
#define WIND_CREATE 100
#define WIND_OPEN 101
#define WIND_CLOSE 102
#define WIND_DELETE 103
#define WIND_GET 104
#define WIND_SET 105
#define WIND_FIND 106
#define WIND_UPDATE 107
#define WIND_CALC 108
				/* Resource Manager			*/
#define RSRC_LOAD 110
#define RSRC_FREE 111
#define RSRC_GADDR 112
#define RSRC_SADDR 113
#define RSRC_OBFIX 114
				/* Shell Manager			*/
#define SHEL_READ 120
#define SHEL_WRITE 121
#define SHEL_GET 122
#define SHEL_PUT 123
#define SHEL_FIND 124
#define SHEL_ENVRN 125
#define SHEL_RDEF 126
#define SHEL_WDEF 127
				/* Extended Graphics Manager		*/
#define XGRF_STEPCALC 130
#define XGRF_2BOX 131
#define XGRF_COLOUR 132	/* [JCE] Set the colours (ViewMAX/2) */
#define XGRF_DTIMAGE 133 /* Set desktop image (ViewMAX/3) */

					/* max sizes for arrays		*/
#define C_SIZE 5		/* was 4 for 2.0	*/
#define G_SIZE 15
#define I_SIZE 16
#define O_SIZE 7
#define AI_SIZE 2
#define AO_SIZE 5		/* was 1 for 2.0	*/
					/* Crystal funtion op code	*/
#define OP_CODE  control[0]
#define IN_LEN   control[1]
#define OUT_LEN  control[2]
#define AIN_LEN  control[3]
#define AOUT_LEN control[4]
			
#define RET_CODE LWGET(pInt_out)
					/* application lib parameters	*/
#define AP_VERSION LWGET(pGlobal)
#define AP_COUNT   LWGET(pGlobal + 2)
#define AP_ID      LWGET(pGlobal + 4)
#define AP_LOPRIVATE LWGET(pGlobal + 6)
#define AP_HIPRIVATE LWGET(pGlobal + 8)
#define AP_LOPNAME LWGET(pGlobal + 10)		/* long ptr. to tree base in rsc*/
#define AP_HIPNAME LWGET(pGlobal + 12)
#define AP_LO1RESV LWGET(pGlobal + 14)		/* long address of memory alloc.*/
#define AP_HI1RESV LWGET(pGlobal + 16)
#define AP_LO2RESV LWGET(pGlobal + 18)		/* length of memory allocated	*/
#define AP_HI2RESV LWGET(pGlobal + 20)		/* colors available on screen	*/
#define AP_LO3RESV LWGET(pGlobal + 22)
#define AP_HI3RESV LWGET(pGlobal + 24)
#define AP_LO4RESV LWGET(pGlobal + 26)
#define AP_HI4RESV LWGET(pGlobal + 28)

#define AP_GLSIZE LWGET(pInt_out + 2)

#define AP_RWID   pInt_in
#define AP_LENGTH pInt_in + 2
#define AP_PBUFF  pAddr_in

#define AP_PNAME pAddr_in

#define AP_TBUFFER pAddr_in
#define AP_TLENGTH pInt_in
#define AP_TSCALE  pInt_in + 2

#define AP_BVDISK pInt_in
#define AP_BVHARD pInt_in + 2

#define AP_XBVMODE pInt_in
#define AP_XBVDISK pAddr_in
#define AP_XBVHARD pAddr_in + 2
#define AP_XBVDISK1 LDGET(pInt_out)
#define AP_XBVHARD1 LDGET(pInt_out + 4)

#define SCR_MGR 0x0001			/* pid of the screen manager*/

#define AP_MSG 0
#define MN_SELECTED 10

#define WM_REDRAW 20
#define WM_TOPPED 21
#define WM_CLOSED 22
#define WM_FULLED 23
#define WM_ARROWED 24
#define WM_HSLID 25
#define WM_VSLID 26
#define WM_SIZED 27
#define WM_MOVED 28
#define WM_NEWTOP 29
#define WM_UNTOPPED 30
#define WM_ONTOP 31
#define WM_OFFTOP 32
#define PR_FINISH 33			/* added 9/8/86 MDF		*/

#define AC_OPEN 40
#define AC_CLOSE 41

#define CT_UPDATE 50
#define CT_MOVE 51
#define CT_NEWTOP 52
						/* event lib parameters	*/
#define IN_FLAGS pInt_in

#define B_CLICKS pInt_in
#define B_MASK   pInt_in + 2
#define B_STATE  pInt_in + 4

#define MO_FLAGS  pInt_in
#define MO_X      pInt_in + 2
#define MO_Y      pInt_in + 4
#define MO_WIDTH  pInt_in + 6
#define MO_HEIGHT pInt_in + 8

#define ME_PBUFF pAddr_in

#define T_LOCOUNT pInt_in
#define T_HICOUNT pInt_in + 2

#define MU_FLAGS pInt_in 
#define EV_MX LWGET(pInt_out + 2)
#define EV_MY LWGET(pInt_out + 4)
#define EV_MB LWGET(pInt_out + 6)
#define EV_KS LWGET(pInt_out + 8)
#define EV_KRET LWGET(pInt_out + 10)
#define EV_BRET LWGET(pInt_out + 12)


#define MB_CLICKS pInt_in + 2
#define MB_MASK   pInt_in + 4
#define MB_STATE   pInt_in + 6

#define MMO1_FLAGS pInt_in + 8
#define MMO1_X pInt_in + 10
#define MMO1_Y pInt_in + 12
#define MMO1_WIDTH pInt_in + 14
#define MMO1_HEIGHT pInt_in + 16

#define MMO2_FLAGS  pInt_in + 18
#define MMO2_X      pInt_in + 20
#define MMO2_Y      pInt_in + 22
#define MMO2_WIDTH  pInt_in + 24
#define MMO2_HEIGHT pInt_in + 26

#define MME_PBUFF pAddr_in

#define MT_LOCOUNT pInt_in + 28
#define MT_HICOUNT pInt_in + 30
						/* mu_flags		*/
#define MU_KEYBD 0x0001	
#define MU_BUTTON 0x0002
#define MU_M1 0x0004
#define MU_M2 0x0008
#define MU_MESAG 0x0010
#define MU_TIMER 0x0020

#define EV_DCRATE pInt_in 
#define EV_DCSETIT pInt_in + 2
						/* menu library parameters */

#define MM_ITREE	pAddr_in		/* ienable,icheck,tnorm	*/

#define MM_PSTR		pAddr_in

#define MM_PTEXT	pAddr_in + 4

#define SHOW_IT		pInt_in 		/* bar			*/

#define	ITEM_NUM	pInt_in 		/* icheck, ienable	*/
#define	MM_PID		pInt_in 		/* register		*/
#define	MM_MID		pInt_in 		/* unregister		*/
#define	CHECK_IT	pInt_in + 2		/* icheck		*/
#define	ENABLE_IT	pInt_in + 2		/* ienable		*/
#define MN_CLICK 	pInt_in 
#define MN_SETIT 	pInt_in + 2

#define	TITLE_NUM	pInt_in 		/* tnorm		*/
#define	NORMAL_IT	pInt_in + 2		/* tnormal		*/

					/* form library parameters	*/
#define FM_FORM pAddr_in
#define FM_START pInt_in 

#define FM_TYPE pInt_in 

#define FM_ERRNUM pInt_in 

#define FM_DEFBUT pInt_in 
#define FM_ASTRING pAddr_in

#define FM_IX pInt_in + 2
#define FM_IY pInt_in + 4
#define FM_IW pInt_in + 6
#define FM_IH pInt_in + 8
#define FM_X pInt_in + 10
#define FM_Y pInt_in + 12
#define FM_W pInt_in + 14
#define FM_H pInt_in + 16

#define FM_XC LWGET(pInt_out + 2)
#define FM_YC LWGET(pInt_out + 4)
#define FM_WC LWGET(pInt_out + 6)
#define FM_HC LWGET(pInt_out + 8)

#define FMD_START 0
#define FMD_GROW 1
#define FMD_SHRINK 2
#define FMD_FINISH 3

#define FMD_FORWARD 0
#define FMD_BACKWARD 1
#define FMD_DEFLT 2

#define FM_OBJ pInt_in 
#define FM_ICHAR pInt_in + 2
#define FM_INXTOB pInt_in + 4

#define FM_ONXTOB LWGET(pInt_out + 2)
#define FM_OCHAR  LWGET(pInt_out + 4)

#define FM_CLKS pInt_in + 2

					/* object library parameters	*/

#define OB_TREE pAddr_in 		/* all ob procedures		*/

#define OB_DELOB pInt_in 		/* ob_delete			*/

#define OB_DRAWOB pInt_in 		/* ob_draw, ob_change		*/
#define OB_DEPTH pInt_in + 2
#define OB_XCLIP pInt_in + 4
#define OB_YCLIP pInt_in + 6
#define OB_WCLIP pInt_in + 8
#define OB_HCLIP pInt_in + 10

#define OB_STARTOB pInt_in 		/* ob_find			*/
/*#define OB_DEPTH pInt_in + 2*/
#define OB_MX pInt_in + 4
#define OB_MY pInt_in + 6

#define OB_PARENT pInt_in 		/* ob_add			*/
#define OB_CHILD pInt_in + 2
#define OB_OBJ pInt_in 		/* ob_offset, ob_order		*/
#define OB_XOFF LWGET(pInt_out + 2)
#define OB_YOFF LWGET(pInt_out + 4)
#define OB_NEWPOS pInt_in + 2		/* ob_order			*/

					/* ob_edit			*/
#define OB_CHAR pInt_in + 2
#define OB_IDX pInt_in + 4
#define OB_KIND pInt_in + 6
#define OB_ODX LWGET(pInt_out + 2)

#define OB_NEWSTATE pInt_in + 12		/* ob_change			*/
#define OB_REDRAW pInt_in + 14
					/* graphics library parameters	*/
#define GR_I1 pInt_in 
#define GR_I2 pInt_in + 2
#define GR_I3 pInt_in + 4
#define GR_I4 pInt_in + 6
#define GR_I5 pInt_in + 8
#define GR_I6 pInt_in + 10
#define GR_I7 pInt_in + 12
#define GR_I8 pInt_in + 14

#define GR_O1 LWGET(pInt_out + 2)
#define GR_O2 LWGET(pInt_out + 4)

#define GR_TREE pAddr_in
#define GR_PARENT pInt_in 
#define GR_OBJ pInt_in + 2
#define GR_INSTATE pInt_in + 4
#define GR_OUTSTATE pInt_in + 6

#define GR_ISVERT pInt_in + 4

#define M_OFF 256
#define M_ON 257

#define GR_MNUMBER pInt_in 
#define GR_MADDR pAddr_in 

#define GR_WCHAR LWGET(pInt_out + 2)
#define GR_HCHAR LWGET(pInt_out + 4)
#define GR_WBOX LWGET(pInt_out + 4)
#define GR_HBOX LWGET(pInt_out + 4)

#define GR_MX LWGET(pInt_out + 2)
#define GR_MY LWGET(pInt_out + 4)
#define GR_MSTATE LWGET(pInt_out + 6)
#define GR_KSTATE LWGET(pInt_out + 8)
					/* scrap library parameters	*/
#define SC_PATH pAddr_in

#define SC_FTCSV 0x0001
#define SC_FTTXT 0x0002
#define SC_FTGEM 0x0004
#define SC_FTIMG 0x0008
#define SC_FTDCA 0x0010
#define SC_FTUSR 0x8000			/* some version is 0x0020	*/
					/* file selector library parms	*/

#define FS_IPATH  (pAddr_in)
#define FS_ISEL   (pAddr_in + 4)
#define FS_ITITLE (pAddr_in + 8)

#define FS_BUTTON LWGET(pInt_out + 2)
					/* window library parameters	*/
#define XFULL 0
#define YFULL gl_hbox
#define WFULL gl_width
#define HFULL (gl_height - gl_hbox)

#define NAME 0x0001
#define CLOSER 0x0002
#define FULLER 0x0004
#define MOVER 0x0008
#define INFO 0x0010
#define SIZER 0x0020
#define UPARROW 0x0040
#define DNARROW 0x0080
#define VSLIDE 0x0100
#define LFARROW 0x0200
#define RTARROW 0x0400
#define HSLIDE 0x0800

#define WF_KIND 1
#define WF_NAME 2
#define WF_INFO 3
#define WF_WXYWH 4
#define WF_CXYWH 5
#define WF_PXYWH 6
#define WF_FXYWH 7
#define WF_HSLIDE 8
#define WF_VSLIDE 9
#define WF_TOP 10
#define WF_FIRSTXYWH 11
#define WF_NEXTXYWH 12
#define WF_IGNORE 13
#define WF_NEWDESK 14
#define WF_HSLSIZ 15
#define WF_VSLSIZ 16
#define WF_SCREEN 17
#define WF_TATTRB 18
#define WF_SIZTOP 19
#define WF_COTOP  20	/* ViewMAX */

#define WF_OBFLAG  1001	/* Window tree: flag words */ 
#define WF_OBTYPE  1002	/* Window tree: type words */ 
#define WF_OBSPEC  1003	/* Window tree: spec dwords */ 

						/* update flags		*/
#define	END_UPDATE 0
#define	BEG_UPDATE 1
#define	END_MCTRL  2
#define	BEG_MCTRL  3
						/* arrow message	*/

#define WA_SUBWIN 1
#define WA_KEEPWIN 2

#define WA_UPPAGE 0
#define WA_DNPAGE 1
#define WA_UPLINE 2
#define WA_DNLINE 3
#define WA_LFPAGE 4
#define WA_RTPAGE 5
#define WA_LFLINE 6
#define WA_RTLINE 7
						/* wm_create		*/
#define WM_KIND pInt_in
						/* wm_open, close, del	*/
#define WM_HANDLE pInt_in
						/* wm_open, wm_create	*/
#define WM_WX pInt_in + 2
#define WM_WY pInt_in + 4
#define WM_WW pInt_in + 6
#define WM_WH pInt_in + 8
						/* wm_find		*/
#define WM_MX pInt_in
#define WM_MY pInt_in + 2
						/* wm_calc		*/
#define WC_BORDER 0
#define WC_WORK 1
#define WM_WCTYPE pInt_in 
#define WM_WCKIND pInt_in + 2
#define WM_WCIX pInt_in + 4
#define WM_WCIY pInt_in + 6
#define WM_WCIW pInt_in + 8
#define WM_WCIH pInt_in + 10
#define WM_WCOX LWGET(pInt_out + 2)
#define WM_WCOY LWGET(pInt_out + 4)
#define WM_WCOW LWGET(pInt_out + 6)
#define WM_WCOH LWGET(pInt_out + 8)
						/* wm_update		*/
#define WM_BEGUP pInt_in 


#define WM_WFIELD pInt_in + 2

#define WM_IPRIVATE pInt_in + 4

#define WM_IKIND pInt_in + 4
						/* for name and info	*/
#define WM_IOTITLE pAddr_in 

#define WM_IX pInt_in + 4
#define WM_IY pInt_in + 6
#define WM_IW pInt_in + 8
#define WM_IH pInt_in + 10

#define WM_OX LWGET(pInt_out + 2)
#define WM_OY LWGET(pInt_out + 4)
#define WM_OW LWGET(pInt_out + 6)
#define WM_OH LWGET(pInt_out + 8)

#define WM_ISLIDE pInt_in + 4

#define WM_IRECTNUM pInt_in + 12
					/* resource library parameters	*/

#define RS_PFNAME pAddr_in 		/* rs_init, 			*/
#define RS_TYPE pInt_in 
#define RS_INDEX pInt_in + 2
#define RS_INADDR pAddr_in 
#define RS_OUTADDR LSGET(pAddr_out)		

#define RS_TREE pAddr_in
#define RS_OBJ pInt_in 

#define R_TREE 0
#define R_OBJECT 1
#define R_TEDINFO 2
#define R_ICONBLK 3
#define R_BITBLK 4
#define R_STRING 5
#define R_IMAGEDATA 6
#define R_OBSPEC 7
#define R_TEPTEXT 8		/* sub ptrs in TEDINFO	*/
#define R_TEPTMPLT 9
#define R_TEPVALID 10
#define R_IBPMASK 11		/* sub ptrs in ICONBLK	*/
#define R_IBPDATA 12
#define R_IBPTEXT 13
#define R_BIPDATA 14		/* sub ptrs in BITBLK	*/
#define R_FRSTR 15		/* gets addr of ptr to free strings	*/
#define R_FRIMG 16		/* gets addr of ptr to free images	*/


				/* shell library parameters	*/
#define SH_DOEX pInt_in 
#define SH_ISGR pInt_in + 2
#define SH_ISCR pInt_in + 4
#define SH_PCMD pAddr_in
#define SH_PTAIL pAddr_in + 4

#define SH_PDATA pAddr_in
#define SH_PBUFFER pAddr_in

#define SH_LEN pInt_in 

#define SH_PATH pAddr_in
#define SH_SRCH pAddr_in + 4

#define SH_LPCMD pAddr_in 
#define SH_LPDIR pAddr_in + 4
				/* extended graphics library		*/
#define XGR_I1 pInt_in 
#define XGR_I2 pInt_in + 2
#define XGR_I3 pInt_in + 4
#define XGR_I4 pInt_in + 6
#define XGR_I5 pInt_in + 8
#define XGR_I6 pInt_in + 10
#define XGR_I7 pInt_in + 12
#define XGR_I8 pInt_in + 14
#define XGR_I9 pInt_in + 16
#define XGR_I10 pInt_in + 18

#define XGR_O1 LWGET(pInt_out + 2)
#define XGR_O2 LWGET(pInt_out + 4)
#define XGR_O3 LWGET(pInt_out + 6)
#define XGR_O4 LWGET(pInt_out + 8)
#define XGR_O5 LWGET(pInt_out + 10)

#define XGR_DTIMAGE pAddr_in 


/* [JCE] property (settings) library */

#define PROP_GET 1010
#define PROP_PUT 1011
#define PROP_DEL 1012
#define PROP_GUI_GET 1013
#define PROP_GUI_SET 1014

#define PROP_PROG pAddr_in
#define PROP_SECT pAddr_in + 4
#define PROP_BUF  pAddr_in + 8
#define PROP_OPT  pInt_in
#define PROP_BUFL pInt_in + 2

#define PROP_NUM pInt_in
#define PROP_VALUE pInt_in + 2

/* [JCE] extended application library */

#define XAPP_GETINFO 1020
#define XAP_GTYPE pInt_in
#define XAP_GOUT1 LWGET(pInt_out + 2)
#define XAP_GOUT2 LWGET(pInt_out + 4)
#define XAP_GOUT3 LWGET(pInt_out + 6)
#define XAP_GOUT4 LWGET(pInt_out + 8)

/* [JCE] extended shell library */

#define XSHL_GETSHELL 1030
#define XSHL_SETSHELL 1031
#define XSH_SHNAME pAddr_in
