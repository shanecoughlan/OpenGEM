/************************************************************************/
/*	File:	display.c						*/
/************************************************************************/
/*									*/
/*		     GGGGG        EEEEEEEE     MM      MM		*/
/*		   GG             EE           MMMM  MMMM		*/
/*		   GG   GGG       EEEEE        MM  MM  MM		*/
/*		   GG   GG        EE           MM      MM		*/
/*		     GGGGG        EEEEEEEE     MM      MM		*/
/*									*/
/************************************************************************/
/*									*/
/*			  +--------------------------+			*/
/*			  | Digital Research, Inc.   |			*/
/*			  | Oxford Street	     |			*/
/*			  | Newbury, (UK)	     |			*/
/*			  +--------------------------+			*/
/*									*/
/*									*/
/*   Author:	Christof Fetzer						*/
/*   PRODUCT:	GEM Sample Desk Top Accessory				*/
/*   Module:	Display							*/
/*   Version:	July 28, 1985						*/
/*									*/
/*   Copyright (c) by Digital Research , 1985				*/
/*									*/
/************************************************************************/


/*

Page*/
/*------------------------------*/
/*	includes		*/
/*------------------------------*/

#include "dispver.h"				/* #define VERSION x	*/
#include "display.h"				/* rsc - defines	*/
#include "dispdefs.h"				/* #defines ...		*/
#include "dispmess.h"				/* display messages	*/
#include "portab.h"				/* portable coding conv	*/
#include "machine.h"				/* machine depndnt conv	*/
#include "obdefs.h"				/* object definitions	*/
#include "gembind.h"				/* gem binding structs	*/


/*------------------------------*/
/*	defines			*/
/*------------------------------*/

#define	ARROW		0
#define	HOUR_GLASS	2			

#define	DESK		0

#define END_UPDATE	0
#define	BEG_UPDATE	1


/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Data Structures			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	Global			*/
/*------------------------------*/

GLOBAL WORD	contrl[11];		/* control inputs		*/
GLOBAL WORD	intin[LENGTH];		/* max string length (= col. len) */
GLOBAL WORD	ptsin[256];		/* polygon fill points		*/
GLOBAL WORD	intout[45];		/* open workstation output	*/
GLOBAL WORD	ptsout[12];


/*------------------------------*/
/*	Local			*/
/*------------------------------*/

WORD	gl_wchar;			/* character width		*/
WORD	gl_hchar;			/* character height		*/
WORD	gl_wbox;			/* box (cell) width		*/
WORD	gl_hbox;			/* box (cell) height		*/
WORD	gem_handle;			/* GEM vdi handle		*/
WORD	vdi_handle;			/* disp vdi handle		*/
WORD	work_out[57];			/* open virt workstation values	*/
GRECT	work_area;			/* current window work area	*/
GRECT	border_area;			/* current window border area	*/
WORD	gl_apid;			/* application ID		*/
WORD	gl_rmsg[8];			/* message buffer		*/
LONG	ad_rmsg;			/* LONG pointer to message bfr	*/
WORD	gl_itemdisp = 0;		/* disp menu item		*/
WORD	gl_xfull;			/* full window 'x'		*/
WORD	gl_yfull;			/* full window 'y'		*/
WORD	gl_wfull;			/* full window 'w' width	*/
WORD	gl_hfull;			/* full window 'h' height	*/
WORD	ev_which;			/* event message returned value	*/
WORD	disp_whndl = 0;			/* disp window handle		*/
WORD	disp_size;			/* system font cell size	*/
WORD	fs_iexbutton;			/* selected exit buttom from fsel_input */
WORD	fs_ireturn;			/* returncode from fsel_input	*/
WORD	gp_x;				/* previously used window 'x'	*/
WORD	gp_y;				/* previously used window 'y'	*/
WORD	gp_w;				/* previously used window 'w'	*/
WORD	gp_h;				/* previously used window 'h'	*/


WORD	columns = INIT_COLS;		/* Number of columns		*/ 
WORD	rows    = INIT_ROWS;		/* Number of rows		*/
WORD	row_offset = 0;			/* absolut row offset to act. line */ 
WORD	row_ptr = 0;			/* relativ offset to start_ptr in the ring buffer */ 
WORD	col_offset = 0;			/* column offset		*/
WORD	h_sd_step = 1000 / LENGTH;	/* step size for one row of the  hor. slider */
WORD	h_sd_len  = (1000/LENGTH) * INIT_COLS; /* slider length		*/
WORD	h_sd_pos  = 0;			/* slider position		*/
WORD	v_sd_step = 1000 / LINES;	/* step size of the vert.slider	*/
WORD	v_sd_len  = (1000/LENGTH) * INIT_ROWS; /* vert. slider length	*/
WORD	v_sd_pos = 0;			/* vert. slider position 	*/

BYTE	lin_buf [LINES] [LENGTH];	/* line ring buffer		*/
WORD	start_ptr = 0;			/* start of text ptr		*/
WORD	end_ptr = 0;			/* end of text ptr		*/

BYTE	*wdw_title = M_TITLE;		/* window title			*/
BYTE	*wdw_info  = M_INFO;		/* info field title		*/
BYTE	wdw_tinfo [100];		/* array to build up info field */
BYTE	fs_iinpath [80] =		/* selected dir. for fsel_inp	*/
	{ ' ', ':', '*', '.', '*', '\0' };
BYTE	fs_iinsel [80]  = { '\0'};	/* selected file for fsel_inp	*/
BYTE	filename [80];			/* filename to disp		*/

/*

Page*/

#define	OP_SIZE		1024		/* Buffersize for input buffer	*/
WORD	op_hndl;			/* act. open file path		*/
WORD	op_fil = FALSE;			/* file open flag		*/
WORD	op_eof;				/* file at end of file		*/
WORD	op_cnt;				/* number of bytes in input buffer	*/
WORD	op_cnter;			/* number of bytes read out of the input buffer */
BYTE	op_buf [OP_SIZE];		/* input buffer			*/

WORD	ev_moflags = 0;			/* mouse in wdw  wait for exit (1) else entry (0) */ 
MFDB	lowr_mfdb;			/* lower scroll window mfdb	*/
MFDB	upr_mfdb;			/* upper scroll window mfdb	*/
GRECT	lowr_area;			/* lower scroll area		*/
GRECT	upr_area;			/* upper scroll area		*/
GRECT	lln_area;			/* last line scroll area	*/

/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Local Procedures			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	min			*/
/*------------------------------*/
WORD
min(a, b)			/* return min of two values	*/
WORD		a, b;
{
	return( (a < b) ? a : b );
}


/*------------------------------*/
/*	max			*/
/*------------------------------*/
WORD
max(a, b)			/* return max of two values	*/
WORD		a, b;
{
	return( (a > b) ? a : b );
}

/*------------------------------*/
/*	distnt			*/
/*------------------------------*/
WORD
distnt (start, end, len, flag)	/* calculate the distant of two buffer	*/
WORD	start, end, len, flag;	/* ptr in a given range (len)		*/
{				

	if (start < end)
		return (end-start);
	else if (flag && start == end) 	/* if the flag is true then the */
		return (0);		/* distant if the two ptr are 	*/
	else				/* is zero else it is len	*/
		return (len - start + end);
}

/*------------------------------*/
/*	sub_rptr		*/
/*------------------------------*/
WORD
sub_rptr (start, len, off)		/* subtract a offset from a buf- */
WORD	start, len, off;		/* fer ptr			*/
{				

	if (start >= off)
		return (start - off);
	else				
		return (len + start - off);
}

/*------------------------------*/
/*	strcpy			*/
/*------------------------------*/
BYTE	*strcpy(s1, s2)			/* copy string	function	*/
BYTE	*s1, *s2;
{

	while (*(s1++) = *(s2++));

}

/*------------------------------*/
/*	strncpy			*/
/*------------------------------*/
BYTE	*strncpy(str1, str2, len)	/* copy string function 	*/
BYTE	*str1, *str2; 			/* max. n characters		*/
WORD	len;
{
	while ((len--) && (*(str1++) = *(str2++)));
}

/*------------------------------*/
/*	strcat			*/
/*------------------------------*/
BYTE	*strcat(str1, str2)		/* cat. two strings		*/
BYTE	*str1, *str2;
{

	while (*(str1++))		/* search for end of string 1	*/
		;	
	strcpy (--str1, str2);		/* copy string 1 at the end of 	*/
}					/* string 1			*/

/*------------------------------*/
/*	readln			*/
/*------------------------------*/
WORD	readln(handle, buffer, count)	/* Read line from handle without */
WORD	handle;				/* storing CR/LF and other	*/
BYTE	*buffer;			/* CTRL Chars			*/
WORD	count;
{

	WORD	cnt = 0;

	while (cnt < count && (read (handle,&(buffer [cnt]), 1) > 0) 
		 && buffer [cnt] != '\n')
	{
		if (buffer [cnt] == '\t')	/* expand tabs 		 */
			do
				buffer [cnt++] = ' ';
			while (cnt % 8 && cnt < count);
		else if ((buffer [cnt] & 0x7f) >= '\0' && (buffer [cnt] & 0x7f) < ' ' )	
			;			/* just skip CR's,.. */
		else
			cnt++;
	}
	while (cnt < count)			/* fill buffer with blanks */
		buffer [cnt++] = ' ';
	buffer [count-1] = '\0';		/* mark end of string	*/
	return (cnt);

}

/*------------------------------*/
/*	open			*/
/*------------------------------*/
WORD	open (str)		/* open a file			*/
BYTE	*str;
{

	if (op_fil)				/* We only support one open file */
		return (-1);
	if ((op_hndl = x_open (str, 0)) < 0)
		return (-1);			/* return error if dos_error	*/
	if ((op_cnt = x_read (op_hndl, op_buf, OP_SIZE)) < 1)	/* read in buffer */
	{					
			return (-1);		/* if dos_error return error code */
	}
	op_eof = FALSE;				/* set open flags		*/
	op_fil = TRUE;
	op_cnter = 0;
	return (op_hndl);
}

/*------------------------------*/
/*	read			*/	
/*------------------------------*/
WORD	read (handle, buffer, count)		/* read count bytes from handle to buffer */
WORD	handle, count;
BYTE	*buffer;
{

	int	cnt = 0;

	if (! op_fil || handle != op_hndl) 	/* Note: we only support one open file */
		return (-1);			/* Return error code		*/
	if (op_eof)				/* if end of file		*/		 	
		return (0);			/*  0 bytes read		*/
	while (count--)				/* while we have no satisfied   */ 
	{					/* the callers byte request	*/
		if (op_cnter == op_cnt)		/* if read buffer is empty	*/
		{				/* fill it up again		*/
			if ((op_cnt = x_read (op_hndl, op_buf, OP_SIZE)) < 1)
			{			/* if dos_error or 0 bytes read	*/
				op_eof = TRUE;	/* set eof true			*/
				return (cnt);	/* and return the bytes already	*/
			}			/* read from the buffer		*/
			op_cnter = 0;		/* clear bytes counter		*/
		}
		buffer [cnt++] = op_buf [op_cnter++];  /* copy char in the 	*/
	}					/* caller's char buffer		*/
	return (cnt);				/* return the bytes we have read*/
}

/*------------------------------*/
/*	eof			*/
/*------------------------------*/
BOOLEAN eof (handle)				/* (handle) at eof ?		*/
WORD	handle;
{

	if (handle != op_hndl || ! op_fil)	/* if handle not in the 'handle */
		return (TRUE);			/* list ' or file not open return eof is TRUE	*/
	return (op_eof);			/* else return the status of the eof flag	*/
}

/*------------------------------*/
/*	read_blk		*/
/*------------------------------*/
VOID
read_blk (handle)							/* fill up ring buffer from (handle)		*/
WORD	handle;								/* dos handle					*/
{

	WORD	temp_ptr = 0;

	if (! eof(handle))						/* while we are not at the end of file		*/
	{
		do							/* and do while the buffer is not filled	*/
		{							/* up						*/
			readln (handle,&(lin_buf [end_ptr] [0]), LENGTH);
			if (eof(handle) && ! temp_ptr)			/* if end of file, 				*/
				temp_ptr = end_ptr;			/* save the end_ptr and clear end of buffer 	*/
			end_ptr = ++end_ptr % LINES;			/* increment end ptr 				*/ 
		}
		while (start_ptr != end_ptr);
		if (temp_ptr)						/* if we had cleared a part of the buf-		*/
			end_ptr = temp_ptr;				/* retore end_ptr				*/
	}

}


/*------------------------------*/
/*	close			*/
/*------------------------------*/
WORD
close (handle)					/* clode handle			*/
WORD	handle;
{

	if (!op_fil || handle != op_hndl)	/* if file closed or wrong handle	*/
		return (-1);			/* return error code			*/
	x_close (handle);			/* call dos close function		*/
	op_fil = FALSE;				/* clear file open flag			*/
	return (1);				/* return Ok code			*/
}

/*------------------------------*/
/*	scroll_up		*/
/*------------------------------*/
scroll_up ()				/* scroll up wdw one line	*/
{

	WORD	pxy [4], wh, ev_mmox, ev_mmoy, ev_mmokstatem, ev;
	GRECT	box;
	BOOLEAN	cl_mouse = FALSE;

#if	(VERSION & ACCESSORY)
	wind_update (BEG_UPDATE);
#endif
	wh = disp_whndl;
	graf_mkstate (&ev_mmox, &ev_mmoy, &ev_mmokstatem, &ev); /* stop flickering of the 'mouse' 	*/
#if	(VERSION & NOFLICKER)
	if (inside (ev_mmox, ev_mmoy, &border_area))		/* if mouse is in the wdw		*/
	{
		graf_mouse(M_OFF, 0x0L);			/* clear mouse				*/
		cl_mouse = TRUE;				/* set flag for turn it on		*/
	}
#else								/* just turn the 'mouse' of		*/
		graf_mouse (M_OFF, 0x0L);
#endif
	grect_to_array(&lln_area, pxy);
	wind_get(wh, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	while ( box.g_w && box.g_h )
	{
		if (rc_intersect(&work_area, &box))
		{
			if ((box.g_y + box.g_h) == (lowr_area.g_y + lowr_area.g_h))
			{		/* if we are in the last line	*/
				set_clip (TRUE, &box);
				rast_op (3, &lowr_area, &lowr_mfdb, &upr_area, &upr_mfdb);
				disp_line (rows-1);
				set_clip (FALSE, &box);
			}
			else		/* else we must update it without scrolling */
				disp_mesag (lin_buf, &box);
			
		}
		wind_get(wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	}
#if	(VERSION & NOFLICKER)		/* test if mouse is turned off		*/
	if (cl_mouse)
#endif
		graf_mouse(M_ON, 0x0L);
#if (VERSION & ACCESSORY)
	wind_update (END_UPDATE);
#endif
}


/*------------------------------*/
/*	rc_equal		*/
/*------------------------------*/
WORD
rc_equal(p1, p2)		/* tests for two rectangles equal	*/
GRECT		*p1, *p2;
{
	if ((p1->g_x != p2->g_x) ||
	    (p1->g_y != p2->g_y) ||
	    (p1->g_w != p2->g_w) ||
	    (p1->g_h != p2->g_h))
		return(FALSE);
	return(TRUE);
}


/*------------------------------*/
/*	rc_intersect		*/
/*------------------------------*/
WORD
rc_intersect(p1, p2)		/* compute inter of two rectangles	*/
GRECT		*p1, *p2;
{
	WORD		tx, ty, tw, th;

	tw = min(p2->g_x + p2->g_w, p1->g_x + p1->g_w);
	th = min(p2->g_y + p2->g_h, p1->g_y + p1->g_h);
	tx = max(p2->g_x, p1->g_x);
	ty = max(p2->g_y, p1->g_y);
	p2->g_x = tx;
	p2->g_y = ty;
	p2->g_w = tw - tx;
	p2->g_h = th - ty;
	return( (tw > tx) && (th > ty) );
}


/*------------------------------*/
/*	inside			*/
/*------------------------------*/
BOOLEAN	inside (x, y, pt)	/* determine if x,y is in rectangle	*/
WORD	x, y;
GRECT	*pt;
{
	if ( (x >= pt->g_x) && (y >= pt->g_y) &&
	    (x < pt->g_x + pt->g_w) && (y < pt->g_y + pt->g_h) )
		return(TRUE);
	else
		return(FALSE);
} /* inside */


/*------------------------------*/
/*	grect_to_array		*/
/*------------------------------*/
VOID
grect_to_array(area, array)	/* convert x,y,w,h to upr lt x,y and	*/
GRECT	*area;			/*		      lwr rt x,y	*/
WORD	*array;
{
	*array++ = area->g_x;
	*array++ = area->g_y;
	*array++ = area->g_x + area->g_w - 1;
	*array = area->g_y + area->g_h - 1;
}


/*------------------------------*/
/*	rast_op			*/
/*------------------------------*/
VOID
rast_op(mode, s_area, s_mfdb, d_area, d_mfdb)	/* bit block level trns	*/
WORD	mode;
GRECT	*s_area, *d_area;
MFDB	*s_mfdb, *d_mfdb;
{
	WORD	pxy[8];

	grect_to_array(s_area, pxy);
	grect_to_array(d_area, &pxy[4]);
	vro_cpyfm(vdi_handle, mode, pxy, s_mfdb, d_mfdb);
}

/*------------------------------*/
/*	do_open			*/
/*------------------------------*/
WORD
do_open(wh, org_x, org_y, x, y, w, h)	/* grow and open specified wdw	*/
WORD	wh;
WORD	org_x, org_y;
WORD	x, y, w, h;
{
	WORD	ret_code;

	graf_mouse(2,0x0L);
	graf_growbox(org_x, org_y, 21, 21, x, y, w, h);
	ret_code = wind_open(wh, x, y, w, h);
	graf_mouse(ARROW,0x0L);
	return(ret_code);
}


/*------------------------------*/
/*	do_close		*/
/*------------------------------*/
VOID
do_close(wh, org_x, org_y)	/* close and shrink specified window	*/
WORD	wh;
WORD	org_x, org_y;
{
	WORD	x, y, w, h;

	graf_mouse(2,0x0L);
	wind_get(wh, WF_CXYWH, &x, &y, &w, &h);
	wind_close(wh);
	graf_shrinkbox(org_x, org_y, 21, 21, x, y, w, h);
	graf_mouse(ARROW,0x0L);
}

/*------------------------------*/
/*	set_clip		*/
/*------------------------------*/
VOID
set_clip(clip_flag, s_area)	/* set clip to specified area		*/
WORD	clip_flag;
GRECT	*s_area;
{
	WORD	pxy[4];

	grect_to_array(s_area, pxy);
	vs_clip(vdi_handle, clip_flag, pxy);
}

/*------------------------------*/
/*	align_x			*/
/*------------------------------*/
WORD
align_x(x)		/* forces word alignment for column positon,	*/
WORD	x;		/*   rounding to nearest word			*/
{
	return((x & 0xfff0) + ((x & 0x000c) ? 0x0010 : 0));
}	



/*------------------------------*/
/*	align_h			*/
/*------------------------------*/
WORD
align_h(h)		/* forces 'line alignment' for wdw_h,		*/
WORD	h;		/*   rounding to nearest line			*/
{

	return(h - (h % gl_hbox) + 2);
}	


/*------------------------------*/
/*	align_y			*/
/*------------------------------*/
WORD
align_y(y, h)		/* align y position				*/
WORD	y, h;
{

	if (y < (gl_yfull + gl_hfull - h))
		return (y);	/* y is ok if window is in the screen	*/
	else			/* else position y so that you have no 'cutted' last line */
		return (y + (((gl_yfull + gl_hfull) - y) % gl_hbox) + 1 - gl_hbox);
}	


/*------------------------------*/
/*	string_addr		*/
/*------------------------------*/
LONG
string_addr(which)		/* returns a tedinfo LONG string addr	*/
WORD	which;
{
	LONG	where;

	rsrc_gaddr(R_STRING, which, &where);
	return (where);
} 


/*------------------------------*/
/*	wdw_size		*/
/*------------------------------*/
VOID
wdw_size(box, w, h)	/* compute window size for given w * h chars	*/
GRECT	*box;
WORD	w, h;
{
	WORD	pw, ph;

	vst_height(vdi_handle, disp_size,
	&gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);
	pw = w * gl_wbox + 1;
	ph = h * gl_hbox + 2;
	wind_calc(WC_BORDER, 0x0fff, gl_wbox*2, gl_hbox*4, pw, ph, &box->g_x, &box->g_y, &box->g_w, &box->g_h);
	box->g_x = align_x(box->g_x) - 1;
}

/*------------------------------*/
/*	wdw_col_rows		*/
/*------------------------------*/
VOID
wdw_col_rows(x, y)	/* compute columns and rows for a given window size */
WORD	x, y;
{


	y =  min (y, lowr_area.g_h + gl_hbox);
	vst_height(vdi_handle, disp_size,
	&gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);
	columns = x / gl_wbox;			/* columns = wide/char_wide */
	rows	= y / gl_hbox;			/* rows    = high/char_high */

}

/*------------------------------*/
/*	calc_sliders		*/
/*------------------------------*/
VOID
calc_sliders ()		/* compute the length and position of the two sliders	*/ 
{

	row_ptr = (start_ptr + min (row_offset,distnt (start_ptr, end_ptr, LINES, FALSE))) % LINES; 
	if (distnt (row_ptr, end_ptr, LINES, FALSE) < rows)		/* if there are less rows left as on window to type 	*/ 
		if (distnt (start_ptr, end_ptr, LINES, FALSE) > rows) 	/* and if there more than (rows) lines in buffer	*/
			row_ptr = sub_rptr (end_ptr, LINES, rows);	/* the position is (rows) lines less than the last line	*/
		else
			row_ptr = start_ptr;				/* if less the (rows) lines in buffer then use start_ptr*/
	row_offset = distnt (start_ptr, row_ptr, LINES, TRUE);
	v_sd_step = 1000 / (LINES - rows);
	h_sd_pos = h_sd_step * col_offset;
	h_sd_len = h_sd_step * columns;
	v_sd_pos = v_sd_step * row_offset;
	v_sd_len = v_sd_step * rows;

}

/*------------------------------*/
/*	calc_lowa_upa		*/	
/*------------------------------*/
VOID	calc_lowa_upa ()	/* calculate upper, lower and last line area */
{

	lowr_area.g_w = upr_area.g_w = lln_area.g_w = work_area.g_w;
	lowr_area.g_h = upr_area.g_h = max (min(work_area.g_h - gl_hbox,
		gl_yfull+gl_hfull-work_area.g_y-gl_hbox), 0);
	lowr_area.g_x = upr_area.g_x = lln_area.g_x = work_area.g_x;
	lowr_area.g_y = work_area.g_y + gl_hbox;
	upr_area.g_y = work_area.g_y;
	lln_area.g_h = ((gl_yfull + gl_hfull - work_area.g_y) > gl_hbox) ?
			gl_hbox : 0;
	lln_area.g_y = min (work_area.g_y + work_area.g_h - gl_hbox,
			    gl_yfull + gl_hfull -gl_hbox);
}


/*------------------------------*/
/*	set_sliders		*/
/*------------------------------*/
VOID	set_sliders ()		/* set vert. and hor. sliders */
{

	wind_set(disp_whndl, WF_VSLIDE, v_sd_pos);
	wind_set(disp_whndl, WF_VSLSIZE, v_sd_len, 0, 0 );
	wind_set(disp_whndl, WF_HSLIDE, h_sd_pos);
	wind_set(disp_whndl, WF_HSLSIZE, h_sd_len, 0, 0 );

}	

/*------------------------------*/
/*	get_fname		*/
/*------------------------------*/
VOID	get_fname (fn)		/* make file nameout of fs_iinpath and fs_iinsel */
BYTE	*fn;
{

	BYTE	*ptr, *last;
	
	last = fn;		/* last appearence of '\' or ':'		*/
	ptr = fs_iinpath;
	while (*ptr)
	{
		if ((*ptr) == '\\' || (*ptr) == ':') 
			last = fn + 1;	/* save position		*/
		else if ((*ptr) == '*' || (*ptr) == '?')
		{
			strcpy (last, fs_iinsel); /* cat. the two strings */
			return;
		}
		*(fn++) = *(ptr++);	/* copy char in caller's buffer	*/
	}
	if (fn[-1] != '\\') 		/* add a backslash if there is no */
		*(fn++) = '\\';
	strcpy (fn, fs_iinsel);		/* and copy selection string	*/
}

/*------------------------------*/
/*	disp_message		*/
/*------------------------------*/
VOID
disp_mesag(strptr, clip_area)	/* display message applying input clip	*/
BYTE	strptr [LINES] [LENGTH];
GRECT	*clip_area;
{
	WORD	pxy[4];
	WORD	ycurr, linecnt = 0, drow_ptr;

	set_clip(TRUE, clip_area);
	vsf_interior(vdi_handle, 1);
	vsf_color(vdi_handle, WHITE);
	grect_to_array(&work_area, pxy);
	if (ev_moflags)
		graf_mouse(M_OFF, 0x0L);
	vr_recfl(vdi_handle, pxy);	/* clear entire message area	*/

	vsl_color(vdi_handle,BLACK);
	vswr_mode(vdi_handle,MD_REPLACE);
	vsl_type (vdi_handle,FIS_SOLID);
	vswr_mode(vdi_handle, 1);
	ycurr = work_area.g_y - 1;
	drow_ptr = row_ptr;
	do
	{
		ycurr += gl_hbox;
		v_gtext(vdi_handle, work_area.g_x - 1, ycurr, strptr [drow_ptr] + col_offset);
		drow_ptr = (++drow_ptr) % LINES;
	}
	while (linecnt++ < rows && drow_ptr != start_ptr &&
		drow_ptr != end_ptr);

	if (ev_moflags)
		graf_mouse(M_ON, 0x0L);
	set_clip(FALSE, clip_area);
}


/*------------------------------*/
/*	disp_line		*/
/*------------------------------*/
VOID
disp_line (line)	/* display one single line	*/
WORD	line;
{
	WORD	pxy[4];
	WORD	ycurr, linecnt = 0, drow_ptr, xcurr;

	vsf_interior(vdi_handle, 1);
	vsf_color(vdi_handle, WHITE);
	if (ev_moflags)				/* if mouse is in the window */
		graf_mouse(M_OFF, 0x0L);	/* switch it off	     */
	grect_to_array(&work_area, pxy);
	vsl_color(vdi_handle,BLACK);
	vswr_mode(vdi_handle,MD_REPLACE);
	vsl_type (vdi_handle,FIS_SOLID);
	vswr_mode(vdi_handle, 1);
	ycurr = work_area.g_y - 1;
	xcurr = (work_area.g_x + gl_wbox - 2) - ((work_area.g_x + gl_wbox - 2) % gl_wbox);
	drow_ptr = row_ptr;
/*	ycurr += gl_hbox * (line +1); 		/* add line offset	    */
 	drow_ptr = (row_ptr + line+1) % LINES;	/* calc line position in buffer */
 	if (distnt (start_ptr, drow_ptr, LINES, TRUE) < distnt (start_ptr, end_ptr, LINES, FALSE))
		v_gtext(vdi_handle, xcurr, ycurr, lin_buf [drow_ptr] + col_offset);
*/	do
	{
		ycurr += gl_hbox;
		if (line == linecnt)
			v_gtext(vdi_handle, xcurr, ycurr, lin_buf [drow_ptr] + col_offset);
		drow_ptr = (++drow_ptr) % LINES;
	}
	while (linecnt++ < line && drow_ptr != start_ptr &&
		drow_ptr != end_ptr);
	if (ev_moflags)				/* switch it again on	    */
		graf_mouse(M_ON, 0x0L);
}


/*------------------------------*/
/*	clear_disp		*/
/*------------------------------*/
VOID
clear_disp (clip_area)	/* clear message display applying input clip	*/
GRECT	*clip_area;
{
	WORD	pxy[4];

	set_clip(TRUE, clip_area);
	vsf_interior(vdi_handle, 1);
	vsf_color(vdi_handle, WHITE);
	grect_to_array(&work_area, pxy);
	graf_mouse(M_OFF, 0x0L);
	vr_recfl(vdi_handle, pxy);	/* clear entire message area	*/
	graf_mouse(M_ON, 0x0L);
	set_clip(FALSE, clip_area);
}






/*------------------------------*/
/*	do_redraw		*/
/*------------------------------*/
VOID
do_redraw(wh, area)		/* redraw message applying area clip	*/
WORD	wh;
GRECT	*area;
{
	GRECT	box;

	wind_update (BEG_UPDATE);
	graf_mouse(M_OFF, 0x0L);
	wind_get(wh, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	while ( box.g_w && box.g_h )
	{
		if (rc_intersect(area, &box))
		{
			if (wh == disp_whndl)
			{
				disp_mesag(lin_buf, &box);
			}
		}
		wind_get(wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	}
	graf_mouse(M_ON, 0x0L);
	wind_update (END_UPDATE);
}



/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Message Handling			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	hndl_mesag		*/
/*------------------------------*/
BOOLEAN	hndl_mesag()
{
	GRECT	box;
	BOOLEAN	done; 
	WORD	wdw_hndl, i;

	done = FALSE;
	wdw_hndl = gl_rmsg[3];			/* wdw handle of mesag	*/
	switch( gl_rmsg[0] )			/* switch on disp of msg*/
	{
	case AC_OPEN:				/* do accessory open	*/
		if ( (gl_rmsg[4] == gl_itemdisp) && 
		 	(!disp_whndl))		/* unless already open	*/
		{
			row_offset = col_offset = row_ptr = 0;
			graf_mouse(HOUR_GLASS, 0x0L);
			/* 0x0fff = NAME | CLOSER | MOVER | INFO SIZE | UPARROW | DNARROW | VSLIDE | HSLIDE */
			disp_whndl = wind_create(0x0fff, align_x(gl_xfull)-1, gl_yfull, gl_wfull, gl_hfull);
			if (disp_whndl == -1)
			{
#if (VERSION & RSC_FILE)
				form_alert (1,string_addr (TYPEERR1));
#else
				form_alert (1, ADDR(M_ERR1));
#endif
				done = TRUE;
			}
			wdw_size(&box, INIT_COLS, INIT_ROWS);
#if	(VERSION & ACCESSORY)			/* open from menu area	*/
			do_open(disp_whndl, gl_wbox*2, gl_hbox*4, box.g_x, box.g_y, box.g_w, box.g_h);
#else						/* open from screen cntr*/
			do_open(disp_whndl, gl_wfull/2, gl_hfull/2, box.g_x, box.g_y, box.g_w, box.g_h);
#endif
			wind_set(disp_whndl, WF_NAME,(WORD) LLOWD(ADDR(wdw_title)), (WORD) LHIWD(ADDR(wdw_title)), 0, 0);
			wind_set(disp_whndl, WF_INFO,(WORD) LLOWD(ADDR(wdw_info)), (WORD) LHIWD(ADDR(wdw_info)), 0, 0); 
			wind_get(disp_whndl, WF_WXYWH,	&work_area.g_x, &work_area.g_y, &work_area.g_w, &work_area.g_h);
			wind_get(disp_whndl, WF_CXYWH,	&border_area.g_x, &border_area.g_y, &border_area.g_w, &border_area.g_h);
			clear_disp (&work_area);
			calc_lowa_upa ();
			wdw_col_rows (work_area.g_w, work_area.g_h);
			calc_sliders ();
			set_sliders ();
			done = -1;
		}
		else				/* do window topped	*/
			wind_set(disp_whndl, WF_TOP, 0, 0, 0, 0);
		break;

	case WM_REDRAW:				/* do redraw wdw contnts*/
		do_redraw(wdw_hndl, (GRECT *) &gl_rmsg[4]);
		break;

	case WM_TOPPED:				/* do window topped	*/
		wind_set(wdw_hndl, WF_TOP, 0, 0, 0, 0);
		break;

	case AC_CLOSE:				/* do accessory close	*/
		done = TRUE;
		disp_whndl = 0;
		break;

	case WM_CLOSED:				/* do window closed	*/
#if	(VERSION & ACCESSORY)			/* close to menu bar	*/
		do_close(disp_whndl, gl_wbox*4, gl_hbox/2);
#else						/* close to screen cntr	*/
		do_close(disp_whndl, gl_wfull/2, gl_hfull/2);
#endif
		wind_delete(disp_whndl);
		disp_whndl = 0;
		done = TRUE;
		break;

	case WM_SIZED:				/* do window size or	*/
	case WM_MOVED:				/* do window move	*/
		wind_set(wdw_hndl, WF_CXYWH, align_x(gl_rmsg[4])-1, align_y (gl_rmsg[5],
			align_h (gl_rmsg[7])), gl_rmsg[6], align_h (gl_rmsg[7]));
		wind_get(disp_whndl, WF_WXYWH,	&work_area.g_x, &work_area.g_y, &work_area.g_w, &work_area.g_h);
			wind_get(disp_whndl, WF_CXYWH,	&border_area.g_x, &border_area.g_y, &border_area.g_w, &border_area.g_h);
		calc_lowa_upa ();
		wdw_col_rows (work_area.g_w, work_area.g_h);
		calc_sliders ();
		set_sliders ();
		break;

	case WM_FULLED:
		do_full (wdw_hndl);
		wind_get(disp_whndl, WF_WXYWH, &work_area.g_x, &work_area.g_y, &work_area.g_w, &work_area.g_h);
		wind_get(disp_whndl, WF_CXYWH,	&border_area.g_x, &border_area.g_y, &border_area.g_w, &border_area.g_h);
		calc_lowa_upa ();
		wdw_col_rows (work_area.g_w, work_area.g_h);
		calc_sliders ();
		set_sliders ();
		break;

	case WM_ARROWED:
		switch (gl_rmsg [4])
		{
		case 0:				/* page up	 	*/
			row_offset = (row_offset >= rows) ? row_offset - rows : 0;
			break;
		case 1:				/* page down		*/
			row_offset = (row_offset < LINES - (2 * rows)) ? row_offset + rows : LINES - rows;
			break;
		case 2:				/* row up		*/
			row_offset = (row_offset >= 1) ? row_offset - 1 : 0;
			break;
		case 3:				/* row down		*/
			row_offset = (row_offset < LINES - rows) ? row_offset + 1 : LINES - rows;
			break;
		case 4:				/* page left		*/ 
			col_offset = (col_offset >= columns) ? col_offset - columns : 0;
			break;
		case 5:				/* page right		*/
			col_offset = (col_offset < LENGTH - columns) ? col_offset + columns : LENGTH - 1;
			break;
		case 6:				/* column left		*/
			col_offset = (col_offset >= 1) ? col_offset - 1 : 0;
			break;
		case 7:				/* column right		*/
			col_offset = (col_offset < LENGTH - columns) ? col_offset + 1 : LENGTH - 1;
			break;
		}
		calc_sliders ();
		set_sliders ();
		disp_mesag (lin_buf, &work_area); 
		break;

	case WM_HSLID:
		col_offset = max ((gl_rmsg [4] / h_sd_step) - 1, 0);
		calc_sliders ();
		wind_set(disp_whndl, WF_HSLIDE, h_sd_pos);
		wind_set(disp_whndl, WF_HSLSIZE, h_sd_len, 0, 0 );
		disp_mesag (lin_buf, &work_area); 
		break;

	case WM_VSLID:
		row_offset = max (gl_rmsg [4] / v_sd_step - 1, 0);
		calc_sliders ();
		wind_set(disp_whndl, WF_VSLIDE, v_sd_pos);
		wind_set(disp_whndl, WF_VSLSIZE, v_sd_len, 0, 0 );
		disp_mesag (lin_buf, &work_area); 
		break;
		
	default:
		break;
	} /* switch */
	return(done);
} /* hndl_mesag */


/*------------------------------*/
/*	do_full			*/
/*------------------------------*/
VOID
do_full(wh)	/* depending on current window state, either make window*/
WORD	wh;	/*   full size -or- return to previous shrunken size	*/
{
	GRECT	curr;
	GRECT	full;

	graf_mouse(M_OFF,0x0L);
	wind_get(wh, WF_CXYWH, &curr.g_x, &curr.g_y, &curr.g_w, &curr.g_h);
	wind_get(wh, WF_FXYWH, &full.g_x, &full.g_y, &full.g_w, &full.g_h);
	align_h (full.g_h);
	if (rc_equal(&curr, &full))
	{					/* is full now so change*/
		/**/					/*   to previous	*/
		graf_shrinkbox(gp_x, gp_y, gp_w, gp_h,
			full.g_x, full.g_y, full.g_w, full.g_h);
		wind_set(wh, WF_CXYWH, gp_x, gp_y, gp_w, gp_h);
	}
	else
	{					/* is not full so make	*/
		/**/					/*   it full		*/
		graf_growbox(curr.g_x, curr.g_y, curr.g_w, curr.g_h,
			full.g_x, full.g_y, full.g_w, full.g_h);
		wind_get(disp_whndl, WF_CXYWH, &gp_x, &gp_y, &gp_w, &gp_h); 
		wind_set(wh, WF_CXYWH, full.g_x, full.g_y, full.g_w, full.g_h);
	}
	graf_mouse(M_ON,0x0L);
}

/*

Page*/

/*************************************************************************/
/*************************************************************************/
/****								      ****/
/****			       Disp a file			      ****/
/****								      ****/
/*************************************************************************/
/*************************************************************************/
WORD
typ_fil ()			/* get filename, open file, disp file , close file */
{

	WORD	path, ready = FALSE, ev_mwitch, done;
	LONG	delay = SCROLL_DELAY;		/* defined in dispver.h	*/
	WORD	ev_mmox, ev_mmoy, ev_mmobutton, ev_mmokstate,
		ev_mkreturn, ev_mbreturn, ev_mflags, ev;


#if (VERSION & SHOWMESS)
	if (form_alert (1, string_addr (TYPEMSG1)) == 2)
	{
		gl_rmsg[0] = WM_CLOSED;		/* Simulate wdw close */
		gl_rmsg[4] = gl_itemdisp;
		hndl_mesag();
#if (VERSION & ACCESSORY)
			wind_update (END_UPDATE);
#endif
		return (-1);
	}
#endif
	if (fs_iinpath [0])
		fs_iinpath [0] = dos_gdrv () + 'A';
Retry:	fs_ireturn = fsel_input (ADDR(fs_iinpath),ADDR(fs_iinsel), &fs_iexbutton);
	get_fname (filename);
	strcpy (wdw_tinfo, wdw_info);
	strcat (wdw_tinfo, filename);
	wind_set(disp_whndl, WF_INFO,(WORD) LLOWD(ADDR(wdw_tinfo)), (WORD) LHIWD(ADDR(wdw_tinfo)), 0, 0); 
	if (!fs_ireturn || !fs_iexbutton)
	{
		gl_rmsg[0] = WM_CLOSED;		/* Simulate wdw close */
		gl_rmsg[4] = gl_itemdisp;
		hndl_mesag();
#if (VERSION & ACCESSORY)
		wind_update (END_UPDATE);
#endif
		return (-1);
	}
	if ((path = open(filename)) < 0)
	{
#if (VERSION & RSC_FILE)
		if (form_alert (1, string_addr (TYPEERR2)) == 1)
		{
			goto Retry;
		}
#else
		form_alert (1, ADDR(M_ERR2));
#endif
		gl_rmsg[0] = WM_CLOSED;		/* Simulate wdw close */
		gl_rmsg[4] = gl_itemdisp;
		hndl_mesag();
#if (VERSION & ACCESSORY)
		wind_update (END_UPDATE);
#endif
		return (-1);
	}
	start_ptr = end_ptr = row_ptr = col_offset = row_offset = 0;
	graf_mouse(ARROW,0x0L);
	read_blk (path);
	disp_mesag (lin_buf, &work_area); 
#if (VERSION & ACCESSORY)
	wind_update (END_UPDATE);
#endif
	ev_mflags = MU_M1 | MU_MESAG | MU_TIMER;
	graf_mkstate (&ev_mmox, &ev_mmoy, &ev_mmokstatem, &ev);
	if (inside (ev_mmox, ev_mmoy, &border_area))	/* if mouse is in the wdw	*/
	{	
		ev_moflags = 1;				/* wait button	and 		*/
		ev_mflags |= MU_BUTTON;			/* set inside flag		*/
	}
	else
		ev_moflags = 0;				/* else clear inside flag	*/
	do
	{
		if (distnt (row_ptr , end_ptr, LINES, FALSE) > rows)
		{
			row_ptr = ++row_ptr % LINES;	/* incr buffer ptr */
			v_sd_pos += v_sd_step;		/* incr v-slider position */
			wind_set(disp_whndl, WF_VSLIDE, v_sd_pos);	/* set v-slider */
			scroll_up ();
		}
		else if (! eof (path))
		{
			start_ptr =  ++start_ptr % LINES;
			row_ptr = ++row_ptr % LINES;
			read_blk (path);
/*			wind_set(disp_whndl, WF_VSLIDE, 1000);	/* set v-slider */
*/			scroll_up ();
		}
		else
			ready = TRUE;
		do 
		{
			ev_mwitch = evnt_multi (ev_mflags, 2, 0x01, 0x01,
				ev_moflags, work_area.g_x, work_area.g_y,
				work_area.g_w, work_area.g_h,
				0,0,0,0,0,
				ad_rmsg,
				LLOWD(delay), LHIWD(delay),
				&ev_mmox, &ev_mmoy,
				&ev_mmobutton, &ev_mmokstate,
				&ev_mkreturn, &ev_mbreturn);
			if (ev_mwitch & MU_MESAG)
			{
				row_offset = distnt (start_ptr, row_ptr, LINES, TRUE);
				done = hndl_mesag();		/* handle event message	*/
				if (done)
				{
					close (path);
					ev_moflags = TRUE;
					return (-1);
				}
			}				
			if (ev_mwitch & MU_M1)
			{
				ev_moflags = ! ev_moflags;
				if (ev_moflags)
					ev_mflags |= MU_BUTTON;
				else
				{
					ev_mflags &= ~MU_BUTTON;
					ev_mwitch &= ~MU_BUTTON;
				}
			}
			if (ev_mwitch & MU_BUTTON)
			{
				if (ev_mbreturn == 1)
				{
				    do
				    {
					ev = evnt_multi (0x12, 2, 0x01, 0x01,
						0,0,0,0,0,0,0,0,0,0,
						ad_rmsg,0,0,
						&ev_mmox, &ev_mmoy,
						&ev_mmobutton, &ev_mmokstate,
						&ev_mkreturn, &ev_mbreturn);
						if (ev & MU_MESAG)
						{
							row_offset = distnt (start_ptr, row_ptr, LINES, TRUE);
							done = hndl_mesag();		/* handle event message	*/
							if (done)
							{
								close (path);
								ev_moflags = TRUE;
								return (-1);
							}
						}				
				    }
				    while (!(ev & MU_BUTTON));
#if (VERSION & DOUBLE_CLICK)
				    if (ev_mbreturn == 2)
				    {				
					gl_rmsg[0] = WM_CLOSED;		/* Simulate wdw close */
					gl_rmsg[4] = gl_itemdisp;
					hndl_mesag();
					ev_moflags = TRUE;
					close (path);
					return (-1);
				    }
#endif
				}
#if (VERSION & DOUBLE_CLICK)
				else
				{				
					gl_rmsg[0] = WM_CLOSED;		/* Simulate wdw close */
					gl_rmsg[4] = gl_itemdisp;
					hndl_mesag();
					ev_moflags = TRUE;
					ev_moflags = TRUE;
					close (path);
					return (-1);
				}
#endif
			}
		}
		while (! (ev_mwitch & MU_TIMER));

	}
	while (!ready);
	ev_moflags = TRUE;
	close (path);
	row_offset = distnt (start_ptr, row_ptr, LINES, TRUE);	/* calculate the absolut row_offset */
	return (0);
}



/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Disp Event Handler			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	disp			*/
/*------------------------------*/
disp()
{
	BOOLEAN	done;

	/**/					/* loop handling user	*/
	/**/					/*   input until done	*/
	done = FALSE;				/*   -or- if DESKACC	*/
	while( !done )				/*   then forever	*/
	{
		ev_which = evnt_mesag(ad_rmsg);	/* wait for message	*/
#if (VERSION & ACCESSORY)
		wind_update(BEG_UPDATE);	/* begin window update	*/
#endif
		done = hndl_mesag();		/* handle event message	*/
		if (done == -1)
			typ_fil ();		/* print file and end window update */
#if	(VERSION & ACCESSORY)
		else
			wind_update(END_UPDATE);/* end window update	*/
		done = FALSE;	/* never exit loop for desk accessory	*/
#endif
	}
}


/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Termination				     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	disp_term		*/
/*------------------------------*/
disp_term()
{
#if	(VERSION & ACCESSORY)
	return;				/* desk accessory never ends	*/
#else
	v_clsvwk( vdi_handle );		/* close virtual work station	*/
	appl_exit();			/* application exit		*/
#endif
}

/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Initialization			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	disp_init		*/
/*------------------------------*/
WORD
disp_init()
{
	WORD	i;
	WORD	work_in[11];

	gl_apid = appl_init();			/* initialize libraries	*/
#if	(VERSION & RSC_FILE)
	if (! rsrc_load (ADDR(M_RSC)))
	{
		graf_mouse (ARROW, 0x0L);
		form_alert (1, ADDR (M_ERR3));
		return (FALSE);
	}
#endif

#if	(VERSION & ACCESSORY)
	wind_update(BEG_UPDATE);
#endif
	for (i=0; i<10; i++)
	{
		work_in[i]=1;
	}
	work_in[10]=2;
	gem_handle = graf_handle(&gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);
	vdi_handle = gem_handle;
	v_opnvwk(work_in,&vdi_handle,work_out);	/* open virtual work stn*/
	disp_size = work_out[48];		/* get system font hbox	*/
	lowr_mfdb.mp = 0x0L;			/* lower and upper scroll */
	upr_mfdb.mp = 0x0L;			/* area physical devices  */

#if	(VERSION & ACCESSORY)			/* enter disp in menu	*/
	gl_itemdisp = menu_register(gl_apid, ADDR(M_ITITLE));
#else
	if (vdi_handle == 0)
		return(FALSE);
#endif
	/**/					/* init. message address*/
	ad_rmsg = ADDR((BYTE *) &gl_rmsg[0]);
	wind_get(DESK, WF_WXYWH, &gl_xfull, &gl_yfull, &gl_wfull, &gl_hfull);
	return(TRUE);
}

/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Main Program			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	GEMAIN			*/
/*------------------------------*/
GEMAIN ()
{
	if (disp_init())			/* initialization	*/
	{
#if	(VERSION & ACCESSORY)
		wind_update(END_UPDATE);
		disp ();
#else						/* simulate AC_OPEN	*/
		gl_rmsg[0] = AC_OPEN;
		gl_rmsg[4] = gl_itemdisp;
		hndl_mesag();
		if (! typ_fil ())
			disp ();
		disp_term();			/* termination		*/
#endif
	}
	appl_exit ();

}
