/*******************************************************************/
/*                                                                 */
/*      Copyright 1999, Caldera Thin Clients, Inc.                 */
/*      This software is licenced under the GNU Public License.    */
/*      Please see LICENSE.TXT for further information.            */
/*                                                                 */
/*                 Historical Copyright                            */
/*******************************************************************/
/* Copyright (c) 1987 Digital Research Inc.		           */
/* The software contained in this listing is proprietary to        */
/* Digital Research Inc., Pacific Grove, California and is         */
/* covered by U.S. and other copyright protection.  Unauthorized   */
/* copying, adaptation, distribution, use or display is prohibited */
/* and may be subject to civil and criminal penalties.  Disclosure */
/* to others is prohibited.  For the terms and conditions of soft- */
/* ware code use refer to the appropriate Digital Research         */
/* license agreement.						   */
/*******************************************************************/

#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "evntlib.h"
#include "windlib.h"
#include "appllib.h"
#include "objclib.h"
#include "gembind.h"     
#include "xgembind.h"
#include "dosbind.h"
#include "vdibind.h"

#include "output.h"
#include "odefs.h"
#include "ovar0.h"

EXTERN VOID	do_info() ;		/* odialog.c */
EXTERN VOID	do_shrtinfo() ;

EXTERN VOID	upd_menu() ;		/* owindow.c */
EXTERN VOID	do_wm_fulled() ;
EXTERN VOID	do_arrowed() ;
EXTERN VOID	send_redraw() ;
EXTERN VOID	do_scrolled() ;
EXTERN VOID	do_wm_sized() ;
EXTERN VOID	do_wm_moved() ;

EXTERN WORD	pos() ;			/* outility.c */
EXTERN WORD	f_alert() ;
EXTERN VOID	pad_n_itoa() ;
EXTERN WORD	atoi() ;
EXTERN VOID	del_fname() ;
EXTERN BOOLEAN	extension() ;

EXTERN BOOLEAN	rc_intersect() ;	/* ooptimz.c */
EXTERN VOID	r_set() ;
EXTERN UWORD	inside() ;
EXTERN VOID	strcpy() ;
EXTERN VOID	strcat() ;

EXTERN VOID	tray_fmenu() ;		/* otrayutl.c */
EXTERN VOID	dial_opt() ;
EXTERN VOID	slct_slot() ;
EXTERN VOID	move_entry() ;
EXTERN VOID	deslct_slots() ;
EXTERN VOID	rub_slct() ;
EXTERN WORD	get_trob() ;
EXTERN WORD	rd_btch() ;
EXTERN VOID	add_name() ;
EXTERN VOID	do_editmenu() ;

EXTERN VOID	OUTCMCL() ;
EXTERN VOID	OUTDISP() ;
EXTERN WORD	OUTINIT() ;

/*--------------------------------------------------------------*/
    VOID
_abort()
{
    if ( *invoker )
	{
#if DEBUG
	show_hex( invoker ) ;
	show_hex( cmd_tail ) ;
#endif
    	shel_write( TRUE, TRUE, TRUE, ADDR( invoker ), ADDR( cmd_tail ) ) ;
	}
    graf_shrinkbox( (gl_win.g_x + (gl_win.g_w/2) ), 
			(gl_win.g_y + (gl_win.g_h/2) ), 1, 1,
			gl_win.g_x, gl_win.g_y, gl_win.g_w, gl_win.g_h ) ;
    wind_close( w_handle ) ;
    wind_delete( w_handle ) ;
    rsrc_free() ;
    v_clsvwk( scr_hndl ) ;
    menu_bar( gl_menu, FALSE ) ;
#if I8086
    dos_sint( 0x23, ad_break ) ;    
#endif
    appl_exit() ;
} /* _abort */


/*--------------------------------------------------------------*/
	VOID
do_ap_write( msg_num ) 
WORD	msg_num ;
{
	gl_smsg[ 0 ] = msg_num ;
	gl_smsg[ 1 ] = global.ap_id ;
	gl_smsg[ 2 ] = 0 ;
	appl_write( global.ap_id, sizeof( gl_smsg ), ADDR( gl_smsg ) ) ;
} /* do_ap_write */


/*--------------------------------------------------------------*/
    VOID
do_strt()
{
    make_fit = ( DEV_TYPE & ( SCRN_TYPE | CMRA_TYPE ) ) ? 
    		TRUE : ( PRNT_PREFS.scale != TRUE_SZ ) ;
    do_ap_write( PRT_MSG ) ; 
} /* do_strt */

/*--------------------------------------------------------------*/
	VOID 
do_deskmenu( item )
	WORD		item ;
{
	switch( item )
	{
	  case ABOUTITM:
		do_info( OUTINFO, OKINFO ) ;
		break ;

	}
} /* do_deskmenu */


/*--------------------------------------------------------------*/
	WORD
do_filemenu( item )
	WORD		item ;
{
WORD	ret_but ;		/* button picked in alert, starts at 1 */

    switch ( item )
	{
	case NEWITEM:		
	case OPENITEM:	
	case QUITITEM:	
	case TOAPPITM:	if ( *cur_file && cur_opts.confirm && modified && 
					( next_slot != 0 ) )
			    {
			    ret_but = f_alert( UNSVDCHG, 1 ) ;
			    if ( ret_but == 3 )		/* Cancel button */
    	    			return( FALSE ) ;
			    if ( ret_but == 2 )		/* Save button */
				tray_fmenu( SAVEITEM ) ;
    			    }
			break ;
	default :	break ;
	}
    switch ( item )
	{
	default : 
		tray_fmenu( item ) ;	
		break ;
	case QUITITEM:	
		*invoker = NULL ;
	case TOAPPITM:
		return( TRUE ) ;
	}
    return( FALSE ) ;
} /* do_filemenu */

/*--------------------------------------------------------------*/
	VOID
do_optimenu( item )
	WORD		item;
{
    switch ( item )
        {
	case SHRTITM0 : do_shrtinfo() ;
			break ;
	case FILMITEM : if ( !( DEV_TYPE & CMRA_TYPE ) )
	  		    break ;
			if ( film_names )
			    {
	    	    	    graf_mouse( MO_HGLASS, 0x0L ) ;
		    	    OUTCMCL( FILMPREF ) ; 
	    	    	    graf_mouse( MO_ARROW, 0x0L ) ;
		    	    }
			film_names = FALSE ;
		 	dial_opt( FILMPREF ) ;	
		    	break ;
	case COLRITEM : if ( !( DEV_TYPE & CMRA_TYPE ) )
	  		    break ;
			OUTCMCL( COLRPREF ) ;
			break;
	default : dial_opt( item ) ;
		  break ;
    } /* switch */
} /* do_optimenu */

/*--------------------------------------------------------------*/
	VOID
hndl_timer( flags, down, mx, my )
	WORD *flags, *down ;
	WORD mx, my ;
{      
GRECT r1, r2 ;

    if ( tmhi )   
	{   /* long timer has gone off- reset copy cntr to 1 like a Xerox */
	if ( atoi( get_teptext( cntl_tr, COPYSTXT ) ) > 1 )
	    {
	    pad_n_itoa( 2, 1, get_teptext( cntl_tr, COPYSTXT ) ) ;
	    objc_draw( cntl_tr, COPYITEM, MAX_DEPTH, gl_win.g_x, gl_win.g_y,
						gl_win.g_w, gl_win.g_h ) ;
	    v_sound( scr_hndl, 440, 30 ) ;	/* an 'A' for 3 seconds */
	    }
	tmhi = tmlo = 0 ;
	*flags &= ~MU_TIMER ;
	return ;
	}
    else if ( gl_ob >=0 && ( get_state( tray_tr, gl_ob ) & SELECTED ) )
	{
	slct_slot( gl_ob, 0 ) ;	
	graf_mouse( MO_FTHAND, 0x0L ) ;
	r_set( &r2, gl_xob, gl_yob, gl_wob, gl_hob ) ;
	rc_intersect( &gl_work, &r2 ) ;
	graf_dragbox( r2.g_w, r2.g_h, r2.g_x, r2.g_y,
		         gl_work.g_x, gl_work.g_y, gl_work.g_w, gl_work.g_h, 
		         &r1.g_x, &r1.g_y ) ;
	graf_mouse( MO_ARROW, 0x0L ) ;
	move_entry( r1.g_x, r1.g_y ) ;	
	}
    else
	{
	r1.g_x = mx ;
	r1.g_y = my ;
	deslct_slots() ;	
	graf_rubbox( r1.g_x, r1.g_y, 4, 4, &r1.g_w, &r1.g_h ) ;
	rub_slct( r1.g_x, r1.g_y, r1.g_w, r1.g_h ) ;
	}
    *flags &= ~MU_TIMER ;
    *down = TRUE ;
} /* hndl_timer */

/*--------------------------------------------------------------*/
	VOID
cntl_bdown( obj )
	WORD obj ;
{
WORD 	j ;

    if ( obj == STRTITEM || obj == STRTICON || obj == STRTSTR0 ) 
	{
/*	if ( SELECTABLE & get_flags( cntl_tr, STRTITEM ) ) */
    	if ( next_slot )	/* same value and less code */
	    {
	    evnt_button( 1, 1, 0, &j, &j, &j, &j ) ; /* wait for b up */    
	    deslct_slots() ;
	    upd_menu() ;
	    tggl_state( cntl_tr, STRTITEM, gl_win.g_x, gl_work.g_y,
			    gl_win.g_w, gl_work.g_h, SELECTED, TRUE ) ;
	    do_strt() ;
	    }
	else
	    f_alert( NOSTART, 1 ) ;	/* must add a name to the list */
	}
    else if ( obj == ADDITEM || obj == DELEITEM || obj == DUPLITEM )
	{
    	if ( ( SELECTABLE & get_flags( cntl_tr, obj ) ) ) 
	    {
	    evnt_button( 1, 1, 0, &j, &j, &j, &j ) ; /* wait for button up */ 
	    hndl_menu( EDITMENU, obj ) ;
	    }
	}
    else if ( obj == COPYITEM || obj == COPYICON || 
				    obj == COPYSTXT || obj == COPYSTR0 )
	{
	edit_copies = TRUE ;
	objc_edit( cntl_tr, COPYSTXT, 0, &copies_idx, ED_INIT ) ;
	}
} /* cntl_bdown */

/*--------------------------------------------------------------*/
    VOID
set_prefmenu()
{
OBJECT	*ob_ptr ;
WORD	ii, pref_menu ;

    switch ( dvc[ cur_opts.cur_dvc ].type )
        {
	case SCRN_TYPE : pref_menu = SPRFMENU ;
			 break ;
	case PLOT_TYPE :
	case PRNT_TYPE : pref_menu = PPRFMENU ;
			 break ;
	case CMRA_TYPE : pref_menu = CPRFMENU ;
			 break ;
	}
    for ( ii = FIRST_MENU; ii <= LAST_MENU; ii++ )
        {
	ob_ptr = OB_PTR( gl_menu, ii ) ;
	ob_ptr->ob_flags = ( pref_menu == ii ) ? NORMAL : HIDETREE ;
	}
} /* set_prefmenu() */

/*--------------------------------------------------------------*/
	VOID
dev_bdown( obj )
WORD	obj ;
{
WORD	j ;
GRECT	cur_obj, new_obj ;

    evnt_button( 1, 1, 0, &j, &j, &j, &j ) ;  /* wait for button up */
    if ( obj > DEVCROOT )	/* select new choice */
	{		/* de-select current choice */
	set_state( dev_tree, cur_opts.cur_dvc + DEV01ICN, NORMAL ) ;
	get_obloc( dev_tree, cur_opts.cur_dvc + DEV01ICN, &cur_obj.g_x, &cur_obj.g_y, &cur_obj.g_w, &cur_obj.g_h ) ;
	do_redraw( dev_tree, 0, cur_obj.g_x, cur_obj.g_y, cur_obj.g_w, cur_obj.g_h ) ;
		/* select new choice */
	set_state( dev_tree, obj, SELECTED ) ;
	get_obloc( dev_tree, obj, &new_obj.g_x, &new_obj.g_y, &new_obj.g_w, &new_obj.g_h ) ;
	do_redraw( dev_tree, 0, new_obj.g_x, new_obj.g_y, new_obj.g_w, new_obj.g_h ) ;
	cur_opts.cur_dvc = obj - DEV01ICN ;
	}
    set_prefmenu() ;
} /* dev_bdown */

/*--------------------------------------------------------------*/
	VOID
hndl_bdown( flags, down, x, y )
	WORD *flags, *down ;
	WORD x, y ;
{
WORD	obj ;

    obj = objc_find( cntl_tr, ROOT, MAX_DEPTH, x, y ) ;
    if ( obj > 0 )
	cntl_bdown( obj ) ;
    else if ( !inside( x, y, &gl_win ) )	/* check for icon action on the desktop */
	{
    	obj = objc_find( dev_tree, DEVCROOT, MAX_DEPTH, x, y ) ;
	dev_bdown( obj ) ;
	}
    else    /* set up timer for rubber select or single select in name tray */
	{
    	gl_ob = get_trob( x, y ) ;	
    	if ( gl_ob >= ROOT ) 
	    get_obloc( tray_tr, gl_ob, &gl_xob, &gl_yob, &gl_wob, &gl_hob ) ;
        tmhi = 0 ;
    	tmlo = 100 ;		/* tenth of a second */
    	*flags |= MU_TIMER ;
    	*down = FALSE ;
	}
} /* hndl_bdown */

/*--------------------------------------------------------------*/
	VOID
hndl_button( flags, down, x, y, kstate )
	WORD *flags, *down ;
	WORD x, y, kstate ;
{
    if ( *down )
	{
	hndl_bdown( flags, down, x, y ) ;
	}
    else
	{
	if ( gl_ob >= ROOT )
	    slct_slot( gl_ob, kstate ) ;	
	else
	    deslct_slots() ;
	upd_menu() ;
	*flags &= ~MU_TIMER ;
	*down = TRUE ;
	}
} /* hndl_button */

/*--------------------------------------------------------------*/
	BOOLEAN
hndl_keybd( key )
WORD		key ;
{
WORD	ii, title, item, done ;
BOOLEAN	enabled ;

    done = FALSE ;
    item = 0 ;
    for ( ii = 0; ii <= LASTITEM - FRSTITEM; ii++ )
	if ( key == key_map[ ii ] )
	   item = FRSTITEM + ii ;
    if ( ! item )
	{
	if ( edit_copies )
	    {
	    if ( key != CRETURN )
	    	objc_edit( cntl_tr, COPYSTXT, key, &copies_idx, ED_CHAR ) ;
	    else
		{
		edit_copies = FALSE ;
	    	objc_edit( cntl_tr, COPYSTXT, key, &copies_idx, ED_END ) ;
		}
	    }
	return( done ) ;
	}
    if ( edit_copies )
	{
	edit_copies = FALSE ;
	objc_edit( cntl_tr, COPYSTXT, 0, &copies_idx, ED_END ) ;
	}
    if ( item == STRTITEM )
	{
	if ( next_slot )	
	    {
	    deslct_slots() ;
	    upd_menu() ;
	    tggl_state( cntl_tr, STRTITEM, gl_win.g_x, gl_work.g_y,
				gl_win.g_w, gl_work.g_h, SELECTED, TRUE ) ;
	    do_strt() ;
	    }
	else
	    f_alert( NOSTART, 1 ) ;		/* must add a name to the list */
	return( done ) ;
	}
    if ( item >= SCRNITEM )
        title = FIRST_MENU ;
    else if ( item >= SHRTITM0 )
	title = GPRFMENU ;
    else if ( item >= NEWITEM )
	title = FILEMENU ;
    else
	title = EDITMENU ;
    if ( title == EDITMENU )
	enabled = ( SELECTABLE & get_flags( cntl_tr, item ) ) ;
    else
	enabled = ( ! ( DISABLED & get_state( gl_menu, item ) ) ) ; 
    if ( enabled )
	done = hndl_menu( title, item ) ;
    return( done ) ;
} /* hndl_keybd */

/*--------------------------------------------------------------*/
	WORD
hndl_menu( title, item )
	WORD		title ;
	WORD		item ;
{
	WORD		done ;

	done = FALSE ;
	switch( title )
	{
	  case DESKMENU:
		do_deskmenu( item ) ;
		break ;
	  case FILEMENU:
		done = do_filemenu( item ) ;
		break ;
	  case GPRFMENU:
	  case SPRFMENU:
	  case PPRFMENU:
	  case CPRFMENU:
		do_optimenu( item ) ;
		break ;
	  case EDITMENU:
		tggl_state( cntl_tr, item, gl_win.g_x, gl_work.g_y, 
				gl_win.g_w, gl_work.g_h, SELECTED, TRUE ) ;
		do_editmenu( item ) ;
		if ( !modified ) /* error state out of add_entry() */
		    {
		    tggl_state( cntl_tr, item, gl_win.g_x, gl_work.g_y, 
				gl_win.g_w, gl_work.g_h, SELECTED, TRUE ) ;
		    upd_menu() ;
	 	    }
		return( FALSE ) ;
	}
	if ( !done )
	  menu_tnormal( gl_menu, title, TRUE ) ;
	return( done ) ;
} /* hndl_menu */


/*--------------------------------------------------------------*/
	WORD
hdle_mesag()
{
WORD	done ;
GRECT	r ;

    done = FALSE ;
    switch( gl_rmsg[ 0 ] )
	{
	case MN_SELECTED:
		done = hndl_menu( gl_rmsg[ 3 ], gl_rmsg[ 4 ] ) ;
		break ;
	case WM_REDRAW:
		do_redraw( tray_tr, gl_rmsg[ 3 ], gl_rmsg[ 4 ], gl_rmsg[ 5 ], gl_rmsg[ 6 ], gl_rmsg[ 7 ] ) ;
		do_redraw( cntl_tr, gl_rmsg[ 3 ], gl_rmsg[ 4 ], gl_rmsg[ 5 ], gl_rmsg[ 6 ], gl_rmsg[ 7 ] ) ;
		break ;
	case WM_TOPPED:
		wind_set( gl_rmsg[ 3 ], WF_TOP, 0, 0, 0, 0 ) ;
		break ;
	case WM_FULLED:
		if ( gl_rmsg[ 3 ] == w_handle )
		    do_wm_fulled() ;
		break ;
	case WM_ARROWED:
    		if ( gl_rmsg[ 3 ] == w_handle )
		    {
		    do_arrowed( gl_rmsg[ 4 ] ) ;
		    send_redraw( &gl_work.g_x ) ;
	    	    }
		break ;
	case WM_HSLID:
	case WM_VSLID:
    		if ( gl_rmsg[ 3 ] == w_handle )
		    {
		    do_scrolled( gl_rmsg[ 0 ], gl_rmsg[ 4 ] ) ;
    		    send_redraw( &gl_work.g_x ) ;
	   	    }
		break ;
	case WM_SIZED:
    		if ( gl_rmsg[ 3 ] == w_handle )
		    {
		    r_set( &r, gl_rmsg[ 4 ], gl_rmsg[ 5 ], gl_rmsg[ 6 ], gl_rmsg[ 7 ] ) ;
		    do_wm_sized( &r ) ;
		    }
		break ;
	case WM_MOVED:
    		if ( gl_rmsg[ 3 ] == w_handle )
		    {
		    r_set( &r, gl_rmsg[ 4 ], gl_rmsg[ 5 ], gl_rmsg[ 6 ], gl_rmsg[ 7 ] ) ;
		    do_wm_moved( &r ) ;
		    }
		break ;
	case PRT_MSG:
		if ( next_slot )
		    OUTDISP() ;	/* OUTDISP overlay */
		set_state( cntl_tr, STRTITEM, NORMAL ) ;
		r_set( &r, gl_win.g_x, gl_work.g_y, gl_win.g_w, gl_work.g_h ) ;
  		send_redraw( &r.g_x ) ;

		break ;
	case STRT_MSG :
	    	tggl_state( cntl_tr, STRTITEM, gl_win.g_x, gl_work.g_y, 
				gl_win.g_w, gl_work.g_h, SELECTED, TRUE ) ;
		do_strt() ;
    		do_ap_write( QUIT_MSG ) ; 
		break ;
	case QUIT_MSG :
		if ( *invoker )
		    done = hndl_menu( FILEMENU, TOAPPITM ) ;
		else
		    done = hndl_menu( FILEMENU, QUITITEM ) ;
		break ;
	}
    gl_rmsg[ 0 ] = 0 ;
    return( done ) ;
} /* hdle_mesage */

/*--------------------------------------------------------------*/
    BOOLEAN
initialize()
{ 
BYTE    *start, *end, *next, null ;
WORD	ok, how_invkd ;

    ok = FALSE ;
    how_invkd = OUTINIT() ;
    if ( how_invkd == (-100) )	/* panic flag for no resource file */
	return( FALSE ) ;
    if ( how_invkd == 1 )
	ok = rd_btch( invkr_file ) ;
    else if ( how_invkd == 2 )
	{
	ok = TRUE ;
    	tray_fmenu( NEWITEM ) ;	
	null = NULL ;
	next = start = invkr_file ;
	while ( *next )
	    {
	    end = start ;
	    next = &null ;
	    while ( ( *end != COMMA ) && ( *end != SP ) && ( *end != NULL ) )
		end++ ;
	    if ( ( *end == COMMA ) || ( *end == SP ) )
		{
		*end = NULL ;
		next = end + 1 ;
		}
	    if ( *start ) 
		{
		if ( pos( BSLASH, start ) < 0 )
		    strcpy( lis_dir, names[ next_slot ] ) ;
		strcat( start, names[ next_slot ] ) ;
	      	if ( dos_sfirst( ADDR( names[ next_slot ] ), 0x0 ) )
		    {
		    --num_empty ;
		    del_fname( names[ next_slot ], lis_dir ) ;
		    strcpy( lis_dir, gsx_wc ) ;			/* cur dir */
		    strcat( GEM_EXT, gsx_wc ) ;			/* add *.GEM*/
		    gsx_wc[ pos( PERIOD, gsx_wc ) ] = NULL ;	/* rm .GEM */
		    extension( names[ next_slot ], gsx_wc ) ;   /* new ext. */
		    add_name() ;
		    }
		else
		    *names[ next_slot ] = NULL ;
		} /* if */
	    start = next ;
	    } /* while */
	e_slot = next_slot ; 
	} /* if how_invkd == 2 */
    if ( !ok )
	tray_fmenu( NEWITEM ) ;	
    else if ( ( how_invkd == 1 ) || ( how_invkd == 2 ) )
	{
	if ( cur_opts.auto_disp )
	    do_ap_write( STRT_MSG ) ;
	}
    return( TRUE ) ;
} /* initialize */


/*--------------------------------------------------------------*/
    VOID
#if I8086
gemain()
#endif
#if MC68K
main()
#endif
{
WORD		done, mx, my, but, kstate ;
WORD		ev_which, rets[ 6 ] ;
WORD		clicks, mask, down ;
    
    if ( ! initialize() )
	{
	_abort() ;
	return ;
	}
    flags = MU_BUTTON | MU_MESAG | MU_KEYBD ;
    clicks = 2 ;	/* single or double clicks */
    mask = 1 ;		/* wait for leftmost mouse button */
    down = TRUE ;	/* wait for button down */
    tmhi = 0 ;		
    tmlo = 0 ;
    done = FALSE ;
    while ( !done ) 	/* loop handling user input until done	*/
	{
	ev_which = evnt_multi( flags, clicks, mask, down, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				ADDR( gl_rmsg ), tmlo, tmhi,
				&rets[ 0 ], &rets[ 1 ], &rets[ 2 ], 
				&rets[ 3 ], &rets[ 4 ], &rets[ 5 ] ) ;
	wind_update( 1 ) ;
	if ( edit_copies && (ev_which & (MU_BUTTON | MU_TIMER | MU_MESAG ) ) )
	    {	/* if editing cpy cntr and any action BUT keybd, then stop */
	    edit_copies = FALSE ;
	    objc_edit( cntl_tr, COPYSTXT, 0, &copies_idx, ED_END ) ;
	    }
	if ( ev_which & MU_MESAG )
	    done = hdle_mesag() ;
	if ( ev_which & MU_KEYBD )
	    done = hndl_keybd( rets[ 4 ] ) ;
	if ( ev_which & MU_BUTTON )
	    {
	    graf_mkstate( &mx, &my, &but, &kstate ) ;
	    hndl_button( &flags, &down, mx, my, kstate ) ;
	    }
	if ( ev_which & MU_TIMER )
	    hndl_timer( &flags, &down, mx, my ) ;
        else if ( down /*not waiting for up action in rubber: reset timer*/
		&& atoi( get_teptext( cntl_tr, COPYSTXT ) ) > 1 )
	    {
	    tmhi = (UWORD)( 120000L >> 16 ) ;
	    tmlo = (UWORD)( 120000L & 0x0000FFFFL ) ;
	    flags |= MU_TIMER ;
	    }

    	wind_update( 0 ) ;
	}
    _abort() ;
} /* main */



/* end of outmain.c */