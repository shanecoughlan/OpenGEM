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

/*----------------------------------------------------------------------*/
/*	VDITOOLS.C : misc utilities for vdi.				*/
/*----------------------------------------------------------------------*/

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
#include "vdicmds.h"

/*----------------------------------------------------------------------*/
    VOID
i_ptsin( p )
WORD	*p ;
{
    pblock.p_ptsin = p ;
} 

/*----------------------------------------------------------------------*/
    VOID
i_ptsout( p )
WORD	*p ;
{
    pblock.p_ptsout = p ;
} 

/*----------------------------------------------------------------------*/
    VOID
i_intin( p )
WORD	*p ;
{
    pblock.p_intin = p ;
} 

/*----------------------------------------------------------------------*/
    VOID
i_intout( p )
WORD	*p ;
{
    pblock.p_intout = p ;
} 

/*----------------------------------------------------------------------*/
    VOID
i_ptr( address )
LONG	address ;
{
    contrl[ 7 ] = LLOWD( address ) ;
    contrl[ 8 ] = LHIWD( address ) ;
}

/*----------------------------------------------------------------------*/
    VOID
i_ptr2( address )
LONG	address ;
{
    contrl[ 9 ] = LLOWD( address ) ;
    contrl[ 10 ] = LHIWD( address ) ;
}

/*----------------------------------------------------------------------*/
    VOID
User_def_cmd( i_ct, i_ptr, p_ct, p_ptr )
WORD i_ct ;
WORD *i_ptr ;
WORD p_ct ;
WORD *p_ptr ;
{
    if ( !meta_device )
	return ;
    v_write_meta( g_handle, i_ct, i_ptr, p_ct, p_ptr ) ;
} /* User_def_cmd */

/*----------------------------------------------------------------------*/
    VOID
draw_mouse( m_form )
WORD m_form ;
{
    cur_mouse = m_form ;
    graf_mouse( m_form, 0x0L ) ;    
}  /* draw_mouse */

/*----------------------------------------------------------------------*/
    VOID
set_clip_rect( rect ) 
    RECT  	*rect ;
{
WORD	pts[ 4 ] ;

    pts[ 0 ] = rect->x ;
    pts[ 1 ] = rect->y ;
    pts[ 2 ] = rect->x + rect->w ;
    pts[ 3 ] = rect->y + rect->h ;
    vs_clip( g_handle, 1, pts ) ;
} /* set_clip_rect */

/*----------------------------------------------------------------------*/
    VOID
Hide_cursor()
{
    graf_mouse( M_OFF, 0x0L ) ;
}  /* Hide_cursor */

/*----------------------------------------------------------------------*/
    VOID
Show_cursor ()
{
    graf_mouse( M_ON, 0x0L ) ;
} /* Show_cursor */

/*----------------------------------------------------------------------*/
    VOID
get_area_att( fill_number, style, index ) 
    WORD  	fill_number ;
    WORD  	*style, *index ;
{
    if ( fill_number == 0 )
	{
	*style = HOLLOW ; 
        *index = 0 ;
	return ;
	}
    if ( fill_number == ( WHITITEM - NOFLITEM ) ) 
	{
	*style = SOLID ; 
        *index = 1 ;
	return ;
	}
    if ( ( fill_number >= ( GR01ITEM - NOFLITEM ) ) &&
		( fill_number <= ( PN16ITEM - NOFLITEM ) ) )
	{
	*style = PATTERN ; 
        *index = fill_number - ( GR01ITEM - WHITITEM ) ;
	return ;
        }
    if ( ( fill_number >= ( HT01ITEM - NOFLITEM ) ) &&
		( fill_number <= ( HT12ITEM - NOFLITEM ) ) )
	{
	*style = HATCH ; 
        *index = fill_number - ( HT01ITEM - WHITITEM ) ;
	return ;
        } 
} /* get_area_att */

/*-----------------------------------------------------------------*/
    VOID
Cvt_rect( rect  ) 
    RECT  	*rect  ;
{
    ptsin[ 6 ] = ptsin[ 8 ] = ptsin[ 0 ] = rect->x ;
    ptsin[ 3 ] = ptsin[ 9 ] = ptsin[ 1 ] = rect->y ;
    ptsin[ 4 ] = ptsin[ 2 ] = rect->x + rect->w ;
    ptsin[ 5 ] = ptsin[ 7 ] = rect->y + rect->h ;
} /* Cvt_rctangle */

/*----------------------------------------------------------------------*/
    VOID
Int_rect( rect ) 
    RECT  	*rect  ;
{
    Cvt_rect( rect ) ;
    v_pline( g_handle, 5, ptsin ) ;
} /* Int_rect */

/*----------------------------------------------------------------------*/
    VOID
Bar( rect  ) 
    RECT  	*rect  ;
{
    ptsin[ 0 ] = rect->x ;
    ptsin[ 1 ] = rect->y ;
    ptsin[ 2 ] = rect->x + rect->w ;
    ptsin[ 3 ] = rect->y + rect->h ;
    v_bar( g_handle, ptsin ) ;
} /* Bar */

/*----------------------------------------------------------------------*/
    VOID
Fill_rect( rect  ) 
RECT  	*rect  ;
{
WORD	pts[ 4 ] ;

    pts[ 0 ] = rect->x ;
    pts[ 1 ] = rect->y ;
    pts[ 2 ] = rect->x + rect->w ;
    pts[ 3 ] = rect->y + rect->h ;
    vr_recfl( g_handle, pts ) ;
} /* Fill_rect */

/*----------------------------------------------------------------------*/
    VOID
Round_bar( rect, id ) 
    RECT  	*rect  ;
    WORD  	id  ;
{
    ptsin[ 0 ] = rect->x ;
    ptsin[ 1 ] = rect->y ;
    ptsin[ 2 ] = rect->x + rect->w ;
    ptsin[ 3 ] = rect->y + rect->h ;
    if ( id == LINRRECTCMD )
	v_rbox( g_handle, ptsin ) ;
    else
	v_rfbox( g_handle, ptsin ) ;
} /* Round_bar */

/*----------------------------------------------------------------------*/
    VOID
set_hw_attrib()
{
    set_font( 1 ) ;
    set_align( 0, 0 ) ;
    vst_rotation( g_handle, 0 ) ;
    set_t_colr( BLACK ) ;
    set_sp_eff( 0 ) ;
} /* set_hw_attrib */

/*----------------------------------------------------------------------*/
    VOID
close_gsx()
{
    vst_unload_fonts( g_handle, 0 ) ;
    v_clswk( g_handle ) ;
} /* close_gsx */

/*----------------------------------------------------------------------*/
    BOOLEAN
ld_mfdriver()
{
    WORD	i,
		work_in[ 11 ],
		work_out[ 57 ] ;

    work_in[ 0 ] = 31 ;
    for ( i = 1 ; i < 10 ; i++ )
	work_in[ i ] = 1 ;
    work_in[ 10 ] = 2 ;		/* set rc system */
    v_opnwk( work_in, &meta_handle, work_out ) ;
    if (!meta_handle)
    {	
	alert_box( NOMFDRVR ) ;     /* couldn't load metafile driver */
	return( FALSE ) ;
    }
    return( TRUE ) ;
} /* ld_mfdriver */

/*----------------------------------------------------------------------*/
    VOID
get_xy( x, y  ) 
    WORD  	*x, *y  ;
{
    WORD	junk ;

    graf_mkstate( x, y, &junk, &junk ) ;
} /* get_xy */

/* end of vditools.c */

