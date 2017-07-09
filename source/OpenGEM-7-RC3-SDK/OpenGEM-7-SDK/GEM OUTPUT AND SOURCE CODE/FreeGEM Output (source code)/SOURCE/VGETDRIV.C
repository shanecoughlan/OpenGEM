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

extern WORD contrl[], intin[], intout[] ;

extern VOID vdi() ;

/*----------------------------------------------------------------------*/
    VOID
v_get_driver_info( device_id, info_select, info_string )
WORD	device_id, info_select ;
BYTE	*info_string ;
{
    WORD	ii ;
    BYTE	*bptr ;
    
    contrl[ 0 ] = -1 ;
    contrl[ 1 ] = 0 ;
    contrl[ 3 ] = 2 ;
    contrl[ 5 ] = 4 ;
    contrl[ 6 ] = 0 ;
    intin[ 0 ] = device_id ;
    intin[ 1 ] = info_select ;
    vdi() ;
    if ( info_select != 5 )
        {
	bptr = (BYTE *) intout ;
	for ( ii = 0; ii < contrl[ 4 ]; ii ++ )
	    *info_string++ = *bptr++ ;
	*info_string = 0 ;
	}
    else
        *(WORD *)info_string = intout[ 0 ] ;
} /* v_get_driver_info() */

/* end of vgetdriv.c */
