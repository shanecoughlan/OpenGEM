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

/* custom dos functions for 8086 land.					*/

#include "portab.h"
#include "machine.h"
#include "dosbind.h"

#if MC68K

EXTERN VOID	swpbyte();

/*----------------------------------------------------------------------*/
    VOID
LSTRCPY( a, b )
LONG	a, b ;
{
    LSTCPY( b, a ) ;
} /* LSTRCPY */


    UWORD
mf_read( handle, cnt, pbuffer )
    WORD handle ;
    UWORD cnt ;
    LONG pbuffer ;
{
UWORD ret ;

    ret = dos_read( handle, cnt, pbuffer ) ;
    swpbyte( pbuffer, (LONG)(cnt) ) ; 
    return( ret ) ;
} /* mf_read */


	WORD
dos_find(pspec)
	LONG	pspec;
{
BYTE    dta[ DTA_LEN ] ;

	dos_sdta(ADDR(&dta[0]));
	DOS_ERR = FALSE;
	return( dos_sfirst(pspec, F_RDONLY | F_SYSTEM) );
} /* dos_find */

#endif	/* MC68K */


/*----------------------------------------------------------------------*/
    LONG
xdos_alloc( nbytes ) 
LONG    nbytes ;
{
    return( dos_alloc( (UWORD)( nbytes ) ) ) ;
} /* dos_alloc */


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
    if (DOS_ERR)
	return ( 0x00L ) ;
    size = ((LONG)(dta[26])      ) + ((LONG)(dta[27]) << 8 ) +
	   ((LONG)(dta[28]) << 16) + ((LONG)(dta[29]) << 24) ;
#if I8086
	dos_sdta( ADDR( &_dta[ 0 ] ) ) ;
#endif
    return( size ) ;
} /* file_size */


/* end of crdos.c */