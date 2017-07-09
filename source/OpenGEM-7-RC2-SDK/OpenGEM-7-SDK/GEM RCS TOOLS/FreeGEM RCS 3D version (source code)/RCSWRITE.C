 /*	RCSWRITE.C	1/28/85 - 1/28/85	Tim Oren		*/
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
#include "ppdrcs.h"

typedef	struct	outobj
	{
	WORD	next;
	WORD	head;
	WORD	tail;
	BYTE	*type;
	BYTE	*flags;
	BYTE	*state;
	LONG	spec;
	UWORD	x;
	UWORD	y;
	UWORD	w;
	UWORD	h;
	} OUTOBJ;

typedef struct 	h_out
	{
	BYTE	*p0;
	WORD	p1;
	WORD	p2;
	} HOUT;



VOID do_tally()
	{
	clr_tally();
	map_all(tally_obj);
	tally_free();
	rcs_tally.nbytes += head->rsh_ntree * sizeof(LONG);
	rcs_tally.nbytes += sizeof(RSHDR);
	}


	LONG
get_dspace( drv )
	WORD	drv;
	{
	LONG	dsk_tot, dsk_avail;
	dos_space( drv, &dsk_tot, &dsk_avail);
	return ( dsk_avail );
	}

	WORD
dspace( drv, nbytes )
	WORD	drv;
	UWORD	nbytes;
	{
	LONG	dsk_tot, dsk_avail;
	dos_space( drv, &dsk_tot, &dsk_avail);
	if ( nbytes < (UWORD) dsk_avail )
		return (FALSE);
	else
		return (TRUE);
	}


/* the following two routines were written to simulate the ibm pc file */
/*   structure for "*.def" files. */

UWORD swap_bytes( UWORD x )
	{
	
	return ( ((UWORD) LLOBT(x) << 8 ) | (UWORD) LHIBT(x) );

	}

VOID wr_68kdef()
{
	UWORD	idx, wr_ndxno,wr_val, wr_kind;
	
	wr_ndxno = swap_bytes(rcs_ndxno);

	fwrite(&wr_ndxno, 1, 2, rcs_fdhndl);

	for ( idx = 0; idx < rcs_ndxno; idx++ )
	{
		wr_val = swap_bytes( (UWORD) rcs_index[idx].val );

		fwrite(&wr_val, 1, 2, rcs_fdhndl);
		wr_kind = swap_bytes( rcs_index[idx].kind );
		fwrite(&wr_kind,                1,  2, rcs_fdhndl);
		fwrite(&rcs_index[idx].name[0], 1, 10, rcs_fdhndl);
	}
}
	

VOID wr_i8086def()
{
	UWORD	idx, wr_ndxno,wr_val, wr_kind;
	
	wr_ndxno = rcs_ndxno;

	fwrite(&wr_ndxno, 1, 2, rcs_fdhndl);

	for ( idx = 0; idx < rcs_ndxno; idx++ )
	{
		wr_val =  (UWORD) rcs_index[idx].val;

		fwrite(&wr_val, 1, 2, rcs_fdhndl);
		wr_kind = rcs_index[idx].kind;
		fwrite(&wr_kind,                1,  2, rcs_fdhndl);
		fwrite(&rcs_index[idx].name[0], 1, 10, rcs_fdhndl);
	}
}
	

	
LPVOID c_imgno(LPVOID addr)
	{
	WORD	imgno;

	if (addr == (LPVOID)-1) return (LPVOID)-1;
	for (imgno = 0; imgno < c_nimage; imgno++)
		if ( (LONG) addr == c_obndx[imgno + c_nstring])
			return ((LPVOID)imgno);	

	return (LPVOID)-1;
  	}

LPVOID c_strno(LPBYTE addr)
	{
	WORD	strno;

	if (addr == (LPBYTE)-1) return (LPVOID)-1;
	for (strno = 0; strno < c_nstring; strno++)
		if ( (WORD) addr == c_obndx[strno])
			return ((LPVOID)strno);
	return (LPVOID)-1;
	}

VOID c_wrlin(VOID)
	{
	fwrite(hline, 1, strlen(hline), rcs_fhhndl);
	}


	
VOID c_comma(WORD yesno)
	{
	if (yesno)
		{
		strcpy(hline, ",\r\n");
		c_wrlin();
		}
	}


	
VOID c_tail(WORD used)
	{
	if (!rcs_cflag) return;
	if (!used)      strcpy(hline, "0};\r\n");
	else 		    strcpy(hline, "};\r\n");
	c_wrlin();
	}


	
VOID wr_sync()
	{
	if (rcs_wraddr & 0x1)
		{
		fputc('0', rcs_frhndl);
		rcs_wraddr++;
		}
	}

	
VOID wr_header()
	{
	fseek (rcs_frhndl, 0L, SEEK_SET);
	lfwrite(head, 1, sizeof(RSHDR), rcs_frhndl);
	}

	
VOID c_defline(BYTE *name, WORD val)
	{
	merge_str(hline, "#define %S %W\r\n", name, val);
	c_wrlin();
	}


	
VOID c_baseline(WORD num, BYTE *field, WORD val)
	{
	BYTE	name[9];

	merge_str(name,"T%W%S", num, field);
	c_defline(name, val);
	}

VOID c_bases(VOID)
	{
	WORD	i;

	if (!rcs_cflag)
		return;
	clr_tally();
	for (i = 0; i < head->rsh_ntree; i++)
		{
		c_baseline(i, "OBJ", rcs_tally.nobj);
/*		c_baseline(i, "IB", rcs_tally.nib);	*/
/*		c_baseline(i, "BB", rcs_tally.nbb);	*/
/*		c_baseline(i, "TI", rcs_tally.nti);	*/
/*		c_baseline(i, "IMG", rcs_tally.nimg);	*/
/*		c_baseline(i, "STR", rcs_tally.nstr);	*/
		map_tree( tree_addr(i), ROOT, NIL, tally_obj);
		}
	c_defline("FREEBB", rcs_tally.nbb);
	c_defline("FREEIMG", rcs_tally.nimg);
	c_defline("FREESTR", rcs_tally.nstr);
	}

VOID c_strhead()
	{
	if (!rcs_cflag) return;
	strcpy(hline, "\r\nBYTE *rs_strings[] = {\r\n");
	c_wrlin();
	c_nstring = 0;
	}


	
VOID c_string(LPBYTE addr)
	{
	WORD	hsub;
	BYTE	hchar;

	if (!rcs_cflag) return; 
	c_comma(c_nstring);

	hline[0] = '"';
	hsub = 1;

	do {
		hchar = LBGET(addr++);
		if (hchar == '"')
			hline[hsub++] = '\\';
		else if (hchar == '\\')
			hline[hsub++] = '\\';
		if (!hchar)
			hline[hsub++] = '"';
		hline[hsub++] = hchar;
		if (hchar && hsub == 70)
			{
			hline[hsub] = '\0';
			strcat(hline, "\\\r\n");
			c_wrlin();
			hsub = 0;
			}
		} while (hchar);

	c_wrlin();
	c_obndx[c_nstring++] = rcs_wraddr;
	}

VOID wr_str(LPLSTR where)
	{
	LPBYTE	staddr;
	WORD	stlen;

	if ( (staddr = (*where)) == (LPBYTE)-1) return;
	c_string(staddr);
	LLSET(where, (LONG) rcs_wraddr & 0xffff);
	stlen = 1 + LSTRLEN(staddr);

	lfwrite(staddr, 1, stlen, rcs_frhndl); 
	rcs_wraddr += stlen;
	}


	
WORD wr_string(LPTREE tree, WORD which)
	{
	LPICON	iconblk;
	LPTEDI  tedinfo;
	WORD	type;

	type = tree[which].ob_type & 0xFF;
	if (type == G_BUTTON || type == G_STRING || type == G_TITLE)
		{
		wr_str((LPLSTR)&tree[which].ob_spec);
		return;
		}
	if (type == G_ICON)
		{
		if ( (iconblk = (LPICON)(tree[which].ob_spec)) != (LPICON)-1)
			wr_str((LPLSTR)&iconblk->ib_ptext);
		return;
		}
	if (type == G_TEXT || type == G_BOXTEXT ||
		type == G_FTEXT || type == G_FBOXTEXT) 
		{
		if ( (tedinfo = (LPTEDI)(tree[which].ob_spec)) == (LPTEDI)-1)
			return;
		wr_str((LPLSTR)&(tedinfo->te_ptext));
		wr_str((LPLSTR)&(tedinfo->te_ptmplt));
		wr_str((LPLSTR)&(tedinfo->te_pvalid));
		}
	return 1;
	}

	
VOID map_frstr(VOID)
	{
	WORD	ifree, nfree;

	c_frstr = c_nstring;
	if ( !(nfree = head->rsh_nstring) ) return;
	for (ifree = 0; ifree < nfree; ifree++)
		wr_str(str_ptr(ifree));
	}


	
VOID c_frshead()
	{
	if (!rcs_cflag) return;
	strcpy(hline, "\r\nLONG rs_frstr[] = {\r\n");
	c_wrlin();
	c_nfrstr = 0;
	}


	
VOID c_freestr(WORD which)
	{
	if (!rcs_cflag) return;
	c_comma(c_nfrstr);
	which += c_frstr;
	merge_str(hline, "%WL", which);
	c_wrlin();
	c_nfrstr++;
	}


	
VOID wr_freestr()
	{
	WORD	ifree, nfree;

	if ( !(nfree = head->rsh_nstring) ) return;
	for (ifree = 0; ifree < nfree; c_freestr(ifree++));
	nfree *= sizeof(LONG);
	lfwrite(str_ptr(0), 1, nfree, rcs_frhndl);
	head->rsh_frstr = rcs_wraddr;
	rcs_wraddr += nfree;
	}


	
VOID c_imdata(addr, size)
	LONG	addr;
	WORD	size;
	{
	WORD	iwd, vwd;

	if (!rcs_cflag)
		return;
	merge_str(hline, "\r\nWORD IMAG%W[] = {", c_nimage);
	c_wrlin();

	for (iwd = 0; iwd < size; iwd += 2)
		{
		if (iwd)
			{
			strcpy(hline, ", ");
			c_wrlin();
			}
		if (iwd % 8 == 0)
			{
			strcpy(hline, "\r\n");
			c_wrlin();
			}
		vwd = LWGET(addr + iwd);
		merge_str(hline, "0x%H", vwd);
		c_wrlin();
		}

	c_tail(TRUE);
	c_obndx[c_nstring + c_nimage++] = rcs_wraddr;
	}


	
VOID wr_imdata(LPLPTR where, WORD w, WORD h)
	{
	WORD	size;
	LPVOID	imaddr;

	if ( (imaddr = *where) == (LPVOID)-1) return;
	size = w * h;
	gsx_untrans(imaddr, w, ADDR(wr_obndx), w, h);
	imaddr = ADDR(wr_obndx);
	c_imdata(imaddr, size);
	LLSET(where, (LONG) rcs_wraddr & 0xffff);

	lfwrite(imaddr, 1, size, rcs_frhndl);
	rcs_wraddr += size;
	}


WORD wr_image(LPTREE tree, WORD which)
	{
	LPICON  iconblk;
	LPBIT   bitblk;
	WORD	type, w, h;

	type = tree[which].ob_type & 0xFF;
	if (type == G_IMAGE)
		{
		if ( (bitblk = (LPBIT)tree[which].ob_spec) == (LPBIT)-1) return;
		w = bitblk->bi_wb;
		h = bitblk->bi_hl;
		wr_imdata((LPLPTR)&bitblk->bi_pdata, w, h);
		return;
		}
	if (type == G_ICON)
		{
		if ( (iconblk = (LPICON)tree[which].ob_spec) == (LPICON)-1) return;
		w = (iconblk->ib_wicon + 7) / 8;
		h =  iconblk->ib_hicon;
		wr_imdata((LPLPTR)&iconblk->ib_pmask, w, h);
		wr_imdata((LPLPTR)&iconblk->ib_pdata, w, h);
		}
	return 1;
	}


	
VOID map_frimg(VOID)
	{
	WORD	ifree, nfree, w, h;
	LPBIT	bitblk;

	if ( !(nfree = head->rsh_nimages) ) return;
	for (ifree = 0; ifree < nfree; ifree++)
		{
		if ( (bitblk = img_addr(ifree)) == (LPBIT)-1)
			break;
		w = bitblk->bi_wb;
		h = bitblk->bi_hl;
		wr_imdata((LPLPTR)&bitblk->bi_pdata, w, h);
		}
	}

VOID c_foobar()
	{
	WORD	img;

	if (!rcs_cflag)
		return;
	strcpy(hline, "\r\nstruct foobar {\r\n\tWORD\tdummy;");
	c_wrlin();
	strcpy(hline, "\r\n\tWORD\t*image;\r\n\t} rs_imdope[] = {\r\n");
	c_wrlin();

	for (img = 0; img < c_nimage; img++)
		{
		c_comma(img);
		merge_str(hline, "0, &IMAG%W[0]", img);
		c_wrlin();
		}

	c_tail(img);
	}


	
VOID c_iconhead()
	{
	if (!rcs_cflag) return;
	strcpy(hline, "\r\nICONBLK rs_iconblk[] = {\r\n");
	c_wrlin();
	c_nib = 0;
	}


	
VOID c_iconblk(LPICON addr)
	{
	ICONBLK	here;

	if (!rcs_cflag)
		return;
	LBCOPY(ADDR(&here), (LPVOID)addr, sizeof(ICONBLK) );
	here.ib_pdata = (LPVOID)c_imgno(here.ib_pdata);
	here.ib_pmask = (LPVOID)c_imgno(here.ib_pmask);
	here.ib_ptext = (LPBYTE)c_strno(here.ib_ptext);
	c_comma(c_nib);
	merge_str(hline, "%LL, %LL, %LL, %W,%W,%W, %W,%W,%W,%W, %W,%W,%W,%W",
		here.ib_pmask, here.ib_pdata, here.ib_ptext,
		here.ib_char,  here.ib_xchar, here.ib_ychar,
		here.ib_xicon, here.ib_yicon, here.ib_wicon, here.ib_hicon,
		here.ib_xtext, here.ib_ytext, here.ib_wtext, here.ib_htext);
	c_wrlin();
	c_nib++;
	}

	
WORD wr_iconblk(LPTREE tree, WORD which)
	{
	LPICON	iconblk;

	if ((tree[which].ob_type & 0xFF) != G_ICON)      return;
	if ( (iconblk = (LPICON)tree[which].ob_spec) ==  (LPICON)-1L) return;
	tree[which].ob_spec = (LPVOID)((LONG)rcs_wraddr & 0xffff);
	lfwrite(iconblk, 1, sizeof(ICONBLK), rcs_frhndl);
	rcs_wraddr += sizeof(ICONBLK);
	++head->rsh_nib;
	c_iconblk(iconblk);
	}

	
VOID c_bithead(VOID )
	{
	if (!rcs_cflag) return;
	strcpy(hline, "\r\nBITBLK rs_bitblk[] = {\r\n");
	c_wrlin();
	c_nbb = 0;
	}


	
VOID c_bitblk(LPBIT addr)
	{
	BITBLK	here;

	if (!rcs_cflag) return;
	LBCOPY(ADDR(&here), (LPVOID)addr, sizeof(BITBLK) );
	here.bi_pdata = (LPVOID)c_imgno(here.bi_pdata);
	c_comma(c_nbb);
	merge_str(hline, "%LL, %W, %W, %W, %W, %W",
	          here.bi_pdata, here.bi_wb, here.bi_hl, here.bi_x, here.bi_y,
	          here.bi_color);
	c_wrlin();
	c_nbb++;
	}

WORD wr_bitblk(LPTREE tree, WORD which)
	{
	LPBIT	bitblk;

	if ((tree[which].ob_type & 0xFF) != G_IMAGE) return 1;

	bitblk = (LPBIT)tree[which].ob_spec;
	
	if ( bitblk == (LPBIT)-1) return 1;

	tree[which].ob_spec = (LPVOID)((LONG) rcs_wraddr & 0xffff);
	lfwrite(bitblk, 1, sizeof(BITBLK), rcs_frhndl);
	rcs_wraddr += sizeof(BITBLK);
	++head->rsh_nbb;
	c_bitblk(bitblk);
	return 1;
	}


	
VOID map_frbit(VOID)
	{
	WORD	ifree, nfree;
	LPBIT	bitblk;

	if ( !(nfree = head->rsh_nimages)) return;
	for (ifree = 0; ifree < nfree; ifree++)
		{
		if ( (bitblk = img_addr(ifree)) == (LPBIT)-1)
			break;
		*img_ptr(ifree) = (LPVOID)( (LONG) rcs_wraddr & 0xffff);
		lfwrite(bitblk, 1, sizeof(BITBLK), rcs_frhndl);
		rcs_wraddr += sizeof(BITBLK);
		++head->rsh_nbb;
		c_bitblk(bitblk);
		}
	} 


	
VOID c_frbhead(VOID)
	{
	if (!rcs_cflag)return;
	strcpy(hline, "\r\nLONG rs_frimg[] = {\r\n");
	c_wrlin();
	c_nfrbit = 0;
	}


	
VOID c_frb(LPBIT addr)
	{
	WORD	blkno;

	if (!rcs_cflag) return;
	blkno = (WORD)(addr - head->rsh_bitblk) / (WORD)sizeof(BITBLK);
	c_comma(c_nfrbit);
	merge_str(hline, "%WL", blkno);
	c_wrlin();
	c_nfrbit++;
	}


	
VOID wr_frbit()
	{
	WORD	ifree, nfree;

	if ( !(nfree = head->rsh_nimages)) return;
	for (ifree = 0; ifree < nfree; ifree++) c_frb(img_addr(ifree));
	nfree *= sizeof(LONG);
	lfwrite(img_ptr(0), 1, nfree, rcs_frhndl);
	head->rsh_frimg = rcs_wraddr;
	rcs_wraddr += nfree;
	} 

	
VOID c_tedhead()
	{
	if (!rcs_cflag) return;
	strcpy(hline, "\r\nTEDINFO rs_tedinfo[] = {\r\n");
	c_wrlin();
	c_nted = 0;
	}

	
VOID c_tedinfo(LPTEDI addr)
	{
	TEDINFO	here;

	if (!rcs_cflag) return;
	LBCOPY(ADDR(&here), (LPVOID)addr, sizeof(TEDINFO));
	here.te_ptext  = (LPBYTE)((LONG) c_strno(here.te_ptext));
	here.te_pvalid = (LPBYTE)((LONG) c_strno(here.te_pvalid));
	here.te_ptmplt = (LPBYTE)((LONG) c_strno(here.te_ptmplt));
	c_comma(c_nted);
	merge_str(hline, "%LL, %LL, %LL, %W, %W, %W, 0x%H, 0x%H, %W, %W,%W", 
		here.te_ptext,  here.te_ptmplt, here.te_pvalid,
		here.te_font,   here.te_junk1,  here.te_just,
		here.te_color,  here.te_junk2,  here.te_thickness,
		here.te_txtlen, here.te_tmplen);
	c_wrlin();
	c_nted++;
	}

	
WORD wr_tedinfo(LPTREE tree, WORD which)
	{
	LPTEDI	tedinfo;
	WORD	type;

	type = tree[which].ob_type & 0xFF;
	if ( !(type == G_TEXT  || type == G_BOXTEXT || 
		   type == G_FTEXT || type == G_FBOXTEXT)) return;
	if ( (tedinfo = (LPTEDI)tree[which].ob_spec) == (LPTEDI)-1) return;

	tree[which].ob_spec = (LPVOID)((LONG) rcs_wraddr & 0xffff);

	lfwrite(tedinfo, 1, sizeof(TEDINFO), rcs_frhndl);
	rcs_wraddr += sizeof(TEDINFO);
	++head->rsh_nted;
	c_tedinfo(tedinfo);
	return 1;
	}

VOID c_objhead()
	{
	if (!rcs_cflag) return;
	strcpy(hline, "\r\nOBJECT rs_object[] = {\r\n");
	c_wrlin();
	c_nobs = 0;
	}


	
VOID c_object(LPTREE tree)
	{
	OUTOBJ	here;
	BYTE	type[10], state[10], flags[10];

	if (!rcs_cflag) return;
	LBCOPY(ADDR(&here.next), ADDR(&tree->ob_next), 3 * sizeof(WORD) );
	LBCOPY(ADDR(&here.spec), ADDR(&tree->ob_spec), 4 * sizeof(UWORD) + sizeof(LONG) );

	switch (tree->ob_type & 0xFF)
	{
		case G_STRING:
		case G_BUTTON:
		case G_TITLE:
			here.spec = (LONG)c_strno((LPVOID)here.spec);
			break;
		case G_TEXT:
		case G_FTEXT:
		case G_BOXTEXT:
		case G_FBOXTEXT:
			here.spec = ( (WORD) here.spec - head->rsh_tedinfo )
				/ (WORD)sizeof(TEDINFO);
			break;
		case G_IMAGE:
			here.spec = ( (WORD) here.spec - head->rsh_bitblk )
				/ (WORD)sizeof(BITBLK);
			break;
		case G_ICON:
			here.spec = ( (WORD) here.spec - head->rsh_iconblk )
				/ (WORD)sizeof(ICONBLK);
			break;
		default:
			break;
		}

	c_comma(c_nobs);
	here.type  = c_lookup(tree->ob_type & 0xFF, c_types,  N_TYPES,  type);
	here.flags = c_lookup(tree->ob_flags,       c_flags,  N_FLAGS,  flags);
	here.state = c_lookup(tree->ob_state,       c_states, N_STATES, state);
	merge_str(hline, "%W, %W, %W, %S, %S, %S, 0x%XL, %W,%W, %W,%W", 
			  here.next, here.head,  here.tail,
			  here.type, here.flags, here.state,
			  here.spec, here.x, here.y, here.w, here.h);
	c_wrlin();
	c_nobs++;
	}


	
VOID wr_obj(WORD itree, LPTREE tree, WORD iobj)
	{
	WORD	entno, where, link, obtype;
	LPTREE	object;

	where = wr_obndx[iobj];		/* the actual object #	*/

	entno = find_obj(tree, where);
	if (entno != NIL)
		set_value(entno, (BYTE *) ( 
			((itree & 0xFF) << 8) | (iobj & 0xFF) ) );
	if ( (link = tree[where].ob_next) != -1) tree[where].ob_next = fnd_ob(link);
	if ( (link = tree[where].ob_head) != -1) tree[where].ob_head = fnd_ob(link);
	if ( (link = tree[where].ob_tail) != -1) tree[where].ob_tail = fnd_ob(link);
	obtype = tree[where].ob_type & 0xFF;

	/* Zero the high byte of the object type for icons & images (why?> */
	if(obtype == G_ICON || obtype == G_IMAGE) /*0 hi byte*/
		tree[where].ob_type = obtype;

	unfix_chpos(&tree[where].ob_x,     0);
	unfix_chpos(&tree[where].ob_width, 0);
	unfix_chpos(&tree[where].ob_y,     1);
	unfix_chpos(&tree[where].ob_height,1);

	lfwrite(object = &tree[where], 1, sizeof(OBJECT), rcs_frhndl);
	rcs_wraddr += sizeof(OBJECT);
	c_object(object);
	}


	
VOID wr_trees()
	{
	LPTREE	tree;
	WORD	ntree, itree, iobj, where;

	ntree = head->rsh_ntree;

	for (itree = 0; itree < ntree; itree++)
		{
		where = find_tree(itree);
		set_value(where, (BYTE *) itree);
		tree = tree_addr(itree);
		LLSET(tree_ptr(itree), (LONG) rcs_wraddr & 0xffff);
		wr_obnum = 0;
		map_tree(tree, 0, -1, indx_obs);
		head->rsh_nobs += wr_obnum;
		iobj = wr_obndx[wr_obnum - 1];
		tree[iobj].ob_flags |= LASTOB;
		for (iobj = 0; iobj < wr_obnum; iobj++)
			wr_obj(itree, tree, iobj);
		}
	}

VOID c_treehead()
	{
	if (!rcs_cflag) return;
	strcpy(hline, "\r\nLONG rs_trindex[] = {\r\n");
	c_wrlin();
	c_ntree = 0; 
	}

	
VOID c_tree(LPTREE addr)
	{
	WORD	objno;

	if (!rcs_cflag)
		return;
	objno = ( (WORD) addr - head->rsh_object) / (WORD)sizeof(OBJECT);
	c_comma(c_ntree);
	merge_str(hline, "%WL", objno);
	c_wrlin();
	c_ntree++;
	}


	
VOID wr_trindex(VOID)
	{
	WORD	ntree, itree;

	ntree = head->rsh_ntree;

	lfwrite(tree_ptr(0), sizeof(LONG), ntree, rcs_frhndl);

	for (itree = 0; itree < ntree; itree++)
		c_tree(tree_addr(itree));

	head->rsh_trindex = rcs_wraddr;	/* Don't move this line! */
	rcs_wraddr += ntree * sizeof(LONG);
	}

VOID c_defs()
	{
	BYTE	*rptr, *sptr;

	if (!rcs_cflag) return;
	strcpy(hline, "\r\n");
	c_wrlin();
	c_defline("NUM_STRINGS", c_nstring);
	c_defline("NUM_FRSTR",   head->rsh_nstring);
	c_defline("NUM_IMAGES",  c_nimage);
	c_defline("NUM_BB",      head->rsh_nbb);
	c_defline("NUM_FRIMG",   head->rsh_nimages);
	c_defline("NUM_IB",      head->rsh_nib);
	c_defline("NUM_TI",      head->rsh_nted);
	c_defline("NUM_OBS",     head->rsh_nobs);
	c_defline("NUM_TREE",    head->rsh_ntree);
	for (sptr = rptr = &rcs_rfile[0]; *sptr; sptr++)
		if (*sptr == '\\' || *sptr == '\:')
			rptr = sptr + 1;
	merge_str(hline, "\r\nBYTE pname[] = \"%S\";", rptr);
	c_wrlin();
	hline[0] = '\032';
	hline[1] = '\0';
	c_wrlin();
	}

VOID ctrl_z(FILE *fp)
	{
	fputc('\032', fp);
	}
	   

VOID wr_include(WORD deftype, WORD ndx, WORD ndef, BYTE *ptns[], WORD trflag)
	{
	WORD	idx, ii;
	HOUT	h;

	for ( idx = 0; idx < ndx; idx++ )
		for ( ii = 0; ii < rcs_ndxno; ii++)
			if((trflag && get_kind(ii) < 4)	|| ( get_kind(ii) == deftype ))
				if ( (WORD) get_value(ii) == idx)
					{
					strup(h.p0 = get_name(ii));
					h.p1 = idx;
					merge_str(hline, ptns[ndef], h.p0, h.p1, h.p2);
					lfwrite(hline, 1, strlen(hline), rcs_fhhndl);
					break;
					}     
	}
      

VOID obj_include( WORD ntree, WORD nobs, BYTE *ptns[] )
	{
	WORD	itree,  iobs, ii;
	HOUT	h;
	
	for ( itree = 0; itree < ntree; itree++ )
            for ( iobs = 0; iobs < nobs; iobs++)
	    	for ( ii = 0; ii < rcs_ndxno; ii++ )
	    	    if( get_kind(ii) == OBJKIND)
			{
			h.p1 = (WORD) get_value(ii);
			h.p2 = (WORD) LHIBT(h.p1) & 0xff;
			h.p1 = (WORD) LLOBT(h.p1) & 0xff;
			if( h.p2 == itree && h.p1 == iobs)
			       {
			       strup(h.p0 = get_name(ii));
			       merge_str(hline,ptns[2], h.p0, h.p1, h.p2);
				   lfwrite(hline, 1, strlen(hline), rcs_fhhndl);
			       break;
			       } 		
			}
	}


VOID wrsrt_inclfile(BYTE *ext, BYTE *ptns[])
	{
	r_to_xfile(rcs_hfile, ext);

	FOREVER
		{
		rcs_fhhndl = fopen(rcs_hfile, "w");
		if (rcs_fhhndl) break;
		if (!ppd_form_error(errno)) return;
		}
	wr_include ( UNKN, head->rsh_ntree,   0,              ptns,TRUE );
	obj_include(       head->rsh_ntree,   head->rsh_nobs, ptns );
	wr_include (ALRT,  head->rsh_nstring, 1,              ptns, FALSE);
	wr_include (FRSTR, head->rsh_nstring, 3,              ptns, FALSE);
	wr_include (FRIMG, head->rsh_nimages, 4,              ptns, FALSE);
	ctrl_z(rcs_fhhndl);
	fclose(rcs_fhhndl);
	}


	
VOID write_inclfile(BYTE *ext, BYTE *ptns[])
	{
	WORD	ii;
	HOUT	h;

	r_to_xfile(rcs_hfile, ext);

	FOREVER
		{
		rcs_fhhndl = fopen(rcs_hfile, "w");
		if (rcs_fhhndl)	break;
		if (!ppd_form_error(errno)) return;
		}

	for (ii = 0; ii < rcs_ndxno; ii++)
		{
		strup(h.p0 = get_name(ii));
		h.p1 = (WORD) get_value(ii);
		switch ( get_kind(ii) ) {
			case UNKN:
			case PANL:
			case DIAL:
			case MENU:
				merge_str(hline, ptns[0], h.p0, h.p1, h.p2);
				break;
			case ALRT:
				merge_str(hline, ptns[1], h.p0, h.p1, h.p2);
				break;
			case OBJKIND:
				h.p2 = (WORD) LHIBT(h.p1) & 0xff;
				h.p1 = (WORD) LLOBT(h.p1) & 0xff;
				merge_str(hline, ptns[2], h.p0, h.p1, h.p2);
				break;
			case FRSTR:
				merge_str(hline, ptns[3], h.p0, h.p1, h.p2);
				break;
			case FRIMG:
				merge_str(hline, ptns[4], h.p0, h.p1, h.p2);
				break;
			default:
				break;
			}
		lfwrite(hline, 1, strlen(hline), rcs_fhhndl);
		}
	ctrl_z(rcs_fhhndl);
	fclose(rcs_fhhndl);
	}

WORD write_files()
	{
	LONG	mem_needs;
	
	do_tally();			 
	if (rcs_cflag)
		mem_needs = 3 * rcs_tally.nbytes;
	else
		mem_needs = (rcs_tally.nbytes >> 1) + rcs_tally.nbytes;

	if( mem_needs > get_dspace( rcs_rfile[0] - 'A' + 1 ) )
           {
	   hndl_alert(1, string_addr(SNDSPACE));
	   return(FALSE);
	   }		
	
	FOREVER
		{	   
/*		rcs_rhndl = dos_open(ADDR(&rcs_rfile[0]), 1);
		if (DOS_ERR)
*/		rcs_frhndl = fopen(rcs_rfile, "wb");
		if (rcs_frhndl) break;
		if (!ppd_form_error(errno)) return (FALSE);
		}

	if (rcs_cflag)
	{
		r_to_xfile(rcs_hfile, "RSH");
		FOREVER
		{
	       	rcs_fhhndl = fopen(rcs_hfile, "w");
			if (rcs_fhhndl)	break;
			if (!ppd_form_error(errno))
			{
				fclose(rcs_frhndl);
				return (FALSE);
			}
		}
	}		       
	dcomp_free();
	dcomp_alerts();

	ini_prog();
	show_prog(STHDR);
	wr_header();
	rcs_wraddr = sizeof(RSHDR);
	head->rsh_string = rcs_wraddr;
	c_bases();

	show_prog(STSTR);
	c_strhead();
	map_all(wr_string);
	map_frstr();		/* handles strings not ref'ed in objects */
	c_tail(c_nstring);
        wr_sync();

	show_prog(STIMG);
	head->rsh_imdata = rcs_wraddr;
	c_nimage = 0;
	map_all(wr_image);
	c_frimg = c_nimage;
	map_frimg();
	  

	show_prog(STFRSTR);
	c_frshead();
	wr_freestr();
	c_tail(c_nfrstr);

	show_prog(STBB);
	head->rsh_bitblk = rcs_wraddr;
	head->rsh_nbb    = 0;
	c_bithead();
	map_all(wr_bitblk);
	map_frbit();	
	c_tail(c_nbb);

	show_prog(STFRIMG);
	c_frbhead();
	wr_frbit();
	c_tail(c_nfrbit);

	show_prog(STIB);
	head->rsh_iconblk = rcs_wraddr;
	head->rsh_nib = 0;
	c_iconhead();
	map_all(wr_iconblk);
	c_tail(c_nib);
        

	show_prog(STTI);
	head->rsh_tedinfo = rcs_wraddr;
	head->rsh_nted = 0;
	c_tedhead();
	map_all(wr_tedinfo);
	c_tail(c_nted);

	show_prog(STOBJ);
	head->rsh_object = rcs_wraddr;
	head->rsh_nobs   = 0;
	c_objhead();
	wr_trees();
	c_tail(c_nobs);

	show_prog(STTRIND);
	c_treehead();
	wr_trindex();		/* also handles RSH_TRINDEX(head) */
	c_tail(c_ntree);

	show_prog(STHDR);
	head->rsh_rssize = rcs_wraddr;
	wr_header();		/* rewrite updated header	  */
	fclose(rcs_frhndl);
	c_foobar();
	c_defs();
	if (rcs_cflag)
		{
		ctrl_z(rcs_fhhndl);
		fclose(rcs_fhhndl);
		}

	FOREVER
		{
		rcs_fdhndl = fopen(rcs_dfile, "wb");
		if (rcs_fdhndl) break;
		if (!ppd_form_error(errno))	return (FALSE);
		}

	show_prog(STNAME);
#if	MC68K
   	wr_68kdef();
#else
	wr_i8086def();
	/*
   	lfwrite(&rcs_ndxno, 2, 1, rcs_fdhndl);
   	lfwrite(rcs_index,  rcs_ndxno * sizeof(INDEX), 1, rcs_fdhndl);
	*/
#endif
   	fclose(rcs_fdhndl);

	if (rcs_hflag)
		if ( rcs_fsrtflag ) wrsrt_inclfile("H", hptns);
		else 		        write_inclfile("H", hptns);
	if (rcs_oflag)
		if (rcs_fsrtflag )  wrsrt_inclfile("I", optns);
		else		        write_inclfile("I", optns);
	if (rcs_cbflag)
		if ( rcs_fsrtflag )	wrsrt_inclfile("B", bptns);
		else		        write_inclfile("B", bptns);
	if (rcs_f77flag)
		if ( rcs_fsrtflag ) wrsrt_inclfile("F", fptns);
		else		        write_inclfile("F", fptns);

	return (TRUE);
	}

