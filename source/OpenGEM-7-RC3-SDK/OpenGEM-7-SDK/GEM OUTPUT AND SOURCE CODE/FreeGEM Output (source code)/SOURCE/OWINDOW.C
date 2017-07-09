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
#include "taddr.h"
#include "evntlib.h"
#include "windlib.h"
#include "appllib.h"
#include "gembind.h"
#include "xgembind.h"
#include "vdibind.h"

#include "output.h"
#include "odefs.h"
#include "ovar.h"

EXTERN WORD	SMUL_DIV() ;		/* util86.asm */

EXTERN WORD	minmax() ;		/* ooptimz.c */
EXTERN WORD	max() ;
EXTERN WORD	min() ;
EXTERN VOID	strcpy() ;

EXTERN VOID	do_ap_write() ;		/* omain.c */

EXTERN VOID	movb() ;		/* outility.c */
EXTERN WORD	rc_equal() ;
EXTERN WORD	align_x() ;
EXTERN WORD	align_h() ;
EXTERN VOID	sidebar_ienable() ;
EXTERN VOID	itoa() ;

EXTERN VOID	move_tray(); 		/* otrayutl.c */


/*--------------------------------------------------------------*/
    VOID
send_redraw( r )
WORD *r ;
{
    gl_smsg[ 3 ] = w_handle ;
    movb( sizeof( GRECT ), r, &gl_smsg[ 4 ] ) ;
    do_ap_write( WM_REDRAW ) ;
} /* send_redraw() */


/*--------------------------------------------------------------*/
    VOID
set_sliders()
{
WORD	size, max_width, width ;

    width = gl_work.g_w - INDX_SZ * gl_wcell ;	/* minus indexes */
    max_width = ( longest_name > 0 ) ? longest_name * gl_wcell : width ;
    size = SMUL_DIV( 1000, width, max_width ) ;
    size = minmax( 0, size, 1000 ) ;
    wind_set( w_handle, WF_HSLSIZ, size, 0, 0, 0 ) ;
    size = SMUL_DIV( 1000, gl_work.g_h / gl_hcell, MAX_ENTRIES ) ;
    size = minmax( 0, size, 1000 ) ;
    wind_set( w_handle, WF_VSLSIZ, size, 0, 0, 0 ) ;
} /* set_sliders */


/*--------------------------------------------------------------*/
    VOID
set_scroll_pos( max_vstrt, max_hstrt )
WORD	max_vstrt, max_hstrt ;
{
WORD	horiz_pos, vert_pos ;

    if ( max_hstrt > 0 )
	{
    	horiz_pos = minmax( 0, SMUL_DIV( 1000, strt_col, max_hstrt ), 1000 ) ;
    	wind_set( w_handle, WF_HSLID, horiz_pos, 0, 0, 0 ) ;
	}
    if ( max_vstrt > 0 )
	{
    	vert_pos  = minmax( 0, SMUL_DIV( 1000, strt_name, max_vstrt ), 1000 )  ;
    	wind_set( w_handle, WF_VSLID, vert_pos, 0, 0, 0 ) ;
	}
} /* set_scroll_pos */


/*--------------------------------------------------------------*/
    VOID
init_scroll_vars( max_hstrt, num_cols, max_vstrt, num_names )
WORD	*max_hstrt, *num_cols, *max_vstrt, *num_names ;
{
    *num_names = gl_work.g_h / gl_hcell ;
    *max_vstrt = MAX_ENTRIES - *num_names ;
    *num_cols = ( gl_work.g_w / gl_wcell ) - INDX_SZ ; /* minus 5 for leading index */
    *max_hstrt = max( 0, longest_name - *num_cols ) ;
} /* init_scroll_vars */

/*--------------------------------------------------------------*/
    VOID
realign_slct()
{
WORD	state, flags, ii, ob ;

    for ( ii = 0; ii < MAX_ENTRIES; ii++ )
	{
	ob = ii - strt_name + FIL0ITEM ;
	if ( ( ob >= FIL0ITEM ) && ( ob <= FILXITEM ) )
	    {
	    state = selected[ ii ] ? SELECTED : NORMAL ;
	    set_state( tray_tr, ob, state ) ;
	    flags = ( ii < next_slot ) ? SELECTABLE : NONE ;
	    set_flags( tray_tr, ob, flags ) ;
	    }
	}
} /* realign_slct */


/*--------------------------------------------------------------*/
    VOID
do_arrowed( cmd )
WORD	cmd ;
{
WORD	max_hstrt, num_cols, max_vstrt, num_names ;
WORD	*to_change, *upper_lim, sign, inc ;

    init_scroll_vars( &max_hstrt, &num_cols, &max_vstrt, &num_names ) ;
    if ( cmd <= 3 )
	{	/* vertical panning */
	to_change = &strt_name ;
	upper_lim = &max_vstrt ;
	}
    else
	{	/* horiz panning */
	to_change = &strt_col ;
	upper_lim = &max_hstrt ;
	}
    sign = ( cmd % 2 ) ? 1 : -1 ;   /* neg for up and left motions */
    inc = 1 ;			/* line up or down or column left or right */
    if ( ( cmd == 1 ) || ( cmd == 0 ) )
	inc = num_names ;	/* page up or down */
    else if ( ( cmd == 4 ) || ( cmd == 5 ) )
   	inc = num_cols ;	/* page left or right */
    *to_change = minmax( 0, *to_change + (sign * inc), *upper_lim ) ;
    if ( cmd <= 3 ) 	/* vertical panning */
	realign_slct() ;
    set_scroll_pos( max_vstrt, max_hstrt ) ;
} /* do_arrowed */


/*--------------------------------------------------------------*/
    VOID
do_scrolled( event, new_pos )
WORD	event, new_pos ;
{
WORD	max_hstrt, num_cols, max_vstrt, num_names ;
WORD	flag ;

    init_scroll_vars( &max_hstrt, &num_cols, &max_vstrt, &num_names ) ;
    if ( event == WM_VSLID )	
	{
	strt_name = minmax( 0, SMUL_DIV( max_vstrt, new_pos, 1000 ), max_vstrt ) ;
	realign_slct() ;
	flag = WF_VSLID ;
	}
    else
	{
	strt_col = minmax( 0, SMUL_DIV( max_hstrt, new_pos, 1000 ), max_hstrt ) ;
	flag = WF_HSLID ;
	}
    wind_set( w_handle, flag, new_pos, 0, 0, 0 ) ;
} /* do_scrolled */


/*--------------------------------------------------------------*/
    VOID
do_wm_moved( r )
GRECT	*r ;
{
WORD	new_x, new_y, new_h ;

    new_x = min( r->g_x, gl_desk.g_x + gl_desk.g_w - 2 * gl_wbox ) ;
    new_x = align_x( new_x ) ;
    new_h = min( r->g_h, 2 * gl_hbox + (FILXNAME-FIL0NAME+1) * gl_hcell ) ;
    new_h = align_h( new_h ) ;
    new_y = min( r->g_y, gl_desk.g_y + gl_desk.g_h - 2 * gl_hbox ) ;
    wind_set( w_handle, WF_CXYWH, new_x, new_y, r->g_w, new_h ) ;
    move_tray() ;
} /* do_wm_moved */


/*--------------------------------------------------------------*/
    VOID
do_wm_sized( r )
GRECT	*r ;
{
WORD 	max_hstrt, num_cols, max_vstrt, num_names ;

    r->g_w = max( r->g_w, menu_width + gl_wbox * 6 ) ;
    do_wm_moved( r ) ;
    set_sliders() ;
    init_scroll_vars( &max_hstrt, &num_cols, &max_vstrt, &num_names ) ;
    if ( ( num_names + strt_name - 1 ) >= MAX_ENTRIES )
	{
	strt_name = MAX_ENTRIES - num_names ;
	realign_slct() ;
	}
    if ( ( longest_name - strt_col ) < num_cols )
	strt_col = max( 0, longest_name - num_cols ) ;
    set_scroll_pos( max_vstrt, max_hstrt ) ;
    set_y( cntl_tr, COPYITEM, max( get_y( cntl_tr, COPYITEM ),
		gl_work.g_h - gl_hbox - get_height( cntl_tr, COPYITEM )  ) ) ;
} /* do_wm_sized */


/*--------------------------------------------------------------*/
    VOID
do_wm_fulled()
{
GRECT	c, p, f ;		/* current, previous and full windows */

    wind_get( w_handle, WF_CXYWH, &c.g_x, &c.g_y, &c.g_w, &c.g_h ) ;
    wind_get( w_handle, WF_PXYWH, &p.g_x, &p.g_y, &p.g_w, &p.g_h ) ;
    wind_get( w_handle, WF_FXYWH, &f.g_x, &f.g_y, &f.g_w, &f.g_h ) ;
    if ( rc_equal( &c, &f ) )	/* current == full, so go to previous */
	{
	do_wm_sized( &p ) ;
	graf_shrinkbox( p.g_x, p.g_y, p.g_w, p.g_h, f.g_x, f.g_y, f.g_w, f.g_h ) ;
	}
    else			/* is not full so make it full */
	{
	graf_growbox( c.g_x, c.g_y, c.g_w, c.g_h, f.g_x, f.g_y, f.g_w, f.g_h ) ;
	do_wm_sized( &f ) ;
	}
} /* do_wm_fulled */


/*--------------------------------------------------------------*/
    VOID
size_slots()
{
    set_x( tray_tr, ROOT, gl_work.g_x ) ;
    set_y( tray_tr, ROOT, gl_work.g_y ) ;
    set_width( tray_tr, ROOT, gl_work.g_w ) ;
    set_height( tray_tr, ROOT, gl_work.g_h ) ;
    set_x( cntl_tr, ROOT, gl_win.g_x ) ;
    set_y( cntl_tr, ROOT, gl_work.g_y ) ;
    set_height( cntl_tr, ROOT, gl_desk.g_h ) ;
} /* size_slots */


/*--------------------------------------------------------------*/
	VOID
pronto_mesg()
{
WORD	ev_which, junk ;

    wind_update( 0 ) ;
    ev_which = evnt_multi( MU_MESAG | MU_TIMER, 
				0, 0, 0,
				0, 0, 0, 0, 0, 
				0, 0, 0, 0, 0, 
				ADDR( gl_rmsg ),
				0, 0,
				&junk, &junk, &junk, &junk, &junk, &junk ) ;
    wind_update( 1 ) ;
    if ( ev_which & MU_MESAG )
	{
	movb( sizeof( gl_smsg ), &gl_rmsg, &gl_smsg ) ;
	appl_write( global.ap_id, sizeof( gl_smsg ), ADDR( gl_smsg ) ) ;
	}
} /* pronto_mesg() */


/*--------------------------------------------------------------*/
    VOID
set_ob( tree, ob, mask, set )
    LONG	tree ;
    WORD	ob, mask ;
    BOOLEAN	set ;
{
WORD	state ;

    state = get_state( tree, ob ) ;
    if ( set )
    	state |= mask ;
    else /* clear */
	state &= ( ~mask ) ;	
    set_state( tree, ob, state ) ;
} /* set_ob */


/*--------------------------------------------------------------*/
/* called from oinitial.c and o_trayut.c			*/
    VOID
upd_tray( name, need_open )
BYTE 	*name ;
BOOLEAN need_open ;
{
WORD	xcen, ycen ;

    strcpy( name, list_name ) ;
    wind_set( w_handle, WF_NAME, ADDR( list_name ), 0, 0 ) ;
    if ( need_open )
	{
	xcen = gl_win.g_x + ( gl_win.g_w/2 ) ;
	ycen = gl_win.g_y + ( gl_win.g_h/2 ) ;
	do_open( w_handle, xcen, ycen, gl_win.g_x, gl_win.g_y, 
		    			gl_win.g_w, gl_win.g_h ) ;
	}
} /* upd_tray */


/*--------------------------------------------------------------*/
    VOID
upd_menu()
{
BOOLEAN  not_empty ;

    sidebar_ienable( DELEITEM, !(num_slct < 1), DELESTR0, DELESTR1 ) ;
    if ( num_slct < 1 )
	sidebar_ienable( DUPLITEM, FALSE, DUPLSTR0, DUPLSTR1 ) ;
    else
	sidebar_ienable( DUPLITEM, !( num_slct -1 > num_empty ), DUPLSTR0, DUPLSTR1 ) ;
    sidebar_ienable( ADDITEM, !( num_empty < 0 ), ADDSTR0, ADDSTR1 ) ; 
    sidebar_ienable( STRTITEM, next_slot > 0, STRTSTR0, STRTSTR0 ) ; 
    not_empty = ( next_slot != 0 ) ;
    menu_ienable( gl_menu, SVASITEM, not_empty ) ;
    menu_ienable( gl_menu, SAVEITEM, modified && not_empty && *cur_file ) ;
} /* upd_menu */



/*----------------------------------------------------------------------*/
/* sets all default text and line attribs just to be sure.		*/
    VOID
def_txt_atts()
{
WORD	junk ;

    vswr_mode( scr_hndl, 1 ) ;		/* replace mode */
/* screen attributes */
    vst_point( scr_hndl, 10, &junk, &junk, &junk, &junk ) ;
    vst_rotation( scr_hndl, 0 ) ;	
    vst_font( scr_hndl, 1 ) ;		/* system face */
    vst_color( scr_hndl, 1 ) ;		/* black */
    vst_effects( scr_hndl, 0 ) ;	/* none */
    vst_alignment( scr_hndl, 0, 0, &junk, &junk ) ;	/* left, baseline */
/* line attributes */
    vsl_type( scr_hndl, 1 ) ;		/* solid */
    vsl_width( scr_hndl, 1 ) ;		/* line width == 1 */
    vsl_color( scr_hndl, 1 ) ;		/* black */
    vsl_ends( scr_hndl, 0, 0 ) ;	/* squared ends */
} /* def_txt_atts */


/*----------------------------------------------------------------------*/
    VOID
fname_node( item, r )
WORD	item;
GRECT   *r ;
{
BYTE	*fname_ptr ;
BYTE	index[ 4 ] ;
WORD	ii, name_indx, y, p[ 4 ] ;

    name_indx = item - FIL0NAME + strt_name ;
    if ( name_indx >= MAX_ENTRIES )
	return ;
    index[ 0 ] = SP ;
    ii = ( name_indx+1 < 10 ) ? 1 : 0 ;
    itoa( name_indx+1, &index[ ii ] ) ;
    y = r->g_y + gl_baseoff ;
    v_gtext( scr_hndl, gl_work.g_x + gl_wcell, y, index ) ;
    p[ 0 ] = p[ 2 ] = gl_work.g_x - 1 + 4*gl_wcell ;
    p[ 1 ] = gl_work.g_y ;
    p[ 3 ] = gl_work.g_y + gl_work.g_h ; 
    v_pline( scr_hndl, 2, &p ) ;
    fname_ptr = &names[ name_indx ][ strt_col ] ;
    if ( *fname_ptr )
	v_gtext( scr_hndl, gl_work.g_x + INDX_SZ*gl_wcell, y, fname_ptr ) ;
} /* fname_node */

/*----------------------------------------------------------------------*/
	WORD
dr_code( pparms )
	LONG		pparms;
{
PARMBLK		pb ;
WORD 		pts[ 4 ] ;

    LBCOPY( ADDR( &pb ), pparms, sizeof( PARMBLK ) ) ;
    if ( pb.pb_tree == tray_tr )
	{
 	if ( ( pb.pb_obj >= FIL0NAME ) && ( pb.pb_obj <= FILXNAME ) )
	    {
    	    pts[ 0 ] = pb.pb_xc ;
    	    pts[ 1 ] = pb.pb_yc ;
	    pts[ 2 ] = pb.pb_xc + pb.pb_wc - 1 ;
	    pts[ 3 ] = pb.pb_yc + pb.pb_hc - 1 ;
	    vs_clip( scr_hndl, TRUE, &pts[ 0 ] ) ;
    	    fname_node( pb.pb_obj, &pb.pb_x ) ;
	    vs_clip( scr_hndl, FALSE, &pts[ 0 ] ) ;
	    }
	}
    return( pb.pb_currstate ) ;
} /* dr_code */


/* end of owindow.c */