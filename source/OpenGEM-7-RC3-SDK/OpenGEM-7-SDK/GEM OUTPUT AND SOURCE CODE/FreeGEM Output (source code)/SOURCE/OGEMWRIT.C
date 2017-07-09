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
#include "xgembind.h"
#include "vdibind.h"
#include "dosbind.h"

#include "output.h"
#include "odefs.h"
#include "ovar.h"

EXTERN WORD	open_mf() ;			/* obuffer.c */
EXTERN WORD	mfread_buffer() ;
EXTERN WORD	close_mf() ;
EXTERN WORD	rewind_mf() ;

EXTERN VOID 	set_trfm() ;			/* odisplay.c */
EXTERN BOOLEAN 	draw_page() ;
EXTERN WORD	check_esc() ;
EXTERN WORD	hold_pix() ;
EXTERN VOID	trfm_pts() ;
EXTERN VOID	change_paper() ;

EXTERN WORD	min() ;				/* outility.c */
EXTERN WORD	max() ;
EXTERN WORD	abs() ;
EXTERN WORD	f_alert() ;
EXTERN VOID 	movb() ;
EXTERN WORD	atoi() ;
EXTERN VOID	del_fname() ;
EXTERN VOID	del_path() ;

EXTERN VOID 	strcpy() ;			/* ooptimz.c */
EXTERN VOID	strcat() ;
EXTERN WORD	strlen() ;
EXTERN BOOLEAN	strcmp() ;

EXTERN VOID	_vq_scan() ;			/* vqscan.c */

/*--------------------------------------------------------------*/
   VOID
set_misc( b_f, l_r, h_j, v_j ) 
WORD	b_f, l_r, h_j, v_j ;
{
    make_fit  = b_f ;
    let_rot   = l_r ;
    horz_just = h_j ;
    vert_just = v_j ;
} /* set_misc */

/*--------------------------------------------------------------*/
/* Finds end of field.						*/
	BYTE
*find_end( s ) 
    BYTE	*s ;
{
WORD	i ;
   
    i = 0 ;
    while ( s[ i ] )
	{
	if ( 	( SP == s[ i ] ) || 
		( COMMA == s[ i ] ) || 
		( CR == s[ i ] ) ||
		( LF == s[ i ] ) )
	    return( &s[ i ] ) ;
	i++ ;
	}
    return( (-1) ) ;
} /* find_end */

/*--------------------------------------------------------------*/
    WORD
check_b_end( found_eol ) 
BOOLEAN		*found_eol ;
{
    WORD	line_len ;
    WORD	b_remaining ;

    line_len = find_eol( b_length, found_eol ) ;
    if ( line_len )
	return( line_len ) ;
    b_remaining = b_length - b_indx ;
    movb( b_remaining, &buffer.TXT[ b_indx ], &buffer.TXT[ 0 ] ) ;
    b_length = mfread_buffer( b_remaining ) ;
    b_indx = 0 ;
    return( find_eol( b_length, found_eol ) ) ;
} /* check_b_end */

/*--------------------------------------------------------------*/
   VOID
get_fname( name, px, py, pw, ph )  
WORD    *px, *py, *pw, *ph ;
BYTE	*name ;
{
BYTE	*start, *f_end;
BYTE	tmp_name[ NAME_LENGTH ] ;
    
    start = find_end( &buffer.TXT[ b_indx ] ) + 1;	/* find first space */
    f_end = find_end( start );
    *f_end = NULL;
    nn = atoi( start ) ;
    start = f_end + 1 ;
    f_end = find_end( start );
    if ( is_1st_word )
	   {
	   *px = nn ;
	   *f_end = NULL ;
	   *py = atoi( start ) ;
	   start = f_end + 1 ;
	   f_end = find_end( start ) ;
	   *f_end = NULL ;
	   *pw = atoi( start ) ;
	   start = f_end + 1 ;
	   f_end = find_end( start ) ;
	   *f_end = NULL ;
	   *ph = atoi( start ) ;
	   nn = *ph ;
	   start = f_end + 1 ;
	   f_end = find_end( start ) ;
           }
    *f_end = NULL;
    strcpy( start, name);
    if ( !dos_sfirst( (FBYTE *)( name ), 0 ) )
	{
	del_path( name, tmp_name ) ;
	del_fname( buffer.CUR_FNAME, name ) ;
	strcat( tmp_name, name ) ;
	}
} /* get_fname */

/*--------------------------------------------------------------*/
/* out_extent[] is used for the clip rect. and for the "output	*/
/* window" call in draw_page(). Set it to the entire area set	*/
/* aside for graphics within the alpha slices. Since the black	*/
/* portions of alpha text chars are not centered in their boxes,*/
/* centering the graphic so that the amount of white space be-	*/
/* tween it and the text above and below is almost a half a 	*/
/* character off at best. An attempt is made, however, to set 	*/
/* dvc_nyoff so that the graphic is not ALWAYS shoved up against*/
/* either the top or the bottom text.				*/
   VOID 
redo_offsets( px, py, pw, ph )
WORD 	*px, *py, *pw, *ph ;
{
WORD    x_offset, y_offset,
	g_height, a_height, a_width, factor, junk ;
WORD	dy_graphic ;	/* size in pixels of graphic/alpha area */
WORD	dy_image ;

    _vq_scan( cur_hndl, &g_height, &junk, &a_height, &junk, &factor,  &a_width, &junk ) ;
	
    dvc_nxoff = dvc_nyoff = 0;
	
    x_offset = *px * a_width / factor ;
    dvc_nxoff = (x_offset < 0 || x_offset >= dvc_nxpixel ) ? 0 : x_offset;
    y_offset = *py * a_height / factor;
    dvc_nyoff = (y_offset < 0 || y_offset >= dvc_nypixel ) ? 0 : y_offset;
	
    mf_ytrans = out_extent.top ;	/* leave trans in world coords */
    mf_xtrans = out_extent.left ;	/* leave trans in world coords */
    trfm_pts( 2, &out_extent ) ; 	/* out_extent goes to device coords */
    dy_image = abs( out_extent.bottom - out_extent.top ) + 1 ;
    out_extent.left = out_extent.right = dvc_nxoff ;
    out_extent.top  = out_extent.bottom = dvc_nyoff ;
    out_extent.right  += ( ( *pw * a_width ) / factor ) - 1 ;
    out_extent.bottom += ( *ph * a_height ) / factor ; /* max alpha size */
    out_extent.bottom = ( out_extent.bottom / g_height ) * g_height ;
    out_extent.bottom -= 1 ; /* trunc to last graphic slice and dec. */
    dy_graphic = abs( out_extent.bottom - out_extent.top ) + 1 ;
    dvc_nyoff += max( 0, ( dy_graphic - dy_image) / 2 ) ;
} /* redo_offsets  */

/*--------------------------------------------------------------*/
   VOID
reset_out()
{
WORD	ii, buf_cnt, junk ;
LONG	lb_indx ;

    open_mf( buffer.CUR_FNAME ) ;
    b_length = buf_lgth ;
    lb_indx = sav_byte ;
    buf_cnt = (UWORD)( sav_byte / (LONG)(sizeof( buffer.MF ) ) ) ;
    for ( ii = 1; ii <= buf_cnt; ii++ ) 
	{
	b_length = mfread_buffer( 0 ) ;
	lb_indx -= (LONG)(sizeof( buffer.MF )) ;
	}
    b_indx = (WORD)( lb_indx ) ;
    check_b_end( &junk ) ;	/* reads first buffer full */
} /* reset_out */

/*--------------------------------------------------------------*/
   BOOLEAN
five_commas()
{
    if (( buffer.TXT[ b_indx + 5  ] == COMMA ) &&
	( buffer.TXT[ b_indx + 8  ] == COMMA ) &&
	( buffer.TXT[ b_indx + 11 ] == COMMA ) &&
	( buffer.TXT[ b_indx + 14 ] == COMMA ) &&
	( buffer.TXT[ b_indx + 17 ] == COMMA ))
	    return(TRUE);
    else
	    return(FALSE);
} /* five_commas */

/*--------------------------------------------------------------*/
   VOID
out_gem() 
{
WORD	tbf, tlr, thj, tvj,
	x, y, w, h ;
BYTE	file[ FNAME_LENGTH ] ;

    if ( DEV_TYPE & SCRN_TYPE )
	{
	nn = 0 ;
	return ;
	}
    tbf = make_fit ;
    tlr = let_rot ;
    thj = horz_just ;
    tvj = vert_just ;
    set_misc( FALSE, FALSE, 1, 0 ) ; 	/* center, top */
    is_1st_word = five_commas() ;
    get_fname( file, &x, &y, &w, &h ) ;
    close_mf() ;
    if ( open_mf( file ) == GEM_FILE )
	{
	strcpy( buffer.CUR_FNAME, buffer.d.s2 ) ;
	strcpy( file, buffer.CUR_FNAME ) ;
	set_trfm() ;
	if ( is_1st_word )
	   redo_offsets( &x, &y, &w, &h ) ;
	vs_clip( cur_hndl, TRUE, &out_extent ) ; 
	v_clear_disp_list( cur_hndl ) ;
	if ( ! fonts_loaded )
	    vst_load_fonts( cur_hndl, 0 ) ;
	draw_page() ;
	close_mf() ;
	strcpy( buffer.d.s2, buffer.CUR_FNAME ) ;
	}
    else
	nn = 0 ;	/* draw line placeholders */
    reset_out() ;
    f_open = TRUE ;
    set_misc( tbf, tlr, thj, tvj ) ;    
} /* out_gem */

/*--------------------------------------------------------------*/
    VOID
do_text( column, row, s )
WORD	column, row ;
BYTE	*s ;
{
BYTE	sav_char, *s_ptr ;

    if ( this_pgnum < cur_opts.start_pgnum || this_pgnum > cur_opts.end_pgnum ) 
	return ;
    if ( DEV_TYPE & SCRN_TYPE )
        { 	/* rm all these eol and eof type chars from v_gtext() call */
    	for ( s_ptr = s; *s_ptr != NULL && *s_ptr != EOF &&
			*s_ptr != LF   && *s_ptr != FF  && *s_ptr != CR ;   
			s_ptr++ ) ;
    	sav_char = *s_ptr ;
    	*s_ptr = NULL ;
	}
    else	/* for alpha text, we want to leave all the eol, eof chars. */
        {	/* so, just set s_ptr, sav_char to default values */
    	s_ptr = &s[ strlen( s ) ] ;
    	sav_char = *s_ptr ;	/* always NULL, since end of the string */
	}	    
    if ( *s )
        {
	if ( !( DEV_TYPE & SCRN_TYPE ) )
	    v_alpha_text( cur_hndl, s ) ; 
	else
	    v_gtext( cur_hndl, column * gl_wcell, row * gl_hcell, s ) ;
	}
    *s_ptr = sav_char ;
} /* do_text() */

/*--------------------------------------------------------------*/
    VOID
line_out( start, length, column, row )
WORD	start, length ;
WORD	*column, row ;
{
BYTE	sv_char, *ptr, eight_spaces[ 9 ] ;
WORD	cur_pos, s, num_spaces ;

    sv_char = buffer.TXT[ start + length ];
    buffer.TXT[ start + length ] = NULL;
    if ( !expand_tabs )
	{
	ptr = &buffer.TXT[ start ] ;
	}
    else
	{
	cur_pos = 0 ;
	s = start ;
	while ( cur_pos < length ) 
	    {
	    if ( buffer.TXT[ start + cur_pos ] == HTAB )
	  	{
		strcpy( "        ", eight_spaces ) ;
		num_spaces = 8 - ( *column % 8 ) ;
		eight_spaces[ num_spaces ] = NULL ;
	   	buffer.TXT[ start + cur_pos ] = NULL ;
		do_text( *column, row, &buffer.TXT[ s ] ) ;
		do_text( *column, row, &eight_spaces ) ;
		buffer.TXT[ start + cur_pos ] = HTAB ;
		s = start + cur_pos + 1 ;
		*column += ( num_spaces - 1 ) ;
		}
	    ++cur_pos ;
	    *column += 1 ;
	    }
	ptr = &buffer.TXT[ s ] ;
	}
    do_text( *column, row, ptr ) ;
    *column += strlen( ptr ) ;
    buffer.TXT[ start + length ] = sv_char;
} /* line_out */

/*--------------------------------------------------------------*/
    VOID
out_txt( l_length, col, row )
WORD	l_length ;
WORD	col, row ;
{
WORD	t_indx, column ;
BOOLEAN	do_pause, end_of_page ;

    do_pause = FALSE ;
    end_of_page = (buffer.TXT[ b_indx + l_length - 1 ] == FF ) ;
    if ( end_of_page ) 
	{
	nn = 0 ;
	brk_out = check_esc() ;
	do_pause = !brk_out ;
	do_pause &= !( PRNT_PREFS.bkgrnd && bkgnd_ok ) ;
	do_pause &= ( DEV_TYPE & PLOT_TYPE ) || 
			( ( DEV_TYPE & PRNT_TYPE ) && PRNT_PREFS.pause ) ;
	}
    if ( ! nn )
	{
	t_indx = b_indx ;
	column = col ;
	while ( l_length > 0 )
	    {
	    line_out( t_indx, min( dev.maxintin, l_length ), &column, row ) ;
	    l_length -= dev.maxintin ;
	    t_indx += dev.maxintin ;
	    }
  	if ( do_pause )
	    {		/* Change paper */
	    graf_mouse( MO_ARROW, 0x0L ) ;
    	    f_alert( CHGPAPER, 1 ) ;
	    graf_mouse( MO_HGLASS, 0x0L ) ;
	    }
	if ( end_of_page )
	    ++this_pgnum ;	/* increment current page number */
	}
    else
	{
	--nn ;
	}
} /* out_txt */

/*--------------------------------------------------------------*/
/* returns back length of line or buffer size, whichever is less. */
/* boolean parameter is set to true iff eol marker was found 	*/
    WORD
find_eol( buf_len, found_eol ) 
WORD	buf_len ;
BOOLEAN	*found_eol ;
{
    WORD	line_len ;
    BYTE	cur_char, next_char ;

    line_len = b_indx ;
    expand_tabs = FALSE ;
    *found_eol = TRUE ;
    while ( line_len < buf_len )
	{
	cur_char = buffer.TXT[ line_len ] ;
	next_char = buffer.TXT[ line_len + 1 ] ;
	expand_tabs |= ( cur_char == HTAB ) ;
	if ( cur_char == EOF )
	    return( line_len - b_indx ) ;
	if ( ( cur_char == LF )  || ( cur_char == FF ) )
	    return( line_len - b_indx + 1 ) ;
	if ( ( cur_char == CR ) && ( next_char == LF ) )
	    return( line_len - b_indx + 2 ) ;
	++line_len ;
	}
    *found_eol = FALSE ;	/* no, we didn't find eol */
    if ( b_indx == 0 )		/* buf contains a long line */
        return( buf_len ) ;
    else 
    	return( 0 ) ;
} /* find_eol */

/*--------------------------------------------------------------*/
   BOOLEAN 
esc_esc( cmd_str )
BYTE 	*cmd_str ;
{
BYTE	first_few[ INTIN_SIZE ] ;

    movb( strlen( cmd_str ), &buffer.TXT[ b_indx ], &first_few ) ;
    first_few[ strlen( cmd_str ) ] = 0 ;
    return( strcmp( &first_few, cmd_str ) ) ;
} /* esc_esc */

/*--------------------------------------------------------------*/
    VOID
do_out()
{
WORD	l_length, line_cnt, col_cnt, max_lines, this_line ;
BOOLEAN write_out, found_eol ;

    rewind_mf() ;
    this_pgnum = 1 ;
    b_length = buf_lgth ;
    sav_byte = 0x0L ;
    b_indx = 0 ;
    nn = 0 ;
    brk_out = FALSE ;
    found_eol = TRUE ;
    l_length = 0 ;
    line_cnt = 0 ;
    col_cnt = 0 ;
    max_lines = ( dev.yres / gl_hcell ) ;
    this_line = line_cnt % max_lines ;
    do
	{
	if ( !( DEV_TYPE & SCRN_TYPE ) )  
	    if ( this_line == 0 || this_line == max_lines )
	    	/* just before first line on first page or at end of page */
	        change_paper() ;	
	write_out = FALSE ;
	col_cnt += l_length ;   /* where to start this part of the line */
	if ( !found_eol )	/* didn't find eol in prev. line in buffer */
	    --line_cnt ;	/* stay on same line */
	else
	    col_cnt = 0 ;
	l_length = check_b_end( &found_eol ) ;
	++line_cnt ;
	if ( !b_length || !l_length )
	    return ;
	if ( esc_esc( GEM_CMD ) )
	    out_gem() ;
	else 
	    write_out = TRUE ;
    	this_line = line_cnt % max_lines ;
	if ( this_line == 0 )
	    this_line = max_lines ; /* our first line is #1, don't use 0 */
	if ( write_out ) 
	    out_txt( l_length, col_cnt, this_line ) ;
	b_indx += l_length ;
 	sav_byte += (LONG)( l_length ) ;
        if ( buffer.TXT[ b_indx - 1 ] == EOF )
	    return ;
	if ( brk_out )
	    return ;
	if ( ( DEV_TYPE & SCRN_TYPE ) && ( this_line == max_lines ) )
	    {	/* at end of screen */
	    hold_pix( 1 ) ;
	    v_clrwk( cur_hndl ) ;
	    }
	} while ( b_length && l_length ) ;
} /* do_out */

/*--------------------------------------------------------------*/
    VOID
scrn_text()		
{
WORD	r[ 4 ] ;
WORD	w_char, h_char, w_cell, h_cell ;

    r[ 0 ] = r[ 1 ] = 0 ;
    r[ 2 ] = dev.xres ;
    r[ 3 ] = dev.yres ;
    vst_font( cur_hndl, 1 ) ;		/* system font */
    vst_point( cur_hndl, 10, &w_char, &h_char, &w_cell, &h_cell ) ;
    vst_color( cur_hndl, 1 ) ;		/* black */
    vst_rotation( cur_hndl, 0 ) ;	/* no rotation */
    vs_clip( cur_hndl, TRUE, &r ) ;
    v_clrwk( cur_hndl ) ;
    do_out() ;
} /* scrn_text */

/*--------------------------------------------------------------*/
    VOID
dsp_out()
{
WORD	num_copy ;

    brk_out = check_esc() ;
    if ( brk_out )
	return ;
    if ( DEV_TYPE & SCRN_TYPE )
	scrn_text() ;
    if ( !( DEV_TYPE & PRNT_TYPE ) )
	return ;
    if ( !prnt_open )         /* ff only between pictures, not at beginning */
        v_clrwk( cur_hndl ) ;
    prnt_open = FALSE ;
    num_copy = atoi( get_teptext( cntl_tr, COPYSTXT ) ) ;
    if ( dev.flags & MULT_CPYS ) /* v_copies() will work for alpha text */
        num_copy = 1 ;
    do
        {
    	do_out() ;
	if ( !is_out_file 	/* .OUT files have FF chars at the end */
	     && ( num_copy > 1 ) ) /* don't do it for the last one */
		v_form_adv( cur_hndl ) ;
	}
    while ( --num_copy ) ;
} /* dsp_out */

/*--------------------------------------------------------------*/
/* OUTGWRT overlay entry point.					*/
   VOID
OUTGWRT()
{
    dsp_out() ;
} /* OUTGWRT */


/* end of ogemwrit.c */

