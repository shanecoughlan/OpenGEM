

#include "ppdgem.h"
#include "ppdd0.h"


ULONG dos_write(WORD handle, ULONG cnt, LPBYTE pbuffer)	/* write 32k or less to a file */
{
	UWORD	buff_piece;
	ULONG	wt_cnt;

	buff_piece = 0x8000;
	wt_cnt = 0L;
	DOS_ERR = FALSE;
	while (cnt && !DOS_ERR)
	{
		if (cnt > 0x00008000L)
			cnt -= 0x00008000L;
		else
		{
			buff_piece = cnt;
			cnt = 0;
		}
		wt_cnt += (ULONG)write_piece(handle, buff_piece, pbuffer);
		{
			WORD seg = FP_SEG(pbuffer);
			WORD off = FP_OFF(pbuffer);
			pbuffer =  FP_CONSTRUCT(seg + 0x800, off);
		}
	}
	return( wt_cnt );
}
