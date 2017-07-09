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

extern WORD contrl[], intin[], intout[] ;

extern VOID vdi() ;

/*--------------------------------------------------------------*/
    VOID
_vq_scan( handle, g_height, g_slice, a_height, a_slice, factor, a_width, a_hslices )
WORD handle, *g_height, *g_slice, *a_height, *a_slice, *factor, *a_width, *a_hslices ;
{
    vq_scan( handle, g_height, g_slice, a_height, a_slice, factor ) ;
    *a_width = ( dev.xdpi * (*factor) ) / 10 ;	/* figure 10 chars per inch */
    *a_hslices = ( dvc_nxpixel + 1 ) / (*a_width) ;
}

/* end of vqscan.c */
