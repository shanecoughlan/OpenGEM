/*	GEMAPLIB.C	03/15/84 - 08/21/85	Lee Lorenzen		*/
/*	merge High C vers. w. 2.2 & 3.0		8/19/87		mdf	*/ 

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

#define TCHNG 0
#define BCHNG 1
#define MCHNG 2
#define KCHNG 3

						/* in PD88.C		*/
EXTERN PD		*fpdnm();

/* ---------- added for metaware compiler -------- */
						
EXTERN VOID		ev_timer();
EXTERN VOID		forkq();		/* in DISP.C 		*/
EXTERN VOID		wm_update();		/* in WMLIB.C 		*/
EXTERN VOID		mn_clsda();		/* in MNLIB.C 		*/
EXTERN VOID		gsx_mfset();		/* in GSXIF.C		*/
EXTERN VOID		all_run(); 		/* in INIT.C 		*/
/* ---------------------------------------------- */

EXTERN	WORD		sc_write();


EXTERN WORD		gl_ticktime;
EXTERN WORD 	gl_hschar;

EXTERN BYTE		gl_logdrv;

GLOBAL WORD		gl_play;
GLOBAL WORD		gl_recd;
GLOBAL WORD		gl_rlen;

#if MULTIAPP
EXTERN LONG		gl_efnorm, gl_efsave;
EXTERN PD		*gl_mnppd;
EXTERN SHELL		sh[];
EXTERN UWORD		dos_gpsp();
#endif

MLOCAL X_BUF_V2 x_buffer = 
{
	sizeof(X_BUF_V2),	/* structure size */
	16,					/* 16-bit */
	cc_s,				/* Colour categories */
	W_ACTIVE,			/* Window frames tree */
	"FreeGEM AES 3.00 alpha",

#if XAPPLIB				/* Generate an "X | Y | Z" abilities list */
	ABLE_GETINFO
#else
	0L
#endif

	|

#if PROPLIB				/* Was PROPLIB compiled in? */
	ABLE_PROP | ABLE_PROP2
#else
	0L 
#endif
	|
#if EMSDESK
	ABLE_EMSDESK			/* Was EMS support compiled in? */
#else
	0L
#endif
        | ABLE_WTREE | ABLE_X3D | ABLE_XBVSET
    		/* Can dynamically change window decorations */
    		/* Supports GEM/5 style 3D */
	
};


/*
*	Routine to initialize the application
*/


//
// [JCE] add support for the X_BUF (extended information buffer)
//

	WORD
ap_init(LPVOID x_buf)
{
	WORD		pid;
	LPBYTE		scdir;
#if MULTIAPP
	SHELL		*psh;
	UWORD		chseg;
#endif

	/* [JCE] Initialise extended info buffer */
	if (x_buf)
	{
		x_buffer.buf_len = min ( (*(LPWORD)x_buf), sizeof(X_BUF_V2));

		LBCOPY(x_buf, &x_buffer, x_buffer.buf_len);

	}
	
	pid = rlr->p_pid;
#if MULTIAPP
	psh = &sh[pid];
	if ( psh->sh_state & SHRINK )
	{
	  chseg = dos_gpsp();			/* get seg of acc's PSP	*/
	  dos_stblk(chseg, psh->sh_shrsz);	/* size down acc	*/

	}
#endif
	rs_gaddr(ad_sysglo, R_STRING, STSCDIR, (LPVOID *)&scdir);
	scdir[0] = gl_logdrv;		/* set drive letter	*/
	sc_write(scdir);
	return( pid );
}


/*
*	APplication READ or WRITE
*/
	VOID
ap_rdwr(code, p, length, pbuff)
	WORD		code;
	REG PD		*p;
	WORD		length;
	LPVOID		pbuff;
{
	QPB		m;
						/* do quick version if	*/
						/*   it is standard 16	*/
						/*   byte read and the	*/
						/*   pipe has only 16	*/
						/*   bytes inside it	*/
	if ( (code == MU_MESAG) &&
	     (p->p_qindex == length) &&
	     (length == 16) )
	{
	  LBCOPY(pbuff, p->p_qaddr, p->p_qindex);
	  p->p_qindex = 0;
	}
	else
	{
	  m.qpb_ppd = p;
	  m.qpb_cnt = length;
	  m.qpb_buf = pbuff;
	  ev_block(code, ADDR(&m));
	  }
}

/*
*	APplication FIND
*/
	WORD
ap_find(pname)
	LPBYTE		pname;
{
	REG PD		*p;
	BYTE		temp[9];

	LSTCPY(ADDR(&temp[0]), pname);
 
	p = fpdnm(&temp[0], 0x0);
	return( ((p) ? (p->p_pid) : (-1)) );
}

/*
*	APplication Tape PLAYer
*/
	VOID
ap_tplay(pbuff, length, scale)
	REG LPFPD	pbuff;
	WORD		length;
	WORD		scale;
{
	REG WORD	i;
	FPD		f;
	LPFPD	ad_f;

	ad_f = (LPFPD)ADDR(&f);

	gl_play = TRUE;
	for(i=0; i<length; i++)
	{
						/* get an event to play	*/
	  LBCOPY(ad_f, pbuff, sizeof(FPD));
	  pbuff++;
						/* convert it to machine*/
						/*   specific form	*/
	  switch( ((WORD)(f.f_code)) )
	  {
	    case TCHNG:
		ev_timer( (f.f_data*100L) / scale );
		f.f_code = 0;
		break;
	    case MCHNG:
		f.f_code = (FPD_CODE)mchange;
		break;
	    case BCHNG:
		f.f_code = (FPD_CODE)bchange;
		break;
	    case KCHNG:
		f.f_code = (FPD_CODE)kchange;
		break;
	  }
						/* play it		*/
	  if (f.f_code)
	    forkq(f.f_code, LLOWD(f.f_data), LHIWD(f.f_data));
						/* let someone else	*/
						/*   hear it and respond*/
	  dsptch();
	}
	gl_play = FALSE;
} /* ap_tplay */

/*
*	APplication Tape RECorDer
*/
	WORD
ap_trecd(pbuff, length)
	REG LPFPD	pbuff;
	REG WORD	length;
{
	REG WORD	i;
	REG WORD	code;
	FPD_CODE	proutine;
	
	
						/* start recording in	*/
						/*   forker()		*/
	cli();
	gl_recd = TRUE;
	gl_rlen = length;
	gl_rbuf = pbuff;
	sti();
	  					/* 1/10 of a second	*/
						/*   sample rate	*/
	while( gl_recd )
	  ev_timer( 100L );
						/* done recording so	*/
						/*   figure out length	*/
	cli();
	gl_recd = FALSE;
	gl_rlen = 0;
	length = (WORD)(gl_rbuf - pbuff);
	gl_rbuf = 0x0L;
	sti();
						/* convert to machine	*/
						/*   independent 	*/
						/*   recording		*/
	for(i=0; i<length; i++)
	{
#if MC68K
	  proutine = (FPD_CODE)LLGET(pbuff);
#endif
#if I8086
	  proutine = pbuff[0].f_code;
#endif
	  if(proutine == tchange)
	  {
		LONG tmp = pbuff[0].f_data;
	    code = TCHNG;
	    tmp *= gl_ticktime;
	    pbuff[0].f_data = tmp;
	  }
	  if(proutine == mchange) code = MCHNG;
	  if(proutine == kchange) code = KCHNG;
	  if(proutine == bchange) code = BCHNG;
	  *(LPWORD)pbuff = code;
	  pbuff++;
	}
	return(length);
} /* ap_trecd */

					/* [JCE] Made it always take an argument, */ 
	VOID            /*  even if SINGLAPP */
ap_exit(isgem)
	WORD		isgem;
{
	wm_update(TRUE);
#if SINGLAPP
	mn_clsda();
#endif
	if (rlr->p_qindex)
	  ap_rdwr(MU_MESAG, rlr, rlr->p_qindex, ad_valstr);
	gsx_mfset(ad_armice);
	wm_update(FALSE);
	all_run();
#if MULTIAPP
	if (isgem)
	  dos_term();
#endif
}

#if MULTIAPP
/*
*	special abort for accessories
*/
	VOID
ap_accexit()
{
	ap_exit(TRUE);
}
#endif


#if XAPPLIB
/* JCE 3-8-1999: xapp_getinfo() */


WORD	xa_getinfo(WORD code, WORD *rets)
{
	WORD n;

	for (n = 0; n < 4; n++) rets[n] = 0;	/* Default: return all 0s */
	/* Therefore "= 0" assignments below are commented out */

	switch(code)
	{
		case 0: /* Large font metrics */
			rets[0] = gl_hchar;
			//rets[1] = 0; /* XXX font ID? */
			//rets[2] = 0; /* system font */
			break;

		case 1: /* Small font metrics */
			rets[0] = gl_hschar;
			//rets[1] = 0; /* XXX font ID? */
			//rets[2] = 0; /* system font */
			break;

		case 2:	/* Colours */
			rets[0] = gl_handle;	/* VDI handle */
			rets[1] = 16;		/* 16 object colours */
			//rets[2] = 0;		/* No Atari clr icons */
			//rets[3] = 0;		/* No enhanced RSCs */
			break;

		case 3:	/* Language */
			//rets[0] = 0;		/* UK */
			break;

		case 4:	/* General */
			//rets[0] = 0;		/* No multitasking */
			//rets[1] = 0;		/* No pid conversion */
			//rets[2] = 0;		/* No appl_search() */
			//rets[3] = 0;		/* No rsrc_rcfix() */
			break;
		case 5:	/* General 2 */
			//rets[0] = 0;		/* No objc_xfind() */
			//rets[1] = 0;		/* Reserved */
			rets[2] = 1;		/* menu_click() available */
			rets[3] = 1;		/* shel_rdef(), shel_wdef() */
			break;
		case 6:	/* General 3 */
			//rets[0] = 0;		/* appl_read(-1, ...) */
			//rets[1] = 0;		/* shel_get(addr, -1)  */
			//rets[2] = 0;		/* menu_bar(tree, -1) */
			//rets[3] = 0;		/* menu_bar(tree, 100) */
			break;			
		case 7: /* MagiC */
			//rets[0] = 0;		/* Nothing */
			break;
		case 8: /* Mouse */
			//rets[0] = 0;		/* Mouse forms 256-258 */
			//rets[1] = 0;		/* AES doesn't remember mouse forms */
			break;
		case 9:	/* Super menus */
			//rets[0] = 0;		/* Submenus */
			//rets[1] = 0;		/* Popups  */
			//rets[2] = 0;		/* Scrollmenus */
			//rets[3] = 0;		/* enhanced MN_SELECTED */
			break;			
		case 10: /* shel_write */
			rets[0] = 1;		/* XXX what does this do? */
			rets[1] = 1;		/* singletasking GEM */
			rets[2] = 1;		/* ditto */
			//rets[3] = 0;		/* play safe */
			break;
		case 11: /* windows */
			//rets[0] = 0;		/* No extra Atari ones */
			//rets[1] = 0;		/* reserved */
			rets[2] = 8;		/* Hot closebox, whatever that is */
			//rets[3] = 0;		/* wind_update 256-257 */
			break;
		case 12: /* Messages */
			rets[0] = 2;		/* WM_UNTOPPED only */
			//rets[1] = 0;
			//rets[2] = 0;		/* WM_ICONIFY options */
			break;
		case 13: /* Objects */
			//rets[0] = 0;		/* ViewMAX 3D only */
			//rets[1] = 0;		/* No objc_sysvar */
			//rets[2] = 0;		/* No extra fonts */
			//rets[3] = 0;		/* Extra object types */
			break;

		case 14:	/* More MagiC */
			//rets[0] = 0;
			//rets[1] = 0;
			//rets[2] = 0;
			//rets[3] = 0;
			break;

		default:
			return 0;
	}
	return 1;
}

#endif /* XAPPLIB */
