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


#include "ppdgem.h"
#include "farmem.h"

/************/
					
VOID fmemcpy(LPVOID d, LPVOID s, int n)
{
	int b;
	LPBYTE db = (LPBYTE)d;
	LPBYTE sb = (LPBYTE)s;
	
	for (b = 0; b < n; b++)
	{
		db[b] = sb[b];
	}
}

WORD fmemcmp(LPVOID d, LPVOID s, int n)
{
	int b;
	LPBYTE db = (LPBYTE)d;
	LPBYTE sb = (LPBYTE)s;
	
	for (b = 0; b < n; b++)
	{
		if (db[b] != sb[b]) return sb[b] - db[b];
	}
	return 0;
}



VOID fmemset(LPVOID d, BYTE c, int n)
{
	int b;
	LPBYTE db = (LPBYTE)d;
	
	for (b = 0; b < n; b++)
	{
		db[b] = c;
	}
}



VOID fstrcpy(LPBYTE d, LPBYTE s)
{
	int b = -1;

	do
	{
		++b;
		d[b] = s[b];
		
	} while (s[b]);
}

VOID fstrncpy(LPBYTE d, LPBYTE s, int max)
{
	int b = -1;

	do
	{
		++b;
		d[b] = s[b];
		--max;
		if (!max) return;
		
	} while (s[b]);
}



WORD fstrlen(LPBYTE s)
{
	int b = 0;

	FOREVER
	{
		if (s[b] == 0) return b;
		++b;
	}
}
