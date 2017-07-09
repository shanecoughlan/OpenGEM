/*		INIT.C		11/17/86 - 4/28/87	J. Grant	*/

#include "portab.h"
#include "vdidef.alw"
#include "vdiext.alw"

#define ROMAN		0
#define BOLD		THICKEN_BIT
#define ITALIC		SKEW_BIT
#define BOLDITALIC 	(BOLD|ITALIC)

static BYTE pre_chek[] = 
    "userdict /gemdict known not {/Times-Roman findfont 12 scalefont\n"
    "setfont newpath 72 700 moveto\n"
    "(Error:  the GEM PostScript preamble is not available)show\n"
    "newpath 72 686 moveto\n"
    "(  on your printer.  Pre-download the preamble or include it with)\n"
    "show newpath 72 672 moveto (your print job.)show\n"
    "newpath 72 658 moveto(This print job has been aborted.)show\n"
    "showpage stop}if gemdict begin\n";

	VOID
init_data()
{
	EXTERN VOID	init_status();
	WORD		i;

	/* Initialize polyline attributes. */
	cur_attr[L_STYLE] = range(intin[1], 1, MAX_L_STYLE, DEF_L_STYLE);
	cur_attr[L_COLOR] = range(intin[2], 0, MAX_L_COLOR, DEF_L_COLOR);
	cur_attr[L_BEG] = DEF_L_BEG;
	cur_attr[L_END] = DEF_L_END;
	cur_attr[L_WIDTH] = DEF_L_WIDTH;
	cur_attr[L_USER_DEF] = 0xffff;

	/* Initialize polymarker attributes. */
	cur_attr[M_TYPE] = range(intin[3], 1, MAX_M_TYPE, DEF_M_TYPE);
	cur_attr[M_COLOR] = range(intin[4], 0, MAX_M_COLOR, DEF_M_COLOR);
	cur_attr[M_HEIGHT] = DEF_M_HEIGHT;

	/* Initialize text attributes. */
	cur_attr[T_FACE] = 2;
	for (i = 0; i < 9; i++) {
		if ((WORD)font_map[i].face == intin[5]) {
			cur_attr[T_FACE] = intin[5];
			break;
		}  /* End if:  found the face name. */
	}  /* End for:  over font map. */
	cur_attr[T_COLOR] = range(intin[6], 0, MAX_T_COLOR, DEF_T_COLOR);
	cur_attr[T_XPOINT] = cur_attr[T_YPOINT] = DEF_T_POINT;
	cur_attr[T_HEIGHT] = PT_2_ABS(DEF_T_POINT);
	cur_attr[T_H_ALIGN] = cur_attr[T_V_ALIGN] = cur_attr[T_ROTATION] =
		cur_attr[T_EFFECTS] = 0;

	/* Initialize fill area attributes. */
	cur_attr[F_INTERIOR] =
		range(intin[7], 0, MAX_F_INTERIOR, DEF_F_INTERIOR);
	cur_attr[F_STYLE] = range(intin[8], 1, MAX_F_STYLE, DEF_F_STYLE);
	cur_attr[F_COLOR] = range(intin[9], 0, MAX_F_COLOR, DEF_F_COLOR);
	cur_attr[F_PERIMETER] = (WORD)TRUE;

	/* Initialize the array which indicates whether a particular fill */
	/* pattern has been downloaded.  Flag them as not being sent yet. */
	for (i = 0; i < NUM_PATTERNS; i++)
		pat_down[i] = FALSE;

	/* Initialize clipping and miscellaneous globals. */
	CLIP = pending = FALSE;
	g_mode = MD_REPLACE;

	/* Initialize status variables. */
	init_status();

	/* Return information to the requesting program. */
	contrl[2] = 6;
	contrl[4] = 45;
	flip_y = TRUE;
	for (i = 0; i < 45; i++)
		intout[i] = dev_tab[i];
	for (i = 0; i < 12; i++)
		ptsout[i] = siz_tab[i];
}  /* End "init_data". */


	VOID
init_page()
{
	EXTERN VOID	init_fonts();
	WORD		height, width;
	BYTE		path[66];

	/* Determine the page width and height in points. */
	width = ((dev_tab[0] + 1 + 13) / 25) * 6;
	height = ((dev_tab[1] + 1 + 13) / 25) * 6;

	/* Output the bounding box comment. */
	output("%%BoundingBox: 0 0 ");
	outdec(width - 1);
	outdec(height - 1);
	output("\n");
	output("%%EndComments\n");

	/* Output the preamble(s). */
	fullpath(path, "pst*.pre");
	if (f_first(path))
	    sendfile(&g_dta[30]);
	output(pre_chek);

	/* Initialize the preamble. */
	width += 72;
	height += 72;
	if (width > 842)
		width = 842;
	if (height > 1853)
		height = 1853;
	outdec(width);
	outdec(height);
	outdec(g_tray);
	outdec(g_copies);
	output("GEM_INIDOC geminit\n");

	/* Initialize matrix. */
	if (g_squeez == 1)
		output("1.55 ");
	else if (g_squeez == 2)
		output("2.19 ");
	else
		outdec(1);
	outdec(dev_tab[0]);
	outdec(dev_tab[0] + 1);
	outdec(dev_tab[1] + 1);
	if (g_orient == CN_PORTRAIT) {
		y_max = dev_tab[1];
		output("false ");
	}  /* End if: portrait. */
	else {
		y_max = dev_tab[0];
		dev_tab[0] = dev_tab[1];
		dev_tab[1] = y_max;
		output("true ");
	}  /* End else:  landscape. */
	output("matinit\n");

	/* Initialize fonts. */
	init_fonts();

	/* Save VM environment. */
	output("save mark\n%Begin page\nUserSoP\n");
}  /* End "init_page". */


	BOOLEAN
init_dev()
{
	output("%!PS-Adobe-1.0\n%%Title: GEM Document\n");
	output("%%Creator: GEM\n%%Pages: (atend)\n");
	return(abort == 0);
}  /* End "init_dev". */


	VOID
init_status()
{
	EXTERN VOID	init_gstate();
	WORD		i;

	for (i = 0; i < NUM_PATTERNS; i++)
		pat_down[i] = FALSE;
	for (i = 0; i < g_maxface; i++)
		font_map[i].attr &= ~(CN_DOWNLOADED | CN_ENCDONE);
	init_gstate();
}  /* End "init_status". */


	VOID
init_gstate()
{
	dev_t_color = dev_t_face = dev_t_h_align = dev_t_rotation =
	    dev_t_xscale = dev_t_yscale = dev_t_under = dev_t_v_align = 
		dev_color = -1;
}  /* End "init_gstate". */


	VOID
init_fonts()
{
	WORD		i;

	output("/fonts ");
	outdec(g_maxface);
	output("array def /fpt 0 def\n");

	for (i = 0; i < g_maxface; i++) {
		if (font_map[i].attr & CN_NOENC)
			output("/");
		else
			output("/G");
		output(font_map[i].name);
		output(" addfont\n");
	}  /* End for:  over extra faces. */
}  /* End "init_fonts". */

	BYTE
*font_name_save(name)
BYTE	*name;
{
    BYTE	*t;

    t = &g_fnames[g_ffmax];
    while ((g_fnames[g_ffmax++] = *name++) != '\0');
    return t;
}


	VOID
font_enter(name, file, face, attr, encode)
BYTE	*name, *file;
WORD	face, attr, encode;
{
    WORD	i;
    
    for (i = 0; i < g_maxface; i++)
	if (font_map[i].face == face &&
	    (font_map[i].attr & 0xf) == (attr & 0xf))
		return;
    font_map[g_maxface].name = font_name_save(name);
    if (file)
	font_map[g_maxface].file = font_name_save(file);
    font_map[g_maxface].face = face;
    font_map[g_maxface].attr = (attr & 0xf) | (file ? CN_REQDOWN : 0)
				| (encode ? 0 : CN_NOENC);
    g_maxface++;
}

	VOID
init_wired()
{
    BYTE	filename[66], *name, *file, *src;
    WORD	face, attr, encode, count, gotone, handle;

    font_enter("Courier",			NULL, 1, ROMAN, 1);
    font_enter("Courier-Bold",			NULL, 1, BOLD, 1);
    font_enter("Courier-Oblique",		NULL, 1, ITALIC, 1);
    font_enter("Courier-BoldOblique",		NULL, 1, BOLDITALIC, 1);

    font_enter("Helvetica",			NULL, 2, ROMAN, 1);
    font_enter("Helvetica-Bold",		NULL, 2, BOLD, 1);
    font_enter("Helvetica-Oblique",		NULL, 2, ITALIC, 1);
    font_enter("Helvetica-BoldOblique",		NULL, 2, BOLDITALIC, 1);

    font_enter("Times-Roman",			NULL, 14, ROMAN, 1);
    font_enter("Times-Bold",			NULL, 14, BOLD, 1);
    font_enter("Times-Italic",			NULL, 14, ITALIC, 1);
    font_enter("Times-BoldItalic",		NULL, 14, BOLDITALIC, 1);

    font_enter("Symbol",			NULL, 128, ROMAN, 0);

    if (g_fontset == FS_LWPLUS) {
	font_enter("NewCenturySchlbk-Roman",	NULL, 20, ROMAN, 1);
	font_enter("NewCenturySchlbk-Bold",	NULL, 20, BOLD, 1);
	font_enter("NewCenturySchlbk-Italic",	NULL, 20, ITALIC, 1);
	font_enter("NewCenturySchlbk-BoldItalic",NULL, 20, BOLDITALIC, 1);

	font_enter("Palatino-Roman",		NULL, 21, ROMAN, 1);
	font_enter("Palatino-Bold",		NULL, 21, BOLD, 1);
	font_enter("Palatino-Italic",		NULL, 21, ITALIC, 1);
	font_enter("Palatino-BoldItalic",	NULL, 21, BOLDITALIC, 1);

	font_enter("Bookman-Light",		NULL, 23, ROMAN, 1);
	font_enter("Bookman-Demi",		NULL, 23, BOLD, 1);
	font_enter("Bookman-LightItalic",	NULL, 23, ITALIC, 1);
	font_enter("Bookman-DemiItalic",	NULL, 23, BOLDITALIC, 1);

	font_enter("Helvetica-Narrow",		NULL, 50, ROMAN, 1);
	font_enter("Helvetica-Narrow-Bold",	NULL, 50, BOLD, 1);
	font_enter("Helvetica-Narrow-Oblique",	NULL, 50, ITALIC, 1);
	font_enter("Helvetica-Narrow-BoldOblique",NULL, 50, BOLDITALIC, 1);

	font_enter("AvantGarde-Book",		NULL, 51, ROMAN, 1);
	font_enter("AvantGarde-Demi",		NULL, 51, BOLD, 1);
	font_enter("AvantGarde-BookOblique",	NULL, 51, ITALIC, 1);
	font_enter("AvantGarde-DemiOblique",	NULL, 51, BOLDITALIC, 1);

	font_enter("ZapfChancery-MediumItalic",	NULL, 29, ITALIC, 1);

	font_enter("ZapfDingbats",		NULL, 129, ROMAN, 0);
    }

    filename[0] = '\0';
    s_cat(filename, g_psfont);
    s_cat(filename, "*.pfi");
    gotone = f_first(filename);
    while (gotone) {
	filename[0] = '\0';
	s_cat(filename, g_psfont);
	s_cat(filename, &g_dta[30]);
	if ((handle = f_open(filename)) != 0) {
	    if ((count = f_read(handle, filbuf, FILBUF_SIZE)) > 0) {
		name = file = NULL;
		attr = face = encode = 0;
		if ((name = s_scan(filbuf, count, "name(", 5)) != NULL) {
		    src = name;
		    while (*src++ != ')');
		    src[-1] = '\0';
		}
		if ((file = s_scan(filbuf, count, "file(", 5)) != NULL) {
		    src = file;
		    while (*src++ != ')');
		    src[-1] = '\0';
		}
		if ((src = s_scan(filbuf, count, "face(", 5)) != NULL)
		    asc2num(src, &face);
		if ((src = s_scan(filbuf, count, "attributes(", 11)) != NULL)
		    asc2num(src, &attr);
		if ((src = s_scan(filbuf, count, "encode(", 7)) != NULL)
		    asc2num(src, &encode);
		if (name && face > 0)
		    font_enter(name, (file && file[0]) ? file : NULL,
			face, attr, encode);
	    }
	    f_close(handle);
	}
	gotone = f_next();
    }
}/* End "init_wired". */
