/*		PRIMATTR.C	11/17/86 - 4/8/87	J. Grant	*/

#include "portab.h"
#include "vdidef.alw"
#include "vdiext.alw"


	VOID
do_image()
{
	WORD		i, j;
	BYTE		filename[66];

	/* Get the bit image parameters from the display list. */
	image.flags = (BYTE)((intin[0] ? 1 : 0) | (intin[1] ? 2 : 0) |
		(intin[2] ? 4 : 0) | (intin[3] << 3) | (intin[4] << 5));
	image.xl = image.clip_xl = ptsin[0];
	image.xr = image.clip_xr = ptsin[2];
	image.yt = image.clip_yt = ptsin[1];
	image.yb = image.clip_yb = ptsin[3];
	if (contrl[5] == 101) {
	    j = 8;
	    image.rotate = intin[5];
	    while (image.rotate < 0) image.rotate += 3600;
	    while (image.rotate > 3600) image.rotate -= 3600;
	    image.rotate = ((image.rotate + 450) / 900) * 900;
	    if (image.rotate == 3600) image.rotate = 0;
	} else {
	    j = 5;
	    image.rotate = 0;
	}
	for (i = 0; i < contrl[3] - j; i++)
		filename[i] = (BYTE)intin[i + j];
	filename[i] = '\0';

	/* Try to open the bit image file.  Process its contents. */
	if ((image.handle = f_open(filename)) != 0) {
		image_setup();
		if (image.remain > 0)
			image_out();
		f_close(image.handle);
	}  /* End if:  bit image file can be opened. */
}  /* End "do_image". */


	VOID
polyline()
{
	WORD		i, points;

	/* Get the polyline count. */
	points = contrl[1];

	/* Bind the current polyline color. */
	out_color(cur_attr[L_COLOR]);

	/* If either end style is rounded, output to the device arrowed */
	/* line procedure.                                              */
	if (cur_attr[L_BEG] == ARROWED || cur_attr[L_END] == ARROWED) {
		/* Tell the device which ends are to be arrowed. */
		output(cur_attr[L_BEG] == ARROWED ? "true " : "false ");
		output(cur_attr[L_END] == ARROWED ? "true[" : "false[");

		/* Output the coordinates. */
		for (i = 0; i < (points << 1); i += 2)
			outxy(ptsin[i], ptsin[i + 1]);
		output("]arrowline stroke\n");
	}  /* End if:  line style arrowed. */

	else {
		/* Output the path definition (vertices) to the device.  */
		/* Tell the device to draw a polyline with the vertices. */
		poly_path(points, ptsin);
		output("stroke\n");
	}  /* End else:  neither end is arrowed. */

	/* Take care of rounded line ends. */
	if (cur_attr[L_BEG] == ROUNDED) {
		outxy(ptsin[0], ptsin[1]);
		output("rend\n");
	}  /* End if:  beginning point is rounded. */
	if (cur_attr[L_END] == ROUNDED) {
		outxy(ptsin[(points << 1) - 2], ptsin[(points << 1) - 1]);
		output("rend\n");
	}  /* End if:  ending point is rounded. */
}  /* End "polyline". */


	VOID
polymarker()
{
	WORD		i, points;

	/* Get the polymarker count and output the vertices. */
	points = contrl[1];
	if (points > 0) {
		for (i = 0; i < points << 1; i += 2)
			outxy(ptsin[i], ptsin[i+1]);
		outdec(points);

		/* Output the polymarker size and type.  Send the command. */
		outdec(cur_attr[M_HEIGHT]);
		outdec(cur_attr[M_TYPE]);
		output("marker\n");
	}  /* End if:  vertices to output. */
}  /* End "polymarker". */


	VOID
text()
{
	/* Bind the text attributes. */
	bind_text(0);

	/* Copy the text to a byte array. */
	copy_string(contrl[3], intin, (BYTE *)&ptsin[4]);

	/* Output to the device. */
	outxy(ptsin[0], ptsin[1]);
	output("(");
	output((BYTE *)&ptsin[4]);
	output(") gtext\n");
}  /* End "text". */


	VOID
fill_area()
{
	WORD		points;

	/* Get the fill area parameters and output the fill area. */
	points = contrl[1];
	out_fill(points);
}  /* End "fill_area". */


	VOID
f_rectangle()
{
	WORD		oldper;

	oldper = cur_attr[F_PERIMETER];
	cur_attr[F_PERIMETER] = 0;
	if (ptsin[0] == ptsin[2])
		ptsin[2]++;
	if (ptsin[1] == ptsin[3])
		ptsin[3]++;
	ptsin[5] = ptsin[7] = ptsin[3];
	ptsin[3] = ptsin[1];
	ptsin[4] = ptsin[2];
	ptsin[6] = ptsin[0];
	out_fill(4);
	cur_attr[F_PERIMETER] = oldper;
}  /* End "f_rectangle". */


	VOID
gdp()
{
	WORD		i, opcode, spaces, x;
	BOOLEAN		fast;
	BYTE		string[8];
	MLOCAL	WORD	ch_map[] = {211, 210, 212, 176};

	/* Process based on the opcode. */
	if ((opcode = contrl[5]) == 1 || opcode == 8 || opcode == 9)
		fix_corners(ptsin, ULLR);
	switch (opcode) {
		case 1:	/* GDP bar */
			/* Build a new ptsin array and output a fill area. */
			ptsin[5] = ptsin[7] = ptsin[3];
			ptsin[3] = ptsin[1];
			ptsin[4] = ptsin[2];
			ptsin[6] = ptsin[0];
			out_fill(4);
			break;

		case 2:	/* GDP arc */
			/* Draw the arc. */
			if (ptsin[6] > 0)
				out_arc(ptsin[0], ptsin[1], ptsin[6],
					ptsin[6], intin[0], intin[1]);
			break;

		case 3:	/* GDP pie */
			/* Set up extents information for the area tiling. */
			if (ptsin[6] <= 0)
				break;
			low_x = ptsin[0] - ptsin[6];
			low_y = ptsin[1] - ptsin[6];
			high_x = ptsin[0] + ptsin[6];
			high_y = ptsin[1] + ptsin[6];

			/* Output the path definition.  Tile fill the area. */
			outxy(ptsin[0], ptsin[1]);
			outdec(ptsin[6]);
			outdec( (intin[0] + 9)/10 );
			outdec( (intin[1] + 9)/10 );
			outxy(ptsin[0], ptsin[1]);
			output("np mto arc\n");
			tile_fill();
			break;

		case 4:	/* GDP circle */
			/* Transform the down-loaded unit circle */
			/* and tile fill it.                     */
			if (ptsin[4] <= 0)
				break;
			xform_ellipse(ptsin[0], ptsin[1], ptsin[4], ptsin[4]);
			tile_fill();
			break;

		case 5:	/* GDP ellipse */
			/* Transform the down-loaded unit circle */
			/* and tile fill it.                     */
			if (ptsin[2] <= 0 || ptsin[3] <= 0)
				break;
			xform_ellipse(ptsin[0], ptsin[1], ptsin[2], ptsin[3]);
			tile_fill();
			break;

		case 6:	/* GDP elliptical arc */
			/* Draw the arc. */
			if (ptsin[2] <= 0 || ptsin[3] <= 0)
				break;
			out_arc(ptsin[0], ptsin[1], ptsin[2], ptsin[3],
				intin[0], intin[1]);
			break;

		case 7:	/* GDP elliptical pie */
			/* Set up extents information for the area tiling. */
			if (ptsin[2] <= 0 || ptsin[3] <= 0)
				break;
			low_x = ptsin[0] - ptsin[2];
			low_y = ptsin[1] - ptsin[3];
			high_x = ptsin[0] + ptsin[2];
			high_y = ptsin[1] + ptsin[3];

			/* Send the pie. */
			outxy(ptsin[0], ptsin[1]);
			outdec(ptsin[2]);
			outdec(ptsin[3]);
			outdec( (intin[0] + 9)/10 );
			outdec( (intin[1] + 9)/10 );
			output("true ellpie\n");
			tile_fill();
			break;

		case 8:	/* GDP rounded rectangle */
			/* Bind the current line color, output the */
			/* rounded rectangle path and draw it.     */
			if (ptsin[0] != ptsin[2] && ptsin[1] != ptsin[3]) {
				out_color(cur_attr[L_COLOR]);
				rbox_path();
				output("stroke\n");
			}  /* End if:  valid. */
			break;

		case 9:	/* GDP filled rounded rectangle */
			if (ptsin[0] != ptsin[2] && ptsin[1] != ptsin[3]) {
				/* Set up extents information for the */
				/* area tiling.                       */
				low_x = ptsin[0];
				low_y = ptsin[1];
				high_x = ptsin[2];
				high_y = ptsin[3];

				/* Output the rounded rectangle path and */
				/* tile it.                              */
				rbox_path();
				tile_fill();
			}  /* End if:  valid. */
			break;

		case 10:	/* GDP justified graphics text */
			/* Bind the text attributes. */
			fast = bind_text(10);

			/* Count spaces. */
			spaces = 0;
			for (i = 2; i < contrl[3]; i++) {
				if ((BYTE)intin[i] == ' ')
					spaces++;
			}  /* End for:  over string. */

			/* Copy the text to a byte array, taking care */
			/* of characters which need to be escaped.    */
			copy_string(contrl[3] - 2, &intin[2],
				(BYTE *)&ptsin[4]);

			/* Output to the device. */
			outxy(ptsin[0], ptsin[1]);
			outdec(ptsin[2]);
			outdec(spaces);
			outdec(intin[1] ? 2 : (intin[0] ? 1 : 0));
			output("(");
			output((BYTE *)&ptsin[4]);
			if (fast)
				output(")fjt\n");
			else
				output(")jtext\n");
			break;

		case 11:	/* GDP escapement text */
			/* Bind the text attributes. */
			fast = bind_text(11);

			/* Output to the device. */
			x = ptsin[0];
			ptsin[0] = 0;
			for (i = contrl[3] - 1; i >= 0; i--) {
				output("(");
				copy_string(1, &intin[i], string);
				output(string);
				output(")");
				outdec(ptsin[i << 1]);
			}  /* End for:  backwards over string. */
			outdec(contrl[3]);
			outxy(x, ptsin[1]);
			if (fast)
				output("fet\n");
			else
				output("etext\n");
			break;
	}  /* End switch. */
}  /* End "gdp". */


	VOID
t_height()
{
	WORD	points;

	points = ABS_2_PT(ptsin[1]);
	cur_attr[T_XPOINT] = cur_attr[T_YPOINT] =
	    points < MIN_T_POINT ? MIN_T_POINT : points;
	cur_attr[T_HEIGHT] = ptsout[0] = ptsout[1] = ptsout[2] = ptsout[3] =
		PT_2_ABS(cur_attr[T_YPOINT]);
	contrl[2] = 2;
	flip_y = TRUE;
}  /* End "t_height". */


	VOID
t_rotation()
{
	cur_attr[T_ROTATION] = intout[0] = 90*((intin[0] + 450)/900);
	contrl[4] = 1;
}  /* End "t_rotation". */


	VOID
l_type()
{
	intout[0] = range(intin[0], 1, MAX_L_STYLE, DEF_L_STYLE);
	if (cur_attr[L_STYLE] != intout[0]) {
		cur_attr[L_STYLE] = intout[0];
		output(line_types[cur_attr[L_STYLE] - 1]);
		output("0 setdash\n");
	}  /* End if:  new line type. */
	contrl[4] = 1;
}  /* End "l_type". */


	VOID
l_width()
{
	ptsout[0] = range(ptsin[0], 1, MAX_L_WIDTH, DEF_L_WIDTH);
	if (cur_attr[L_WIDTH] != ptsout[0]) {
		cur_attr[L_WIDTH] = ptsout[0];
		if ((cur_attr[L_WIDTH] & 1) == 0 && cur_attr[L_WIDTH] > 0)
			cur_attr[L_WIDTH]--;
		outdec(cur_attr[L_WIDTH]);
		output("setlinewidth\n");
	}  /* End if:  new line width. */
	contrl[2] = 1;
	ptsout[1] = 0;
}  /* End "l_width". */


	VOID
l_color()
{
	cur_attr[L_COLOR] = intout[0] =
		range(intin[0], 0, MAX_L_COLOR, DEF_L_COLOR);
	contrl[4] = 1;
}  /* End "l_color". */


	VOID
m_type()
{
	cur_attr[M_TYPE] = intout[0] =
		range(intin[0], 1, MAX_M_TYPE, DEF_M_TYPE);
	contrl[4] = 1;
}  /* End "m_type". */


	VOID
m_height()
{
	cur_attr[M_HEIGHT] = ptsout[1] =
		range(ptsin[1], MIN_M_HEIGHT, MAX_M_HEIGHT, DEF_M_HEIGHT);
	contrl[2] = 1;
	ptsout[0] = 0;
	flip_y = TRUE;
}  /* End "m_height". */


	VOID
m_color()
{
	cur_attr[M_COLOR] = intout[0] =
		range(intin[0], 0, MAX_M_COLOR, DEF_M_COLOR);
	contrl[4] = 1;
}  /* End "m_color". */


	VOID
t_face()
{
	WORD	i;

	intout[0] = 1;
	for (i = 0; i < g_maxface; i++) {
		if ((WORD)font_map[i].face == intin[0]) {
			intout[0] = intin[0];
			break;
		}  /* End if:  found the face name. */
	}  /* End for:  over font map. */

	cur_attr[T_FACE] = intout[0];
	contrl[4] = 1;
}  /* End "t_face". */


	VOID
t_color()
{
	cur_attr[T_COLOR] = intout[0] =
		range(intin[0], 0, MAX_T_COLOR, DEF_T_COLOR);
	contrl[4] = 1;
}  /* End "t_color". */


	VOID
t_alignment()
{
	cur_attr[T_H_ALIGN] = intout[0] = range(intin[0], 0, 2, 0);
	cur_attr[T_V_ALIGN] = intout[1] = range(intin[1], 0, 5, 0);
	contrl[4] = 2;
}  /* End "t_alignment". */


	VOID
t_effects()
{
	cur_attr[T_EFFECTS] = intout[0] = intin[0] & 0xf;
	contrl[4] = 1;
}  /* End "t_effects". */


	VOID
t_point()
{
	cur_attr[T_XPOINT] = cur_attr[T_YPOINT] = intout[0] =
		intin[0] < MIN_T_POINT ? MIN_T_POINT : intin[0];
	contrl[2] = 2;
	contrl[4] = 1;
	flip_y = TRUE;
	ptsout[0] = ptsout[1] = ptsout[2] = ptsout[3] = cur_attr[T_HEIGHT] =
		PT_2_ABS(intout[0]);
}  /* End "t_point". */


	VOID
f_interior()
{
	cur_attr[F_INTERIOR] = intout[0] =
		range(intin[0], 0, MAX_F_INTERIOR, DEF_F_INTERIOR);
	contrl[4] = 1;
}  /* End "f_interior". */


	VOID
f_style()
{
	WORD	i;

	if (cur_attr[F_INTERIOR] == 3)
	    i = range(intin[0], 1, dev_tab[12], 1);
	else
	    i = range(intin[0], 1, MAX_F_STYLE, DEF_F_STYLE);
	cur_attr[F_STYLE] = intout[0] = i;
}  /* End "f_style". */


	VOID
f_color()
{
	cur_attr[F_COLOR] = intout[0] =
		range(intin[0], 0, MAX_F_COLOR, DEF_F_COLOR);
	contrl[4] = 1;
}  /* End "f_color". */


	VOID
f_perimeter()
{
	cur_attr[F_PERIMETER] = intout[0] = intin[0] ? TRUE : FALSE;
	contrl[4] = 1;
}  /* End "f_perimeter". */


	VOID
l_end_styles()
{
	cur_attr[L_BEG] = range(intin[0], 0, 2, 0);
	cur_attr[L_END] = range(intin[1], 0, 2, 0);
}  /* End "l_end_styles". */


	VOID
f_ud_pattern()
{
	WORD		count, i, j;
	BYTE		*from, *to;

	/* Convert the first sixteen words of the pattern to device format. */
	count = contrl[3];
	to = user_fill;
	for (i = 0; i < 16; i++) {
		/* Convert to a hex string. */
		from = i_hs(intin[i]);

		/* Transfer to the user-defined pattern string. */
		for (j = 0; j < 4; j++)
			*to++ = *from++;
	}  /* End for:  over words in the pattern. */

	/* Flag that the pattern needs to be downloaded. */
	pat_down[NUM_PATTERNS - 1] = FALSE;
}  /* End "f_ud_pattern". */


	VOID
l_ud_style()
{
	WORD		count, i, index, style, toggle;
	UWORD		mask;
	WORD		bits[16];
	BYTE		*from;

	/* Get the desired line style and convert it to device format. */
	style = intin[0];
	for (i = 0, mask = 0x8000; i < 16; i++, mask >>= 1)
		bits[i] = (style & mask) ? 1 : 0;
	for (toggle = count = i = 0, style = index = 1; i < 16 && toggle < 10;
		i++, count++) {
		if (bits[i] != style) {
			from = i_ds(count);
			while (*from != '\0')
				user_line[index++] = *from++;
			count = 0;
			toggle++;
			style = 1 - style;
		}  /* End if:  bit toggle. */
	}  /* End for. */
	from = i_ds(count);
	while (*from != ' ')
		user_line[index++] = *from++;
	user_line[index++] = ']';
	user_line[index] = '\0';

	/* Output the line style, if necessary. */
	if (cur_attr[L_STYLE] == 7) {
		output(user_line);
		output("0 setdash\n");
	}  /* End if:  line style must be output. */
}  /* End "l_ud_style". */


	VOID
clip()
{
	EXTERN VOID	set_clip();

	CLIP = intin[0] != 0 ? TRUE : FALSE;
	output("initclip ");

	g_x1clip = ptsin[0];
	g_y1clip = ptsin[1];
	g_x2clip = ptsin[2];
	g_y2clip = ptsin[3];
	set_clip();

	init_gstate();
}  /* End "clip". */


	VOID
set_clip()
{
	if (CLIP) {
		outxy(g_x1clip, g_y1clip);
		outxy(g_x2clip, g_y1clip);
		outxy(g_x2clip, g_y2clip);
		outxy(g_x1clip, g_y2clip);
		output("np mto lto lto lto clip np\n");
	}  /* End if:  clipping is to be turned on. */
}  /* End "set_clip". */


	VOID
w_mode()
{
	g_mode = intin[0];
}  /* End "w_mode". */
