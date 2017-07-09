/*		GRAPHICS.C	11/17/86 - 4/8/87	J. Grant	*/

#include "portab.h"
#include "vdidef.alw"
#include "vdiext.alw"


	BOOLEAN
bind_text(esc_type)
	WORD		esc_type;
{
	WORD		dev_face, dev_attr, dev_gray, dev_under, dev_v_align;
	WORD		i, index;
	BOOLEAN		change;
	BYTE		cface, cattr;

	/* Bind text font and related attributes. */
	cattr = cur_attr[T_EFFECTS] & 7;
	cface = cur_attr[T_FACE];
	dev_face = 0;
	for (i = index = 0; i < g_maxface; i++) {
		if (font_map[i].face == cface) {
			if ((font_map[i].attr & 0x7) == cattr) {
				index = i;
				break;
			}
			if (!dev_face)
				index = i;
		}  /* End if:  found the face name. */
	}  /* End for:  over font map. */
	dev_face = index;
	dev_attr = (WORD)font_map[index].attr;

	if ((dev_attr & CN_REQDOWN) && !(dev_attr & CN_DOWNLOADED))
		fontdown(index);

	change = dev_face != dev_t_face;
	if (change) {
		output("/tface ");
		outdec(dev_t_face = dev_face);
		output("def\n");
	}  /* End if:  current face must be output. */

	if (!(dev_attr & CN_NOENC) && !(dev_attr & CN_ENCDONE)) {
	    outdec(dev_t_face);
	    output("encfont\n");
	    font_map[index].attr |= CN_ENCDONE;
	}  /* End if:  encoding information must be sent. */

	/* Output the appropriate text color if the current device text */
	/* color is not the same as the one requested.  Note that the   */
	/* current lightening effect must be bound in, too.             */
	if (cur_attr[T_EFFECTS] & LIGHTEN_BIT && cur_attr[T_COLOR])
		dev_gray = 1;
	else
		dev_gray = cur_attr[T_COLOR] ? 0 : 2;
	if (dev_gray != dev_t_color) {
		output("/tcolor ");
		outdec(dev_t_color = dev_gray);
		output("2 div def\n");
	}  /* End if:  current text color must be output. */

	/* Output the appropriate text scaling if the current device text */
	/* scaling is not the same as the one requested.                  */
	if (cur_attr[T_XPOINT] != dev_t_xscale ||
	    cur_attr[T_YPOINT] != dev_t_yscale) {
		output("/txscale ");
		outdec(dev_t_xscale = cur_attr[T_XPOINT]);
		output("300 mul 72 div def ");
		output("/tyscale ");
		outdec(dev_t_yscale = cur_attr[T_YPOINT]);
		output("300 mul 72 div def\n");
		change = TRUE;
	}  /* End if:  current text scaling must be output. */

	/* Output the appropriate text rotation if the current device text */
	/* rotation is not the same as the one requested.                  */
	if (cur_attr[T_ROTATION] != dev_t_rotation) {
		output("/trotate ");
		outdec(dev_t_rotation = cur_attr[T_ROTATION]);
		output("def\n");
	}  /* End if:  current text rotation must be output. */

	/* Output the appropriate text alignment if the current device text */
	/* alignment is not the same as the one requested.  Note that top   */
	/* and ascent alignment are equivalent, as are bottom and descent.  */
	if (cur_attr[T_H_ALIGN] != dev_t_h_align) {
		output("/halign ");
		outdec(dev_t_h_align = cur_attr[T_H_ALIGN]);
		output("def\n");
	}  /* End if:  current text horizontal alignment must be output. */

	dev_v_align = cur_attr[T_V_ALIGN] > 3 ? 7 - cur_attr[T_V_ALIGN] :
		cur_attr[T_V_ALIGN];
	if (dev_v_align != dev_t_v_align) {
		output("/valign ");
		outdec(dev_t_v_align = dev_v_align);
		output("def\n");
	}  /* End if:  current text vertical alignment must be output. */

	/* Output the appropriate text underlining if the current device */
	/* text underlining is not the same as the one requested.        */
	if ((dev_under = cur_attr[T_EFFECTS] & UNDER_BIT) != dev_t_under) {
		if ((dev_t_under = dev_under) != 0)
			output("/tunder true def\n");
		else
			output("/tunder false def\n");
	}  /* End if:  current text underlining must be output. */

	/* Change text fonts, if required. */
	if (change)
		output("sf\n");

	/* Will this be fast justified/escapement text? */
	return(dev_t_rotation == 0 && dev_t_color == 0 &&
		(esc_type == 10 ? dev_t_h_align == 0 && dev_t_v_align == 0 :
		TRUE));
}  /* End "bind_text". */


	VOID
fill_extents(points)
	WORD		points;
{
	WORD		i;

	low_x = high_x = ptsin[0];
	low_y = high_y = ptsin[1];

	for (i = 2; i < 2*points; i++) {
		if (ptsin[i] < low_x)
			low_x = ptsin[i];
		if (ptsin[i] > high_x)
			high_x = ptsin[i];
		if (ptsin[++i] < low_y)
			low_y = ptsin[i];
		if (ptsin[i] > high_y)
			high_y = ptsin[i];
	}  /* End for:  over ptsin array. */
}  /* End "fill_extents". */


	VOID
fill_pat()
{
	WORD		dev_fill, i;
	BYTE		*pat;

	switch (cur_attr[F_INTERIOR]) {
		default:
		case 0:	/* Hollow. */
			output("1 setgray\n");
			return;

		case 1:	/* Solid. */
			outdec(cur_attr[F_COLOR] ? 0 : 1);
			output("setgray\n");
			return;

		case 2:	/* Pattern. */
			if (cur_attr[F_STYLE] < 9) {
				if (cur_attr[F_COLOR] == 0)
				    outdec(8);
				else
				    outdec(8 - cur_attr[F_STYLE]);
				output("8 div setgray\n");
				return;
			}  /* End if:  dither pattern. */
			dev_fill = cur_attr[F_STYLE] - 9;
			break;

		case 3:	/* Hatch. */
			dev_fill = cur_attr[F_STYLE] + 15;
			break;

		case 4:	/* User-defined. */
			dev_fill = 28;
			break;
	}  /* End switch. */

	/* If the fill pattern has not been downloaded, do so. */
	if (!pat_down[dev_fill]) {
		output("/p");
		outdec(dev_fill);
		output("<");
		pat = pat_def[dev_fill];
		for (i = 0; i < 32; i++)
			outhex(*pat++);
		output("> def\n");
		pat_down[dev_fill] = TRUE;
	}  /* End if:  pattern needs to be downloaded. */
	output("p");
	outdec(dev_fill);
	output("16 2 10 setpattern\n");
}  /* End "fill_pat". */


	VOID
fix_corners(xy, type)
	WORD		xy[];
	WORD		type;
{
	WORD		temp;

	/* Fix the x coordinate values, if necessary. */
	if (xy[0] > xy[2]) {
		temp = xy[0];
		xy[0] = xy[2];
		xy[2] = temp;
	}  /* End if:  x must be swapped. */

	/* Fix the y coordinate values based on whether traditional */
	/* (ll, ur) or raster-op (ul, lr) format is desired.        */
	if ((type == LLUR && xy[1] < xy[3]) ||
		(type == ULLR && xy[1] > xy[3])) {
		temp = xy[1];
		xy[1] = xy[3];
		xy[3] = temp;
	}  /* End if:  y must be swapped. */
}  /* End "fix_corners". */


	VOID
out_arc(x, y, x_radius, y_radius, start, end)
	WORD		x;
	WORD		y;
	WORD		x_radius;
	WORD		y_radius;
	WORD		start;
	WORD		end;
{
	EXTERN VOID	out_ellarc();
	EXTERN VOID	out_color();

	/* Divide the angles by ten to get degrees. */
	start = (start + 9)/10;
	end = (end + 9)/10;

	/* Bind the current line color. */
	out_color(cur_attr[L_COLOR]);

	/* If either end style is arrowed, output to the device arrowed arc */
	/* procedure.                                                       */
	if (cur_attr[L_BEG] == ARROWED || cur_attr[L_END] == ARROWED) {
		/* Tell the device which ends are to be arrowed. */
		output(cur_attr[L_BEG] == ARROWED ? "true " : "false ");
		output(cur_attr[L_END] == ARROWED ? "true " : "false ");

		/* Send the arc information. */
		out_ellarc(x, y, x_radius, y_radius, start, end);
		output("arrowarc\n");
	}  /* End if:  arrowed end style. */

	else {
		/* Send the arc information. */
		out_ellarc(x, y, x_radius, y_radius, start, end);
		output("false ellpie gs stroke gr\n");
	}  /* End else:  neither end is arrowed. */

	/* Take care of rounded line ends only if the line is wide. */
	if (cur_attr[L_WIDTH] > 5) {
		if (cur_attr[L_END] == ROUNDED) {
			out_ellarc(x, y, x_radius, y_radius, start, end);
			output("false ellpie roundarc\n");
		}  /* End if:  end is rounded. */
		if (cur_attr[L_BEG] == ROUNDED) {
			out_ellarc(x, y, x_radius, y_radius, start, start);
			output("false ellpie roundarc\n");
		}  /* End if:  end is rounded. */
	}  /* End if:  lines are wide enough for rounded ends. */
}  /* End "out_arc". */


	VOID
out_ellarc(x, y, x_radius, y_radius, start, end)
	WORD		x;
	WORD		y;
	WORD		x_radius;
	WORD		y_radius;
	WORD		start;
	WORD		end;
{
	/* Output the elliptical arc parameters. */
	outxy(x, y);
	outdec(x_radius);
	outdec(y_radius);
	outdec(start);
	outdec(end);
}  /* End "out_ellarc". */


	VOID
out_color(color)
	WORD		color;
{
	/* If the requested color must be transmitted to the device, do so. */
	/* Note that color indices are reversed on the device:  0 is black  */
	/* and 1 is white.                                                  */
#if 0
	if (dev_color != 1 - color)
#endif
	{
		outdec(dev_color = 1 - color);
		output("setgray\n");
	}  /* End if:  device color must be sent. */
}  /* End "out_color". */


	VOID
out_fill(points)
	WORD		points;
{
	EXTERN VOID	poly_path();
	EXTERN VOID	tile_fill();

	/* Find the bounding rectangle for the fill area. */
	fill_extents(points);

	/* Output the path definition (vertices) to the device.  Tile fill */
	/* the area.                                                       */
	poly_path(points, ptsin);
	tile_fill();
}  /* End "out_fill". */


	VOID
poly_path(points, xy)
	WORD		points;
	WORD		xy[];
{
	WORD		i;

	/* Output the path.  Put out the points in reverse order, since */
	/* they will be processed backwards.                            */
	for (i = 2*points - 1; i > 1; i -= 2)
		outxy(xy[i - 1], xy[i]);
	outdec(points - 1);
	outxy(xy[0], xy[1]);
	output("path\n");
}  /* End "poly_path". */


	VOID
rbox_path()
{
	/* Output the path.  Pass the points in the order (llx, lly), */
	/* (urx, ury).                                                */
	outxy(ptsin[0], ptsin[3]);
	outxy(ptsin[2], ptsin[1]);
	output("rbox ");
}  /* End "rbox_path". */


	VOID
set_xform(xt, yt, xs, ys)
	WORD		xt;
	WORD		yt;
	WORD		xs;
	WORD		ys;
{
	/* Save the current transformation and output the desired      */
	/* additional transformation.  Note that the calling procedure */
	/* should later do a "setmatrix" to restore the original       */
	/* transformation.                                             */
	output("6 array currentmatrix ");
	outdec(xs);
	outdec(ys);
	outxy(xt, yt);
	output("translate scale ");
}  /* End "set_xform". */


	VOID
tile_fill()
{
	output("gs ");
	fill_pat();
	if (g_mode != MD_TRANSPARENT || cur_attr[F_INTERIOR] != 0)
	    output("gs eofill gr ");

	/* If necessary, draw the perimeter. */
	if (cur_attr[F_PERIMETER]) {
	    out_color(cur_attr[F_COLOR]);
	    output("closepath stroke\n");
	}
	output("gr ");

}  /* End "tile_fill". */


	VOID
xform_ellipse(x, y, x_radius, y_radius)
	WORD		x;
	WORD		y;
	WORD		x_radius;
	WORD		y_radius;
{
	/* Set up extents information for the area tiling. */
	low_x = x - x_radius;
	low_y = y - y_radius;
	high_x = x + x_radius;
	high_y = y + y_radius;

	/* Transform the down-loaded unit circle to give the desired    */
	/* ellipse.  The final "setmatrix" must be performed to restore */
	/* the transformation which was saved by "set_xform".           */
	set_xform(x, y, x_radius, y_radius);
	output("circle setmatrix\n");
}  /* End "xform_ellipse". */
