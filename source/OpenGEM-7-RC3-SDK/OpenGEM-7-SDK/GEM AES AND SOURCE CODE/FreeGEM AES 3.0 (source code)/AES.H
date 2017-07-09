/*
*       AES.H: Single include file for all AES source files
*
*       Copyright 1999, John Elliott
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*/

#ifndef __AES_H_INCLUDED__

#define __AES_H_INCLUDED__ 18-10-1999


#define RED_UNDERLINES	1		/* Introduced in ViewMAX/2 */
#define APPLE_COMPLIANT 0		/* Make changes required by Apple lawsuit? 
                                 *
                                 * nb: The Apple-compliant version 
                                 * will still have a full-width scrollbar  
                                 * in the Item Selector; you'll have to use
                                 * RCS to make it thin again. You Have Been
                                 * Warned. 
                                 */
#define GROWBOX 1				/* Include growbox & shrinkbox? */
#define PROPLIB 1				/* Include get_prop(), put_prop() etc */
#define XAPPLIB 1				/* Include appl_getinfo()? */
#define XSHELL  1				/* Include xshl_*() calls? */
#define EMSDESK 1				/* Allow xgrf_dtmfdb to use EMS? */

#define DEBUG 0					/* Include code for debugging? */

#include "portab.h"				/* Original GEM headers */
#include "machine.h"
#include "obdefs.h"
#include "taddr.h"
#include "struct.h"
#include "basepage.h"
#include "gemlib.h"
#include "gemkeybd.h"
#include "crysbind.h"
#include "gem.h"
#include "dos.h"
#include "funcdef.h"
#include "gsxdefs.h"

#include "aesproto.h"		/* Function prototypes */
#include "aesvar.h"		/* Variable prototypes */

#endif /* ndef __AES_H_INCLUDED__ */

