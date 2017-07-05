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
#include "formlib.h"
#include "windlib.h"
#include "gembind.h"
#include "xgembind.h"
#include "vdibind.h"
#include "vdicmds.h"
#include "dosbind.h"
#include "rsrclib.h"

#include "output.h"
#include "odefs.h"
#include "ovar.h"

EXTERN WORD	SMUL_DIV() ;			/* util86.asm */
EXTERN WORD	RMUL_DIV() ;		
EXTERN VOID	b_stuff() ;

EXTERN WORD	open_mf() ;			/* obuffer.c */
EXTERN WORD	close_mf() ;
EXTERN BOOLEAN	search_path() ;
EXTERN WORD	rewind_mf() ;
EXTERN BOOLEAN	get_mf() ;
EXTERN VOID	get_pgsize() ;

EXTERN VOID	strcat() ;			/* ooptimz.c */
EXTERN WORD	strlen() ;
EXTERN VOID	strcpy() ;
EXTERN WORD	min() ;
EXTERN WORD	max() ;
		
EXTERN VOID	def_txt_atts() ;		/* owindow.c */
EXTERN VOID	pronto_mesg() ;

EXTERN WORD	f_alert() ;			/* outility.c */
EXTERN WORD	abs() ;
EXTERN WORD	atoi() ;
EXTERN VOID	pad_n_itoa() ;
EXTERN VOID	wswap() ;
EXTERN VOID	del_path() ;
EXTERN WORD	atoi() ;
EXTERN VOID	del_fname() ;
EXTERN VOID	movb() ;
EXTERN WORD	minmax() ;
EXTERN VOID	pswap() ;
EXTERN VOID	vopen() ;
EXTERN BOOLEAN	comp_ext() ;
EXTERN VOID	new_ext() ;

EXTERN BOOLEAN	OUTIMAG() ;			/* oimage.c */
EXTERN VOID	OUTGWRT() ;			/* ogemwrit.c */

EXTERN VOID	v_copies() ;			/* vcopies.c */
EXTERN VOID	v_tray() ;			/* vtray.c */

EXTERN BOOLEAN	is_serial() ;			/* ooptions.c */

/*--------------------------------------------------------------*/
    BOOLEAN
print_spooler( pstr )
	BYTE		*pstr;
{
	WORD		spl_id;
	WORD		ap_msg[8];

	if ( is_serial( &PRNT_PREFS ) )
	    return( FALSE ) ;
	spl_id = appl_find(ADDR("CALCLOCK"));
	if (spl_id < 0)
	  return(FALSE);
	ap_msg[0] = 100;
	ap_msg[1] = global.ap_id;
	ap_msg[2] = ( 0x0100 | PRNT_PREFS.port ) ; /* flag for binary file */
	ap_msg[3] = strlen(pstr);
	ap_msg[4] = LLOWD(ADDR(pstr));
	ap_msg[5] = LHIWD(ADDR(pstr));
	ap_msg[6] = atoi( get_teptext( cntl_tr, COPYSTXT ) ) ;
	ap_msg[7] = PRNT_PREFS.delete ;	/* delete tmp file when finished. */
	appl_write(spl_id, 16, ADDR(&ap_msg[0]));
	wind_update( 2 ) ;
	wind_update( 0 ) ;
	do
	    {
	    evnt_mesag( ADDR( gl_rmsg ) ) ;
	    if ( gl_rmsg[ 0 ] != 101 )
		{
		movb( sizeof( gl_smsg ), &gl_rmsg, &gl_smsg ) ;
		appl_write( global.ap_id, sizeof( gl_smsg ), ADDR( gl_smsg ) ) ;
		}
	    } while ( gl_rmsg[ 0 ] != 101 ) ;
	wind_update( 1 ) ;
	wind_update( 3 ) ;
	return(TRUE);
} /* print_spooler() */

/*--------------------------------------------------------------*/
    VOID
set_avail_tsizes()
{
    WORD	font ;
    WORD	size ;
    WORD	new_size ;
    WORD	psize ;
    WORD	start ;
    BOOLEAN	ok ;
    BYTE	name[ 32 ] ;		/* not used by OUTPUT */

    for ( font = 0 ; font < nfonts ; font++ )
    	{
	font_ids[ font ] = vqt_name( cur_hndl, font + 2, name ) ;
    	vst_font( cur_hndl, font_ids[ font ] ) ;
    	size = 32767 ;
    	psize = start = 0 ;
	ok = TRUE ;
    	do
    	    {
	    new_size = vst_point( cur_hndl, size, &char_width, &char_hgt,
					&cell_width, &cell_hgt ) ;
	    if (new_size > size)
		ok = FALSE ;
	    if ( ok )
		{
	    	buffer.AVL_PIXEL[ font ][ psize ] = char_hgt ;
	    	buffer.AVL_POINT[ font ][ psize++ ] = new_size ;
	    	if ( (psize - start) == (MX_FSIZES - 1) )
		    ok = FALSE ;
		if ( ok )
		    size = new_size - 1 ;
		}
            } while ( ok ) ;
	buffer.AVL_POINT[ font ][ psize ] = 0 ;
	buffer.AVL_PIXEL[ font ][ psize ] = 0 ;
    	}
} /* set_avail_tsizes */

/*--------------------------------------------------------------*/
    VOID
set_col_reps( handle )
WORD	handle ;
{
WORD	i ;
WORD	rgb_in[ 3 ] ;

    for ( i=0; i<MAX_COLORS; i++ )
	{
	rgb_in[ 0 ] = 10 * CMRA_PREFS.cmclr[ i ].red ;
	rgb_in[ 1 ] = 10 * CMRA_PREFS.cmclr[ i ].green ;
   	rgb_in[ 2 ] = 10 * CMRA_PREFS.cmclr[ i ].blue ;
	vs_color( handle, i, &rgb_in ) ;
	} /* for */
} /* set_col_reps */

/*--------------------------------------------------------------*/
    VOID
grey_box( xyr )
WORD	*xyr ;
{
WORD	attribs[ 5 ] ;

    vqf_attributes( cur_hndl, attribs ) ;
    vsf_interior( cur_hndl, HATCH ) ;	
    vsf_color( cur_hndl, BLACK ) ;		
    vsf_style( cur_hndl, 7 ) ;		/* a big 45 diagonal */
    vswr_mode( cur_hndl, 1 ) ;		/* REPLACE */
    vsf_perimeter( cur_hndl, TRUE ) ;
    v_bar( cur_hndl, xyr ) ;
    vsf_interior( cur_hndl, attribs[ 0 ] ) ;	
    vsf_color( cur_hndl, attribs[ 1 ] ) ;
    vsf_style( cur_hndl, attribs[ 2 ] ) ;	
    vswr_mode( cur_hndl, attribs[ 3 ] ) ;		
    vsf_perimeter( cur_hndl, attribs[ 4 ] ) ;
} /* grey_box */


/*--------------------------------------------------------------*/
    VOID
do_v_tray()
{
WORD	tray ;

    switch ( PRNT_PREFS.paper_tray )
        {
	case MANUFEED :	tray = -1 ;
			break ;
	case DFLTTRAY :	
	case CONTFORM : tray = 0 ;
			break ;
	default :	tray = PRNT_PREFS.paper_tray - AUXNTRAY + 1 ;
			break ;
	}
    v_tray( cur_hndl, tray ) ;
} /* do_v_tray() */

/*--------------------------------------------------------------*/
    WORD
open_dvc()	/* Open output device	*/
{
	WORD 	handle, workin[ 103 ] ;
	WS   	opn ;
	EXTND 	ex ;
	WORD 	ii ;
    	BYTE	name[ 32 ] ;		/* not used by OUTPUT */

    b_stuff( &ex, sizeof( EXTND ), NULL ) ;
    is_spool = ( (DEV_TYPE & ( PRNT_TYPE | PLOT_TYPE ) ) 
		&& PRNT_PREFS.to_file && PRNT_PREFS.bkgrnd && bkgnd_ok ) ;
    if ( DEV_TYPE & SCRN_TYPE )
	{
	handle = scr_hndl ;
	vq_extnd( handle, 0, &opn ) ; /* open wkstn values */
	}
    else
	{
	workin[ 0 ] = dvc[ cur_opts.cur_dvc ].num ;
	for ( ii=1 ; ii<10 ; ii++ )
	    workin[ ii ] = 1 ;
	workin[ 10 ] = 2 ;
	if ( DEV_TYPE & ( PRNT_TYPE | PLOT_TYPE ) )
	    {
	    workin[ 11 ] = PRNT_PREFS.to_file ? FILEPORT : DEFAPORT ;
	    workin[ 11 ] |= CUSTOMSZ ;
	    if ( PRNT_PREFS.to_file )
	        {
		for ( ii = 0; PRNT_PREFS.file[ ii ] != NULL; ii++ )
		    workin[ 12 + ii ] = (WORD)( PRNT_PREFS.file[ ii ] ) ;
		workin[ 12 + ii ] = 0 ;
		}
	    workin[ 101 ] = PRNT_PREFS.paper_width ;
	    workin[ 102 ] = PRNT_PREFS.paper_height ;
	    }
	v_opnwk( workin, &handle, &opn ) ;
	}
    if ( !handle )		/* Device not found */
    	return( FALSE ) ;
    cur_hndl = handle ;
    if ( DEV_TYPE & PRNT_TYPE )
        v_copies( handle, atoi( get_teptext( cntl_tr, COPYSTXT ) ) ) ;
    if ( DEV_TYPE & ( PRNT_TYPE | PLOT_TYPE ) )
        do_v_tray() ;
    dev.xres    = opn.ws_xres ;
    dev.yres 	= opn.ws_yres ;
    dev.wpixel  = opn.ws_wpixel ;
    dev.hpixel  = opn.ws_hpixel ;
    dev.minwch  = opn.ws_minwch ;
    dev.minhch  = opn.ws_minhch ;
    dev.maxwch  = opn.ws_maxwch ;
    dev.maxhch  = opn.ws_maxhch ;
    vq_extnd( handle, 1, &ex ) ;
    dev.nplanes   = ex.ws_nplanes ;
    dev.maxptsin  = ex.ws_maxptsin ;
    dev.maxintin  = ex.ws_maxintin ;
    dev.xdpi      = ex.ws_xdpi ;
    dev.ydpi      = ex.ws_ydpi ;
    dev.img_rot   = ex.ws_imgrot ;
    dev.flags 	  = ex.ws_flags ;
    nfonts = min( MX_FONTS, vst_load_fonts( handle, 0 ) ) ;
    if ( DEV_TYPE & ( SCRN_TYPE | CMRA_TYPE ) )
	set_avail_tsizes() ;	/* makes up table of available fonts, sizes */
    else
	font_ids[ 0 ] = vqt_name( cur_hndl, 2, name ) ;	/*2=1st graphic font*/
    fonts_loaded = TRUE ;
    cur_font = font_ids[ 0 ] ; /* use first graphic font - usually Swiss */
    if ( !dev.ydpi || !dev.xdpi || 
		(nfonts && ( DEV_TYPE & ( SCRN_TYPE | CMRA_TYPE ) ) ) )
	{
	vst_font( handle, cur_font ) ;
	dev.ydpi = SMUL_DIV( cell_hgt, 72, vst_point( handle, 10, &char_width,
				&char_hgt, &cell_width, &cell_hgt ) ) ;
	dev.xdpi = SMUL_DIV( dev.ydpi, dev.hpixel, dev.wpixel ) ;
	}
    dev.xlen = (LONG)( dev.xres+1 ) * 100L * CM_PER_INx100 / (LONG)(dev.xdpi);
    dev.ylen = (LONG)( dev.yres+1 ) * 100L * CM_PER_INx100 / (LONG)(dev.ydpi);
    grf_mode = TRUE ;
    if ( DEV_TYPE & CMRA_TYPE )
	{
	vsp_film( handle, CMRA_PREFS.film + 1, CMRA_PREFS.lgt - 3 ) ;
	set_col_reps( handle ) ;
	}
    return( handle ) ;
} /* open_dvc */

/*--------------------------------------------------------------*/
    VOID
upd_pcnt( i, str )		/* Update print count dialog */
	WORD i ;		/* Number printed */
	BYTE *str ;		/* File name	  */
{
	upd_itep( pcnt_tree, NUMPRTD, i ) ;
	upd_teptext( pcnt_tree, CURPIC, str ) ;
} /* upd_pcnt */

/*--------------------------------------------------------------*/
    VOID
remv_pcnt( x, y, w, h )	/* remove print count dialog */
	WORD x, y, w, h ;	/* Print count dialog extent */
{
	end_dial( pcnt_tree, x, y, w, h ) ;
	LSTRCPY( ADDR( "0" ), get_teptext( pcnt_tree, NUMPRTD ) ) ;
	LSTRCPY( ADDR( "" ), get_teptext( pcnt_tree, CURPIC ) ) ;
} /* remv_pcnt */

/*--------------------------------------------------------------*/
	WORD
justify( just, length, size )	/* Calculate justifcation offset */
	WORD just ;	/* justification type	*/
	WORD length ;	/* Length of side in pixels */
	WORD size ;	/* size of device in pixels */
{
	WORD offset, used ;

	used = length % size ;		/* Calculate remainder of page */
	if ( used == 0 && length != 0 )
	    used = size ;
	switch( just )
	    {
	    case TOPLEFT : 
		offset = 0 ;
		break ;
	    case CENTERED : 
		offset = ( size - used ) / 2 ;
		break ;
	    case BOTTOMRIGHT : 
		offset = size - used ;
		break ;
	    } /* switch ( just ) */
    return ( offset ) ;
} /* justify */


/*--------------------------------------------------------------*/
	UWORD
_umul_div( m1, m2, d1 )
LONG	m1 ;
WORD	m2, d1 ;
{
LONG	temp ;

    temp =  (LONG)(UWORD)( m2 ) ;
    temp *= (LONG)( 2 ) ;
    temp *= m1 ;
    temp /= (LONG)(UWORD)( d1 ) ;
    temp += (LONG)( 1 ) ;	/* rounds the result by effectively */
    temp /= (LONG)( 2 ) ;		/* adding one-half, then truncing */
    return( (UWORD)( temp ) ) ;    
} /* _umul_div */

/*--------------------------------------------------------------*/
/* Integer mul/div. Same as SMUL_DIV() except that the result	*/
/* is not rounded.						*/
	WORD
_imul_div( m1, m2, d1 )
WORD	m1, m2, d1 ;
{
LONG	temp ;

    temp =  (LONG)( m1 ) ;
    temp *= (LONG)( m2 ) ;
    temp /= (LONG)( d1 ) ;
    return( (WORD)( temp ) ) ;    
} /* _imul_div */

/*--------------------------------------------------------------*/
    VOID
trfm_mag( num, pts )     /* transform pairs of magintudes	*/
	WORD num, *pts ;
{
	WORD i ;

	for ( i=0 ; i<num ; i++ )
	{
	    *pts = _imul_div( *pts, x_size, mf_xlength ) ;
	    *pts = ( *pts > 0 ) ? *pts : -*pts ;
	    pts++ ;
	    *pts = _imul_div( *pts, y_size, mf_ylength ) ;
	    *pts = ( *pts > 0 ) ? *pts : -*pts ;
	    if ( rotate )
		wswap( (pts-1), pts ) ;
	    pts++ ;
	}
} /* trfm_mag */

/*--------------------------------------------------------------*/
    VOID
trfm_pts( num, pts )		/* Transform coordinates	*/
	WORD num, *pts ;
{
	WORD i ;

	for ( i=0 ; i<num ; i++ )
	{
	    *pts = dvc_nxoff+_imul_div( ( *pts-mf_xtrans ), x_size, mf_xlength ) ;
	    pts++ ;
	    *pts = _imul_div( ( *pts-mf_ytrans ), y_size, mf_ylength ) ;
	    if ( rotate )
	    {
		wswap( (pts-1), pts ) ;
		*( pts-1 ) = ( dvc_nyoff + nypixel - 1 ) - *(pts-1) ;
	    }
	    else
	    {
		*pts += dvc_nyoff ;
	    }
	    pts++ ;
	}

} /* trfm_pts */


/*--------------------------------------------------------------*/
    VOID
flip_mag( num, pts )		/* Change magintude from y to x pixels */
	WORD num, *pts ;
{
	WORD i, temp ;

	for ( i=0 ; i< num ; i++ )
	{
	    temp = _imul_div( *pts, dvc_hpixel, dvc_wpixel ) ;
	    *pts = _imul_div( *( pts+1 ), dvc_wpixel, dvc_hpixel ) ;
	    *( ++pts ) = temp ;
	    pts++ ;
	}
} /* flip_mag */


/*--------------------------------------------------------------*/
	WORD
_smuldiv( m1, m2, d1 )
WORD	m1, m2, d1 ;
{
WORD	sign ;
ULONG	temp ;

    sign  = ( m1 < 0 ) ? (-1) : 1 ;
    sign *= ( m2 < 0 ) ? (-1) : 1 ;
    sign *= ( d1 < 0 ) ? (-1) : 1 ;
    m1 = abs( m1 ) ;
    m2 = abs( m2 ) ;
    d1 = abs( d1 ) ;
    temp  = (ULONG)( m1 ) ;
    temp *= 0x2L ;
    temp *= (ULONG)( m1 ) ;
    temp /= (ULONG)( d1 ) ;
    temp += 0x1L ;
    temp /= 0x2L ;
    return( sign * (WORD)( temp ) ) ;    
} /* _smuldiv */

/*--------------------------------------------------------------*/
	WORD
comp_ratios()
{
    WORD 	ret ;
#if I8086
    UWORD	xperu, yperu ;    
#endif
#if MC68K
    LONG	xperu, yperu;
#endif

#if I8086
    xperu = _umul_div( (LONG)(dvc_wpixel)*100, dvc_nxpixel+1, page_width  ) ;
    yperu = _umul_div( (LONG)(dvc_hpixel)*100, dvc_nypixel+1, page_height ) ;
    if ( abs( 100 - _umul_div( (LONG)(xperu), 100, yperu ) ) <= 2 )  
#endif

#if MC68K
    xperu = ((LONG)( dvc_wpixel)* 100 * (LONG) (dvc_nxpixel + 1) ) / (LONG) page_width ;
    yperu = ((LONG)( dvc_hpixel)* 100 * (LONG) (dvc_nypixel + 1) )/ (LONG) page_height ;
    if ( abs( 100 - (WORD)( (xperu * 100) / yperu ) ) <= 2 )
#endif
 
	ret = 2 ;		/* ratios are within 2 percent */
    else if ( xperu > yperu )
	ret = 1 ;	
    else
	ret = 0 ;	
    return( ret ) ;
} /* comp_ratios */

/*--------------------------------------------------------------*/
    VOID
out_adjust()
{
WORD	gr_ypixel,	/* pixels per graphics slice on printer  */
	cr_ypixel, 	/* pixels per carriage return */
	junk,
	dx, dy,		/* image width */
	factor,		/* scan height division factor. */
	gem_ypixel ;	/* pixels in y direction for gem picture */
RECTANGLE	out_pgsz ;

    vq_scan(cur_hndl, &gr_ypixel, &junk, &cr_ypixel, &junk, &factor );
    gem_ypixel = (cr_ypixel * nn) / factor ; /* Calculate height of picture */
    gem_ypixel = ( gem_ypixel / gr_ypixel ) * gr_ypixel ; /* trunc to last slice */
    dvc_nxoff = dvc_nyoff = 0 ;
    out_pgsz.left = out_pgsz.right = mf_xtrans ;
    out_pgsz.top = out_pgsz.bottom = mf_ytrans ;
    out_pgsz.right += mf_xlength ;
    out_pgsz.bottom += mf_ylength ;
    mf_ytrans = out_extent.top ;	/* leave trans in world coords */
    trfm_pts( 2, &out_extent ) ; 	/* out_extent goes to device coords */
    trfm_pts( 2, &out_pgsz ) ; 		/* out_pgsz goes to device coords */
    dx = abs( out_pgsz.right - out_pgsz.left ) ;
    dy = abs( out_extent.bottom - out_extent.top ) ;
    dvc_nxoff = max( 0, ( (dvc_nxpixel - dx ) / 2) ) ;
    dvc_nyoff = max( 0, ( (gem_ypixel - dy ) / 2) ) ;
    out_extent.right  = min( dvc_nxpixel, (out_extent.right + dvc_nxoff) ) ;
    out_extent.bottom = gem_ypixel + out_extent.top - 1 ;
    out_extent.left   = max( 0, out_extent.left ) ;
    out_extent.top    = max( 0, out_extent.top ) ;
} /* out_adjust */

/*--------------------------------------------------------------*/
    VOID		/* determine whether or not to rotate 	*/
find_rotate( xdpi, ydpi ) 
WORD	*xdpi, *ydpi ;
{
UWORD	dvc_width, dvc_height, npgs_nrml, npgs_rotd ;

    rotate = FALSE ;
    horz_just = ( DEV_TYPE & ( SCRN_TYPE | CMRA_TYPE ) ) ? 
    		CENTERED : PRNT_PREFS.horzjust ;
    vert_just = ( DEV_TYPE & ( SCRN_TYPE | CMRA_TYPE ) ) ? 
    		CENTERED : PRNT_PREFS.vertjust ;
    let_rot = !( is_out_file || ( DEV_TYPE & SCRN_TYPE ) ) ;
    if ( !let_rot )
	return ;
    if ( make_fit )
	{
	rotate = ( page_width > page_height ) ;
	if ( dev.xlen > dev.ylen ) 
	    rotate = ( !rotate ) ;
	}
    else	/* not make_fit */
	{
	dvc_width = (UWORD)( dev.xlen / (LONG)100 ) ;
	dvc_height = (UWORD)( dev.ylen / (LONG)100 ) ;
	npgs_nrml = ( ( page_width + dvc_width - 1)/ dvc_width ) *
			( ( page_height + dvc_height - 1)/ dvc_height ) ;
	npgs_rotd = ( ( page_height + dvc_width - 1)/ dvc_width ) *
			( ( page_width + dvc_height - 1)/ dvc_height ) ;
	rotate = npgs_rotd < npgs_nrml ;
	} 
    if ( rotate )
	{				/* Rotate transform */
	wswap( &dvc_nxpixel, &dvc_nypixel ) ;
	wswap( &dvc_wpixel, &dvc_hpixel ) ;
	wswap( &horz_just, &vert_just ) ;
	wswap( xdpi, ydpi ) ;
	}	
} /* find_rotate */

/*--------------------------------------------------------------*/
	VOID		/* Set transformation size values */
set_npixel( xdpi, ydpi ) 
WORD	xdpi, ydpi ;
{
WORD	temp ;

    if ( ! make_fit )
	{		/* Set full scale number of pixels	*/
	nxpixel = SMUL_DIV( page_width, xdpi, CM_PER_INx100 ) ;
	nypixel = SMUL_DIV( page_height, ydpi, CM_PER_INx100 ) ;
	bf_height = org_height ;
	}	/* if not make_fit */
    else
	{				/* Set for best fit		*/
	nxpixel = dvc_nxpixel + 1 ;
	nypixel = dvc_nypixel + 1 ;
	switch ( comp_ratios() )
	    {
	    case 0:  	/* Use full x range - reset y	*/
		    temp = SMUL_DIV( nxpixel, dvc_wpixel, dvc_hpixel ) ;
	            nypixel = SMUL_DIV( temp, page_height, page_width ) ;
		    break ;
	    case 1:		/* Use full y range - reset x	*/
	            temp = SMUL_DIV( nypixel, dvc_hpixel, dvc_wpixel ) ;
	            nxpixel = SMUL_DIV( temp, page_width, page_height ) ;
		    break ;
	    case 2: /* target & source so close, use full range of both */
		    break ;
	    } /* switch  comp_ratios() */
			/* Set best fit page height	*/
	bf_height = SMUL_DIV( nypixel+1, CM_PER_INx100, dev.ydpi ) ;
	}	/* else make_fit */
    x_size = nxpixel ;
    y_size = nypixel ;
} /* set_npixel */

/*--------------------------------------------------------------*/
	VOID
set_trfm()		/* Set transformation parameters	*/
{
WORD	xdpi, ydpi ;

    get_pgsize() ;	/* Get page size and coordinates range from metafile*/
    org_height  = page_height ;	/* save original page height */
    dvc_nxpixel = dev.xres ;	/* Set device paramters to original */
    dvc_nypixel = dev.yres ;	/* values			    */
    dvc_wpixel  = dev.wpixel ;
    dvc_hpixel  = dev.hpixel ;
    xdpi 	= dev.xdpi ;
    ydpi 	= dev.ydpi ;
    find_rotate( &xdpi, &ydpi ) ;
    set_npixel( xdpi, ydpi ) ;
    if ( is_out_file )
        {
        if ( !is_1st_word )
	    out_adjust() ;
	}
    else	/* Set justification offsets */
	{
	nxoffset = justify( horz_just, nxpixel, dvc_nxpixel + 1 ) ;
	nyoffset = justify( vert_just, nypixel, dvc_nypixel + 1 ) ;
	}	/* else not is_out_file */
} /* set_trfm */

/*--------------------------------------------------------------*/
	WORD
full_circle( beg, end )		/* Check for full circle */
	WORD beg, end ;
{
	return( abs( end - beg ) >= 3600 ) ;
} /* full_circle */


/*--------------------------------------------------------------*/
    VOID
rot_ang( num, inti )		/* Rotate angle		*/
	WORD num, *inti ;
{
	WORD i ;

	for ( i=0 ; i < num ; i++ )
	{
	    *inti = *inti -900 ;
	    if ( *inti < 0 )
	    {
		*inti = 3600 + *inti ;
	    }
	    inti++ ;
	}
} /* rot_ang */
	

/*--------------------------------------------------------------*/
	VOID
reset_mf()		/* Reset metafile to start of vdi commands, */
{			/* skipping any user defines at beginning.  */
    rewind_mf() ;
    while ( get_mf() && (OPCODE == 5 && SUBCODE == 99) )
	{
	}
} /* reset_mf */


/*--------------------------------------------------------------*/
	WORD
check_esc()		/* check if escape key pressed	*/
{
	WORD ev_which, flags, rets[ 6 ] ;

	flags = MU_KEYBD | MU_TIMER ;
	ev_which = evnt_multi( flags, 0, 0, 0, 0, 0, 0, 0, 0, 
			       0, 0, 0, 0, 0, 0x0L, 0, 0, 
			       &rets[ 0 ], &rets[ 1 ], &rets[ 2 ], 
			       &rets[ 3 ], &rets[ 4 ], &rets[ 5 ] ) ;
	if ( ev_which & MU_KEYBD )
	{
	    return( rets[ 4 ] == ESCAPE ) ;
	}

	return( FALSE ) ;
} /* check_esc */


/*--------------------------------------------------------------*/
	WORD
upd_count( slot, direction )	/* Update list slot count */
	WORD *slot, direction ;
{

    if ( brk_out )
	return( TRUE ) ;
    *slot += direction ;
    if ( *slot >= next_slot )
	{
	if ( (DEV_TYPE & SCRN_TYPE) && SCRN_PREFS.cycle_until_esc )
	    {		/* Cycle if at end of list */
	    *slot = 0 ;
	    return( FALSE ) ;
	    }
	else		/* stop if at end of list */
	    return( TRUE ) ;
	}
    *slot = max( 0, *slot ) ;
    return( FALSE ) ;
} /* upd_count */


/*--------------------------------------------------------------*/
	WORD
hold_pix( direction ) 	/* Hold display */
	WORD direction ;
{
	WORD flags, rets[ 6 ], ev_which, done ;

	if ( brk_out )
	    return( direction ) ;
	flags = ( MU_KEYBD | MU_BUTTON ) ;
	if ( (DEV_TYPE & SCRN_TYPE) && !SCRN_PREFS.wait_for_key ) 
	    flags |= MU_TIMER ;		/* If timer set check for time */
	done = FALSE ;
	while ( !done )
	{
	    ev_which = evnt_multi( flags, 
				   1, 0xFFFF, TRUE, /* wait for any down */ 
				   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				   ADDR( gl_rmsg ), 
				   SCRN_PREFS.time * 1000, 0, 
				   &rets[ 0 ], &rets[ 1 ], &rets[ 2 ], 
				   &rets[ 3 ], &rets[ 4 ], &rets[ 5 ] ) ;
	    if ( ev_which & MU_KEYBD )
	    {
		done = TRUE ;
		switch ( rets[ 4 ] )
		    {
		    case ESCAPE :   brk_out = TRUE ;
				    break ;
		    case SPACE :
		    case TAB :
		    case PGDN :
		    case CRETURN :
		    case RARROW :
		    case DARROW :  direction = 1 ;
				    break ;
		    case BSPACE :
		    case BTAB :
		    case UARROW :
		    case LARROW :
		    case PGUP :	    direction = -1 ;
				    break ;
		    default :	    done = FALSE ;
				    break ;
		    }
	    }
	    if ( ev_which & MU_BUTTON || ev_which & MU_TIMER )
	    {
		direction = 1 ;
		done = TRUE ;
	    }
	}
	return( direction ) ;
} /* hold_pix */			    

/*--------------------------------------------------------------*/
    VOID
case_gdp()		/* transform gdp data */
{
    switch ( SUBCODE )
	{
	case CIRCLE : 
		trfm_pts( 1, &ptsin[ 0 ] ) ;
		trfm_mag( 1, &ptsin[ 4 ] ) ;
		if ( rotate )
		    flip_mag( 1, &ptsin[ 4 ] ) ;
		break ;
	case PIE : 
	case ARC : 
		trfm_pts( 1, &ptsin[ 0 ] ) ;
		trfm_mag( 1, &ptsin[ 6 ] ) ;
		if ( rotate )
		    {
		    flip_mag( 1, &ptsin[ 6 ] ) ;
		    if ( !full_circle( intin[ 0 ], intin[ 1 ] ) )
			rot_ang( 2, &intin[ 0 ] ) ;
		    }
		break ;
	case ELLIP_ARC : 
	case ELLIP_PIE : 
		if ( rotate && !full_circle( intin[ 0 ], intin[ 1 ] ) )
		    rot_ang( 2, &intin[ 0 ] ) ;
	case ELLIPSE : 
		trfm_pts( 1, &ptsin[ 0 ] ) ;
		trfm_mag( 1, &ptsin[ 2 ] ) ;
		break ;
	case FORMT_TEXT : 
		trfm_pts( 1, &ptsin[ 0 ] ) ;
		trfm_mag( 1, &ptsin[ 2 ] ) ;
		if ( rotate )
		    flip_mag( 1, &ptsin[ 2 ] ) ;
		break ;
	default : 
		trfm_pts( NUM_PTSIN, &ptsin[ 0 ] ) ;
	} /* switch SUBCODE */
} /* case_gdp */

/*--------------------------------------------------------------*/
    BOOLEAN
close_enough( close_size, hgt_pixels )
WORD	*close_size, hgt_pixels ;
{
WORD	*ptsize, *pxsize, *start, delta, index, ii ;
BOOLEAN	ok ;

    index = -1 ;
    for ( ii = 0; ii < nfonts; ii++ )
        {
	if ( font_ids[ ii ] == cur_font )
	    {
	    index = ii ;
	    ii = nfonts ;	/* break out of this for loop */
	    }
	}
    if ( index < 0 )	/* the font requested is not loaded for this device */
        return( FALSE ) ;
    pxsize = start = buffer.AVL_PIXEL[ index ] ;
    ptsize = buffer.AVL_POINT[ index ] ;
    ok = TRUE ;
    while ( ok )
	{
	if ( *pxsize == 0 )
	    return( FALSE ) ;
        if ( (WORD)(pxsize - start) == (MX_FSIZES - 1) )
	    return( FALSE ) ;
	delta = abs( *pxsize - hgt_pixels ) ;	/* within one pixel */
	if ( delta <= FONT_EPS )
	    ok = FALSE ;
	if ( ok )
	    {
	    delta = abs( *ptsize - intin[ 0 ] ) ;    /* or within one point */
	    if ( delta <= FONT_EPS )
	    	ok = FALSE ;
	    if ( ok )
		{
		pxsize++ ;
		ptsize++ ;
		} /* if ok */
	    } /* if ok */
	} /* while ok */
    *close_size = *ptsize ;
    return( TRUE ) ;
} /* close_enough */

/*--------------------------------------------------------------*/
/* intin[ 0 ] holds requested height in points.			*/
/* Must translate to scaled pixels and then look for close size */
/* so that whole sizes are used as much as possible.		*/
    VOID
fudge_sizes()
{
WORD 	close_size, hgt_pixels ;

    hgt_pixels = RMUL_DIV( dev.ydpi,   intin[ 0 ], 72 ) ;
    hgt_pixels = RMUL_DIV( hgt_pixels, char_hgt,   cell_hgt ) ;
    if ( close_enough( &close_size, hgt_pixels ) )
	{
	intin[ 0 ] = close_size ;	/* use close point size */ 
	return ;
	}
    if ( DEV_TYPE & CMRA_TYPE )		/* cameras don't do scaled text well*/
	return ;
    OPCODE = TEXTHGTCMD ;    /* use absolute text size - driver interpolates*/
    NUM_PTSIN = 1 ;
    NUM_INTIN = ptsin[ 0 ] = intin[ 0 ] = 0 ;
    ptsin[ 1 ] = hgt_pixels ;
} /* fudge_sizes */

/*--------------------------------------------------------------*/
/* This functn does a special preliminary search for .IMG files	*/
/* in Desktop Publisher .GMP files in the .GMP file's directory.*/
/* This search is usually done last, but since DP's "Copy Doc"	*/
/* command will copy files around and NOT redo the .IMG paths,	*/
/* this search will satisfy the oddball case where a document on*/
/* a floppy drive was copied elsewhere and the drive door is open.*/
    BOOLEAN
is_DP_kludge( file )
BYTE	*file ;
{
BYTE	tmp[ FNAME_LENGTH ] ;

    if ( !( is_gmp_file && buffer.CUR_FNAME[ 0 ] != file[ 0 ] 
				    && file[ 0 ] < 'C' ) )
        return( FALSE ) ;
    del_fname( buffer.CUR_FNAME, tmp ) ;
    del_path( file, &tmp[ strlen( tmp ) ] ) ;
    if ( dos_sfirst( tmp, 0 ) )
	{
	strcpy( tmp, file ) ;
	return( TRUE ) ;
	}
    return( FALSE ) ;
} /* is_DP_kludge */

/*--------------------------------------------------------------*/
    VOID
add_path( cntrl3, strt, inttin, file )
WORD	*cntrl3, strt, *inttin ;
BYTE	*file ;
{
WORD	cnt ;
BYTE	*pp, img_name[ NAME_LENGTH ] ; 

    for ( pp = &file[ 0 ], cnt = strt ; cnt < *cntrl3; pp++ )
	*pp = LLOBT( inttin[ cnt++ ] ) ;	/* pack into bytes */
    *pp = NULL ;
    if ( file[ 1 ] == COLON )
	{
	if ( is_DP_kludge( file ) )
	    return ;
	if ( dos_sfirst( ADDR( file ), 0 ) )
	    return ;
	del_path( file, img_name ) ;    
	}
    else
	strcpy( file, img_name ) ;
    del_fname( buffer.CUR_FNAME, file ) ;   /* then try the .GEM file's dir */
    strcat( img_name, file ) ;
    search_path( file ) ;  /* look in this dir, then current, then along sys search path */
} /* add_path */


/*--------------------------------------------------------------*/
	BOOLEAN
do_image_cmd()
{
WORD	xy[ 4 ], delta[ 2 ] ; /* target rect, in dev coords & dx,dy */
WORD	flg_sz, flags[ 8 ] = { 0,0,0,0,0,0, BLACK, WHITE } ;
BYTE	fname[ FNAME_LENGTH ] ;

    flg_sz = (SUBCODE == IMGFCMD) ? 5 : 8 ;
    movb( flg_sz * sizeof( WORD ), &intin, &flags ) ;
    movb( sizeof( xy ), &ptsin, &xy ) ;
    add_path( &NUM_INTIN, flg_sz, &intin, fname ) ; /* add path to image name in gem file */
    if ( fname[ 1 ] != COLON )
	return( FALSE ) ;
    delta[ 0 ] = abs( ptsin[ 2 ] - ptsin[ 0 ] ) + 1 ;
    delta[ 1 ] = abs( ptsin[ 3 ] - ptsin[ 1 ] ) + 1 ;
    trfm_mag( 1, &delta[ 0 ] ) ;/* trfm_pts() doesn't always preserve       */
    --delta[ 0 ] ;		/* magnitude, which is important to         */
    --delta[ 1 ] ;
    trfm_pts( 2, &xy ) ;	/* the very pixel when doubling is likely.  */
    if ( xy[ 2 ] > xy[ 0 ] )	/* So the idea here is to preserve the      */
       xy[ 2 ] = xy[ 0 ] + delta[ 0 ];/* magnitude via trfm_mag() and then     */
    else			/* add the proper deltas to the rectangle   */
       xy[ 0 ] = xy[ 2 ] + delta[ 0 ];/* sent to the printers in v_bit_image().*/
    if ( xy[ 3 ] > xy[ 1 ] )	/* <-this check is in case the rectangle is */
       xy[ 3 ] = xy[ 1 ] + delta[ 1 ];	/* for some wild reason not right-  */
    else			/* side-up.				    */
       xy[ 1 ] = xy[ 3 ] + delta[ 1 ];
    if ( rotate )
	wswap( &xy[ 0 ], &xy[ 2 ] ) ;
    if ( DEV_TYPE & SCRN_TYPE  && !is_img_file )  /* GWD file to screen */
	{
	grey_box( &xy ) ;
	return( TRUE ) ;
	}
    if ( make_fit && !is_gmp_file )
	{
	flags[ 1 ] = 0 ;	/* fract scale x (for printers) */
	xy[ 0 ] = xy[ 1 ] = 0 ;
	xy[ 2 ] = dev.xres ;
	xy[ 3 ] = dev.yres ;
	vs_clip( cur_hndl, TRUE, &xy ) ;
	}
    if ( DEV_TYPE & SCRN_TYPE )
	{
	return( OUTIMAG( &fname, &xy ) ) ;
	}
    if ( DEV_TYPE & (PRNT_TYPE | CMRA_TYPE) )
	{
	if ( !( DEV_TYPE & PRNT_TYPE ) ) 
	    vst_unload_fonts( cur_hndl, 0 ) ; /* make room in memory */
	if ( rotate )
	    wswap( &flags[ 1 ], &flags[ 2 ] ) ;
	flags[ 3 ] = rotate ? vert_just : horz_just ;
	flags[ 4 ] = rotate ? horz_just : vert_just ;
	flags[ 5 ] = rotate ? (flags[ 5 ] + 2700) % 3600 : flags[ 5 ] ;
	if ( !flags[ 5 ] || !dev.img_rot )
	    v_bit_image( cur_hndl, fname, flags[ 0 ], flags[ 1 ], flags[ 2 ],
		    flags[ 3 ], flags[ 4 ], &xy ) ;
	else
	   v_xbit_image( cur_hndl, fname, flags[ 0 ], flags[ 1 ], flags[ 2 ],
			   flags[ 3 ], flags[ 4 ], flags[ 5 ], flags[ 6 ], 
					   flags[ 7 ], &xy ) ;
	if ( !( DEV_TYPE & PRNT_TYPE ) )
	    vst_load_fonts( cur_hndl, 0 ) ;
	}
    return( TRUE ) ;
} /* do_image_cmd */

/*----------------------------------------------------------------------*/
    BOOLEAN
is_stroke_cmd( cmd )
WORD	cmd ;
{
    switch ( cmd )
        {
	case PLINECMD    :
	case PMARKERCMD  :
	case TEXTCMD  	 :
	case FILAREACMD  :
	case CELLARRCMD  :
	case GDPCMD 	 :
	case CONTOURFILL :
	case FILLRECTCMD : return( TRUE ) ;
    	case ESCAPECMD   : if ( SUBCODE == BITIMAGE || SUBCODE == ALPHATEXT )
				return( TRUE ) ;
	}
    return( FALSE ) ;
} /* is_stroke_cmd() */

/*----------------------------------------------------------------------*/
/* returns -1 at end of file, 1 at end of page, and 0 if aborting.	*/
/* also returns 0 if image file not found.				*/
    BOOLEAN
process_page()		/* draw device page	*/
{
    WORD 	count, this_cmd, last_cmd ;
    BOOLEAN	ret ;

    this_cmd = OPCODE ;
    last_cmd = 0 ;
    count = 19 ;
    ret = FALSE ;
    while ( this_cmd != 0xFFFF )
	{
	count = ( count + 1 ) % 20 ;
	if ( count == 0 )	/* check for ESC key each 20 calls to vdi */
	    {
	    brk_out = check_esc() ;
	    if ( brk_out )
		return( FALSE ) ;
	    }
	if ( ( this_pgnum < cur_opts.start_pgnum || 
	      this_pgnum > cur_opts.end_pgnum ) && is_stroke_cmd( this_cmd ) )
	    {	/* skip this vdi() call */
	    }
	else if ( this_cmd == ESCAPECMD && 
			( SUBCODE == IMGFCMD || SUBCODE == XIMGFCMD ) ) 
	    {
	    is_img_file = !is_gmp_file ;
	    ret = do_image_cmd() ;
	    if ( !ret && is_img_file )	/* file not found or some such */
		return( FALSE ) ;
	    }
	else if ( ! ( this_cmd == ESCAPECMD && SUBCODE == 99 ) )
	    { 
	    switch ( this_cmd )
	        {
		case GDPCMD :
			case_gdp() ;
			break ;
		case PLINWIDCMD : 
		case PMRKSCLCMD : 
		case TEXTHGTCMD : 
		        trfm_mag( 1, &ptsin[ 0 ] ) ;
			if ( rotate )
			    flip_mag( 1, &ptsin[ 0 ] ) ;
		        break ;
		case PTS_CHAR_HGT : 
			intin[ 0 ] = SMUL_DIV( intin[ 0 ], bf_height, org_height ) ;
			if ( DEV_TYPE & ( SCRN_TYPE | CMRA_TYPE ) )
			    fudge_sizes() ;
			break ;
		case TEXTUPCMD : 
			if ( rotate )
			    rot_ang( 1, &intin[ 0 ] ) ;
			break ;
		case FONTCMD : 
			cur_font = intin[ 0 ] ;
			break ;
		case SETCLIPCMD : 
			if ( intin[ 0 ] == FALSE )
			{
			    intin[ 1 ] = TRUE ;
			    movb( sizeof( RECTANGLE ), &page_wdw, &ptsin ) ;
			}
			else
			{
			    trfm_pts( NUM_PTSIN, &ptsin[ 0 ] ) ;
			    ptsin[ 0 ] = minmax( page_wdw.left, ptsin[ 0 ], 
					       page_wdw.right ) ;
			    ptsin[ 1 ] = minmax( page_wdw.top, ptsin[ 1 ], 
					       page_wdw.bottom ) ;
			    ptsin[ 2 ] = minmax( page_wdw.left, ptsin[ 2 ], 
					       page_wdw.right ) ;
			    ptsin[ 3 ] = minmax( page_wdw.top, ptsin[ 3 ], 
					       page_wdw.bottom ) ;
			}
			break ;
	    	default : 
		        trfm_pts( NUM_PTSIN, &ptsin[ 0 ] ) ;
		} /* switch ( this_cmd ) */
	    DEV_HNDL = cur_hndl ;
	    vdi() ;
	    } /* if any call BUT user defineds (esc 99) and bit images */
	if ( !get_mf() )
	    return( -1 ) ;	/* end of file */
	last_cmd = this_cmd ;
	this_cmd = OPCODE ;
	if ( (this_cmd == CLEARCMD) && (last_cmd == UPDATECMD) )
	    { /* note: this v_clrwk will get executed when process_page() is called again from draw_page() */
	    return( 1 ) ;	/* end of page */
	    }
	} /* while ( this_cmd != FFFF ) */
    return( -1 ) ;	/* end of file encountered */
} /* process_page */

/*--------------------------------------------------------------*/
    VOID
change_paper() 
{
    if ( DEV_TYPE & ( PLOT_TYPE | PRNT_TYPE ) )
        {
	if ( PRNT_PREFS.pause && !PRNT_PREFS.to_file )
	    {	
	    graf_mouse( MO_ARROW, 0x0L ) ;
    	    f_alert( CHGPAPER, 1 ) ;
	    graf_mouse( MO_HGLASS, 0x0L ) ;
	    }
	}
} /* change_paper() */

/*--------------------------------------------------------------*/
	BOOLEAN
draw_page()		/* draw device page	*/
{
BOOLEAN	page_ok ;

    reset_mf() ;
    this_pgnum = 0 ;
    page_ok = TRUE ;
    while ( page_ok == TRUE )
	{
	++this_pgnum ;
	page_ok = process_page() ;	/* return of (-1) means EOF found, ie normal terminate */	
	if ( page_ok == TRUE ) 
	    {
	    if ( ( this_pgnum >= cur_opts.start_pgnum &&
				      this_pgnum <= cur_opts.end_pgnum ) )
	    	{
	    	if ( DEV_TYPE & SCRN_TYPE ) 
	    	    hold_pix( 1 ) ;		/* direction == 1 (forward) */
	    	change_paper() ;
		}
	    }
	}
    if ( page_ok == FALSE )	/* terminated via ESC key */
	return( FALSE ) ;
    if ( DEV_TYPE & ( PRNT_TYPE | CMRA_TYPE ) )
	{
        if ( is_out_file )
		v_output_window( cur_hndl, &out_extent ) ;
        else
	    	v_updwk( cur_hndl ) ;
	}
    return( TRUE ) ;
} /* draw_page */

/*--------------------------------------------------------------*/
/* display an alert if the mouse is off.			*/
    VOID
moff_alert( tree_num )
WORD	tree_num ;
{
    graf_mouse( MOUSE_ON, 0x0L ) ;
    f_alert( tree_num, 1 ) ;
    graf_mouse( MOUSE_OFF, 0x0L ) ;
} /* moff_alert */

/*--------------------------------------------------------------*/
	BOOLEAN
dsp_pix()		/* Draw picture, mosaic if necessary */
{
	WORD num_copy ;
	WORD *pleft, *ptop, *pright, *pbottom ;

    if ( !grf_mode )
	{		/* return to graph mode */
	grf_mode = TRUE ;
	v_exit_cur( cur_hndl ) ;
	}
    num_copy = atoi( get_teptext( cntl_tr, COPYSTXT ) ) ;
    if ( ( DEV_TYPE & (SCRN_TYPE | PRNT_TYPE) ) || is_spool )
        num_copy = 1 ;
    set_trfm() ;	/* Set trans formation */
    trfm_pts( 2, &out_extent ) ; 	/* out_extent goes to device coords */
    pleft   = &page_wdw.left ;	/* Left x */
    ptop    = &page_wdw.top ;	/* Top y */
    pright  = &page_wdw.right ;	/* Right x */
    pbottom = &page_wdw.bottom ;	/* bottom y */
    if ( rotate )	/* Set clip rectangle pointers */
	{
	pswap( &pleft, &ptop ) ;
	pswap( &pright, &pbottom ) ;
	}
    do
	{		/* Loop for copies	*/
	*ptop = nyoffset ;
	for ( dvc_nyoff=nyoffset ; -dvc_nyoff < y_size ;
		    dvc_nyoff-=(dvc_nypixel+1) )
	    {	/* Loop in y pixels	*/
            *pleft = nxoffset ;
	    *pbottom = min( dvc_nyoff+nypixel - 1, dvc_nypixel ) ;
	    for ( dvc_nxoff=nxoffset ; -dvc_nxoff < x_size ; 
			dvc_nxoff-=(dvc_nxpixel+1) )
		{		/* Loop In x pixels	*/
		*pright = min( (dvc_nxoff + nxpixel - 1), dvc_nxpixel ) ;
		change_paper() ;
		if (   (is_img_file && (DEV_TYPE & SCRN_TYPE) ) || 
				( prnt_open )
				|| ( prnt_open && PRNT_PREFS.pause ) )
		    {	/* don't clear til later or skip initial FF */
		    }
		else
		    v_clrwk( cur_hndl ) ;
		prnt_open = FALSE ;
		if ( ! fonts_loaded )
		    vst_load_fonts( cur_hndl, 0 ) ;
		fonts_loaded = TRUE ;
		vs_clip( cur_hndl, TRUE, &page_wdw ) ;
		if ( !draw_page() )
		    return( FALSE ) ;
		if ( ( DEV_TYPE & CMRA_TYPE ) && (CMRA_PREFS.film == 0 ) )
		    moff_alert( PRNTFILM ) ;   /* pull land camera print */
		*pleft = 0 ;
		} /* for x */
	    *ptop = 0 ;
	    } /* for y */
	} while ( --num_copy ) ;
    return( TRUE ) ;
} /* dsp_pix */

/*--------------------------------------------------------------*/
    VOID
init_prntr( x, y, w, h )		
WORD 	*x, *y, *w, *h ;
{
    rsrc_gaddr( GAD_TREE, PRTCNT, &pcnt_tree ) ;
    *w = get_width( pcnt_tree, ROOT ) ;
    *h = get_height( pcnt_tree, ROOT ) ;
    *x = gl_desk.g_x + gl_desk.g_w - *w -10 ;
    *y = gl_desk.g_y + gl_desk.g_h - *h - 10 ;
    pcnt_tree = start_dial( PRTCNT, x, y, w, h ) ;
    upd_itep( pcnt_tree, TOTCNT, next_slot ) ;
} /* init_prntr() */

/*--------------------------------------------------------------*/
    BOOLEAN
prt_init( x, y, w, h )		
WORD 	*x, *y, *w, *h ;
{
    if ( 0 >= atoi( get_teptext( cntl_tr, COPYSTXT ) ) )
	{
	pad_n_itoa( 2, 1, get_teptext( cntl_tr, COPYSTXT ) ) ;
	objc_draw( cntl_tr, COPYITEM, MAX_DEPTH, gl_win.g_x, gl_win.g_y,
					gl_win.g_w, gl_win.g_h ) ;
	}
    if ( !( DEV_TYPE & SCRN_TYPE ) )
    	v_clsvwk( scr_hndl ) ;
    graf_mouse( MO_HGLASS, 0x0L ) ;
    cur_hndl = open_dvc() ;
    if ( cur_hndl )	
    	{
	if ( DEV_TYPE & ( CMRA_TYPE | SCRN_TYPE ) )
    	    graf_mouse( MOUSE_OFF, 0x0L ) ;
	prnt_open = ( DEV_TYPE & PRNT_TYPE ) ;
	if ( !( DEV_TYPE & SCRN_TYPE ) )
	    scr_hndl = (-1) ;
	wind_get( w_handle, WF_TOP, x, y, w, h ) ;
	if ( *x != w_handle )
	    {		/* Top output window */
	    wind_set( w_handle, WF_TOP, 0, 0, 0, 0 ) ;
	    pronto_mesg() ;
	    }
	wind_update( 3 ) ;		/* Take total control of the mouse */
	if ( DEV_TYPE & ( PRNT_TYPE | PLOT_TYPE ) )
	    init_prntr( x, y, w, h ) ;
	if ( DEV_TYPE & ( SCRN_TYPE | CMRA_TYPE ) )
	    {			/* If screen, remove menu bar */
	    menu_bar( gl_menu, FALSE ) ;
	    }
	if ( ( DEV_TYPE & CMRA_TYPE ) && (CMRA_PREFS.film == 0 ) )
	    moff_alert( DRKSLIDE ) ;   /* Remove Dark Slide */
	return( TRUE ) ;
	} /* if open_dvc successful */
    graf_mouse( MO_ARROW, 0x0L ) ;
    f_alert( DVCMISS, 1 ) ;
    if ( !( DEV_TYPE & SCRN_TYPE ) )
    	vopen( FALSE ) ;
    return( FALSE ) ;
} /* prt_init */

/*--------------------------------------------------------------*/
    VOID
prt_loop()		/* Draw list of files	*/
{
WORD 	ii ;
WORD 	direction ;
BOOLEAN	f_exists, cont_loop, redirect ;
BYTE	name[ NAME_LENGTH ] ;

    brk_out = FALSE ;
    ii = 0 ;
    direction = 1 ;
    cont_loop = TRUE ;
    do
	{
	strcpy( &names[ ii ], buffer.CUR_FNAME ) ;
	redirect = FALSE ;
	if ( DEV_TYPE & ( PRNT_TYPE | PLOT_TYPE ) )
            {		/* Update print count */
	    del_path( buffer.CUR_FNAME, name ) ;
	    upd_pcnt( ii+1, name ) ;
	    redirect = comp_ext( PRNT_PREFS.file, buffer.CUR_FNAME ) ;
	    }/* redirect == TRUE iff trying to print a spool file from list */
	if ( redirect ) 
	    print_spooler( PRNT_PREFS.file ) ;
	else
	{
	if ( comp_ext( IMG_EXT, buffer.CUR_FNAME ) )
	    {
	    is_img_file = TRUE ;
	    new_ext( buffer.CUR_FNAME, GEM_EXT ) ;
	    }
	f_exists = open_mf( buffer.CUR_FNAME ) ;
	if ( DEV_TYPE & CMRA_TYPE )
	    if ( !f_exists || f_exists == GMP_FILE || f_exists == OUT_FILE )
		{   /* driver bug requires update between open and close */
		f_exists = FALSE ;
		v_clrwk( cur_hndl ) ;
		v_updwk( cur_hndl ) ;
		}
	switch ( f_exists ) 	
	    {
	    case GMP_FILE :
		    is_gmp_file = TRUE ;
	    case GEM_FILE : 
		    f_exists = dsp_pix() ;
		    break ;
	    case OUT_FILE : 
		    is_out_file = TRUE ;
	    case TXT_FILE : 
		    OUTGWRT() ;			/* OUTGWRT overlay */
		    break ;
	    default : 
		    break ;
	    }
	is_img_file = FALSE ;
	is_gmp_file = FALSE ;
	is_out_file = FALSE ;	    
	is_1st_word = FALSE ;	    
	close_mf() ;
	if ( f_exists && ( cur_hndl == scr_hndl ) )
	    {		/* Hold screen display */
	    if ( ( this_pgnum >= cur_opts.start_pgnum &&
				      this_pgnum <= cur_opts.end_pgnum ) )
	    	direction = hold_pix( direction ) ;
	    }
	} /* else ( !redirect ) */
	brk_out = upd_count( &ii, direction ) ;
	cont_loop = !brk_out ;
    } while ( cont_loop ) ;
} /* prt_loop */

/*--------------------------------------------------------------*/
    VOID
prt_cleanup( x, y, w, h )	
WORD 	x, y, w, h ;
{
GRECT	r ;

     if ( DEV_TYPE & PRNT_TYPE ) 
	{
	v_clrwk( cur_hndl ) ;	
	if ( PRNT_PREFS.final_ff && !PRNT_PREFS.pause ) 
	    v_form_adv( cur_hndl ) ;
	}
    vst_unload_fonts( cur_hndl, 0 ) ;
    fonts_loaded = FALSE ;
    if ( cur_hndl != scr_hndl )
	{	/* Close device */
	v_clswk( cur_hndl ) ;
	if ( is_spool )
	    print_spooler( PRNT_PREFS.file ) ;
	vopen( FALSE ) ;		/* open screen driver */
 	}
    else if ( !grf_mode )
	{	/* Return to graph mode */
	grf_mode = TRUE ;
	v_exit_cur( cur_hndl ) ;
	}
    wind_update( 2 ) ;		/* Release control of the mouse */
    if ( DEV_TYPE & ( PRNT_TYPE | PLOT_TYPE ) )
	{		/* Remove print count dialog	*/
	remv_pcnt( x, y, w, h ) ;
	}
    if ( DEV_TYPE & ( SCRN_TYPE | CMRA_TYPE ) ) 
	{		/* Restore desktop */
	def_txt_atts() ;
	form_dial( FMD_FINISH, 0, 0, 0, 0, gl_desk.g_x, gl_desk.g_y, gl_desk.g_w, 
		      gl_desk.g_h ) ;
	get_obloc( dev_tree, DEVCROOT, &r.g_x, &r.g_y, &r.g_w, &r.g_h ) ;
	do_redraw( dev_tree, 0, r.g_x, r.g_y, r.g_w, r.g_h ) ;
	menu_bar( gl_menu, TRUE ) ;
        graf_mouse( MOUSE_ON, 0x0L ) ;
	}
    graf_mouse( MO_ARROW, 0x0L ) ;
} /* prt_cleanup */

/*--------------------------------------------------------------*/
    VOID
prt_files()		/* Draw list of files	*/
{
WORD	x, y, w, h ;

    if ( prt_init( &x, &y, &w, &h ) )
	{
	prt_loop() ;
	prt_cleanup( x, y, w, h ) ;
	}
} /* prt_files */

/*--------------------------------------------------------------*/
/* OUTDISP overlay entry point.					*/
    VOID
OUTDISP()
{
    prt_files() ;
} /* OUTDISP */


/* end of display.c */