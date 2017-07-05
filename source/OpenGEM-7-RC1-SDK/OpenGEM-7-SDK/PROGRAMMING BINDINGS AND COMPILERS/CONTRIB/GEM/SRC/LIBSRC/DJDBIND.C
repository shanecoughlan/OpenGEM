/*	DOSBIND.C	2/15/85			Lee Lorenzen		*/

#include "portab.h"				/* portable coding conv	*/
#include "machine.h"				/* machine depndnt conv	*/

/* [JCE] Note: These dos_* functions bypass the DOS extender and 
   work in the DOS segment. If it is at all possible, use DJGPP's own
   libraries instead. */

static __dpmi_regs DOS_REGS;

GLOBAL UWORD	DOS_ERR;


VOID	__DOS(VOID)
{
	__dpmi_int(0x21, &DOS_REGS);
	DOS_ERR = (DOS_REGS.x.flags & 1);	/* Carry flag */
}


#define DOS_AX DOS_REGS.x.ax
#define DOS_BX DOS_REGS.x.bx
#define DOS_CX DOS_REGS.x.cx
#define DOS_DX DOS_REGS.x.dx
#define DOS_DS DOS_REGS.x.ds
#define DOS_ES DOS_REGS.x.es
#define DOS_SI DOS_REGS.x.si
#define DOS_DI DOS_REGS.x.di


VOID dos_func(UWORD ax, UWORD lodsdx, UWORD hidsdx)  
{
	DOS_AX = ax;
	DOS_DX = lodsdx;	/* low  DS:DX   */
	DOS_DS = hidsdx;	/* high DS:DX   */

	__DOS();
}


VOID dos_chdir(LPBYTE pdrvpath) 	/*	change the current directory 	*/
{
	dos_func(0x3b00, LP_OFF(pdrvpath), LP_SEG(pdrvpath));	
}


WORD dos_gdir(WORD drive, LPBYTE pdrvpath)	/*	get current directory	*/
{
	DOS_AX = 0x4700;
	DOS_DX = (UWORD) drive;	/* 0 = default drive, 1 = A:,etc */
	DOS_SI = LP_OFF(pdrvpath);
	DOS_DS = LP_SEG(pdrvpath);

	__DOS();

	return(TRUE);
}


WORD dos_gdrv(VOID)	/*	get current drive	*/
{
	DOS_AX = 0x1900;

	__DOS();
	return((WORD) DOS_AX & 0x00ff);	/* 	0 = A:, 1 = B: etc */
}


WORD dos_sdrv(WORD newdrv)	/* select new drive, new drive becomes default */
{
	DOS_AX = 0x0e00;
	DOS_DX = newdrv;

	__DOS();

	return((WORD) DOS_AX & 0x00ff);	/* 	0 = A:, 1 = B: etc */
}


VOID dos_sdta(LPVOID ldta)	/* set the Disk Transfer Address ( DTA ) */
{
	dos_func(0x1a00, LP_OFF(ldta), LP_SEG(ldta));
}


WORD dos_sfirst(LPBYTE pspec, WORD attr)	/* search for first matching file */
{
	DOS_CX = attr; /* file attributes */

	dos_func(0x4e00, LP_OFF(pspec), LP_SEG(pspec));
	return(!DOS_ERR);
}


WORD dos_snext(VOID)	/* search for next matching file  		  */
		/* dos_sfirst() must be used just before this one */
{
	DOS_AX = 0x4f00;

	__DOS();

	return(!DOS_ERR);
}


WORD dos_open(LPBYTE pname, WORD access)	/* open file */
{
	dos_func((UWORD) 0x3d00 + access, LP_OFF(pname), LP_SEG(pname));
	return((WORD) DOS_AX);	/* DOS_AX contains file handle */
}


WORD dos_close(WORD handle)	/* close file */
{
	DOS_AX = 0x3e00;
	DOS_BX = handle;

	__DOS();

	return(!DOS_ERR);
}

WORD read_piece(WORD handle, UWORD cnt, LPVOID pbuffer)	/* read file */
{
	DOS_CX = cnt;
	DOS_BX = handle;
	dos_func(0x3f00, LP_OFF(pbuffer), LP_SEG(pbuffer));
	return((WORD) DOS_AX);	/* DOS_AX = number of bytes actually read */
}

LONG dos_read(WORD handle, LONG cnt, LPVOID pbuffer) /* read complete file 32k at a time */
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
		pbuffer += 0x00008000L;
	}
	return( rd_cnt );
}


LONG dos_lseek(WORD handle, WORD smode, LONG sofst) /* move file read / write pointer */
{
	DOS_AX = 0x4200;
	DOS_AX += smode;
	DOS_BX = handle;
	DOS_CX = LHIWD(sofst); /* contains the      */
	DOS_DX = LLOWD(sofst); /* desired offset    */

	__DOS();

	return(LONG)( DOS_AX + HW(DOS_DX ));   /* return pointers new location */
}

WORD dos_wait()	/* get return code of a sub-process */
{
	DOS_AX = 0x4d00;
	__DOS();

	return((WORD) DOS_AX); /* 0 = normal termination */
}

/* dos_alloc and dos_free are done elsewhere */

LONG dos_avail(VOID)  /* Returns the amount of memory available in paragraphs */
{
	LONG		mlen;

	DOS_AX = 0x4800;
	DOS_BX = 0xffff;

	__DOS();

	mlen = ((LONG) DOS_BX) << 4;
	return(mlen);
}



VOID dos_space(WORD drv, LONG *ptotal, LONG *pavail)	
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


WORD dos_rmdir(LPBYTE ppath)	/* remove directory entry */
{
	dos_func(0x3a00, LP_OFF(ppath), LP_SEG(ppath));
	return(!DOS_ERR);
}


WORD dos_create(LPBYTE pname, WORD attr)	/* create file */
{
	DOS_CX = attr;
	dos_func(0x3c00, LP_OFF(pname), LP_SEG(pname));

	return((WORD) DOS_AX);
}


WORD dos_mkdir(LPBYTE ppath)	/* create a sub-directory */
{
	dos_func(0x3900, LP_OFF(ppath), LP_SEG(ppath));
	return(!DOS_ERR);
}


WORD dos_delete(LPBYTE pname)	/* delete file */
{
	dos_func(0x4100, LP_OFF(pname), LP_SEG(pname));
	return((WORD) DOS_AX);
}


WORD dos_rename(LPBYTE poname, LPBYTE pnname)	/* rename file */
{
	DOS_DI = LP_OFF(pnname);
	DOS_ES = LP_SEG(pnname);
	dos_func(0x5600, LP_OFF(poname), LP_SEG(poname));
	return((WORD) DOS_AX);
}


WORD write_piece(WORD handle, UWORD cnt, LPVOID pbuffer)	/*  write to a file */
{
	DOS_CX = cnt;
	DOS_BX = handle;
	dos_func(0x4000, LP_OFF(pbuffer), LP_SEG(pbuffer));
	return((WORD) DOS_AX);
}

LONG dos_write(WORD handle, LONG cnt, LPVOID pbuffer)	/* write 32k or less to a file */
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
		pbuffer += 0x00008000L;
	}
	return( wt_cnt );
}


WORD dos_chmod(LPBYTE pname, WORD func, WORD attr)	/* change file mode */
{
	DOS_CX = attr;
	dos_func((UWORD) 0x4300 + func, LP_OFF(pname), LP_SEG(pname));
	return((WORD) DOS_CX);
}


VOID dos_setdt(WORD handle, WORD time, WORD date)	/* set a file's date and time */
{
	DOS_AX = 0x5701;
	DOS_BX = handle;
	DOS_CX = time;
	DOS_DX = date;

	__DOS();
}
/* DOSBIND.C  */
