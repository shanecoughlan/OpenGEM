/*		DISPATCH.C	11/17/86 - 1/1/87	J. Grant	*/

#include "portab.h"
#include "vdidef.alw"
#include "vdiext.alw"


    VOID
dispatch()
{
    EXTERN VOID	init_page();
	/* Save state if appropriate to. */
	if (contrl[0] == 5) {
	    if (contrl[5] == 0x17 || contrl[5] == 0x19 || contrl[5] == 0x65)
		pending = TRUE;
	} else {
	    if (contrl[0] >= 6 && contrl[0] <= 11)
		pending = TRUE;
	}
	if (g_endpre) {
		if (contrl[0] != 119) {
			if (contrl[0] != 5 ||
				(contrl[0] == 5 && contrl[5] != 26 &&
				contrl[5] != 27 && contrl[5] != 28 &&
				contrl[5] != 29 && contrl[5] != 31)) {
				init_page();
				g_endpre = FALSE;
			}  /* End if:  still pending. */
		}  /* End if:  still pending. */
	}  /* End if:  preamble must be terminated. */

	/* Based on the opcode, dispatch to the appropriate routine. */
	if (contrl[0] > 0 && contrl[0] < 40)
		(*low_table[contrl[0]])();
	else if (contrl[0] > 99 && contrl[0] < 132)
		(*high_table[contrl[0] - 100])();
}  /* End "dispatch". */

    VOID
nop()
{
}  /* End "nop". */
