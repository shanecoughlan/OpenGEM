
#include "ppdrcs.h"
#include	<stdio.h>

/*
 * The original RCS used the dos_* calls to do its deeds. However, we have
 * the more comprehensive Pacific C library.
 *
 * Here is a C library equivalent of form_error().
 *
 */

WORD ppd_form_error(WORD errno)
{
	char strbuf[256];

	sprintf(strbuf, "[1][%s.][ Cancel ]", strerror(errno));


	return form_alert(1, ADDR(strbuf)) - 1;
}



/* These are to keep the compiler's "possible pointer truncation" 
 * warnings all in this file. In this memory model, there is no 
 * pointer truncation, but the compiler warns anyway. */
 
WORD lfwrite(LPVOID data, WORD size, WORD count, FILE *fp)
{
	return fwrite(data, size, count, fp);
}


WORD lfread (LPVOID data, WORD size, WORD count, FILE *fp)
{
	return fread(data, size, count, fp);
}

