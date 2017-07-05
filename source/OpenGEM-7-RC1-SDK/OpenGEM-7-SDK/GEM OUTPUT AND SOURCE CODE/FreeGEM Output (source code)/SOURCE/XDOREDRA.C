/*******************************************************************/
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

#include <portab.h>
#include <machine.h>
#include <obdefs.h>
#include <windlib.h>
#include <gembind.h>

EXTERN	VOID	r_set() ;		/* ooptimz.c */
EXTERN	BOOLEAN	rc_intersect() ;	/* ooptimz.c */

	WORD
do_redraw(tree, w_handle, x, y, w, h )
	LONG		tree ;
	WORD		w_handle;
	WORD		x, y, w, h ;
{
	GRECT		rc, r ;

	wind_update(TRUE);
	graf_mouse( 256, 0x0L);
	r_set( &rc, x, y, w, h ) ;
	wind_get(w_handle, WF_FIRSTXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	while ( r.g_w && r.g_h )
	{
	  if ( rc_intersect( &rc, &r ) )
	  {
	      objc_draw( tree, 0, MAX_DEPTH, r.g_x, r.g_y, r.g_w, r.g_h);
	  }
	  wind_get(w_handle, WF_NEXTXYWH, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	}
	graf_mouse( 257, 0x0L);
	wind_update(FALSE);
	return( TRUE ) ;
}