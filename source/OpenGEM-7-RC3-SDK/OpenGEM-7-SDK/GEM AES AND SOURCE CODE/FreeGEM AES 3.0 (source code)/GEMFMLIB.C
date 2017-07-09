/*	GEMFMLIB.C	03/15/84 - 06/16/85	Gregg Morris		*/
/*	merge High C vers. w. 2.2 & 3.0		8/20/87		mdf	*/ 

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

#define FORWARD 0
#define BACKWARD 1
#define DEFLT 2

#define BACKSPACE 0x0E08			/* backspace		*/
#define SPACE 0x3920				/* ASCII <space>	*/
#define UP 0x4800				/* up arrow		*/
#define DOWN 0x5000				/* down arrow		*/
#define LEFT 0x4B00				/* left arrow		*/
#define RIGHT 0x4D00				/* right arrow		*/
#define DELETE 0x5300				/* keypad delete	*/
#define TAB 0x0F09				/* tab			*/
#define BACKTAB 0x0F00				/* backtab		*/
#define RETURN 0x1C0D				/* carriage return	*/


						/* in GEMRSLIB.C	*/
EXTERN BYTE		*rs_str();
/* --------------- added for metaware compiler --------------- */
EXTERN VOID   		wm_update();		/* in WMLIB.C		*/
EXTERN VOID		get_ctrl();		/* in INPUT.C 		*/
EXTERN VOID		get_mown();
EXTERN VOID 		fq();
EXTERN VOID 		ct_chgown();		/* in CTRL.C		*/
EXTERN VOID 		ct_mouse();
EXTERN VOID 		ob_change();
EXTERN WORD 		ob_find();
EXTERN WORD 		get_par();		/* in OBJOP.C		*/
EXTERN WORD		gr_watchbox();		/* in GRLIB.C		*/
EXTERN WORD 		ev_multi();
EXTERN VOID		gsx_sclip();		/* in GRAF.C		*/
EXTERN VOID		merge_str();		/* in OPTIMIZE.C	*/
EXTERN WORD 		fm_alert();		/* in FMALT.C		*/
/* ---------------------------------------------------------- */

EXTERN GRECT		gl_rfull;
EXTERN GRECT		gl_rscreen;


EXTERN THEGLO		D;

MLOCAL	WORD		ml_ocnt = 0;
MLOCAL	LPTREE		ml_mnhold;
MLOCAL	GRECT		ml_ctrl;
MLOCAL	PD		*ml_pmown;

MLOCAL WORD	ml_alrt[] = 
		{AL00CRT,AL01CRT,AL02CRT,AL03CRT,AL04CRT,AL05CRT};
MLOCAL WORD	ml_pwlv[] = 
		{0x0102,0x0102,0x0102,0x0101,0x0002,0x0001};



		
	VOID
fm_own(beg_ownit)
	WORD		beg_ownit;
{

	if (beg_ownit)
	{
	  wm_update(TRUE);
	  if (ml_ocnt == 0)
	  {
	    ml_mnhold = gl_mntree;
	    gl_mntree = 0x0L;
	    get_ctrl(&ml_ctrl);
	    get_mown(&ml_pmown);
	    ct_chgown(rlr, &gl_rscreen);
	  }
	  ml_ocnt++;
	}
	else
	{
	  ml_ocnt--;
	  if (ml_ocnt == 0)
	  {
	    ct_chgown(ml_pmown, &ml_ctrl);
	    gl_mntree = ml_mnhold;
	    menu_keys( gl_mntree, THEACTIVE );
	  }
	  wm_update(FALSE);
	}
}


/*
*	Routine to find the next editable text field, or a field that
*	is marked as a default return field.
*/
MLOCAL	WORD
find_obj(REG LPTREE tree, WORD start_obj, WORD which, WORD flag)
{
	REG WORD	obj, state, inc;
	WORD		theflag;

	obj = 0;
	inc = 1;
	switch(which)
	{
	  case BACKWARD:
		inc = -1;
						/* fall thru		*/
	  case FORWARD:
		obj = start_obj + inc;
		break;
	  case DEFLT:
		flag = DEFAULT;
		break;
	}

	while ( obj >= 0 )
	{
	  state = ob_fs(tree, obj, &theflag);
	  if ( !(theflag & HIDETREE) &&
	       !(state & DISABLED) )
	  {
	    if (theflag & flag)
	      return(obj);
	  }
	  if (theflag & LASTOB)
	    obj = -1;
	  else
	    obj += inc;
	}
	return(start_obj);
}

      	WORD
fm_inifld(LPTREE tree, WORD start_fld)
{
						/* position cursor on	*/
						/*   the starting field	*/
	if (start_fld == 0)
	  start_fld = find_obj(tree, 0, FORWARD, DEFAULT );/* was EDITABLE | SELECTABLE );*/
	return( start_fld );
}




	WORD
fm_keybd(tree, obj, pchar, pnew_obj)
	LPTREE		tree;
	WORD		obj;
	WORD		*pchar;
	WORD		*pnew_obj;
{
	WORD		direction;
						/* handle character	*/
	direction = -1;
	switch( *pchar )
	{
	  case RETURN:
		obj = 0;
		direction = DEFLT;
		break;
	  case BACKTAB:
	  case UP:
		direction = BACKWARD;
		break;
	  case TAB:
	  case DOWN:
	        direction = FORWARD;
		break;
	}

	if (direction != -1)
	{
	  *pchar = 0x0;
	  *pnew_obj = find_obj(tree, obj, direction, EDITABLE);
	  if ( (direction == DEFLT) &&
 	       (*pnew_obj != 0) )
	  {
	    ob_change(tree, *pnew_obj, 
		(tree+(*pnew_obj))->ob_state | SELECTED, TRUE);
	    return(FALSE);
	  }
	}

	return(TRUE);
}




	WORD
fm_button(tree, new_obj, clks, pnew_obj)
	REG LPTREE	tree;
	REG WORD	new_obj;
	WORD		clks;
	WORD		*pnew_obj;
{
	REG WORD	tobj;
	UWORD		orword;
	WORD		parent, state, flags;
	WORD		cont, junk, tstate, tflags;
	UWORD		rets[6];

	cont = TRUE;
	orword = 0x0;

	state = ob_fs(tree, new_obj, &flags);
						/* handle touchexit case*/
						/*   if double click,	*/
						/*   then set high bit	*/
	if (flags & TOUCHEXIT)
	{
	  if (clks == 2)
	    orword = 0x8000;
	  cont = FALSE;
	}

						/* handle selectable case*/
	if ( (flags & SELECTABLE) &&
	    !(state & DISABLED) ) 
	{
		 				/* if its a radio button*/
	  if (flags & RBUTTON)
	  {
						/* check siblings to	*/
						/*   find and turn off	*/
						/*   the old RBUTTON	*/
	    parent = get_par(tree, new_obj, &junk);
	    tobj = (tree+parent)->ob_head ;
	    while ( tobj != parent )
	    {
	      tstate = ob_fs(tree, tobj, &tflags);
	      if ( (tflags & RBUTTON) &&
		   ( (tstate & SELECTED) || 
		     (tobj == new_obj) ) )
	      {
	        if (tobj == new_obj)
		  state = tstate |= SELECTED;
		else
		  tstate &= ~SELECTED;
		ob_change(tree, tobj, tstate, TRUE);
	      }
	      tobj = (tree+tobj)->ob_next ;
	    }
	  }
	  else
	  {					/* turn on new object	*/
	    if ( gr_watchbox(tree, new_obj, state ^ SELECTED, state) )
	      state ^= SELECTED;
	  }
						/* if not touchexit 	*/
						/*   then wait for 	*/
						/*   button up		*/
	  if ( (cont) &&
	       (flags & (SELECTABLE | EDITABLE)) )
	    ev_button(1, 0x00001, 0x0000, (WORD *)rets);
	}
						/* see if this selection*/
						/*   gets us out	*/
	if ( (state & SELECTED) &&
	     (flags & EXIT) )
	  cont = FALSE;
						/* handle click on 	*/
						/*   another editable	*/
						/*   field		*/
	if ( (cont) &&
	     ( (flags & HIDETREE) ||
	       (state & DISABLED) ||
	       !(flags & EDITABLE) ) )
	  new_obj = 0;

	*pnew_obj = new_obj | orword;
	return( cont );
}




MLOCAL	VOID
field_cursor(LPTREE tree, WORD obj, WORD on_flag)
{
UWORD		state;
UWORD		flags;
OBJECT far *	pObj ;

	pObj = tree + obj ;
	state = pObj->ob_state ;
	flags = pObj->ob_flags ;
	
	if( !(flags & (SELECTABLE|EDITABLE)) || obj==0 || obj==NIL )
	  return;
	
	if( on_flag && !(state&HIGHLIGHTED) ){
	    if( flags & EXIT ){
	      flags|=DEFAULT;
	      pObj->ob_flags = flags ;
	    }
	    state|=HIGHLIGHTED;
	    ob_change(tree, obj, state, FALSE );
	    ob_highlight(tree, obj);	/*910404WHF*/
	}
	
	if( !on_flag && state&HIGHLIGHTED ){
	    state^=HIGHLIGHTED;
	    if( (flags & EXIT) && (flags & DEFAULT) ){
	      flags^=DEFAULT;
	      pObj->ob_flags = flags ;
            }
	    ob_highlight(tree, obj);		/*910404WHF*/
	    ob_change(tree, obj, state, FALSE );
	}
}




/*
*	ForM DO routine to allow the user to interactively fill out a 
*	form.  The cursor is placed at the starting field.  This routine
*	returns the object that caused the exit to occur
*/

	WORD
fm_do(tree, start_fld)
	REG LPTREE	tree;
	WORD		start_fld;
{
	WORD		edit_obj;
	WORD		next_obj;
	WORD		sav_def;	/* Save entry DEFAULT button */
	UWORD		which, junk, cont;
	WORD		idx, sjunk;
	WORD		rets[6];
	WORD		parent ;
	OBJECT far *	pEditObj ;
						/* grab ownership of 	*/
						/*   screen and mouse	*/
	fm_own(TRUE);
						/* flush keyboard	*/
	fq();
						/* set clip so we can	*/
						/*   draw chars, and	*/
						/*   invert buttons	*/
	gsx_sclip(&gl_rfull);
						/* determine which is 	*/
						/*   the starting field	*/
						/*   to edit		*/


	next_obj = fm_inifld(tree, start_fld);
	edit_obj = 0;
	pEditObj = tree + edit_obj ;
	sav_def = find_obj(tree, 0, FORWARD, DEFAULT );
	if( !( (tree+sav_def)->ob_flags & DEFAULT) ) 
	    sav_def=0; /* Save default button */
	  
	menu_keys( tree, 0 );		/* Get shortcuts for exit keys	*/
						/* interact with user	*/
	cont = TRUE;
	while(cont)
	{
						/* position cursor on	*/
						/*   the selected 	*/
						/*   editting field	*/
	  
	  if ( (next_obj != 0) && 
	       (edit_obj != next_obj) )
	  {
	    edit_obj = next_obj;
	    pEditObj = tree + edit_obj ;
  	    next_obj = 0;
	    field_cursor( tree, edit_obj, TRUE );
	    if( pEditObj->ob_flags & EDITABLE )
	      ob_edit(tree, edit_obj, 0, &idx, EDINIT);
	  }
	  

						/* wait for mouse or key */
	  which = ev_multi(MU_KEYBD | MU_BUTTON, (MOBLK *)NULLPTR, (MOBLK *)NULLPTR,
			 0x0L, 0x0002ff01L, 0x0L, (WORD*)rets);
						/* handle keyboard event*/

	  if (which & MU_KEYBD)
	  {
	    cont = TRUE;
	    switch( rets[4] )
	    {
	      case UP:
	        if( pEditObj->ob_flags & RBUTTON )
	        {
		  		parent = get_par( tree, edit_obj, (WORD*)&junk ) ;
		  		next_obj = find_obj( tree, (tree+parent)->ob_head,
								BACKWARD, EDITABLE | SELECTABLE );
		  		parent = get_par(tree, next_obj, (WORD*)&junk ) ;
		  		next_obj = find_obj( tree, (tree+parent)->ob_head,
								FORWARD, EDITABLE | SELECTABLE );
			}
	        else next_obj = edit_obj;
			next_obj = find_obj(tree, next_obj, BACKWARD, EDITABLE | SELECTABLE );
			if( next_obj == edit_obj  /* Wrap to default (last?) object */
		    	&& !( (tree+next_obj)->ob_flags & LASTOB ) ) /*!*/
		  	next_obj = find_obj(tree, next_obj, FORWARD, LASTOB );
		  break;
	      case BACKTAB:
	      case LEFT:
			if( rets[4]==BACKTAB 
			|| !(pEditObj->ob_flags & EDITABLE) 
			|| ((pEditObj->ob_flags & EDITABLE) && idx==0) )
				next_obj = find_obj(tree, edit_obj, BACKWARD, EDITABLE | SELECTABLE );
			if( next_obj == edit_obj  /* Wrap to default (last?) object */
		  	&& !( (tree+next_obj)->ob_flags & LASTOB) )
		    	next_obj = find_obj(tree, next_obj, FORWARD, LASTOB );
		  break;
	      case DOWN:
	        if( pEditObj->ob_flags & RBUTTON )
	        {
		  		parent = get_par( tree, edit_obj, (WORD*)&junk ) ;
		  		next_obj = find_obj( tree, (tree+parent)->ob_tail,
							FORWARD, EDITABLE | SELECTABLE );
			}
	      	else if( pEditObj->ob_flags & LASTOB ) 
		    	 next_obj = fm_inifld(tree, 0); /* Wrap to first object */
		  	else next_obj = find_obj(tree, edit_obj, FORWARD, EDITABLE | SELECTABLE );
	      break;
	      case TAB:
	      case RIGHT:
			if( pEditObj->ob_flags & LASTOB ) 
			     next_obj = fm_inifld(tree, 0); /* Wrap to first object */
			else if( rets[4]==TAB 
		  		 || !(pEditObj->ob_flags & EDITABLE) 
			     || ( (pEditObj->ob_flags & EDITABLE) 
				 && idx >= strlen(&D.g_rawstr[0])) )
	      	    	next_obj = find_obj(tree, edit_obj, FORWARD, EDITABLE | SELECTABLE );
			break;
	      case P_DOWN:
			next_obj = find_obj(tree, next_obj, FORWARD, LASTOB );
			next_obj = find_obj( tree, next_obj, BACKWARD, SCROLLER );
	        cont = fm_button( tree, next_obj, 1, &next_obj );
		  break;
		  
	      case P_UP:
			next_obj = find_obj( tree, 0, FORWARD, SCROLLER );
	        cont = fm_button( tree, next_obj, 1, &next_obj );
		  break;
	      case RETURN:
	        if( pEditObj->ob_flags & EXIT )
		 	     next_obj = edit_obj; 
			else next_obj = find_obj(tree, edit_obj, DEFLT, SELECTABLE );

			if( (tree+next_obj)->ob_flags & EXIT )
			{
		  /*field_cursor( tree, edit_obj, FALSE ); 910507WHF*/
		  	if( next_obj != edit_obj )
		  	  cont = fm_button(tree, edit_obj, 1, (WORD*)&junk);
	      	  edit_obj = next_obj;
		  	pEditObj = tree + edit_obj ;
	          cont = fm_button(tree, next_obj, 1, &next_obj);
		}
		break;
	      case ESC:
	        junk = find_obj(tree, 0, FORWARD, ESCCANCEL );
		if( (tree+junk)->ob_flags & ESCCANCEL ){
		  field_cursor( tree, edit_obj, FALSE );  
		  next_obj = edit_obj = junk;
		  pEditObj = tree + edit_obj ;
	          cont = fm_button(tree, next_obj, 1, &next_obj);
		}
		break;
	      case SPACE:
	        next_obj = edit_obj;
#if 00000
		/*910404WHF*/
	        if( pEditObj->ob_flags & EXIT )
#endif
		  field_cursor( tree, edit_obj, FALSE );  
		cont = fm_button(tree, next_obj, 1, &next_obj);
		field_cursor( tree, edit_obj, TRUE );  
		break;
	      default:
	        if( (rets[4]&0xFF) == 0 ){	/* ALT+Letter ? */
	  	  sjunk = find_shortcut( rets[4] ); 
		  if( sjunk != NIL ){	/* If so can we match it ? */
	            field_cursor( tree, edit_obj, FALSE );  
		    next_obj = edit_obj = sjunk;
		    pEditObj = tree + edit_obj ;
	            cont = fm_button(tree, next_obj, 1, &next_obj);
	          }
		}
		break;
	    }
	    if ( rets[4] && (pEditObj->ob_flags & EDITABLE) )
	      ob_edit(tree, edit_obj, rets[4], &idx, EDCHAR);
	  }
	  					/* handle button event	*/
	  if (which & MU_BUTTON)
	  {
	    next_obj = ob_find(tree, ROOT, MAX_DEPTH, rets[0], rets[1]);
	    if (next_obj == NIL)
	    {
	      sound(TRUE, 440, 2);
	      next_obj = 0;
	    }
	    else
	    {
	      field_cursor( tree, edit_obj, FALSE );  
	      cont = fm_button(tree, next_obj, rets[5], (WORD*)&junk);
	      next_obj |= (junk&0x8000);	/* Register a double click */
	    }
	  }
					/* handle end of field	*/
					/*   clean up		*/
	 if ( (!cont) || 
	      ( (next_obj != 0) && 
	        (next_obj != edit_obj) ) )

	  {
	    if( pEditObj->ob_flags & EDITABLE )
	      ob_edit(tree, edit_obj, 0, &idx, EDEND);
	    field_cursor( tree, edit_obj, FALSE );
	  }

	}
	
						/* give up mouse and	*/
						/*   screen ownership	*/
	field_cursor( tree, edit_obj, FALSE );
	field_cursor( tree, next_obj, FALSE );
	
	if( sav_def )	/* Reinstate entry default button */
	  (tree+sav_def)->ob_flags |= DEFAULT ;
	        
						/* Turn off highlights */

	fm_own(FALSE);
						/* return exit object	*/
	return(next_obj);
}


/*
*	Form DIALogue routine to handle visual effects of drawing and
*	undrawing a dialogue
*/
	WORD 
fm_dial(fmd_type, pt, ipt)
	REG WORD	fmd_type;
	REG GRECT	*pt;
	GRECT 		*ipt;	/* [JCE] ipt is the rectangle for zoom effects */
						/*      NULL for no zoom effect */
{
						/* adjust tree position	*/
	gsx_sclip(&gl_rscreen);
	switch( fmd_type )
	{
	  case FMD_START:
	  	gl_fmactive = TRUE;
						/* grab screen sync or	*/
						/*   some other mutual	*/
						/*   exclusion method	*/
		break;

#if GROWBOX
	  case FMD_GROW:	/* Growing box */
		if (ipt) gr_growbox(ipt, pt);
		break;
		
	  case FMD_SHRINK:	/* Shrinking box */
		if (ipt) gr_shrinkbox(ipt, pt);
		break;
#endif
		
	  case FMD_FINISH:
						/* update certain 	*/
						/*   portion of the	*/
						/*   screen		*/
		gl_fmactive = FALSE;
		w_drawdesk(pt);
		w_update(0, pt, 0, FALSE, TRUE);
		break;			/*          ^---- Force full redraw of borders */
	}
	return(TRUE);
}

	WORD
fm_show(string, pwd, level)
	WORD		string;
	UWORD		*pwd;
	WORD		level;
{
	BYTE		*alert;
	LPBYTE		ad_alert;

	ad_alert = ADDR( alert = rs_str(string) );
	if (pwd)
	{
	  merge_str(&D.g_loc2[0], alert, pwd);
	  ad_alert = ad_g2loc;
	}
	return( fm_alert(level, ad_alert) );
}


				/* TRO 9/20/84	- entered from dosif	*/
				/* when a DOS error occurs		*/
	WORD
eralert(n, d)	
	WORD		n;		/* n = alert #, 0-5 	*/	
	WORD		d;		/* d = drive code	*/
{
	WORD		ret, level;
	WORD		*drive_let;
	WORD		**pwd;
	BYTE		drive_a[2];

	drive_a[0] = 'A' + d;
	drive_a[1] = 0;
	drive_let = (WORD *) &drive_a[0];

	level = (ml_pwlv[n] & 0x00FF);
	level |= 0x0100;	/* All alerts have ESCCANCEL button 1 */
	pwd = (ml_pwlv[n] & 0xFF00) ? &drive_let : 0;

	ct_mouse(TRUE);
	ret = fm_show(ml_alrt[n], (UWORD *)pwd, level);
	ct_mouse(FALSE);

	return (ret != 1);
}


	WORD
fm_error(n)
	WORD		 n;		/* n = dos error number */
{
	WORD		ret, string;

	if (n > 63)
	  return(FALSE);

	ret = 1;

	switch (n)
	{
	  case 2:
	  case 18:	
	  case 3:
		string = AL18ERR;
		break;
	  case 4:
		string = AL04ERR;
		break;
	  case 5:
		string = AL05ERR;
		break;
	  case 15:
		string = AL15ERR;
		ret |= 0x100;	/* Enable ESC to cancel */
		break;
	  case 16:
		string = AL16ERR;
		break;
	  case 8:
	  case 10:
	  case 11:
		string = AL08ERR;
		break;
	  default:
		string = ALXXERR;
		ret |= 0x100;	/* Enable ESC to cancel */
		break;
	}

	ret = fm_show(string, ((string == ALXXERR) ? (UWORD *)&n : (UWORD *)0), 1);

	return (ret != 1);
}



