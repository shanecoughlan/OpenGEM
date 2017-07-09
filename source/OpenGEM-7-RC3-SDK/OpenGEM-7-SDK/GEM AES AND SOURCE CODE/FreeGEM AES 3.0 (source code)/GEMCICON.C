/****************************************************************************
*   Copyright 1999, Caldera Thin Client Systems, Inc.                       *
*   This software is licensed under the GNU Public License                  *
*   For further information, please see LICENSE.TXT                         *
*                                                                           *
*   Historical Copyright                                                    *
*                                                                           *
*   Copyright (c) 1992  Digital Research Inc.				    *
*   All rights reserved.						    *
*   The Software Code contained in this listing is proprietary to Digital   *
*   Research Inc., Monterey, California, and is covered by U.S. and other   *
*   copyright protection.  Unauthorized copying, adaption, distribution,    *
*   use or display is prohibited and may be subject to civil and criminal   *
*   penalties.  Disclosure to others is prohibited.  For the terms and      *
*   conditions of software code use, refer to the appropriate Digital       *
*   Research License Agreement.						    *
*****************************************************************************
*		      U.S. GOVERNMENT RESTRICTED RIGHTS			    *
*                    ---------------------------------                      *
*  This software product is provided with RESTRICTED RIGHTS.  Use, 	    *
*  duplication or disclosure by the Government is subject to restrictions   *
*  as set forth in FAR 52.227-19 (c) (2) (June, 1987) when applicable or    *
*  the applicable provisions of the DOD FAR supplement 252.227-7013 	    *
*  subdivision (b)(3)(ii) (May 1981) or subdivision (c)(1)(ii) (May 1987).  *
*  Contractor/manufacturer is Digital Research Inc. / 70 Garden Court /     *
*  BOX DRI / Monterey, CA 93940.					    *
*****************************************************************************
* $Header: m:/davinci/users//groups/panther/aes/rcs/gemcicon.c 4.3 92/03/26 14:48:02 sbc Exp $
* $Log:	gemcicon.c $
 * Revision 4.3  92/03/26  14:48:02  sbc
 * Merge in RSF's changes
 * 
 * Revision 4.3  92/03/20  20:16:49  Fontes
 * Removed bmp file processing from AES
 * 
 * Revision 4.2  92/02/27  15:24:00  rsf
 * Conversion to medium model. Replace GEM.H with VIEWRUN.H
 * 
 * Revision 4.2  92/02/11  11:32:02  Fontes
 * Background image display implementation
 * 
 * Revision 4.1  92/01/03  13:15:31  Fontes
 * Susan's cleanup
 * 
*/

/****************************************************************************
* File:		gemcicon.c
*
* Description:	
*
* Build Info:	ndmake -f aes.mak
*
*****************************************************************************/


#include "aes.h"

extern	WORD	DOS_ERR;
extern	WS	gl_ws;
extern	WORD	contrl[];
extern	WORD	ptsin[];

#define	BITSperBYTE	8
#define BITSperWORD	16



VOID	render_bmp(FDB FAR* fdb, WORD x, WORD y)
{
	FDB	screen;
	
	gsx_fix(&screen, 0L, 0, 0);

	/* Transform if necessary */
	if (fdb->fd_stand)
	{
		*(LONG*)&contrl[7] = (LONG)fdb;
		*(LONG*)&contrl[9] = (LONG)fdb;
		gsx_ncode(TRANSFORM_FORM, 0, 0);
	}

	/* send to screen */

	*(LONG*)&contrl[7] = (LONG)fdb;
	*(LONG*)&contrl[9] = (LONG)(FDB far *)(&screen);

	ptsin[0] = 0;
	ptsin[1] = 0;
	ptsin[2] = fdb->fd_w-1;
	ptsin[3] = fdb->fd_h-1;
	ptsin[4] = x;
	ptsin[5] = y;
	ptsin[6] = x + fdb->fd_w - 1;
	ptsin[7] = y + fdb->fd_h - 1;
	if (gl_ws.ws_color && fdb->fd_nplanes == 1)
	{
		/* If color device but mono icon */
		intin[0] = MD_REPLACE;
		intin[1] = BLACK;
		intin[2] = WHITE;
		gsx_ncode(TRAN_RASTER_FORM, 4, 3);
		
	}
	else
	{
		intin[0] = 3;
		gsx_ncode(COPY_RASTER_FORM, 4, 1);
	}

}



VOID	render_ico(FDB FAR *mask, FDB FAR *data, WORD x, WORD y, WORD selected)
{
	FDB	screen;
	
	gsx_fix(&screen, 0L, 0, 0);

	
	/* First, put out the mask */
	if (mask->fd_stand)
	{
		*(LONG*)&contrl[7] = (LONG)mask;
		*(LONG*)&contrl[9] = (LONG)mask;
		gsx_ncode(TRANSFORM_FORM, 0, 0);
	}

	*(LONG*)&contrl[7] = (LONG)mask;
	*(LONG*)&contrl[9] = (LONG)(FDB far *)&screen;

	ptsin[0] = 0;
	ptsin[1] = 0;
	ptsin[2] = mask->fd_w-1;
	ptsin[3] = mask->fd_h-1;
	ptsin[4] = x;
	ptsin[5] = y;
	ptsin[6] = x + mask->fd_w - 1;
	ptsin[7] = y + mask->fd_h - 1;

	if (gl_ws.ws_color && mask->fd_nplanes == 1)
	{
		/* If color device but mono icon */
		intin[0] = MD_REPLACE;
		intin[1] = BLACK;
		intin[2] = WHITE;
		gsx_ncode(TRAN_RASTER_FORM, 4, 3);
	}
	else
	{
		intin[0] = MD_XOR;	
		gsx_ncode(COPY_RASTER_FORM, 4, 1);
	}

	
	/* Now proceed to the image */
	if (data->fd_stand)
	{
		*(LONG*)&contrl[7] = (LONG)data;
		*(LONG*)&contrl[9] = (LONG)data;
		gsx_ncode(TRANSFORM_FORM, 0, 0);
	}

	/* send to screen */

	*(LONG*)&contrl[7] = (LONG)data;
	*(LONG*)&contrl[9] = (LONG)(FDB far *)(&screen) ;

	ptsin[0] = 0;
	ptsin[1] = 0;
	ptsin[2] = data->fd_w-1;
	ptsin[3] = data->fd_h-1;
	ptsin[4] = x;
	ptsin[5] = y;
	ptsin[6] = x + data->fd_w - 1;
	ptsin[7] = y + data->fd_h - 1;
	if (gl_ws.ws_color && data->fd_nplanes == 1)
	{
		/* If color device but mono icon */
		intin[0] = MD_REPLACE;
		intin[1] = BLACK;
		intin[2] = WHITE;
		gsx_ncode(TRAN_RASTER_FORM, 4, 3);
		
	}
	else
	{
		intin[0] = 6;		/* XOR */
		gsx_ncode(COPY_RASTER_FORM, 4, 1);
	}

    /* [JCE 8-8-1999] Highlight a selected colour icon by 
     *                shading it - Panther did not
     *                highlight colour icons at all.
     *
     * This actually shades the whole rectangle, when it 
     * should just shade the icon. 
     *
     */
	if (selected)
	{
		
	   	vsf_color(gl_ws.ws_color ? BLUE : BLACK);
	   	bb_fill(MD_TRANS, FIS_PATTERN, IP_4PATT, x, y,
		mask->fd_w, mask->fd_h );	


		/* If the mask is mono, we can cut out the surplus shading */
		if (mask->fd_nplanes == 1)
		{
			*(LONG*)&contrl[7] = (LONG)mask;
			*(LONG*)&contrl[9] = (LONG)(FDB far *)&screen;

			ptsin[0] = 0;
			ptsin[1] = 0;
			ptsin[2] = mask->fd_w-1;
			ptsin[3] = mask->fd_h-1;
			ptsin[4] = x;
			ptsin[5] = y;
			ptsin[6] = x + mask->fd_w - 1;
			ptsin[7] = y + mask->fd_h - 1;
	
			intin[0] = MD_TRANS;
			intin[1] = WHITE;
			intin[2] = BLACK;
			gsx_ncode(TRAN_RASTER_FORM, 4, 3);	
		}
	}


}
