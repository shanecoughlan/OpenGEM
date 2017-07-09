/*******************************************************************/
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

/* 18/11/2000:  Updated to include help support in dialogs (orudge) */
/* 29/11/2000:  Moved help to RSC (orudge) */

#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "gembind.h"
#include "xgembind.h"
#include "dosbind.h"
#include "output.h"
#include "rsrclib.h"
#include "gemcmds.h"

#include "odefs.h"
#include "ovar.h"

EXTERN VOID	strcpy() ; 			/* ooptimz.c */
EXTERN VOID	strcat() ; 		
EXTERN WORD	strlen() ; 		
EXTERN WORD	max() ;
EXTERN WORD	min() ;

EXTERN VOID	movb() ; 			/* outility.c */
EXTERN WORD	atoi() ; 
EXTERN VOID	pad_n_itoa() ;
EXTERN VOID	itoa() ; 
EXTERN VOID	del_fname() ;
EXTERN VOID	del_path() ;
EXTERN UWORD	dec_to_int() ;
EXTERN WORD	valid_drive() ;
EXTERN VOID	etext_to_fname() ;
EXTERN VOID	fname_to_etext() ;

EXTERN VOID	do_icn_img() ;		/* orscinit.c */

EXTERN UWORD	_umul_div() ;		/* odisplay.c */

EXTERN VOID	do_miss() ;		/* odialog.c */

EXTERN WORD	get_ext() ;		/* obuffer.c */

/*--------------------------------------------------------------*/
/* save OUTPUT.INF file. cur_opts.cur_dvc is usually set to 	*/
/* the object # of the icon of the current device - in the 	*/
/* info file, though, it is the device number, ie 1, 21,11, etc	*/
    VOID
make_def()
{
BYTE 	file[ FNAME_LENGTH ] ;
WORD	sav_dvc, ii ;
UWORD 	f_hndl ;
LONG	size;

    graf_mouse( MO_HGLASS, 0x0L ) ;
    sav_dvc = cur_opts.cur_dvc ;
    cur_opts.cur_dvc = dvc[ cur_opts.cur_dvc ].num ;
    strcpy( gsx_wc, &cur_opts.gsx_wc[ 0 ] ) ;
    strcpy( opt_dir, file ) ;
    strcat( INFNAME, file ) ;
    movb( sizeof( GRECT ), &gl_win, &cur_opts.window ) ;
    cur_opts.numcopies = atoi( get_teptext( cntl_tr, COPYSTXT ) ) ;
    f_hndl = dos_create( file, 0 ) ;
    if ( f_hndl )
	{
	size = (LONG)(sizeof( OPTIONS) ) ;
	size = dos_write( f_hndl, size, &cur_opts ) ;
	size = (LONG)(sizeof( DVC_STRUCT ) ) ;
	for ( ii = 0; dvc[ ii ].type != 0; ii++ )
	    size = dos_write( f_hndl, size, &dvc[ ii ] ) ;
	dos_close( f_hndl ) ;
	}
    else
        {
	    /* Alert ? */
	}
    cur_opts.cur_dvc = sav_dvc ;
    graf_mouse( MO_ARROW, 0x0L ) ;
} /* make_def */

/*----------------------------------------------------------------------*/  
    VOID
gt_position( tree, root_item, horz, vert )
LONG	tree ;
WORD	root_item ;
WORD	*horz, *vert ;
{
OBJECT	*pos_ptr, *tl_ptr ; /* ptrs to position box, & top left most item. */

    pos_ptr = OB_PTR( tree, (root_item + ( POSIITEM - POSIROOT )) ) ;
    tl_ptr = OB_PTR( tree, (root_item + ( LEFTTOP - POSIROOT )) ) ;
    *horz = (( ( pos_ptr->ob_x - tl_ptr->ob_x ) * 2 ) + 1)/ tl_ptr->ob_width ;
    *vert = (( ( pos_ptr->ob_y - tl_ptr->ob_y ) * 2 ) + 1)/ tl_ptr->ob_height;
} /* gt_position() */

/*----------------------------------------------------------------------*/  
    VOID
mv_position( tree, item )
LONG	tree ;
WORD	item ;
{
WORD	item_offset ;	/* in the range [ 0..9 ] */
OBJECT	*pos_ptr, *tl_ptr ; /* ptrs to position box, & top left most item. */

    pos_ptr = OB_PTR( tree, (POSIROOT + ( POSIITEM - POSIROOT )) ) ;
    tl_ptr = OB_PTR( tree, (POSIROOT + ( LEFTTOP - POSIROOT )) ) ;
    item_offset = item - POSIROOT - ( LEFTTOP - POSIROOT ) ;
    pos_ptr->ob_x = tl_ptr->ob_x + 
    		( tl_ptr->ob_width * ( item_offset % 3 ) / 2 ) ;
    pos_ptr->ob_y = tl_ptr->ob_y + 
    		( tl_ptr->ob_height * ( item_offset / 3 ) / 2 ) ;
} /* mv_position() */

/*--------------------------------------------------------------*/
/* Set radio buttons, etc for the printer/plotter page 		*/
/* preferences dialog. "Final form feed:" needs to be disabled	*/
/* for plotters.						*/
    VOID
set_printer( tree )
	LONG tree ;
{
WORD	pos_item ;	/* obj # for alignment. LEFTTOP + [0..8] */
WORD	ii, state ;	/* state for "Final FF?" radio buttons */
OBJECT	*ob_ptr ;	/* pointer to object */

    state = ( DEV_TYPE == PRNT_TYPE ) ? NORMAL : DISABLED ;
    for ( ii = FFSTRING; ii <= NOFF; ii++ )
        {
    	ob_ptr = OB_PTR( tree, ii ) ;
    	ob_ptr->ob_state = state ;    
	}
    if ( state != DISABLED )
    	set_rbut( tree, FFFROOT, (YESFFF + !PRNT_PREFS.final_ff) ) ;
    set_rbut( tree, PRSCROOT, (PRNTFS + PRNT_PREFS.scale) ) ;
    pos_item = (LEFTTOP + (3 * PRNT_PREFS.vertjust) + PRNT_PREFS.horzjust ) ;
    mv_position( tree, pos_item ) ;
} /* set_printer */

/*----------------------------------------------------------------------*/  
    WORD
do_prntdial( tree, x, y, w, h )
    LONG	tree ;
    WORD	x, y, w, h ;
{
    WORD	cur_item, new_item ;
    WORD	dial_out ;
    WORD	ms_x, ms_y ;		/* mouse x and y cursor pos */
    WORD	but, kstate ;		/* not used */
    BOOLEAN	ttrue = TRUE ;	/* metaware doesn't like "while (TRUE)" */
    OBJECT	*ob_ptr ;
    
    objc_draw( tree, ROOT, MAX_DEPTH, x, y, w, h ) ; /* draw entire dialog */
    objc_offset( tree, POSIROOT, &x, &y ) ;	/* change xywh so that */
    ob_ptr = OB_PTR( tree, POSIROOT ) ;	/* subsequent ob_draws */
    w = ob_ptr->ob_width ;			/* draw only position item */
    h = ob_ptr->ob_height ;
    cur_item = (LEFTTOP + (3 * PRNT_PREFS.vertjust) + PRNT_PREFS.horzjust ) ;
    new_item = cur_item ;
    do
    {
	form_do( tree, 0 ) ;
	dial_out = okcan( tree, OKPRNT, CANPRNT ) ;
	if ( dial_out )
	    return( dial_out ) ;
        graf_mkstate( &ms_x, &ms_y, &but, &kstate ) ;
        new_item = objc_find( tree, POSIROOT, MAX_DEPTH, ms_x, ms_y ) ;
  	if ( new_item > 0 && cur_item != new_item )
 	    {
	    mv_position( tree, new_item ) ;
	    objc_draw( tree, ROOT, MAX_DEPTH, x, y, w, h ) ;
	    cur_item = new_item ;
	    }
    } while ( ttrue ) ;
} /* do_prntdial() */

/*--------------------------------------------------------------*/
    VOID
set_screen( tree )
	LONG tree ;
{
    set_rbut( tree, WAITROOT, (WAITKEY + !SCRN_PREFS.wait_for_key ) ) ;
    set_rbut( tree, CYCLROOT, (NOCYCLE + SCRN_PREFS.cycle_until_esc ) ) ;
    pad_n_itoa( 2, SCRN_PREFS.time, get_teptext( tree, TIMEITEM ) ) ;
} /* set_screen */

/*--------------------------------------------------------------*/
    VOID
set_glob( tree )
	LONG tree ;
{
    set_rbut( tree, CNFMROOT, (CONFMYES + !cur_opts.confirm) ) ;
    set_rbut( tree, AUTOROOT, (AUTODYES + !cur_opts.auto_disp) ) ;
    pad_n_itoa( 3, cur_opts.start_pgnum, get_teptext( tree, STRTPGNO ) ) ;
    pad_n_itoa( 3, cur_opts.end_pgnum, get_teptext( tree, ENDPGNO ) ) ;
} /* set_glob */

/*--------------------------------------------------------------*/
    VOID
set_psize( tree ) 
LONG	tree ;
{
    set_rbut( tree, UNITROOT, PRNT_PREFS.paper_units ) ;
    set_rbut( tree, SIZEROOT, PRNT_PREFS.paper_item ) ;
} /* set_psize() */

/*--------------------------------------------------------------*/
    VOID
set_ptray( tree ) 
LONG	tree ;
{
    set_rbut( tree, PTRYROOT, min( AUXNTRAY, PRNT_PREFS.paper_tray ) ) ;
    itoa( max( 1, PRNT_PREFS.paper_tray - AUXNTRAY + 1 ), 
    					get_teptext( tree, AUXNTEXT ) ) ;
} /* set_ptray() */

/*--------------------------------------------------------------
	First byte after "zyxg" indicates port value :
LPT1 :	00h	LPT4 :	10h		COM3 :	20h
LPT2 :	01	LPT5 :	11		COM4 :	21
LPT3 :	02	LPTn :	10 + n - 4	COMn :	20 + n - 3
COM1 :	03
COM2 :	04
--------------------------------------------------------------*/
    BOOLEAN
is_serial( dev )
PDPREFS	*dev ;
{
    return( dev->port == 3 || dev->port == 4 || dev->port >= 0x20 ) ;
} /* is_serial() */

/*--------------------------------------------------------------*/
    VOID
setup_port( tree ) 
LONG	tree ;
{
OBJECT	*ob_ptr ;
BOOLEAN	is_file, show_bkgnd, show_dele ;
WORD	bkgnd_item, dele_item ;
WORD	state, flags, ii ;

    ob_ptr = OB_PTR( tree, FILEITEM ) ;
    is_file = ( ob_ptr->ob_state & SELECTED ) ;
    bkgnd_item = get_rbut( tree, BKGNROOT ) ;
    if ( bkgnd_item < 0 )
    	bkgnd_item = BKGNDYES + !PRNT_PREFS.bkgrnd ;
    dele_item = get_rbut( tree, DELEROOT ) ;
    if ( dele_item < 0 )
        dele_item = DELEYES + !PRNT_PREFS.delete ;
    flags = ( is_file ) ? EDITABLE : NORMAL ;
    state = ( is_file ) ? NORMAL : DISABLED ;
    ob_ptr = OB_PTR( tree, FILEPATH ) ;
    ob_ptr->ob_flags = flags ;
    ob_ptr->ob_state = state ;
    ob_ptr = OB_PTR( tree, FILENAME ) ;
    ob_ptr->ob_flags = flags ;
    ob_ptr->ob_state = state ;
    show_bkgnd = ( bkgnd_ok && is_file && !is_serial( &PRNT_PREFS ) ) ;
    state = ( show_bkgnd ) ? NORMAL : DISABLED ;
    for ( ii = BKGNDSTR; ii <= BKGNDYES + 1; ii++ )
        {
    	ob_ptr = OB_PTR( tree, ii ) ;
    	ob_ptr->ob_state = state ;    
	}
    if ( show_bkgnd )
    	set_rbut( tree, BKGNROOT, bkgnd_item ) ;
    show_dele = show_bkgnd && ( bkgnd_item == BKGNDYES ) ;
    state = ( show_dele ) ? NORMAL : DISABLED ;
    for ( ii = DELESTR; ii <= DELEYES + 1; ii++ )
        {
    	ob_ptr = OB_PTR( tree, ii ) ;
    	ob_ptr->ob_state = state ;    
	}
    if ( show_dele )
	set_rbut( tree, DELEROOT, dele_item ) ;
} /* setup_port() */	    		   

/*--------------------------------------------------------------*/
/* set appropriate states and flags. redraw only that area of	*/
/* the dialog affected. 					*/
    VOID
port_tggl( tree, tggl_dele )
LONG	tree ;BOOLEAN	tggl_dele ;
{
WORD	x, y, h, junk, y2 ;
OBJECT	*ob_ptr ;
    
    setup_port( tree ) ;
    if ( tggl_dele )
        {
    	objc_offset( tree, DELEROOT, &junk, &y ) ;   /* where to beg redraw */
    	objc_offset( tree, FILEPATH, &junk, &y2 ) ; /* where to end redraw */
	}
    else
        {
    	objc_offset( tree, BKGNROOT, &junk, &y ) ;   /* where to beg redraw */
    	objc_offset( tree, OKPFILE, &junk, &y2 ) ; /* where to end redraw */
	}
    h = y2 - y ;
    ob_ptr = OB_PTR( tree, FILEPATH ) ; /* the widest object */
    objc_offset( tree, FILEPATH, &x, &junk ) ;   /* where to beg redraw */
    objc_draw( tree, ROOT, MAX_DEPTH, x, y, ob_ptr->ob_width, h ) ;
} /* port_tggl() */

/*--------------------------------------------------------------*/
    VOID
port_string( tree ) 
LONG	tree ;
{
OBJECT	*ob_ptr ;
WORD	port_type, port_num, delta ;

    ob_ptr = OB_PTR( tree, DFLTPORT ) ;	/* pointer to default port button */
    port_type = ( is_serial( &PRNT_PREFS ) ) ? COM1ITEM : LPT1ITEM ;
    rsrc_gaddr( GAD_STRING, port_type, &ob_ptr->ob_spec ) ;
    if ( port_type == COM1ITEM )
        {
	delta = ( PRNT_PREFS.port <= 4 ) ? (-2) : (-( 0x20 - 0x03 )) ;
	port_num = PRNT_PREFS.port + delta ;
	}
    else
        {
	delta = ( PRNT_PREFS.port <= 2 ) ? 1 : ( -( 0x10 - 0x04 ) ) ;
	port_num = PRNT_PREFS.port + delta ;
	}
    itoa( port_num, ob_ptr->ob_spec + strlen( ob_ptr->ob_spec ) - 1 ) ;
} /* port_string() */	    		   

/*--------------------------------------------------------------*/
/* Stretch or shrink port preferences dialog to include or not 	*/
/* include background printing options. These options are not 	*/
/* available for serial devices. Shift items up or down in dialog */
/* and shorten or lengthen dialog.				*/
    VOID
fix_bkgnd( tree ) 
LONG	tree ;
{
OBJECT	*ob1_ptr, *ob2_ptr ;
WORD	flags, delta, ii ;

    flags = ( bkgnd_ok && !is_serial( &PRNT_PREFS ) ) ? NORMAL : HIDETREE ;
    ob2_ptr = OB_PTR( tree, DELEROOT ) ;
    ob2_ptr->ob_flags = flags ;
    ob1_ptr = OB_PTR( tree, BKGNROOT ) ;
    ob1_ptr->ob_flags = flags ;
    delta = 2 * ( ob2_ptr->ob_y - ob1_ptr->ob_y ) ;
    ob2_ptr = OB_PTR( tree, FILEPATH ) ;
    if ( flags == NORMAL && ob2_ptr->ob_y <= ob1_ptr->ob_y )
        {
	}
    else if ( flags == HIDETREE && ob2_ptr->ob_y > ob1_ptr->ob_y )
	delta = ( -delta ) ;	
    else
        delta = 0 ;
    for ( ii = FILEPATH; ii <= NOTESTR; ii++ )
        {
	ob1_ptr = OB_PTR( tree, ii ) ;
	ob1_ptr->ob_y += delta ;
	}
    ob1_ptr = OB_PTR( tree, ROOT ) ;
    ob1_ptr->ob_height += delta ;
} /* fix_bkgnd() */	    		   

/*--------------------------------------------------------------*/
/* Checks path string to make sure that drive and path exist.	*/
/* If path string is not terminated with a back slash, one is	*/
/* added, so path had best have room for one more character.	*/
    BOOLEAN
check_path( path )
FBYTE	*path ;
{
WORD	length ;
BOOLEAN	ret ;	

    ret = FALSE ;
    if ( valid_drive( path[ 0 ] ) )
        {
	length = strlen( path ) ;
	if ( path[ length - 1 ] == BSLASH )
	    path[ length - 1 ] = NULL ;
	else
	    path[ length ] = NULL ;
	length = strlen( path ) ;
	if ( length == 2 && path[ 1 ] == COLON )
	    ret = TRUE ;
    	else if ( dos_sfirst( path, F_SUBDIR ) )
	    ret = TRUE ;
	path[ length ] = BSLASH ;
	path[ length + 1 ] = NULL ;
	}
    return( ret ) ;
} /* check_path() */	    		   

/*--------------------------------------------------------------*/
/* set first values for PORTPREF dialog.			*/
    VOID
set_port( tree ) 
LONG	tree ;
{
BYTE	fname[ NAME_LENGTH ] ;
FBYTE	*path_ptr ;

    if ( *PRNT_PREFS.file == NULL )
        {
   	rsrc_gaddr( GAD_STRING, CLIPBRD, &path_ptr ) ;
	strcpy( path_ptr, PRNT_PREFS.file ) ;
	PRNT_PREFS.file[ 0 ] = opt_dir[ 0 ] ;	/* same drive as output.app */
	if ( ! check_path( PRNT_PREFS.file ) )
            del_fname( cur_opts.gsx_wc, PRNT_PREFS.file ) ;
	strcat( PFILE_NAME, PRNT_PREFS.file ) ;
	}
    del_fname( PRNT_PREFS.file, get_teptext( tree, FILEPATH ) ) ;
    del_path( PRNT_PREFS.file, fname ) ;
    fname_to_etext( fname, get_teptext( tree, FILENAME ) ) ;
    port_string( tree ) ;
    set_rbut( tree, PORTROOT, DFLTPORT + PRNT_PREFS.to_file ) ;
    set_rbut( tree, BKGNROOT, BKGNDYES + !PRNT_PREFS.bkgrnd  ) ;
    set_rbut( tree, DELEROOT, DELEYES  + !PRNT_PREFS.delete  ) ;
    fix_bkgnd( tree ) ;
    setup_port( tree ) ;
} /* set_port() */	    		   

/*--------------------------------------------------------------*/
    VOID
set_film( tree )
	LONG tree ;
{
	set_rbut( tree, FILMROOT, (FILMTYP0 + CMRA_PREFS.film) ) ;
	set_rbut( tree, LGHTROOT, (DARKEN3 + CMRA_PREFS.lgt) ) ;
} /* set_film */

/*--------------------------------------------------------------*/
    VOID
get_screen( tree )
	LONG tree ;
{
    SCRN_PREFS.wait_for_key = !( get_rbut( tree, WAITROOT ) - WAITKEY ) ;
    SCRN_PREFS.cycle_until_esc = (get_rbut( tree, CYCLROOT ) - NOCYCLE) ;
    SCRN_PREFS.time = atoi( get_teptext( tree, TIMEITEM ) ) ;
} /* get_screen */

/*--------------------------------------------------------------*/
    VOID
get_printer( tree )
	LONG tree ;
{
    PRNT_PREFS.final_ff = !(get_rbut( tree, FFFROOT ) - YESFFF) ;
    PRNT_PREFS.scale = (get_rbut( tree, PRSCROOT ) - PRNTFS) ;
    gt_position( tree, POSIROOT, &PRNT_PREFS.horzjust, &PRNT_PREFS.vertjust );
} /* get_printer */

/*--------------------------------------------------------------*/
    VOID
get_psize( tree ) 
LONG	tree ;
{
UWORD	width, height ;
WORD	str_num ;
FBYTE	*ptr ;

    PRNT_PREFS.paper_units = get_rbut( tree, UNITROOT ) ;
    PRNT_PREFS.paper_item = get_rbut( tree, SIZEROOT ) ;
    str_num = ( PRNT_PREFS.paper_units == INCHUNIT ) ? HFSZINCH : HFSZCENT ;
    str_num += PRNT_PREFS.paper_item - HALFSIZE ;
    rsrc_gaddr( GAD_STRING, str_num, &ptr ) ;
    while ( *ptr != BEG_PAREN )
        ++ptr ;
    width = dec_to_int( ptr ) ;
    while ( *ptr != LCASE_X && *ptr != UCASE_X )
        ++ptr ;
    height = dec_to_int( ptr ) ;
    if ( PRNT_PREFS.paper_units != INCHUNIT ) 
        {
	width = _umul_div( (LONG)(width), 100, CM_PER_INx100 ) ;
	height = _umul_div( (LONG)(height), 100, CM_PER_INx100 ) ;
	}
    PRNT_PREFS.paper_width = width ;
    PRNT_PREFS.paper_height = height ;
} /* get_psize() */

/*--------------------------------------------------------------*/
    VOID
get_ptray( tree ) 
LONG	tree ;
{
    PRNT_PREFS.paper_tray = get_rbut( tree, PTRYROOT ) ;
    PRNT_PREFS.pause = ( PRNT_PREFS.paper_tray == MANUFEED ) ;
    if ( PRNT_PREFS.paper_tray == AUXNTRAY )
        PRNT_PREFS.paper_tray += (atoi( get_teptext( tree, AUXNTEXT ) ) - 1 );
} /* get_ptray() */

/*--------------------------------------------------------------*/
    VOID
get_port( tree ) 
LONG	tree ;
{
TEDINFO	*path, *name ;
BYTE	temp[ NAME_LENGTH ] ;

    PRNT_PREFS.to_file = get_rbut( tree, PORTROOT ) - DFLTPORT ;
    if ( !PRNT_PREFS.to_file )
        return ;
    if ( bkgnd_ok )
        {
    	PRNT_PREFS.bkgrnd = !(get_rbut( tree, BKGNROOT) - BKGNDYES) ;
   	if ( PRNT_PREFS.bkgrnd )    
	    PRNT_PREFS.delete = !(get_rbut( tree, DELEROOT) - DELEYES ) ;
	}
    path = get_spec( tree, FILEPATH ) ;
    name = get_spec( tree, FILENAME ) ;
    etext_to_fname( name->te_ptext, temp ) ;
    if ( !check_path( path->te_ptext ) || *temp == NULL )
        { 	/* can't find this path or name == null */
	do_miss( path->te_ptext, temp ) ;
	return ; 
	}
    strcpy( path->te_ptext, PRNT_PREFS.file ) ;
    if ( !get_ext( temp ) )	/* get_ext() returns 0 iff != .GEM,.OUT, etc*/
    	strcat( temp, PRNT_PREFS.file ) ;
    else
        strcat( PFILE_NAME, PRNT_PREFS.file ) ;
} /* get_port() */	    		   

/*--------------------------------------------------------------*/
    VOID
get_glob( tree )
	LONG tree ;
{
WORD	start, end ;

    cur_opts.confirm = !( get_rbut( tree, CNFMROOT ) - CONFMYES ) ;
    cur_opts.auto_disp = !( get_rbut( tree, AUTOROOT ) - AUTODYES ) ;
    start = max( 1, atoi( get_teptext( tree, STRTPGNO ) ) ) ;
    end = max( start, atoi( get_teptext( tree, ENDPGNO ) ) ) ;
    cur_opts.start_pgnum = start ;
    cur_opts.end_pgnum = end ;
} /* get_glob */

/*--------------------------------------------------------------*/
    VOID
gt_film( tree )
	LONG tree ;
{
    CMRA_PREFS.film = (get_rbut( tree, FILMROOT ) - FILMTYP0 ) ;
    CMRA_PREFS.lgt = (get_rbut( tree, LGHTROOT ) - DARKEN3 ) ;
} /* gt_film */

/*--------------------------------------------------------------*/
    VOID
set_opts( dial_num, tree ) 
WORD	dial_num ;
LONG 	*tree ;
{
	rsrc_gaddr( GAD_TREE, dial_num, tree ) ;
	switch ( dial_num )
	    {
	    case SCRNPREF : set_screen( *tree ) ;
			    break ;
	    case PRNTPREF : set_printer( *tree ) ;
			    break ;
	    case PORTPREF : set_port( *tree ) ;
	    		    break ;
	    case PSIZPREF : set_psize( *tree ) ;
	    		    break ;
	    case TRAYPREF : set_ptray( *tree ) ;
	    		    break ;
	    case FILMPREF  : set_film( *tree ) ;
			    break ;
	    case GLOBPREF : set_glob( *tree ) ;
			    break ;
	    } /* switch */
} /* set_opts */

/*--------------------------------------------------------------*/
    VOID
get_opts( item, tree ) 
WORD	item ;
LONG 	tree ;
{
	switch ( item )
	    {
	    case SCRNPREF : get_screen( tree ) ;
			    break ;
	    case PRNTPREF:  get_printer( tree ) ;
			    break ;
	    case PSIZPREF : get_psize( tree ) ;
	    		    break ;
	    case TRAYPREF : get_ptray( tree ) ;
	    		    break ;
	    case PORTPREF : get_port( tree ) ;
	    		    break ;
	    case FILMPREF : gt_film( tree ) ;
			    break ;
	    case GLOBPREF : get_glob( tree ) ;
			    break ;
	    } /* switch */
} /* get_opts */

/*--------------------------------------------------------------*/
/* count on the fact that the icon is always in the top, left	*/
/* of the dialog, and that once sorted, is always object #1	*/
/* in the dialog tree. With these assumptions this routine can 	*/
/* be used for four different dialogs, PSIZPREF, TRAYPREF,    	*/
/* PORTPREF, and PRNTPREF. (both printers and plotters) 	*/
    VOID
pref_icon( tree )
LONG	tree ;			/* ptr to dialog's tree */
{
LONG	icn_tree ;		/* ptr to icon dialog */
WORD	dial_num ;		/* correct dialog for icons */
WORD	icon_num, string_num ;  /* correct icon, string numbers */
OBJECT	*ob_ptr ;

    if ( gl_hcell >= 12 )
	{
	dial_num = HRESICNS ;
    	icon_num = ( DEV_TYPE == PRNT_TYPE ) ? HPRNTICN : HPLOTICN ;
	}
    else
	{
	dial_num = LRESICNS ;
    	icon_num = ( DEV_TYPE == PRNT_TYPE ) ? LPRNTICN : LPLOTICN ;
	}
    rsrc_gaddr( GAD_TREE, dial_num, &icn_tree ) ;
    do_icn_img( tree, 1, icn_tree, icon_num ) ; /* icon is #1 object */
    string_num = ( DEV_TYPE == PRNT_TYPE ) ? PRINTER : PLOTTER ;
    ob_ptr = OB_PTR( tree, 2 ) ;	/* title is #2 object */
    rsrc_gaddr( GAD_STRING, string_num, &ob_ptr->ob_spec ) ;
} /* pref_icon() */

/*--------------------------------------------------------------*/
    VOID
icon_name( tree )
LONG	tree ;
{
ICONBLK	*icn_blk ;

    icn_blk = get_spec( tree, 1 ) ;	/* all dialogs are sorted so that */
       					/* icon in upper left is #1.	  */
    icn_blk->ib_ptext = DEV_ID ;
} /* icon_name() */

/*--------------------------------------------------------------*/
    VOID
opt_dial( dial_num, ok_num, can_num, help_num)
WORD	dial_num, ok_num, can_num, help_num ;
{
 	LONG	tree, junk ;
	WORD	ok ;

startagain: ;

    set_opts( dial_num, &tree ) ;
    if ( dial_num == TRAYPREF )
        pref_icon( tree ) ;
    if ( dial_num != GLOBPREF )
        icon_name( tree ) ;
    junk = do_dialog( dial_num ) ;
    ok = okcanhlp( tree, ok_num, can_num, help_num) ;
    
    if ( ok == OK )
	get_opts( dial_num, tree ) ;
    else if (ok == 2 )  /* help */
    {
       ShowHelp(dial_num);
       goto startagain;
    }

} /* opt_dial */

/*--------------------------------------------------------------*/
    VOID
prnt_dial()
{
LONG	tree ;
WORD	dial_out ;
WORD 	xd, yd, wd, hd, xc, yc ;

    set_opts( PRNTPREF, &tree ) ;
    pref_icon( tree ) ;
    icon_name( tree ) ;
    xc = gl_desk.g_w / 2 ;
    yc = gl_desk.g_h / 2 ;
    form_center( tree, &xd, &yd, &wd, &hd ) ;
    form_dial(FMD_START, xc, yc, 0, 0, xd, yd, wd, hd);
    form_dial(FMD_GROW,  xc, yc, 0, 0, xd, yd, wd, hd);
    dial_out = do_prntdial( tree, xd, yd, wd, hd ) ;
    form_dial(FMD_SHRINK, xc, yc, 0, 0, xd, yd, wd, hd);
    form_dial(FMD_FINISH, xc, yc, 0, 0,	xd, yd, wd, hd);
    if ( dial_out != OK )
	return ;
    get_opts( PRNTPREF, tree ) ;
 } /* prnt_dial */

/*----------------------------------------------------------------------*/  
    VOID
swap_psize( tree, new_units )
    LONG	tree ;
    WORD	new_units ;
{
WORD	free_str, ii ;
OBJECT	*ob_ptr ;

    free_str = ( new_units == INCHUNIT ) ? HFSZINCH : HFSZCENT ;
    for ( ii = HALFSIZE; ii <= B5SIZE; ii++, free_str++ )
	{
    	ob_ptr = OB_PTR( tree, ii ) ;
    	rsrc_gaddr( GAD_STRING, free_str, &ob_ptr->ob_spec ) ;
	}
} /* swap_psize */

/*----------------------------------------------------------------------*/  
    WORD
do_pszdial( tree, x, y, w, h )
    LONG	tree ;
    WORD	x, y, w, h ;
{
    BOOLEAN	new_units, cur_units ;
    WORD	dial_out ;
    BOOLEAN	ttrue = TRUE ;
    
    swap_psize( tree, PRNT_PREFS.paper_units ) ;
    objc_draw( tree, ROOT, MAX_DEPTH, x, y, w, h ) ;
    cur_units = new_units = PRNT_PREFS.paper_units ;
    do
    {
	form_do( tree, 0 ) ;
	dial_out = okcan( tree, OKPSIZE, CANPSIZE ) ;
	if ( dial_out )
	    return( dial_out ) ;
        new_units = get_rbut( tree, UNITROOT ) ;
  	if ( cur_units != new_units )
 	    {
	    swap_psize( tree, new_units ) ;
	    objc_draw( tree, SIZEROOT, MAX_DEPTH, x, y, w, h ) ;
	    cur_units = new_units ;
	    }
    } while ( ttrue ) ;	/* metaware doesn't like "while (TRUE)" */
} /* do_pszdial() */

/*----------------------------------------------------------------------*/  
    VOID
psize_dial()
{
LONG	tree ;
WORD	dial_out ;
WORD 	xd, yd, wd, hd, xc, yc ;

    set_opts( PSIZPREF, &tree ) ;
    pref_icon( tree ) ;
    icon_name( tree ) ;
    xc = gl_desk.g_w / 2 ;
    yc = gl_desk.g_h / 2 ;
    form_center( tree, &xd, &yd, &wd, &hd ) ;
    form_dial(FMD_START, xc, yc, 0, 0, xd, yd, wd, hd);
    form_dial(FMD_GROW,  xc, yc, 0, 0, xd, yd, wd, hd);
    dial_out = do_pszdial( tree, xd, yd, wd, hd ) ;
    form_dial(FMD_SHRINK, xc, yc, 0, 0, xd, yd, wd, hd);
    form_dial(FMD_FINISH, xc, yc, 0, 0,	xd, yd, wd, hd);
    if ( dial_out != OK )
	return ;
    get_opts( PSIZPREF, tree ) ;
} /* psize_dial() */

/*--------------------------------------------------------------*/
    VOID
port_dial()
{
	LONG tree, l_dummy ;
	WORD x, y, w, h;
	WORD dial_out ;
	WORD	cur_port, new_port, cur_bkgnd, new_bkgnd ;

    set_opts( PORTPREF, &tree ) ;
    pref_icon( tree ) ;
    icon_name( tree ) ;
    w = 0;
    h = 0;
    l_dummy = start_dial( PORTPREF, &x, &y, &w, &h ) ;
    do {
	cur_port = get_rbut( tree, PORTROOT ) ;
	cur_bkgnd = get_rbut( tree, BKGNROOT ) ;
	form_do( tree, 0 ) ;
	dial_out = okcan( tree, OKPFILE, CANPFILE ) ;
	if ( !dial_out )
	    {
    	    new_port = get_rbut( tree, PORTROOT ) ;
	    new_bkgnd = get_rbut( tree, BKGNROOT ) ;
	    if ( ( cur_port != new_port ) || ( cur_bkgnd != new_bkgnd ) )
		port_tggl( tree, cur_bkgnd != new_bkgnd ) ;
	    } /* if !dial_out */
	} while ( !dial_out ) ;
    end_dial( tree, x, y, w, h ) ;
    if ( dial_out == OK )
	get_port( tree ) ;
} /* port_dial */

/*--------------------------------------------------------------*/
    VOID
dial_opt( item )
WORD	item ;
{
    switch ( item )
	{
	case GLOBITM0 : opt_dial( GLOBPREF, OKPGOPT, CANPGOPT, GLOBHELP ) ;
			break ;
	case SVPFITM0 : make_def() ;
			break ;
	case SCRNITEM :
		if ( DEV_TYPE & SCRN_TYPE )
		    opt_dial( SCRNPREF, OKSCRN, CANSCRN, SCRNHELP ) ;
		break;
	case PAGEITEM :
		if ( DEV_TYPE & ( PLOT_TYPE | PRNT_TYPE ) )
		    prnt_dial() ;
		break;
	case PGSZITEM :
		if ( DEV_TYPE & ( PRNT_TYPE | PLOT_TYPE ) )
		    psize_dial() ;
		break ;
	case PTRAYITM :
		if ( DEV_TYPE & ( PRNT_TYPE | PLOT_TYPE ) )
		    opt_dial( TRAYPREF, OKPTRAY, CANPTRAY, PTRYHELP ) ;
		break ;
	case FILERDIR :
		if ( DEV_TYPE & ( PRNT_TYPE | PLOT_TYPE ) )
		    port_dial() ;
		break ;
	case FILMITEM :
		if ( DEV_TYPE & CMRA_TYPE )
		    opt_dial( FILMPREF, OKFLMOPT, CANFMOPT, FILMHELP ) ;
		break;
	} /* switch */
} /* dial_opt */

#define HLP_NONE  2000

/* FUNCTION: ShowHelp - Shows help for specified dialog. Reads from RSC */
void ShowHelp(dial_num)
{
   int rsad;

   switch(dial_num)
   {
      case SCRNPREF: 
         rsad = SCRNPREH;
         break;
      case PRNTPREF:
         rsad = PRNTPREH;
         break;
/*      case FILMPREF:
         rsad = FILMPREH;*/
/*         strcpy(helpstr, "[1][Use this dialog to set options regarding|camera film.][ OK ]");*/
         break;
/*      case COLRPREF:
         rsad = COLRPREH;*/
/*         strcpy(helpstr, "[1][Use this dialog to set options regarding|camera colour settings.][ OK ]");*/
         break;
/*      case GLOBPREF:
         rsad = GLOBPREH;*//*
         strcpy(helpstr, "[1][Use this dialog to set options that affect|the whole of Output.][ OK ]");*/
/*         break;
      case PSIZPREF:
         rsad = PSIZPREH; *//*
         strcpy(helpstr, "[1][Use this dialog to set options regarding|paper size.][ OK ]");*/
/*         break;
      case TRAYPREF:
         rsad = TRAYPREH; *//*
         strcpy(helpstr, "[1][Use this dialog to set options regarding|paper trays.][ OK ]");*/
/*         break;
      case PORTPREF:
         rsad = PORTPREH; *//*
         strcpy(helpstr, "[1][Use this dialog to set options regarding|where output should be sent - the|printer or a file.][ OK ]");*/
         break;
      default:
         rsad = HLPNONE;
         break;
   }

   f_alert(rsad, 1);
/*   form_alert(1, helpstr);*/
}

/* end of ooptions.c */

