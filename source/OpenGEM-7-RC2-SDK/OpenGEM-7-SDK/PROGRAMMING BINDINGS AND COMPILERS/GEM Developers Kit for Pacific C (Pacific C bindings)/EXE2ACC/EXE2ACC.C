
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

/* The following arrays are:

   srcp[] - the "signature" for a large-model program
   srcs[] - the "signatute" for a small-model program
   dstp[] - patch 1 for the large model. This sets up a 4k stack segment
   dsts[] - patch 1 for the small model.
   dsts2[]- patch 2 for both models. This stops the program following 
            dangling pointers in the quest for an environment. 
*/

static unsigned char srcp[] = {0x2b, 0xc3, 0xb9, 0x04, 0x00, 0xd3, 0xe0};
static unsigned char dstp[] = {0xb8, 0x00, 0x10, 0x90, 0x90, 0x90, 0x90};
static unsigned char srcs[] = {0x75, 0x06, 0xb1, 0x04, 0xd3, 0xe3, 0x8b, 0xc3};
static unsigned char dsts[] = {0xb8, 0x00, 0x10, 0x90, 0x90, 0x90, 0x90, 0x90};
static unsigned char dsts2[]= {0x8c, 0xd8, 0x8e, 0xc0, 0x90};

static FILE *exefile, *accfile, *symfile;

static char *exename, *accname;
static char symname[200];

static unsigned char exebuf[512];
static char symbuf[512];

static unsigned int hBss, lBss;
static char Bssv;
static long exelen, newlen;

void main(int argc, char **argv)
{
	char *pDot;
	int	lrlen;
	long exeleft;

	if (argc < 3)
	{
		fprintf(stderr,"Syntax: exe2acc file.exe file.acc\n\n"
                       "You need to be sure that PPD generated a symbol file for file.exe");
		exit(1);
	}
	exename = argv[1];
	accname = argv[2];

	Bssv = 0;

	strcpy(symname, exename);
	
	pDot = strrchr(symname,'.');
	if (pDot) strcpy(pDot,".SYM");

	exefile = fopen(exename,"rb"); if (!exefile) {	perror(exename); exit(1); }
	accfile = fopen(accname,"w+b"); if (!accfile) {	perror(accname); exit(1); }
	symfile = fopen(symname,"r");  
	if (!symfile && pDot)
	{
		strcpy(pDot,".sym");
		symfile = fopen(symname,"r");  	
	}
	if (!symfile) { perror(symname); exit(1); }

	fread(exebuf, 1, 512, exefile);

	if (exebuf[0] != 'M' || exebuf[1] != 'Z') 
		fprintf(stderr,"WARNING: %s does not start with MZ signature\n", exename);

	exelen = 512L * exebuf[4] + 131072L * exebuf[5];

	lrlen = exebuf[2] + 256 * exebuf[3];

	if (lrlen) exelen -= (512 - lrlen);

	while (!feof(symfile) && (Bssv < 3))
	{
		fgets(symbuf, 511, symfile);
		if (!strncmp(symbuf,"__Hbss ", 7))
		{
			hBss  = xtoi(symbuf + 7);
			Bssv |= 1;
		}
		if (!strncmp(symbuf,"__Lbss ", 7))
		{
			lBss  = xtoi(symbuf + 7);
			Bssv |= 2;
		}
	}
	if (feof(symfile))
	{
		fprintf(stderr,"Could not find symbols: %s %s   in %s\n",
						(Bssv & 1) ? "" : "__Hbss",
						(Bssv & 2) ? "" : "__Lbss",
						symname);
		exit(1);
	}

	printf("__Lbss = 0x%x  __Hbss = 0x%x  EXE length = 0x%lx\n", lBss, hBss, exelen);

	newlen = hBss - lBss + exelen + 4096;	/* 4k for the stack */

	lrlen = newlen & 0x1FF; if (!lrlen) lrlen = 0x200;
	
	exebuf[2] = lrlen & 0xFF;
	exebuf[3] = (lrlen >> 8) & 0xFF;
	lrlen = (newlen + 511L) / 512L;
	exebuf[4] = lrlen & 0xFF;
	exebuf[5] = (lrlen >> 8) & 0xFF;

	exeleft = exelen;
	while (exeleft > 0)
	{
		int buflen = (exeleft < 512 ? exeleft : 512);

		fwrite(exebuf, 1, buflen, accfile);
		if (ferror(accfile)) 
		{
			perror(accname);
			fclose(accfile);
			exit(1);
		}
		exeleft -= buflen;
		if (exeleft) fread(exebuf, 1, 512, exefile);
	}
	while (exelen < newlen)
	{
		fputc(0, accfile);
		++exelen;
	}

	/* Patch the C runtime so that it doesn't try to get EXE size from
	 * the PSP (there is no PSP) */

	memset(exebuf, 0, sizeof(exebuf));
	fseek(accfile, 0, SEEK_SET);
	fread(exebuf, 1, sizeof(exebuf), accfile);

	lrlen = (exebuf[8] + 256 * exebuf[9]) * 16 + 0x1A;

	fseek(accfile, lrlen, SEEK_SET);

	fread(exebuf, 1, 11, accfile);

	if (!memcmp(exebuf + 4, srcp, 7))
	{
		printf("Performing large-model runtime patch\n");
		memcpy(exebuf + 4, dstp, 7);
		exebuf[0] = 0x90;
		exebuf[1] = 0x90;
		fseek(accfile, lrlen, SEEK_SET);
		fwrite(exebuf, 1, 11, accfile);
		fseek(accfile, lrlen + 0x1F, SEEK_SET);
		fwrite(dsts2, 1, 5, accfile);
	}
	else if (!memcmp(exebuf + 1, srcs, 8))
	{
		printf("Performing small-model runtime patch\n");
		memcpy(exebuf + 1, dsts, 8);
		exebuf[2] = hBss & 0xFF;
		exebuf[3] = ((hBss >> 8) & 0xFF) + 0x10;
		fseek(accfile, lrlen, SEEK_SET);
		fwrite(exebuf, 1, 11, accfile);
		fseek(accfile, lrlen + 0x1E, SEEK_SET);
		fwrite(dsts2, 1, 5, accfile);
	}

	else printf("Warning: No runtime patch was made\n");
	
	fclose(symfile);
	fclose(exefile);
	fclose(accfile);
	exit(0);
}
