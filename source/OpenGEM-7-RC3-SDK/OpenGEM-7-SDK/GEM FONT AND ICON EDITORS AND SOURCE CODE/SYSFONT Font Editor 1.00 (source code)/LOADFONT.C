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

/* Load/save font files */

#include "sysfonti.h"


typedef struct filefont
{
	word minade;
	word maxade;
	byte height;
	word charw;
	word width;
	long fpos;
} FILEFONT;

static word peek16(byte *b)
{
	return (((word)b[1]) << 8) | b[0];
}


static unsigned long peek32(byte *b)
{
	return (((unsigned long)b[3]) << 24) |
	       (((unsigned long)b[2]) << 16) |
	       (((unsigned long)b[1]) <<  8) | b[0];
}

static char errbuf[512];

void syserror()
{
	sprintf(errbuf, "[3][%s][ Stop ]", strerror(errno));
	form_alert(1, errbuf);	
}


FONTSET *load_psf(FILE *fp)
{
	byte header[4];
	word nsets;
	byte height[2];
	int size;
	FONTSET *set;
	
	if (fread(header, 1, 4, fp) < 4) 
	{ 
		syserror(); 
		fclose(fp); 
		return NULL;
	}
	height[0] = height[1] = header[3];
	nsets = (header[2] & 1) ? 2 : 1;
	size = height[0] * 256;	
	set = alloc_fontset(nsets, height);
	if (!set) 
	{
		rsrc_alert(ALNOMEM, 1); 
		fclose(fp); 
		return NULL; 
	}
	if (fread(set->font[0].data, 1, size, fp) < size) syserror();
	else if (nsets > 1)
	{
		if (fread(set->font[1].data, 1, size, fp) < size) syserror();
	}
	fclose(fp);
	set->format = 'P';
	return set;	
}


static byte zx_header[128] =
{
	 'P',  'L',  'U',  'S',  '3',  'D',  'O',  'S',
	0x1a, 0x01, 0x00, 0x80, 0x03, 0x00, 0x00, 0x03,
	0x00, 0x03, 0x00, 0x3D
};

FONTSET *load_zx(FILE *fp)
{
	byte header[128];
	FONTSET *set;
	byte height = 8;
	
	if (fread(header, 1, 128, fp) < 128) 
	{ 
		syserror(); 
		fclose(fp); 
		return NULL;
	}
	if (memcmp(header + 11, zx_header + 11, 7))
	{
		rsrc_alert(ALZXFON, 1);
		fclose(fp);
		return NULL;
	}
	set = alloc_fontset(1, &height);
	if (!set) 
	{
		rsrc_alert(ALNOMEM, 1); 
		fclose(fp); 
		return NULL; 
	}
	if (fread(set->font[0].data + 256, 1, 768, fp) < 768) syserror();
	fclose(fp);
	set->format = 'Z';	
	return set;
}


void load_bitmap(FILE *fp, FONTSET *set, int n, FILEFONT *ff)
{
	static byte row[256];
	int y, x;
	int ptr;
	byte d;

	fseek(fp, ff->fpos, SEEK_SET);
	for (y = 0; y < ff->height; y++)
	{
		if (fread(row, 1, ff->width, fp) < ff->width) return;
		ptr = 0; 
		for (x = ff->minade; x < 256; x++)
		{
			if (ptr >= ff->width) d = 0;
			else if (ff->charw < 8) switch(x % 4)
			{
				case 0: d = row[ptr] & 0xFC; break;
				case 1: d  = ((row[ptr] & 3) << 6);
					d |= ((row[ptr+1] & 0xF0) >> 2); break;
				case 2: d  = ((row[ptr+1] & 0x0F) << 4);
					d |= ((row[ptr+2] & 0xC0) >> 4); break;
				case 3: d  = ((row[ptr+2] & 0x3F) << 2);
					ptr += 3; 
					break;
			}
			else d = row[ptr++];
			set->font[n].data[x * ff->height + y] = d;
		}	
	}
}

int merge_bitmap(FILE *fp, FONTSET *set, FILEFONT *ff)
{
	static byte row[256];
	int y, x;
	int ptr;
	byte d;
	int nf;

	for (nf = 0; nf < set->nfonts; nf++)
	{
		if (ff->height == set->font[nf].height) break;
	}
	if (nf >= set->nfonts) return 0;	/* No match found */

	fseek(fp, ff->fpos, SEEK_SET);
	for (y = 0; y < ff->height; y++)
	{
		ptr = 0; 
		memset(row, 0, sizeof(row));
		for (x = ff->minade; x < 256; x++)
		{
			d = set->font[nf].data[x * ff->height + y];

			if (ptr >= ff->width) continue;
			if (ff->charw < 8) switch(x % 4)
			{
				case 0: row[ptr]   |=  d & 0xFC; 	 break;
				case 1: row[ptr]   |= ((d & 0xC0) >> 6);
					row[ptr+1] |= ((d & 0x3C) << 2); break;
				case 2: row[ptr+1] |= ((d & 0xF0) >> 4);
					row[ptr+2] |= ((d & 0x0C) << 4); break;
				case 3: row[ptr+2] |= ((d & 0xFC) >> 2); 
					ptr += 3; 
					break;
			}
			else row[ptr++] = d;
		}	
		if (fwrite(row, 1, ff->width, fp) < ff->width) return 0;
	}
	return 1;
}

int find_fonts(FILE *fp, FILEFONT *ffnt)
{
	static byte exe_buf[88];
	int n, nf;
	long exe_hdr_len;
	
	if (fread(exe_buf, 1, sizeof(exe_buf), fp) < (int)sizeof(exe_buf)) 
		return 0;
	exe_hdr_len = 16 * exe_buf[8] + 4096 * exe_buf[9];

	nf = 0;
	do
	{
		for (n = 0; n < (int)sizeof(exe_buf)-1 ; n++) 
		{
			exe_buf[n] = exe_buf[n+1];
		}
		n = fgetc(fp);
		if (n == EOF) break;
		exe_buf[sizeof(exe_buf) - 1] = n;	
		if (!strcmp((char *)exe_buf + 4, "System") 
		||  !strcmp((char *)exe_buf + 4, "SYSTEM"))	// System font hdr
		{
			ffnt[nf].height = exe_buf[82];
			ffnt[nf].minade = exe_buf[36];
			ffnt[nf].maxade = exe_buf[38];
			ffnt[nf].charw  = exe_buf[52];
			ffnt[nf].width  = exe_buf[80] + 256 * exe_buf[81];
			ffnt[nf].fpos   = (exe_buf[76] & 0xFF) 
				   + ((((unsigned)exe_buf[77]) << 8) & 0xFF00)
				   + ((((unsigned)exe_buf[78]) << 4) & 0x0FF0)
  				   + ((((unsigned)exe_buf[79]) << 12)& 0xF000);
			ffnt[nf].fpos += exe_hdr_len;
			++nf;					
		}
	} while (nf < 4);
	return nf;
}


FONTSET *load_sys(FILE *fp)
{
	FONTSET *set;
	FILEFONT ffnt[4];
	byte height[4];
	int n;
	int nf = find_fonts(fp, ffnt);

	if (!nf) { rsrc_alert(ALNOFONT,1); fclose(fp); return NULL; }

	for (n = 0; n < 4; n++) height[n] = ffnt[n].height;
	set = alloc_fontset(nf, height);
	if (!set) { rsrc_alert(ALNOMEM,1); fclose(fp); return NULL; }
		
	for (n = 0; n < nf; n++)
	{
		load_bitmap(fp, set, n, &ffnt[n]);
	}	
	fclose(fp);
	set->format = 'E';	/* EXE */
	return set;
}	

int merge_sys(FILE *fp, FONTSET *set)
{
	FILEFONT ffnt[4];
	int n, count;
	int nf = find_fonts(fp, ffnt);

	if (!nf) { rsrc_alert(ALNOFONT,1); fclose(fp); return 0; }
		
	for (n = count = 0; n < nf; n++)
	{
		count += merge_bitmap(fp, set, &ffnt[n]);
	}	
	fclose(fp);
	return count;
}


FILEFONT *tos_filefonts()
{
	static int done = 0;
	int n;
	static byte height[] = { 6, 8, 16 };
	static byte charw[]  = { 6, 8, 8  };
	static word width[]  = {192,256, 256};
	static long fpos[]   = { 0x12826L, 0x12F02L, 0x1395EL };
	static FILEFONT tos_ffnt[3];
	
	if (done) return tos_ffnt;
	for (n = 0; n < 3; n++)
	{
		tos_ffnt[n].minade = 0;
		tos_ffnt[n].maxade = 255;
		tos_ffnt[n].height = height[n];
		tos_ffnt[n].charw  = charw[n];
		tos_ffnt[n].width  = width[n];
		tos_ffnt[n].fpos   = fpos[n];
	}
	done = 1;
	return tos_ffnt;
}


FONTSET *load_tos(FILE *fp)
{
	int n;
	FONTSET *set;
	static byte height[] = { 6, 8, 16 };
	FILEFONT *ff = tos_filefonts();

	set = alloc_fontset(3, height);
	if (!set) { rsrc_alert(ALNOMEM,1); fclose(fp); return NULL; }
	for (n = 0; n < 3; n++)
	{
		load_bitmap(fp, set, n, &ff[n]);
	}	
	set->format = 'A';	/* Atari */
	return set;
}


int merge_tos(FILE *fp, FONTSET *set)
{
	int n, count;
	FILEFONT *ff = tos_filefonts();

	for (n = count = 0; n < 3; n++)
	{
		count += merge_bitmap(fp, set, &ff[n]);
	}
	fclose(fp);
	return count;
}


byte msw_font_header[128];

/* Check for a Windows-style NE header - if there is one, this might be
 * a Windows font-resource file. */
static int check_ne(FILE *fp, byte *header)
{
	word reltable;
	unsigned long ne_offset;

	reltable = peek16(header + 0x18);
	if (reltable < 0x40) return 0;

	ne_offset = peek32(header + 0x3C);

	fseek(fp, ne_offset, SEEK_SET);	
	if (fread(header + 0x40, 1, 0x40, fp) < 0x40) return 0;
	if (header[0x40] != 'N' || header[0x41] != 'E') return 0;
	return 1;	
}

/* The first 64 bytes of "header" are the MZ header. The seond 64 are the 
 * NE header. */
static int ne_filefonts(FILE *fp, byte *header, FILEFONT *ff)
{
	unsigned long ne_offset, rdir_begin, font_offsets[4];
	int font_index = 0;
	word rsrc_offset;
	word rsrc_align;
	byte rdir_typeinfo[8];
	byte rdir_nameinfo[12];
	int ch, nfonts = 0;
	word nf, rt_type, rt_count;

	ne_offset = peek32(header + 0x3C);

	rsrc_offset = peek16(header + 0x64);
	fseek(fp, ne_offset + rsrc_offset, SEEK_SET);
/* Load the resource root directory */
	ch = fgetc(fp); if (ch == EOF) { syserror(); return 0; }
	rsrc_align = ch;
	ch = fgetc(fp); if (ch == EOF) { syserror(); return 0; }
	rsrc_align |= (ch << 8);
/* Offset of resource root dir */
	rdir_begin = ftell(fp);	
	do
	{
		if (fread(rdir_typeinfo, 1, 8, fp) < 8) { syserror(); return 0; }
		rt_type = peek16(rdir_typeinfo);
		if (rt_type) rt_count = peek16(rdir_typeinfo + 2);
			
		if (rt_type == 0x8008 && font_index < 4) // RT_FONT | 0x8000
		{
			for (nf = 0; nf < rt_count; nf++)
			{
				if (fread(rdir_nameinfo, 1, 12, fp) < 12) 
				{ 
					syserror(); 
					return 0; 
				}
				font_offsets[font_index++] = 
					((unsigned long)peek16(rdir_nameinfo)) << rsrc_align;
 
			}
		}
		else if (rt_type)
		{
/* Skip over the NAMEINFOs for this resource */
			fseek(fp, 12 * rt_count, SEEK_CUR);
		}
	} while (peek16(rdir_typeinfo));
/* OK. We have up to 4 possible fonts */
	for (ch = 0; ch < font_index; ch++)
	{
		fseek(fp, font_offsets[ch], 0);
		if (fread(msw_font_header, 1, 128, fp) < 128) continue;
		if (peek16(msw_font_header + 0x56) != 8) continue;
		/* Height */
		ff[nfonts].height = peek16(msw_font_header + 0x58);
		ff[nfonts].charw  = 8;
		ff[nfonts].minade = msw_font_header[0x5F];
		ff[nfonts].maxade = msw_font_header[0x60];
		ff[nfonts].width  = peek16(msw_font_header + 0x63);
		ff[nfonts].fpos   = font_offsets[ch];
		++nfonts;
	}
	if (nfonts == 0) rsrc_alert(ALNOFON,1);

	return nfonts;
} 

static byte offsets[2048];
/* The first 64 bytes of "header" are the MZ header. The seond 64 are the 
 * NE header. */
static FONTSET *load_ne(FILE *fp, byte *header)
{
	FILEFONT ffnt[4];
	FONTSET *set;
	int n, max, y, x;
	static byte height[4];
	
	max = ne_filefonts(fp, header, ffnt);
	if (!max) { fclose(fp); return NULL; }

	for (n = 0; n < 4; n++) height[n] = ffnt[n].height;
	set = alloc_fontset(max, height);
	if (!set) { rsrc_alert(ALNOMEM,1); fclose(fp); return NULL; }
	set->format = 'W';
/* ffnt now points to up to 4 Windows font headers. */
	for (n = 0; n < max; n++)
	{
		fseek(fp, ffnt[n].fpos, 0);
		if (fread(msw_font_header, 1, 128, fp) < 128) continue;
		switch(peek16(msw_font_header))
		{
/* The Windows 3.x format is the same as 2.x except that the offsets 
 * table has 6 bytes per entry, not 4. */
			case 0x300:
/* Load the offsets table, and use it to find the characters */
				fseek(fp, ffnt[n].fpos + 0x9A, SEEK_SET);
				for (x = ffnt[n].minade; x <= ffnt[n].maxade; x++)
				{
					if (fread(offsets + 6 * x, 1, 6, fp) < 6)
					{
						syserror();
						fclose(fp);
						free_fontset(set);
						return set;
					}
				}
				for (x = ffnt[n].minade; x <= ffnt[n].maxade; x++)
				{
					fseek(fp, ffnt[n].fpos + peek32(offsets + 6*x + 2), SEEK_SET);
					for (y = 0; y < ffnt[n].height; y++)
						set->font[n].data[x * ffnt[n].height+ y] = fgetc(fp);
				}
				break;
			case 0x200:
/* Load the offsets table, and use it to find the characters */
				fseek(fp, ffnt[n].fpos + 0x76, SEEK_SET);
				for (x = ffnt[n].minade; x <= ffnt[n].maxade; x++)
				{
					if (fread(offsets + 4 * x, 1, 4, fp) < 4)
					{
						syserror();
						fclose(fp);
						free_fontset(set);
						return set;
					}
				}
				for (x = ffnt[n].minade; x <= ffnt[n].maxade; x++)
				{
					fseek(fp, ffnt[n].fpos + peek16(offsets + 4*x + 2), SEEK_SET);
					for (y = 0; y < ffnt[n].height; y++)
						set->font[n].data[x * ffnt[n].height+ y] = fgetc(fp);
				}
				break;
			case 0x100:
				fseek(fp, ffnt[n].fpos + 0x76, SEEK_SET);
/* Windows 1.x has all the bytes in a form, like GEM. There's no offset table. */
				for (y = 0; y < ffnt[n].height; y++)
				    for (x = ffnt[n].minade; x <= ffnt[n].maxade; x++)
				{
					set->font[n].data[x * ffnt[n].height+ y] = fgetc(fp);
				} 
				break;
		}
	}

	fclose(fp);
	return set;
}



FONTSET *load_font(FILE *fp)
{
	byte header[128];
	long size;
	byte height;
	FONTSET *set;

	/* Supported font types: PSF EXE RAW ZX Atari ROM 
	 * All except RAW have magic numbers, so... */
	memset(header, 0, sizeof header);
	fread(header, 1, 128, fp);
	fseek(fp, 0, SEEK_SET);

	if (header[0] == 'M'  && header[1] == 'Z' ) 
	{
		if (check_ne(fp, header)) return load_ne(fp, header);
		return load_sys(fp);
	}
	if (header[0] == 0x60 && header[1] == 0x1E) return load_tos(fp);
	if (header[0] == 0x36 && header[1] == 4   ) return load_psf(fp);
	if (!memcmp(header, "PLUS3DOS\032", 9)) return load_zx(fp);
	/* Check for PSF2 */
	if (header[0] == 0x72 && header[1] == 0xb5 && 
	    header[2] == 0x4a && header[3] == 0x86)
	{
		rsrc_alert(ALPSF2,1); 
		fclose(fp); 
		return NULL;
	}
		
	/* Load as raw */
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	height = size / 256;
	set = alloc_fontset(1, &height);
	if (!set) { rsrc_alert(ALNOMEM,1); fclose(fp); return NULL; }
	if (fread(set->font[0].data, 1, size, fp) < size) syserror();
	fclose(fp);
	set->format = 'R';	
	return set;
}


/* The first 64 bytes of "header" are the MZ header. The seond 64 are the 
 * NE header. */
static int merge_ne(FILE *fp, FONTSET *set)
{
	FILEFONT ffnt[4];
	int n, max, y, x;
	static byte header[128];

	fseek(fp, 0, SEEK_SET);
	if (fread(header, 1, 64, fp) < 64 || !check_ne(fp, header)) 
	{ 
		rsrc_alert(ALNOFON,1);
		fclose(fp);
		return 0;
	}
	
	max = ne_filefonts(fp, header, ffnt);
	if (!max) { fclose(fp); return 0; }

/* ffnt points to up to 4 Windows font headers. */
	for (n = 0; n < max; n++)
	{
		fseek(fp, ffnt[n].fpos, 0);
		if (fread(msw_font_header, 1, 128, fp) < 128) continue;
		switch(peek16(msw_font_header))
		{
/* The Windows 3.x format is the same as 2.x except that the offsets 
 * table has 6 bytes per entry, not 4. */
			case 0x300:
/* Load the offsets table, and use it to find the characters */
				fseek(fp, ffnt[n].fpos + 0x9A, SEEK_SET);
				for (x = ffnt[n].minade; x <= ffnt[n].maxade; x++)
				{
					if (fread(offsets + 6 * x, 1, 6, fp) < 6)
					{
						syserror();
						fclose(fp);
						return 0;
					}
				}
				for (x = ffnt[n].minade; x <= ffnt[n].maxade; x++)
				{
					fseek(fp, ffnt[n].fpos + peek32(offsets + 6*x + 2), SEEK_SET);
					for (y = 0; y < ffnt[n].height; y++)
						fputc(set->font[n].data[x * ffnt[n].height+ y], fp);
				}
				break;
			case 0x200:
/* Load the offsets table, and use it to find the characters */
				fseek(fp, ffnt[n].fpos + 0x76, SEEK_SET);
				for (x = ffnt[n].minade; x <= ffnt[n].maxade; x++)
				{
					if (fread(offsets + 4 * x, 1, 4, fp) < 4)
					{
						syserror();
						fclose(fp);
						return 0;
					}
				}
				for (x = ffnt[n].minade; x <= ffnt[n].maxade; x++)
				{
					fseek(fp, ffnt[n].fpos + peek16(offsets + 4*x + 2), SEEK_SET);
					for (y = 0; y < ffnt[n].height; y++)
						fputc(set->font[n].data[x * ffnt[n].height+ y], fp);
				}
				break;
			case 0x100:
				fseek(fp, ffnt[n].fpos + 0x76, SEEK_SET);
/* Windows 1.x has all the bytes in a form, like GEM. There's no offset table. */
				for (y = 0; y < ffnt[n].height; y++)
				    for (x = ffnt[n].minade; x <= ffnt[n].maxade; x++)
				{
					fputc(set->font[n].data[x * ffnt[n].height+ y], fp);
				} 
				break;
		}
	}

	fclose(fp);
	return 1;
}



int save_raw(FONTSET *f, FILE *fp)
{
	if (fwrite(f->font[0].data, f->font[0].height, 
		   256, fp) < 256) 
	{ 
		syserror(); 
		fclose(fp); 
		return 0; 
	}
	return 1;
}


int save_psf(FONTSET *f, FILE *fp)
{
	byte err = 0;
	byte header[4];

	header[0] = 0x36;
	header[1] = 4;
	header[2] = (f->nfonts > 1) ? 1 : 0;
	header[3] = f->font[0].height;

	if (f->font[0].height != f->font[1].height)
	{
		header[2] = 0;
	}
	
	if (fwrite(header, 1, 4, fp) < 4) err = 1;
	else if (fwrite(f->font[0].data, f->font[0].height, 256, fp) < 256) 
		err = 1;
	else if (header[2])
	{
		if (fwrite(f->font[1].data, f->font[1].height, 256, fp) < 256)
			err = 1;
	}
	if (err) syserror();
	return (err == 0);
}

int  save_zx(FONTSET *f, FILE *fp)
{
	int n, okay = 1;
	byte ck;

	if (f->font[0].height != 8)
	{
		if (rsrc_alert(ALZXFON2,2) != 1) { fclose(fp); return 0;	}
	}
	for (n = 0, ck = 0; n < 127; n++) ck += zx_header[n];
	zx_header[n] = ck;
	if (fwrite(zx_header, 1, 128, fp) < 128) okay = 0;
	for (n = 32; n < 128; n++)
	{
		if (fwrite(f->font[0].data + (n*f->font[0].height), 1, 8, fp) < 8)
			okay = 0;
	}
	if (!okay) syserror();
	return okay;
}




int save_font(FONTSET *f, FILE *fp)
{
	switch(f->format)
	{
		case 'E': return merge_sys(fp, f); 
		case 'W': return merge_ne(fp, f); 
		case 'A': return merge_tos(fp, f); 
		case 'P': return save_psf(f, fp); 
		case 'R': return save_raw(f, fp); 
		case 'Z': return save_zx (f, fp); 
	}
	rsrc_alert(ALFORMAT,1);
	return 0;
}

