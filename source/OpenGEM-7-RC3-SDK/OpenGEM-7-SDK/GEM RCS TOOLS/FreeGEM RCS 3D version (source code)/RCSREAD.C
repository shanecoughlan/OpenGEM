 /*	RCSREAD.C	1/28/85 - 1/28/85	Tim Oren		*/
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
#include "ppdrcs.h"

#define ROB_TYPE (psubstruct + 6)	/* Long pointer in OBJECT	*/
#define ROB_FLAGS (psubstruct + 8)	/* Long pointer in OBJECT	*/
#define ROB_SPEC (psubstruct + 12)	/* Long pointer in OBJECT	*/

#define RIB_PMASK (psubstruct + 0)	/* Long pointers in ICONBLK	*/
#define RIB_PDATA (psubstruct + 4)
#define RIB_PTEXT (psubstruct + 8)

#define RBI_PDATA (psubstruct + 0)	/* Long pointer in BITBLK	*/
#define RBI_WB (psubstruct + 4)
#define RBI_HL (psubstruct + 6)
					/* in global array		*/
#define APP_LOPNAME (rs_global + 12)
#define APP_LO1RESV (rs_global + 16)
#define APP_LO2RESV (rs_global + 20)



WORD read_68K( BOOLEAN merge )
	/* simulate IBM PC data storage in "*.def" files. */
	{
	UWORD	idx, old_ndxno, rd_ndxno, rd_val, rd_kind;

	if ( merge ) old_ndxno = rcs_ndxno;
	else	     old_ndxno = 0;

	if (fseek( rcs_fdhndl, 0x0L, SEEK_SET) == EOF) return( rcs_ndxno = 0 );
	if(fread(&rd_ndxno, 1, 2, rcs_fdhndl) < 2)     return( rcs_ndxno = 0 );
	if (ferror(rcs_fdhndl))  	                   return ( rcs_ndxno = 0 );
	else rcs_ndxno = swap_bytes( rd_ndxno );
	rcs_ndxno = old_ndxno + rcs_ndxno;
	for ( idx = old_ndxno; idx < rcs_ndxno; idx++)
	    {
	    if(fread( &rd_val, 1, 2, rcs_fdhndl) < 2 )  return( rcs_ndxno = 0 );
	    rcs_index[idx].val = (BYTE *) ( LW(swap_bytes(rd_val)) & 0xffffL );
	    if(fread( &rd_kind, 1, 2, rcs_fdhndl) < 2 )  return( rcs_ndxno = 0 );
	    rcs_index[idx].kind = swap_bytes( rd_kind );
		if (fread( rcs_index[idx].name, 1, 10, rcs_fdhndl) < 10) 
			return( rcs_ndxno = 0 );
	    }
	return 0;
	}


/* Does not rely on internal representation of INDEX struct */
	
WORD read_i8086( BOOLEAN merge )
	{
	UWORD	idx, old_ndxno, rd_ndxno, rd_val, rd_kind;

	if ( merge ) old_ndxno = rcs_ndxno;
	else	     old_ndxno = 0;

	if (fseek( rcs_fdhndl, 0x0L, SEEK_SET) == EOF) return( rcs_ndxno = 0 );
	if(fread(&rd_ndxno, 1, 2, rcs_fdhndl) < 2)     return( rcs_ndxno = 0 );
	if (ferror(rcs_fdhndl))  	                   return ( rcs_ndxno = 0 );
	else rcs_ndxno = rd_ndxno;
	rcs_ndxno = old_ndxno + rcs_ndxno;
	for ( idx = old_ndxno; idx < rcs_ndxno; idx++)
	    {
	    if(fread( &rd_val, 1, 2, rcs_fdhndl) < 2 )  return( rcs_ndxno = 0 );
	    
	    rcs_index[idx].val = (BYTE *) ( LW(rd_val) & 0xffffL );

	    if(fread( &rd_kind, 1, 2, rcs_fdhndl) < 2 )  return( rcs_ndxno = 0 );
	    rcs_index[idx].kind = rd_kind;
		if (fread( rcs_index[idx].name, 1, 10, rcs_fdhndl) < 10) return( rcs_ndxno = 0 );
	    }
	return 0;
	}
	

	
WORD open_files(WORD def)
	{
	WORD	ii;

	if (!def)
	if (!get_file())
		return (FALSE);
	FOREVER
		{
		rcs_frhndl = fopen(rcs_rfile, "rb");

		if (rcs_frhndl) break;

		if (!ppd_form_error(errno)) return FALSE; 
		}
	FOREVER
		{
		rcs_fdhndl = fopen(rcs_dfile, "rb");

		if (rcs_fdhndl) return TRUE;

		ii = hndl_alert(2, string_addr(STNODEF));
		if (ii == 1)
			{
			fclose(rcs_frhndl);
			return (FALSE);
			}
		if (ii == 2)
			{
			rcs_fdhndl = NULL;
			return (TRUE);
			}
		}
	}


LPBYTE get_sub(UWORD rsindex, WORD rtype, WORD rsize)
{
	UWORD		offset;

	offset = LWGET( ((LPBYTE)rs_hdr) + LW(rtype*2) );
						/* get base of objects	*/
						/*   and then index in	*/
	return( ((LPBYTE)rs_hdr) + LW(offset) + LW(rsize * rsindex) );
}


/*
 *	return address of given type and index, INTERNAL ROUTINE
*/
LPBYTE get_addr(WORD rstype, WORD rsindex)
{
	LPTEDI		ptedi;
	LPICON		picon;
	LPBYTE		psubstruct;
	WORD		size;
	WORD		rt;
	WORD		valid;

	valid = TRUE;
	switch(rstype)
	{
	  case R_OBJECT:	rt = RT_OB;
						size = sizeof(OBJECT);
						break;
	  case R_TEDINFO:
	  case R_TEPTEXT:	rt = RT_TEDINFO;
						size = sizeof(TEDINFO);
						break;
	  case R_ICONBLK:
	  case R_IBPMASK:	rt = RT_ICONBLK;
						size = sizeof(ICONBLK);
						break;
	  case R_BITBLK:
	  case R_BIPDATA:	rt = RT_BITBLK;
						size = sizeof(BITBLK);
						break;
	  case R_OBSPEC:	psubstruct = get_addr(R_OBJECT, rsindex);
						return( ROB_SPEC );
	  case R_TEPTMPLT:
	  case R_TEPVALID:	ptedi = (LPTEDI)get_addr(R_TEDINFO, rsindex);
	  					if (rstype == R_TEPTMPLT) return (LPBYTE)&(ptedi->te_ptmplt);
	  					else					  return (LPBYTE)&(ptedi->te_pvalid);
	  case R_IBPDATA:
	  case R_IBPTEXT:	picon = (LPICON)get_addr(R_ICONBLK, rsindex);
	 					if (rstype == R_IBPDATA)  return (LPBYTE)&(picon->ib_pdata);
		  				else					  return (LPBYTE)&(picon->ib_ptext);
	  case R_STRING:
		return( (LPBYTE)LLGET( get_sub(rsindex, RT_FREESTR, sizeof(LONG)) ) );
	  case R_IMAGEDATA:
		return( (LPBYTE)LLGET( get_sub(rsindex, RT_FREEIMG, sizeof(LONG)) ) );
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
	  return((LPBYTE)-1);
} /* get_addr() */


WORD fix_long(LPLPTR plong)
{
	LPBYTE	lngval;

	lngval = (LPBYTE)(*plong);
	if (lngval != (LPBYTE)-1)
	{
	  *plong = (LPBYTE)((LPBYTE)rs_hdr + (LONG)lngval);
	  return(TRUE);
	}
	else return(FALSE);
}


	VOID
fix_trindex()
{
	WORD		ii;
	LPBYTE		ptreebase;

	ptreebase = get_sub(0, RT_TRINDEX, sizeof(LONG));

	for (ii = rs_hdr->rsh_ntree - 1; ii >= 0; ii--)
	  fix_long((LPLPTR)(ptreebase + LW(ii*4)));
}		     


WORD fix_ptr(WORD type, WORD index)
{
	return( fix_long( (LPLPTR)get_addr(type, index) ) );
}


	VOID
fix_objects()
{
	WORD		ii;
	WORD		obtype;
	LPTREE		psubstruct;


	for (ii = rs_hdr->rsh_nobs - 1; ii >= 0; ii--)
	{
	  psubstruct = (LPTREE)get_addr(R_OBJECT, ii);
	  rs_obfix(psubstruct, 0);
	  obtype = LLOBT( psubstruct->ob_type );
	  switch (obtype)
	  {
		case G_TEXT:
		case G_BOXTEXT:
		case G_FTEXT:
		case G_FBOXTEXT:
		case G_TITLE:
		case G_ICON:
		case G_IMAGE:
		case G_STRING:
		case G_BUTTON:
			fix_long(&psubstruct->ob_spec);
			break;
		default:
			break;
	  }
	psubstruct->ob_flags &= ~LASTOB;	/* zap LASTOB's */
	}
}

VOID fix_tedinfo()
{
	WORD		ii, i;
	LPTEDI		psubstruct;
	LPWORD		tl[2];
	LPLSTR		ls[2];


	for (ii = rs_hdr->rsh_nted - 1; ii >= 0; ii--)
	{
	  psubstruct = (LPTEDI)get_addr(R_TEDINFO, ii);
	  tl[0] = tl[1] = 0x0L;
	  if (fix_ptr(R_TEPTEXT, ii) )
	  {
	    tl[0] = &psubstruct->te_txtlen;
	    ls[0] = &psubstruct->te_ptext;
	  }
	  if (fix_ptr(R_TEPTMPLT, ii) )
	  {
	    tl[1] = &psubstruct->te_tmplen;
	    ls[1] = (LPLSTR)(&psubstruct->te_ptmplt);
	  }
	  for(i=0; i<2; i++)
	  {
		if (tl[i]) tl[i][0] = LSTRLEN (ls[i][0]) + 1;
	  }
	  fix_ptr(R_TEPVALID, ii);
	}
}


VOID fix_iconblk()
{
	WORD		ii;

	for (ii = rs_hdr->rsh_nib - 1; ii >= 0; ii--)
	{
	  fix_ptr(R_IBPMASK, ii);
	  fix_ptr(R_IBPDATA, ii);
	  fix_ptr(R_IBPTEXT, ii);
	}
}


VOID fix_bitblk()
{
	WORD	ii;

	for (ii = rs_hdr->rsh_nbb - 1; ii >= 0; ii--)
	  fix_ptr(R_BIPDATA, ii);
}


VOID fix_frstr(/* WORD type, WORD index */)
{
	WORD		ii;

	for (ii = rs_hdr->rsh_nstring - 1; ii >= 0; ii--)
	  fix_ptr(R_FRSTR, ii);
}


VOID fix_frimg()
{
	WORD	ii;

	for (ii = rs_hdr->rsh_nimages - 1; ii >= 0; ii--)
	  fix_ptr(R_FRIMG, ii);
}
       
	    
VOID fix_all()
	{
	fix_trindex();
	fix_objects();
	fix_tedinfo();
	fix_iconblk();
	fix_bitblk();
	fix_frstr();
	fix_frimg();
	}   

VOID comp_str(LPRSHDR hdr)
	{
	LPLSTR  frstr;
	LPBYTE	where;
	LPTREE	tree, maddr;
	WORD	istr, nstr, lstr, w, h, obj, ndex;
	BYTE	name[9];

	if ( !(nstr = hdr->rsh_nstring)) 
		return;
	frstr = (LPLSTR)(((LPBYTE)hdr) + hdr->rsh_frstr);

	ini_tree(&maddr, NEWPANEL);
	tree = copy_tree(maddr, ROOT, TRUE);
	add_trindex(tree);
	ini_tree(&maddr, FREEPBX);

	for (istr = 0; istr < nstr; istr++)
		{
		where = frstr[istr];
		if (where != (LPBYTE)-1)
			{
			lstr = LSTRLEN(where);
			obj = copy_objs(maddr, PBXSTR, tree, FALSE);
			objc_add(tree, ROOT, obj);
			tree[obj].ob_spec = where;
			tree[obj].ob_width = gl_wchar * lstr;
			if ((ndex = find_value(where)) != NIL)
				{
				set_value(ndex, (LPBYTE)(&tree[obj]));
				set_kind(ndex, OBJKIND);
				}
			}
		}
	tree[ROOT].ob_width = 1;	/* fool arrange_tree into putting */
	arrange_tree(tree, 2 * gl_wchar, gl_hchar, &w, &h);
	tree[ROOT].ob_height = h;	/* each on a different line */
	tree[ROOT].ob_width  = max (w, 20 * gl_wchar);
	unique_name(&name[0], "FRSTR%W", 1);	/* make up a name */
	new_index((LPBYTE)tree, FREE, name);
	hdr->rsh_nstring = 0;
	}


	
VOID comp_img(LPRSHDR hdr)
	{
	LPBIT	far *frimg;
	LPBIT   where;
	LPTREE	tree, maddr;
	WORD	iimg, nimg, w, h, obj, ndex;
	BYTE	name[9];

	if ( !(nimg = hdr->rsh_nimages)) return;
	frimg = (LPBIT far *)(((LPBYTE)hdr) + hdr->rsh_frimg);

	ini_tree(&maddr, NEWPANEL);
	tree = copy_tree(maddr, ROOT, TRUE);
	add_trindex(tree);
	ini_tree(&maddr, FREEPBX);

	for (iimg = 0; iimg < nimg; iimg++)
		{
		where = frimg[iimg];
		obj = copy_objs(maddr, PBXIMG, tree, FALSE);
		objc_add(tree, ROOT, obj);
		tree[obj].ob_spec = where;
		tree[obj].ob_height = where->bi_hl;
		tree[obj].ob_width  = where->bi_wb << 3;
		if ((ndex = find_value((LPBYTE)where)) != NIL)
			{
			set_value(ndex, (LPBYTE)&tree[obj]);
			set_kind(ndex, OBJKIND);
			}
		}
	tree[ROOT].ob_width = full.g_w;	
	arrange_tree(tree, 2 * gl_wchar, gl_hchar, &w, &h);
	tree[ROOT].ob_height = h;
	tree[ROOT].ob_width  = w;
	map_tree(tree, ROOT, NIL, trans_obj);
	unique_name(&name[0], "FRIMG%W", 1);	/* make up a name */
	new_index((LPBYTE)tree, FREE, name);
	hdr->rsh_nimages = 0;
	}


	
WORD read_files()
	{
	WORD	ii, where;
	WORD	ntree, nobj;
	BYTE	name[9];
	if ( ad_clip ) 
		clr_clip();
	ini_buff();
	if ( !dmcopy(rcs_frhndl, 0x0L, head, sizeof(RSHDR)) || ferror(rcs_frhndl))
		{	  
		fclose(rcs_frhndl);
		ini_buff();
		return (FALSE);
		}
	if (avail_mem() < head->rsh_rssize)
		{
		hndl_alert(1, string_addr(STNROOM) );
		ini_buff();
		return (FALSE);
		}
	if(!dmcopy(rcs_frhndl, 
	           (LONG)sizeof(RSHDR), 
	           ((LPBYTE)head)   + sizeof(RSHDR),
		       head->rsh_rssize - sizeof(RSHDR)) || ferror(rcs_frhndl))
		{
		fclose(rcs_frhndl);
		ini_buff();
		return (FALSE);
		}
	fclose(rcs_frhndl);
	rcs_free = ((LPBYTE)head + head->rsh_rssize);

#if	  MC68K   /* memory can only be written on even boundary */
	if (rcs_free & 0x1) 
	    rcs_free++;
#endif

	rs_hdr = head;
	fix_all();
	map_all(trans_obj);
	if (!rcs_fdhndl) rcs_ndxno = 0;
	else

#if	MC68K
		read_68k( FALSE );
#else
/* This simple DFN-reader won't work in PPDRCS, because the INDEX struct
  is a different size from the INDEX on disc 
		{
		if (!dmcopy (rcs_fdhndl, 0x0L, ADDR(&rcs_ndxno), 2) || ferror(rcs_fdhndl)) rcs_ndxno = 0;
		else
			{
				
			if (!dmcopy(rcs_fdhndl, 0x02L, ADDR(rcs_index),
				sizeof(INDEX) * rcs_ndxno) || ferror(rcs_fdhndl))
				rcs_ndxno = 0;

			

			}
		}
*/
		read_i8086( FALSE );
#endif

	fclose(rcs_fdhndl);
			/* convert stored values to addresses */
	for (ii = 0; ii < rcs_ndxno; ii++)
		{
		switch ( get_kind(ii) ) {
			case UNKN:
			case PANL:
			case DIAL:
			case MENU:
				set_value(ii, (LPBYTE) tree_addr(
					(WORD) get_value(ii)) );
				break;
			case ALRT:
			case FRSTR:
				set_value(ii, (LPBYTE) str_addr(
					(WORD) get_value(ii)) );
				break;
			case FRIMG:
				set_value(ii, (LPBYTE) img_addr(
					(WORD) get_value(ii)) );
				break;
			case OBJKIND:
				nobj = (WORD) LLOBT( (UWORD) get_value(ii) ) & 0xff;
				ntree = (WORD) LHIBT( (UWORD) get_value (ii) ) & 0xff;
				set_value(ii, (LPBYTE) (&tree_addr(ntree)[nobj]));
				break;
			default:
				break;
			}
		}

	comp_alerts(head);	/* convert freestrs into alert trees */
	comp_str(head);	/* scavenge all other freestrs	*/
	comp_img(head);	/* and likewise for free images */
			/* make sure all trees are indexed */
	for (ii = 0; ii < head->rsh_ntree; ii++)
		if (find_tree(ii) == NIL)
			{
			unique_name(name, "TREE%W", ii + 1);
			where = new_index((LPBYTE) tree_addr(ii), UNKN, name);
			}

	return head->rsh_rssize;
	}


	
WORD merge_files()
	{
	UWORD	foo, indoff /*, rd_val, rd_kind */;
	WORD	total, i, ii;
	WORD	nobj, ntree, old_ntree;
	LONG	old_ndxno;
	LPLSTR  frstr;
	LPBIT   FAR *frimg;
	LPRSHDR merge;
	LPBYTE	here;
	LPLPTR  oldndx, newndx;
	
	merge = (LPRSHDR)get_mem(sizeof(RSHDR));
	if(!dmcopy(rcs_frhndl, 0x0L, merge, sizeof(RSHDR) ) || ferror(rcs_frhndl))
		{ 
		fclose(rcs_frhndl);
		return (FALSE);
		}
	if (rcs_low || avail_mem() < (UWORD)(merge->rsh_rssize) - (UWORD) sizeof(RSHDR) )
		{
		hndl_alert(1, string_addr(STNROOM));
		return (FALSE);
		}
			/* synchronize objects */
	foo = (UWORD) ((LONG)(   (LPBYTE)merge + merge->rsh_object ) -
		           (LONG)(   (LPBYTE)head  + head->rsh_object  ));
	foo %= sizeof(OBJECT);
	if (foo) 
	{
		get_mem(foo = sizeof(OBJECT) - foo);
		merge = merge + foo;
		if(!dmcopy(rcs_frhndl, 0x0L, merge, sizeof(RSHDR) ) || ferror(rcs_frhndl))
		{
			fclose(rcs_frhndl);
			return FALSE;
		}
	} 

	here = get_mem(merge->rsh_rssize - (UWORD) sizeof(RSHDR));
	if(!dmcopy (rcs_frhndl, (LONG) sizeof(RSHDR), 
		here, merge->rsh_rssize) || ferror(rcs_frhndl))
		{
		fclose(rcs_frhndl);
		return (FALSE);
		}
	fclose(rcs_frhndl);
	rs_hdr = merge;
	fix_all();
	
	old_ntree = head->rsh_ntree;
	if (merge->rsh_ntree)
		{
		total  = head->rsh_ntree + merge->rsh_ntree;
		newndx = (LPLPTR)get_mem(sizeof(LONG) * total);
		indoff = (UWORD) ((LPBYTE)newndx - (LPBYTE)head);		
		for (i = 0; i < head->rsh_ntree; i++)
			{
			*newndx = tree_addr(i);
			++newndx;
			}
		oldndx = (LPLPTR)(((LPBYTE)merge) + merge->rsh_trindex);
		for (i = 0; i < merge->rsh_ntree; i++)
			{
			*newndx = *oldndx;
			++newndx;
			++oldndx;
			}
		head->rsh_ntree   = total;
		head->rsh_trindex = indoff;
		}

	if (rcs_fdhndl)

		{  


#if	MC68K				      
		{	
		old_ndxno = rcs_ndxno;
		read_68k( TRUE );
		total = rcs_ndxno;
		rcs_ndxno = old_ndxno;
		}
#else
		{
		old_ndxno = rcs_ndxno;		
		if(!dmcopy(rcs_fdhndl, 0x0L, ADDR(&total), sizeof(WORD) ) 
		|| ferror(rcs_fdhndl)) total = 0;
		else
			{
			if(!dmcopy(rcs_fdhndl, 0x02L, ADDR(&rcs_index[old_ndxno]),
				sizeof(INDEX) * total) || ferror(rcs_fdhndl)) total = 0;
			else total = total + old_ndxno;
			}
		}
#endif

		for ( ii = old_ndxno; ii < total; ii++ )				
			{	
			if (find_name(&rcs_index[ii].name[0]) != NIL)
			if (tree_kind(rcs_index[ii].kind))
				unique_name(&rcs_index[ii].name[0], "TREE%W", old_ntree);
			else
				unique_name(&rcs_index[ii].name[0], "OBJ%W", 0);
			switch (rcs_index[ii].kind) {
				case UNKN:
				case PANL:
				case DIAL:
				case MENU:
					rcs_index[ii].val = (LPBYTE) tree_addr(
						(WORD) rcs_index[ii].val + old_ntree);
					break;
				case ALRT:
				case FRSTR:
					frstr = (LPLSTR)((LPBYTE)merge + merge->rsh_frstr);
					rcs_index[ii].val = (BYTE *) LLGET(frstr + 
						(UWORD) (sizeof(LONG) * (WORD) rcs_index[ii].val));
					break;
				case FRIMG:
					frimg = (LPBIT far *)((LPBYTE)merge + merge->rsh_frimg );
					rcs_index[ii].val = (BYTE *) LLGET(frimg +
						(UWORD) (sizeof(LONG) * (WORD)rcs_index[ii].val));
					break;

				case OBJKIND:
					nobj = (WORD) LLOBT( (UWORD) rcs_index[ii].val) & 0xff;
					ntree = (WORD) LHIBT( (UWORD) rcs_index[ii].val) & 0xff;
					rcs_index[ii].val = (LPBYTE) (tree_addr(ntree + old_ntree) 
						+ (UWORD) (nobj * sizeof(OBJECT))); 
					break;
				default:
					break;
				}
			if (new_index(rcs_index[ii].val, rcs_index[ii].kind, &rcs_index[ii].name[0]) == NIL)
				break;
			}
		fclose(rcs_fdhndl);
		}


	for (i = old_ntree; i < head->rsh_ntree; i++)
		{
		map_tree(tree_addr(i), ROOT, NIL, trans_obj); 
		if (find_tree(i) == NIL)
			{
			unique_name(&rcs_index[i].name[0], "TREE%W", i + 1);
			if (new_index((LPBYTE) tree_addr(i), UNKN, &rcs_index[i].name[0]) == NIL)
				break;
			}
		}

	comp_alerts(merge);
	comp_str(merge);
	comp_img(merge);
	return (TRUE);
	}
