/*************************************************************************
**       Copyright 1999, Caldera Thin Clients, Inc.                     ** 
**       This software is licenced under the GNU Public License.        ** 
**       Please see LICENSE.TXT for further information.                ** 
**                                                                      ** 
**                  Historical Copyright                                ** 
**									**
**									**
**  Copyright (c) 1987, Digital Research, Inc. All Rights Reserved.	**
**  The Software Code contained in this listing is proprietary to	**
**  Digital Research, Inc., Monterey, California and is covered by U.S.	**
**  and other copyright protection.  Unauthorized copying, adaptation,	**
**  distribution, use or display is prohibited and may be subject to 	**
**  civil and criminal penalties.  Disclosure to others is prohibited.	**
**  For the terms and conditions of software code use refer to the 	**
**  appropriate Digital Research License Agreement.			**
**									**
*************************************************************************/

#define	FHEAD struct font_head

/* Bit flag definitions for the primary flags word. */
#define	DEFAULT		0x0001
#define	HORZ_OFF	0x0002
#define	STDFORM		0x0004
#define MONOSPACE	0x0008
#define	UNAVAILABLE	0x0010
#define	DEVICE_FONT	0x4000
#define	MAPPED		0x8000

/* Bit flag definitions for the device flags word. */
#define	NEED_DOWNLOAD	0x0100
#define	DOWNLOADED	0x0200
#define FLOAT_ACCENT	0x0400
#define	PERMANENT	0x0800
#define	PORTRAIT	0x8000

/* Bit flag definitions for the font special effects. */
#define	THICKEN		0x1
#define	LIGHT		0x2
#define	SKEW		0x4
#define	UNDER		0x8
#define	OUTLINE 	0x10
#define	SHADOW		0x20

FHEAD {
	UWORD	font_id;
	WORD	point;
	BYTE	name[32];
	UWORD	first_ade;
	UWORD	last_ade;
	UWORD	top;
	UWORD	ascent;
	UWORD	half;
	UWORD	descent;		
	UWORD	bottom;
	UWORD	max_char_width;
	UWORD	max_cell_width;
	UWORD	left_offset;
	UWORD	right_offset;
	UWORD	thicken;
	UWORD	ul_size;
	UWORD	lighten;
	UWORD	skew;
	UWORD	flags;			
	UBYTE	*hor_table;
	UWORD	*off_table;
	UWORD	*dat_table;
	UWORD	form_width;
	UWORD	form_height;
	FHEAD	*next_font;
	FHEAD	*next_sect;
	WORD	lfu_low;
	WORD	lfu_high;
	WORD	*file_off;
	WORD	data_size;
	WORD	reserve[5];
	BYTE	*map_addr;
	UWORD	dev_flags;
	BYTE	esc_seq[32];
};
