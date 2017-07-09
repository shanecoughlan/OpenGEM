/*		UTILITY.C	11/17/86 - 12/23/86	J. Grant	*/

#include "portab.h"
#include "vdidef.alw"
#include "vdiext.alw"



	WORD
abs(x)
	WORD	x;
{
	return (x >= 0) ? x : -x;
}

	BYTE *
asc2num(ptr, valptr)
	BYTE		*ptr;
	WORD		*valptr;
{
	WORD		fraction, sign, value;
	BOOLEAN		point, firstpoint;
	BYTE		ch;

	/* Skip leading white. */
	while ((ch = *ptr++) <= ' ')
		;

	/* Initialize for interpretation. */
	if (ch == '-')
		sign = -1;
	else {
		ptr--;
		sign = 1;
	}  /* End else:  not negative. */
	value = fraction = 0;

	/* Interpret the string. */
	point = FALSE;
	FOREVER {
		ch = *ptr;
		if (ch == '.') {
			point = firstpoint = TRUE;
			ptr++;
			continue;
		}  /* End if:  decimal point. */

		if (ch < '0' || ch > '9')
			break;
		if (!point)
			value = 10*value + ch - '0';
		else if (firstpoint) {
			firstpoint = FALSE;
			if (ch >= '5')
				fraction = 1;
		}  /* End if:  first digit after decimal point. */
		ptr++;
	}  /* End FOREVER. */
	*valptr = sign*(value + fraction);
	return(ptr);
}  /* End "asc2num". */


	VOID
copy_string(count, from, to)
	WORD		count;
	WORD		*from;
	BYTE		*to;
{
	WORD		i, temp;
	BYTE		hold;

	/* Copy the string from the word array to the byte array.  If a */
	/* '(' or ')' is encountered, quote it.                         */
	for (i = 0; i < count; i++, from++) {
		if (((hold = (BYTE)*from) == '(') || (*from == ')') ||
			(*from == '\\') || (*from >= 127) || (*from < 32))
			*to++ = '\\';
		temp = *from;
	
		if (temp < 32 || temp >= 127) {
			*to++ = (BYTE)(((temp >> 6) & 3) + 48);
			*to++ = (BYTE)(((temp >> 3) & 7) + 48);
			*to++ = (BYTE)((temp & 7) + 48);
		}
		else
			*to++ = hold;
	}  /* End for. */
	*to = '\0';
}  /* End "copy_string". */

	BYTE *
i_ds(integer)
	WORD		integer;
{
	MLOCAL BYTE	string[8];
	WORD		digit, i, j, sign;

	/* Save the sign. */
	if ((sign = integer) < 0)
		integer = -integer;

	/* Convert digits in reverse order.  Pre-pend a space character (it */
	/* will become a trailing space later).                             */
	string[0] = ' ';
	for (i = 1; i < 7; ) {
		string[i++] = (integer % 10) + '0';
		if (!(integer /= 10))
			break;
	}  /* End for. */

	/* Append the sign, if necessary.  Append a null to terminate the */
	/* string.                                                        */
	if (sign < 0)
		string[i++] = '-';
	string[i] = 0;

	/* Reverse the string. */
	for (--i, j = 0; j < i; i--, j++) {
		digit = string[i];
		string[i] = string[j];
		string[j] = digit;
	}  /* End for. */

	/* Return the string pointer. */
	return(string);
}  /* End "i_ds". */


	BYTE *
i_hs(integer)
	UWORD		integer;
{
	MLOCAL BYTE	string[5];
	WORD		i, temp;

	/* Perform the conversion.  Note that the hex conversion differs */
	/* from the decimal conversion in that no space is appended to   */
	/* the resulting string.                                         */
	for (i = 3; i >= 0; i--, integer >>= 4)
		string[i] = (temp = (integer & 0xf)) > 9 ?
			(BYTE)(temp - 10 + 'a') : (BYTE)(temp + '0');
	string[4] = '\0';
	return(string);
}  /* End "i_hs". */


	VOID
outdec(number)
	WORD		number;
{
	EXTERN VOID	output();

	/* Convert the number to a string and output it. */
	output(i_ds(number));
}  /* End "outdec". */


	VOID
outhex(number)
	BYTE		number;
{
	EXTERN VOID	output();
	MLOCAL BYTE	string[3] = {'\0', '\0', '\0'};
	WORD		temp;

	/* Convert the number to a hex string of two bytes and output it. */
	string[0] = (temp = (number >> 4) & 0xf) > 9 ?
		(BYTE)(temp - 10 + 'a') : (BYTE)(temp + '0');
	string[1] = (temp = number & 0xf) > 9 ?
		(BYTE)(temp - 10 + 'a') : (BYTE)(temp + '0');
	output(string);
}  /* End "outhex". */


	VOID
output(string)
	BYTE		*string;
{
	/* Output characters until a null is found. */
	while (*string) {
		if (*string == '\n')
			char_out('\015');
		char_out(*string++);
	}  /* End while. */
}  /* End "output". */


	VOID
outxy(x, y)
	WORD		x;
	WORD		y;
{
	/* Output the points:  flip y on the way out. */
	outdec(x);
	outdec(y_max - y);
}  /* End "outxy". */


	WORD
range(value, low, high, def)
	WORD		value;
	WORD		low;
	WORD		high;
	WORD		def;
{
	return( (value < low || value > high) ? def : value );
}  /* End "range". */


	VOID
transfer(bytes, loc)
	WORD		bytes;
	BYTE		loc[];
{
	WORD		amount, i, left;

	/* Can the request be satisfied from the current buffer? */
	if (image.pos + bytes < image.remain)
		for (i = 0; i < bytes; i++)
			loc[i] = image.buf[image.pos++];

	else {
		/* Transfer what is left in the current buffer. */
		left = image.remain - image.pos;
		for (amount = 0; amount < left; amount++)
			loc[amount] = image.buf[image.pos++];

		/* Copy the last byte of the current file buffer into the */
		/* first byte of the file buffer to be read in so that a  */
		/* single byte backspace can be performed, if necessary.  */
		/* Read in the new file buffer.  Transfer more to the     */
		/* destination, if necessary.                             */
		image.buf[0] = image.buf[image.remain - 1];
		image.remain = f_read(image.handle, &image.buf[1],
			IMAGE_SIZE - 1) + 1;

		if (amount < bytes) {
			for (i = 0 ; i < bytes - amount; i++)
				loc[i + amount] = image.buf[i + 1];
			image.pos = i + 1;
		}  /* End if:  more must be transferred. */
		else
			image.pos = 1;
	}  /* End else:  new buffer needed. */
}  /* End "transfer". */


	VOID
sendfile(filename)
	BYTE		*filename;
{
	WORD		count, handle, i;
	BYTE		path[66];

	fullpath(&path[0], filename);
	if ((handle = f_open(&path[0])) != 0) {
		while ((count = f_read(handle, &filbuf[0], FILBUF_SIZE)) != 0)
		{
			for (i = 0; i < count; i++)
				char_out(filbuf[i]);
		}  /* End while:  data available. */
		f_close(handle);
	}  /* End if:  file open successful. */
}  /* End "sendfile". */


#define    HALFPI    900 
#define    PI	     1800
#define    PI32	     2700
#define    TWOPI     3600

    WORD
xsin (x, angle) 
WORD	x;
WORD	angle;
{
    EXTERN WORD	muldiv();
    WORD	index, remainder, tmpsin, quadrant;
    static WORD	sintab[92] =
		{    
		     0,   279,   558,   837,  1116,  1394,  1672,  1950,
		  2227,  2503,  2778,  3053,  3327,  3599,  3871,  4141,
		  4410,  4678,  4944,  5209,  5472,  5734,  5994,  6252,
		  6508,  6762,  7014,  7264,  7512,  7757,  8000,  8241,
		  8479,  8714,  8947,  9177,  9405,  9629,  9851, 10069,
		 10285, 10497, 10706, 10912, 11115, 11314, 11509, 11702,
		 11890, 12075, 12257, 12434, 12608, 12778, 12944, 13106,
		 13265, 13419, 13569, 13715, 13856, 13994, 14127, 14256,
		 14381, 14501, 14617, 14728, 14835, 14937, 15035, 15128,
		 15217, 15301, 15380, 15455, 15525, 15590, 15650, 15706,
		 15757, 15803, 15844, 15881, 15912, 15939, 15961, 15978,
		 15990, 15998, 16000, 16000
		};

    while (angle >= TWOPI) angle -= TWOPI;
    while (angle < 0) angle += TWOPI;
    if (angle <= HALFPI)
	quadrant = 1;
    else if (angle <= PI) {
	angle = PI - angle;
	quadrant = 2;
    }
    else if (angle <= PI32) {
	angle -= PI;
	quadrant = 3;
    }
    else {
	angle = TWOPI - angle;
	quadrant = 4;
    }
    index = angle / 10;
    remainder = angle - index * 10;
    tmpsin = sintab[index];
    if (remainder) tmpsin += ((sintab[index + 1] - tmpsin) * remainder) / 10;
    tmpsin = muldiv (x, tmpsin, 16000);
    if (quadrant > 2) tmpsin = -tmpsin;
    return tmpsin;
}

    WORD
xcos (x, angle) 
WORD	x;
WORD	angle;
{
    while (angle >= TWOPI) angle -= TWOPI;
    return xsin (x, angle + HALFPI);
}

