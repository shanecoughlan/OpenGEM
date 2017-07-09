/*	DOSBIND.C	2/15/85			Lee Lorenzen		*/

#include "portab.h"
#include "machine.h"
#include "dosbind.h"

EXTERN WORD	__DOS();

GLOBAL UWORD	DOS_AX;
GLOBAL UWORD	DOS_BX;
GLOBAL UWORD	DOS_CX;
GLOBAL UWORD	DOS_DX;
GLOBAL UWORD	DOS_DS;
GLOBAL UWORD	DOS_ES;
GLOBAL UWORD	DOS_SI;
GLOBAL UWORD	DOS_DI;
GLOBAL UWORD	DOS_ERR;

	VOID
dos_func(ax, lodsdx, hidsdx)
	UWORD		ax;
	UWORD		lodsdx;
	UWORD		hidsdx;
{
	DOS_AX = ax;
	DOS_DX = lodsdx;
	DOS_DS = hidsdx;

	__DOS();
}


	WORD
dos_chdir(pdrvpath)
	LONG		pdrvpath;
{
	dos_func(0x3b00, LLOWD(pdrvpath), LHIWD(pdrvpath));
}


	WORD
dos_gdir(drive, pdrvpath)
	WORD		drive;
	LONG		pdrvpath;
{
	DOS_AX = 0x4700;
	DOS_DX = (UWORD) drive;
	DOS_SI = LLOWD(pdrvpath);
	DOS_DS = LHIWD(pdrvpath);

	__DOS();

	return(TRUE);
}


	WORD
dos_gdrv()
{
	DOS_AX = 0x1900;

	__DOS();
	return((WORD) DOS_AX & 0x00ff);
}


	WORD
dos_sdrv(newdrv)
	WORD		newdrv;
{
	DOS_AX = 0x0e00;
	DOS_DX = newdrv;

	__DOS();

	return((WORD) DOS_AX & 0x00ff);
}


	WORD
dos_sdta(ldta)
	LONG		ldta;
{
	dos_func(0x1a00, LLOWD(ldta), LHIWD(ldta));
}


	WORD
dos_sfirst(pspec, attr)
	LONG		pspec;
	WORD		attr;
{
	DOS_CX = attr;

	dos_func(0x4e00, LLOWD(pspec), LHIWD(pspec));
	return(!DOS_ERR);
}


	WORD
dos_snext()
{
	DOS_AX = 0x4f00;

	__DOS();

	return(!DOS_ERR);
}


	WORD
dos_open(pname, access)
	LONG		pname;
	WORD		access;
{
	dos_func((UWORD) 0x3d00 + access, LLOWD(pname), LHIWD(pname));

	return((WORD) DOS_AX);
}


	WORD
dos_close(handle)
	WORD		handle;
{
	DOS_AX = 0x3e00;
	DOS_BX = handle;

	__DOS();

	return(!DOS_ERR);
}


	WORD
read_piece(handle, cnt, pbuffer)
	WORD		handle;
	UWORD		cnt;
	LONG		pbuffer;
{
	DOS_CX = cnt;
	DOS_BX = handle;
	dos_func(0x3f00, LLOWD(pbuffer), LHIWD(pbuffer));
	return((WORD) DOS_AX);
}

	WORD
dos_read(handle, cnt, pbuffer)
	WORD		handle;
	LONG		cnt;
	LONG		pbuffer;
{
	UWORD	buff_piece;

	buff_piece = 0x8000;
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
		read_piece(handle, buff_piece, pbuffer);
#if	I8086
		pbuffer += 0x08000000L;
#else
		pbuffer += 0x00008000L;
#endif
	}
}



	LONG
dos_lseek(handle, smode, sofst)
	WORD		handle;
	WORD		smode;
	LONG		sofst;
{
	DOS_AX = 0x4200;
	DOS_AX += smode;
	DOS_BX = handle;
	DOS_CX = LHIWD(sofst);
	DOS_DX = LLOWD(sofst);

	__DOS();

	return((WORD) DOS_AX + HW(DOS_DX) );
}


	WORD
dos_wait()
{
	DOS_AX = 0x4d00;
	__DOS();

	return((WORD) DOS_AX);
}


	LONG
dos_alloc(nbytes)
	LONG		nbytes;
{
	LONG		maddr;

	DOS_AX = 0x4800;
	if (nbytes == 0xFFFFFFFFL)
	  DOS_BX = 0xffff;
	else
	  DOS_BX = (nbytes + 15L) >> 4L;

	__DOS();

	if (DOS_ERR)
	  maddr = 0x0L;
	else
	  maddr = HW(DOS_AX) & 0xFFFF0000L;

	return(maddr);
}


/*
*	Returns the amount of memory available in bytes
*/
	LONG
dos_avail()
{
	LONG		mlen;

	DOS_AX = 0x4800;
	DOS_BX = 0xffff;

	__DOS();

	mlen = ((LONG) DOS_BX) << 4;
	return(mlen);
}


	WORD
dos_free(maddr)
	LONG		maddr;
{
	DOS_AX = 0x4900;
	DOS_ES = LHIWD(maddr);

	__DOS();

	return((WORD) DOS_AX);
}


	WORD
dos_space(drv, ptotal, pavail)
	WORD		drv;
	LONG		*ptotal, *pavail;
{
	DOS_AX = 0x3600;
	DOS_DX = drv;
	__DOS();
	
	DOS_AX *= DOS_CX;
	*ptotal = (LONG) DOS_AX * (LONG) DOS_DX;
	*pavail = (LONG) DOS_AX * (LONG) DOS_BX;
}


	WORD
dos_rmdir(ppath)
	LONG		ppath;
{
	dos_func(0x3a00, LLOWD(ppath), LHIWD(ppath));
	return(!DOS_ERR);
}


	WORD
dos_create(pname, attr)
	LONG		pname;
	WORD		attr;
{
	DOS_CX = attr;
	dos_func(0x3c00, LLOWD(pname), LHIWD(pname));

	return((WORD) DOS_AX);
}


	WORD
dos_mkdir(ppath)
	LONG		ppath;
{
	dos_func(0x3900, LLOWD(ppath), LHIWD(ppath));
	return(!DOS_ERR);
}


	WORD
dos_delete(pname)
	LONG		pname;
{
	dos_func(0x4100, LLOWD(pname), LHIWD(pname));
	return((WORD) DOS_AX);
}


	WORD
dos_rename(poname, pnname)
	LONG		poname;
	LONG		pnname;
{
	DOS_DI = LLOWD(pnname);
	DOS_ES = LHIWD(pnname);
	dos_func(0x5600, LLOWD(poname), LHIWD(poname));
	return((WORD) DOS_AX);
}


	WORD
write_piece(handle, cnt, pbuffer)
	WORD		handle;
	UWORD		cnt;
	LONG		pbuffer;
{
	DOS_CX = cnt;
	DOS_BX = handle;
	dos_func(0x4000, LLOWD(pbuffer), LHIWD(pbuffer));
	return((WORD) DOS_AX);
}

	WORD
dos_write(handle, cnt, pbuffer)
	WORD		handle;
	LONG		cnt;
	LONG		pbuffer;
{
	UWORD	buff_piece;

	buff_piece = 0x8000;
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
		write_piece(handle, buff_piece, pbuffer);
#if	I8086
		pbuffer += 0x08000000L;
#else
		pbuffer += 0x00008000L;
#endif
	}
}

	WORD
dos_chmod(pname, func, attr)
	LONG		pname;
	WORD		func;
	WORD		attr;
{
	DOS_CX = attr;
	dos_func((UWORD) 0x4300 + func, LLOWD(pname), LHIWD(pname));
	return((WORD) DOS_CX);
}


	WORD
dos_setdt(handle, time, date)
	WORD		handle, time, date;
{
	DOS_AX = 0x5701;
	DOS_BX = handle;
	DOS_CX = time;
	DOS_DX = date;

	__DOS();
}
