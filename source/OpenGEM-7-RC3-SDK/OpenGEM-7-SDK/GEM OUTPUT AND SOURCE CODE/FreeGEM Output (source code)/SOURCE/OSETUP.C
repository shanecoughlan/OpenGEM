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
/* 28/12/2000: Added version check to stop crashing on GEM/1 or 2. (orudge) */
#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "taddr.h"
#include "windlib.h"
#include "rsrclib.h"
#include "gembind.h"
#include "dosbind.h"
#include "xgembind.h"
#include "vdibind.h"

#include "output.h"
#include "odefs.h"
#include "ovar.h"

EXTERN WORD	align_h() ;			/* outility.c */
EXTERN VOID	movb() ;
EXTERN VOID	pad_n_itoa() ;
EXTERN VOID	del_fname() ;
EXTERN VOID	del_path() ;
EXTERN WORD	pos() ;
EXTERN WORD	extension() ;
EXTERN VOID	vopen() ;

EXTERN VOID	b_stuff() ;			/* util86.asm */

EXTERN VOID	strcpy() ;			/* ooptimz.c */
EXTERN VOID	strcat() ;
EXTERN WORD	strlen() ;
EXTERN WORD	min() ;
EXTERN WORD	max() ;
EXTERN BOOLEAN	strcmp() ;

EXTERN VOID	upd_tray() ;			/* owindow.c */
EXTERN VOID	size_slots() ;
EXTERN VOID	set_sliders() ;
EXTERN VOID	def_txt_atts() ;

EXTERN BOOLEAN	setup_rsc() ;			/* orscinit.c */

EXTERN VOID	set_prefmenu() ;		/* omain.c */

EXTERN VOID	make_def() ;			/* ooptions.c */
EXTERN BOOLEAN	check_path() ;		

BOOLEAN FreeGEM = FALSE;

/*--------------------------------------------------------------*/
    VOID
ini_tray()
{
UWORD 	wkind ;
GRECT	r ;
WORD	x, y ;

    modified = FALSE ;
    wkind = NAME | SIZER | MOVER | UPARROW | DNARROW | 
				VSLIDE | LFARROW | RTARROW | HSLIDE | FULLER ;
    r.g_h = min( gl_desk.g_h, 2 * gl_hbox + MAX_ENTRIES * gl_hcell ) ;
    r.g_h = align_h( r.g_h ) ;
    objc_offset( dev_tree, DEV01ICN, &x, &y ) ;
    r.g_w = x - gl_desk.g_x - 2 * gl_wbox ;
    w_handle = do_create( wkind, gl_desk.g_x, gl_desk.g_y, gl_desk.g_w, r.g_h,
				 &r.g_x, &r.g_y, &r.g_w, &r.g_h ) ;
    if ( cur_opts.window.g_w && cur_opts.window.g_h )
	movb( sizeof( GRECT ), &cur_opts.window, &gl_win ) ;
    else
    	movb( sizeof( GRECT ), &r, &gl_win ) ;
/*    gl_win.g_h = min( gl_win.g_h, MAX_ENTRIES * gl_hcell ) ; */
    upd_tray( UNTITLED, TRUE ) ;	/* will open and do a wind_set() */
    wind_get( w_handle, WF_WXYWH, &gl_work.g_x, &gl_work.g_y, 
						&gl_work.g_w, &gl_work.g_h ) ;
    gl_work.g_x += (menu_width - 1) ;
    gl_work.g_w -= (menu_width - 1) ;
    set_y( cntl_tr, COPYITEM, max( get_y( cntl_tr, COPYITEM ),
		gl_work.g_h - gl_hbox - get_height( cntl_tr, COPYITEM )  ) ) ;
    size_slots() ;
    strt_name = 0 ;
    strt_col  = 0 ;
    longest_name = 0 ;
    def_txt_atts() ;
    set_sliders() ;
} /* ini_tray */

/*--------------------------------------------------------------*/
/* read preferences nodes one at a time from the OUTPUT.INF	*/
/* file. If the type AND the icon label match a device that's	*/
/* already loaded, copy the preferences into the dvc[] structure*/
    VOID
dev_prefs( f_hndl )
WORD	f_hndl ;
{
LONG		size ;		/* number of bytes to read at a time */
LONG		ret_size ;	/* number of bytes successfully read */ 
DVC_STRUCT	tmp_dev ;	/* read one device's prefs at a time */
WORD		ii ;		/* index through all devices in file */
WORD		sav_port ;	/* use port value from previous gdos call */
WORD		sav_num ;	/* likewise device number (21,22,etc) */

    size = (LONG)( sizeof( DVC_STRUCT ) ) ;
    ret_size = dos_read( f_hndl, size, ADDR( &tmp_dev ) ) ;
    while ( ret_size == size )
        {
	for ( ii = 0; dvc[ ii ].type != 0; ii++ )
	    {
	    if ( dvc[ ii ].type == tmp_dev.type )
	        if ( strcmp( dvc[ ii ].id, tmp_dev.id ) )
		    { /* use port, num from gdos call, not prefs .INF file */
		    sav_port = dvc[ ii ].prefs.pd.port ;
		    sav_num  = dvc[ ii ].num ;
		    movb( sizeof( DVC_STRUCT ), &tmp_dev, &dvc[ ii ] ) ;
		    dvc[ ii ].prefs.pd.port = sav_port ;
		    dvc[ ii ].num = sav_num ;
		    }
	    }
    	ret_size = dos_read( f_hndl, size, ADDR( &tmp_dev ) ) ;
	}
} /* dev_prefs() */

/*--------------------------------------------------------------*/
    VOID
rd_def()
{
BOOLEAN	ok ;
LONG	info_tree ;
GRECT	r ;
WORD	ii ;
UWORD 	f_hndl ;
LONG	size, ret_size;

    graf_mouse( MO_HGLASS, 0x0L ) ;
    strcpy( opt_dir, buffer.m.s2 ) ;
    strcat( INFNAME, buffer.m.s2 ) ;
    rsrc_gaddr( GAD_TREE, OUTINFO, &info_tree ) ;
    LSTRCPY( get_spec( info_tree, VERSNUM ), ADDR( cur_opts.version ) ) ;
    strcpy( lis_dir, cur_opts.gsx_wc ) ;
    strcat( GEM_EXT, cur_opts.gsx_wc ) ;
    strcpy( cur_opts.gsx_wc, gsx_wc ) ;
    size = ( LONG )( sizeof( OPTIONS ) ) ;
    f_hndl = dos_open( ADDR( buffer.m.s2 ), 0 ) ;
    ok = ( ! DOS_ERR ) ;
    if ( ok )
	{
	    ret_size = dos_read( f_hndl, size, ADDR( &buffer.m.opts ) ) ;
	    ok = ( DOS_ERR ) ? FALSE : ( ret_size == size ) ;
	}
    if ( ok )
	ok = strcmp( &cur_opts.version, &buffer.m.opts.version ) ;
    if ( ok )
	{
	movb( sizeof( OPTIONS ), &buffer.m.opts, &cur_opts ) ;
	del_fname( cur_opts.gsx_wc, buffer.m.s1 ) ;
	if ( ! check_path( buffer.m.s1 ) )
	    strcpy( gsx_wc, cur_opts.gsx_wc ) ;
	}
    strcpy( cur_opts.gsx_wc, gsx_wc ) ;
    del_fname( gsx_wc, lis_wc ) ;
    strcat( LIS_EXT, lis_wc ) ;
    del_fname( lis_wc, lis_dir ) ;
    for ( ii = 0; ii < MX_DVCS; ii++ )
	if ( dvc[ ii ].num == cur_opts.cur_dvc )
	    {
	    cur_opts.cur_dvc = ii ;    
	    ii = MX_DVCS + 100;	/* break out of for loop above */
	    }
    if ( ii < 100 )
        cur_opts.cur_dvc = 0 ;
    set_state( dev_tree, cur_opts.cur_dvc + DEV01ICN, SELECTED ) ;
    get_obloc( dev_tree, cur_opts.cur_dvc + DEV01ICN, &r.g_x, &r.g_y, &r.g_w, &r.g_h ) ;
    set_prefmenu() ;
    do_redraw( dev_tree, 0, r.g_x, r.g_y, r.g_w, r.g_h ) ;
    pad_n_itoa( 2, cur_opts.numcopies, get_teptext( cntl_tr, COPYSTXT ) ) ;
    if ( !ok )
        make_def() ;		/* rewrite .INF file */
    else
        dev_prefs( f_hndl ) ;		/* set any device preferences */
    dos_close( f_hndl ) ;
    graf_mouse( MO_ARROW, 0x0L ) ;
} /* rd_def */

/*--------------------------------------------------------------*/
/*  need to shorten file menu OR change string in last item.	*/
    VOID
fix_filemenu()
{
    BYTE	tmpname[ NAME_LENGTH ], appname[ 30 ] ;
    WORD	ii, menu_height ;

    if ( *invoker )	/* put invoker name in file menu */
	{
	LSTRCPY( get_spec( gl_menu, TOAPPITM ), ADDR( &appname[ 0 ] ) ) ;
	b_stuff( ADDR( &appname[ 5 ] ), NAME_LENGTH-1, SP ) ;
	del_path( invoker, tmpname ) ;
	tmpname[ pos( PERIOD, tmpname ) ] = NULL ;
	for ( ii = 1; tmpname[ ii ]; ii++ )
	    {
	    if ( tmpname[ ii ] >= UCASE_A && tmpname[ ii ] <= UCASE_Z )
	        tmpname[ ii ] |= 0x20 ;	/* to lower case */
	    }
 	movb( strlen( tmpname ), &tmpname, &appname[ 5 ] ) ;
	menu_text( gl_menu, TOAPPITM, ADDR( appname ) ) ;
	}
    else			/* delete "To WHATEVER.APP" item from file menu */
	{
    	menu_height = get_height( gl_menu, NEWITEM - 1 ) ; /* NEW-1 = root */
    	menu_height -= get_height( gl_menu, TOAPPITM ) ;
    	set_height( gl_menu, NEWITEM-1, menu_height ) ;
    	set_tail( gl_menu, NEWITEM-1, TOAPPITM ) ;
    	set_next( gl_menu, TOAPPITM, NEWITEM-1 ) ;
	menu_text( gl_menu, TOAPPITM, get_spec( gl_menu, QUITITEM ) ) ;
	key_map[ TOAPPITM - FRSTITEM ] = key_map[ QUITITEM - FRSTITEM ] ;
	}
} /* fix_filemenu */


/*--------------------------------------------------------------*/
	VOID
get_invoker( scratch )
BYTE	*scratch ;
{
BYTE	*tptr, *tptr2, *tail_ptr ;
BOOLEAN	ret_name ;

    tail_ptr = &cmd_tail[ 1 ] ;
    tptr = invkr_file ;
    while( (*tptr != SLASH) && (*tptr != NULL) )
	tptr++ ;
    ret_name = FALSE ;		/* only gem write requires different name returned */
    if ( *tptr == SLASH )
	{
	tptr2 = tptr + 1 ;
    	while( (*tptr2 != SLASH) && (*tptr2 != NULL) )
	    tptr2++ ;
	if ( *tptr2 == SLASH )
	    {
	    ret_name = TRUE ;
	    *tptr2 = NULL ;
	    }
	strcpy( tptr+1, invoker ) ;
	cmd_tail[ strlen( invkr_file ) - strlen( tptr ) + 1 ] = NULL ;
	*tptr = NULL ;
	}
    else
	*invoker = NULL ;
    if ( ret_name )
	{
	if ( *(tptr2+2) == COLON )
	    strcpy( tptr2+1, tail_ptr ) ;
	else
	    {
	    strcpy( tail_ptr, scratch ) ;
	    del_fname( scratch, tail_ptr ) ;
	    strcat( tptr2+1, tail_ptr ) ;
	    }
	}
    strcpy( APPNAME, scratch ) ;
    shel_find( ADDR( scratch ) ) ;
    del_fname( scratch, opt_dir ) ;	/* directory for .INF file */
    if ( ( strlen( tail_ptr ) + strlen( scratch ) + 2 ) < sizeof( cmd_tail ) )
	{		/* plus two for length byte at front and null at end */
    	strcat( "/", tail_ptr ) ;
    	strcat( scratch, tail_ptr ) ;
	}
    cmd_tail[ 0 ] = strlen( tail_ptr ) ;
    fix_filemenu() ;
} /* get_invoker */


/*--------------------------------------------------------------*/
    VOID
get_lisdir()
{
BYTE	*tptr ;

    tptr = invkr_file ;
    while ( ( *tptr != COMMA ) && ( *tptr != SP ) && ( *tptr != NULL ) )
	tptr++ ;
    --tptr ;
    while ( ( *tptr != BSLASH ) && ( *tptr != NULL ) )
	tptr-- ;
    if ( *tptr == BSLASH )
	{
	b_stuff( ADDR( lis_dir ), sizeof( lis_dir ), NULL ) ;
	movb( strlen( invkr_file ) - strlen( tptr ) + 1, invkr_file, lis_dir ) ;
	strcpy( lis_dir, lis_wc ) ;
	strcat( LIS_EXT, lis_wc ) ;
	strcpy( lis_dir, gsx_wc ) ;
	strcat( GEM_EXT, gsx_wc ) ;
	}
} /* get_lisdir */

/*--------------------------------------------------------------*/
    WORD
get_files()
{
BYTE	tmp[ FNAME_LENGTH ], first_name[ FNAME_LENGTH ] ;
BYTE	*tptr ;

    tptr = invkr_file ;
    while ( ( *tptr != COMMA ) && ( *tptr != SP ) && ( *tptr != NULL ) )
	tptr++ ;
    b_stuff( ADDR( first_name ), sizeof( first_name ), NULL ) ;
    movb( strlen( invkr_file ) - strlen( tptr ), invkr_file, first_name ) ;
    if ( *first_name == NULL )
	return( FALSE ) ;			/* flag for new_tray() */
    if ( pos( PERIOD, first_name ) < 0 )
	strcat( GEM_EXT, first_name ) ;
    if ( first_name[ 1 ] != COLON )
    	shel_find( ADDR( first_name ) ) ;
    if ( !extension( LIS_EXT, first_name ) )
	return( 2 ) ;				/* flag to add_name() */
    if ( pos( BSLASH, first_name ) < 0 )
	{
	strcpy( lis_dir, tmp ) ;
	strcat( first_name, tmp ) ;
	strcpy( tmp, first_name ) ;
	}
    if ( dos_sfirst( ADDR( first_name ), 0 ) ) 
	{
	strcpy( first_name, invkr_file ) ;
	return( 1 ) ;				/* flag to rd_btch */
	}
    return( FALSE ) ;				/* flag for new_tray() */
} /* get_files */

/*--------------------------------------------------------------*/
    VOID
get_dir()
{
BYTE 	cmnd[ 128 ] ;

    shel_read( ADDR( &cmnd[ 0 ] ), ADDR( cmd_tail ) ) ;
    strcpy( APPNAME, cmnd ) ;
    shel_find( ADDR( cmnd ) ) ;    
#if DEBUG	
    show_hex( cmd_tail ) ;
#endif
    del_path( cmnd, inv_fname ) ;
    del_fname( cmnd, lis_dir ) ;
    lis_dir[ 3 ] = NULL ;		/* default to root of home directory */
    cmd_tail[ cmd_tail[ 0 ] + 1 ] = NULL ;	/* [0] holds length */
    strcpy( &cmd_tail[ 1 ], invkr_file ) ;
    get_invoker( cmnd ) ;/* use cmnd as scratch - why declare more on stack? */
} /* get_dir */

/*--------------------------------------------------------------*/
    WORD
init1()
{
#ifdef FREEGEM_STUFF
	X_BUF_V2 xbuf;

	memset(&gl_xbuf, 0, sizeof(gl_xbuf));
	xbuf.buf_len = sizeof(X_BUF_V2); /* OCR: Added X_BUF_V2 for FreeGEM */

	appl_init(&xbuf) ;

	if (xbuf.arch)
	   FreeGEM = TRUE;
	else
	   FreeGEM = FALSE;
#else
	appl_init();
#endif

	if (global.ap_version < 0x0300)
	{
	   form_alert(1, "[3][This version of Output requires|GEM 3.0 or higher.][ Quit ]");
	   return(-100);
	}

#if I8086
	ad_break = dos_gint( 0x23 ) ;
	dos_sint( 0x23, brk_addr ) ;
#endif
	dos_sdta( ADDR( &_dta[ 0 ] ) ) ;
	vopen( TRUE ) ;	
	if ( !setup_rsc() )
	    return( -100 ) ;	/* panic flag - no resource file */
	graf_mouse( MO_ARROW, 0x0L ) ;
	bkgnd_ok = ( appl_find( ADDR( "CALCLOCK" ) ) >= 0 ) ;
	get_dir() ;
	rd_def() ;
	ini_tray() ;
	get_lisdir() ;
	return( get_files() ) ;
} /* init1 */

/*--------------------------------------------------------------*/
    WORD
OUTINIT()
{
    return( init1() ) ;
} /* OUTINIT */


/* end of initialize.c */

	
