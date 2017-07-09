/*  	GEMRSLIB.C	5/14/84 - 06/23/85	Lowell Webster		*/
/*	merge High C vers. w. 2.2 		8/24/87		mdf	*/ 

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include "aes.h"

#define NUM_OBS      rs_hdr[R_NOBS]
#define NUM_TREE     rs_hdr[R_NTREE]
#define NUM_TI       rs_hdr[R_NTED]
#define NUM_IB       rs_hdr[R_NICON]
#define NUM_BB       rs_hdr[R_NBITBLK]
#define NUM_FRSTR    rs_hdr[R_NSTRING]
#define NUM_FRIMG    rs_hdr[R_IMAGES]

#define ROB_TYPE  (&((LPTREE)psubstruct)->ob_type)	/* Long pointer in OBJECT	*/
#define ROB_STATE (&((LPTREE)psubstruct)->ob_state)	/* Long pointer in OBJECT	*/
#define ROB_SPEC  (&((LPTREE)psubstruct)->ob_spec)	/* Long pointer in OBJECT	*/

#define RTE_PTEXT  (&((LPTEDI)psubstruct)->te_ptext)	/* Long pointers in TEDINFO	*/
#define RTE_PTMPLT (&((LPTEDI)psubstruct)->te_ptmplt)
#define RTE_PVALID (&((LPTEDI)psubstruct)->te_pvalid)
#define RTE_TXTLEN (&((LPTEDI)psubstruct)->te_txtlen)
#define RTE_TMPLEN (&((LPTEDI)psubstruct)->te_tmplen)

#define RIB_PMASK (&((LPICON)psubstruct)->ib_pmask)	/* Long pointers in ICONBLK	*/
#define RIB_PDATA (&((LPICON)psubstruct)->ib_pdata)
#define RIB_PTEXT (&((LPICON)psubstruct)->ib_ptext)

#define RBI_PDATA (&((LPBIT)psubstruct)->bi_pdata)	/* Long pointer in BITBLK	*/
#define RBI_WB    (&((LPBIT)psubstruct)->bi_wb)
#define RBI_HL    (&((LPBIT)psubstruct)->bi_hl)
					/* in global array		*/
#define APP_LOPNAME ((LPLPTR FAR *)(&rs_global[10]))
#define APP_LO1RESV ((LONG FAR *)(&rs_global[14]))
#define APP_LO2RESV ((WORD FAR *)(&rs_global[18]))
					/* in DOS.C			*/
WORD		rs_gaddr();

EXTERN	WORD	dos_open();
EXTERN	LONG	dos_lseek();
EXTERN	WORD	dos_read();
EXTERN	WORD	dos_close();
EXTERN	WORD	dos_free();
					/* in SHIB.C, added for hc      */
EXTERN  WORD	sh_find(); 


EXTERN	WORD	DOS_ERR;
EXTERN WORD	gl_width;
EXTERN WORD	gl_height;
EXTERN WORD	gl_wchar;
EXTERN WORD	gl_hchar;

EXTERN THEGLO		D;

WORD fix_ptr(WORD type, WORD index);
VOID fix_nptrs(WORD count, WORD type);

LPVOID get_addr(UWORD rstype, UWORD rsindex);


/*******  LOCALS  **********************/
LPWORD		rs_hdr;
LPBYTE		rs_global;

#if ALCYON
	ULONG		junk;
#endif


/*
*	Fix up a character position, from offset,row/col to a pixel value.
*	If column or width is 80 then convert to rightmost column or 
*	full screen width. 
*/
	VOID
fix_chpos(pfix, offset)
	LPWORD	pfix;
	WORD	offset;
{
	WORD	coffset;
	WORD	cpos;

	cpos = *pfix;
	coffset = (cpos >> 8) & 0x00ff;
	cpos &= 0x00ff;

	switch(offset)
	{
	  case 0: cpos *= gl_wchar;
	    break;
	  case 1: cpos *= gl_hchar;
	    break;
	  case 2: if (cpos == 80)
	       cpos = gl_width;
	     else
	       cpos *= gl_wchar;  	
	    break;
	  case 3: if (cpos == 25)
	       cpos = gl_height;
	     else
	       cpos *= gl_hchar;  	
	    break;	
	}

	cpos += ( coffset > 128 ) ? (coffset - 256) : coffset;
	*pfix = cpos;
}


/************************************************************************/
/* rs_obfix								*/
/************************************************************************/
	WORD
rs_obfix(tree, curob)
	LPTREE		tree;
	WORD		curob;
{
	REG WORD	offset;
	REG LPWORD	p;
						/* set X,Y,W,H */
	p = &tree[curob].ob_x;

	for (offset=0; offset<4; offset++)
	  fix_chpos(&p[offset], offset);
	return TRUE;
}


	BYTE
*rs_str(stnum)
{
	LPVOID		ad_string;

	rs_gaddr(ad_sysglo, R_STRING, stnum, &ad_string);
	LSTCPY(ad_g1loc, ad_string);
	return( &D.g_loc1[0] );
}


	LPBYTE
get_sub(rsindex, rtype, rsize)
	WORD		rsindex, rtype, rsize;
{
	UWORD		offset;

	offset = rs_hdr[rtype];
						/* get base of objects	*/
						/*   and then index in	*/
	return( ((LPBYTE)rs_hdr) + LW(offset) + LW(rsize * rsindex) );
}


/*
 *	return address of given type and index, INTERNAL ROUTINE
*/
	LPVOID
get_addr(rstype, rsindex)
	REG UWORD	rstype;
	REG UWORD	rsindex;
{
	REG LPVOID	psubstruct;
	REG WORD	size;
	REG WORD	rt;
	WORD		valid;

	valid = TRUE;
	switch(rstype)
	{
	  case R_TREE:
#if ALCYON
		junk = LW(rsindex*4);
		return( LLGET( LLGET(APP_LOPNAME) + junk ) );
#else
		return(*APP_LOPNAME)[rsindex];
#endif
	  case R_OBJECT:
		rt = RT_OB;
		size = sizeof(OBJECT);
		break;
	  case R_TEDINFO:
	  case R_TEPTEXT:
		rt = RT_TEDINFO;
		size = sizeof(TEDINFO);
		break;
	  case R_ICONBLK:
	  case R_IBPMASK:
		rt = RT_ICONBLK;
		size = sizeof(ICONBLK);
		break;
	  case R_BITBLK:
	  case R_BIPDATA:
		rt = RT_BITBLK;
		size = sizeof(BITBLK);
		break;
	  case R_OBSPEC:
		psubstruct = get_addr(R_OBJECT, rsindex);
		return( (LPVOID)ROB_SPEC );
	  case R_TEPTMPLT:
	  case R_TEPVALID:
		psubstruct = get_addr(R_TEDINFO, rsindex);
	  	if (rstype == R_TEPTMPLT)
	  	  return( RTE_PTMPLT );
	  	else
	  	  return( RTE_PVALID );
	  case R_IBPDATA:
	  case R_IBPTEXT:
	  	psubstruct = get_addr(R_ICONBLK, rsindex);
	 	if (rstype == R_IBPDATA)
		  return( RIB_PDATA );
		else
		  return( RIB_PTEXT );
	  case R_STRING:
		return(LPVOID)( *(LPLPTR)( get_sub(rsindex, RT_FREESTR, sizeof(LONG)) ) );
	  case R_IMAGEDATA:
		return(LPVOID)( *(LPLPTR)( get_sub(rsindex, RT_FREEIMG, sizeof(LONG)) ) );
	  case R_FRSTR:
		rt = RT_FREESTR;
		size = sizeof(LONG);
		break;
	  case R_FRIMG:
		rt = RT_FREEIMG;
		size = sizeof(LONG);
		break;
	  default:
		valid = FALSE;
		break;
	}
	if (valid)
	  return( get_sub(rsindex, rt, size) );
	else
	  return((LPVOID)(-1L));
} /* get_addr() */


	LPLONG
fix_long(plong)
	REG LPLONG	plong;
{
	REG LONG	lngval;

	lngval = *plong;
	if (lngval != -1L)
	{
	  lngval += ((LONG)rs_hdr);
	  *plong = lngval;
	  return( (LPLONG)lngval );
	}
	else
	  return( 0x0L );
}

        VOID
fix_trindex()
{
	REG WORD	ii;
	REG LPLPTR	ptreebase;
	LPTREE		tree;

	ptreebase = (LPLPTR)get_sub(0, RT_TRINDEX, sizeof(LONG) );
	*APP_LOPNAME = ptreebase;

	for (ii = NUM_TREE-1; ii >= 0; ii--)
	{
	  tree = (LPTREE)fix_long(&ptreebase[ii]);
	  if ( (tree[ROOT].ob_state == OUTLINED) &&
	       (tree[ROOT].ob_type == G_BOX) &&
               (gl_opts.rsrc_compat & 1))
	   tree[ROOT].ob_state = SHADOWED;
	}
}

        VOID
fix_objects()
{
	REG WORD	ii;
	REG WORD	obtype;
	REG LPTREE	psubstruct;
	WORD		len;
	LPBYTE		farstr;

	for (ii = NUM_OBS-1; ii >= 0; ii--)
	{
	  psubstruct = (LPTREE)get_addr(R_OBJECT, ii);
	  rs_obfix(psubstruct, 0);
	  obtype = (*ROB_TYPE) & 0x00ff;
	  if ( (obtype != G_BOX) &&
	       (obtype != G_IBOX) &&
	       (obtype != G_BOXCHAR) )
	    fix_long((LPLONG)ROB_SPEC);
						/* fix up menu divider	*/
	  if ( (obtype == G_STRING) &&
	       ((*ROB_STATE ) & DISABLED) )
	  {
	    farstr = (LPBYTE)(*ROB_SPEC);
	    len = LSTRLEN(farstr);
	    if ( (*farstr == '-') &&
	    	 (farstr[len - 1] == '-') )
	    {
	      while(len--)
	        (*farstr++) = 0x13;
	    }
	  }
		
	}
}

        VOID
fix_tedinfo()
{
	REG WORD	ii, i;
	REG LPTEDI	psubstruct;
	LPWORD 		tl[2];
	LPBYTE FAR *ls[2];


	for (ii = NUM_TI-1; ii >= 0; ii--)
	{
	  psubstruct = get_addr(R_TEDINFO, ii);
	  tl[0] = tl[1] = 0x0L;
	  if (fix_ptr(R_TEPTEXT, ii) )
	  {
	    tl[0] = RTE_TXTLEN;
	    ls[0] = RTE_PTEXT;
	  }
	  if (fix_ptr(R_TEPTMPLT, ii) )
	  {
	    tl[1] = RTE_TMPLEN;
	    ls[1] = RTE_PTMPLT;
	  }
	  for(i=0; i<2; i++)
	  {
	    if (tl[i])
	      *(tl[i]) = LSTRLEN(*ls[i]) + 1;
	  }
	  fix_ptr(R_TEPVALID, ii);
	}
}

	VOID
fix_nptrs(cnt, type)
	WORD		cnt;
	WORD		type;
{
	REG WORD	i;

	for(i=cnt; i>=0; i--)
	  fix_long( get_addr(type, i) );
}


	WORD
fix_ptr(type, index)
	WORD		type;
	WORD		index;
{
	return( fix_long( get_addr(type, index) ) != 0x0L );
}



/*
*	Set global addresses that are used by the resource library sub-
*	routines
*/
	VOID
rs_sglobe(pglobal)
	LPWORD		pglobal;
{
	rs_global = (LPBYTE)pglobal;
	rs_hdr = (LPWORD) *(APP_LO1RESV);
}


/*
*	Free the memory associated with a particular resource load.
*/
	WORD
rs_free(pglobal)
	LPWORD		pglobal;
{
	rs_global = (LPBYTE)pglobal;

	dos_free((LPVOID)*(APP_LO1RESV));
	return(!DOS_ERR);
}/* rs_free() */


/*
*	Get a particular ADDRess out of a resource file that has been
*	loaded into memory.
*/
	WORD
rs_gaddr(pglobal, rtype, rindex, rsaddr)
	LPWORD		pglobal;
	UWORD		rtype;
	UWORD		rindex;
	REG LPVOID	*rsaddr;
{
	rs_sglobe(pglobal);

	*rsaddr = get_addr(rtype, rindex);
	return(*rsaddr != (LPVOID)(-1L));
} /* rs_gaddr() */


/*
*	Set a particular ADDRess in a resource file that has been
*	loaded into memory.
*/
	WORD
rs_saddr(pglobal, rtype, rindex, rsaddr)
	LPWORD		pglobal;
	UWORD		rtype;
	UWORD		rindex;
	LPVOID		rsaddr;
{
	REG LPLPTR	psubstruct;

	rs_sglobe(pglobal);

	psubstruct = (LPLPTR)get_addr(rtype, rindex);
	if (psubstruct != (LPLPTR)(-1L))
	{
	  *psubstruct = rsaddr;
	  return(TRUE);
	}
	else
	  return(FALSE);
} /* rs_saddr() */


/*
*	Read resource file into memory and fix everything up except the
*	x,y,w,h, parts which depend upon a GSX open workstation.  In the
*	case of the GEM resource file this workstation will not have
*	been loaded into memory yet.
*/
	WORD
rs_readit(pglobal, rsfname)
	LPWORD		pglobal;
	LPBYTE		rsfname;
{
	WORD		ibcnt;
	REG UWORD	rslsize, fd, ret;
						/* make sure its there	*/
	LSTCPY(ad_scmd, rsfname);
	if ( !sh_find(ad_scmd) )
	  return(FALSE);
						/* init global		*/
	rs_global = (LPBYTE)pglobal;
						/* open then file and	*/
						/*   read the header	*/
	fd = dos_open( ad_scmd, RMODE_RD);

	if ( !DOS_ERR )
	{
	  dos_read(fd, HDR_LENGTH, ADDR(&hdr_buff[0]));
						/* read in resource and	*/
						/*   interpret it	*/
	}
	if ( !DOS_ERR )
	{
	  
						/* get size of resource	*/
	  rslsize = hdr_buff[RS_SIZE];
						/* allocate memory	*/
	  rs_hdr = dos_alloc( LW(rslsize) );
	  if ( !DOS_ERR )
	  {
						/* read it all in 	*/
	    dos_lseek(fd, SMODE, 0x0L);
	    dos_read(fd, rslsize, rs_hdr);
	    if ( !DOS_ERR)
	    {
	      *APP_LO1RESV = (LONG)rs_hdr;
	      *APP_LO2RESV  = rslsize;
					/* xfer RT_TRINDEX to global	*/
					/*   and turn all offsets from	*/
					/*   base of file into pointers	*/
	      fix_trindex();
	      fix_tedinfo();
	      ibcnt = NUM_IB-1;
	      fix_nptrs(ibcnt, R_IBPMASK);
	      fix_nptrs(ibcnt, R_IBPDATA);
	      fix_nptrs(ibcnt, R_IBPTEXT);
	      fix_nptrs(NUM_BB-1, R_BIPDATA);
	      fix_nptrs(NUM_FRSTR-1, R_FRSTR);
	      fix_nptrs(NUM_FRIMG-1, R_FRIMG);
	    }
	  }
	}
						/* close file and return*/
	ret = !DOS_ERR;
	dos_close(fd);
	return(ret);
}


/*
*	Fix up objects separately so that we can read GEM resource before we
*	do an open workstation, then once we know the character sizes we
*	can fix up the objects accordingly.
*/
	VOID
rs_fixit(pglobal)
	LONG		pglobal;
{
	rs_sglobe(pglobal);
	fix_objects();
}


/*
*	RS_LOAD		mega resource load
*/
	WORD
rs_load(pglobal, rsfname)
	REG LPWORD	pglobal;
	LPBYTE		rsfname;
{
	REG WORD	ret;

	ret = rs_readit(pglobal, rsfname);
	if (ret)
	  rs_fixit(pglobal);
	return(ret);
}

