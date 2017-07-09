
#include <stdio.h>
#include <stdarg.h>
#include "djgppgem.h"

extern LPBYTE pGeneral;	/* General transfer buffer */

/* Utility functions to make it easier to do things from 32-bit code */

LPBYTE dj_string_addr(char *s) /* Create a string in DOS space */
{
	int    n = 1 + strlen(s);
	LPBYTE p = dos_alloc(n);

	if (!p) return p;

	dosmemput(s, n, p);
	return p;
}


WORD dj_form_alert(WORD opt, char *s, ...)
					/* Do a form_alert using text in */
{					/* linear memory */
	LPBYTE p;
	WORD ret;
	char strbuf[4096];
	va_list ap;

	va_start(ap, s);
	vsprintf(strbuf, s, ap);

	va_end(ap);

	if (strlen(strbuf) < 4096)	/* It had better be :-) */
	{
		dosmemput(strbuf, 1 + strlen(strbuf), pGeneral);
		return form_alert(opt, pGeneral);
	}

	p = dj_string_addr(strbuf);

	if (!p) return -1;

	ret = form_alert(opt, p);
		
	dos_free(p);	

	return ret;
}


WORD dj_lstrlen(LPBYTE s)	       /* Do a strlen() on a string in DOS memory */
{
	WORD n;

	for (n = 0; LBGET(s); n++) s++;	

	return n;
}


WORD dj_putb2w(BYTE *s, LPBYTE d)	/* Convert string to list of integers */
{
	WORD n;

	for (n = 0; *s; n+=2) 
	{
		LBSET(d + n, *s++);
		LBSET(d+n+1, 0);
	}
	LWSET(d + n, 0);	/* Final null */
	
	return (n/2);
}


WORD dj_getw2b(LPBYTE s, WORD cnt, BYTE *d)
{
	WORD n;
	
	cnt *= 2;
	for (n = 0; n < cnt; n+=2) *d++ = (BYTE)LWGET(s + n);

	return (cnt / 2);
}
