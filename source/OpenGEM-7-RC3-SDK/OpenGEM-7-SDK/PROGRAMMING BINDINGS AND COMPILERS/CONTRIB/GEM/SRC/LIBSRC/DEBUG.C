
#include <stdio.h>
#include <stdarg.h>
#include "djgppgem.h"
#include "debug.h"

void dj_TRACE(char *s, ...)
{
	va_list ap;
	FILE *fp;

	va_start(ap, s);
	fp = fopen("D:/GEMAPP.LOG", "a");
	vfprintf(fp, s, ap);
	fclose(fp);

	va_end(ap);
}


void dj_dnode(int o, OBJECT *tr)
{
	dj_TRACE("%08x: [%02x] next=%04x     head=%04x  tail=%04x\n"
             "               type=%04x     flags=%04x state=%04x\n"
             "               spec=%08lx x=%04x y=%04x w=%04x h=%04x\n",
		(int)tr, o, tr->ob_next, tr->ob_head, tr->ob_tail, tr->ob_type,
                tr->ob_flags, tr->ob_state, tr->ob_spec, tr->ob_x, tr->ob_y,
                tr->ob_width, tr->ob_height);

	switch(tr->ob_type)
	{
		case G_STRING:
		case G_TITLE:
		case G_BUTTON:
			dj_TRACE("          %s\n", (char *)(tr->ob_spec));
			break;
		case G_TEXT:
		case G_BOXTEXT:
		case G_FBOXTEXT:
			dj_TRACE("          %s\n", *((char **)(tr->ob_spec)));
			break;
	}
}


void dj_dnode16(int o, LPTREE t16)
{
	OBJECT t32, *tr;
	char str[80];

	dosmemget(t16, sizeof(OBJECT), &t32);

	tr = &t32;	

	dj_TRACE("%08x: [%02x] next=%04x     head=%04x  tail=%04x\n"
             "               type=%04x     flags=%04x state=%04x\n"
             "               spec=%08lx x=%04x y=%04x w=%04x h=%04x\n",
		(int)t16, o, tr->ob_next, tr->ob_head, tr->ob_tail, tr->ob_type,
                tr->ob_flags, tr->ob_state, tr->ob_spec, tr->ob_x, tr->ob_y,
                tr->ob_width, tr->ob_height);

	switch(tr->ob_type)
	{
		case G_STRING:
		case G_TITLE:
		case G_BUTTON:
			dosmemget(LSGET(t16 + 12), 80, str);
			str[79] = 0;	
			dj_TRACE("          %s\n", str);
			break;
		case G_TEXT:
		case G_BOXTEXT:
		case G_FBOXTEXT:
			dosmemget(LSGET(LSGET(t16 + 12)), 80, str);
			str[79] = 0;	
			dj_TRACE("          %s\n", str);
			break;


	}

}





void dj_dtree(OBJECT *tr)
{
	int ob = 0;

	dj_TRACE("%08x: Tree32\n", (int)tr);
	do
	{
		dj_dnode(ob++, tr);
		if (tr->ob_flags & LASTOB) return;
		++tr;
	} while(1);


}


void dj_dtree16(LPTREE tr)
{
	int ob = 0;

	dj_TRACE("%08x: Tree16\n", (int)tr);
	do
	{
		dj_dnode16(ob++, tr);
		if (LWGET(tr+8) & LASTOB) return;
		tr += sizeof(OBJECT);
	} while(1);

}


void dj_dmfdb16(LPMFDB mf)
{
	dj_TRACE("    mp=%lx  fwp=%d  fh=%d  fww=%d  ff=%d  np=%d\n",
                 LSGET(mf), LWGET(mf+4), LWGET(mf+6), LWGET(mf+8), LWGET(mf+10), LWGET(mf+12));	
}

