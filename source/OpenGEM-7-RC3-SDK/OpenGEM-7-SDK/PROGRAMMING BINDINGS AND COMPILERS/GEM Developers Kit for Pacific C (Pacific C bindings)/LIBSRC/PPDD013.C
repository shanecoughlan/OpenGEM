

#include "ppdgem.h"
#include "ppdd0.h"


ULONG dos_read(WORD handle, ULONG cnt, LPBYTE pbuffer) 
		/* read complete file 32k at a time */
{
	UWORD	buff_piece;
	ULONG	rd_cnt;

	buff_piece = 0x8000; /* 32k */
	rd_cnt = 0L;
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
						 /* read 32k or less */
		rd_cnt += (ULONG)read_piece(handle, buff_piece, pbuffer);
		{
			WORD seg = FP_SEG(pbuffer);
			WORD off = FP_OFF(pbuffer);
			pbuffer =  FP_CONSTRUCT(seg + 0x800, off);
		}
	}
	return( rd_cnt );
}
