/************************************************************************

    EDICON 2.00 - icon editor for GEM

    Copyright (C) 1998,1999,2000  John Elliott <jce@seasip.demon.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*************************************************************************/

typedef struct         /* Windows 3 icon file header */
{
	int ih_magic;      /* Magic number = 0 */
	int ih_typ;        /* Type         = 1 */
	int ih_icno;       /* No. of icons     */
} WIN31IH;

typedef struct		   /* Windows 3 icon record */
{
	char ir_width;     /* Width, pixels    */
	char ir_height;    /* Height, pixels   */
	char ir_cdepth;    /* Colour depth = 2 */
	char ir_spare[5];  /* Zeroes           */
	long ir_len;       /* Length of bitmap + mask + palette + bmp header */
	long ir_offset;	   /* Offset to picture data */ 
} WIN31IR;

typedef struct			/* Windows 3 bitmap record */
{
	long bm_srecl;     /* Icon BMP header - size */
	long bm_width;     /* Width */
	long bm_height;    /* 2*height */
	int  bm_nplanes;   /* No. of planes */
	int  bm_cdepth;    /* Colour depth */
	long bm_compr;     /* Compression (none) */
	long bm_dsize;     /* Size in bytes of icon */
	long bm_xres;      /* X resolution */
	long bm_yres;      /* Y resolution */
	long bm_usedc;     /* Colours used */
	long bm_imptc;     /* Important colours */
	unsigned char bm_pal[8];    /* Palette */
} WIN31BM;


typedef struct			/* Windows 3 bitmap record */
{
	long bm_srecl;     /* Icon BMP header - size */
	long bm_width;     /* Width */
	long bm_height;    /* 2*height */
	int  bm_nplanes;   /* No. of planes */
	int  bm_cdepth;    /* Colour depth */
	long bm_compr;     /* Compression (none) */
	long bm_dsize;     /* Size in bytes of icon */
	long bm_xres;      /* X resolution */
	long bm_yres;      /* Y resolution */
	long bm_usedc;     /* Colours used */
	long bm_imptc;     /* Important colours */
	unsigned char bm_pal[64];   /* Palette */
} WIN31BM16;

