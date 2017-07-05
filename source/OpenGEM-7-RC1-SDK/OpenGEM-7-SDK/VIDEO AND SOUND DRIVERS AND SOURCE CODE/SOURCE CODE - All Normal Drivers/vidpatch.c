/************************************************************************

    VIDPATCH - patch the driver type in a multi-mode GEM or ViewMAX driver

    Copyright (C) 2007  John Elliott <jce@seasip.demon.co.uk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

void syntax()
{
		fprintf(stderr, "Syntax: VIDPATCH <driver> { <number> }\n"
				"    or: VIDPATCH <driver> auto\n\n"
"        where <driver> is the name of a GEM or ViewMAX driver and <number>\n"
"        is the video mode to use. Some possible mode numbers are:\n\n"
"        SDUNI9.VGA / UDUNI9.VGA:   0 = EGA mono\n"
"                                   1 = EGA colour\n"
"                                   2 = VGA mono\n"
"                                   3 = VGA colour\n"
"        SDCAT9.EGA / UDCAT9.EGA:   6 = CGA\n"
"                                  64 = AT&T 6300\n"
"                                 116 = Toshiba 3100\n"
"        SDU869.VGA:               88 = Paradise VGA\n"
"                                  91 = Trident VGA\n"
"                                 106 = VESA-compatible\n\n");
	exit(0);
}

FILE *gl_fp;

long lookup(char *signature)
{
	unsigned char buf[6];
	long pos;
	int n;

	fseek(gl_fp, 0, SEEK_SET);
	if (fread(buf, 1, sizeof(buf), gl_fp) < (int)sizeof(buf))
	{
		return -1;
	}
	pos = 5;
	while (1)	
	{
		if (!memcmp(buf, signature, 4))
		{
			return pos;
		}
		for (n = 0; n < 5; n++)
		{
			buf[n] = buf[n+1];
		}
		pos++;
		n = fgetc(gl_fp);
		if (n == EOF)
		{
			return -1;
		}
		buf[5] = n;
	//	for (n = 0; n < 6; n++) printf("%02x ", buf[n]); printf("\n");
	}
	return -1;
}

int main(int argc, char **argv)
{
	long offset;
	int mode;

	if (argc < 2)
	{
		syntax();
	}
	gl_fp = fopen(argv[1], "r+b");
	if (!gl_fp)
	{
		perror(argv[1]);
		return 1;
	}
	offset = lookup("temp");
	if (offset == -1)
	{
		fprintf(stderr, "'temp' signature not found in driver %s\n", 
			argv[1]);
		return 1;
	}
	fseek(gl_fp, offset, SEEK_SET);
	if (argc == 2)
	{
		printf("Current video mode is ");
		mode = fgetc(gl_fp);
		if (mode > 127) printf("automatic.\n");
		else		printf("%d.\n", mode);
		fclose(gl_fp);
		return 0;	
	}
	if (!strcmp(argv[2], "auto") || !strcmp(argv[2], "Auto") ||
	    !strcmp(argv[2], "AUTO"))
	{
		fputc(0x80, gl_fp);
		fclose(gl_fp);
		printf("%s set to mode: automatic.\n", argv[1]);
		return 0;
	}
	if (!isdigit(argv[2][0]))
	{
		fclose(gl_fp);
		syntax();
		return 1;
	}
	mode = atoi(argv[2]);
	fputc(mode, gl_fp);
	fclose(gl_fp);
	printf("%s set to mode: %d.\n", argv[1], mode);
	return 0;

}
