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

/*	VDIBIND.C	7/18/84 - 4/19/85	John Grant		*/

#include "portab.h"
extern WORD contrl[], intin[], ptsin[] ;
extern VOID vdi() ;


    WORD
v_xbit_image( handle, filename, aspect, x_scale, y_scale, h_align, v_align, 
			rotate, background, foreground, xy )
WORD handle, aspect, x_scale, y_scale, h_align, v_align;
WORD rotate, background, foreground;
WORD xy[];
BYTE *filename;
{
    WORD ii;

    for (ii = 0; ii < 4; ii++)
	ptsin[ ii ] = xy[ ii ] ;
    intin[0] = aspect;
    intin[1] = x_scale;
    intin[2] = y_scale;
    intin[3] = h_align;
    intin[4] = v_align;
    intin[5] = rotate ;
    intin[6] = background ;
    intin[7] = foreground ;
    ii = 8 ;
    while ( *filename )
    	{
	    intin[ ii++ ] = (WORD)( *filename++ ) ;
	}
    intin[ ii ] = 0 ;		/* final null */
    contrl[0] = 5;
    contrl[1] = 2;
    contrl[3] = ii ;
    contrl[5] = 101 ;
    contrl[6] = handle ; 
    vdi();
    return( TRUE ) ;
}
