/*************************************************************************
**       Copyright 1999, Caldera Thin Clients, Inc.                     ** 
**       This software is licenced under the GNU Public License.        ** 
**       Please see LICENSE.TXT for further information.                ** 
**                                                                      ** 
**                  Historical Copyright                                ** 
**									**
**									**
**  Copyright (c) 1987, Digital Research, Inc. All Rights Reserved.	**
**  The Software Code contained in this listing is proprietary to	**
**  Digital Research, Inc., Monterey, California and is covered by U.S.	**
**  and other copyright protection.  Unauthorized copying, adaptation,	**
**  distribution, use or display is prohibited and may be subject to 	**
**  civil and criminal penalties.  Disclosure to others is prohibited.	**
**  For the terms and conditions of software code use refer to the 	**
**  appropriate Digital Research License Agreement.			**
**									**
*************************************************************************/

#include "portab.h"
#include "gsxdef.h"
#include "defines.h"

#define MIN(A,B) ((A) <= (B) ? (A) : (B))
#define MAX(A,B) ((A) >= (B) ? (A) : (B))

EXTERN WORD	TOKEN, xmn_clip, ymn_clip, xmx_clip, ymx_clip;
EXTERN WORD	YS_MIN;
EXTERN WORD	CONTRL[], INTIN[], PTSIN[];
EXTERN WORD	dev_tab[], inq_tab[];
EXTERN WORD	dln_byte;
EXTERN WORD	g_plane;

#ifdef COLOR_PRINTER
EXTERN WORD	plane_sz;
#endif

EXTERN VOID	PT_OP_BY ();
EXTERN VOID	PT_INTIN ();
EXTERN VOID	GT_INTIN ();
EXTERN WORD	MUL_DIV ();
EXTERN	WORD	f_open();		/* imageasm.a86 */
EXTERN	WORD	f_close();
EXTERN	WORD	f_read();
EXTERN	WORD	f_seek();
EXTERN	WORD	allo_mem();
EXTERN	WORD	free_mem();
EXTERN VOID	b_move ();
EXTERN VOID	b_stuff ();
EXTERN VOID	b_or ();
EXTERN VOID	b_shl1or ();
EXTERN VOID	b_shl2or ();
EXTERN VOID	b_xlat ();


#define MAX_IN_FILE	80	/* Maxinum characters if .IMG file name */
#define NUL		0	/* String terminator */
#define IMGBUF_SIZE	2048	/* .IMG file buffer (1 total) */
#define MAX_HEADER	16	/* Maximum # of header bytes needed to be */
				/*  decoded.  Actual header may be larger */
				/*  but must be smaller than IMGBUF_SIZE. */
 
#define	IMGINFO	struct img_info
struct img_info {		/* image info - 1 for each bit image call   */
    WORD		id;		/* bit_image call number first = 1  */
    WORD		aspect;		/* preserve aspect ratio flag       */
    WORD		x_scale;	/* 1 => integer scale, 0 => fract   */
    WORD		y_scale;	/* 1 => integer scale, 0 => fract   */
    WORD		h_align;	/* horizontal alignment 0, 1, or 2  */
    WORD		v_align;	/* vertical alignment 0, 1, or 2    */
    BYTE		file_name[MAX_IN_FILE]; /* .IMG file name 	    */
    WORD		xul;		/* device x upper left coordinate   */
    WORD		yul;		/* device y upper left coordinate   */
    WORD		xlr;		/* device x lower right coordinate  */
    WORD		ylr;		/* device y lower right coordinate  */
    WORD		save_xul;	         /* for reinit_struct()	    */
    WORD		save_yul;                /* for reinit_struct()	    */
    WORD		save_xlr;	         /* for reinit_struct()	    */
    WORD		save_ylr;	         /* for reinit_struct()	    */
    WORD		rotate;		/* rotation angle in 1/10 degrees   */
    WORD		foreground;	/* foreground & background color    */
    WORD		background;	/*  for mono image on color print.  */
    WORD		version;	/* .IMG version from header	    */
    WORD		header_len;	/* .IMG header length from header   */
    WORD		nplanes;	/* .IMG # of planes from header	    */
    WORD		patlen;		/* .IMG pattern length from header  */
    WORD		pixwd;		/* .IMG pixel width from header     */
    WORD		pixht;		/* .IMG pixel height from header    */
    WORD		scanwd;		/* .IMG scan width from header      */
    WORD		scanht;		/* .IMG scan height from header	    */
    WORD		plane_size;	/* scan width rounded up to * 8	    */
    WORD		data_size;	/* plane_size * nplanes		    */
    UBYTE		*data;		/* dynamic .IMG line buffer	    */
    WORD		v_repeat;	/* current vertical repeat from .IMG*/
    WORD		line_number;	/* current line number in .IMG	    */
    WORD		record_num;	/* current .IMG record number	    */
    WORD		record_cnt;	/* # bytes left in .IMG record	    */
    WORD		clip_xul;	/* x left clipping coordinate	    */
    WORD		clip_xlr;	/* x right clipping coordinate	    */
    WORD		clip_yul;	/* y upper clipping coordinate      */
    WORD		clip_ylr;	/* y lower clipping coordinate      */
    WORD		save_clipxul;	         /* for reinit_struct()	    */
    WORD		save_clipyul;	         /* for reinit_struct()	    */
    WORD		save_clipxlr;	         /* for reinit_struct()	    */
    WORD		save_clipylr;            /* for reinit_struct()	    */
    WORD		srcxsize;	/* .IMG x size given rotation	    */
    WORD		srcysize;	/* .IMG y size given rotation       */
    WORD		dstxsize;	/* resultant device x size	    */
    WORD		dstysize;	/* resultant device y size	    */
    WORD		srcxpix;	/* .IMG x pixel size given rotation */
    WORD		srcypix;	/* .IMG y pixel size given rotation */
    WORD		ddarem;		/* dda remainder for continuing	    */
    UBYTE		look_up[8];	/* image -> device bit map (color)  */
    IMGINFO		*link;		/* link to next .IMG's structure    */
};

static IMGINFO	*imgbase;	/* base of .IMG structure list	    */
static IMGINFO	*imgptr;	/* pointer to current .IMG structure*/
static UBYTE	imgbuf[IMGBUF_SIZE]; /* .IMG record buffer	    */
static WORD	imgbufcnt;	/* # of bytes left in current record*/
static WORD	imgbufrec;	/* record number (0 = first)	    */
static UBYTE	*imgbufptr;	/* pointer into next byte of imgbuf */
static UBYTE	*imgdataptr;	/* imgptr->data for convenience     */
static WORD	imghandle;	/* file handle for active .IMG      */
static UBYTE	*raster;	/* pointer into raster slice buffer */
static WORD	clip_xul;	/* current x left clip (fixed)      */
static WORD	clip_xlr;	/* current x right clip (fixed)     */
static WORD	clip_yul;	/* current y top clip (varying)     */
static WORD	clip_ylr;	/* current y bottom clip (varying)  */
static UBYTE	clrbit[8] = {0x7f, 0xbf, 0xdf, 0xef,
				     0xf7, 0xfb, 0xfd, 0xfe};
static UBYTE	setbit[8] = {0x80, 0x40, 0x20, 0x10,
				     0x08, 0x04, 0x02, 0x01};
WORD		images = 0;	/* number of images: initialized by v_opnwk */


/*****************************************************************************
*
*  status = c_image ()
*
*	Called from c_escape (case 23 or 101) in monoprin.c
*
*	Accept bit image calls, allocate structures for bit image data, and
*	insert token into display list so image will be output at replay time.
*
*	input:
*		CONTRL[3] = INTIN count (to determine file name length)
*		CONTRL[5] = escape code for v_bit_image (23) or 
*			    v_xbit_image (101)
*		INTIN[0]  = aspect flag
*			    ignore aspect ratio = 0, honor aspect ratio = 1
*		INTIN[1]  = x axis scaling
*			    fractional scaling = 0, integer scaling = 1
*		INTIN[2]  = y axis scaling
*			    fractional scaling = 0, integer scaling = 1
*		INTIN[3]  = horizontal alignment
*			    left = 0, center = 1, right = 2
*		INTIN[4]  = vertical alignment
*			    top = 0, center = 1, bottom = 2
*	    for v_bit_image call (CONTRL[5] == 23)
*		INTIN[5] : INTIN[CONTRL[3] = .IMG file name (1 char / word)
*	    for v_xbit_image call (CONTRL[5] == 101)
*		INTIN[5]  = image rotation angle in 1/10 degrees
*		INTIN[6]  = foreground color index for mono bit images on
*			    color images
*		INTIN[7]  = background color index for mono bit images on
*			    color images
*		INTIN[8] : INTIN[CONTRL[3] = .IMG file name (1 char / word)
*	output:
*		status =  0 if image file succesfully processed
*			 -1 if memory allocation or file access problem
*		imgptr structure initialized
*		Display list entry containing bit image request number
*	notes:
*		Each bit image call requires memory to be allocated for
*		a control data structure of about 10 paragraphs and for
*		a single bit image line (all planes, byte for each bit,
*		lines a multiple of 8 bits).  These data structures are
*		linked so there is "no" limit to their number.
*		Only 1 file is open at any instance of time.
*
*****************************************************************************/
WORD	c_image()
{
    WORD		i;
    WORD		status;
    WORD		mem_avail;
    BYTE		*fptr;
    IMGINFO		*parentptr;		/* parent of list entry */
    WORD		newxsize;		/* x and y device rectangle */
    WORD		newysize;		/*  size after adjusting    */
    WORD		offset;			/* offset for alignments  */
    BYTE		headerb[MAX_HEADER];	/* buffer for .IMG header */
    WORD		*headerw;		/* WORD access to header */
    BYTE		t;
    WORD		nread;
    EXTERN	VOID	set_scale();
    
#ifdef COLOR_PRINTER
			/* map vdi colors to RGB where bit 0 = R,        */
			/*  bit 1 = G, bit 2 = B (except WHITE = 000 and */
			/*  BLACK = 111)				 */
    static UBYTE	vdi_to_image[8] = {0, 7, 1, 2, 4, 6, 3, 5};
#endif

    imgptr = NULL;	/* each bit image requires an IMGINFO structure */
    imghandle = 0;	/* assume 0 is invalid to close on aborted read */
    status = allo_mem((sizeof(IMGINFO) + 15) / 16, &imgptr, &mem_avail);
    if (status != 0) 
	    goto abort;

    imgptr->data = NULL;	/* initially no line buffer */
    imgptr->link = NULL;	/* link to next bit image to process */
    imgptr->id = images + 1;
    imgptr->aspect = INTIN[0];
    imgptr->x_scale = INTIN[1];
    imgptr->y_scale = INTIN[2];
    imgptr->h_align = INTIN[3];
    imgptr->v_align = INTIN[4];

    if (CONTRL[5] == 101) {		/* v_xbit_image call */
	imgptr->rotate = INTIN[5];
	imgptr->foreground = INTIN[6];
	imgptr->background = INTIN[7];
	i = 8;
    } else {				/* v_bit_image call */
	imgptr->rotate = 0;
	imgptr->foreground = 1;
	imgptr->background = 0;
	i = 5;
    }					/* adjust rotate to be multiple 900 */
    while (imgptr->rotate < 0) imgptr->rotate += 3600;
    while (imgptr->rotate >= 3600) imgptr->rotate -= 3600;
    imgptr->rotate = ((imgptr->rotate + 450) / 900) * 900;
    if (imgptr->rotate == 3600) imgptr->rotate = 0;

    fptr = imgptr->file_name;
    while (i < CONTRL[3]) *fptr++ = INTIN[i++];
    *fptr = NUL;
    imgptr->xul = PTSIN[0];
    imgptr->yul = PTSIN[1];
    imgptr->xlr = PTSIN[2];
    imgptr->ylr = PTSIN[3];

    status = f_open(imgptr->file_name, &imghandle);
    if (status != 0)
	    goto abort;

    status = f_read(imghandle, headerb, MAX_HEADER, &nread);
    if (status != 0 || nread < 16)
	    goto abort;

    for (i = 0; i < nread; i += 2) {	/* swap bytes in header */
	t = headerb[i];
	headerb[i] = headerb[i+1];
	headerb[i+1] = t;
    }
    headerw = (WORD *) headerb;		/* will access them as words */
    if (headerw[0] != 1)
	    goto abort;

    imgptr->version = headerw[0];
    imgptr->header_len = headerw[1];
    imgptr->nplanes = headerw[2];
    imgptr->patlen = headerw[3];
    imgptr->pixwd = headerw[4];
    imgptr->pixht = headerw[5];
    imgptr->scanwd = headerw[6];
    imgptr->scanht = headerw[7];
    status = f_close(imghandle);
    if (status != 0)
	    goto abort;

    imgptr->plane_size = ((imgptr->scanwd + 7) / 8) * 8;
    imgptr->data_size = imgptr->plane_size * imgptr->nplanes;
    status = allo_mem((imgptr->data_size + 15) / 16, &imgptr->data, \
								  &mem_avail);
    if (status != 0)
	    goto abort;

    imgptr->v_repeat = 0;
    imgptr->line_number = 0;
    imgptr->record_num = 0;
    imgptr->record_cnt = 0;
/* save original device rect. defintion: we'll always clip to at least it */
    imgptr->clip_xul = imgptr->xul;
    imgptr->clip_xlr = imgptr->xlr;
    imgptr->clip_yul = imgptr->yul;
    imgptr->clip_ylr = imgptr->ylr;
    if (imgptr->rotate == 0 || imgptr->rotate == 1800) {
	imgptr->srcxsize = imgptr->scanwd;
	imgptr->srcysize = imgptr->scanht;
	imgptr->srcxpix = imgptr->pixwd;
	imgptr->srcypix = imgptr->pixht;
    } else {
	imgptr->srcxsize = imgptr->scanht;
	imgptr->srcysize = imgptr->scanwd;
	imgptr->srcxpix = imgptr->pixht;
	imgptr->srcypix = imgptr->pixwd;
    }
    imgptr->dstxsize = imgptr->xlr - imgptr->xul + 1;
    imgptr->dstysize = imgptr->ylr - imgptr->yul + 1;

    set_scale (&newxsize, &newysize);  /* go get real device rectangle size */

    switch (imgptr->h_align) {
	case 0: offset = 0;
		break;
	case 1: offset = (imgptr->dstxsize - newxsize) / 2;
		break;
	case 2: offset = imgptr->dstxsize - newxsize;
		break;
    }
    imgptr->xul += offset;
    imgptr->xlr = imgptr->xul + newxsize - 1;
    imgptr->dstxsize = newxsize;
				    /* clip to intersection with original */
    if (imgptr->clip_xul < imgptr->xul) imgptr->clip_xul = imgptr->xul;
    if (imgptr->clip_xlr > imgptr->xlr) imgptr->clip_xlr = imgptr->xlr;

    switch (imgptr->v_align) {
	case 0: offset = 0;
		break;
	case 1: offset = (imgptr->dstysize - newysize) / 2;
		break;
	case 2: offset = imgptr->dstysize - newysize;
		break;
    }
    imgptr->yul += offset;
    imgptr->ylr = imgptr->yul + newysize - 1;
    imgptr->dstysize = newysize;
        
/* clip to intersection with original */
    if (imgptr->clip_yul < imgptr->yul) imgptr->clip_yul = imgptr->yul;
    if (imgptr->clip_ylr > imgptr->ylr) imgptr->clip_ylr = imgptr->ylr;
    
/* save final xul/xlr/yul/ylr & clip_xul/clip_xlr/clip_yul/clip_ylr for	*/
/*	use in r_image on subsequent copies of page */
    imgptr->save_xul = imgptr->xul;
    imgptr->save_xlr = imgptr->xlr;    
    imgptr->save_yul = imgptr->yul;
    imgptr->save_ylr = imgptr->ylr;
    imgptr->save_clipxul = imgptr->clip_xul;
    imgptr->save_clipxlr = imgptr->clip_xlr;
    imgptr->save_clipyul = imgptr->clip_yul;
    imgptr->save_clipylr = imgptr->clip_ylr;    

#ifdef COLOR_PRINTER
		    /* for color printers the input bit image planes are   */
		    /*  packed into a byte. imgptr->look_up maps this byte */
		    /*  to a CMY 3 bit representation.  The order of the   */
		    /*  CMY planes in the output raster slice is device    */
		    /*  dependent					   */
    if (imgptr->nplanes == 1) {	     /* mono image => use colors specified */
	imgptr->look_up[0] =
	    image_to_cmy[vdi_to_image[imgptr->background & 7]];
	imgptr->look_up[1] =
	    image_to_cmy[vdi_to_image[imgptr->foreground & 7]];
    } else {			     /* color image => map "RGB" to "CMY"  */
	for (i = 0; i < 8; i++) imgptr->look_up[i] = image_to_cmy[i];
    }
#endif

    if (images == 0) {		/* 1st time thru => set base pointer */
	imgbase = imgptr;
    } else {			/* otherwise link this entry in      */
	parentptr = imgbase;
	while (parentptr->link != NULL) parentptr = parentptr->link;
	parentptr->link = imgptr;
    }
    TOKEN = CONTRL[5];		/* add trigger to display list */
    PT_OP_BY();
    CONTRL[3] = 1;

    INTIN[0] = ++images;	/* image id to be processed    */
    PT_INTIN();
    return 0;
    
abort:				/* release any allocated resources */
    if (imgptr != NULL) {
	if (imgptr->data != NULL)
		free_mem(imgptr->data);
	free_mem(imgptr);
    }
    f_close(imghandle);			/* HCA - check status of close? */
    return -1;
    
} /* end of c_image() */


/*****************************************************************************
**
**	status = r_image ()
**
**	Bit image routine to insert section of a bit image file into the
**	current raster slice.
**
**	input:
**		id = image identifier inserted into display list from c_image
**		     call
**		imgptr structure
**	output:
**		status =  0 if image file succesfully processed
**			 -1 if file access problem
**		Updated raster slice.
**
*****************************************************************************/
WORD	r_image()
{
    WORD	id;		/* image number to be processed */
    WORD	status;
    WORD	nread;
    WORD	x, y;
    WORD	e;
    WORD	maptype;	/* image -> device bit plane mapping type */
    LONG	newloc;
    EXTERN	VOID	decode_img_line();
    EXTERN	VOID	out_line();
    EXTERN	VOID	out_revline();
    EXTERN	VOID	out_column();
    EXTERN	VOID	out_revcolumn();
    
    CONTRL[3] = 1;	/* retrieve image number from display list */
    GT_INTIN();
    id = INTIN[0];

    imgptr = imgbase;	/* set imgptr to proper structure */

    while (imgptr != NULL && imgptr->id != id) imgptr = imgptr->link;

    if (imgptr == NULL) return -1;
    clip_xul = MAX(imgptr->clip_xul,xmn_clip);	/* intersect bit image clip */
    clip_xlr = MIN(imgptr->clip_xlr,xmx_clip);  /*  with user specified and */
    clip_yul = MAX(imgptr->clip_yul,ymn_clip);  /*  slice imposed rectangles*/
    clip_ylr = MIN(imgptr->clip_ylr,ymx_clip);
    
    if (clip_yul <= clip_ylr && clip_xul <= clip_xlr) {
	status = f_open(imgptr->file_name, &imghandle);
	
	if (status != 0)
		return -1;

		/* if 0 degree rotation was specified and we've started */
		/*  scan converting this image already, pick up where   */
		/*  we left off						*/
	if (imgptr->rotate == 0 && imgptr->line_number > 0) {
	    if (imgptr->record_num > 0) {
		status = f_seek(imghandle, 0, \
		    (LONG) imgptr->record_num * (LONG) IMGBUF_SIZE, &newloc);
		if (status != 0) return -1;
	    }
	    status = f_read(imghandle, imgbuf, IMGBUF_SIZE, &nread);
	    if (status != 0) return -1;
	} else {		/* otherwise start fresh */
	    status = f_read(imghandle, imgbuf, IMGBUF_SIZE, &nread);
	    if (status != 0) return -1;
	    imgptr->line_number = 0;
	    imgptr->record_num = 0;
	    imgptr->record_cnt = nread - imgptr->header_len * 2;
	    imgptr->v_repeat = 0;
	}
	imgbufptr = imgbuf + nread - imgptr->record_cnt;
	imgbufcnt = imgptr->record_cnt;
	imgbufrec = imgptr->record_num;
	imgdataptr = imgptr->data;		/* for convenience */
		/* color image -> mono printer => "OR" planes		 */
		/* color image -> color printer => pack bits and look up */
		/* mono image -> color printer => just look up		 */
	if (imgptr->nplanes >= 2 && inq_tab[4] == 1) maptype = 1;
	else if (imgptr->nplanes >= 3 && inq_tab[4] == 3) maptype = 2;
	else if (imgptr->nplanes == 1 && inq_tab[4] == 3) maptype = 3;
	else maptype = 0;

	if (imgptr->line_number == 0) decode_img_line (maptype);
	switch (imgptr->rotate) {
	    case    0: if (imgptr->line_number > 1) { /* keep dda in step */
			   e = imgptr->ddarem;
		       } else {			      /* initialize dda */
			   e = -imgptr->dstysize;
		       }
		       for (y = imgptr->yul; y <= clip_ylr; y++) {
			   if (y >= clip_yul)
			       out_line (imgptr->xul, y);
			   e += imgptr->srcysize;
			   while (e > 0) {
			       if (imgptr->line_number < imgptr->scanht)
				   decode_img_line (maptype);
			       e -= imgptr->dstysize;
			   }
		       }

		       imgptr->yul = y;  /* save context to resume later */
		       imgptr->record_num = imgbufrec;
		       imgptr->record_cnt = imgbufcnt;
		       imgptr->ddarem = e;
		       break;
	    case 900: e = -imgptr->dstxsize;
		       for (x = imgptr->xul; x <= clip_xlr; x++) {
			   if (x >= clip_xul) out_revcolumn (x, imgptr->ylr);
			   e += imgptr->srcxsize;
			   while (e > 0) {
			       if (imgptr->line_number < imgptr->scanht)
				   decode_img_line (maptype);
			       e -= imgptr->dstxsize;
			   }
		       }
		       break;
	    case 1800: e = -imgptr->dstysize;
		       for (y = imgptr->ylr; y >= clip_yul; y--) {
			   if (y <= clip_ylr) out_revline (imgptr->xlr, y);
			   e += imgptr->srcysize;
			   while (e > 0) {
			       if (imgptr->line_number < imgptr->scanht)
				   decode_img_line (maptype);
			       e -= imgptr->dstysize;
			   }
		       }
		       break;
	    case 2700: e = -imgptr->dstxsize;
		       for (x = imgptr->xlr; x >= clip_xul; x--) {
			   if (x <= clip_xlr)
			       out_column (x, imgptr->yul);
			   e += imgptr->srcxsize;
			   while (e > 0) {
			       if (imgptr->line_number < imgptr->scanht)
				   decode_img_line (maptype);
			       e -= imgptr->dstxsize;
			   }
		       }
		       break;
	}

	status = f_close(imghandle);
	if (status != 0) return -1;
    }
    
    return 0;
    
}  /* end r_image() */


/************************************************************************
 *  Invoked from v_updwk() to reinitialize all image structures between	*
 *   each copy of the page being printed.				*
 ************************************************************************/
VOID reinit_struct()
{
	IMGINFO	*ptr;
	
	for (ptr = imgbase; ptr; ptr = ptr->link) {
		ptr->v_repeat = 0;
		ptr->line_number = 0;
		ptr->record_num = 0;
		ptr->record_cnt = 0;
		ptr->clip_xul = ptr->save_clipxul;
		ptr->clip_xlr = ptr->save_clipxlr;
		ptr->clip_yul = ptr->save_clipyul;
		ptr->clip_ylr = ptr->save_clipylr;	
		ptr->xul = ptr->save_xul;
		ptr->xlr = ptr->save_xlr;    
		ptr->yul = ptr->save_yul;
		ptr->ylr = ptr->save_ylr;
	} /* End for: every image on the page */
	
}  /* end reinit_struct() */


/*****************************************************************************
**
**	init_image ()
**
**	Initialize bit image output (called at update workstation time).
**
**	input:
**		g_plane = paragraph address of raster buffer
**	output:
**		raster  = local pointer to raster buffer
**
*****************************************************************************/
VOID	init_image ()
{
    raster = (UBYTE *) ((LONG) g_plane << 16);
}


/*****************************************************************************
**
**	status = clear_image ()
**
**	Clear bit image resource requirements (called at clear and close
**	workstation time).
**
**	input:
**		imgbase = pointer to base of linked list of image structures
**	output:
**		status  = 0 if successful deallocation of all memory,
**			  non-zero otherwise
**
*****************************************************************************/
WORD	clear_image ()
{
    WORD		status;
    IMGINFO		*tmpptr;

    status = 0;
    while (imgbase != NULL) {
	tmpptr = imgbase->link;
	status != free_mem(imgbase->data);
	status != free_mem(imgbase);
	imgbase = tmpptr;
    }
    imgbase = NULL;
    images = 0;
    return status;
}


/*****************************************************************************
**
**	set_scale (newxsize, newysize)
**
**	Calculate device coordinate rectangle size for the input bit image
**	with consideration for the aspect and scaling flags in effect.
**
**	input:
**		preserve aspect ratio flag
**		x integer/fractional scaling flag
**		y integer/fractional scaling flag
**		bit image x and y sizes
**		requested device x and y sizes
**		bit image pixel x and y sizes
**		output device pixel x and y sizes
**	output:
**		newxsize = final output device bit x image size
**		newxsize = final output device bit y image size
**
*****************************************************************************/
VOID set_scale (newxsize, newysize)
WORD	*newxsize;
WORD	*newysize;
{
    WORD	xint;	/* x integer scale factor required */
    WORD	yint;	/* y integer scale factor required */
    WORD	t;
    
    xint = imgptr->dstxsize / imgptr->srcxsize;
    if (xint == 0) xint = 1;
    yint = imgptr->dstysize / imgptr->srcysize;
    if (yint == 0) yint = 1;

    if (!imgptr->aspect) {			/* ignore aspect ratio  */
	if (imgptr->x_scale)			/* integer x scaling    */
	    *newxsize = xint * imgptr->srcxsize;
	else					/* fractional x scaling */
	    *newxsize = imgptr->dstxsize;
	if (imgptr->y_scale)			/* integer y scaling    */
	    *newysize = yint * imgptr->srcysize;
	else					/* fractional y scaling */
	    *newysize = imgptr->dstysize;
    } else {					/* preserve aspect ratio   */
	if (imgptr->x_scale && imgptr->y_scale) { /* integer x and y scale */
	    *newxsize = xint * imgptr->srcxsize;
	    t = asp_x2y (*newxsize);	/* t = corresponding y size */
	    if (t <= imgptr->dstysize) {	/* y size is ok */
		yint = t / imgptr->srcysize;
		if (yint == 0) yint = 1;
		*newysize = yint * imgptr->srcysize;
	    } else {			     /* y too big -> make x smaller */
		*newysize = yint * imgptr->srcysize;
		xint  = asp_y2x (*newysize) / imgptr->srcxsize;
		if (xint == 0) xint = 1;
		*newxsize = xint * imgptr->srcxsize;
	    }
	} else if (imgptr->x_scale && !imgptr->y_scale) { /* int x, fract y */
	    while (asp_x2y (xint * imgptr->srcxsize) > imgptr->dstysize) 
		xint--;		/* x as big as possible and y will fit */
	    if (xint == 0) xint = 1;
	    *newxsize = xint * imgptr->srcxsize;
	    *newysize = asp_x2y (*newxsize);	    
	} else if (!imgptr->x_scale && imgptr->y_scale) { /* fract x, int y */
	    while (asp_y2x (yint * imgptr->srcysize) > imgptr->dstxsize) 
		yint--;		/* y as big as possible and x will fit */
	    if (yint == 0) yint = 1;
	    *newysize = yint * imgptr->srcysize;
	    *newxsize = asp_y2x (*newysize);
	} else {					/* fract x, fract y */
	    *newxsize = asp_y2x (imgptr->dstysize);
	    if (*newxsize <= imgptr->dstxsize)	/* full size y, calculate x */
		*newysize = imgptr->dstysize;
	    else {				/* full size x, calculate y */
		*newxsize = imgptr->dstxsize;
		*newysize = asp_x2y (*newxsize);
	    }
	}
    }
}


/*****************************************************************************
**
**	y = asp_x2y (x)
**
**	Calculate y output device size given x output device size so that
**	bit image retains the proper aspect ratio
**
**	input:
**		x = x output device size
**	output:
**		y = y output device size
**
*****************************************************************************/
WORD	asp_x2y (x)
WORD	x;
{
    return (MUL_DIV (MUL_DIV (MUL_DIV (x, dev_tab[3], imgptr->srcxsize),
	imgptr->srcysize, imgptr->srcxpix), imgptr->srcypix, dev_tab[4]));
}



/*****************************************************************************
**
**	x = asp_y2x (y)
**
**	Calculate x output device size given y output device size so that
**	bit image retains the proper aspect ratio
**
**	input:
**		y = y output device size
**	output:
**		x = x output device size
**
*****************************************************************************/
WORD	asp_y2x (y)
WORD	y;
{
    return (MUL_DIV (MUL_DIV (MUL_DIV (y, dev_tab[4], imgptr->srcysize),
	imgptr->srcxsize, imgptr->srcypix), imgptr->srcxpix, dev_tab[3]));
}


/*****************************************************************************
**
**	decode_img_line (maptype)
**
**	Decode a scan line from the .IMG file and perform indicated
**	post-processing.  A byte is used for each .IMG bit.
**
**	input:
**		maptype = indicator of post-processing desired
**			  0: no postprocessing (for mono image on mono
**			     printer)
**			  1: image planes 2 thru imgptr->nplanes are "or"ed
**			     into the first image plane (for color image on
**			     mono printer)
**			  2: plane 2 is shifted left 1 bit and "or"ed with
**			     plane 1, plane 3 is shifted left 2 bits and
**			     "or"ed with plane 1 and then plane 1 is passed
**			     through color printer "RGB" to "CMY" look-up
**			     table (for normal color image on color printer)
**			  3: plane 1 is passed through color printer "RGB"
**			     to "CMY" look-up table (for mono image on color
**			     printer)
**	output:
**		data at imgdataptr (imgptr->data) is filled with decoded and
**		post-processed .IMG line
**
*****************************************************************************/
VOID	decode_img_line (maptype)
WORD	maptype;
{
    UBYTE	opcode;
    UBYTE	count;
    UBYTE	value;
    WORD	n;
    WORD	i;
    UBYTE	*dataptr;
    UBYTE	*tmpptr;
    EXTERN	VOID	unpack();
    
/* XFER1 is a macro to retrieve 1 byte from imgbuf */
#define XFER1() (imgbufcnt-- \
			    ? *imgbufptr++ \
			    : (img_transfer (), imgbufcnt--, *imgbufptr++))

/* XFERN is a macro to retrieve N bytes from imgbuf */
#define XFERN(N,PTR) \
if (imgbufcnt >= N) \
 {b_move (imgbufptr, N, PTR); imgbufptr += N; imgbufcnt -= N;}\
else \
 {b_move (imgbufptr, imgbufcnt, PTR); PTR += imgbufcnt; N -= imgbufcnt;\
 img_transfer (); b_move (imgbufptr, N, PTR); imgbufptr += N; \
 imgbufcnt -= N;}

    if (imgptr->v_repeat > 0) {	/* if last line is to be repeated */
	imgptr->v_repeat--;
    } else {
	n = imgptr->data_size;	/* total # of bits for all planes */
	dataptr = imgdataptr;
	while (n > 0) {		/* still need more bytes */
	    opcode = XFER1();
	    if (opcode == 0) {	
		count = XFER1();
		if (count == 0) {	/* 0 0 => vertical repeat entry */
		    opcode = XFER1();
		    opcode = XFER1();
		    imgptr->v_repeat = (WORD) opcode - 1;
		} else {		/* 0 N => pattern repeat entry */
		    i = imgptr->patlen;
		    tmpptr = dataptr;
		    XFERN(i,tmpptr);
		    unpack (imgptr->patlen, dataptr);	/* bits to bytes */
		    tmpptr = dataptr;
		    dataptr += imgptr->patlen * 8;
		    i = imgptr->patlen * 8 * (WORD) count;
		    n -= i;
		    i -= imgptr->patlen * 8;
		    b_move (tmpptr, i, dataptr);	/* repeat sequence */
		    dataptr += i;			/* by copying      */
		}
	    } else if (opcode == 0x80) {	/* 80 => bit pattern */
		count = XFER1();
		i = (WORD) count;
		tmpptr = dataptr;
		XFERN(i,tmpptr);
		i = (WORD) count;
		unpack (i, dataptr);
		dataptr += i * 8;
		n -= i * 8;
	    } else {				/* otherwise its a run */
		i = (WORD) (opcode & 0x7f) * 8;
		value = (opcode & 0x80) != 0;
		n -= i;
		b_stuff (dataptr, i, value);
		dataptr += i;
	    }
	}
	if (maptype == 1) {
	    dataptr = imgdataptr;
	    i = imgptr->nplanes;
	    while (--i) {
		dataptr += imgptr->plane_size;
		b_or (dataptr, imgptr->plane_size, imgdataptr);
	    }
	} else if (maptype == 2) {
	    i = imgptr->plane_size;
	    b_shl1or (imgdataptr + i, i, imgdataptr);
	    b_shl2or (imgdataptr + i + i, i, imgdataptr);
	    b_xlat (imgdataptr, i, imgptr->look_up);
	} else if (maptype == 3) {
	    b_xlat (imgdataptr, imgptr->plane_size, imgptr->look_up);
	}
    }
    imgptr->line_number++;
}


/*****************************************************************************
**
**	unpack (count, buffer)
**
**	Unpack 8 * count bits from buffer to 1 byte per bit and store back
**	into buffer.
**
**	input:
**		count  = number of bytes worth of bits to unpack
**		buffer = array containing bits to unpack
**	output:
**		buffer = array of count bytes containing 0 or 1
**	notes:
**		Unpacking is done starting from the end of buffer so that
**		the array will not be overwritten.
**		The most significant bit of a byte will appear first in the
**		output byte array.
**
*****************************************************************************/
VOID	unpack (count, buffer)
WORD	count;
UBYTE	*buffer;
{
    UBYTE	*bptr;
    UBYTE	*dptr;
    
    dptr = buffer + count - 1;
    bptr = buffer + count * 8 - 1;
    while (count--) {
	*bptr-- = (*dptr & 0x01) != 0;
	*bptr-- = (*dptr & 0x02) != 0;
	*bptr-- = (*dptr & 0x04) != 0;
	*bptr-- = (*dptr & 0x08) != 0;
	*bptr-- = (*dptr & 0x10) != 0;
	*bptr-- = (*dptr & 0x20) != 0;
	*bptr-- = (*dptr & 0x40) != 0;
	*bptr-- = (*dptr-- & 0x80) != 0;
    }
    
}  /* end of unpack() */


/*****************************************************************************
**
**	status = img_transfer ()
**
**	Reads IMGBUF_SIZE bytes into .IMG file buffer imgbuf and resets
**	buffer pointer and count to allow buffered .IMG file accesses.
**
**	input:
**		.IMG file handle: imghandle
**	output:
**		status =  0 if image file read succesfull
**			 -1 if image file read failed
**		imgbuf filled with next record of file data
**		imgbufptr set to start of imgbuf
**		imgbufcnt = number of significant bytes remaining in record
**
*****************************************************************************/
WORD	img_transfer ()
{
    WORD	status;

    imgbufptr = imgbuf;
    status = f_read(imghandle, imgbufptr, IMGBUF_SIZE, &imgbufcnt);
if (imgbufcnt != IMGBUF_SIZE)
    if (status != 0) return -1;
    imgbufrec++;
    return 0;
}


/*****************************************************************************
**
**	out_line (x, y)
**
**	Output the current .IMG scan line into a horizontal line of dots
**	in the printer raster slice.  DDA repetition/skipping is performed
**	according to desired source .IMG and destination output line sizes.
**	Dots are placed in order of increasing x and constant y.
**
**	input:
**		x = x coordinate for first dot to appear
**		y = y coordinate for first dot to appear
**		imgptr->srcxsize = bit image x size
**		imgptr->dstxsize = corresponding required output device x size
**		imgdataptr = pointer to bit image line buffer
**		clip_xul = x minimum of clipping region
**		clip_xlr = x maximum of clipping region
**		YS_MIN = y device coordinate of first line in output raster
**		dln_byte = number of bytes in an output device raster line
**		plane_sz = number of bytes in an output device raster plane
**		setbit = array mapping bit to set with appropriate OR mask
**		clrbit = array mapping bit to clear with appropriate AND mask
**	output:
**		non-clipped part of .IMG line in printer raster slice
**	notes:
**		The .IMG line is in packed pixel format at entry to this
**		routine, i.e. there are imgptr->srcxsize bytes to consider
**		with bit 0 <=> "cyan plane", bit 1 <=> "magenta plane",
**		bit 2 <=> "yellow plane" where "cyan plane" means the first
**		bit plane in the slice buffer, "magenta plane" means the
**		second, and "yellow plane" means the third.  Since the color
**		corresponding to slice buffer planes is device dependent,
**		the look-up table imgptr->look_up has this device dependent
**		ordering incorporated.
**
*****************************************************************************/
VOID	out_line (x, y)
WORD	x;
WORD	y;
{
    WORD		bit;
    WORD		e;
    WORD		srcsize;
    WORD		dstsize;
    UBYTE		*buffer;
    UBYTE		*craster;
#ifdef COLOR_PRINTER
    UBYTE		*mraster;
    UBYTE		*yraster;
#endif

    srcsize = imgptr->srcxsize;
    dstsize = imgptr->dstxsize;
    buffer = imgdataptr;
    e = -dstsize;
    while (x < clip_xul) {	/* keep dda in step */
	x++;
	e += srcsize;
	while (e >= 0) {
	    buffer++;
	    e -= dstsize;
	}
    }
    craster = raster + (y - YS_MIN) * dln_byte + (x >> 3);

#ifdef COLOR_PRINTER
    mraster = craster + plane_sz;
    yraster = mraster + plane_sz;
#endif

    while (x <= clip_xlr) {
	bit = x++ & 7;
#ifdef COLOR_PRINTER
	if (*buffer & 0x01)
	    *craster = *craster | setbit[bit];
	else 
	    *craster = *craster & clrbit[bit];
	if (*buffer & 0x02)
	    *mraster = *mraster | setbit[bit];
	else 
	    *mraster = *mraster & clrbit[bit];
	if (*buffer & 0x04)
	    *yraster = *yraster | setbit[bit];
	else 
	    *yraster = *yraster & clrbit[bit];
#else
	if (*buffer)
	    *craster = *craster | setbit[bit];
	else 
	    *craster = *craster & clrbit[bit];
#endif
	if (bit == 7) {
	    craster++;
#ifdef COLOR_PRINTER
	    mraster++;
	    yraster++;
#endif
	}
	e += srcsize;
	while (e >= 0) {
	    buffer++;
	    e -= dstsize;
	}
    }
    
}  /* end of out_line() */


/*****************************************************************************
**
**	out_revline (x, y)
**
**	Output the current .IMG scan line into a horizontal line of dots
**	in the printer raster slice.  DDA repetition/skipping is performed
**	according to desired source .IMG and destination output line sizes.
**	Dots are placed in order of decreasing x and constant y.
**
**	input:
**		x = x coordinate for first dot to appear
**		y = y coordinate for first dot to appear
**		imgptr->srcxsize = bit image x size
**		imgptr->dstxsize = corresponding required output device x size
**		imgdataptr = pointer to bit image line buffer
**		clip_xul = x minimum of clipping region
**		clip_xlr = x maximum of clipping region
**		YS_MIN = y device coordinate of first line in output raster
**		dln_byte = number of bytes in an output device raster line
**		plane_sz = number of bytes in an output device raster plane
**		setbit = array mapping bit to set with appropriate OR mask
**		clrbit = array mapping bit to clear with appropriate AND mask
**	output:
**		non-clipped part of .IMG line in printer raster slice
**	notes:
**		The .IMG line is in packed pixel format at entry to this
**		routine, i.e. there are imgptr->srcxsize bytes to consider
**		with bit 0 <=> "cyan plane", bit 1 <=> "magenta plane",
**		bit 2 <=> "yellow plane" where "cyan plane" means the first
**		bit plane in the slice buffer, "magenta plane" means the
**		second, and "yellow plane" means the third.  Since the color
**		corresponding to slice buffer planes is device dependent,
**		the look-up table imgptr->look_up has this device dependent
**		ordering incorporated.
**
*****************************************************************************/
VOID	out_revline (x, y)
WORD	x;
WORD	y;
{
    WORD		bit;
    WORD		e;
    WORD		srcsize;
    WORD		dstsize;
    UBYTE		*buffer;
    UBYTE		*craster;
#ifdef COLOR_PRINTER
    UBYTE		*mraster;
    UBYTE		*yraster;
#endif

    srcsize = imgptr->srcxsize;
    dstsize = imgptr->dstxsize;
    buffer = imgdataptr;
    e = -dstsize;
    while (x > clip_xlr) {
	x--;
	e += srcsize;
	while (e >= 0) {
	    buffer++;
	    e -= dstsize;
	}
    }
    craster = raster + (y - YS_MIN) * dln_byte + (x >> 3);

#ifdef COLOR_PRINTER
    mraster = craster + plane_sz;
    yraster = mraster + plane_sz;
#endif

    while (x >= clip_xul) {
	bit = x-- & 7;
#ifdef COLOR_PRINTER
	if (*buffer & 0x01)
	    *craster = *craster | setbit[bit];
	else 
	    *craster = *craster & clrbit[bit];
	if (*buffer & 0x02)
	    *mraster = *mraster | setbit[bit];
	else 
	    *mraster = *mraster & clrbit[bit];
	if (*buffer & 0x04)
	    *yraster = *yraster | setbit[bit];
	else 
	    *yraster = *yraster & clrbit[bit];
#else
	if (*buffer)
	    *craster = *craster | setbit[bit];
	else 
	    *craster = *craster & clrbit[bit];
#endif
	if (bit == 0) {
	    craster--;
#ifdef COLOR_PRINTER
	    mraster--;
	    yraster--;
#endif
	}
	e += srcsize;
	while (e >= 0) {
	    buffer++;
	    e -= dstsize;
	}
    }
    
}  /* end of out_revline() */


/*****************************************************************************
**
**	out_column (x, y)
**
**	Output the current .IMG scan line into a vertical line of dots
**	in the printer raster slice.  DDA repetition/skipping is performed
**	according to desired source .IMG and destination output line sizes.
**	Dots are placed in order of increasing y and constant x.
**
**	input:
**		x = x coordinate for first dot to appear
**		y = y coordinate for first dot to appear
**		imgptr->srcxsize = bit image x size
**		imgptr->dstxsize = corresponding required output device x size
**		imgdataptr = pointer to bit image line buffer
**		clip_yul = y minimum of clipping region
**		clip_ylr = y maximum of clipping region
**		YS_MIN = y device coordinate of first line in output raster
**		dln_byte = number of bytes in an output device raster line
**		plane_sz = number of bytes in an output device raster plane
**		setbit = array mapping bit to set with appropriate OR mask
**		clrbit = array mapping bit to clear with appropriate AND mask
**	output:
**		non-clipped part of .IMG line in printer raster slice
**	notes:
**		The .IMG line is in packed pixel format at entry to this
**		routine, i.e. there are imgptr->srcxsize bytes to consider
**		with bit 0 <=> "cyan plane", bit 1 <=> "magenta plane",
**		bit 2 <=> "yellow plane" where "cyan plane" means the first
**		bit plane in the slice buffer, "magenta plane" means the
**		second, and "yellow plane" means the third.  Since the color
**		corresponding to slice buffer planes is device dependent,
**		the look-up table imgptr->look_up has this device dependent
**		ordering incorporated.
**
*****************************************************************************/
VOID	out_column (x, y)
WORD	x;
WORD	y;
{
    WORD		bit;
    WORD		e;
    WORD		srcsize;
    WORD		dstsize;
    UBYTE		*buffer;
    UBYTE		*craster;
#ifdef COLOR_PRINTER
    UBYTE		*mraster;
    UBYTE		*yraster;
#endif
    UBYTE		smask;
    UBYTE		cmask;

    srcsize = imgptr->srcysize;
    dstsize = imgptr->dstysize;
    buffer = imgdataptr;
    e = -dstsize;
    while (y < clip_yul) {
	y++;
	e += srcsize;
	while (e >= 0) {
	    buffer++;
	    e -= dstsize;
	}
    }
    craster = raster + (y - YS_MIN) * dln_byte + (x >> 3);

#ifdef COLOR_PRINTER
    mraster = craster + plane_sz;
    yraster = mraster + plane_sz;
#endif

    bit = x & 7;
    smask = setbit[bit];
    cmask = clrbit[bit];
    while (y++ <= clip_ylr) {
#ifdef COLOR_PRINTER
	if (*buffer & 0x01)
	    *craster = *craster | smask;
	else 
	    *craster = *craster & cmask;
	if (*buffer & 0x02)
	    *mraster = *mraster | smask;
	else 
	    *mraster = *mraster & cmask;
	if (*buffer & 0x04)
	    *yraster = *yraster | smask;
	else 
	    *yraster = *yraster & cmask;
#else
	if (*buffer)
	    *craster = *craster | smask;
	else 
	    *craster = *craster & cmask;
#endif
	craster += dln_byte;
#ifdef COLOR_PRINTER
	mraster += dln_byte;
	yraster += dln_byte;
#endif
	e += srcsize;
	while (e >= 0) {
	    buffer++;
	    e -= dstsize;
	}
    }
    
}  /* end of out_column() */


/*****************************************************************************
**
**	out_revcolumn (x, y)
**
**	Output the current .IMG scan line into a vertical line of dots
**	in the printer raster slice.  DDA repetition/skipping is performed
**	according to desired source .IMG and destination output line sizes.
**	Dots are placed in order of decreasing y and constant x.
**
**	input:
**		x = x coordinate for first dot to appear
**		y = y coordinate for first dot to appear
**		imgptr->srcxsize = bit image x size
**		imgptr->dstxsize = corresponding required output device x size
**		imgdataptr = pointer to bit image line buffer
**		clip_yul = y minimum of clipping region
**		clip_ylr = y maximum of clipping region
**		YS_MIN = y device coordinate of first line in output raster
**		dln_byte = number of bytes in an output device raster line
**		plane_sz = number of bytes in an output device raster plane
**		setbit = array mapping bit to set with appropriate OR mask
**		clrbit = array mapping bit to clear with appropriate AND mask
**	output:
**		non-clipped part of .IMG line in printer raster slice
**	notes:
**		The .IMG line is in packed pixel format at entry to this
**		routine, i.e. there are imgptr->srcxsize bytes to consider
**		with bit 0 <=> "cyan plane", bit 1 <=> "magenta plane",
**		bit 2 <=> "yellow plane" where "cyan plane" means the first
**		bit plane in the slice buffer, "magenta plane" means the
**		second, and "yellow plane" means the third.  Since the color
**		corresponding to slice buffer planes is device dependent,
**		the look-up table imgptr->look_up has this device dependent
**		ordering incorporated.
**
*****************************************************************************/
VOID	out_revcolumn (x, y)
WORD	x;
WORD	y;
{
    WORD		bit;
    WORD		e;
    WORD		srcsize;
    WORD		dstsize;
    UBYTE		*buffer;
    UBYTE		*craster;
#ifdef COLOR_PRINTER
    UBYTE		*mraster;
    UBYTE		*yraster;
#endif
    UBYTE		smask;
    UBYTE		cmask;

    srcsize = imgptr->srcysize;
    dstsize = imgptr->dstysize;
    buffer = imgdataptr;
    e = -dstsize;
    while (y > clip_ylr) {
	y--;
	e += srcsize;
	while (e >= 0) {
	    buffer++;
	    e -= dstsize;
	}
    }
    craster = raster + (y - YS_MIN) * dln_byte + (x >> 3);

#ifdef COLOR_PRINTER
    mraster = craster + plane_sz;
    yraster = mraster + plane_sz;
#endif

    bit = x & 7;
    smask = setbit[bit];
    cmask = clrbit[bit];
    while (y-- >= clip_yul) {
#ifdef COLOR_PRINTER
	if (*buffer & 0x01)
	    *craster = *craster | smask;
	else 
	    *craster = *craster & cmask;
	if (*buffer & 0x02)
	    *mraster = *mraster | smask;
	else 
	    *mraster = *mraster & cmask;
	if (*buffer & 0x04)
	    *yraster = *yraster | smask;
	else 
	    *yraster = *yraster & cmask;
#else
	if (*buffer)
	    *craster = *craster | smask;
	else 
	    *craster = *craster & cmask;
#endif
	craster -= dln_byte;
#ifdef COLOR_PRINTER
	mraster -= dln_byte;
	yraster -= dln_byte;
#endif
	e += srcsize;
	while (e >= 0) {
	    buffer++;
	    e -= dstsize;
	}
    }
    
}  /* end of out_revcolumn() */

