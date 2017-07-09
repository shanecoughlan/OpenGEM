/* JCE 7 February 1998
 *
 * The callback code is essentially the same for all drawing operations, so
 * it is stored in the library.
 *
 * The assembly language entry code is in PPDLDRAW.AS or PPDSDRAW.AS 
 * depending on memory model. When it is called it sends execution to 
 * _dr_code().
 *
 */

#include "ppdgem.h"

extern LPVOID _drawaddr;
extern LPPARM _drawpar;
extern WORD   _drawret;


void interrupt _dr_code(void)			
{	
	/* This code does the following: */

	/* The parameter passed back to us in _drawpar is the address of 
	 * a PPDUBLK. */
				
	LPPBLK pBLK = _drawpar->pb_parm;

	/* The caller expects the parameter to be the ub_parm field in the
	 * PPDUBLK, so make it so. */
	
	_drawpar->pb_parm = (LPVOID)(pBLK->ub_parm);

	/* And the caller's code is at pBLK->ub_code */
	
	_drawret = (pBLK->ub_code)(_drawpar);
}


VOID ppd_userdef(LPTREE tree, WORD nobj, LPPBLK ub)
{
	tree[nobj].ob_type = ((tree[nobj].ob_type & 0xFF00) | G_USERDEF);

	ub->ub_reserved[0] = _drawaddr;	/* Draw handler function */
	ub->ub_reserved[1] = ub;		/* Parameter = address of userblk */

	tree[nobj].ob_spec = ub;
}

	


