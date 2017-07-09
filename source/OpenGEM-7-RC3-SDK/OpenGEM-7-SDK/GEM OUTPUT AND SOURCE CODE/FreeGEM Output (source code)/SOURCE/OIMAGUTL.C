/* file I/O, load/save pattern code for PAINT			   */
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
#include "dosbind.h"
#include "vdibind.h"

#include "odefs.h"
#include "output.h"
#include "ovar.h"

#define MAX_XPELS	2700		/* 300 dpi * 9 inches */
#define MAX_PLANES	4		

LONG	raw_fptr ;
UWORD	raw_fsize ;
UWORD	scratch[ ( ( MAX_XPELS / 16 ) + 1 ) * MAX_PLANES ] ; 
WORD	xyr[ 8 ] ;		/* source and dest rects in rcs */
GLOBAL	MFDB	src_MFDB, dest_MFDB ;

LONG	decode_buf, dcode_ptr ;
LONG	f_ptr ;

UWORD	f_buf_len, f_buf_size, f_index ;
WORD	vert_rep ;
ULONG	lines_avail ;
WORD 	s_next_plane, s_scan_bytes ;
GLOBAL WORD	blt_byte, scan_byt, planes ;
GLOBAL LONG	blt_addr, buff_addr, nxt_plane ;
GLOBAL WORD	buff_cnt ;
GLOBAL LONG	buff_ptr, blt_ptr ;

GLOBAL struct img_header
{
    WORD	img_ver ;	/* version number */
    WORD	length ;		/* length in words of header */
    WORD	plane_cnt ;
    WORD	patlen ;		/* pattern length 2 bytes*/
    WORD	xsize ;		/* pixel x size in microns */
    WORD	ysize ;		/* pixel y size in microns */
    WORD	num_pels ;	/* length of line in pels */
    WORD	num_scans ;
} header ;

EXTERN WORD	min() ;			/* ooptimz.c */

EXTERN VOID	b_stuff() ;		/* util86.asm */
EXTERN VOID	b_move() ;
EXTERN LONG	seg_off() ;
EXTERN VOID	swapbyte() ;

/*-----------------------------------------------------------------*/
/* When seg-offset pair is stored as a long, add segs and offsets  */
/* separately.							   */
    LONG
addr_add( l1, l2 )
LONG	l1, l2 ;
{
LONG	lo_word ;
UWORD	hi_word ;
    
    lo_word = LLOWD( l1 ) + LLOWD( l2 ) ;
    hi_word = LHIWD( l1 ) + LHIWD( l2 ) + LHIWD( lo_word ) ;
    return( (LONG)(LLOWD(lo_word)) + ( (LONG)(hi_word) << 16) ) ;
} /* addr_add */


/*-----------------------------------------------------------------*/
	UWORD
umin( a, b )
	UWORD		a, b;
{
	return( (a < b) ? a : b );
}


/*-----------------------------------------------------------------*/
    VOID
lb_stuff( m_ptr, m_size, value )
LONG	m_ptr, m_size ;
BYTE	value ;
{
LONG	ptr, size ;

    ptr = m_ptr ;
    size = m_size ;
    while ( size > 0 )
	{
	if ( size > MAX_INT )
	    b_stuff( ptr, MAX_INT, value ) ;
	else
	    b_stuff( ptr, size, value ) ;
	ptr = addr_add( (LONG)(MAX_INT), ptr ) ; 
	size -= MAX_INT ;
	} 
} /* lb_stuff */


/*-----------------------------------------------------------------*/
    VOID
init_MFDB( img_addr, n_planes, p_xy )
LONG	img_addr ;
UWORD	n_planes ;
WORD	*p_xy ;
{
WORD	dx, dy ;

    dx = min( header.num_pels  - 1, p_xy[ 2 ] - p_xy[ 0 ] ) ;
    dy = min( header.num_scans - 1, p_xy[ 3 ] - p_xy[ 1 ] ) ;
    src_MFDB.mp = img_addr ;
    src_MFDB.fwp = ( (header.num_pels + 15) & (~0x0F) ) ;
    src_MFDB.fh = dy + 1 ;
    src_MFDB.fww = src_MFDB.fwp / 16 ; /* size in words from size in bits */
    b_stuff( ADDR( &dest_MFDB ), sizeof( dest_MFDB ), 0 ) ;
    src_MFDB.np = dest_MFDB.np = n_planes ;
    xyr[ 0 ] = xyr[ 1 ] = 0 ;	/* top left source rect */
    xyr[ 4 ] = ( dev.xres - dx ) / 2 ;
    xyr[ 5 ] = ( dev.yres - dy ) / 2 ;
    xyr[ 4 ] -= ( xyr[ 4 ] % 16 ) ;
    xyr[ 5 ] -= ( xyr[ 5 ] % 16 ) ;
    xyr[ 2 ] = xyr[ 0 ] + dx ;
    xyr[ 3 ] = xyr[ 1 ] + dy ;
    xyr[ 6 ] = xyr[ 4 ] + dx ;
    xyr[ 7 ] = xyr[ 5 ] + dy ;
} /* init_MFDB */

#define	RAW_NEED	0x00001000L	/* 4096 */
#define	MAX_BUF		0x0000FFFFL	/* 64K  */
/*-----------------------------------------------------------------*/
    LONG
image_alloc( n_planes, p_xy )
UWORD	n_planes ;
WORD	*p_xy ;
{
LONG	mem_avail ;
LONG	mem_need, scratch_size ;
LONG	img_addr ;

    scratch_size = ( ((LONG)(header.num_pels) / 8) + 1) * (LONG)( header.plane_cnt ) ;
    mem_need =  scratch_size * 0x0AL ;	/* min of 10 scan lines at a time */
    mem_avail = dos_avail() ;
    if ( mem_avail < ( mem_need + RAW_NEED ) ) 
	return( (LONG)(FALSE) ) ;
    raw_fptr = dos_alloc( RAW_NEED ) ;   
    f_buf_size = ( UWORD )( RAW_NEED ) ;
    mem_avail = dos_avail() ;
    if ( mem_avail > MAX_BUF ) 
        mem_avail = MAX_BUF ;
    img_addr = dos_alloc( mem_avail ) ;	/* take all the rest of mem */
    lb_stuff( img_addr, mem_avail, 0 ) ; 
    init_MFDB( img_addr, n_planes, p_xy ) ;
    return( mem_avail ) ;
} /* image_alloc */


/************************************************************************/
/* t r a n s f e r							*/
/************************************************************************/
    VOID
transfer( bytes, loc, file_handle )
    WORD	bytes ;
    LONG	loc ;
    WORD 	file_handle ;
{
    WORD	amount ;
    LONG	tmp_ptr ;

	/* Can the request be satisfied from the current buffer?	*/
    if ( f_index + bytes < f_buf_len )
    {
	/* Transfer the requested bytes from the current buffer.	*/
	b_move( f_ptr, bytes, loc ) ;
	f_index += bytes ;
	f_ptr += ( LONG )( bytes ) ;
	loc += ( LONG )( bytes ) ;
    }  /* End if:  no new buffer needed. */
    else
    {
			/* Transfer what is left in the current buffer.	*/
	b_move( f_ptr, ( amount = f_buf_len - f_index ), loc ) ;
	loc += ( LONG )( amount ) ;
	f_ptr += ( LONG )( amount ) ;
	/* Copy the last byte of the current file buffer into the	*/
	/* first byte of the file buffer to be read in so that a single	*/
	/* byte backspace can be performed, if necessary.  Read in the	*/
	/* new file buffer.  If more must be transferred to the		*/
	/* destination, do so.						*/
	tmp_ptr = raw_fptr ;
	LBSET( tmp_ptr,LBGET( f_ptr ) ) ;
	f_ptr = tmp_ptr ;
	f_ptr++ ;
	f_buf_len = (UWORD)(dos_read( file_handle, (LONG)(f_buf_size - 1), f_ptr ) ) + 1 ;

	if ( amount < bytes )
	{
	    f_index = bytes - amount ;
	    b_move( f_ptr, f_index++, loc ) ;
	    f_ptr += ( LONG )( f_index - 1 ) ;
	}  /* End if:  more to be transferred. */
	else
	    f_index = 1 ;
    }  /* End else: new buffer needed. */
} /* transfer */



#define PATSIZE	 0x0002
/************************************************************************/
/* g e t _ l i n e							*/
/************************************************************************/
    VOID
get_line( file_handle )
WORD	file_handle ;
{
    BYTE	color, opcode ;
    BYTE	temp_array[3], pat_buf[PATSIZE] ;
    WORD	bytes, i, j, run ;

/* If a vertical replication is to be made, little needs to be done.	*/
    if ( vert_rep )
    {
	vert_rep-- ;
	return ;
    }  /* End if:  vertical replication. */

	/* Process a scan line escape command, if one is specified.	*/
    if ( !LBGET( f_ptr ) )
    {
	transfer( 1, ADDR( temp_array ), file_handle ) ;
	if ( !LBGET( f_ptr ) )
	{
		/* Currently, only one scan line escape is defined:	*/
		/* the vertical replication escape.  Process it.	*/
	    transfer( 3, ADDR( temp_array ), file_handle ) ;
	    vert_rep = temp_array[2] - 1 ;
	}  /* End if:  scan line escape command specified. */
	else
	{	
	    f_ptr-- ;
	    f_index-- ;
	}
    }  /* End if:  must check for scan line escape command. */
		/* Loop over the number of bit planes in the source.	*/
    for ( i = 0 ; i < header.plane_cnt ; i++ )
    {
	/* Set the decode buffer pointer to the address of the first	*/
	/* byte in the decode buffer of the plane being processed.	*/
	dcode_ptr = decode_buf + ( LONG )( i * s_next_plane ) ;
	/* Loop until this plane of the scan line has been filled.	*/
	bytes = 0 ;
	while ( bytes < s_scan_bytes )
	{
		/* Process according to the byte being pointed to.  If	*/
		/* the low seven bits is non-zero, a solid run is to be */
		/* output.  Otherwise, if the byte is zero, a pattern   */
		/* run is to be output.  Otherwise ( low seven bits are  */
		/* zero, but the high bit is set ), a bit string is to   */
		/* be output.                                           */
	    if ( ( opcode = ( LBGET( f_ptr ) ) ) & 0x7f )
	    {
		transfer( 1, ADDR( temp_array ), file_handle ) ;
		run = ( WORD )( opcode & 0x7f ) ;
		color = ( opcode & 0x80 ) ? 0xff : 0 ;
		b_stuff( dcode_ptr, run, color ) ;
		dcode_ptr += ( LONG )( run ) ;
		bytes += run ;
	    }  /* End if:  solid run. */
	    else
	    {
		/* Get the first two bytes of the command -- the second	*/
		/* byte is the length of the data which follows.	*/
		transfer( 2, ADDR( temp_array ), file_handle ) ;
		run = ( WORD )( temp_array[1] ) ;
		/* Process according to whether a pattern run or a bit	*/
		/* string has been specified.				*/
		if ( !opcode )
		{
		    transfer( header.patlen, ADDR( pat_buf ), file_handle ) ;
		    for ( j = 0 ; j < run ; j++ )
		    {
			b_move( ADDR( pat_buf ), header.patlen, dcode_ptr ) ;
			dcode_ptr += ( LONG )( header.patlen ) ;
		    }  /* End for:  over pattern run length. */
		    bytes += run * header.patlen ;
		}  /* End if:  pattern run. */
		else
		{
		    transfer( run, dcode_ptr, file_handle ) ;
		    dcode_ptr += ( LONG )( run ) ;
		    bytes += run ;
		}  /* End else:  bit string. */
	    }  /* End else:  pattern run or bit string. */
	}  /* End while:  bytes remain to filled in the scan line. */
    }  /* End for:  over source bit planes. */
}  /* get_line */


/*----------------------------------------------------------------------*/
    VOID
do_decode( cur_hndl, f_hndl, sheight, swidth, src_inc, dst_inc, dst_plane, 
		n_planes, first_time ) 
WORD	cur_hndl, f_hndl ;
WORD	sheight, swidth ;
LONG 	src_inc, dst_inc ;
LONG	dst_plane ;
UWORD	n_planes ;
BOOLEAN first_time ;
{
    WORD	i, j ,k ;
    UWORD	*sptr, *dptr ;
    LONG 	src_mp, dst_mp, sav_dst ;

    dst_mp = src_MFDB.mp ;
    for ( i = sheight ; i > 0 ; i-- )
    	{
	get_line( f_hndl ) ;
	for ( j = header.plane_cnt ; j > n_planes ; j-- )
	    {
	    dptr = scratch ;
	    sptr = &scratch[( j - 1 ) * ( s_next_plane >> 1 )] ;
	    for ( k = ( swidth >> 1 ) ; k > 0 ; k-- )
		*dptr++ |= *sptr++ ;
	    }	     
	src_mp = ADDR( scratch ) ;
  	sav_dst = dst_mp ;
	for ( j = n_planes ; j > 0 ; j-- )
	    {
	    b_move( src_mp, swidth, dst_mp ) ;
#if  I8086
	    swapbyte( dst_mp, (LONG)(swidth >> 1) ) ;
#endif
	    src_mp = addr_add( src_inc, src_mp ) ;
	    dst_mp = addr_add( dst_plane, dst_mp ) ;
	    }
   	dst_mp = sav_dst ;
	dst_mp = addr_add( dst_inc, dst_mp ) ;
        }  /* End for:  over scan lines. */
    src_MFDB.ff = 1 ;  /* standard format */
    vr_trnfm( cur_hndl, &src_MFDB, &src_MFDB ) ;
    if ( first_time ) 
    	v_clrwk( cur_hndl ) ;
    vro_cpyfm( cur_hndl, 3, xyr, &src_MFDB, &dest_MFDB ) ;
} /* do_decode */


#define	HEADSIZE 	0x0008
#define	MAX_UWORD 	0x00007FFFL
/************************************************************************/
/* d e c o d e								*/
/************************************************************************/
    BOOLEAN
decode( cur_hndl, file_handle, n_planes, p_xy )
WORD	cur_hndl, file_handle ;
UWORD	n_planes ;
WORD	*p_xy ;
{
    WORD	swidth, sheight ;
    LONG	dst_plane, mem_sz ;
    LONG 	l_dummy, src_inc, dst_inc ;
    BOOLEAN	first_time ;

    l_dummy = dos_read( file_handle, (LONG)( HEADSIZE<<1 ), ADDR( &header ) ) ;
/* Swap bytes in the header.	*/
#if I8086
    swapbyte( ADDR( &header ), (LONG)(HEADSIZE) ) ;
#endif	
    mem_sz = image_alloc( n_planes, p_xy ) ;
    if ( !mem_sz )
	return( FALSE ) ;
    s_scan_bytes = ( header.num_pels + 7 ) / 8 ;
    s_next_plane = 2*( (header.num_pels + 15 )/16 ) ;
    decode_buf = ADDR( scratch ) ;
    b_stuff( decode_buf, s_next_plane * n_planes, 0 ) ;
    f_ptr = raw_fptr ;
    f_index = 0 ;
    f_buf_len = (UWORD)(dos_read( file_handle, (LONG)(f_buf_size), f_ptr ) ) ;
    vert_rep = 0 ;
    swidth = 1 + min( src_MFDB.fwp >> 3, s_next_plane ) ;
    swidth &= ( ~0x01 ) ;
    sheight = min( p_xy[ 3 ]-p_xy[ 1 ]+1, header.num_scans ) ;
    if ( header.plane_cnt > 1 )
	src_inc = seg_off( s_next_plane ) ;
    else
	src_inc = 0x0L ;
    dst_inc = seg_off( src_MFDB.fww << 1 ) ;
    lines_avail = ( mem_sz / (ULONG)(n_planes) ) / (ULONG)(src_MFDB.fww << 1) ;
    if ( lines_avail > MAX_UWORD )
	lines_avail = MAX_UWORD ;
    vs_clip( cur_hndl, 1, &xyr[ 4 ] ) ;
    first_time = TRUE ;
    do
	{
	src_MFDB.fh = umin( sheight, (UWORD)(lines_avail) ) ;
 	xyr[ 3 ] = xyr[ 1 ] + src_MFDB.fh - 1 ;
 	xyr[ 7 ] = xyr[ 5 ] + src_MFDB.fh - 1 ;
    	dst_plane = seg_off( ( src_MFDB.fww << 1 ) * src_MFDB.fh ) ;
	lb_stuff( src_MFDB.mp, mem_sz, 0 ) ; 
    	do_decode( cur_hndl, file_handle, 
			src_MFDB.fh, swidth, 
			src_inc, dst_inc, dst_plane, 
			n_planes, first_time ) ;
	first_time = FALSE ;
	sheight -= (UWORD)(lines_avail) ;
 	xyr[ 5 ] += src_MFDB.fh ;
	} while ( sheight > 0 ) ;
    dos_free( raw_fptr, 0 ) ;
    dos_free( src_MFDB.mp, 0 ) ;
    return( TRUE ) ;
} /* decode */

/* end of o_image.c */