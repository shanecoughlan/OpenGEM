/*	DOSBIND.C	2/15/85			Lee Lorenzen		*/

#include "portab.h"				/* portable coding conv	*/
#include "machine.h"				/* machine depndnt conv	*/
#include "dosbind.h"  				/* file i/o defines 	*/ 


EXTERN WORD	__DOS();  			/* in DOSASM.ASM 	*/

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
	DOS_DX = lodsdx;	/* low  DS:DX   */
	DOS_DS = hidsdx;	/* high DS:DX   */

	__DOS();
}


	WORD
dos_chdir(pdrvpath) 	/*	change the current directory 	*/
	LONG		pdrvpath;
{
	dos_func(0x3b00, LLOWD(pdrvpath), LHIWD(pdrvpath));	
}


	WORD
dos_gdir(drive, pdrvpath)	/*	get current directory	*/
	WORD		drive;
	LONG		pdrvpath;
{
	DOS_AX = 0x4700;
	DOS_DX = (UWORD) drive;	/* 0 = default drive, 1 = A:,etc */
	DOS_SI = LLOWD(pdrvpath);
	DOS_DS = LHIWD(pdrvpath);

	__DOS();

	return(TRUE);
}


	WORD
dos_gdrv()	/*	get current drive	*/
{
	DOS_AX = 0x1900;

	__DOS();
	return((WORD) DOS_AX & 0x00ff);	/* 	0 = A:, 1 = B: etc */
}


	WORD
dos_sdrv(newdrv)	/* select new drive, new drive becomes default */
	WORD		newdrv;
{
	DOS_AX = 0x0e00;
	DOS_DX = newdrv;

	__DOS();

	return((WORD) DOS_AX & 0x00ff);	/* 	0 = A:, 1 = B: etc */
}


	WORD
dos_sdta(ldta)	/* set the Disk Transfer Address ( DTA ) */
	LONG		ldta;
{
	dos_func(0x1a00, LLOWD(ldta), LHIWD(ldta));
}


	WORD
dos_sfirst(pspec, attr)	/* search for first matching file */
	LONG		pspec;
	WORD		attr;
{
	DOS_CX = attr; /* file attributes */

	dos_func(0x4e00, LLOWD(pspec), LHIWD(pspec));
	return(!DOS_ERR);
}


	WORD
dos_snext()	/* search for next matching file  		  */
		/* dos_sfirst() must be used just before this one */
{
	DOS_AX = 0x4f00;

	__DOS();

	return(!DOS_ERR);
}


	WORD
dos_open(pname, access)	/* open file */
	LONG		pname; /* filename */
	WORD		access;/* 0 = read, 1 = write, 2 = both */
{
	dos_func((UWORD) 0x3d00 + access, LLOWD(pname), LHIWD(pname));
	return((WORD) DOS_AX);	/* DOS_AX contains file handle */
}


	WORD
dos_close(handle)	/* close file */
	WORD		handle;
{
	DOS_AX = 0x3e00;
	DOS_BX = handle;

	__DOS();

	return(!DOS_ERR);
}

	WORD
read_piece(handle, cnt, pbuffer)	/* read file */
	WORD		handle;
	UWORD		cnt;		/* number of bytes to read */
	LONG		pbuffer;	/* buffer to read into 	   */
{
	DOS_CX = cnt;
	DOS_BX = handle;
	dos_func(0x3f00, LLOWD(pbuffer), LHIWD(pbuffer));
	return((WORD) DOS_AX);	/* DOS_AX = number of bytes actually read */
}

	LONG
dos_read(handle, cnt, pbuffer) /* read complete file 32k at a time */
	WORD		handle;
	LONG		cnt;
	LONG		pbuffer;
{
	UWORD	buff_piece;
	LONG	rd_cnt;

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
		rd_cnt += (LONG)read_piece(handle, buff_piece, pbuffer);
#if	I8086
		pbuffer += 0x08000000L;
#else
		pbuffer += 0x00008000L;
#endif
	}
	return( rd_cnt );
}


	LONG
dos_lseek(handle, smode, sofst) /* move file read / write pointer */
	WORD		handle;
	WORD		smode; /* 0 = from beginning, 1 from current */
			       /* 2 = EOF plus offset                */
	LONG		sofst; /* offset in bytes 		     */
{
	DOS_AX = 0x4200;
	DOS_AX += smode;
	DOS_BX = handle;
	DOS_CX = LHIWD(sofst); /* contains the      */
	DOS_DX = LLOWD(sofst); /* desired offset    */

	__DOS();

	return(LONG)( DOS_AX + HW(DOS_DX ));   /* return pointers new location */
}

	WORD
dos_wait()	/* get return code of a sub-process */
{
	DOS_AX = 0x4d00;
	__DOS();

	return((WORD) DOS_AX); /* 0 = normal termination */
}

	LONG
dos_alloc(nbytes)	/* allocate memory */
	LONG		nbytes;
{
	LONG		maddr;

	DOS_AX = 0x4800;
	if (nbytes == 0xFFFFFFFFL)	/* convert number */
	  DOS_BX = 0xffff;		/* 	of bytes  */
	else				/*	to	  */
	  DOS_BX = (nbytes + 15L) >> 4L;/*	paragraphs*/
	__DOS();

	if (DOS_ERR)
	  maddr = 0x0L;
	else
	  maddr = HW(DOS_AX) & 0xFFFF0000L;

	return(maddr); /* return location of allocated memory block */
}

	LONG
dos_avail()  /* Returns the amount of memory available in paragraphs */
{
	LONG		mlen;

	DOS_AX = 0x4800;
	DOS_BX = 0xffff;

	__DOS();

	mlen = ((LONG) DOS_BX) << 4;
	return(mlen);
}


	WORD
dos_free(maddr)	/* free memory that was allocated via dos_alloc() */
	LONG		maddr;
{
	DOS_AX = 0x4900;
	DOS_ES = LHIWD(maddr);

	__DOS();

	return((WORD) DOS_AX);
}


	WORD
dos_space(drv, ptotal, pavail)	/* get disk free space     */
	WORD		drv;	/* 0 = default, 1 = A: etc */
	LONG		*ptotal, *pavail;
{
	DOS_AX = 0x3600;
	DOS_DX = drv;
	__DOS();

	/*	DOS_AX contains number of sectors per cluster */
	/*	DOS_BX contains number of available clusters  */
	/*	DOS_CX contains number of bytes per sector    */
	/*	DOS_DX contains total number of clusters      */
	
	DOS_AX *= DOS_CX;
	*ptotal = (LONG) DOS_AX * (LONG) DOS_DX;
	*pavail = (LONG) DOS_AX * (LONG) DOS_BX;
}


	WORD
dos_rmdir(ppath)	/* remove directory entry */
	LONG		ppath;
{
	dos_func(0x3a00, LLOWD(ppath), LHIWD(ppath));
	return(!DOS_ERR);
}


	WORD
dos_create(pname, attr)	/* create file */
	LONG		pname;
	WORD		attr;
{
	DOS_CX = attr;
	dos_func(0x3c00, LLOWD(pname), LHIWD(pname));

	return((WORD) DOS_AX);
}


	WORD
dos_mkdir(ppath)	/* create a sub-directory */
	LONG		ppath;
{
	dos_func(0x3900, LLOWD(ppath), LHIWD(ppath));
	return(!DOS_ERR);
}


	WORD
dos_delete(pname)	/* delete file */
	LONG		pname;
{
	dos_func(0x4100, LLOWD(pname), LHIWD(pname));
	return((WORD) DOS_AX);
}


	WORD
dos_rename(poname, pnname)	/* rename file */
	LONG		poname;
	LONG		pnname;
{
	DOS_DI = LLOWD(pnname);
	DOS_ES = LHIWD(pnname);
	dos_func(0x5600, LLOWD(poname), LHIWD(poname));
	return((WORD) DOS_AX);
}


	WORD
write_piece(handle, cnt, pbuffer)	/*  write to a file */
	WORD		handle;
	UWORD		cnt;
	LONG		pbuffer;
{
	DOS_CX = cnt;
	DOS_BX = handle;
	dos_func(0x4000, LLOWD(pbuffer), LHIWD(pbuffer));
	return((WORD) DOS_AX);
}

	LONG
dos_write(handle, cnt, pbuffer)	/* write 32k or less to a file */

	WORD		handle;
	LONG		cnt;
	LONG		pbuffer;
{
	UWORD	buff_piece;
	LONG	wt_cnt;

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
		wt_cnt += (LONG)write_piece(handle, buff_piece, pbuffer);
#if	I8086
		pbuffer += 0x08000000L;
#else
		pbuffer += 0x00008000L;
#endif
	}
	return( wt_cnt );
}

	WORD
dos_chmod(pname, func, attr)	/* change file mode */
	LONG		pname;
	WORD		func;
	WORD		attr;
{
	DOS_CX = attr;
	dos_func((UWORD) 0x4300 + func, LLOWD(pname), LHIWD(pname));
	return((WORD) DOS_CX);
}


	WORD
dos_setdt(handle, time, date)	/* set a file's date and time */
	WORD		handle, time, date;
{
	DOS_AX = 0x5701;
	DOS_BX = handle;
	DOS_CX = time;
	DOS_DX = date;

	__DOS();
} /* DOSBIND.C  */
