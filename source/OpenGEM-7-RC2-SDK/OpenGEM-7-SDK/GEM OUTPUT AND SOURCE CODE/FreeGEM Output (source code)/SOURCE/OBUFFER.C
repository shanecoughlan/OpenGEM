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

#include "output.h"
#include "odefs.h"
#include "ovar.h"

EXTERN VOID	del_path() ;			/* outility.c */
EXTERN BOOLEAN	comp_ext() ;
EXTERN VOID	movb() ;
EXTERN VOID	wswap() ;

EXTERN LONG	file_size() ;			/* oportabl.c */

EXTERN VOID	strcpy() ;			/* ooptimz.c */

/*--------------------------------------------------------------*/
/* Searches in the directory specified in 'file' first, then	*/
/* in the current directory, and finally along the search path. */
/* If a file is found, the full name with whatever path it is 	*/
/* in is returned in 'file' and the routine returns TRUE.	*/
    BOOLEAN
search_path( file )
BYTE	*file ;
{
BYTE	name_only[ NAME_LENGTH ] ;

    if ( file[ 1 ] == COLON )
	{
    	if ( dos_sfirst( ADDR( file ), 0 ) ) 
	    return( TRUE ) ;
	del_path( file, &name_only ) ;
    	strcpy( name_only, file ) ;
	}
    shel_find( ADDR( file ) ) ;	/* look in current directory and along search path */
    if ( !dos_sfirst( ADDR( file ), 0 ) ) 
	return( FALSE ) ;
    return( TRUE ) ;
} /* search_path() */

/*-------------------------------------------------------------*/
	WORD
get_ext( f ) 
BYTE	*f ;
{
WORD	ret ;

    ret = 0 ;
    if ( comp_ext( GEM_EXT, f ) )
	ret = GEM_FILE ;
    else if ( comp_ext( OUT_EXT, f ) )
	ret = OUT_FILE ;
    else if ( comp_ext( GMP_EXT, f ) )
	ret = GMP_FILE ;
    return( ret ) ;
} /* get_ext() */

/*-------------------------------------------------------------*/
	WORD
mfread_buffer( start_byte )
WORD	start_byte ;
{
    WORD 	length ;
    LONG	size ;

    fst_buf = FALSE ;
    buf_pos = &buffer.MF_MRKR ;
    size = (LONG)( sizeof( buffer.MF ) - start_byte ) ;
#if I8086
    size = dos_read( f_handle, size, ADDR( &buffer.TXT[ start_byte ] ) ) ;
    length = (UWORD)( size ) ;
#endif
#if MC68K
    /* must byte swap the file while reading it in */
    length = mf_read( f_handle, (UWORD)(size), ADDR( &buffer.TXT[ start_byte ] ) ) ;
#endif
    if ( DOS_ERR )
        return( FALSE ) ;
    length += start_byte ;
    buf_end = buf_pos + length / 2 ;
    return( length ) ;
} /* mfread_buffer */

/*-------------------------------------------------------------*/
	WORD
open_mf( file )
	BYTE *file ;
{
BYTE	*f ;

    if ( f_open )
	return( FALSE ) ;
    file_type = FALSE ;
    f = &buffer.d.s1 ;
    strcpy( file, f ) ;
    if ( !search_path( f ) )
	return( FALSE ) ;
    f_handle = dos_open( ADDR( f ), 0 ) ;
    if ( DOS_ERR )
	return( FALSE ) ;
    if ( file_size( f ) <= 0L )
	return( FALSE ) ;
    f_open = TRUE ;
    buf_lgth = mfread_buffer( 0 ) ;
    if ( buf_lgth )
	{
	fst_buf = TRUE ;
	file_type = get_ext( f ) ;
	switch ( file_type )
    	    {
	    case GEM_FILE :
	    case GMP_FILE : {
			    if ( buffer.MF_MRKR != 0xffff )
			    	return( FALSE ) ;
			    if ( mf_skip( buffer.HDR_SIZE ) )
		    		return( FALSE ) ;
			    break ;
	    		    }
	    case OUT_FILE : break ;
	    default :	    file_type = TXT_FILE ;
			    break ;
	    }
	return( file_type ) ;
	}
    return( FALSE ) ;
} /* open_mf */

/*----------------------------------------------------------------------*/
/* Get an opcode from the metafile.  If it is an end-of-file opcode or 	*/
/* an invalid opcode, return FALSE. Otherwise, get remaining parameters */
/* and return TRUE.							*/
    BOOLEAN
get_mf()
{
    WORD 	ok ;

    if ( !f_open )
    	return( FALSE ) ;
    if ( mf_words( 1, &OPCODE ) )
    	return( FALSE ) ;
    if ( OPCODE == -1 )
    	return( FALSE ) ;
    if ( mf_words( 1, &NUM_PTSIN ) )
        return( FALSE ) ;
    if ( mf_words( 1, &NUM_INTIN ) )
        return( FALSE ) ;
    if ( mf_words( 1, &SUBCODE ) )
        return( FALSE ) ;
    ok = TRUE ;
    if ( NUM_PTSIN > 0 )
      {
        if ( 2*NUM_PTSIN > PTSIN_SIZE )
          ok &= ( 0 == mf_skip( 2*NUM_PTSIN ) ) ;
        else
          ok &= ( 0 == mf_words( 2*NUM_PTSIN, ptsin ) ) ;
      }  
    if ( NUM_INTIN > 0 )
      {
        if ( NUM_INTIN > INTIN_SIZE )
          ok &= ( 0 == mf_skip( NUM_INTIN ) ) ;
        else
          ok &= ( 0 == mf_words( NUM_INTIN, intin ) ) ;
      }  
    return( ok ) ;
}  /* get_mf() */

/**************************************************************/
	WORD
mf_words( count, buf )
	WORD count ;
	WORD *buf ;
{
	WORD n ;

	while( count )
	{
	    if ( buf_pos == buf_end )
	    {
		if ( !mfread_buffer( 0 ) )
		    break ;
	    }
	    n = buf_end - buf_pos ;
	    n = ( n > count ) ? count : n ;
	    count -= n ;
	    while( n-- )
	    {
		*buf++ = *buf_pos++ ;
	    }
	}
	return( count ) ;
} /* mf_words */



/**************************************************************/
	WORD
mf_skip( count )
	WORD count ;
{
WORD n ;

	while( count )
	{
	    if ( buf_pos == buf_end )
	    {
		if ( !mfread_buffer( 0 ) )
		    break ;
	    }
	    n = ( buf_end - buf_pos ) / 2 ;
	    n = ( n > count ) ? count : n ;
	    count -= n ;
	    buf_pos += n ;
	}
	return( count ) ;
} /* mf_skip */


/**************************************************************/
	WORD
close_mf()
{
	if ( f_open )
	{
	    dos_close( f_handle ) ;
    	    f_open = FALSE ;
	    return( TRUE ) ;
	}
	return( FALSE ) ;
} /* close_mf */


/**************************************************************/
	WORD
rewind_mf()
{
	if ( fst_buf )
	{
	    buf_pos = &buffer.MF_MRKR ;
	}
	else
	{
	    dos_lseek( f_handle, 0, 0L ) ;
 	    if ( DOS_ERR )
		return( FALSE ) ;
	    buf_lgth = mfread_buffer( 0 ) ;
	    if ( !buf_lgth )
		return( FALSE ) ;
	    fst_buf = TRUE ;
	}
	if ( file_type == GMP_FILE || file_type == GEM_FILE )
	    mf_skip( buffer.HDR_SIZE ) ;
	return( TRUE ) ;
} /* rewind_mf */


/**************************************************************/
    VOID
get_pgsiz()
{
    rewind_mf() ;
    mf_xtrans = mf_ytrans = 0 ;			/* default sizes */
    mf_xlength = mf_ylength = 32766 ;
    if ( buffer.TRNSFM_KIND == 0 )    /* 0 for NDC coords, 2 for RC coords */
        {
	mf_ytrans = 32766 ;
	mf_ylength = -32766 ;
        }
    if ( (buffer.LL_X - buffer.UR_X ) && 
	    (buffer.LL_Y - buffer.UR_Y ) )
	{
	mf_xtrans = buffer.LL_X ;
	mf_ytrans = buffer.UR_Y ;
	mf_xlength = buffer.UR_X - mf_xtrans ;
	mf_ylength = buffer.LL_Y - mf_ytrans ;
	}
    mf_xlength += ( mf_xlength >= 0 ) ? 1 : -1 ;
    mf_ylength += ( mf_ylength >= 0 ) ? 1 : -1 ;
    if ( buffer.MF_WIDTH && buffer.MF_HEIGHT )
	{
	page_width = buffer.MF_WIDTH ;
	page_height = buffer.MF_HEIGHT ;
	}
    else
	{
	page_width  = ( mf_xlength > 0 ) ? -mf_xlength : mf_xlength ;
	page_height = ( mf_ylength > 0 ) ? -mf_ylength : mf_ylength ;
	make_fit = TRUE ;
	}
    movb( sizeof( RECTANGLE ), &buffer.XMIN, &out_extent ) ;
    if ( mf_xlength < 0 ) 
    	wswap( &out_extent.left, &out_extent.right ) ;
    if ( mf_ylength < 0 ) 
	wswap( &out_extent.top, &out_extent.bottom ) ;
    is_img_file |= ( (file_type == GEM_FILE) && 	/* not .GMP file */
			( buffer.HDR_SIZE >= 15 ) && 
				( buffer.MF_KIND & IMG_FLAG ) ) ;
} /* get_pgsize */
	

/* end of mfbuf.c */