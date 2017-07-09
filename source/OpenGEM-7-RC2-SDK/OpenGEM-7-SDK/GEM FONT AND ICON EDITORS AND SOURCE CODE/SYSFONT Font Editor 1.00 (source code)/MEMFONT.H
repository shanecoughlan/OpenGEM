/*****************************************************************************
*   SYSFONT - Edit the GEM system font                                       *
*   Copyright 2002, John Elliott                                             *
*                                                                            *
* This program is free software; you can redistribute it and/or              *
* modify it under the terms of the GNU General Public License                *
* as published by the Free Software Foundation; either version 2             *
* of the License, or (at your option) any later version.                     *
*                                                                            *
* This program is distributed in the hope that it will be useful,            *
* but WITHOUT ANY WARRANTY; without even the implied warranty of             *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
* GNU General Public License for more details.                               *
*                                                                            *
* You should have received a copy of the GNU General Public License          *
* along with this program; if not, write to the Free Software                *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*
*                                                                           *
******************************************************************************/

/* Memory structure for a font */

typedef unsigned char byte;
typedef unsigned short word;

#pragma pack(1)	/* So all files compile with the same structure sizes */
typedef struct font
{
	byte height;
	byte *data;	
} FONT;

typedef struct fontset
{
	char format;	/* P=PSF R=Raw Z=ZX E=EXE A=Atari ROM W=Windows */
	word nfonts;
	FONT font[0];
} FONTSET;

FONTSET *alloc_fontset(word nfonts, byte *height);
void free_fontset(FONTSET *f);
