#include "portab.h"
#include "vdidef.alw"
#include "vdiext.alw"

static	WORD	nlines = 63;	/* alpha text vertical position on page */
static	WORD	blank = 1;	/* alpha text page is blank */

    VOID
alpha_advance()
{
    blank = 1;
}


    VOID
alpha_send(string, end)
BYTE	string[];			/* Alpha string to transmit. */
WORD	end;				/* Index of end of string. */
{
	EXTERN	VOID	output();
	EXTERN	VOID	bind_text();

	/* If any text is in the string, send the text and command. */
	if (end > 0) {
	    bind_text (-1);
	    string[end] = '\0';
	    output("(");
	    output(string);
	    output(") atext\n");
	    pending = TRUE;
	}  /* End if:  must send. */
}  /* End "alpha_send". */


    VOID
alpha_text()
{
    EXTERN	VOID	alpha_send();
    EXTERN	VOID	output();
    EXTERN	VOID	outdec();
    EXTERN	WORD	form_advance();
#define BUFFSIZE 128
    BYTE	string[BUFFSIZE+10];
    WORD	i, sp, temp;
    static WORD	a_effects = 0;
    static WORD	a_face = 1;
    static WORD	a_color = 1;
    static WORD	a_xpoint = 12;
    static WORD	a_ypoint = 12;
    static WORD	a_rotation = 0;
    static WORD	a_h_align = 0;
    static WORD	a_v_align = 0;
    static WORD	a_super = 0;
    static WORD	a_sub = 0;
    static WORD	size[] = {12, 24, 10, 20, 7, 13, 12, 24};
    static WORD	expanded = 0;
    static WORD	style = 0;
    WORD	save_effects, save_face, save_color, save_xpoint, save_ypoint,
		save_rotation, save_h_align, save_v_align;

    save_effects = cur_attr[T_EFFECTS];
    save_face = cur_attr[T_FACE];
    save_color = cur_attr[T_COLOR];
    save_xpoint = cur_attr[T_XPOINT];
    save_ypoint = cur_attr[T_YPOINT];
    save_rotation = cur_attr[T_ROTATION];
    save_h_align = cur_attr[T_H_ALIGN];
    save_v_align = cur_attr[T_V_ALIGN];

    cur_attr[T_EFFECTS] = a_effects;
    cur_attr[T_FACE] = a_face;
    cur_attr[T_COLOR] = a_color;
    cur_attr[T_XPOINT] = a_xpoint;
    cur_attr[T_YPOINT] = a_ypoint;
    cur_attr[T_ROTATION] = a_rotation;
    cur_attr[T_H_ALIGN]= a_h_align;
    cur_attr[T_V_ALIGN] = a_v_align;

    sp = 0;
    for (i = 0; i < contrl[3]; i++) {
	if (blank) {
	    nlines = (dev_tab[1] + 1) / 50;
	    output("/ah 0 def /av ");
	    outdec(dev_tab[1] + 1 - 50);
	    output("def\n");
	    blank = 0;
	}
	temp = intin[i];
	if (temp == '\t') temp = ' ';
	if (temp < ' ') {
	    alpha_send(string, sp);
	    sp = 0;
	    switch (temp) {
		case '\n':  output("LF\n");
			    if (--nlines > 0)
				break;
		case '\f':  form_advance(); /* sets blank = 1 */
			    break;
		case '\r':  output("CR ");
			    break;
		case 18:    switch (intin[++i]) {
				 case '0':	/* Begin bolding. */
					a_effects = (cur_attr[T_EFFECTS]
						    |= THICKEN_BIT);
					break;
				 case '1':	/* End bolding. */
					 a_effects = (cur_attr[T_EFFECTS]
						     &= ~THICKEN_BIT);
					 break;
				 case '2':	/* Begin italicizing. */
					 a_effects = (cur_attr[T_EFFECTS]
						     |= SKEW_BIT);
					 break;
				 case '3':	/* End italicizing. */
					 a_effects = (cur_attr[T_EFFECTS]
						     &= ~SKEW_BIT);
					 break;
				 case '4':	/* Begin underlining. */
					 a_effects = (cur_attr[T_EFFECTS]
						     |= UNDER_BIT);
					 break;
				 case '5':	/* End underlining. */
					 a_effects = (cur_attr[T_EFFECTS]
						     &= ~UNDER_BIT);
					 break;
				 case '6':	/* Begin superscript. */
				         if (!a_super) {
					     output("0 20 translate\n");
					     a_ypoint = cur_attr[T_YPOINT] =
						 8;
					     a_super = 1;
					 }
					 break;
				 case '7':	/* End superscript. */
				         if (a_super) {
					     output("0 -20 translate\n");
					     a_ypoint = cur_attr[T_YPOINT] =
						 12;
					     a_super = 0;
					 }
					 break;
				 case '8':	/* Begin subscript. */
				         if (!a_sub) {
					     output("0 -10 translate\n");
					     a_ypoint = cur_attr[T_YPOINT] =
						 8;
					     a_sub = 1;
					 }
					 break;
				 case '9':	/* End subscript. */
				         if (a_sub) {
					     output("0 10 translate\n");
					     a_ypoint = cur_attr[T_YPOINT] =
						 12;
					     a_sub = 0;
					 }
					 break;
				 case 'C':
					 expanded = 1;
					 goto l;
				 case 'D':
					 expanded = 0;
					 goto l;
				 case 'E':	/* Begin light. */
					 a_effects = (cur_attr[T_EFFECTS]
						     |= LIGHTEN_BIT);
					 break;
				 case 'F':	/* End lighten. */
					 a_effects = (cur_attr[T_EFFECTS]
						     &= ~LIGHTEN_BIT);
					 break;
				 case 'W':	/* Set pica. */
					 a_face = cur_attr[T_FACE] = 1;
					 style = 0;
				 l:	 a_xpoint = cur_attr[T_XPOINT] =
					     size[style+expanded];
					 break;
				 case 'X':	/* Set elite. */
					 a_face = cur_attr[T_FACE] = 1;
					 style = 2;
					 goto l;
				 case 'Y':	/* Set condensed. */
					 a_face = cur_attr[T_FACE] = 1;
					 style = 4;
					 goto l;
				 case 'Z':	/* Set proportional. */
					 a_face = cur_attr[T_FACE] = 2;
					 style = 6;
					 goto l;
				 default:
					 break;
			     }
	    }  /* End switch. */
	} else {
	    if (temp >= 127) {
		string[sp++] = '\\';
		string[sp++] = (BYTE)(((temp >> 6) & 3) + 48);
		string[sp++] = (BYTE)(((temp >> 3) & 7) + 48);
		string[sp++] = (BYTE)((temp & 7) + 48);
	    } else {
		if (temp == '(' || temp == ')' || temp == '\\')
		    string[sp++] = '\\';
		string[sp++] = temp;
	    }
	    if (sp >= BUFFSIZE) {
		alpha_send(string, sp);
		sp = 0;
	    }
	}  /* End else:  printable character. */
    }  /* End for:  over all characters. */

    /* Send out whatever remains. */
    alpha_send(string, sp);

    cur_attr[T_EFFECTS] = save_effects;
    cur_attr[T_FACE] = save_face;
    cur_attr[T_COLOR] = save_color;
    cur_attr[T_XPOINT] = save_xpoint;
    cur_attr[T_YPOINT] = save_ypoint;
    cur_attr[T_ROTATION] = save_rotation;
    cur_attr[T_H_ALIGN]= save_h_align;
    cur_attr[T_V_ALIGN] = save_v_align;

}  /* End "alpha_text". */


    VOID
output_window()
{
    EXTERN	VOID	output();
    EXTERN	VOID	outdec();
    EXTERN	WORD	abs();
    EXTERN	WORD	form_advance();
    WORD	y, ns;

    y = abs(ptsin[1] - ptsin[3]);
    ns = (y / 50) + 1;
    nlines -= ns;
    if (nlines > 0) {
	outdec(0);
	outdec(ns * -50);
	output  ("translate /ah 0 def initclip\n");
    } else {
	form_advance();
    }
}  /* End "output_window". */
