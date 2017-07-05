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

WORD fmemcmp(LPVOID d, LPVOID s, int n);
VOID fmemcpy(LPVOID d, LPVOID s, int n);
VOID fmemset(LPVOID d, BYTE c, int n);
VOID fstrcpy(LPBYTE d, LPBYTE s);
VOID fstrncpy(LPBYTE d, LPBYTE s, int n);
WORD fstrlen(LPBYTE d);

