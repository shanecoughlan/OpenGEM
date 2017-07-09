/*	GEMDISP.C	1/27/84 - 09/13/85	Lee Jay Lorenzen	*/
/*	merge High C vers. w. 2.2 & 3.0		8/19/87		mdf	*/ 
/*	add beep in chkkbd			11/12/87	mdf	*/

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

#define KEYSTOP 0x00002b1cL			/* control backslash	*/
						/* in ASM.A86    	*/

						/* in PD.C		*/
EXTERN VOID	insert_process();

EXTERN WORD 	gsx_kstate();			/* in GSXIF.C 		*/
EXTERN WORD 	gsx_char();
						/* in APLIB.C		*/
EXTERN WORD		gl_play;
EXTERN WORD		gl_recd;
EXTERN WORD		gl_rlen;

EXTERN PD		*gl_mowner;

EXTERN WORD		kstate;


EXTERN THEGLO		D;

#if MULTIAPP
EXTERN VOID		ap_accexit();
EXTERN VOID		psetup();
GLOBAL LONG		gl_prret;
GLOBAL LONG		gl_prpid;
EXTERN WORD	 	gl_ldpid;
EXTERN SHELL		sh[];
EXTERN LONG		gl_mntree;
EXTERN PD		*gl_mnppd;
GLOBAL PD		*gl_displock = 0;
#endif

/* forkq puts a fork block with a routine in the fork ring	*/

	VOID
forkq(fcode, lodata, hidata)
	FPD_CODE	fcode;
	WORD		lodata, hidata;
{
	REG FPD		*f;
						/* q a fork process,	*/
						/*   enter with ints OFF*/
	if (fpcnt == 0)
	  fpt = fph = 0;

	if (fpcnt < NFORKS)
	{
	  f = &D.g_fpdx[fpt++];
						/* wrap pointer around	*/
	  if (fpt == NFORKS)
	    fpt = 0;

	  f->f_code = fcode;
	  f->f_data = HW(hidata) | LW(lodata);

	  fpcnt++;
	}
}


	VOID
disp_act(p)
	REG PD		*p;
{      
						/* process is ready,	*/
						/*   so put him on RLR	*/
	p->p_stat &= ~WAITIN;
	insert_process(p, &rlr);	
}


	VOID
mwait_act(p)
	REG PD		*p;
{	
						/* sleep on nrl if	*/
						/*   event flags are 	*/
						/*   not set		*/
	if (p->p_evwait & p->p_evflg)
	  disp_act(p);
	else
	{ 
						/* good night, Mrs.	*/
						/*   Calabash, wherever	*/
						/*   you are 		*/
	  p->p_link = nrl;
	  nrl = p;
	}
}


	VOID
forker()
{
	REG FPD		*f;
	REG PD		*oldrl;
	FPD		g;
	LPFPD	rbufm1;
	
	oldrl = rlr;
	rlr = (PD *) -1;
	while(fpcnt)
	{
/* critical area	*/
	  cli();
	  fpcnt--;
	  f = &D.g_fpdx[fph++];
  					/* copy FPD so an interrupt	*/
					/*  doesn't overwrite it.	*/
	  LBCOPY(ADDR(&g), ADDR(f), sizeof(FPD) );
	  if (fph == NFORKS) 
	    fph = 0;
	  sti();
/* */
						/* see if recording	*/
	  if (gl_recd)
	  {
						  /* check for stop key	*/
	    if ( ((VOID *)g.f_code == (VOID *)kchange) &&
	         ((g.f_data & 0x0000ffffL) == KEYSTOP) )
	      gl_recd = FALSE;
						/* if still recording	*/
						/*   then handle event	*/
	    if (gl_recd)
	    {
						/* if its a time event &*/
						/*   previously recorded*/
						/*   was a time event	*/
						/*   then coalesce them	*/ 
						/*   else record the	*/
						/*   event		*/
		  rbufm1 = gl_rbuf - 1;

	      if ( (g.f_code == tchange) &&
	           (rbufm1->f_code == tchange) )
	      {
	        rbufm1->f_data += g.f_data;
	      }
	      else
	      {
	        LBCOPY(gl_rbuf, ADDR(f), sizeof(FPD));
	        gl_rbuf += sizeof(FPD);
	        gl_rlen--;
		gl_recd = gl_rlen;
	      }
	    }
	  }
	  (*g.f_code)(LLOWD(g.f_data), (LHIWD(g.f_data)));
	}
	rlr = oldrl;
}

	VOID
chkkbd()
{
	REG WORD	achar, kstat;
						/* poll keybd 		*/
	if (!gl_play)
	{
	  kstat = gsx_kstate();
	  achar = gsx_char();
	}
	if( !gl_fmactive )	/* WIndow controls not active during form_do */
	{
	 switch( (UWORD)achar )
	  {
	  case ALT_F4:		/* Close window ? */
	  case ALT_F5:		/* Full window ? */
	  case P_UP:		/* Page window vertical slider up ? */
	  case P_DOWN:		/* Page window vertical slider down ? */
	  case CTL_P_UP:	/* Page window horizontal slider left ? */
	  case CTL_P_DOWN:	/* Page window horizontal slider right ? */
#if 0 
RSF: let app do this
	  case HOME:		/* Move window vertical slider to top ? */ 
	  case END:		/* Move window vertical slider to bottom ? */
#endif

#if 0		
/* [JCE] Don't use TAB/BACKTAB for window management. Firstly, 
 *       programs (such as WORDPLUS.APP) may want them. Secondly,
 *       it can cause crashes since this AES can run other things 
 *       than ViewMAX. */
	  case TAB:
#endif
	    if( (kstat&0x04) && (achar != CTL_P_UP)
		    && (achar != CTL_P_DOWN) ) /* Let CTRL+I through as TAB */
	      break;
#if 0
 	  case BACKTAB:
#endif
	    kstate=kstat;
	    post_keybd( ctl_pd->p_cda, achar );
	    return;
	 }
	}
	
        if( gl_mntree != 0x0L && achar && 
	/* if not ctrl key and menu bar is active and F10 or F1 or */
	    ( achar == FUN_10 || achar == FUN_1 || /* an ALT short cut */
	    ( !(achar & 0x00FF) && find_shortcut( achar )!=NIL)) ){
		kstate=kstat;	/* Give F10 or shortcut to menu bar */
		post_keybd( ctl_pd->p_cda, achar );
	}
	else
	{
	  if (achar && (gl_mowner->p_cda->c_q.c_cnt >= KBD_SIZE))
	  {
	    achar = 0x0;			/* buffer overrun	*/
	    sound(TRUE, 880, 2);
	  }
	  if ( (achar) ||
	     (kstat != kstate) )
	  {
	    cli();
	    forkq(kchange, achar, kstat);
	    sti();
	  }
	}
}


	VOID
schedule()
{
	REG PD		*p;

						/* run through lists	*/
						/*   until someone is	*/
						/*   on the rlr or the	*/
						/*   fork list		*/
	do
	{
						/* poll the keyboard	*/
	  chkkbd();
						/* now move drl		*/
						/*   processes to rlr	*/
	  while ( drl )
	  {
	    drl = (p = drl) -> p_link;
	    disp_act(p);
	  }
	if (!rlr && !fpcnt) idle();		/* [JCE] Tell multitasking * 
						 * OS we're idle */

						/* check if there is	*/
						/*   something to run	*/
	} while ( !rlr && !fpcnt );
}


/************************************************************************/
/*									*/
/* dispatcher maintains all flags/regs so it looks like an rte to	*/
/*   the caller.							*/
/*   dispatch() = rte							*/
/*   rlr -> p_stat determines the action to perform on the process that	*/
/*		was in context						*/
/*   rlr -> p_uda -> dparam is used by the action routines		*/
/*									*/
/************************************************************************/

	VOID
disp()
{
	REG PD		*p;
/* savestate() is a machine (& compiler) dependent routine which:
 *	1) saves any flags that will be trashed by the TAS
 *	2) if (indisp) restore flags, return to dsptch caller
 *	3) otherwise 
 *		save machine state, 
 *		return to dsptch here 
*/

/* Don't put anything in here before the savestat! The structure 
 * of the stack is very delicate */

 	savestat(rlr->p_uda);

#if MULTIAPP
skip:
#endif
						/* take the process p	*/
						/*   off the ready list	*/
						/*   root		*/
	rlr = (p=rlr) -> p_link;
						/* based on the state	*/
						/*   of the process p	*/
						/*   do something	*/
	if (p->p_stat & WAITIN) 
	  mwait_act(p);
	else
	  disp_act(p);
						/* run through and 	*/
						/*   execute all the	*/
						/*   fork processes	*/
	do 
	{
	  if (fpcnt)
	    {
	    forker();
	    }
	  schedule();
	  
	} while (fpcnt);

/* switchto() is a machine dependent routine which:
 *	1) restores machine state
 *	2) clear "indisp" semaphore
 *	3) returns to appropriate address
 *		so we'll never return from this
 */
#if MULTIAPP
	if (gl_displock && (rlr != gl_displock))
	  goto skip;

	if (rlr->p_pid == 1)
	{
	  if (gl_mntree)
	    pr_load(gl_mnppd->p_pid);	
	}
	else
	  pr_load(rlr->p_pid);
#endif

	switchto(rlr->p_uda);
}

