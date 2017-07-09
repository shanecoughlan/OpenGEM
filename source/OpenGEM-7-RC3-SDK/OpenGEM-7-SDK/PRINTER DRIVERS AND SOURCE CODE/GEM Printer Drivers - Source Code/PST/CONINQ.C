/*		CONINQ.C	11/17/86 - 3/30/87	J. Grant	*/

#include "portab.h"
#include "vdidef.alw"
#include "vdiext.alw"
#include "vdifunc.alw"


MLOCAL	BYTE		l_psfont[9] = "..\\FONTS\\";

MLOCAL	BYTE		type_tbl[7] = {2, 2, 2, 1, 1, 3, 0};
MLOCAL	BYTE		port_tbl[7] = {0, 1, 2, 0, 1, 0, 0};


    VOID
clear_wk()
{
	EXTERN VOID	form_advance();

	/* Force imaging, if necessary. */
	form_advance();
}  /* End "clear_wk". */


    VOID
close_wk()
{
	/* If imaging is pending, force output.  Send out an end-of-job. */
	if (pending)
		output("showpage ");
	abort = FALSE;
	output("cleartomark restore end\n%%Trailer\n%%Pages: ");
	outdec(g_copies*g_pages);
	output("\n%%EOF\n");
	if (g_outeof)
		output("\004");

	/* De-initialize I/O. */
	dinit_io();
}  /* End "close_wk". */


	VOID
config()
{
	WORD		count, handle;
	BYTE		ch;
	BYTE		*dst, *src;
	BYTE		path[66];

	g_psfont[0] = '\0';
	g_outeof = TRUE;
	g_fstimg = TRUE;
	g_fontset = FS_LW;
	marg_xl = PT_2_ABS(18);
	marg_xr = PT_2_ABS(18);
	marg_yt = PT_2_ABS(18);
	marg_yb = PT_2_ABS(18);
	fullpath(path, "PST*.CNF");
	if (f_first(path))
	    fullpath(path, &g_dta[30]);
	if ((handle = f_open(path)) != 0) {
		if ((count = f_read(handle, filbuf, FILBUF_SIZE)) > 8) {
			/* Take care of downloadable font directory. */
			dst = g_psfont;
			if ((src = s_scan(filbuf, count, "psfonts(", 8))
			    != NULL) {
				while ((ch = *src++) != ')')
					*dst++ = ch;
				if (*(dst - 1) != '\\')
					*dst++ = '\\';
			}  /* End if:  string found. */
			*dst = '\0';

			/* Take care of end-of-file type. */
			if ((src = s_scan(filbuf, count, "eoftype(", 8))
			    != NULL)
				g_outeof = !((*src & 0xdf) == 'M' &&
					(*(src + 1) & 0xdf) == 'A' &&
					(*(src + 2) & 0xdf) == 'C');

			/* Take care of image type. */
			if ((src = s_scan(filbuf, count, "imgtype(", 8))
			    != NULL)
				g_fstimg = !((*src & 0xdf) == 'C' &&
					(*(src + 1) & 0xdf) == 'O' &&
					(*(src + 2) & 0xdf) == 'M' &&
					(*(src + 3) & 0xdf) == 'P' &&
					(*(src + 4) & 0xdf) == 'A' &&
					(*(src + 5) & 0xdf) == 'C' &&
					(*(src + 6) & 0xdf) == 'T');

			/* Margins. */
			if ((src = s_scan(filbuf, count, "margins(", 8))
			    != NULL) {
			    src = asc2num(src, &marg_xl);
			    marg_xl = PT_2_ABS(marg_xl);
			    src = asc2num(src, &marg_xr);
			    marg_xr = PT_2_ABS(marg_xr);
			    src = asc2num(src, &marg_yt);
			    marg_yt = PT_2_ABS(marg_yt);
			    src = asc2num(src, &marg_yb);
			    marg_yb = PT_2_ABS(marg_yb);
			}
			/* Resident font set. */
			if ((src = s_scan(filbuf, count, "resfont(", 8))
			    != NULL)
				if ((src[0] & 0xdf) == 'L' &&
				    (src[1] & 0xdf) == 'W' &&
				    src[2] == '+')
					g_fontset = FS_LWPLUS;

		}  /* End if:  data available. */
		f_close(handle);
	}  /* End if:  file open successful. */
	if (g_psfont[0] == '\0') {
		fullpath(g_psfont, l_psfont);
	}  /* End if:  use default. */
}  /* End "config". */


	VOID
dev_fonts()
{
	WORD		i, j;
	BOOLEAN		no_download;
	BYTE		name[66], file[66];

	if (contrl[3] <= 5 || g_maxface >= MAX_T_FACES ||
	    g_ffmax >= FFNAME_SIZE)
		return;
	i = 0;
	j = 5;
	while (j < contrl[3] && intin[j] != '/' && intin[j] != 0)
	    name[i++] = intin[j++];
	name[i] = '\0';
	i = 0;
	if (j < contrl[3] && intin[j++] == '/')
	    while (j < contrl[3] && intin[j] != 0)
		file[i++] =  intin[j++];
	file[i] = '\0';

	no_download = (intin[0] & 0x80) != 0 || file[0] == '\0'
	    || !fontfind(file);

	font_enter(name, no_download ? NULL : file, intin[1], intin[3],
	    !intin[4]);
}  /* End "dev_fonts". */


    VOID
escape()
{
	EXTERN VOID	form_advance();
	EXTERN VOID	q_scan();

	/* Branch to the appropriate routine, based on sub-opcode. */
	switch (contrl[5]) {

		case 20:  /* Form advance. */
			form_advance();
			break;
		case 21:  /* Output window. */
			output_window();
			break;
		case 22:  /* Clear display list (output pages). */
/*			form_advance();*/
			break;

		case 23:   /* Output bit image file. */
		case 101:  /* Output rotated bit image file. */
			do_image();
			break;

		case 24:  /* Inquire printer scan heights. */
			q_scan();
			break;

		case 25:  /* alpha text */
			alpha_text ();
			break;

		case 26:  /* Device specific font data */
			dev_fonts();
			break;

		case 27:  /* Set printer page orientation. */
			g_orient = intin[0];
			break;

		case 28:  /* Set number of copies. */
			g_copies = intin[0] > 0 ? intin[0] : 1;
			break;

		case 29:  /* Set paper tray. */
			g_tray = intin[0];
			break;

		case 30:  /* Include print file. */
			ps_include();
			break;

		case 31:  /* Squeeze page to fit. */
			if (intin[1] == 0)
				g_squeez = (intin[0] == 50 ? 1 : 0) +
					(intin[0] == 55 ? 2 : 0);
			break;
	}  /* End switch. */
}  /* End "escape". */


    VOID
form_advance()
{
	extern VOID	alpha_advance ();

	/* If imaging is pending, force the image out. */
	if (pending) {
		output("%End page\nshowpage cleartomark restore gr\n");
		output("gs save mark\n%Begin page\nUserSoP\n");
		alpha_advance ();

		pending = FALSE;
		g_pages++;
	}  /* End if: image is pending. */

	/* Since a VM restore was performed, reset the internal device */
	/* status variables.                                           */
	init_status();
}  /* End "form_advance". */


    VOID
load_fonts()
{
	contrl[4] = 1;
	intout[0] = 0;
}  /* End "load_fonts". */


    VOID
open_wk()
{
	WORD		df_ioinf, i;
	BYTE		*ptr;

	config();

	/* Initialize wired-in font names. */
	g_ffmax = 0;
	init_wired();

	/* Get default port information. */
	df_ioinf = df_pport();
	io_type = type_tbl[df_ioinf];
	io_port = port_tbl[df_ioinf];

	/* Take care of I/O port specification, if provided. */
	if (contrl[3] > 11) {
		i = (intin[11] >> 8) & 0xff;
		if (i == 255) {
		    dev_tab[0] = intin[101] * 3 - 1;
		    dev_tab[1] = intin[102] * 3 - 1;
		} else {
		    if (i < 0 || i > 59)
			i = 0;
		    dev_tab[0] = page_tab[i][0];
		    dev_tab[1] = page_tab[i][1];
		}
		if ((intin[11] & 0xff) != 0xff) {
		    switch (io_type = (intin[11] & 0xff)) {
			case 0:		/* file */
				ptr = io_fname;
				for (i = 12; i < contrl[3]; i++)
					*ptr++ = (BYTE)(intin[i]);
				break;
			case 1:		/* serial port */
			case 2:		/* parallel port*/
				io_port = (BYTE)(intin[12]);
				break;
			case 3:		/* device-specific (direct) */
				break;
		    }  /* End switch:  over I/O type. */
		}
	}  /* End if:  I/O information specified. */
	dev_tab[0] -= marg_xl + marg_xr;
	dev_tab[1] -= marg_yt + marg_yb;
	y_max = dev_tab[1];

	/* Perform initializations.  If bogus information is returned by */
	/* the device, return a handle of zero.                          */
	init_io();
	if ((abort == 0) && init_dev()) {
		init_data();
	}  /* End if:  successful initialization. */
	else
		contrl[6] = 0;
	g_pages = g_orient = g_tray = 0;
	g_copies = 1;
	g_endpre = TRUE;
	g_squeez = 0;
}  /* End "open_wk". */

    VOID
q_extended()
{
	WORD		i;
	WORD		*ipt, *ppt;

	/* Copy the requested information. */
	flip_y = TRUE;
	contrl[2] = 6;
	contrl[4] = 45;
	if (intin[0]) {
		ipt = inq_tab;
		ppt = inq_pts;
	}  /* End if:  extended info desired. */

	else {
		ipt = dev_tab;
		ppt = siz_tab;
	}  /* End else:  open workstation values desired. */

	for (i = 0; i < 12; i++)
		ptsout[i] = *ppt++;
	for (i = 0; i < 45; i++)
		intout[i] = *ipt++;	
}  /* End "q_extended". */


    VOID
q_scan()
{
	contrl[4] = 7;
	intout[0] = 1;
	intout[1] = dev_tab[1] + 1;
	intout[2] = 50;
	intout[3] = (dev_tab[1] + 1) / 50;
	intout[4] = 1;
	intout[5] = 30;
	intout[6] = 80;
}  /* End "q_scan". */

    VOID
update_wk()
{
/*	pending = TRUE; */
}  /* End "update_wk". */

    VOID
vqt_name()
{
	BYTE	*ptr;
	WORD	index, i;

	/* Return the requested face index and the associated name. */
	index = intin[0] - 1;
	if (index < 0 || index >= g_maxface) {
	    intout[0] = 0;
	    contrl[4] = 1;
	}

	intout[0] = font_map[index].face;
	ptr = font_map[index].name;
	i = 1;
	while ((intout[i++] = *ptr++) != 0);
	contrl[4] = i - 1;
}  /* End "vqt_name". */
