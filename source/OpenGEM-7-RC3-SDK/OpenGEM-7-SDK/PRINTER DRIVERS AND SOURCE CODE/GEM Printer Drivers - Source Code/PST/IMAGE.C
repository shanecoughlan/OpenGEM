/*		IMAGE.C		11/17/86 - 2/15/87	J. Grant	*/

#include "portab.h"
#include "vdidef.alw"
#include "vdiext.alw"


	WORD
aspect_x2y(x)
	WORD		x;
{
	/* Convert the 'x' value passed in to a 'y' value, preserving */
	/* aspect ratio.                                              */
	return(muldiv(muldiv(x, image.srcysize, image.srcxsize), image.srcypix,
		image.srcxpix));
}  /* End "aspect_x2y". */


	WORD
aspect_y2x(y)
	WORD		y;
{
	/* Convert the 'y' value passed in to an 'x' value, preserving */
	/* aspect ratio.                                               */
	return(muldiv(muldiv(y, image.srcxsize, image.srcysize), image.srcxpix,
		image.srcypix));
}  /* End "aspect_y2x". */


	VOID
image_out()
{
	EXTERN	VOID	compact();
	EXTERN	VOID	fast();
	WORD		bit_width;
	WORD		width_in_bytes, wtemp1, angle;
	BYTE		*bptr;

	/* Initialize for the image output loop. */
	bptr = (BYTE *)&ptsin[0];
	width_in_bytes = (image.width + 7)/8;

	/* Initialize the device for this bit image output. */
	outdec(width_in_bytes);
	outdec(image.pat_bytes);
	output("/vrep 0 def /patlen exch def dup string /scan exch def /smax exch def\n");
	outxy(image.clip_xl, image.clip_yb);
	outxy(image.clip_xr, image.clip_yb);
	outxy(image.clip_xr, image.clip_yt);
	outxy(image.clip_xl, image.clip_yt);
	output("gs np mto lto lto lto clip\n");
	wtemp1 = image.xr - image.xl + 1;
	if (wtemp1 & 7)
		wtemp1 = (wtemp1 & 0xfff8) + 8;	
	outdec(wtemp1);
	outdec(image.yb - image.yt + 1);
	outxy(image.xl, image.yb);
	output("translate scale\n");
	outdec(bit_width = 8*width_in_bytes);
	outdec(image.height);
	output("1 [");

	angle = image.rotate;
	outdec (xcos (image.width, angle));
	outdec (xsin (image.height, angle));
	outdec (xsin (image.width, angle));
	outdec (xcos (-image.height, angle));
	outdec ((WORD) (((LONG) image.width -
			 (LONG) xcos (image.width, angle) -
			 (LONG) xsin (image.width, angle)) >> 1));
	outdec ((WORD) (((LONG) image.height +
			 (LONG) xcos (image.height, angle) -
			 (LONG) xsin (image.height, angle)) >> 1));

	/* Output according to the requested image type:  fast or compact. */
	if (g_fstimg) {
		output("]fstimg\n");
		fast(width_in_bytes);
	}  /* End if:  fast image. */
	else {
		output("]decode\n");
		compact(bptr, width_in_bytes);
	}  /* End else:  compact image. */

	/* Restore the device environment. */
	output("gr ");
}  /* End "image_out". */


	VOID
compact(bptr, width_in_bytes)
	BYTE		*bptr;
	WORD		width_in_bytes;
{
	WORD		bytes, color, i, planes, rep, run, scan;
	BYTE		opcode;

	/* Loop over the number of scan lines in the source. */
	for (scan = 0; scan < image.height; scan++) {
		/* Process a scan line escape command, if one is specified. */
		if (image.buf[image.pos] == 0) {
			/* Potentially a scan line escape.  Check the next */
			/* byte.  If it is a zero, we're in business.      */
			transfer(1, bptr);
			if (image.buf[image.pos] == 0) {
				/* Currently, only one scan line escape  */
				/* is defined:  the vertical replication */
				/* escape.  Process it.                  */
				transfer(3, bptr);
				scan += (rep = (WORD)*(bptr + 2)) - 1;
				outdec(4);
				outdec(rep);
			}  /* End if:  scan line escape. */

			/* Not really a scan line escape.  Back up a byte. */
			else
				image.pos--;
		}  /* End if:  potential scan line escape. */

		/* Loop over the number of bit planes in the source. */
		for (planes = 0; planes < image.s_planes; planes++) {
			/* Loop until this plane of the scan line is done. */
			bytes = 0;
			while (bytes < width_in_bytes) {
				/* Process according to the byte being     */
				/* pointed to.  If the low seven bits is   */
				/* non-zero, a solid run is to be output.  */
				/* Otherwise, if the byte is zero, a       */
				/* pattern run is to be output.  Otherwise */
				/* (i.e., the low seven bits are zero, but */
				/* the high bit is set), a string of bits  */
				/* is to be output.                        */
				if ((opcode = image.buf[image.pos]) & 0x7f) {
					/* Process a solid run. */
					transfer(1, bptr);
					bytes +=
						(run = (WORD)(opcode & 0x7f));
					if (planes == 0) {
						color = opcode & 0x80 ? 1 : 0;
						outdec(color);
						outdec(run);
					}  /* End if:  output necessary. */
				}  /* End if:  solid run. */

				else {
					/* Get the first two bytes of the   */
					/* command.  The second byte is the */
					/* amount of data which follows.    */
					transfer(2, bptr);
					run = (WORD)*(bptr + 1);

					/* Process according to whether a */
					/* pattern run or bit string has  */
					/* been specified.                */
					if (opcode == 0) {
						transfer(image.pat_bytes,
							bptr);
						rep = image.pat_bytes;
						bytes += run*rep;
						opcode = 2;
					}  /* End if:  pattern run. */

					else {
						transfer(run, bptr);
						bytes += (rep = run);
						opcode = 3;
					}  /* End else:  bit string. */

					/* If this is the first plane, */
					/* output the hex bit string.  */
					/* Since black is denoted by a */
					/* zero bit on the device, the */
					/* one's complement of the hex */
					/* bit string must be taken.   */
					if (planes == 0) {
						outdec((WORD)opcode);
						outdec(run);
						for (i = 0; i < rep; i++)
							outhex(~(*(bptr + i)));
						output("\n");
					}  /* End if:  output necessary. */
				}  /* End else:  pattern run or bit string. */
			}  /* End while:  over bytes in this plane. */
		}  /* End for:  over bit planes in the source. */
	}  /* End for:  over scan lines. */
}  /* End "compact". */


	VOID
fast(width_in_bytes)
	WORD		width_in_bytes;
{
	WORD		bytes, i, planes, rep, run, scan;
	BYTE		color, opcode;
	BYTE		*sptr;
	BYTE		repbuf[4];

	/* Loop over the number of scan lines in the source. */
	for (scan = 0; scan < image.height; scan++) {
		/* Process a scan line escape command, if one is specified. */
		if (image.buf[image.pos] == 0) {
			/* Potentially a scan line escape.  Check the next */
			/* byte.  If it is a zero, we're in business.      */
			transfer(1, repbuf);
			if (image.buf[image.pos] == 0) {
				/* Currently, only one scan line escape  */
				/* is defined:  the vertical replication */
				/* escape.  Process it.                  */
				transfer(3, repbuf);
				scan += (rep = (WORD)repbuf[2] - 1);
				outdec(1);
				outdec(rep);
			}  /* End if:  scan line escape. */

			/* Not really a scan line escape.  Back up a byte. */
			else
				image.pos--;
		}  /* End if:  potential scan line escape. */

		/* Loop over the number of bit planes in the source. */
		outdec(0);
		for (planes = 0; planes < image.s_planes; planes++) {
			/* Loop until this plane of the scan line is done. */
			bytes = 0;
			sptr = filbuf;
			while (bytes < width_in_bytes) {
				if ((opcode = image.buf[image.pos]) & 0x7f) {
					/* Process a solid run. */
					transfer(1, repbuf);
					run = (WORD)(opcode & 0x7f);
					bytes += run;
					color = opcode & 0x80 ? 0xff : 0;
					if (planes == 0) {
					    while (run-- > 0)
						*sptr++ = color;
					} else {
					    while (run-- > 0)
						*sptr++ |= color;
					}
				}  /* End if:  solid run. */

				else {
					transfer(2, repbuf);
					run = (WORD)repbuf[1];

					if (opcode == 0) {
					    rep = image.pat_bytes;
					    transfer(rep, repbuf);
					    bytes += run*rep;
					    if (planes == 0) {
						while (run-- > 0) {
						    for (i = 0; i < rep; i++)
							*sptr++ = repbuf[i];
						}  /* End while. */
					    } else {
						while (run-- > 0) {
						    for (i = 0; i < rep; i++)
							*sptr++ |= repbuf[i];
						}  /* End while. */
					    }
					}  /* End if:  pattern run. */

					else {
					    bytes += run;
					    if (planes == 0) {
						transfer(run, sptr);
						sptr += run;
					    } else {
						while (run-- > 0) {
						    transfer (1, &color);
						    *sptr++ |= color;
						}  /* End while. */
					    }				
					}  /* End else:  bit string. */
				}  /* End else:  pattern run or bit string. */
			}  /* End while:  over bytes in this plane. */

			/* If this is the last bit plane of the source, */
			/* output the scan line.                         */
			if (planes == image.s_planes - 1) {
				for (i = 0; i < width_in_bytes; i++)
					outhex(~filbuf[i]);
				output("\n");
			}  /* End if:  output necessary. */
		}  /* End for:  over bit planes in the source. */
	}  /* End for:  over scan lines. */
}  /* End "fast". */


	VOID
image_setup()
{
	WORD		count, dest_height, dest_width, i;
	WORD		temp1, temp2, temp3;

	/* Read in the image file header and extract information from it. */
	if ((image.remain = f_read(image.handle, ptsin, 4)) > 0) {
		/* Swap bytes in the count element of the header and */
		/* read in the rest of the header.  Swap the rest of */
		/* the header.                                       */
		swapbyte(1, &ptsin[1]);
		if ((count = (ptsin[1] - 2) << 1) > 2*PTSIN_SIZE) {
			image.remain = 0;
			return;
		}  /* End if:  bizarre header info. */

		image.remain = f_read(image.handle, ptsin, count);
		swapbyte(count >> 1, ptsin);

		/* Extract information from the header. */
		image.s_planes = ptsin[0];
		image.pat_bytes = ptsin[1];
		image.pix_width = ptsin[2];
		image.pix_height = ptsin[3];
		image.width = ptsin[4];
		image.height = ptsin[5];

		image.srcxsize = abs (xcos (image.width, image.rotate)) +
			     abs (xsin (image.height, image.rotate));
		image.srcysize = abs (xsin (image.width, image.rotate)) +
			     abs (xcos (image.height, image.rotate));
		image.srcxpix = abs (xcos (image.pix_width, image.rotate)) +
			    abs (xsin (image.pix_height, image.rotate));
		image.srcypix = abs (xsin (image.pix_width, image.rotate)) +
			    abs (xcos (image.pix_height, image.rotate));

		/* Prime the pump:  read a bit image buffer in. */
		image.remain = f_read(image.handle, image.buf, IMAGE_SIZE);
		image.pos = 0;

		/* Modify the x and y output edges according to the input */
		/* parameters.  First, take care of the case where aspect */
		/* ratio is to be ignored.                                */
		dest_width = image.xr - image.xl + 1;
		dest_height = image.yb - image.yt + 1;
		if ((image.flags & (BYTE)1) == (BYTE)0) {
			/* Apply integer scaling to x, if necessary. */
		    if ( (image.flags & (BYTE)2) != (BYTE)0 )
			image.xr = image.xl - 1 + (dest_width>image.srcxsize ?
				image.srcxsize*(dest_width/image.srcxsize) :
				image.srcxsize);

			/* Apply integer scaling to y, if necessary. */
			if ((image.flags & (BYTE)4) != (BYTE)0)
			    image.yb = image.yt - 1 + (dest_height >
				image.srcysize ?
				image.srcysize*(dest_height/image.srcysize) :
				image.srcysize);
		}  /* End if:  ignore aspect ratio. */

		/* Process the cases where aspect ratio is to be preserved. */
		else {
			/* Process integer scaling of x. */
			if ((image.flags & (BYTE)2) != (BYTE)0) {
				/* Process integer scaling of both x and y. */
				if ((image.flags & (BYTE)4) != (BYTE)0) {
					temp1 = dest_width > image.srcxsize ?
					    image.srcxsize * (dest_width / 
						image.srcxsize) :
					    image.srcxsize;
					temp2 = (temp3 = aspect_x2y(temp1)) > 
					    image.srcysize ?
					    image.srcysize * (temp3 /
						image.srcysize) :
					    image.srcysize;
					if (temp3 > dest_height) {
					    temp2 = dest_height >
						image.srcysize ?
						image.srcysize * (dest_height
						    /image.srcysize) :
						image.srcysize;
					    temp1 = (temp1=aspect_y2x(temp2))
						> image.srcxsize ?
					       image.srcxsize * (temp1 /
						   image.srcxsize) :
					       image.srcxsize;
					}  /* End if:  x primary no good. */
					image.xr = image.xl - 1 + temp1;
					image.yb = image.yt - 1 + temp2;
				}  /* End if:  integer scaling in x and y. */

				/* Process integer scaling in x and */
				/* fractional in y.                 */
				else {
					for (i = 1; i*image.srcxsize <
					    dest_width && aspect_x2y(i
					    * image.srcxsize) < dest_height;
					    i++);
					if (i > 1)
						i--;
					image.xr = image.xl +
						i*image.srcxsize - 1;
					image.yb = image.yt - 1 +
						aspect_x2y(image.xr -
						image.xl + 1);
				}  /* End else:  integer x, fractional y. */
			}  /* End if:  integer scaling of x required. */

			/* Process fractional scaling of x. */
			else {
				/* Process integer scaling in y and */
				/* fractional in x.                 */
				if ((image.flags & (BYTE)4) != (BYTE)0) {
					for (i = 1; i*image.srcysize <
					    dest_height && aspect_y2x(i *
					    image.srcysize)<dest_width; i++);
					if (i > 1)
						i--;
					image.yb = image.yt +
						i*image.srcysize - 1;
					image.xr = image.xl - 1 +
						aspect_y2x(image.yb -
						image.yt + 1);
				}  /* End if:  fractional x, integer y. */

				/* Process fractional in both x and y. */
				else {
					if ((temp1 = aspect_y2x(
						dest_height)) <= dest_width)
						image.xr = image.xl -
							1 + temp1;
					else
						image.yb = image.yt - 1 +
							aspect_x2y(dest_width);
				}  /* End else:  fractional in x and y. */
			}  /* End else:  fractional scaling of x. */
		}  /* End else:  aspect ratio is to be preserved. */

		/* Align the rectangle. */
		if ((image.flags & (BYTE)8) != (BYTE)0)
			temp1 = (image.clip_xr - image.xr)/2;
		else if ((image.flags & (BYTE)16) != (BYTE)0)
			temp1 = image.clip_xr - image.xr;
		else
			temp1 = 0;
		image.xl += temp1;
		image.xr += temp1;

		if ((image.flags & (BYTE)32) != (BYTE)0)
			temp1 = (image.clip_yb - image.yb)/2;
		else if ((image.flags & (BYTE)64) != (BYTE)0)
			temp1 = image.clip_yb - image.yb;
		else
			temp1 = 0;
		image.yt += temp1;
		image.yb += temp1;
	}  /* End if:  no problem with the read. */
}  /* End "image_setup". */
