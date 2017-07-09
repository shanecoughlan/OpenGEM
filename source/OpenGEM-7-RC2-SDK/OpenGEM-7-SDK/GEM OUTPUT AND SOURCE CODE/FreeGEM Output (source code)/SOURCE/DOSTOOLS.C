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

#include "PORTAB.H"
#include "MACHINE.H"
#include "OBDEFS.H"

#include "gemcmds.h"
#include "gembind.h"
#include "dosbind.h"
#include "vdibind.h"

#include "draw.h"
#include "constdef.h"
#include "typedefs.h"
#include "extnvars.h"
						/* in DOSIF.A86		*/
EXTERN WORD	__DOS();

EXTERN UWORD	DOS_AX;
EXTERN UWORD	DOS_BX;
EXTERN UWORD	DOS_ERR ;

#if MC68K
EXTERN VOID	swpbyte() ;
#endif

/*----------------------------------------------------------------------*/
/* remove path from filename, return filename minus disk and dir.	*/
    VOID
del_path( name, name_only )
    BYTE	*name, *name_only ;
{
    BYTE	*cptr ;

    cptr = &name[ strlen( name ) ] ;
    while ( ( *cptr != BSLASH ) && ( *cptr != COLON ) )
        cptr-- ;
    cptr++ ;
    strcpy( cptr, name_only ) ;
} /* del_path() */

/*----------------------------------------------------------------------*/
/* remove filename from path, return disk and dir minus filename. 	*/
    VOID
del_fname( name, path_only )
    BYTE	*name, *path_only ;
{
    BYTE	*cptr ;

    strcpy( name, path_only ) ;
    cptr = &path_only[ strlen( path_only ) ] ;
    while ( ( *cptr != BSLASH ) && ( *cptr != COLON ) )
        cptr-- ;		/* points to last backslash in path name */
    ++cptr ;
    *cptr = NULL ;
} /* del_fname */

/*----------------------------------------------------------------------*/
/* remove extension from file name descriptor 				*/
    VOID
del_extension( full_name, name_only )
    BYTE	*full_name, *name_only ;
{
    BYTE	*cptr ;

    if ( full_name != name_only )
    	strcpy( full_name, name_only ) ;
    cptr = name_only ;
    while ( ( *cptr != PERIOD ) && ( *cptr ) )
	cptr++ ;	/* pointer now points to null or period at end */
    *cptr = NULL ;
} /* del_extension */

/*--------------------------------------------------------------*/
	BOOLEAN
d_read( file, size, buffer )
	BYTE *file, *buffer ;
	LONG size;
{
WORD    f_hndl ;
LONG	r_size ;

	f_hndl = dos_open( ADDR( file ), 0 );
	if ( !DOS_ERR )
	{
	    r_size = dos_read( f_hndl, size, ADDR( buffer ) );
	    if ( DOS_ERR )
		return( FALSE );
	    dos_close( f_hndl );
	    return( r_size == size );
	}
	return( FALSE );
} /* d_read */


/*--------------------------------------------------------------*/
	BOOLEAN
d_write( file, size, buffer )
	BYTE *file, *buffer;
	LONG size;
{
	WORD f_hndl ;
	LONG	r_size ;

	if ( dos_sfirst( file, 0x0 ) )
	{
	    /* Alert */
	}
	f_hndl = dos_create( file, 0 ) ;	
	if ( f_hndl )
	{
	    r_size = dos_write( f_hndl, size, ADDR( buffer ) );
	    dos_close( f_hndl );
	    return( r_size == size );
	}
	return( FALSE );
} /* d_write */

/*----------------------------------------------------------------------*/
    LONG
mf_read( handle, cnt, pbuffer )
    WORD	handle ;
    LONG	cnt ;
    BYTE	*pbuffer ;
{
LONG	ret ;

    ret = dos_read( handle, cnt, pbuffer ) ;
#if MC68K
    swpbyte( pbuffer, cnt ) ; 
#endif
    return( ret ) ;
} /* mf_read */

/*----------------------------------------------------------------------*/
/* get file size with DOS FIND FIRST call.				*/
    LONG
file_size( name )
    BYTE	*name ;
{
    BYTE	dta[ DTA_LEN ] ;
    LONG	size ;

    dos_sdta( ADDR( dta ) ) ;
    dos_sfirst( ADDR( name ), 0 ) ;
    if ( DOS_ERR )
	return ( 0x00L ) ;
    size = ( ( LONG )( dta[26] )      ) + ( ( LONG )( dta[27] ) << 8 ) +
	   ( ( LONG )( dta[28] ) << 16 ) + ( ( LONG )( dta[29] ) << 24 ) ;
    return( size ) ;
} /* file_size */

/*----------------------------------------------------------------------*/
/* S1 holds current drive and directory in the form "C:\GEMAPPS or	*/
/* "C:".  S2 holds the path to verify in the form "C:\PICTURES"		*/
    BOOLEAN
dir_exists( path )
BYTE	*path ;
{
BOOLEAN	exists ;

    del_fname( path, S2 ) ;
    strcat( "*.*", S2 ) ;
   exists = dos_sfirst( ADDR( S2 ), 0x0 ) ;
   *S2 = NULL ;
   return( exists ) ;
} /* dir_exists */

/* end of dostools.c */
