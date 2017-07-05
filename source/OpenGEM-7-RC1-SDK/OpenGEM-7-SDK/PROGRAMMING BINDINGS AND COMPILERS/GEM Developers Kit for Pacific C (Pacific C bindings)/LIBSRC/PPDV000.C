/*	LVDIBIND.C	7/18/84 - 4/19/85	John Grant		*/
/*	Modified VDIBIND.C for use with LARGE Model		*/

#include "ppdgem.h"

extern WORD contrl[], intin[], ptsin[], intout[], ptsout[];

GSXPAR pblock = {
    &contrl[0], &intin[0], &ptsin[0], &intout[0], &ptsout[0]
};

#define i_ptsin(ptr) pblock.ptsin = ptr
#define i_intin(ptr) pblock.intin = ptr
#define i_intout(ptr) pblock.intout = ptr
#define i_ptsout(ptr) pblock.ptsout = ptr

LPVOID *lptr1 = (LPVOID *) (&contrl[7]);
LPVOID *lptr2 = (LPVOID *) (&contrl[9]);

#define i_ptr(ptr)   *lptr1 = ptr
#define i_lptr1(ptr) *lptr1 = ptr
#define i_ptr2(ptr)  *lptr2 = ptr
#define m_lptr2(ptr) *ptr   = *lptr2


    WORD
v_opnwk( work_in, handle, work_out )
WORD work_in[], *handle, work_out[];
{
        i_intin( work_in );
        i_intout( work_out );
        i_ptsout( work_out + 45 );

        contrl[0] = 1;
        contrl[1] = 0;
        contrl[3] = 103 ;
        vdi();

        *handle = contrl[6];    

        i_intin( intin );
        i_intout( intout );
        i_ptsout( ptsout );
	return( TRUE ) ;
}
