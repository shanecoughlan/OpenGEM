/*	GEMPPLIB.C	24-07-1999 John Elliott		*/

/*
*       Copyright 1999, John Elliott                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*/

#include "aes.h"

#if PROPLIB

typedef struct          /* We don't have fopen(), fgetc() etc here, so make */
{                       /* our own. These do buffering just as if they      */
	WORD f;         /* were running under CP/M, with 128-byte records;  */
	BYTE buf[128];  /* this is intended to avoid the overhead of a DOS  */
	WORD pos;       /* call to read every single byte                   */
	WORD mode;
	BYTE nextc;
} FILEBUF;

extern UWORD DOS_ERR;

MLOCAL FILEBUF fin, fout;
MLOCAL LPBYTE buf, buf2;
MLOCAL WORD len;

MLOCAL WORD fbopen(LPBYTE filename, FILEBUF *fb, WORD mode);
MLOCAL WORD fbclose(FILEBUF *fb);
MLOCAL WORD fbgetc(FILEBUF *fb);
MLOCAL WORD fbnextc(FILEBUF *fb);
MLOCAL WORD fbputc(FILEBUF *fb, BYTE ch);
MLOCAL WORD fbgets(FILEBUF *fb, LPBYTE buf, WORD max);
MLOCAL WORD fbputs(FILEBUF *fb, LPBYTE buf);

MLOCAL WORD prop_alloc(LPBYTE program, LPBYTE section);
MLOCAL WORD prop_open(FILEBUF *fb, WORD mode);
MLOCAL WORD prop_ren();

/* Routines to "get" and "put" settings. The settings are currently 
 * stored in one big text file, but the calls should be flexible enough
 * to allow separate files or a Registry. 
 */


WORD prop_get(LPBYTE program, LPBYTE section, LPBYTE buffer, WORD buflen, WORD options)
/* program: Application section name - eg "GEM.AES" or "DOODLE" 
 * section: Subsection name          - eg "Colour.Desktop"
 * buf:     Address to store retrieved value, if any
 * buflen:  Space allowed for returned value
 * options: Currently ignored. Bit 0 is 0 if the setting is per-user (HKCU)
 *          and 1 if the setting is global (HKLM). Global settings are 
 *          not encouraged; but since GEM doesn't have user profiles 
 *          anyway it's all academic.
 *
 * Returns 0 if OK, 1 if property not found, -1 if error
 */
{
	
	WORD eq, r, p;

//#asm
//	int	#3
//#endasm
	
	if (prop_alloc(program, section)) return -2;

	r = prop_open(&fin, 0);
	if (r) 
	{ 
		dos_free(buf); 
		return r;
	}
	
	p = 0;
	while (p != 26)
	{
		/* Read in the "key" name */
		fbgets(&fin, buf2, len);

		/* Does it match? */
		eq = 1;
		for (p = 0; p < (len-1); p++)
		{
			char c1, c2;

			c1 = buf[p]; c2 = buf2[p];

			/* (case-insensitive comparison) */
			
			if (c1 >= 'a' && c1 <= 'z') c1 -= ('a'-'A');
			if (c2 >= 'a' && c2 <= 'z') c2 -= ('a'-'A');
			
			if (c1 != c2) { eq = 0; break; }
		}
		if (eq)
		{
			while (fbnextc(&fin) == ' ') fbgetc(&fin);
			fbgets(&fin, buffer, buflen);
			fbclose(&fin);
			dos_free(buf);
			return 0;
		}
		p = 0;
		while (p != 26 && p != '\n' && p != '\r')
		{
			p = fbgetc(&fin);

			if (p == 26) break;
		}
		p = fbnextc(&fin);
		/* Copy & skip all newlines & carriage returns */
		while (p == '\r' || p == '\n')
		{
			fbgetc(&fin);
			p = fbnextc(&fin);
		}
		
	}
	
	fbclose(&fin);
	dos_free(buf);
	return 1;
}



/* Delete / put a property. "buffer" is NULL to delete, else new value */

WORD prop_put(LPBYTE program, LPBYTE section, LPBYTE buffer, WORD options)
{
	WORD r, rv, p, eq;

	if (prop_alloc(program, section)) return -2;
	
	
	r = prop_open(&fin,  0); if (r) { dos_free(buf); return r; }
	r = prop_open(&fout, 2); if (r) { fbclose(&fin); dos_free(buf); return r; }	

	rv = 1;
	
	p = 0;
	while (p != 26)
	{

		/* Read in the "key" name */
		r = fbgets(&fin, buf2, len); 

		/* Does it match? */
		eq = 1;
		for (p = 0; p < (len-1); p++)
		{
			char c1, c2;

			c1 = buf[p]; c2 = buf2[p];

			/* (case-insensitive comparison) */
			
			if (c1 >= 'a' && c1 <= 'z') c1 -= ('a'-'A');
			if (c2 >= 'a' && c2 <= 'z') c2 -= ('a'-'A');
			
			if (c1 != c2) { eq = 0; break; }
		}
		if (eq) rv = 0;
		else    fbputs(&fout, buf2); 

		p = 0;
		while (p != 26 && p != '\n' && p != '\r')
		{
			p = fbgetc(&fin);

			if (p == 26) break;

			if (!eq) fbputc(&fout, p);
		}
		p = fbnextc(&fin);
		/* Copy & skip all newlines & carriage returns */
		while (p == '\r' || p == '\n')
		{
			p = fbgetc(&fin);
			if (!eq) fbputc(&fout, p);
			p = fbnextc(&fin);
		}
		/* Old value has been deleted & a new value is being inserted */
		if (eq && buffer)
		{
			fbputs(&fout, buf);	/* "app.prop:" */
			fbputc(&fout, ' ');
			fbputs(&fout, buffer);	/* value */
			fbputc(&fout, '\r');
			fbputc(&fout, '\n');
		}
		
	}
	if (rv == 1 && buffer)	/* Property is new */
	{
		fbputs(&fout, buf);	/* "app.prop:" */
		fbputc(&fout, ' ');
		fbputs(&fout, buffer);	/* value */
		fbputc(&fout, '\r');
		fbputc(&fout, '\n');
		rv = 0;
	}
	
	fbclose(&fin);
	fbclose(&fout);
	dos_free(buf);

	if (prop_ren()) return -1;

	return rv;
}

/************ Ancillary routines, not exposed *****************************/


MLOCAL WORD prop_open(FILEBUF *fb, WORD mode)
{
	LPBYTE pfile;

	rs_gaddr(ad_sysglo, R_STRING, mode ? STTMPPAT : STCFGPAT, (LPVOID *)&pfile);
	pfile[0] = D.s_cdir[0] ;		/* set the drive	*/
	
	return fbopen(pfile, fb, mode); 
}

MLOCAL WORD prop_ren()
{
	LPBYTE pfile1, pfile2;

	rs_gaddr(ad_sysglo, R_STRING, STCFGPAT, (LPVOID *)&pfile1);
	rs_gaddr(ad_sysglo, R_STRING, STTMPPAT, (LPVOID *)&pfile2);

	dos_delete(pfile1);
	dos_rename(pfile2, pfile1);

	return DOS_ERR ? -1 : 0;
}



/* Allocate the "program.section" string */

MLOCAL WORD prop_alloc(LPBYTE program, LPBYTE section)
{
	WORD l1, l2;

	l1 = LSTRLEN(program);
	l2 = LSTRLEN(section);

	len = l1 + l2 + 3;

	buf = (LPBYTE)dos_alloc(2*len); if (!buf) return -2;
	buf2 = buf + len;

	LBCOPY(buf, program, l1);		/* Construct the "program.section" string */
	buf[l1] = '.';
	LBCOPY(buf + l1 + 1, section, l2);
	buf[l1 + l2 + 1] = ':';
	buf[l1 + l2 + 2] = 0;
	return 0;
}



/*
 ***************************************************************************
 * 
 * Buffered file operations:
 *
 * fbopen() : Open a file. mode is 0 for read, 1 for write, 2 for create.
 *
 */

MLOCAL WORD fbopen(LPBYTE pfile, FILEBUF *fb, WORD mode)
{
	fb->mode  = mode;
	fb->nextc = 0;
	switch(mode)
	{
		case 0: 
		case 1: fb->f = dos_open  (pfile, mode); break;
		case 2: fb->f = dos_create(pfile, 0);    break;
	}
		
	if ( (!fb->f) || DOS_ERR)
	{
		fb->f = 0;
		return -1;
	}
	fb->pos = mode ? 0 : 128;	/* Buffer empty */
	return 0;
}

/* 
 *  Close a buffered file
 */

MLOCAL WORD fbclose(FILEBUF *fb)
{
	if (fb->mode && fb->pos)
	{
		dos_write(fb->f, fb->pos, fb->buf);

		if (DOS_ERR) 
		{
			dos_close(fb->f);
			return -1;
		}
	}
	dos_close(fb->f);

	return DOS_ERR ? -1 : 0;
}

/* 
 *  Read from a buffered file
 */
MLOCAL WORD fbgetc(FILEBUF *fb)
{
	WORD n, count;

	if (fb->nextc)
	{
		n = fb->nextc;
		fb->nextc = 0;
		return n;
	}
	
	if (fb->pos < 128) 
	{	
		return fb->buf[fb->pos++];
	}
	fb->pos = 0;
	count = dos_read(fb->f, 128, fb->buf);

	if (DOS_ERR) count = 0;

	for (n = count; n < 128; n++) fb->buf[n] = 26;	/* soft-EOF */

	fb->pos = 1;

	return fb->buf[0];
}

/* Peek at the next character, but don't advance file pointer */

MLOCAL WORD fbnextc(FILEBUF *fb)
{
	if (fb->nextc) return fb->nextc;

	fb->nextc = fbgetc(fb);

	return fb->nextc;
}



/* 
 *  Write to a buffered file
 */
MLOCAL WORD fbputc(FILEBUF *fb, BYTE ch)
{
	fb->buf[fb->pos++] = ch;
	if (fb->pos < 128) return 0;

	fb->pos = 0;
	dos_write(fb->f, 128, fb->buf);

	if (DOS_ERR) return -1;
	return 0;
}


/* Read a newline-terminated string. If newline is encountered, stop
 * so that newline will be the next character read. */

MLOCAL WORD fbgets(FILEBUF *fb, LPBYTE buf, WORD max)
{
	WORD ch, pos;

	for (pos = 0; pos < (max-1); pos++)
	{
		ch = fbnextc(fb);

		if (ch == '\r' || ch == '\n' || ch == 26) ch = 0;
		else ch = fbgetc(fb);
		
		buf[pos] = ch;
		
		if (!ch) break;
	}
	buf[max-1] = 0;
	
	return pos;	
}

/* Write a 0-terminated string */

MLOCAL WORD fbputs(FILEBUF *fb, LPBYTE buf)
{
	WORD pos=0, r;

	while (buf[pos])
	{
		r = fbputc(fb, buf[pos]); 
		if (r) return r;
		++pos;
	}
	return 0;
}

/***********************************************************************/
/* Routines for setting GUI properties at run time */

WORD prop_gui_set(WORD property, WORD value)
{
	WORD *opts = (WORD *)&gl_opts;

	if (property < 1 || property >= gl_opts.opt_count) return 0;

	if (property == 3) /* GUI compatibility */
	{
		if (value != 2) /* FreeGEM */ value = 0;
	}
	opts[property] = value;

	if (property == 2) /* 3D window frames */
	{
		if (value)
		{
			LBCOPY(ADDR(gl_waspec), ADDR(gl_waspec3d), sizeof(gl_waspec));
			LBCOPY(ADDR(gl_waflag), ADDR(gl_waflag3d), sizeof(gl_waflag));
		}
		else
		{
			LBCOPY(ADDR(gl_waspec), ADDR(gl_waspec2d), sizeof(gl_waspec));
			LBCOPY(ADDR(gl_waflag), ADDR(gl_waflag2d), sizeof(gl_waflag));
		}

	}
	
	return value;
}



WORD prop_gui_get(WORD property)
{
	WORD *opts = (WORD *)&gl_opts;
	if (property < 0 || property >= gl_opts.opt_count) return 0;

	return opts[property];
}


#endif /* PROPLIB */

