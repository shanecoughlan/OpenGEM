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
#include "ediconf.h"
#include "farmem.h"
#include "clipbd.h"

#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>


static LPBYTE pb = NULL;
static WORD pw, ph;

void cb_copy (WORD w, WORD h, LPBYTE data, LPBYTE mask)
{
	WORD len;
	
	if (pb) local_free(pb);

	len = (w >> 3) * h;	/* This assumes horizontal size is word-aligned, 
	                     * which it is in standard icons */
	pb = local_alloc(2*len);

	if (!pb) return;

	fmemcpy(pb,     data, len);
	fmemcpy(pb+len, mask, len);
	pw = w;
	ph = h;
}


void cb_paste(WORD w, WORD h, LPBYTE data, LPBYTE mask)
{
	WORD len, len2;
	
	if (!pb) return;

	len  = ( w >> 3) *  h;
	len2 = (pw >> 3) * ph;

	if (len > len2) len = len2;
	if (pw == w)
	{
		fmemcpy(data, pb,      len);
		fmemcpy(mask, pb+len2, len);
	}
}


