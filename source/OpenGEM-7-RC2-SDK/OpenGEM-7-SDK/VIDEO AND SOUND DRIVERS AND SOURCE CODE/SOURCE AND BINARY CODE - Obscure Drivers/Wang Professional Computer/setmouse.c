/************************************************************************

    SETMOUSE - patch the driver type in a multi-mode GEM or ViewMAX driver

    Copyright (C) 2012  John Elliott <jce@seasip.demon.co.uk>

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
		fprintf(stderr, "Syntax: SETMOUSE <driver> { <number> { <port> }}\n\n"
"        where <driver> is the name of a GEM or ViewMAX driver, <number>\n"
"        is the mouse type, and <port> is the number of the serial port\n"
"        to which a serial mouse is connected.\n\nValid mouse types:"
"        0 = No mouse\n"
"        1 = Mouse Systems serial mouse\n"
"        2 = Bus or other mouse using MOUSE.COM / MOUSE.SYS"
"        3 = Microsoft serial mouse\n"
"        4 = MM1201 SummaSketch with stylus\n"
"        5 = MM1201 SummaSketch with cursor\n"
"        6 = MM961  SummaSketch with stylus\n"
"        7 = MM961  SummaSketch with cursor\n"
"        8 = MM1812 SummaSketch with stylus\n"
"        9 = MM1812 SummaSketch with cursor\n"
"       10 = PS/2 mouse\n"

"                                 106 = VESA-compatible\n\n");
	exit(0);
}


const char *nameof(int drvr)
{
	static const char *dnames[] = {"No mouse", "Mouse Systems",
					"Bus mouse", "Microsoft serial",
					"MM1201 stylus", "MM1201 tablet",
					"MM951 stylus", "MM961 tablet",
					"MM1812 stylus", "MM1812 tablet",
					"PS/2 mouse" };

	if (drvr >= 0 && drvr <= 10) return dnames[drvr];
	return "Unknown";

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
	int drvr, ioport;

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
	offset = lookup("zyxg");
	if (offset == -1)
	{
		fprintf(stderr, "'zyxg' signature not found in driver %s\n", 
			argv[1]);
		return 1;
	}
	--offset;
	fseek(gl_fp, offset, SEEK_SET);
	ioport = (signed char)(fgetc(gl_fp));
	drvr = fgetc(gl_fp);
	fseek(gl_fp, offset, SEEK_SET);
	if (argc == 2)
	{
		printf("Current mouse driver is ");
		printf("%d (%s) on port %d.\n", drvr, nameof(drvr), ioport + 1);
		fclose(gl_fp);
		return 0;	
	}
	if (!isdigit(argv[2][0]))
	{
		fclose(gl_fp);
		syntax();
		return 1;
	}
	drvr = atoi(argv[2]);
	if (isdigit(argv[3][0]))
	{
		ioport = atoi(argv[3]) - 1;
	}
	fputc(ioport, gl_fp);
	fputc(drvr, gl_fp);
	fclose(gl_fp);
	printf("%s set to mouse driver %d (%s) on port %d.\n", argv[1], 
			drvr, nameof(drvr), ioport + 1);
	return 0;

}
