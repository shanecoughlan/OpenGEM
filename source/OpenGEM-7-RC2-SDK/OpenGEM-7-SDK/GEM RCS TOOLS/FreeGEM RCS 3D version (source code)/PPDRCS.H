
#include <string.h>	/* strlen, strcpy etc. */
#include <stdarg.h>
#include <stdio.h>
#include <dos.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#include "ppdgem.h"

typedef LPBYTE FAR *LPLSTR;
typedef LPBIT  FAR *LPLBIT;
typedef LPTREE FAR *LPLTREE;
typedef RSHDR  FAR *LPRSHDR;

#define NOTREE ((LPTREE)-1)


#include "rcsdefs.h"
#include "rcs.h"
#include "rcskeybd.h"
#include "gsxdefs.h"

#include "rcsproto.h"


