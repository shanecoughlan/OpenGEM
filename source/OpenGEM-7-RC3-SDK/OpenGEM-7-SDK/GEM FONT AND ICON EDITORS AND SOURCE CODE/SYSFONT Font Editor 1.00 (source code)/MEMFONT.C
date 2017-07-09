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

#include <stdlib.h>
#include <string.h>
#include "memfont.h"


#define H(n) (height[n] ? height[n] : 1)

FONTSET *alloc_fontset(word nfonts, byte *height)
{
	unsigned data_len = nfonts * sizeof(FONT);
	unsigned n;
	FONTSET *set;
	byte *data;

	data_len += sizeof(FONTSET);
	for (n = 0; n < nfonts; n++) data_len += 256 * H(n);
	data = malloc(data_len);
	memset(data, 0, data_len);
	if (!data) return NULL;
	set = (FONTSET *)data;
	set->nfonts = nfonts;
	data += sizeof(FONTSET);
	data += nfonts * sizeof(FONT);
	for (n = 0; n < nfonts; n++)
	{
		set->font[n].height = H(n);
		set->font[n].data   = data;
		data += 256 * H(n);
	}
	return set;
}

void free_fontset(FONTSET *set)
{
	free(set);
}

