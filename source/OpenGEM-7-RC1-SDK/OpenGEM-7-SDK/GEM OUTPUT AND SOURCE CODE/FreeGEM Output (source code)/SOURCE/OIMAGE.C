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

/* Display bit image files : Susan Bancroft  5-30-85 */

#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "gembind.h"
#include "dosbind.h"
#include "xgembind.h"

#include "output.h"
#include "odefs.h"
#include "ovar.h"

EXTERN LONG  	file_size() ;		/* oportabl.c */
EXTERN BOOLEAN	decode() ;		/* oimagutl.c */


/*-----------------------------------------------------------------*/
    BOOLEAN
OUTIMAG( name, p_xy ) 
BYTE	*name ;
WORD	*p_xy ;
{
    return( dsp_img( name, p_xy ) ) ;
} /* OUTIMAG */


/*-----------------------------------------------------------------*/
    BOOLEAN
dsp_img( name, p_xy ) 
BYTE	*name ;
WORD	*p_xy ;
{
WORD	f_hndl ;
BOOLEAN ok ;

    if ( file_size( name ) == 0x0L )
	return( FALSE ) ;
    f_hndl = dos_open( ADDR( name ), 0 ) ;
    if ( DOS_ERR )
	return( FALSE ) ;
    ok = decode( cur_hndl, f_hndl, dev.nplanes, p_xy ) ;
    if ( ! ok )
	{
	/* alert: memory allocation error. Probably not enough */
	} 
    dos_close( f_hndl ) ;
    return( ok ) ;
} /* dsp_img */


/* end of oimagutl.c */