/*		INCLUDE.C	12/21/86 - 4/28/87	J. Grant	*/

#include "portab.h"
#include "vdidef.alw"
#include "vdiext.alw"

#define	PS_VPTYPE	(1)
#define	PS_EPSFTYPE	(2)
#define	PS_WINTYPE	(3)
#define	SZ_BADSTRINGS	(5)

#define	BADSTRNG	struct bad_string

BADSTRNG
{
	WORD		length;
	BYTE		*string;
};


MLOCAL	WORD		l_count;
MLOCAL	WORD		l_end;
MLOCAL	WORD		l_handle;
MLOCAL	WORD		l_llx;
MLOCAL	WORD		l_lly;
MLOCAL	WORD		l_pstype;
MLOCAL	WORD		l_urx;
MLOCAL	WORD		l_ury;

MLOCAL	LONG		l_pssize;

MLOCAL	BADSTRNG	l_badstr[SZ_BADSTRINGS] =
{
	1, "\032",
	1, "\004",
	8, "showpage",
	8, "copypage",
	9, "erasepage",
};



	BOOLEAN
ps_atend()
{
	LONG		seek;
	BYTE		*ptr;

	/* Try to find the bounding box at the file end. */
	if (l_pstype == PS_WINTYPE)
		l_pssize = *((LONG *)&filbuf[8]);
	if ((seek = l_pssize - FILBUF_SIZE) < 0L)
		return(FALSE);
	f_seek(l_handle, seek, 0);
	l_count = l_end = f_read(l_handle, &filbuf[0], FILBUF_SIZE);
	if ((ptr = s_scan(&filbuf[0], l_count, "%%Trailer", 9)) == NULL)
		return(FALSE);
	if ((ptr = s_scan(ptr, l_count - (ptr - &filbuf[0]),
		"%%BoundingBox:", 14)) == NULL)
		return(FALSE);

	/* Get the bounding box. */
	ptr = asc2num(ptr, &l_llx);
	ptr = asc2num(ptr, &l_lly);
	ptr = asc2num(ptr, &l_urx);
	ptr = asc2num(ptr, &l_ury);

	/* Return to the beginning of the file. */
	f_seek(l_handle, 0L, 0);
	l_count = l_end = f_read(l_handle, &filbuf[0], FILBUF_SIZE);
	return(TRUE);
}  /* End "ps_atend". */


	BOOLEAN
ps_bbox()
{
	BYTE		*ptr;

	/* Bail out if no bounding box specified. */
	if ((ptr = s_scan(&filbuf[0], l_count,
		"%%BoundingBox:", 14)) == NULL)
		return(FALSE);

	/* Check for bounding box at end of file. */
	if (s_scan(ptr, 20, "(atend)", 7))
		return(ps_atend());

	/* Parse the remaining information. */
	ptr = asc2num(ptr, &l_llx);
	ptr = asc2num(ptr, &l_lly);
	ptr = asc2num(ptr, &l_urx);
	ptr = asc2num(ptr, &l_ury);
	return(TRUE);
}  /* End "ps_bbox". */


	VOID
ps_include()
{
	EXTERN VOID	ps_send();
	EXTERN VOID	ps_type();
	WORD		cx1, cx2, cy1, cy2, i, newsize, x300, x72, y300, y72;
	LONG		t1, t2;
	BYTE		filename[66];

	/* If this is not a PostScript file, ignore it. */
	if (intin[0] != 1)
		return;

	/* Open the file and read in the initial buffer. */
	for (i = 3; i < contrl[3]; i++)
		filename[i - 3] = intin[i];
	filename[i - 3] = '\0';
	if ((l_handle = f_open(filename)) == 0)
		return;
	l_pssize = f_seek(l_handle, 0L, 2);
	f_seek(l_handle, 0L, 0);
	l_count = l_end = f_read(l_handle, &filbuf[0], FILBUF_SIZE);

	/* Find out what type of PostScript include file this is and get */
	/* the bounding box.  Bail out if there are problems.            */
	ps_type();
	if ((l_pstype == 0) || !ps_bbox()) {
		f_close(l_handle);
		return;
	}  /* End if:  invalid file type. */

	/* Reset to the initial state and set up a save/restore context */
	/* for the include.                                             */
	output("cleartomark restore gr gs save mark\n");

	/* Calculate a new transformation. */
	x300 = (ptsin[2] + 1) - ptsin[0];
	y300 = (ptsin[3] + 1) - ptsin[1];
	x72 = l_urx - l_llx + 1;
	y72 = l_ury - l_lly + 1;
	if (intin[1] != 0) {
		t1 = (LONG)x72 * (LONG)y300;
		t2 = (LONG)y72 * (LONG)x300;
		if (t1 > t2) {
			newsize = muldiv(y72, x300, x72);
			ptsin[1] += (y300 - newsize) >> 1;
			ptsin[3] = ptsin[1] + newsize - 1;
			y300 = newsize;
		}  /* End if:  squeeze y. */
		else if (t1 < t2) {
			newsize = muldiv(x72, y300, y72);
			ptsin[0] += (x300 - newsize) >> 1;
			ptsin[2] = ptsin[0] + newsize - 1;
			x300 = newsize;
		}  /* End elseif:  squeeze x. */
	}  /* End if:  honor aspect ratio. */

	/* Intersect and send new clipping region. */
	if (ptsin[0] < ptsin[2]) {
		cx1 = ptsin[0];
		cx2 = ptsin[2];
	}  /* End if. */
	else {
		cx1 = ptsin[2];
		cx2 = ptsin[0];
	}  /* End else. */
	if (ptsin[1] < ptsin[3]) {
		cy1 = ptsin[1];
		cy2 = ptsin[3];
	}  /* End if. */
	else {
		cy1 = ptsin[3];
		cy2 = ptsin[1];
	}  /* End else. */
	if (g_x1clip < g_x2clip) {
		if (cx1 < g_x1clip)
			cx1 = g_x1clip;
		if (cx2 > g_x2clip)
			cx2 = g_x2clip;
	}  /* End if. */
	else {
		if (cx1 < g_x2clip)
			cx1 = g_x2clip;
		if (cx2 > g_x1clip)
			cx2 = g_x1clip;
	}  /* End else. */
	if (g_y1clip < g_y2clip) {
		if (cy1 < g_y1clip)
			cy1 = g_y1clip;
		if (cy2 > g_y2clip)
			cy2 = g_y2clip;
	}  /* End if. */
	else {
		if (cy1 < g_y2clip)
			cy1 = g_y2clip;
		if (cy2 > g_y1clip)
			cy2 = g_y1clip;
	}  /* End else. */
	outxy(cx1, cy1);
	outxy(cx1, cy2);
	outxy(cx2, cy2);
	outxy(cx2, cy1);
	output("np mto lto lto lto clip np\n");

	/* Send the transformation. */
	outxy(ptsin[0], ptsin[3]);
	output("translate 300 72 div dup scale\n");
	outdec(x300);
	output("300 div ");
	outdec(x72);
	output("72 div div ");
	outdec(y300);
	output("300 div ");
	outdec(y72);
	output("72 div div scale\n");
	outdec(-l_llx);
	outdec(-l_lly);
	output("translate\n");

	/* Send the file. */
	output("%%BeginFile: ");
	output(filename);
	output("\n");
	ps_send();
	output("\n%%EndFile\n");

	/* Close the file and restore the PostScript context. */
	f_close(l_handle);
	output("cleartomark restore gr gs save mark\n");
	init_status();
}  /* End "ps_include". */


	VOID
ps_send()
{
	WORD		amount, i, j, len;
	LONG		seek;
	BYTE		*pt;

	if (l_pstype == PS_WINTYPE) {
		l_pssize = *((LONG *)&filbuf[8]);
		seek = *((LONG *)&filbuf[4]);
		f_seek(l_handle, seek, 0);
		l_count = l_end = f_read(l_handle, &filbuf[0], FILBUF_SIZE);
	}  /* End if:  MS Windows EPSF type. */

	/* Process the file. */
	FOREVER {
		/* Replace naughty strings. */
		for (i = 0; i < SZ_BADSTRINGS; i++) {
			len = l_badstr[i].length;
			if ((pt = s_scan(&filbuf[1], l_end - 1,
				l_badstr[i].string, len)) != 0) {
				pt -= len;
				if (len == 1 || (*(pt - 1) <= ' ' &&
					*(pt + len) <= ' ')) {
					for (j = 0; j < len; j++)
						*pt++ = ' ';
					i--;
				}  /* End if:  bad string. */
			}  /* End if:  maybe bad string. */
		}  /* End for:  over bad strings. */

		/* GEM_INIDOC is special:  substitute GEM_INISUB. */
		if ((pt = s_scan(&filbuf[1], l_end - 1, "GEM_INIDOC", 9))
		    != 0) {
			if (*(pt - 10) != '/') {
				pt -= 3;
				*pt++ = 'S';
				*pt++ = 'U';
				*pt++ = 'B';
			}  /* End if:  substitute. */
		}  /* End if:  GEM_INIDOC found. */

		/* The amount of data output depends on how much is left */
		/* in the file to process.                               */
		l_pssize -= l_count;
		amount = l_pssize > 0L ? l_end - 10 : l_end;

		/* Output the data and quit if done. */
		for (i = 0; i < amount; i++)
			char_out(filbuf[i]);
		if (l_pssize <= 0L)
			break;

		/* Copy the last ten bytes of data to the beginning */
		/* of the input buffer.                             */
		for (i = 0, j = l_end - 10; i < 10; i++, j++)
			filbuf[i] = filbuf[j];

		/* Read the next buffer. */
		l_count = f_read(l_handle, &filbuf[10], FILBUF_SIZE - 10);
		if (l_count == 0) {
			for (i = 0; i < l_end; i++)
				char_out(filbuf[i]);
			break;
		}  /* End if:  no data. */
		l_end = l_count + 10;
	}  /* End FOREVER. */
}  /* End "ps_send". */


	VOID
ps_type()
{
	if (filbuf[0] == 0xc5 && filbuf[1] == 0xd0 &&
		filbuf[2] == 0xd3 && filbuf[3] == 0xc6)
		l_pstype = PS_WINTYPE;
	else if (s_scan(&filbuf[0], l_count,
		"%%Creator: GEM", 14))
		l_pstype = PS_VPTYPE;
	else if (s_scan(&filbuf[0], l_count,
		"%!PS-Adobe-", 11))
		l_pstype = PS_EPSFTYPE;
	else
		l_pstype = 0;
}  /* End "ps_type". */
