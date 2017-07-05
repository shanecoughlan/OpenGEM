

#include <stdio.h>
#include "djgppgem.h"

/* Lists sizes of structs. Used to check structure packing. */

#define showsize(x) printf("sizeof(" #x ") = %ld\n", sizeof(x))

int main(int argc, char **argv)
{
	showsize(OBJECT);  showsize(ORECT);  showsize(GRECT);   showsize(TEDINFO);
	showsize(ICONBLK); showsize(BITBLK); showsize(USERBLK); showsize(PARMBLK);
	showsize(MFDB);    showsize(FILLPAT);
	return 0;
}

