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
/* 29/12/2000: Changed OUTPUT.RSC error message (orudge)           */
/*******************************************************************/

#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "gembind.h"
#include "xgembind.h"
#include "vdibind.h"
#include "windlib.h"
#include "rsrclib.h"

#include "output.h"
#include "odefs.h"
#include "ovar.h"

EXTERN VOID	movb() ;		/* outility.c */

EXTERN VOID	rc_union() ;		/* ooptimz.c */
EXTERN WORD	strlen() ;
EXTERN VOID	strcpy() ;
EXTERN WORD	max() ;

EXTERN VOID	v_get_driver_info() ;	/* vgetdriv.c */

/*----------------------------------------------------------------------*/
    VOID
set_mfdb( mf, image, width, hgt, ff, np )
    MFDB    *mf;
    LONG	image;
    WORD    width, hgt, ff, np;
{
    mf->mp = image;
    mf->fww = (width+15) / 16;
    mf->fwp = mf->fww * 16;
    mf->fh = hgt;
    mf->ff = ff;
    mf->np = np;
    mf->r1 = mf->r2 = mf->r3 = 0;
} /* set_mfdb */

/*----------------------------------------------------------------------*/
    VOID
set_icon( dst_tree, dst_obj, src_tree, src_obj )
LONG	dst_tree, src_tree ;
WORD	dst_obj, src_obj ;
{
ICONBLK		*dst_spec, *src_spec ;
OBJECT		*obj_ptr ;
GRECT		r ;
BYTE		*t_ptr ;		/* maintain unique text pointer */
WORD		wchar, dx_icon ;
    
    src_spec = get_spec( src_tree, src_obj ) ;
    dst_spec = get_spec( dst_tree, dst_obj ) ;
    obj_ptr = OB_PTR( dst_tree, dst_obj ) ;    
    t_ptr = dst_spec->ib_ptext ;
    wchar = src_spec->ib_wtext / strlen( src_spec->ib_ptext ) ;
    movb( sizeof( ICONBLK ), src_spec, dst_spec ) ;
    dst_spec->ib_ptext = t_ptr ;
    dst_spec->ib_wtext = max( dst_spec->ib_wicon, wchar * strlen( t_ptr ) ) ;
    dx_icon = ( dst_spec->ib_wtext - dst_spec->ib_wicon ) / 2 ;    
    obj_ptr->ob_x = obj_ptr->ob_x + dst_spec->ib_xicon - dx_icon ;
    dst_spec->ib_xicon = dx_icon ;
    dst_spec->ib_xtext = 0 ;
    dst_spec->ib_ytext = src_spec->ib_ytext ;
    movb( sizeof( GRECT ), &dst_spec->ib_xicon, &r ) ;
    rc_union( &dst_spec->ib_xtext, &r ) ;
    obj_ptr->ob_width = r.g_w + r.g_x ;
    obj_ptr->ob_height = r.g_h + r.g_y ;
} /* set_icon */

/*----------------------------------------------------------------------*/
    VOID
set_image( dst_tree, dst_obj, src_tree, src_obj )
LONG	dst_tree, src_tree ;
WORD	dst_obj, src_obj ;
{
BITBLK		*dst_spec, *src_spec ;
OBJECT		*obj_ptr ;
    
    src_spec = get_spec( src_tree, src_obj ) ;
    dst_spec = get_spec( dst_tree, dst_obj ) ;
    movb( sizeof( BITBLK ), src_spec, dst_spec ) ;
    obj_ptr = OB_PTR( dst_tree, dst_obj ) ;
    obj_ptr->ob_width  = dst_spec->bi_wb * 8 ;
    obj_ptr->ob_height = dst_spec->bi_hl ;
} /* set_image */

/*----------------------------------------------------------------------*/
    VOID
trnfm_bitblk( bt_blk )
BITBLK	*bt_blk;
{
	MFDB	mf;
	
	set_mfdb( &mf, bt_blk->bi_pdata, bt_blk->bi_wb * 8,
			  bt_blk->bi_hl, TRUE, 1 );
	vr_trnfm( scr_hndl, &mf, &mf );
}

/*----------------------------------------------------------------------*/
    VOID
trnfm_icon( icon )
	ICONBLK		*icon;
{
	MFDB	mf;
	
	set_mfdb( &mf, icon->ib_pmask, icon->ib_wicon, icon->ib_hicon,
			  TRUE, 1 );
	vr_trnfm( scr_hndl, &mf, &mf );
	mf.mp = icon->ib_pdata ;
	mf.ff = TRUE;
	vr_trnfm( scr_hndl, &mf, &mf );
}

/*----------------------------------------------------------------------*/
    VOID
trnfm_rsc()
{
WORD    	dial_num, n_dialogs ;
WORD    	obj, prev_obj, head;
LONG    	tree;
OBJECT		*obj_ptr ;
    
    n_dialogs = LWGET( global.ap_rsc + 22 ) ;
    for ( dial_num = n_dialogs-1 ; dial_num >= 0; dial_num-- )
    	{
	if ( dial_num != DEVCTREE )
	    {
            rsrc_gaddr( GAD_TREE, dial_num, &tree );
            obj = 0;
            while( obj >= 0 )
          	{
		obj_ptr = OB_PTR( tree, obj ) ;
            	switch ( obj_ptr->ob_type & 0x00FF )
            	    {
                    case G_ICON	: trnfm_icon( obj_ptr->ob_spec );
                    		  break;
                    case G_IMAGE: trnfm_bitblk( obj_ptr->ob_spec );
                    		  break;
                    default:	  break;
            	    } /* switch */
            	head = obj_ptr->ob_head ;
            	if ( head > 0 )
                    obj = head;
		else do
		    {
		    prev_obj = obj ;
		    obj = obj_ptr->ob_next ;
		    obj_ptr = OB_PTR( tree, obj ) ;
		    } while ( obj >= 0 && prev_obj == obj_ptr->ob_tail ) ;
		} /* while obj >= 0 */ 
	    } /* if !DEVCTREE */
    	} /* for all trees */
} /* trnfm_rsc() */

/*----------------------------------------------------------------------*/
/* Flags a item in resource data so that APP is called to draw an object*/
/* in the middle of the objc_draw.  Used for user defined menu types and*/
/* etc.									*/
    VOID
flag_userdef( ob_ptr ) 
OBJECT	*ob_ptr ;
{
    ob_ptr->ob_type = G_USERDEF ;
    ob_ptr->ob_spec = ADDR( &gl_udtray ) ;
    gl_udtray.ub_code = drawaddr ;
    gl_udtray.ub_parm = 0x0L ;
}  /* flag_userdef */

/*--------------------------------------------------------------*/
/* fixup preferences menu titles' pointers.			*/
    VOID
fix_menu()
{
OBJECT	*ob_ptr ;
WORD	x, ii ;

    ob_ptr = OB_PTR( gl_menu, FIRST_MENU ) ;
    x = ob_ptr->ob_x ;
    for ( ii = FIRST_MENU+1; ii <= LAST_MENU; ii++ )
        {
/* reset all preferences menus titles to begin at the same x location. 	*/
/* also set flags to hidden for first call to menu_bar().		*/
	ob_ptr = OB_PTR( gl_menu, ii ) ;
	ob_ptr->ob_x = x ;
	ob_ptr->ob_flags = HIDETREE ;
	}
    ob_ptr = OB_PTR( gl_menu, (SCRNITEM-1) ) ;
    x = ob_ptr->ob_x ;
/* Reset all menu item boxes x values to begin directly after Global menu. */
    ob_ptr = OB_PTR( gl_menu, (PAGEITEM-1) ) ;
    ob_ptr->ob_x = x ;
    ob_ptr = OB_PTR( gl_menu, (FILMITEM-1) ) ;
    ob_ptr->ob_x = x ;
} /* fix_menu() */

/*--------------------------------------------------------------*/
    BOOLEAN
rsrc_init()
{
WORD	ii ;

    if ( ! rsrc_load( ADDR( "OUTPUT.RSC" ) ) )
	{
	form_alert( 1, ADDR( "[3][Cannot find OUTPUT.RSC.][ Exit ]" ) ) ;
	return( FALSE ) ;
	}
    rsrc_gaddr( GAD_TREE, OUTMENUS, &gl_menu ) ;
    rsrc_gaddr( GAD_TREE, DEVCTREE, &dev_tree ) ;
    rsrc_gaddr( GAD_TREE, FILETREE, &tray_tr ) ;
    rsrc_gaddr( GAD_TREE, CNTLTREE, &cntl_tr ) ;
    rsrc_gaddr( GAD_STRING, SGEMEXT, &GEM_EXT ) ;
    rsrc_gaddr( GAD_STRING, SINFEXT, &INF_EXT ) ;
    rsrc_gaddr( GAD_STRING, SLISEXT, &LIS_EXT ) ;
    rsrc_gaddr( GAD_STRING, SOUTEXT, &OUT_EXT ) ;
    rsrc_gaddr( GAD_STRING, SIMGEXT, &IMG_EXT ) ;
    rsrc_gaddr( GAD_STRING, SGMPEXT, &GMP_EXT ) ;
    rsrc_gaddr( GAD_STRING, SUNTITLD, &UNTITLED ) ;
    rsrc_gaddr( GAD_STRING, SINFNAME, &INFNAME ) ;
    rsrc_gaddr( GAD_STRING, SAPPNAME, &APPNAME ) ;
    rsrc_gaddr( GAD_STRING, SPFILNAM, &PFILE_NAME ) ;
    for ( ii = FIL0NAME; ii <= FILXNAME; ii++ )
	flag_userdef( OB_PTR( tray_tr, ii ) ) ;
    trnfm_rsc() ;
    fix_menu() ;
    return( TRUE ) ;
} /* rsrc_init */
 
/*--------------------------------------------------------------*/
    VOID
do_icn_img( dst_tree, dst_obj, src_tree, src_obj ) 
LONG	dst_tree, src_tree ;
WORD	dst_obj, src_obj ;
{
WORD	ob_type ;

    ob_type = get_type( dst_tree, dst_obj ) ;
    if ( ob_type == G_ICON )
        set_icon( dst_tree, dst_obj, src_tree, src_obj ) ;
    else if ( ob_type == G_IMAGE )
        set_image( dst_tree, dst_obj, src_tree, src_obj ) ;
} /* do_icn_img */

/*--------------------------------------------------------------*/
    VOID
set_icn_specs( start, end )
WORD	start, end ;
{
LONG	p_tree1, p_tree2 ;
WORD	ii ;
    
    ii = start ;
    rsrc_gaddr( GAD_TREE, icon[ ii ].tree1, &p_tree1 ) ;
    rsrc_gaddr( GAD_TREE, icon[ ii ].tree2, &p_tree2 ) ;
    while ( ii <= end && icon[ ii ].tree1 != 0xFFFF )
	{
	if ( icon[ ii ].tree2 != 0 )
	    {
	    do_icn_img( p_tree1, icon[ ii ].obj1, p_tree2, icon[ ii ].obj2 ) ;
	    }
	++ii ;
	if ( icon[ ii ].tree1 != 0xFFFF && ii <= end )
	    {
	    if ( icon[ ii ].tree1 != icon[ ii-1 ].tree1 )
		rsrc_gaddr( GAD_TREE, icon[ ii ].tree1, &p_tree1 ) ;
	    if ( icon[ ii ].tree2 != icon[ ii-1 ].tree2 )
		rsrc_gaddr( GAD_TREE, icon[ ii ].tree2, &p_tree2 ) ;
	    }
	}
} /* set_icn_specs */

/*--------------------------------------------------------------*/
    VOID
do_ini_icon()
{
    if ( gl_hcell >= 12 )
	set_icn_specs( 0, ( sizeof( icon ) / sizeof( ICN_SPC ) ) - 1 ) ;
    else
	set_icn_specs( 0, MX_DVCS-1 ) ;
} /* do_ini_icon */


/*--------------------------------------------------------------*/
    VOID
ini_desk()
{
WORD	new_x, new_y ;

    wind_get( 0, WF_WXYWH, &gl_desk.g_x, &gl_desk.g_y, &gl_desk.g_w, &gl_desk.g_h ) ;
    set_x( dev_tree, ROOT, gl_desk.g_x ) ;
    set_y( dev_tree, ROOT, gl_desk.g_y ) ;
    set_width( dev_tree, ROOT, gl_desk.g_w ) ;
    set_height( dev_tree, ROOT, gl_desk.g_h ) ;
    menu_width = get_width( cntl_tr, ROOT ) ;
    wind_set( 0, WF_NEWDESK, LLOWD( dev_tree ), LHIWD( dev_tree ), ROOT, 0 ) ;
    menu_bar( gl_menu, TRUE ) ;	
    new_y = gl_desk.g_y + 2*gl_hbox - 1 ;
    new_x = gl_desk.g_x + gl_desk.g_w - 4*gl_wcell 
					- get_width( dev_tree, DEVCROOT ) ;
    set_x( dev_tree, DEVCROOT, new_x ) ;
    set_y( dev_tree, DEVCROOT, new_y ) ;
    do_ini_icon() ;
    do_redraw( dev_tree, 0, gl_desk.g_x, gl_desk.g_y, gl_desk.g_w, gl_desk.g_h ) ;
} /* ini_desk */

/*--------------------------------------------------------------*/
    VOID
get_icn_ptrs( dev_type, hres_icon, lres_icon )
WORD	dev_type ;
WORD	*hres_icon, *lres_icon ;
{
    switch ( dev_type )
	{
	case SCRN_TYPE : *hres_icon = HSCRNICN ;
			 *lres_icon = LSCRNICN ;
			 break ;
	case PLOT_TYPE : *hres_icon = HPLOTICN ;
			 *lres_icon = LPLOTICN ;
			 break ;
	case PRNT_TYPE : *hres_icon = HPRNTICN ;
			 *lres_icon = LPRNTICN ;
			 break ;
	case CMRA_TYPE : *hres_icon = HCMRAICN ;
			 *lres_icon = LCMRAICN ;
			 break ;
	} /* switch */
} /* get_icn_ptrs */

/*--------------------------------------------------------------*/
    VOID
dvc_icons( dev_cnt )
WORD	dev_cnt ;
{
WORD	hres_icon, lres_icon ;
WORD	ii ;
ICONBLK	*icn_blk ;

    for ( ii = 0; ii < dev_cnt; ii ++ )
        {
	icn_blk = get_spec( dev_tree, ii + DEV01ICN ) ;
	icn_blk->ib_ptext = dvc[ ii ].id ;
	get_icn_ptrs( dvc[ ii ].type, &hres_icon, &lres_icon ) ;
	icon[ ii ].tree2 = ( gl_hcell >= 12 ) ? HRESICNS :  LRESICNS ;
    	icon[ ii ].obj2  = ( gl_hcell >= 12 ) ? hres_icon : lres_icon ;
	set_flags( dev_tree, ii + DEV01ICN, SELECTABLE ) ;
	set_state( dev_tree, ii + DEV01ICN, NORMAL ) ;
	}
    for ( ii = dev_cnt+DEV01ICN; ii <= DEV20ICN; ii++ )
	set_flags( dev_tree, ii, HIDETREE ) ;
} /* dvc_icons */

/*--------------------------------------------------------------*/
    VOID
add_device( dvc_id, dev_cnt )
WORD	dvc_id, *dev_cnt ;
{
DVC_STRUCT	*dev_ptr ;
PDPREFS		*pref_ptr ;
FBYTE		*ptr ;

    dev_ptr = &dvc[ *dev_cnt ] ;
    v_get_driver_info( dvc_id, 2, dev_ptr->id ) ;
    dev_ptr->num = dvc_id ;
    dev_ptr->type = 1 << ( (dvc_id-1) / 10) ; /* yields 1,2,4,8,16 */
    if ( dev_ptr->type & ( PRNT_TYPE | PLOT_TYPE ) )
        {
	pref_ptr = &dev_ptr->prefs.pd ;
	movb( sizeof( PDPREFS ), &pd_defaults, pref_ptr ) ;
	if ( dev_ptr->type & PLOT_TYPE )
	    {
	    pref_ptr->paper_tray = MANUFEED ;
	    pref_ptr->pause = TRUE ;
	    }
	v_get_driver_info( dvc_id, 5, &pref_ptr->port ) ;
	}
    else if ( dev_ptr->type & SCRN_TYPE ) 
        {
	movb( sizeof( SDPREFS ), &sd_defaults, &dev_ptr->prefs.sd ) ;
	if ( *dev_ptr->id == NULL )	/* dual floppy: screen label == NULL*/
            {	
  	    rsrc_gaddr( GAD_STRING, SCREEN, &ptr ) ;
            strcpy( ptr, dev_ptr->id ) ;
	    }
	}
    else if ( dev_ptr->type & CMRA_TYPE ) 
	movb( sizeof( CDPREFS ), &cd_defaults, &dev_ptr->prefs.cd ) ;
    *dev_cnt += 1 ;
} /* add_device */

/*--------------------------------------------------------------*/
    WORD
setup_devs()
{
WORD	dev_cnt ;
WORD	ii ;
BYTE	info[ 129 ] ;

    dev_cnt = 0 ;
    for ( ii = 1; ii <= 50; ii++ )
        {
	v_get_driver_info( ii, 1, info ) ;
	if ( *info )
	    add_device( ii, &dev_cnt ) ;
	if ( ii == 30 )	     /* skip metafiles. do output type devices only */
	    ii += 10 ;
	}
    return( dev_cnt ) ;
} /* setup_devs */
 
/*--------------------------------------------------------------*/
    BOOLEAN
setup_rsc()
{
WORD	dev_cnt ;

    if ( !rsrc_init() )
	return( FALSE ) ;	/* panic flag - no resource file */
    dev_cnt = setup_devs() ;
    if ( !dev_cnt )
	return( FALSE ) ;
    dvc_icons( dev_cnt ) ;    
    ini_desk() ;
    return( TRUE ) ;
} /* setup_rsc */

/* end of orscinit.c */