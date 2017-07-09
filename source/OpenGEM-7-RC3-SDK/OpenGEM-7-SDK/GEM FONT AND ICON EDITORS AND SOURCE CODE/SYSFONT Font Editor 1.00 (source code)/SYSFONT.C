/************************************************************************

    SYSFONT 1.00 - Edit GEM system fonts

    Copyright (C) 2002  John Elliott <jce@seasip.demon.co.uk>

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
#define INSTANTIATE
#include "sysfonti.h"

#define SAME   0x0000
#define NOCHAR 0x2020

word dos_to_gem[256] = { NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  0 -  3 */
			 NOCHAR, NOCHAR, NOCHAR, 0x2004, /*  4 -  7 */	 
			 NOCHAR, NOCHAR, NOCHAR, 0x200D, /*  8 -  B */	 
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  C -  F */	 
			 SAME,   SAME,   NOCHAR, 0x2016, /* 10 - 13 */
			 SAME,   SAME,   0x2012, SAME,   /* 14 - 17 */
			 SAME,   SAME,   SAME,   SAME,   /* 18 - 1B */
			 SAME,   SAME,   SAME,   SAME,   /* 1C - 1F */
			 SAME,   SAME,   SAME,   SAME,   /* 20 - 23 */
			 SAME,   SAME,   SAME,   SAME,   /* 24 - 27 */
			 SAME,   SAME,   SAME,   SAME,   /* 28 - 2B */
			 SAME,   SAME,   SAME,   SAME,   /* 2C - 2F */
			 SAME,   SAME,   SAME,   SAME,   /* 30 - 33 */
			 SAME,   SAME,   SAME,   SAME,   /* 34 - 37 */
			 SAME,   SAME,   SAME,   SAME,   /* 38 - 3B */
			 SAME,   SAME,   SAME,   SAME,   /* 3C - 3F */
			 SAME,   SAME,   SAME,   SAME,   /* 40 - 43 */
			 SAME,   SAME,   SAME,   SAME,   /* 44 - 47 */
			 SAME,   SAME,   SAME,   SAME,   /* 48 - 4B */
			 SAME,   SAME,   SAME,   SAME,   /* 4C - 4F */
			 SAME,   SAME,   SAME,   SAME,   /* 50 - 53 */
			 SAME,   SAME,   SAME,   SAME,   /* 54 - 57 */
			 SAME,   SAME,   SAME,   SAME,   /* 58 - 5B */
			 SAME,   SAME,   SAME,   SAME,   /* 5C - 5F */
			 SAME,   SAME,   SAME,   SAME,   /* 60 - 63 */
			 SAME,   SAME,   SAME,   SAME,   /* 64 - 67 */
			 SAME,   SAME,   SAME,   SAME,   /* 68 - 6B */
			 SAME,   SAME,   SAME,   SAME,   /* 6C - 6F */
			 SAME,   SAME,   SAME,   SAME,   /* 70 - 73 */
			 SAME,   SAME,   SAME,   SAME,   /* 74 - 77 */
			 SAME,   SAME,   SAME,   SAME,   /* 78 - 7B */
			 SAME,   SAME,   SAME,   SAME,   /* 7C - 7F */
			 SAME,   SAME,   SAME,   SAME,   /* 80 - 83 */
			 SAME,   SAME,   SAME,   SAME,   /* 84 - 87 */
			 SAME,   SAME,   SAME,   SAME,   /* 88 - 8B */
			 SAME,   SAME,   SAME,   SAME,   /* 8C - 8F */
			 SAME,   SAME,   SAME,   SAME,   /* 90 - 93 */
			 SAME,   SAME,   SAME,   SAME,   /* 94 - 97 */
			 SAME,   SAME,   SAME,   SAME,   /* 98 - 9B */
			 SAME,   SAME,   SAME,   SAME,   /* 9C - 9F */
			 SAME,   SAME,   SAME,   SAME,   /* A0 - A3 */
			 SAME,   SAME,   SAME,   SAME,   /* A4 - A7 */
			 SAME,   NOCHAR, NOCHAR, 0x203C, /* A8 - AB */
			 0x203E, SAME,   SAME,   SAME,   /* AC - AF */
			 0x7E61, 0x7E6F, 0x209D, 0x209B, /* B0 - B3 */
			 NOCHAR, NOCHAR, 0x6041, 0x7E41, /* B4 - B7 */
			 0x7E4F, 0x2015, NOCHAR, NOCHAR, /* B8 - BB */
			 0x2014, NOCHAR, NOCHAR, NOCHAR, /* BC - BF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* C0 - C3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* C4 - C7 */
			 0x2741, 0x5E41, 0x6045, 0x5E45, /* C8 - CB */
			 NOCHAR, 0x6049, 0x2749, 0x5E49, /* CC - CF */
			 0x604F, 0x274F, 0x5E4F, NOCHAR, /* D0 - D3 */
			 NOCHAR, 0x6055, 0x2755, 0x5E55, /* D4 - D7 */
			 NOCHAR, 0x20E1, NOCHAR, NOCHAR, /* D8 - DB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* DC - DF */
			 SAME,   SAME,   SAME,   SAME,   /* E0 - E3 */
			 SAME,   SAME,   SAME,   SAME,   /* E4 - E7 */
			 SAME,   SAME,   SAME,   SAME,   /* E8 - EB */
			 SAME,   SAME,   SAME,   SAME,   /* EC - EF */
			 SAME,   SAME,   SAME,   SAME,   /* F0 - F3 */
			 SAME,   SAME,   SAME,   SAME,   /* F4 - F7 */
			 SAME,   SAME,   SAME,   SAME,   /* F8 - FB */
			 SAME,   SAME,   SAME,   SAME,   /* FC - FF */
}; 
word iso_to_gem[256] = { NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  0 -  3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  4 -  7 */	 
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  8 -  B */	 
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  C -  F */	 
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 10 - 13 */
			 0x20B6, 0x20A7, NOCHAR, NOCHAR, /* 14 - 17 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 18 - 1B */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 1C - 1F */
			 SAME,   SAME,   SAME,   SAME,   /* 20 - 23 */
			 SAME,   SAME,   SAME,   SAME,   /* 24 - 27 */
			 SAME,   SAME,   SAME,   SAME,   /* 28 - 2B */
			 SAME,   SAME,   SAME,   SAME,   /* 2C - 2F */
			 SAME,   SAME,   SAME,   SAME,   /* 30 - 33 */
			 SAME,   SAME,   SAME,   SAME,   /* 34 - 37 */
			 SAME,   SAME,   SAME,   SAME,   /* 38 - 3B */
			 SAME,   SAME,   SAME,   SAME,   /* 3C - 3F */
			 SAME,   SAME,   SAME,   SAME,   /* 40 - 43 */
			 SAME,   SAME,   SAME,   SAME,   /* 44 - 47 */
			 SAME,   SAME,   SAME,   SAME,   /* 48 - 4B */
			 SAME,   SAME,   SAME,   SAME,   /* 4C - 4F */
			 SAME,   SAME,   SAME,   SAME,   /* 50 - 53 */
			 SAME,   SAME,   SAME,   SAME,   /* 54 - 57 */
			 SAME,   SAME,   SAME,   SAME,   /* 58 - 5B */
			 SAME,   SAME,   SAME,   SAME,   /* 5C - 5F */
			 SAME,   SAME,   SAME,   SAME,   /* 60 - 63 */
			 SAME,   SAME,   SAME,   SAME,   /* 64 - 67 */
			 SAME,   SAME,   SAME,   SAME,   /* 68 - 6B */
			 SAME,   SAME,   SAME,   SAME,   /* 6C - 6F */
			 SAME,   SAME,   SAME,   SAME,   /* 70 - 73 */
			 SAME,   SAME,   SAME,   SAME,   /* 74 - 77 */
			 SAME,   SAME,   SAME,   SAME,   /* 78 - 7B */
			 SAME,   SAME,   SAME,   SAME,   /* 7C - 7F */
			 0x20C7, 0x20FC, 0x20E9, 0x20E2, /* 80 - 83 */
			 0x20E4, 0x20E0, 0x20E5, 0x20E7, /* 84 - 87 */
			 0x20EA, 0x20EB, 0x20E8, 0x20EF, /* 88 - 8B */
			 0x20EE, 0x20EC, 0x20C4, 0x20C5, /* 8C - 8F */
			 0x20C9, 0x20E6, 0x20C6, 0x20F4, /* 90 - 93 */
			 0x20F6, 0x20F3, 0x20FB, 0x20FC, /* 94 - 97 */
			 0x20FF, 0x20D6, 0x20DB, 0x20A2, /* 98 - 9B */
			 0x20A3, 0x20A5, NOCHAR, NOCHAR, /* 9C - 9F */
			 0x20E1, 0x20ED, 0x20F3, 0x20F9, /* A0 - A3 */
			 0x20F1, 0x20D1, 0x20AA, 0x20BA, /* A4 - A7 */
			 0x20BF, 0x2022, 0x2022, 0x203C, /* A8 - AB */
			 0x203E, 0x20A1, 0x20AB, 0x20BB, /* AC - AF */
			 0x20E3, 0x20F5, 0x20A5, 0x20A2, /* B0 - B3 */
			 0x2003, 0x2002, 0x20C0, 0x20C3, /* B4 - B7 */
			 0x20D5, 0x20A7, NOCHAR, NOCHAR, /* B8 - BB */
			 0x20B6, 0x20A9, 0x20AE, NOCHAR, /* BC - BF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* C0 - C3 */
			 0x20AD, NOCHAR, 0x20B0, 0x20C1, /* C4 - C7 */
			 0x20C2, 0x20C8, SAME,   SAME,   /* C8 - CB */
			 SAME,   SAME,   SAME,   SAME,   /* CC - CF */
			 0x20D2, 0x20D3, 0x20D4, NOCHAR, /* D0 - D3 */
			 NOCHAR, 0x20D8, 0x20D9, 0x20DA, /* D4 - D7 */
			 NOCHAR, 0x20DF, NOCHAR, NOCHAR, /* D8 - DB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* DC - DF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* E0 - E3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* E4 - E7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* E8 - EB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* EC - EF */
			 NOCHAR, 0x20B1, 0x2014, 0x2013, /* F0 - F3 */
			 NOCHAR, NOCHAR, 0x20F7, 0x2001, /* F4 - F7 */
			 0x20B1, NOCHAR, 0x20B7, NOCHAR, /* F8 - FB */
			 NOCHAR, 0x20B2, NOCHAR, 0x20D8  /* FC - FF */
};

word zx_to_gem [256] = { NOCHAR, NOCHAR, NOCHAR, NOCHAR,
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  4 -  7 */	 
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  8 -  B */	 
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  C -  F */	 
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 10 - 13 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 14 - 17 */
			 0x205E, NOCHAR, NOCHAR, NOCHAR, /* 18 - 1B */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 1C - 1F */
			 SAME,   SAME,   SAME,   SAME,   /* 20 - 23 */
			 SAME,   SAME,   SAME,   SAME,   /* 24 - 27 */
			 SAME,   SAME,   SAME,   SAME,   /* 28 - 2B */
			 SAME,   SAME,   SAME,   SAME,   /* 2C - 2F */
			 SAME,   SAME,   SAME,   SAME,   /* 30 - 33 */
			 SAME,   SAME,   SAME,   SAME,   /* 34 - 37 */
			 SAME,   SAME,   SAME,   SAME,   /* 38 - 3B */
			 SAME,   SAME,   SAME,   SAME,   /* 3C - 3F */
			 SAME,   SAME,   SAME,   SAME,   /* 40 - 43 */
			 SAME,   SAME,   SAME,   SAME,   /* 44 - 47 */
			 SAME,   SAME,   SAME,   SAME,   /* 48 - 4B */
			 SAME,   SAME,   SAME,   SAME,   /* 4C - 4F */
			 SAME,   SAME,   SAME,   SAME,   /* 50 - 53 */
			 SAME,   SAME,   SAME,   SAME,   /* 54 - 57 */
			 SAME,   SAME,   SAME,   SAME,   /* 58 - 5B */
			 SAME,   SAME,   NOCHAR, SAME,   /* 5C - 5F */
			 NOCHAR, SAME,   SAME,   SAME,   /* 60 - 63 */
			 SAME,   SAME,   SAME,   SAME,   /* 64 - 67 */
			 SAME,   SAME,   SAME,   SAME,   /* 68 - 6B */
			 SAME,   SAME,   SAME,   SAME,   /* 6C - 6F */
			 SAME,   SAME,   SAME,   SAME,   /* 70 - 73 */
			 SAME,   SAME,   SAME,   SAME,   /* 74 - 77 */
			 SAME,   SAME,   SAME,   SAME,   /* 78 - 7B */
			 SAME,   SAME,   SAME, NOCHAR,   /* 7C - 7F */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 80 - 83 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 84 - 87 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 88 - 8B */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 8C - 8F */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 90 - 93 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 94 - 97 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 98 - 9B */
			 0x2060, NOCHAR, NOCHAR, NOCHAR, /* 9C - 9F */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* A0 - A3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* A4 - A7 */
			 NOCHAR, NOCHAR, NOCHAR, 0x203C, /* A8 - AB */
			 0x203E, NOCHAR, NOCHAR, NOCHAR, /* AC - AF */
			 0x7E61, 0x7E6F, 0x209D, 0x209B, /* B0 - B3 */
			 NOCHAR, NOCHAR, NOCHAR, 0x7E41, /* B4 - B7 */
			 0x7E4F, 0x2015, NOCHAR, NOCHAR, /* B8 - BB */
			 0x2014, 0x207F, NOCHAR, NOCHAR, /* BC - BF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* C0 - C3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* C4 - C7 */
			 0x2741, NOCHAR, NOCHAR, NOCHAR, /* C8 - CB */
			 NOCHAR, NOCHAR, 0x2749, NOCHAR, /* CC - CF */
			 NOCHAR, 0x274F, NOCHAR, NOCHAR, /* D0 - D3 */
			 NOCHAR, NOCHAR, 0x2755, NOCHAR, /* D4 - D7 */
			 NOCHAR, 0x20E1, NOCHAR, NOCHAR, /* D8 - DB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* DC - DF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* E0 - E3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* E4 - E7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* E8 - EB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* EC - EF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* F0 - F3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* F4 - F7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* F8 - FB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* FC - FF */
}; 

word gem_to_dos[256] = { NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  0 -  3 */
			 0x2004, NOCHAR, NOCHAR, NOCHAR, /*  4 -  7 */	 
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  8 -  B */	 
			 NOCHAR, 0x200B, NOCHAR, NOCHAR, /*  C -  F */	 
			 SAME,   SAME,   0x2016, NOCHAR, /* 10 - 13 */
			 SAME,   SAME,   0x2013, SAME,   /* 14 - 17 */
			 SAME,   SAME,   SAME,   SAME,   /* 18 - 1B */
			 SAME,   SAME,   SAME,   SAME,   /* 1C - 1F */
			 SAME,   SAME,   SAME,   SAME,   /* 20 - 23 */
			 SAME,   SAME,   SAME,   SAME,   /* 24 - 27 */
			 SAME,   SAME,   SAME,   SAME,   /* 28 - 2B */
			 SAME,   SAME,   SAME,   SAME,   /* 2C - 2F */
			 SAME,   SAME,   SAME,   SAME,   /* 30 - 33 */
			 SAME,   SAME,   SAME,   SAME,   /* 34 - 37 */
			 SAME,   SAME,   SAME,   SAME,   /* 38 - 3B */
			 SAME,   SAME,   SAME,   SAME,   /* 3C - 3F */
			 SAME,   SAME,   SAME,   SAME,   /* 40 - 43 */
			 SAME,   SAME,   SAME,   SAME,   /* 44 - 47 */
			 SAME,   SAME,   SAME,   SAME,   /* 48 - 4B */
			 SAME,   SAME,   SAME,   SAME,   /* 4C - 4F */
			 SAME,   SAME,   SAME,   SAME,   /* 50 - 53 */
			 SAME,   SAME,   SAME,   SAME,   /* 54 - 57 */
			 SAME,   SAME,   SAME,   SAME,   /* 58 - 5B */
			 SAME,   SAME,   SAME,   SAME,   /* 5C - 5F */
			 SAME,   SAME,   SAME,   SAME,   /* 60 - 63 */
			 SAME,   SAME,   SAME,   SAME,   /* 64 - 67 */
			 SAME,   SAME,   SAME,   SAME,   /* 68 - 6B */
			 SAME,   SAME,   SAME,   SAME,   /* 6C - 6F */
			 SAME,   SAME,   SAME,   SAME,   /* 70 - 73 */
			 SAME,   SAME,   SAME,   SAME,   /* 74 - 77 */
			 SAME,   SAME,   SAME,   SAME,   /* 78 - 7B */
			 SAME,   SAME,   SAME,   SAME,   /* 7C - 7F */
			 SAME,   SAME,   SAME,   SAME,   /* 80 - 83 */
			 SAME,   SAME,   SAME,   SAME,   /* 84 - 87 */
			 SAME,   SAME,   SAME,   SAME,   /* 88 - 8B */
			 SAME,   SAME,   SAME,   SAME,   /* 8C - 8F */
			 SAME,   SAME,   SAME,   SAME,   /* 90 - 93 */
			 SAME,   SAME,   SAME,   SAME,   /* 94 - 97 */
			 SAME,   SAME,   SAME,   SAME,   /* 98 - 9B */
			 SAME,   SAME,   SAME,   SAME,   /* 9C - 9F */
			 SAME,   SAME,   SAME,   SAME,   /* A0 - A3 */
			 SAME,   SAME,   SAME,   SAME,   /* A4 - A7 */
			 SAME,   NOCHAR, NOCHAR, NOCHAR, /* A8 - AB */
			 NOCHAR, SAME,   SAME,   SAME,   /* AC - AF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* B0 - B3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* B4 - B7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* B8 - BB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* BC - BF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* C0 - C3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* C4 - C7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* C8 - CB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* CC - CF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* D0 - D3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* D4 - D7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* D8 - DB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* DC - DF */
			 SAME,   SAME,   SAME,   SAME,   /* E0 - E3 */
			 SAME,   SAME,   SAME,   SAME,   /* E4 - E7 */
			 SAME,   SAME,   SAME,   SAME,   /* E8 - EB */
			 SAME,   SAME,   SAME,   SAME,   /* EC - EF */
			 SAME,   SAME,   SAME,   SAME,   /* F0 - F3 */
			 SAME,   SAME,   SAME,   SAME,   /* F4 - F7 */
			 SAME,   SAME,   SAME,   SAME,   /* F8 - FB */
			 SAME,   SAME,   SAME,   NOCHAR  /* FC - FF */
};


word gem_to_iso[256] = { 
			 NOCHAR, 0x20F7, 0x20B4, 0x20B5, /*  0 -  3 */
			 0x2007, NOCHAR, NOCHAR, NOCHAR, /*  4 -  7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  8 -  B */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  C -  F */
			 NOCHAR, NOCHAR, NOCHAR, 0x20F3, /* 10 - 13 */
			 0x20F2, NOCHAR, 0x2011, 0x2010, /* 14 - 17 */
			 SAME,   SAME,   SAME,   SAME,   /* 18 - 1B */
			 SAME,   SAME,   NOCHAR, 0x20E3, /* 1C - 1F */
			 SAME,   SAME,   SAME,   SAME,   /* 20 - 23 */
			 SAME,   SAME,   SAME,   SAME,   /* 24 - 27 */
			 SAME,   SAME,   SAME,   SAME,   /* 28 - 2B */
			 SAME,   SAME,   SAME,   SAME,   /* 2C - 2F */
			 SAME,   SAME,   SAME,   SAME,   /* 30 - 33 */
			 SAME,   SAME,   SAME,   SAME,   /* 34 - 37 */
			 SAME,   SAME,   SAME,   SAME,   /* 38 - 3B */
			 SAME,   SAME,   SAME,   SAME,   /* 3C - 3F */
			 SAME,   SAME,   SAME,   SAME,   /* 40 - 43 */
			 SAME,   SAME,   SAME,   SAME,   /* 44 - 47 */
			 SAME,   SAME,   SAME,   SAME,   /* 48 - 4B */
			 SAME,   SAME,   SAME,   SAME,   /* 4C - 4F */
			 SAME,   SAME,   SAME,   SAME,   /* 50 - 53 */
			 SAME,   SAME,   SAME,   SAME,   /* 54 - 57 */
			 SAME,   SAME,   SAME,   SAME,   /* 58 - 5B */
			 SAME,   SAME,   SAME,   SAME,   /* 5C - 5F */
			 SAME,   SAME,   SAME,   SAME,   /* 60 - 63 */
			 SAME,   SAME,   SAME,   SAME,   /* 64 - 67 */
			 SAME,   SAME,   SAME,   SAME,   /* 68 - 6B */
			 SAME,   SAME,   SAME,   SAME,   /* 6C - 6F */
			 SAME,   SAME,   SAME,   SAME,   /* 70 - 73 */
			 SAME,   SAME,   SAME,   SAME,   /* 74 - 77 */
			 SAME,   SAME,   SAME,   SAME,   /* 78 - 7B */
			 SAME,   SAME,   SAME,   0x20F8, /* 7C - 7F */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 80 - 83 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 84 - 87 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 88 - 8B */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 8C - 8F */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 90 - 93 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 94 - 97 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 98 - 9B */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 9C - 9F */
			 NOCHAR, 0x20AD, 0x209B, 0x209C, /* A0 - A3 */
			 NOCHAR, 0x209D, 0x207C, 0x20B9, /* A4 - A7 */
			 NOCHAR, 0x20BD, 0x20A6, 0x20AE, /* A8 - AB */
			 NOCHAR, 0x20C4, 0x20BE, NOCHAR, /* AC - AF */
			 0x20F8, 0x20F1, 0x20FD, NOCHAR, /* B0 - B3 */
			 NOCHAR, 0x20E6, 0x20BC, 0x20FA, /* B4 - B7 */
			 NOCHAR, NOCHAR, 0x20E7, 0x20EF, /* B8 - BB */
			 NOCHAR, NOCHAR, NOCHAR, 0x20A8, /* BC - BF */
			 0x20B6, 0x20C7, 0x20C8, 0x20B7, /* C0 - C3 */
			 0x208E, 0x208F, 0x2092, 0x2080, /* C4 - C7 */
			 0x20C9, 0x2090, SAME,   SAME,   /* C8 - CB */
			 SAME,   SAME,   SAME,   SAME,   /* CC - CF */
			 NOCHAR, 0x2D44, 0x20A5, 0x20D0, /* D0 - D3 */
			 0x20D1, 0x20D2, 0x2099, NOCHAR, /* D4 - D7 */
			 0x20FF, 0x20D5, 0x20D6, 0x20D7, /* D8 - DB */
			 NOCHAR, 0x2759, NOCHAR, 0x20D9, /* DC - DF */
			 0x2085, 0x20A0, 0x2083, 0x20B0, /* E0 - E3 */
			 0x2084, 0x2086, 0x2081, 0x2087, /* E4 - E7 */
			 0x208A, 0x2082, 0x2088, 0x2089, /* E8 - EB */
			 0x208D, 0x20A1, 0x208C, 0x208B, /* EC - EF */
			 NOCHAR, 0x20A4, 0x2095, 0x20D1, /* F0 - F3 */
			 0x2093, 0x20B1, 0x2094, 0x20F6, /* F4 - F7 */
			 0x2F6E, 0x2097, 0x20A3, 0x2096, /* F8 - FB */
			 0x2081, 0x2779, NOCHAR, 0x2098  /* FC - FF */
			 
};


word gem_to_zx [256] = { NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  0 -  3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  4 -  7 */	 
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  8 -  B */	 
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /*  C -  F */	 
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 10 - 13 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 14 - 17 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 18 - 1B */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 1C - 1F */
			 SAME,   SAME,   SAME,   SAME,   /* 20 - 23 */
			 SAME,   SAME,   SAME,   SAME,   /* 24 - 27 */
			 SAME,   SAME,   SAME,   SAME,   /* 28 - 2B */
			 SAME,   SAME,   SAME,   SAME,   /* 2C - 2F */
			 SAME,   SAME,   SAME,   SAME,   /* 30 - 33 */
			 SAME,   SAME,   SAME,   SAME,   /* 34 - 37 */
			 SAME,   SAME,   SAME,   SAME,   /* 38 - 3B */
			 SAME,   SAME,   SAME,   SAME,   /* 3C - 3F */
			 SAME,   SAME,   SAME,   SAME,   /* 40 - 43 */
			 SAME,   SAME,   SAME,   SAME,   /* 44 - 47 */
			 SAME,   SAME,   SAME,   SAME,   /* 48 - 4B */
			 SAME,   SAME,   SAME,   SAME,   /* 4C - 4F */
			 SAME,   SAME,   SAME,   SAME,   /* 50 - 53 */
			 SAME,   SAME,   SAME,   SAME,   /* 54 - 57 */
			 SAME,   SAME,   SAME,   SAME,   /* 58 - 5B */
			 SAME,   SAME,   0x2018, SAME,   /* 5C - 5F */
			 0x209C, SAME,   SAME,   SAME,   /* 60 - 63 */
			 SAME,   SAME,   SAME,   SAME,   /* 64 - 67 */
			 SAME,   SAME,   SAME,   SAME,   /* 68 - 6B */
			 SAME,   SAME,   SAME,   SAME,   /* 6C - 6F */
			 SAME,   SAME,   SAME,   SAME,   /* 70 - 73 */
			 SAME,   SAME,   SAME,   SAME,   /* 74 - 77 */
			 SAME,   SAME,   SAME,   SAME,   /* 78 - 7B */
			 SAME,   SAME,   SAME, 0x20BD,   /* 7C - 7F */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 80 - 83 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 84 - 87 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 88 - 8B */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 8C - 8F */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 90 - 93 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 94 - 97 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 98 - 9B */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* 9C - 9F */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* A0 - A3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* A4 - A7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* A8 - AB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* AC - AF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* B0 - B3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* B4 - B7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* B8 - BB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* BC - BF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* C0 - C3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* C4 - C7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* C8 - CB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* CC - CF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* D0 - D3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* D4 - D7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* D8 - DB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* DC - DF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* E0 - E3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* E4 - E7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* E8 - EB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* EC - EF */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* F0 - F3 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* F4 - F7 */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* F8 - FB */
			 NOCHAR, NOCHAR, NOCHAR, NOCHAR, /* FC - FF */
};


word *transforms[6] = 
{ 
	&dos_to_gem[0], &iso_to_gem[0], &zx_to_gem[0],
	&gem_to_dos[0], &gem_to_iso[0], &gem_to_zx[0] 
};

MLOCAL VOID transfont(FONT *f, FONT *f2, WORD what)
{
	int n, y;
	word op;
	word *tf = transforms[what];
	byte *d, *s1, *s2;

	memcpy(f2->data, f->data, 256 * f->height);
	for (n = 0; n < 256; n++)
	{
		op = tf[n];
		switch (op >> 8)
		{
			case 0: 	/* leave alone */
			break;

			case 0x20:	/* Copy another char */
			memcpy(&f2->data[f->height * n],
			       &f->data[f->height * (op & 0xFF)],  f->height);
			break;

			default: /* OR two chars together */
			d  = f2->data + n * f->height;
			s1 = f->data  + (op & 0xFF) * f->height;
			s2 = f->data  + (op >>  8) * f->height;
			for (y = 0; y < f->height; y++) 
			{	
				d[y] = s1[y] | s2[y];
			}
			break;
		}
	}
	memcpy(f->data, f2->data, 256 * f->height);
}

/* Trans-code a font. */
MLOCAL VOID transcode(FONTSET *f, WORD what)
{
	byte height[4];
	FONTSET *f2;
	int n;

	for (n = 0; n < f->nfonts; n++) height[n] = f->font[n].height;
	f2 = alloc_fontset(f->nfonts, height);
	if (!f2) 
	{
		rsrc_alert(ALNOMEM, 1);
		return;
	}
	for (n = 0; n < f->nfonts; n++) 
	{
		transfont(&f->font[n], &f2->font[n], what);
	}
	free_fontset(f2);	
}

/* Find out if there is a document window with the focus; if there is,
 * enable various menu items 
 */
 
VOID do_enabling(VOID)
{

	BOOLEAN b = (topdoc == -1) ? 0 : 1;
		
	menu_ienable(gl_menu, MENUSAVE, b);
	menu_ienable(gl_menu, MENUSVAS, b);

	b = (topdoc == -1) ? 0 : 1;

	menu_ienable(gl_menu, MENUCOPY, b);
	menu_ienable(gl_menu, MENUCUT,  b);
	menu_ienable(gl_menu, MENUPAST, b);
	menu_ienable(gl_menu, MENUSELA, b);

	menu_ienable(gl_menu, MENUFXRH, b);
	menu_ienable(gl_menu, MENUFXRV, b);
	menu_ienable(gl_menu, MENUFXRA, b);
	menu_ienable(gl_menu, MENUFXRC, b);
	menu_ienable(gl_menu, MENUFXMU, b);
	menu_ienable(gl_menu, MENUFXMD, b);
	menu_ienable(gl_menu, MENUFXML, b);
	menu_ienable(gl_menu, MENUFXMR, b);

	menu_ienable(gl_menu, MENUBOLD, b);
	menu_ienable(gl_menu, MENUITAL, b);
	menu_ienable(gl_menu, MENUUL,   b);
	menu_ienable(gl_menu, MENUINV,  b);

	menu_ienable(gl_menu, MENUTRCD, b);

}


/* Find out if the internal variables topdoc and topedt are correct */

VOID calc_topwnd(VOID)
{
	int n;

	if (topdoc == -1)
	{
		for (n = 0; n < MAXW; n++) if (sysfont_view[n].exists) 
		{
			if (sysfont_view[n].hwnd >= 0)
			{
				wind_setl(sysfont_view[n].hwnd, WF_TOP, 0);
				topdoc = n;
				return;
			}
		}
	} 
}


static WORD winlist[MAXW + 1];


static int get_winlist(void)
{
	int nw, n;
	for (n = nw = 0; n < MAXW; n++)
	{
		if (sysfont_view[n].exists && sysfont_view[n].hwnd > 0)
		{
			winlist[nw++] = sysfont_view[n].hwnd;
		}
	}
	winlist[nw] = 0;
	return nw;
}


void tile_windows(void)
{
	WORD wc, y;
	GRECT rc;

	wc = get_winlist();
	if (!wc) return;

	if (wc == 1) 
	{
		do_message(winlist[0], NULL, full_view, NULL);
	}
	else
	{
		rc.g_x = align_x(gl_xfull) - 1;
		rc.g_h = gl_hfull / wc;
		rc.g_w = gl_wfull;
		for (y = 0; y < wc; y++)
		{
			rc.g_y = gl_yfull + rc.g_h * y;
			do_message(winlist[y], &rc, resize_view, NULL);
		}
	}
}

void cascade_windows(void)
{
	WORD nw, wc, xstep, ystep;
	GRECT rc;
	
	wc = get_winlist(); if (!wc) return;

	rc.g_w = gl_wfull / 2;
	rc.g_h = gl_hfull / 2;

	xstep = rc.g_w / wc;
	ystep = rc.g_h / wc;

	for (nw = 0; nw < wc; nw++)
	{
		rc.g_x = align_x(gl_xfull + (nw * xstep)) - 1;
		rc.g_y = gl_yfull + (nw * ystep);
	
		do_message(winlist[nw], &rc, resize_view, NULL);
	}

}




/*   
 * This function finds out which view object owns a window, and
 * calls that object's message handler. Since we this is C and not C++,
 * we pass the function a handle to the object.
 */
VOID do_message(WORD wh, GRECT *area, MSGFUNC f, MSGFUNC f2)
{
	int n;
	
	for (n = 0; n < MAXW; n++)
	{
		if (wh == sysfont_view[n].hwnd) 
		{
			f(n, area);
			return;
		}
	}
}


WORD hndl_button(VOID)
{
	SYSFONT_VIEW *pDoc;

	if (topdoc >= 0)
	{
		pDoc = &sysfont_view[topdoc];
		if (pDoc->hwnd >= 0)
		{
			if (mousex >= pDoc->wx && mousex <= (pDoc->wx + pDoc->ww) &&
			    mousey >= pDoc->wy && mousey <= (pDoc->wy + pDoc->wh))
			{
				click_view(topdoc);
			}
		}
	}
	return 0;
}



WORD hndl_menu(WORD title, WORD item)
{
	WORD	done;
	WORD	i;
	FILE	*fp;
	char	filename[160];
	FONTSET *set;
	static char types[] = "RPZEAW";	
	graf_mouse(ARROW, 0x0L);
	done = FALSE;

	switch (title) 
	{
	case MENUDESK: /* Desk menu */
		if (item == MENUABOU)	/* 'About' menu item	*/
			do_aboutbox();
		break;

	case MENUFILE:
		switch(item)
		{
		case MENUNEW:
			set = newfile_dlg();
			if (set) create_view("Untitled", set);
			do_enabling();
			break;

		case MENUOPEN:
			fp = get_file(TRUE, filename, "*.*", "rb");
			if (fp) set = load_font(fp); else set = NULL;
			if (set) create_view(filename, set);
			do_enabling();
			break;

		case MENUSAVE:
			strcpy(filename, sysfont_view[topdoc].title + 1);
			filename[strlen(filename)-1] = 0;

			set = sysfont_view[topdoc].fontset;
			if (set->format)
			{
				fp = fopen(filename, "r+b");
				if (!fp) fp = fopen(filename, "w+b");	
				if (fp) 
				{
					i = save_font(set, fp);
					fclose(fp);
					if (i) sysfont_view[topdoc].dirty_flag = 0;
				}	
				else syserror();
				break;
			}
			// If no format set, fall through to Save As
		case MENUSVAS:
			i = saveas_dlg(sysfont_view[topdoc].fontset->format); 
			if (i < 0) break;
			set = sysfont_view[topdoc].fontset;
			set->format = types[i];
			if (i == 3 || i == 4 || i == 5)	
				fp = get_file(FALSE, filename, "*.*", "r+b");
			else	fp = get_file(FALSE, filename, "*.*", "w+b");
			if (fp) i = save_font(set, fp);
			else	{ syserror(); i = 0; }
			if (i) 
			{
				gen_title(topdoc, filename);
				gen_info(topdoc);	
				sysfont_view[topdoc].dirty_flag = 0;
			}
			fclose(fp);
			break;
			
		case MENUQUIT:	
			done = 1; 
			break;
		}
		break;
	case MENUWND:
		switch(item)
		{
			case MENUTILE:	tile_windows();    break;
			case MENUCASC:	cascade_windows(); break;
		}
		break;
	case MENUEDIT:
		switch(item)
		{
			case MENUSELA:	sysfont_view[topdoc].mark[0] = 0;
					sysfont_view[topdoc].mark[1] = 255;
					send_redraw(sysfont_view[topdoc].hwnd, (GRECT *)(&sysfont_view[topdoc].rx));	
					break;
			case MENUCOPY:	do_copy(topdoc);
					break;
			case MENUCUT:	do_copy(topdoc);
					do_action(topdoc, act_cut);
					break;
			case MENUPAST:	do_paste(topdoc);
					break;
			case MENUPREF:	prefs_dlg();
					if (topdoc >= 0) send_redraw(sysfont_view[topdoc].hwnd, (GRECT *)(&sysfont_view[topdoc].wx));	
					break;
		}
		break;
	case MENUFONT:
		switch(item)
		{
			case MENUTRCD:	i = transcode_dlg(sysfont_view[topdoc].fontset->format);
					if (i >= 0) transcode(sysfont_view[topdoc].fontset, i);
					rebuild_fdbs(topdoc);
					redraw_view(topdoc);
					break;	
		}
		break;
	case MENUFX:
		switch(item)
		{
			case MENUFXRH:	do_action(topdoc, act_reflecth); break;
			case MENUFXRV:	do_action(topdoc, act_reflectv); break;
			case MENUFXRA:	do_action(topdoc, act_rotatea); break;
			case MENUFXRC:	do_action(topdoc, act_rotatec); break;
			case MENUFXMU:	do_action(topdoc, act_moveu); break;
			case MENUFXMD:	do_action(topdoc, act_moved); break;
			case MENUFXML:	do_action(topdoc, act_movel); break;
			case MENUFXMR:	do_action(topdoc, act_mover); break;
		}
		break;
	case MENUFX2:
		switch(item)
		{
			case MENUBOLD:	do_action(topdoc, act_bold); break;
			case MENUITAL:	do_action(topdoc, act_italic); break;
			case MENUUL:	do_action(topdoc, act_underline); break;
			case MENUINV:	do_action(topdoc, act_invert); break;
		}
		break;
	}
	menu_tnormal(gl_menu,title,TRUE);
	graf_mouse(CROSS, 0x0L);
	return (done);	
}

WORD hndl_keybd(WORD kreturn)
{
	/* Menu shortcuts */
	WORD kcode = (kreturn & 0xFF);
	
	if (kcode == ('Q' - '@')) return hndl_menu(MENUFILE, MENUQUIT);
	if (kcode == ('O' - '@')) return hndl_menu(MENUFILE, MENUOPEN);
	if (kcode == ('S' - '@')) return hndl_menu(MENUFILE, MENUSAVE);
	if (kcode == ('N' - '@')) return hndl_menu(MENUFILE, MENUNEW);
	if (kcode == ('A' - '@')) return hndl_menu(MENUFILE, MENUSVAS);
	if (kcode == ('X' - '@')) return hndl_menu(MENUEDIT, MENUCUT);
	if (kcode == ('C' - '@')) return hndl_menu(MENUEDIT, MENUCOPY);
	if (kcode == ('V' - '@')) return hndl_menu(MENUEDIT, MENUPAST);
	return 0;
}
/*------------------------------*/
/*			hndl_msg			*/
/*------------------------------*/
BOOLEAN	hndl_msg()
{
	BOOLEAN	done; 
	WORD	wdw_hndl;
//	GRECT	work;

	done = FALSE;
	wdw_hndl = gl_rmsg[3]; 
	switch( gl_rmsg[0] )
	{
	case MN_SELECTED:
		done = hndl_menu(wdw_hndl, gl_rmsg[4]);// Title, Item	
		break;

	case WM_REDRAW:
		graf_mouse(M_OFF, 0x0L);  	// turn mouse off
		do_message(wdw_hndl, (GRECT *)&gl_rmsg[4], paint_view, NULL);
		graf_mouse(M_ON, 0x0L);
		break;

	case WM_TOPPED:
		do_message(wdw_hndl, NULL, topped_view, NULL);
		break;
		
	case WM_CLOSED:
		do_message(wdw_hndl, NULL, ask_close_view, NULL);
		break;

	case WM_SIZED:
		do_message(wdw_hndl, (GRECT *)&gl_rmsg[4], resize_view, NULL);
		break;

	case WM_MOVED:
		do_message(wdw_hndl, (GRECT *)&gl_rmsg[4], move_view, NULL);
		break;

	case WM_FULLED:
		do_message(wdw_hndl, NULL, full_view, NULL);
		break;

	case WM_VSLID:
		do_message(wdw_hndl, (GRECT *)&gl_rmsg[4], slider_view, NULL);
		break;

	case WM_ARROWED:
		do_message(wdw_hndl, (GRECT *)&gl_rmsg[4], arrow_view, NULL);
		break;

	case WM_UNTOPPED:
	case WM_NEWTOP:
		do_message(wdw_hndl, NULL, newtop_view, NULL);
		break;

/*	default:
		{
			char s[50];

			sprintf(s, "[1][Message %d][OK]", gl_rmsg[0]);
			form_alert(1,s);
		}
*/		
	} 
	do_enabling();
	return(done);
} 


/*
#define WM_HSLID 25
#define WM_UNTOPPED 30
#define WM_ONTOP 31
#define WM_OFFTOP 32
*/
BOOLEAN hndl_mouse(SYSFONT_VIEW *view, WORD mousex, WORD mousey)
{
	if (mousex < view->wx             ||
	    mousex >= view->wx + view->ww ||
	    mousey < view->wy		  ||
	    mousey >= view->wy + view->wh)
	{
		graf_mouse(ARROW, 0x0L);
		view->m_outl = 0;
		view->m_outr = 0;
		return FALSE;
	}

	if (mousex < view->rx) 
	{
		graf_mouse(CROSS, 0x0L);
		view->m_outl = 1;
		view->m_outr = 0;
	}
	else
	{
		graf_mouse(IBEAM, 0x0L);
		view->m_outl = 0;
		view->m_outr = 1;
	}
	return FALSE;
}


VOID sysfont(VOID)				/* main event multi loop		*/
{
	BOOLEAN	done;
	WORD flags;
	SYSFONT_VIEW *view;
	MLOCAL SYSFONT_VIEW dummy;
	
	done = FALSE;
	FOREVER
	{
		flags = MU_BUTTON | MU_MESAG | MU_KEYBD;

		if (topdoc > -1) 
		{
			view = &sysfont_view[topdoc];
			flags |= MU_M1 | MU_M2;
		}
		else	view = &dummy;
	  	/* wait for           Button ,    Message, 1 Mouse,keyboard */
               	/* 		      Event	  Event    Rect    Event    */
	
		ev_which = evnt_multi(flags,
		0x01, 0x01, 0x01, /* 1 click, 1 button, button down */
		view->m_outl,  /* entry , desk_area x,y,w,h */ 
		(UWORD) view->lx, (UWORD) view->ly,
		(UWORD) view->lw, (UWORD) view->lh,
		/* mouse rect 2 flags , x,y,w,h */
		view->m_outr,
		(UWORD) view->rx, (UWORD) view->ry,
		(UWORD) view->rw, (UWORD) view->rh,
		/* Message buffer, timer low count , high count 	*/
		ad_rmsg, 0, 0,  
		/* mouse posit ,  btn state, r & lshift, Ctrl, Alt 	*/
		&mousex, &mousey, &bstate, &kstate, 
		/* keybd key,# btn clicks */
		&kreturn, &bclicks);

		wind_update(BEG_UPDATE);

		if (ev_which & MU_MESAG)   
		if (hndl_msg()) 
			break;  

		if (ev_which & MU_BUTTON)
		if (hndl_button())
			break;

		if (ev_which & MU_M1)
		if (hndl_mouse(view, mousex, mousey))
			break;

		if (ev_which & MU_M2)
		if (hndl_mouse(view, mousex, mousey))
			break;

		if (ev_which & MU_KEYBD)
		if (hndl_keybd(kreturn))
			break;
		wind_update(END_UPDATE);
	}
}

/*************************************************************/



/*------------------------------*/
/*			edicon_term			*/
/*------------------------------*/
VOID sysfont_term(WORD term_type)
{
	int nw;

// Clean up any document objects. 
	
	for (nw = 0; nw < MAXW; nw++) destroy_view(nw);
	
	switch (term_type)	/* NOTE: all cases fall through		*/
	{
		case (0 /* normal termination */):
		
		case (3 /* no window available */):
			menu_bar(0x0L, FALSE);
		case (2 /* not enough memory */): 
			graf_mouse(ARROW, 0x0L);
			v_clsvwk( vdi_handle );
		case (1 /* no .RSC file or no virt wksta */):
			wind_update(END_UPDATE);
			appl_exit();
		case (4 /* appl_init() failed */):
			break;
	}
	if (clipboard) free(clipboard);
}


/*************************************************************/



WORD sysfont_init(WORD ARGC, BYTE *ARGV[])
{
	WORD	work_in[11];
	WORD	i;
	LPTREE	tree;

	memset(sysfont_view, 0, sizeof(sysfont_view));
	memset(&g_xbuf, 0, sizeof(g_xbuf));
	g_xbuf.buf_len = sizeof(g_xbuf);
	
	gl_apid = appl_init(&g_xbuf);			/* init application	*/
	if (gl_apid == -1)
		return(4); 

	wind_update(BEG_UPDATE);
	graf_mouse(HOUR_GLASS, 0x0L);
	/* open virtual workstation */
	for (i=0; i<10; i++)
	{
		work_in[i]=1; /* initial defaults: line style,color,etc	*/
	}
	work_in[10]=2;	/* raster coordinates	*/

	/* Get the VDI handle for GEM AES screen workstation */
	gem_handle = graf_handle(&gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);
	vdi_handle = gem_handle;
	
	v_opnvwk(work_in,&vdi_handle,work_out);
	if (vdi_handle == 0)
		return(1);

	if (!rsrc_init( ADDR("SYSFONT.RSC"), ABOUTBOX, DLGTRANS, MAINMENU ))
	{    	
		/* No Resource File  */
		graf_mouse(ARROW, 0x0L);
		form_alert(1,"[3][Fatal Error !|SYSFONT.RSC|File Not Found][ Abort ]");
		return(2);
	}

	scrn_width = work_out[0] + 1; 		/* # of pixels wide	*/
	scrn_height = work_out[1] + 1; 		/* # of pixels high	*/
	scrn_xsize = work_out[3];		/* pixel width (microns)*/
	scrn_ysize = work_out[4];		/* pixel height(microns)*/

	scrn_area.g_x = 0;
	scrn_area.g_y = 0;
	scrn_area.g_w = scrn_width;
	scrn_area.g_h = scrn_height;

	rsrc_gaddr(R_TREE, ABOUTBOX, (LPVOID *)&tree);
	trans_gimage(vdi_handle, tree, APPICON); 

	ad_rmsg = ADDR((BYTE *) &gl_rmsg[0]); /* init message address	*/

	/* Get Desktop work area	*/
	wind_get(DESK, WF_WXYWH, &gl_xfull, &gl_yfull, &gl_wfull, &gl_hfull);

	desk_area.g_x = gl_xfull;
	desk_area.g_y = gl_yfull;
	desk_area.g_w = gl_wfull;
	desk_area.g_h = gl_hfull;	
	
	/* initialize menu    */
	rsrc_gaddr(R_TREE, MAINMENU, &gl_menu);

	/* show menu	      */
	menu_bar(gl_menu, TRUE);	
		
	gl_xfull = align_x(gl_xfull);

	/* get work area of window */
	/* Since we allow multiple windows, we can't just set a "work_area" to
	   the size of a window. Instead, we have to handle messages from the 
	   entire desk area */

#if 0
	// Get the resource tree for editor windows
	rsrc_gaddr(R_TREE, BITMAPED, (LPVOID *)&trEdit);

	load_settings();	/* Load EDICON.CFG if there is one */
#endif	
	// Load command line parameters
	for (i = 1; i < ARGC; i++)
	{
		FILE *fp = fopen(ARGV[i], "rb");
		if (fp)
		{
			FONTSET *set;

			if (fp) set = load_font(fp); else set = NULL;
			if (set) create_view(ARGV[i], set);
		}	
	}
	
	graf_mouse(ARROW, 0x0L);
	wind_update(END_UPDATE);
	do_enabling();
	load_prefs();
	
	return(0);	/* successful initialization */
}


/*

Page*/
/************************************************************************/
/************************************************************************/
/****															     ****/
/****			    			Main Program					     ****/
/****															     ****/
/************************************************************************/
/************************************************************************/

/*------------------------------*/
/*	GEMAIN			*/
/*------------------------------*/
WORD GEMAIN(WORD ARGC, BYTE *ARGV[])
{
	WORD	term_type;
	
	if (!(term_type = sysfont_init(ARGC, ARGV)))
		{
		sysfont();
		}
	sysfont_term(term_type);

}




