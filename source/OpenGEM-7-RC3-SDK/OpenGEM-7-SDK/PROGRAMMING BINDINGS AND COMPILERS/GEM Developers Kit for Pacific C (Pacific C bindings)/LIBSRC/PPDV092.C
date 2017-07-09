

#include "ppdgem.h"
#include "ppdv0.h"


WORD v_opnvwk( work_in, handle, work_out )
WORD work_in[], *handle, work_out[];
{
   i_intin( work_in );
   i_intout( work_out );
   i_ptsout( work_out + 45 );

   contrl[0] = 100;
   contrl[1] = 0;
   contrl[3] = 11;
   contrl[6] = *handle;
   vdi();

   *handle = contrl[6];    
   i_intin( intin );
   i_intout( intout );
   i_ptsout( ptsout );
}
