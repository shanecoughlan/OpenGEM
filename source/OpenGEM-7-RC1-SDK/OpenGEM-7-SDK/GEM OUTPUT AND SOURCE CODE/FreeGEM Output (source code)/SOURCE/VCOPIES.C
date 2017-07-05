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

extern WORD contrl[], intin[] ;

extern VOID vdi() ;

/*----------------------------------------------------------------------*/
    VOID
v_copies( handle, copies )
WORD	handle, copies ;
{
    contrl[ 0 ] = 5 ;
    contrl[ 1 ] = 0 ;
    contrl[ 3 ] = 1 ;
    contrl[ 5 ] = 28 ;
    contrl[ 6 ] = handle ;
    intin[ 0 ] = copies ;
    vdi() ;
} /* v_copies() */

/* end of vcopies.c */
