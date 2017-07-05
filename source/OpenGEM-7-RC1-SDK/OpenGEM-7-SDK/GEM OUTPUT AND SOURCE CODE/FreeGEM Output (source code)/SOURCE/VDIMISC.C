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

#include "modelc.h"
#include "portab.h"
#include "machine.h"


EXTERN 	WORD	contrl[] ;
EXTERN	WORD	pblock[] ;

    VOID
i_ptsin( p )
WORD	*p ;
{
    pblock[ 4 ] = LLOWD( ADDR( p ) ) ;
    pblock[ 5 ] = LHIWD( ADDR( p ) ) ;
} 

    VOID
i_ptsout( p )
WORD	*p ;
{
    pblock[ 8 ] = LLOWD( ADDR( p ) ) ;
    pblock[ 9 ] = LHIWD( ADDR( p ) ) ;
} 

    VOID
i_intin( ii )
WORD	*ii ;
{
    pblock[ 2 ] = LLOWD( ADDR( ii ) ) ;
    pblock[ 3 ] = LHIWD( ADDR( ii ) ) ;
} 

    VOID
i_intout( ii )
WORD	*ii ;
{
    pblock[ 6 ] = LLOWD( ADDR( ii ) ) ;
    pblock[ 7 ] = LHIWD( ADDR( ii ) ) ;
} 

    VOID
i_ptr( address )
LONG	address ;
{
    contrl[ 7 ] = LLOWD( address ) ;
    contrl[ 8 ] = LHIWD( address ) ;
}

    VOID
i_ptr2( address )
LONG	address ;
{
    contrl[ 9 ] = LLOWD( address ) ;
    contrl[ 10 ] = LHIWD( address ) ;
}

    VOID
i_lptr1( address )
LONG	address ;
{
    i_ptr( address ) ;
}

    VOID
m_lptr2( ptr )
LONG	*ptr ;
{
    *ptr = ((LONG)( contrl[ 10 ] ) << 16) | (LONG)( contrl[ 9 ] ) ;    	
} 

/* end of vdimisc.c */