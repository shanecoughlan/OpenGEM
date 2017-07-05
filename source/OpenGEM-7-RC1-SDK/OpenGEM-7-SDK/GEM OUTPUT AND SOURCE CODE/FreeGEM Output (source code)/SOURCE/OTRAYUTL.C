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
#include "gembind.h"
#include "xgembind.h"
#include "dosbind.h"

#include "output.h"
#include "odefs.h"
#include "ovar.h"

#ifndef R_STRING
   #define R_STRING  5
#endif

EXTERN LONG	file_size() ;		/* oportabl.c */

EXTERN VOID	b_stuff() ;		/* util86.asm */

EXTERN VOID	do_miss() ;		/* odialog.c */

EXTERN VOID	send_redraw() ;		/* owindow.c */
EXTERN VOID	size_slots() ;
EXTERN VOID	set_sliders() ;			
EXTERN VOID	init_scroll_vars() ;
EXTERN VOID	set_scroll_pos() ;
EXTERN VOID	realign_slct() ;
EXTERN VOID	do_scrolled() ;
EXTERN VOID	upd_tray() ;
EXTERN VOID	upd_menu() ;

EXTERN WORD	minmax() ;		/* outility.c */
EXTERN WORD	max() ;
EXTERN VOID	del_fname() ;
EXTERN VOID	del_path() ;
EXTERN VOID	bld_dir() ;
EXTERN WORD	min() ;
EXTERN VOID	wswap() ;
EXTERN WORD	extension() ;
EXTERN WORD	f_alert() ;
EXTERN VOID	movb() ;
EXTERN WORD	pos() ;

EXTERN VOID	rc_union() ;			/* ooptimz.c */
EXTERN VOID	r_set() ;
EXTERN WORD	rc_intersect() ;
EXTERN WORD	strchk() ;
EXTERN VOID	strcpy() ;
EXTERN VOID	strcat() ;
EXTERN WORD	strlen() ;
EXTERN UWORD	inside() ;
EXTERN BOOLEAN  strcmp() ;

/*--------------------------------------------------------------*/
    VOID
redr_slots()
{
GRECT 	sr, er ;
WORD  	s_ob, e_ob ;
    
    s_ob = FIL0NAME + s_slot - strt_name ;
    e_ob = FIL0NAME + e_slot - strt_name ;
    s_ob = minmax( FIL0NAME, s_ob, FILXNAME ) ;
    e_ob = minmax( FIL0NAME, e_ob, FILXNAME ) ;
    get_obloc( tray_tr, s_ob, &sr.g_x, &sr.g_y, &sr.g_w, &sr.g_h ) ;
    get_obloc( tray_tr, e_ob, &er.g_x, &er.g_y, &er.g_w, &er.g_h ) ;
    rc_union( &sr, &er ) ;
    send_redraw( &er.g_x ) ;
} /* redr_slots */


/*--------------------------------------------------------------*/
    VOID
move_tray()
{
    wind_get( w_handle, WF_CXYWH, &gl_win.g_x, &gl_win.g_y, 
					&gl_win.g_w, &gl_win.g_h ) ;
    wind_get( w_handle, WF_WXYWH, &gl_work.g_x, &gl_work.g_y, 
					&gl_work.g_w, &gl_work.g_h ) ;
    gl_work.g_x += menu_width ;
    gl_work.g_w -= menu_width ;
    size_slots() ;
} /* move_tray */


/*--------------------------------------------------------------*/
    VOID
ini_slots()
{
WORD 	cur_ob ;

    next_slot = 0 ;
    strt_name = 0 ;
    s_slot = 0 ;
    e_slot = MAX_ENTRIES - 1 ;
    for ( cur_ob = FIL0ITEM; cur_ob <= FILXITEM; cur_ob++ )
	{
	set_flags( tray_tr, cur_ob, NONE ) ;
	set_state( tray_tr, cur_ob, NORMAL ) ;
	}
    longest_name = 0 ;
    for ( num_empty = 0; num_empty < MAX_ENTRIES; num_empty++ )
	names[ num_empty ][ 0 ] = NULL ;
    --num_empty ;
    b_stuff( ADDR( selected ), sizeof( selected ), NULL ) ;
    redr_slots() ;
    num_slct = 0 ;
    set_sliders() ;
} /* ini_slots */


/*--------------------------------------------------------------*/
	WORD
get_trob( x, y )
	WORD x, y ;
{
WORD 	obj ;

    obj = objc_find( tray_tr, ROOT, MAX_DEPTH, x, y ) ;
    if ( ( obj >= FIL0ITEM ) && ( obj <= FILXITEM ) )
	{
	if ( get_flags( tray_tr, obj ) & SELECTABLE )
	    return( obj ) ;
	}
    return( -1 ) ;
} /* get_trob */


/*--------------------------------------------------------------*/
	WORD
get_obslot( y )
	WORD y ;
{
WORD 	obj, obj_y, obj_x ;
    
    for ( obj = FILXITEM; obj >= FIL0ITEM; obj-- )
	{
	objc_offset( tray_tr, obj, &obj_x, &obj_y ) ;		
	if ( y >= obj_y )
	   return( obj ) ;
	}
    return( FIL0ITEM ) ;
} /* get_obslot */

/*--------------------------------------------------------------*/
    VOID
deslct_slots()
{
WORD 	ob, ii ;

    for ( ii = 0; ii < next_slot; ii++ )
	{
	if ( selected[ ii ] )
	    {
	    selected[ ii ] = FALSE ;
	    ob = ii - strt_name + FIL0ITEM ;
	    if ( ( ob >= FIL0ITEM ) && ( ob <= FILXITEM ) )
	    	objc_change( tray_tr, ob, MAX_DEPTH, gl_work.g_x, 
			gl_work.g_y, gl_work.g_w, gl_work.g_h, NORMAL, TRUE ) ;
	    }
	}
    num_slct = 0 ;
} /* deslct_slot */


/*--------------------------------------------------------------*/
    VOID
slct_slot( ob, kstate )
	WORD ob, kstate ;
{
WORD 	ii ;

    ii = strt_name + ob - FIL0ITEM ;
    if ( ( kstate & K_LSHIFT ) || ( kstate & K_RSHIFT ) )
	{
	if ( ( ob >= FIL0ITEM ) && ( ob <= FILXITEM ) )
 	    tggl_state( tray_tr, ob, gl_work.g_x, gl_work.g_y, 
			   gl_work.g_w, gl_work.g_h, SELECTED, TRUE ) ;
	num_slct += ( selected[ ii ] * (-2) + 1 ) ;   /* inc by 1 or -1 */
	selected[ ii ] = ! selected[ ii ] ;
	}
    else
	{
	deslct_slots() ;
	if ( ( ob >= FIL0ITEM ) && ( ob <= FILXITEM ) )
	    {
	    objc_change( tray_tr, ob, MAX_DEPTH, gl_work.g_x, gl_work.g_y, 
			gl_work.g_w, gl_work.g_h, SELECTED, TRUE ) ;
	    }
	num_slct++ ;
	selected[ ii ] = TRUE ;
	}
} /* slct_slot */


/*--------------------------------------------------------------*/
    VOID
rub_slct( x, y, w, h )
    WORD	x, y, w, h ;
{
WORD 	ob, state, last_ob ;
GRECT 	rbox, r ;

    r_set( &rbox, x, y, w, h ) ;
    if ( rc_intersect( &gl_work, &rbox ) )	/* clamps rbox to gl_work */
	{
    	last_ob = FIL0ITEM + next_slot - strt_name - 1 ; 
    	for ( ob = FIL0ITEM ; ob <= last_ob; ob++ )
	    {
	    get_obloc( tray_tr, ob, &r.g_x, &r.g_y, &r.g_w, &r.g_h ) ;
	    if ( rc_intersect( &rbox, &r ) )
	    	{
	    	selected[ strt_name + ob - FIL0ITEM ] = TRUE ;
	    	state = get_state( tray_tr, ob ) | SELECTED ;
	    	objc_change( tray_tr, ob, MAX_DEPTH, gl_work.g_x, gl_work.g_y, 
			    gl_work.g_w, gl_work.g_h, state, TRUE ) ;
	    	num_slct++ ;
	    	}
	    }
	}
    upd_menu() ;
} /* rub_slct */

/*--------------------------------------------------------------*/
    VOID
swap_strs( str1, str2 ) 
BYTE		*str1, *str2 ;
{
BYTE		tmp[ FNAME_LENGTH ] ;

    strcpy( str1, tmp ) ;
    strcpy( str2, str1 ) ;
    strcpy( tmp, str2 ) ;
} /* swap_strs */

/*--------------------------------------------------------------*/
    VOID
sort_list( hi_indx, lo_indx ) 
WORD	hi_indx, lo_indx ;
{
WORD		mid ;
WORD		i, j, gap;

    if ( hi_indx == lo_indx )
	return ;
    mid =  max( 1, ( hi_indx - lo_indx ) / 2 ) ;   /* sort 2 to n names */
    for (gap = mid; gap > 0; gap /= 2)
	{
	for (i = lo_indx + gap; i <= hi_indx; i++)
	    {
	    for (j = i-gap; j >= lo_indx ; j -= gap)
	    	{
	      	if ( strchk( names[ j ], names[ j + gap ] ) < 0 )
		    break ;
		swap_strs( names[ j ], names[ j + gap ] ) ;
	    	} /* for j */
	    } /* for i */
	} /* for gap */
} /* sort_list */


/*--------------------------------------------------------------*/
    VOID
add_name()
{
WORD	ob ;

    ob = next_slot - strt_name + FIL0ITEM ;
    if ( ( ob >= FIL0ITEM ) && ( ob <= FILXITEM ) )
    	set_flags( tray_tr, ob, SELECTABLE ) ;
    next_slot++ ;
    upd_menu() ;
} /* add_name */


/*--------------------------------------------------------------*/
    VOID
show_last( last )
WORD	last ;
{
WORD	max_hstrt, num_cols, max_vstrt, num_names ;

    init_scroll_vars( &max_hstrt, &num_cols, &max_vstrt, &num_names ) ;
    if ( ( last > (strt_name + num_names - 1) ) || ( last < strt_name ) )
	{
	strt_name = minmax( 0, last - num_names + 1, MAX_ENTRIES - num_names ) ;
	s_slot = 0 ;
	e_slot = MAX_ENTRIES - 1 ;
    	set_scroll_pos( max_vstrt, max_hstrt ) ;
	realign_slct() ;
	}
} /* show_last */

/*--------------------------------------------------------------*/
    VOID
add_one()
{
    s_slot = next_slot ;
    --num_empty ;
    add_name() ;
    e_slot = next_slot - 1 ;
    show_last( e_slot ) ;
    redr_slots() ;
} /* add_one */

/*--------------------------------------------------------------*/
    BOOLEAN
add_all( dir, wcard )
	BYTE	*dir, *wcard ;
{
BYTE		dirwcard[ FNAME_LENGTH ] ;
BOOLEAN 	ret, ok ;
BYTE		gl_dta[ INTOUT_SIZE ] ;
WORD		ii, start, finish ;

    strcpy( dir, dirwcard ) ;
    strcat( wcard, dirwcard ) ;
    dos_sdta( ADDR( &gl_dta[ 0 ] ) ) ;
    ok = dos_sfirst( ADDR( dirwcard ), 0x0 ) ;
    ret = ok ;
    start = finish = next_slot ;
    while ( ok )
	{
        strcpy( dir, names[ finish ] ) ;
	strcat( &gl_dta[ 30 ], names[ finish ] ) ;
	longest_name = max( longest_name, strlen( names[ finish ] ) ) ;
	++finish ;
    	ok = ( dos_snext() && ( finish < MAX_ENTRIES ) ) ;
	}
    sort_list( finish-1, start ) ;
    for ( ii = start; ii < finish; ii++ )
	add_one() ;
    set_sliders() ;
    return( ret ) ;
} /* add_all */

/*--------------------------------------------------------------*/
/* Wild card descriptors from fsel_input can look like :	*/
/* "C:\IMAGES\*.GEM,*.IMG,*.DOC", etc up to total of 64 bytes.	*/
	BOOLEAN
add_wcards( wc )
BYTE	*wc ;
{
BYTE 	dir[ PATH_LENGTH ] ;
BYTE 	wcard[ PATH_LENGTH ], *wcptr, *end, *wcend ;
BOOLEAN	ok, ret ;

    ret = FALSE ;
    del_fname( wc, dir ) ;
    del_path( wc, wcard ) ;
    end = &wcard[ strlen( wcard ) ] ;
    wcptr = wcard ;
    ok = TRUE ;
    while ( ok )
	{
	wcend = wcptr ;
	while ( ( wcend < end ) && ( *wcend != COMMA ) )
	    ++wcend ;
	*wcend = NULL ;
	ret |= add_all( dir, wcptr ) ;
	if ( wcend >= end )
	    ok = FALSE ;
	if ( ok )
	    wcptr = wcend + 1 ;
	}
    return( ret ) ;
} /* add_wcards() */

/*--------------------------------------------------------------*/
	WORD
add_entry()
{
BYTE 	dir[ PATH_LENGTH ] ;
BYTE	name[ NAME_LENGTH ] ;
BYTE 	wc[ PATH_LENGTH ] ;
WORD	ob ;
BYTE  title[50];

    rsrc_gaddr(R_STRING, SELFILES, &title);

    if ( !get_name_xtnd( gsx_wc, wc, name, title ) )
	return( FALSE ) ;

    del_fname( wc, dir ) ;
    if ( *name )
	{
	bld_dir( dir, name, names[ next_slot ] ) ;
	if ( !dos_sfirst( ADDR( names[ next_slot ] ), 0 ) ) 
	    {
	    *names[ next_slot ] = NULL ;
	    do_miss( ADDR( dir ), ADDR( name ) ) ;
	    return( FALSE ) ;
	    }
	}
    if ( next_slot == 0 )	/* ie: this is the first name in the list */
	{
    	strcpy( dir, lis_dir ) ;
    	strcpy( dir, lis_wc ) ;
    	strcat( LIS_EXT, lis_wc ) ;
	}
    strcpy( wc, gsx_wc ) ;
    deslct_slots() ;
    if ( *name )
	{
	if ( longest_name < strlen( names[ next_slot ] ) ) 
	    {
	    longest_name = strlen( names[ next_slot ] ) ;
	    set_sliders() ;
	    }
	selected[ next_slot ] = TRUE ;
	++num_slct ;
	add_one() ;
	ob = e_slot - strt_name + FIL0ITEM ;
	if ( ( ob >= FIL0ITEM ) && ( ob <= FILXITEM ) )
   	    tggl_state( tray_tr, ob, gl_work.g_x, gl_work.g_y, 
		   gl_work.g_w, gl_work.g_h, SELECTED, TRUE ) ;
	return( TRUE ) ;
	}
    else
	return( add_wcards( wc ) ) ;
} /* add_entry */

/*--------------------------------------------------------------*/
    VOID
dup_entry()
{
WORD 	ob, ii, jj ;

    ii = next_slot - 1 ;
    next_slot += num_slct ;
    s_slot = e_slot = next_slot - 1 ;
    for ( jj = e_slot; jj > ii; jj-- )
	{
	ob = jj - strt_name + FIL0ITEM ;
	if ( ( ob >= FIL0ITEM ) && ( ob <= FILXITEM ) )
            set_flags( tray_tr, ob, SELECTABLE ) ;
	}
    jj = e_slot ;
    while ( ii >= 0 )
	{
	if ( selected[ ii ] )
	    {
	    strcpy( names[ ii ], names[ jj ] ) ;
	    selected[ ii ] = FALSE ;
	    ob = ii - strt_name + FIL0ITEM ;
	    if ( ( ob >= FIL0ITEM ) && ( ob <= FILXITEM ) )
	        tggl_state( tray_tr, ob, gl_work.g_x, gl_work.g_y, 
			   gl_work.g_w, gl_work.g_h, SELECTED, FALSE ) ;
	    selected[ jj ] = TRUE ;
	    ob = jj - strt_name + FIL0ITEM ;
	    if ( ( ob >= FIL0ITEM ) && ( ob <= FILXITEM ) )
	    	tggl_state( tray_tr, ob, gl_work.g_x, gl_work.g_y, 
			   gl_work.g_w, gl_work.g_h, SELECTED, FALSE ) ;
	    --num_empty ;
       	    --jj ;
	    s_slot = ii ;
	    }
	--ii ;
	}
    show_last( e_slot ) ;
    redr_slots() ;
    upd_menu() ;
} /* dup_entry */

/*--------------------------------------------------------------*/
    VOID
del_entry()
{
WORD 	ob, cur_ii, prv_ii ;

    cur_ii = 0 ;
    s_slot = e_slot = next_slot - 1 ;
    for ( prv_ii = cur_ii; cur_ii != next_slot; cur_ii++ )
	{
	if ( selected[ cur_ii ] )
	    {
	    s_slot = min( s_slot, cur_ii ) ;
	    selected[ cur_ii ] = FALSE ;
    	    b_stuff( ADDR( names[ cur_ii ] ), FNAME_LENGTH, NULL ) ;
	    ob = cur_ii - strt_name + FIL0ITEM ;
	    if ( ( ob >= FIL0ITEM ) && ( ob <= FILXITEM ) )
	    	set_state( tray_tr, ob, NORMAL ) ;
	    num_slct = 0 ;
	    }
	else
	    {
	     if ( cur_ii != prv_ii )
	    	strcpy( names[ cur_ii ], names[ prv_ii ] ) ; 
	    ++prv_ii ;
	    }
	}
    --next_slot ;
    while ( next_slot >= prv_ii )
	{
	ob = next_slot - strt_name + FIL0ITEM ;
	if ( ( ob >= FIL0ITEM ) && ( ob <= FILXITEM ) )
	    set_flags( tray_tr, ob, NONE ) ;
	*names[ next_slot ] = NULL ;
	++num_empty ;
	--next_slot ;
	}
    ++next_slot ;
    if ( !( get_flags( tray_tr, FIL0ITEM ) & SELECTABLE ) )
    	show_last( prv_ii - 1 ) ;
    redr_slots() ;
    upd_menu() ;
} /* del_entry */


/*--------------------------------------------------------------*/
    VOID
move_entry( x, y )
	WORD x, y ;
{
BYTE	save_name[ FNAME_LENGTH ] ;
WORD 	ii, state ;
WORD 	ob, start, end, step ;

    if ( ! inside( x, y, &gl_work.g_x ) )
	return ;
    ob = get_obslot( y ) ;
    if ( !( get_flags( tray_tr, ob ) & SELECTABLE ) )
	ob = min( FILXITEM, FIL0ITEM + next_slot - strt_name - 1 ) ;
    start = s_slot = gl_ob - FIL0ITEM + strt_name ;
    end   = e_slot = ob    - FIL0ITEM + strt_name ;
    if ( start == end )
	return ;
    step = 1 ;
    if ( start > end )
	{
	step = -1 ;
	wswap( &s_slot, &e_slot ) ;
	}
    state = selected[ start ] ;
    strcpy( names[ start ], save_name ) ;
    while( start != end )
        {
	strcpy( names[ start + step ], names[ start ] ) ;
	selected[ start ] = selected[ start + step ] ;
	start += step ;
	}
    strcpy( save_name, names[ end ] ) ;
    selected[ end ] = state ;
    for ( ii = s_slot; ii <= e_slot ; ii++ )
	{
	ob = ii - strt_name + FIL0ITEM ;
	state = selected[ ii ] ? SELECTED : NORMAL ;
	if ( ( ob >= FIL0ITEM ) && ( ob <= FILXITEM ) )
	    set_state( tray_tr, ob, state ) ;
	}
    redr_slots() ;
    modified = TRUE ;
    upd_menu() ;
} /* move_entry */


/*--------------------------------------------------------------*/
    VOID
new_tray()
{
GRECT	r ;

    ini_slots() ;
    upd_tray( UNTITLED, FALSE ) ;
    do_scrolled( WM_VSLID, 0 ) ;	/* set scrollers to top, left */
    do_scrolled( WM_HSLID, 0 ) ;
    *cur_file = NULL ;
    modified = FALSE ;
    upd_menu() ;
    r_set( &r, gl_work.g_x, gl_work.g_y, INDX_SZ*gl_wcell, gl_work.g_h ) ;
    send_redraw( &r.g_x ) ;
} /* new_tray */

/*--------------------------------------------------------------*/
    VOID	
svas_tray()
{
BYTE 		file[ NAME_LENGTH ] ;
BYTE 		dir[ FNAME_LENGTH ] ;
BYTE 		wc[ PATH_LENGTH ] ;

    if ( !get_name( lis_wc, wc, file ) )
	return ;
    if ( *file == NULL ) 
	return ;
    if ( !extension( LIS_EXT, file ) )
	{
	f_alert( NOTLIST, 1 ) ;
	return ;
	}
    del_fname( wc, dir ) ;
    strcat( file, dir ) ;
    if ( dos_sfirst( ADDR( dir ), 0 ) )
	{
	if ( f_alert( REPLACEF, 1 ) != 1 )
	    return ;
	}
    dos_close( dos_create( ADDR( dir ), 0 ) ) ;
    if ( save_tray( dir ) )
	{
	strcpy( dir, cur_file ) ;
	upd_tray( cur_file, FALSE ) ;
    	del_fname( dir, lis_dir ) ;
    	strcpy( lis_dir, lis_wc ) ;
    	strcat( LIS_EXT, lis_wc ) ;
	} /* if save_tray */
} /* svas_tray */


/*--------------------------------------------------------------*/
	WORD
save_tray( file )
	BYTE *file ;
{
WORD 	fl_hndl ;
LONG	l_dummy ;

    graf_mouse( MO_HGLASS, 0x0L ) ;
    fl_hndl = dos_open( ADDR( file ), 1 ) ;
    if ( fl_hndl )
	{
	strcpy( btch_hdr, buffer.btch.head ) ;
	movb( sizeof( buffer.btch.names ), names, buffer.btch.names ) ;
	buffer.btch.num = next_slot ;
	l_dummy = dos_write( fl_hndl, (LONG)(sizeof( BTCHFL )), ADDR( &buffer.btch.head[ 0 ] ) ) ;
	dos_close( fl_hndl ) ;
	modified = FALSE ;
	upd_menu() ;
	graf_mouse( MO_ARROW, 0x0L ) ;
	return( TRUE ) ;
	}
    graf_mouse( MO_ARROW, 0x0L ) ;
    return( FALSE ) ;
} /* save_tray */


/*--------------------------------------------------------------*/
    VOID
open_tray()
{
    BYTE file[ NAME_LENGTH ], dir[ PATH_LENGTH ], wc[ PATH_LENGTH ] ;
    BYTE full_name[ FNAME_LENGTH ] ;

    if ( !get_name( lis_wc, wc, file ) )
	return ;
    if ( *file == NULL )
	return ;
    if ( !extension( LIS_EXT, file ) )
	{
	f_alert( NOTLIST, 1 ) ;
	return ;
	}
    del_fname( wc, dir ) ;
    bld_dir( dir, file, full_name ) ;
    if ( !dos_sfirst( ADDR( full_name ), 0 ) )
	{
	do_miss( ADDR( dir ), ADDR( file ) ) ;
	return ;
	}
    rd_btch( full_name ) ;
} /* open_tray */


/*--------------------------------------------------------------*/
    BOOLEAN
rd_btch( file )
	BYTE *file ;
{
    WORD 	fl_hndl ;
    LONG 	size ;
    BYTE 	dir[ PATH_LENGTH ] ;

    size = file_size( file ) ;
    if ( size < (LONG)(sizeof( BTCHFL )) )
	return( FALSE ) ;
    del_fname( file, dir ) ;
    strcpy( dir, lis_dir ) ;
    strcpy( dir, lis_wc ) ;
    strcat( LIS_EXT, lis_wc ) ;
    graf_mouse( MO_HGLASS, 0x0L ) ;
    fl_hndl = dos_open( ADDR( file ), 0 ) ;
    if ( !fl_hndl )
   	return( FALSE ) ;
    ini_slots() ;
    size = dos_read( fl_hndl, (LONG)(sizeof( BTCHFL )), ADDR( &buffer.btch.head[ 0 ] ) ) ;
    dos_close( fl_hndl ) ;
    if ( strcmp( btch_hdr, buffer.btch.head ) )
	{
	for ( next_slot=0 ; next_slot < buffer.btch.num ; next_slot++ )
    	    {
	    set_flags( tray_tr, FIL0ITEM + next_slot, SELECTABLE ) ;
	    if ( pos( BSLASH, buffer.btch.names[ next_slot ] ) < 0 )
		strcpy( lis_dir, names[ next_slot ] ) ;
	    strcat( buffer.btch.names[ next_slot ], names[ next_slot ] ) ;
	    longest_name = max( longest_name, strlen( names[ next_slot ] ) ) ;
	    }
 	num_empty = MAX_ENTRIES - buffer.btch.num - 1 ;
	strcpy( file, cur_file ) ;
	upd_tray( cur_file, FALSE ) ;
	modified = FALSE ;
	upd_menu() ;
	if ( e_slot < next_slot )
	    e_slot = next_slot ;
	set_sliders() ;
    	do_scrolled( WM_VSLID, 0 ) ;	/* set scrollers to top, left */
    	do_scrolled( WM_HSLID, 0 ) ;
	graf_mouse( MO_ARROW, 0x0L ) ;
	return( TRUE ) ;
	}
    graf_mouse( MO_ARROW, 0x0L ) ;
    f_alert( NOTLIST, 1 ) ;
    return( FALSE ) ;
} /* rd_btch */


/*--------------------------------------------------------------*/
    VOID
do_editmenu( item )
	WORD		item ;
{
    modified = TRUE ;
    switch ( item )
	{
	case ADDITEM:
		modified = add_entry() ;
		break ;
	case DUPLITEM:
		dup_entry() ;
		break ;
	case DELEITEM:
		del_entry() ;
		break ;
	} /* switch item */
} /* do_editmenu */


/*--------------------------------------------------------------*/
    VOID
tray_fmenu( item )
WORD	item ;
{
	switch( item )
	{
	  case NEWITEM:
		new_tray() ;
		break ;
	  case OPENITEM:
		open_tray() ;
		break ;
	  case SAVEITEM:
		save_tray( cur_file ) ;
		break ;
	  case SVASITEM:
		svas_tray() ;
		break ;
	}
} /* tray_fmenu */


/* end of trayutil.c */
