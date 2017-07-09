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

extern WORD contrl[], intin[], intout[], ptsout[] ;

extern VOID vdi() ;
extern VOID i_intin() ;
extern VOID i_intout() ;
extern VOID i_ptsout() ;


    WORD
v_opnwk( work_in, handle, work_out )
WORD work_in[], *handle, work_out[];
{
        i_intin( work_in );
        i_intout( work_out );
        i_ptsout( work_out + 45 );

        contrl[0] = 1;
        contrl[1] = 0;
        contrl[3] = 103 ;
        vdi();

        *handle = contrl[6];    

        i_intin( intin );
        i_intout( intout );
        i_ptsout( ptsout );
	return( TRUE ) ;
}

