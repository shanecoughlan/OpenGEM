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
#include "gembind.h"
#include "dosbind.h"
#include "xgembind.h"
#include "vdibind.h"
#include "rsrclib.h"

#include "output.h"
#include "odefs.h"
#include "ovar.h"

EXTERN WORD	min() ;				/* ooptimz.c */


EXTERN VOID	movb() ;			/* outility.c */
EXTERN WORD	atoi() ;
EXTERN WORD	pad_n_itoa() ;

EXTERN VOID	set_ob() ;			/* owindow.c */

EXTERN VOID	icon_name() ;			/* ooptions.c */

CLBNDL  new_bndls[ MAX_COLORS ] ;
CLBNDL	cur_col ;
WORD	cur_index ;


/*--------------------------------------------------------------*/
    VOID
get_flm_names()
{
	WORD 	handle ;
	WORD 	workin[ 11 ], workout[ 57 ] ;
	WORD 	ii, max_films, status ;
	BYTE 	name[ FNAME_LENGTH ] ;	/* probably only need 25-30 chars */
	LONG 	tree ;

    workin[ 0 ] = 41 ;
    for ( ii = 0; ii < MX_DVCS; ii++ )
	{
	if ( dvc[ ii ].type & CMRA_TYPE )
	    workin[ 0 ] = dvc[ ii ].num ;
	}
    for ( ii=1 ; ii<10 ; ii++ )
        workin[ ii ] = 1 ;
    workin[ 10 ] = 2 ;
    v_opnwk( workin, &handle, workout ) ;
    if ( handle )
	{
    	max_films = FILMNAMN - FILMNAM0 + 1 ;
    	rsrc_gaddr( GAD_TREE, FILMPREF, &tree ) ;
    	ii = 1 ;
    	status = TRUE ;    
    	do 
	    {
	    status = vqp_filmname( handle, ii, &name ) ;
	    if ( status )
	    	{
	    	set_state( tree, FILMNAM0 + ii - 1, NORMAL ) ;
	    	set_flags( tree, FILMTYP0 + ii - 1, ( SELECTABLE | RBUTTON ) ) ;
	    	LSTRCPY( ADDR( name ), get_teptext( tree, FILMNAM0 + ii - 1 ) ) ;
	    	}
    	    } while ( status && (++ii < max_films) ) ;
    	CMRA_PREFS.film = min( CMRA_PREFS.film, ii - 2 ) ;
    	v_clswk( handle ) ;
	}
} /* get_flm_names */


/*--------------------------------------------------------------*/
    VOID
set_camcolr( tree )
	LONG *tree ;
{
WORD i ;

    rsrc_gaddr( GAD_TREE, COLRPREF, tree ) ;
    for ( i = COLWHITE; i <= COLPURPL; i++ )
    	set_ob( *tree, i, DISABLED, TRUE ) ;
    for ( i = LGTSHADE; i <= DRKSHADE; i++ )
    	set_ob( *tree, i, DISABLED, TRUE ) ;
    set_ob( *tree, COLSTR00, DISABLED, TRUE ) ;
    set_ob( *tree, COLSTR01, DISABLED, TRUE ) ;
    set_ob( *tree, COLSTR02, DISABLED, TRUE ) ;
    set_ob( *tree, COLSTR03, DISABLED, TRUE ) ;
    set_ob( *tree, COLSTR04, DISABLED, TRUE ) ;
    set_ob( *tree, COLSTR05, DISABLED, TRUE ) ;
    set_flags( *tree, REDRGB, NONE ) ;
    set_ob( *tree, REDRGB, DISABLED, TRUE ) ;
    set_flags( *tree, GREENRGB, NONE ) ;
    set_ob( *tree, GREENRGB, DISABLED, TRUE ) ;
    set_flags( *tree, BLUERGB, NONE ) ;
    set_ob( *tree, BLUERGB, DISABLED, TRUE ) ;
    movb( sizeof( new_bndls ), &CMRA_PREFS.cmclr[0], &new_bndls ) ;
} /* set_camcolr */


/*--------------------------------------------------------------*/
    VOID
shad_tggl( tree, x, y, w, h )
	LONG tree ;
	WORD x, y, w, h ;
{
WORD i ;

    for ( i = LGTSHADE; i <= DRKSHADE; i++ )
    	tggl_state( tree, i, x, y, w, h, DISABLED, TRUE ) ;
} /* shad_tggl */


/*--------------------------------------------------------------*/
    VOID
cam_tggl( tree, x, y, w, h )
	LONG tree ;
	WORD x, y, w, h ;
{
WORD i ;

    for ( i = COLWHITE; i <= COLPURPL; i++ )
    	tggl_state( tree, i, x, y, w, h, DISABLED, TRUE ) ;
    shad_tggl( tree, x, y, w, h ) ;
    tggl_state( tree, COLSTR00, x, y, w, h, DISABLED, TRUE ) ;
    tggl_state( tree, COLSTR01, x, y, w, h, DISABLED, TRUE ) ;
    tggl_state( tree, COLSTR02, x, y, w, h, DISABLED, TRUE ) ;
    tggl_state( tree, COLSTR03, x, y, w, h, DISABLED, TRUE ) ;
    tggl_state( tree, COLSTR04, x, y, w, h, DISABLED, TRUE ) ;
    tggl_state( tree, COLSTR05, x, y, w, h, DISABLED, TRUE ) ;
    tggl_flags( tree, REDRGB, EDITABLE, TRUE ) ;
    tggl_state( tree, REDRGB, x, y, w, h, DISABLED, TRUE ) ;
    tggl_flags( tree, GREENRGB, EDITABLE, TRUE ) ;
    tggl_state( tree, GREENRGB, x, y, w, h, DISABLED, TRUE ) ;
    tggl_flags( tree, BLUERGB, EDITABLE, TRUE ) ;
    tggl_state( tree, BLUERGB, x, y, w, h, DISABLED, TRUE ) ;
} /* cam_tggl */

/*--------------------------------------------------------------*/
    VOID
new_rgb( tree, x, y, w, h )
LONG 	tree ;
WORD	x, y, w, h ;
{
BYTE 	rb[ 4 ], gb[ 4 ], bb[ 4 ] ;

    pad_n_itoa( 3, cur_col.red, rb ) ;
    pad_n_itoa( 3, cur_col.green, gb ) ;
    pad_n_itoa( 3, cur_col.blue, bb ) ;
    LSTRCPY( ADDR( rb ), get_teptext( tree, REDRGB ) ) ;
    LSTRCPY( ADDR( gb ), get_teptext( tree, GREENRGB ) ) ;
    LSTRCPY( ADDR( bb ), get_teptext( tree, BLUERGB ) ) ;
    objc_draw( tree, REDRGB, MAX_DEPTH, x, y, w, h ) ;
    objc_draw( tree, GREENRGB, MAX_DEPTH, x, y, w, h ) ;
    objc_draw( tree, BLUERGB, MAX_DEPTH, x, y, w, h ) ;
} /* new_rgb */

/*--------------------------------------------------------------*/
    VOID
get_rgb()
{
WORD pri1, pri2, sec1, sec2 ;

    switch ( cur_col.bas_col )
	{
	case 0 :    /* white */
		    cur_col.red = cur_col.green = cur_col.blue = 100 ;
		    return ;
	case 1 :    /* black */
		    cur_col.red = cur_col.green = cur_col.blue = 0 ;
		    return ;
	case 10 :   /* grey */
		    cur_col.red = cur_col.green = cur_col.blue = n_col[ cur_col.lgtns - 1 ] ;
		    return ;
	} /* switch */
    pri1 = p_col1[ cur_col.lgtns - 1 ] ;
    pri2 = p_col2[ cur_col.lgtns - 1 ] ;
    sec1 = s_col1[ cur_col.lgtns - 1 ] ;
    sec2 = s_col2[ cur_col.lgtns - 1 ] ;
    switch ( cur_col.bas_col )
	{
	case 2 :    /* red */
		    cur_col.red = pri1 ;
		    cur_col.green = cur_col.blue = pri2 ;
		    break ;
	case 3 :    /* green */
		    cur_col.green = pri1 ;
		    cur_col.red = cur_col.blue = pri2 ;
		    break ;
	case 4 :    /* blue */
		    cur_col.blue = pri1 ;
		    cur_col.green = cur_col.red = pri2 ;
		    break ;
	case 5 :    /* cyan */
		    cur_col.green = cur_col.blue = sec1 ;
		    cur_col.red = sec2 ;
		    break ;
	case 6 :    /* yellow */
	case 9 :    /* orange */
		    cur_col.green = cur_col.red = sec1 ;
		    cur_col.blue = sec2 ;
		    break ;
	case 8 :    /* brown */
		    cur_col.blue = sec2 ;
		    cur_col.red = cur_col.green = n_col[ cur_col.lgtns - 1 ] ;
		    break ;
	case 7 :    /* magenta */
	case 11 :   /* purple */
		    cur_col.red = cur_col.blue = sec1 ;
		    cur_col.green = sec2 ;
		    break ;
	} /* switch */
    if ( cur_col.bas_col == 9 ) /* orange */
	{
	cur_col.green  = ( cur_col.red + cur_col.blue ) / 2 ;
	}
    if ( cur_col.bas_col == 11 ) /* purple */
	{
	cur_col.red  = ( cur_col.green + cur_col.blue ) / 2 ;
	}
} /* get_rgb */


/*--------------------------------------------------------------*/
/* returns TRUE if color number is 0,1,8, or 9.			*/
    BOOLEAN
wht_or_blk( color )
WORD	color ;
{
    return( (color == 0) || (color == 1) ) ;
} /* wht_or_blk */


/*--------------------------------------------------------------*/
    VOID
tune_col( tree, x, y, w, h )
LONG 	tree ;
WORD	x, y, w, h ;
{
WORD	new_name ;

    new_name = get_rbut( tree, COLNROOT ) - COLWHITE ;
    if ( new_name == cur_col.bas_col )
	cur_col.lgtns = get_rbut( tree, SHADROOT ) - LGTSHADE + 1 ;
    else
	{
	if ( wht_or_blk( cur_col.bas_col ) != wht_or_blk( new_name ) )
	    {
	    if ( !cur_col.lgtns )   /* enable lightness buttons */
		{
		shad_tggl( tree, x, y, w, h ) ;
		cur_col.lgtns = ( ( DRKSHADE - LGTSHADE ) / 2 ) ; /* medium */
    	    	tggl_state( tree, (cur_col.lgtns + LGTSHADE - 1 ), 
					x, y, w, h, SELECTED, TRUE ) ;
		}
	    else	/* disable lightness buttons */
		{
    	    	tggl_state( tree, (cur_col.lgtns + LGTSHADE - 1 ), 
					x, y, w, h, SELECTED, TRUE ) ;
		cur_col.lgtns = 0 ;
		shad_tggl( tree, x, y, w, h ) ;
		} /* else */
	    } /* if need to toggle lightness buttons */
    	cur_col.bas_col = new_name ;
	} /* else */
    get_rgb() ;
    new_rgb( tree, x, y, w, h ) ;
} /* tune_col */

/*--------------------------------------------------------------*/
    VOID
tggl_col( tree, x, y, w, h )
    LONG tree ;
    WORD x, y, w, h ;
{
    tggl_state( tree, (cur_col.bas_col + COLWHITE), 
					x, y, w, h, SELECTED, TRUE ) ;
    if ( cur_col.lgtns )
    	tggl_state( tree, (cur_col.lgtns + LGTSHADE - 1 ), 
					x, y, w, h, SELECTED, TRUE ) ;
    else
	shad_tggl( tree, x, y, w, h ) ; 	/* turn it off */
} /* tggl_col */


/*--------------------------------------------------------------*/
    VOID
new_col( new_index, tree, x, y, w, h )
WORD	new_index ;
LONG 	tree ;
WORD	x, y, w, h ;
{
    cur_index = new_index ;
    movb( sizeof( CLBNDL ), &new_bndls[ cur_index ], &cur_col ) ;
    tggl_col( tree, x, y, w, h ) ;
    new_rgb( tree, x, y, w, h ) ;
} /* new_col */



/*--------------------------------------------------------------*/
    VOID
save_col( tree )
    LONG tree ;
{
BYTE	rb[ 4 ], gb[ 4 ], bb[ 4 ] ;

    LSTRCPY( get_teptext( tree, REDRGB ), ADDR( rb ) ) ;
    LSTRCPY( get_teptext( tree, GREENRGB ), ADDR( gb ) ) ;
    LSTRCPY( get_teptext( tree, BLUERGB ), ADDR( bb ) ) ;
    cur_col.red     = min( atoi( rb ), 100 ) ;
    cur_col.green   = min( atoi( gb ), 100 ) ;
    cur_col.blue    = min( atoi( bb ), 100 ) ;
    movb( sizeof( CLBNDL ), &cur_col, &new_bndls[ cur_index ] ) ;
} /* save_col */


/*--------------------------------------------------------------*/
    VOID
desel_camcol( tree, x, y, w, h, first )
    LONG tree ;
    WORD x, y, w, h ;
    BOOLEAN first ;
{
    if ( !first )
	{
    	tggl_state( tree, (CAMCOL00 + cur_index),
				x, y, w, h, SELECTED, TRUE ) ;
    	tggl_state( tree, (COLWHITE + cur_col.bas_col), 
				x, y, w, h, SELECTED, TRUE ) ;
    	if ( cur_col.lgtns )
            tggl_state( tree, (LGTSHADE + cur_col.lgtns - 1), 
				x, y, w, h, SELECTED, TRUE ) ;
	}
} /* desel_camcol */


/*--------------------------------------------------------------*/
    VOID
cmcl_dial()
{
	LONG tree, l_dummy ;
	WORD x, y, w, h;
	WORD dial_out ;
	WORD	new_index ;
	BOOLEAN first ;

	set_camcolr( &tree ) ;
	icon_name( tree ) ;
	w = 0;
	h = 0;
	l_dummy = start_dial( COLRPREF, &x, &y, &w, &h ) ;
	first = TRUE ;
	do {
	    form_do( tree, 0 ) ;
	    dial_out = okcan( tree, OKCAMCOL, CANCMCOL ) ;
	    if ( !dial_out )
	    	{
    		new_index = get_rbut( tree, COLIROOT ) - CAMCOL00 ;
	 	if ( first )
		    {
		    cam_tggl( tree, x, y, w, h ) ;
		    new_col( new_index, tree, x, y, w, h ) ;
		    } /* if first */
		else
		    {
		    if ( new_index != cur_index )
			{
			save_col( tree ) ;
			tggl_col( tree, x, y, w, h ) ;
			new_col( new_index, tree, x, y, w, h ) ;
			} /* if new != cur */
		    else
			{
			tune_col( tree, x, y, w, h ) ;
			} 
		    } /* else */
		first = FALSE ;
		} /* if !dial_out */
	    } while ( !dial_out ) ;
  	save_col( tree ) ;
	desel_camcol( tree, x, y, w, h, first ) ;
	end_dial( tree, x, y, w, h ) ;
	if ( dial_out == OK )
   	    movb( sizeof( CMRA_PREFS.cmclr ), &new_bndls, &CMRA_PREFS.cmclr[0] ) ;
} /* cmcl_dial */

/*--------------------------------------------------------------*/
/* OUTCMCL overlay entry point.					*/
    VOID
OUTCMCL( item )
WORD	item ;
{
    if ( item == COLRPREF )
	cmcl_dial() ;
    else
	get_flm_names() ;
} /* OUTCMCL */


/* end of ocamcolr.c */