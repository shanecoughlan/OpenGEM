/*		DOWNFONT.C	12/16/86 - 2/5/87	J. Grant	*/

#include "portab.h"
#include "vdidef.alw"
#include "vdiext.alw"


MLOCAL	WORD		l_count;
MLOCAL	WORD		l_i;

MLOCAL	BYTE		l_file[66];

MLOCAL	BYTE		l_ascfnt[] = ".PFA";
MLOCAL	BYTE		l_binfnt[] = ".PFB";



	VOID
font_asc(handle)
	WORD		handle;
{
	WORD		count, i;

	while ((count = f_read(handle, filbuf, FILBUF_SIZE)) != 0) {
		for (i = 0; i < count; i++)
			char_out(filbuf[i]);
	}  /* End while:  data available. */
}  /* End "font_asc". */


	VOID
font_bin(handle)
	WORD		handle;
{
	WORD		i, k;
	BYTE		ch, type;
	LONG		length;

	/* Read in a buffer. */
	if (!font_buf(handle))
		return;

	k = 0;
	/* Loop until done. */
	FOREVER {
		/* Skip over the segment identifier byte and get the type */
		/* and length.                                            */
		if (l_i >= l_count)
			if (!font_buf(handle))
				break;
		l_i++;
		if (l_i >= l_count)
			if (!font_buf(handle))
				break;
		if ((type = filbuf[l_i++]) == 3)
			break;
		length = 0L;
		for (i = 0; i < 4; i++) {
			if (l_i >= l_count)
				if (!font_buf(handle))
					break;
			length += (LONG)filbuf[l_i++] << (i << 3);
		}  /* End for:  over length bytes. */

		/* Interpret and output the data. */
		for (i = 0; i < length; i++) {
			if (l_i >= l_count)
				if (!font_buf(handle))
					return;
			ch = filbuf[l_i++];
			if (type == 1) {
			    char_out(ch);
			    if (ch == '\r')
				char_out('\n');
		        }
			else {
			    outhex(ch);
			    if (++k == 80) {
				output("\n");
				k = 0;
			    }
			}
		}  /* End for:  over data length. */
	}  /* End FOREVER. */
}  /* End "font_bin". */


	BOOLEAN
font_buf(handle)
	WORD		handle;
{
	l_i = 0;
	return((l_count = f_read(handle, filbuf, FILBUF_SIZE)) != 0);
}  /* End "font_buf". */


	VOID
fontdown(index)
	WORD		index;
{
	WORD		handle;

	/* Restore state on the printer and reset internal status. */
	output("cleartomark restore gr gs save mark\n");
	init_status();
	set_clip();

	/* Find a font file.  The file name will appear in the DTA. */
	if (!fontfind(font_map[index].file))
		return;
	l_file[0] = '\0';
	s_cat(l_file, g_psfont);
	s_cat(l_file, &g_dta[30]);
	if ((handle = f_open(l_file)) != 0) {
		output("%%BeginFont: ");
		output(font_map[index].name);
		if (g_fftype == CN_FFBINARY)
			font_bin(handle);
		else
			font_asc(handle);
		f_close(handle);
		output("\n%%EndFont\n");
	}  /* End if:  file opened successfully. */
	font_map[index].attr |= CN_DOWNLOADED;
}  /* End "fontdown". */


	BOOLEAN
fontfind(name)
	BYTE		*name;
{
	/* Check for a binary file, then an ASCII file (if necessary). */
	l_file[0] = '\0';
	s_cat(l_file, g_psfont);
	s_cat(l_file, name);
	s_cat(l_file, l_binfnt);
	if (f_first(l_file)) {
		g_fftype = CN_FFBINARY;
		return(TRUE);
	}  /* End if:  binary file found. */
	l_file[0] = '\0';
	s_cat(l_file, g_psfont);
	s_cat(l_file, name);
	s_cat(l_file, l_ascfnt);
	g_fftype = CN_FFASCII;
	return(f_first(l_file));
}  /* End "fontfind". */
