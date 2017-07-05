/*************************************************************************
**       Copyright 1999, Caldera Thin Clients, Inc.                     ** 
**       This software is licenced under the GNU Public License.        ** 
**       Please see LICENSE.TXT for further information.                ** 
**                                                                      ** 
**                  Historical Copyright                                ** 
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

pragma On(Optimize_for_space);

#define printf printfmd

#include "portab.h"
#include "gsxdef.h"

#define HEXEOF	(-1)
#define HEXEOL	(-2)

EXTERN BYTE	*ds_gdos_path;
EXTERN WORD	f_open(BYTE *, WORD *);
EXTERN WORD	f_seek(WORD, WORD, LONG, LONG *);
EXTERN WORD	f_read(WORD, VOID *, UWORD, UWORD *);
EXTERN WORD	f_close(WORD);
EXTERN WORD	allo_mem(WORD, VOID **, WORD *);
EXTERN WORD	free_mem(VOID *);

#define localbuf PTSIN
EXTERN UBYTE	localbuf[256];	/* must be in local data segment for	*/
				/* printer_out call			*/

static WORD	initted = 0;
static UBYTE	*strbuf = (UBYTE *) 0, *curptr = (UBYTE *) 0;
static WORD	*strmap = (WORD *) 0;
static BYTE	*atmfile = DEVICE_ABBREVIATION ".ATM";

BYTE	*strcpy(BYTE *dst, BYTE *src)
{
    while ((*dst++ = *src++) != '\0');
    return dst - 1;
} /* strcpy */

WORD	gethex(VOID)
{
    WORD	i, j, c;
    
    while (*curptr == ' ' || *curptr == '\t' || *curptr == '\r')
	curptr++;
    i = j = 0;
    c = *curptr++;
    switch (c) {
	case '\n':
	    return HEXEOL;
	case '\0':
	    curptr--;
	    return HEXEOF;
	case '0'..'9':
	    i = c - '0';
	    break;
	case 'A'..'F':
	    i = c - 'A' + 10;
	    break;
	case 'a'..'f':
	    i = c - 'a' + 10;
	    break;
	default:
	    break;
    }

    c = *curptr++;
    switch (c) {
	case '\n':
	    return HEXEOL;
	case '\0':
	    curptr--;
	    return HEXEOF;
	case '0'..'9':
	    j = c - '0';
	    break;
	case 'A'..'F':
	    j = c - 'A' + 10;
	    break;
	case 'a'..'f':
	    j = c - 'a' + 10;
	    break;
	default:
	    break;
    }
    return (i << 4) + j;
} /* gethex */

/* Given a character in [128,255] alpha_map returns a pointer to a string */
/* containing a count and data which is to be printed instead of c.	  */
/* Mapping information is obtained from xxx.atm in the GDOS directory	  */
/* where xxx is the DEVICE_ABBREVIATION in gsxdef			  */
/* Each line in the file is, for example, of the form:			  */
/*	80 410842							  */
/* where 80 hex is the character to be replaced by the sequence 41, 08,	  */
/* 42, i.e. A, <BS>, B							  */

UBYTE	*alpha_map(UBYTE c)
{
    WORD	handle, error, size, i, t, free;
    BYTE	file_name[80], *dstptr;
    LONG	new_pos;

    if (!initted) {
	dstptr = file_name;
	*dstptr++ = ds_gdos_path[0] + 'a';
	*dstptr++ = ':';
	dstptr = strcpy(dstptr, ds_gdos_path + 1);
	*dstptr++ = '\\';
	strcpy(dstptr, atmfile);
	f_open(file_name, &handle);
	if (handle) {
	    f_seek(handle, 2, 0L, &new_pos);
	    size = (WORD)new_pos;
	    error = allo_mem((size + 2 + 256 + 15) >> 4, (VOID **)&strmap,&t);
	    if (!error) {
		f_seek(handle, 0, 0L, &new_pos);
		strbuf = (BYTE *) (strmap + 128);
		f_read(handle, strbuf, size, (UWORD *)&t);
		strbuf[size] = '\n';
		strbuf[size + 1] = '\0';

		free = 0;
		for (i = 0; i < 128; i++)
		    strmap[i] = -1;
		curptr = strbuf;
		while ((i = gethex()) != HEXEOF) {
		    if (i > 127) {
			strmap[i - 128] = free++;
			while ((t = gethex()) != HEXEOL)
			    strbuf[free++] = t;
			strbuf[strmap[i - 128]] = free - strmap[i - 128] - 1;
		    }	
		}
	    }
	    f_close(handle);
	}
	initted = 1;
    }
    if (strmap && strmap[c - 128] >= 0) {
	curptr = strbuf + strmap[c-128];
	for (i = 0; i <= curptr[0]; i++)
	    localbuf[i] = curptr[i];

    }
    else {
	localbuf[0] = 1;
	localbuf[1] = c;
    }
    return localbuf;
} /* end alpha_map */

/* clear_alpha is called at close workstation time */

VOID	clear_alpha(VOID)
{
    if (strmap)
	free_mem((VOID *)strmap);
} /* clear_alpha */

/* end alpha_text.c */