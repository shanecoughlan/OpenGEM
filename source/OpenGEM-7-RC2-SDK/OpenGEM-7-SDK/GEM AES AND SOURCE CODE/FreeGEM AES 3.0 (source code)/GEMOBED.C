/*	GEMOBED.C	05/29/84 - 06/20/85		Gregg Morris	*/
/*	merge High C vers. w. 2.2 		8/21/87		mdf	*/ 

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

#define BACKSPACE 0x0E08			/* backspace		*/
#define SPACE 0x3920				/* ASCII <space>	*/
#define UP 0x4800				/* up arrow		*/
#define DOWN 0x5000				/* down arrow		*/
#define LEFT 0x4B00				/* left arrow		*/
#define RIGHT 0x4D00				/* right arrow		*/
#define DELETE 0x5300				/* keypad delete	*/
#define TAB 0x0F09				/* tab			*/
#define BACKTAB 0x0F00				/* backtab		*/
#define RETURN 0x1C0D				/* carriage return	*/
#define ESCAPE 0x011B				/* escape		*/
						/* in GSXIF or APGSXIF	*/
EXTERN VOID	gsx_gclip();
EXTERN VOID	gsx_sclip();

EXTERN BYTE	*rs_str();
EXTERN WORD	min();


/* --------------- added for metaware compiler --------------- */
EXTERN VOID     r_set();			/* in OPTIMOPT.A86	*/
EXTERN VOID     bfill();
EXTERN WORD     min();
EXTERN WORD     max();
EXTERN BYTE     toupper();
EXTERN WORD     strlen();
EXTERN VOID     ob_actxywh();			/* in OBLIB.C		*/
EXTERN VOID     ob_draw();
EXTERN VOID     ob_format(WORD, BYTE *, BYTE *, BYTE *);
EXTERN WORD     gr_just();			/* in GRAF.C		*/
EXTERN VOID     gsx_attr();
EXTERN VOID     gsx_cline();
EXTERN VOID     ins_char();			/* in OPTIMIZE.C	*/
/* ----------------------------------------------------------- */

EXTERN WORD	gl_wchar;
EXTERN WORD	gl_hchar;

EXTERN WORD	gl_wbox;
EXTERN WORD	gl_hbox;

EXTERN WORD	gl_width;
EXTERN WORD	gl_height;
EXTERN TEDINFO	edblk;

EXTERN THEGLO	D;


	VOID
ob_getsp(tree, obj, pted)
	REG LPTREE	tree;
	REG WORD	obj;
	TEDINFO		*pted;
{
	WORD		flags;
	REG LONG	spec;

	flags = tree[obj].ob_flags;
	spec = tree[obj].ob_spec;
	if (flags & INDIRECT)
	  spec = *((LPLONG)(spec));
	LBCOPY(ADDR(pted), (LPTEDI)spec, sizeof(TEDINFO));
}

	VOID
ob_center(tree, pt)
	LPTREE		tree;
	GRECT		*pt;
{
	REG WORD	xd, yd, wd, hd;
	WORD		height;

	wd = tree[ROOT].ob_width;
	hd = tree[ROOT].ob_height;
	xd = ((gl_width - wd) / 2) / gl_wchar * gl_wchar;
					/* don't center on xtra long screens */
	height = min(gl_height, 25 * gl_hchar);
	yd = gl_hbox + ((height - gl_hbox - hd) / 2);
	tree[ROOT].ob_x = xd;	  
	tree[ROOT].ob_y = yd;
						/* account for outline	*/
						/*   or shadow		*/
	if ( tree[ROOT].ob_state & (OUTLINED | SHADOWED) )
	{
	  xd -= 8;
	  yd -= 8;
 	  wd += 16;
	  hd += 16;
	}
	r_set(pt, xd, yd, wd, hd);
}


/*
*	Routine to scan thru a string looking for the occurrence of
*	the specified character.  IDX is updated as we go based on
*	the '_' characters that are encountered.  The reason for
*	this routine is so that if a user types a template character
*	during field entry the cursor will jump to the first 
*	raw string underscore after that character.
*/
	WORD
scan_to_end(pstr, idx, chr)
	REG BYTE	*pstr;
	REG WORD	idx;
	BYTE		chr;
{
	while( (*pstr) &&
	       (*pstr != chr) )
	{
	  if (*pstr++ == '_')
	    idx++;
	}
	return(idx);
}

/*
*	Routine that returns a format/template string relative number
*	for the position that was input (in raw string relative numbers).
*	The returned position will always be right before an '_'.
*/
	WORD
find_pos(str, pos)
	REG BYTE	*str;
	REG WORD	pos;
{
	REG WORD	i;


	for (i=0; pos > 0; i++) 
	{
	  if (str[i] == '_')
	    pos--;
	}
						/* skip to first one	*/
	while( (str[i]) &&
	       (str[i] != '_') )
	  i++;
	return(i);
}


	VOID
pxl_rect(tree, obj, ch_pos, pt)
	REG LPTREE	tree;
	REG WORD	obj;
	WORD		ch_pos;
	REG GRECT	*pt;
{
	GRECT		o;
	WORD		numchs;

	ob_actxywh(tree, obj, &o);
	numchs = gr_just(edblk.te_just, edblk.te_font, edblk.te_ptmplt, 
				o.g_w, o.g_h, &o);
	pt->g_x = o.g_x + (ch_pos * gl_wchar);
	pt->g_y = o.g_y;
	pt->g_w = gl_wchar;
	pt->g_h = gl_hchar;
}


/*
*	Routine to redraw the cursor or the field being editted.
*/
	VOID
curfld(tree, obj, new_pos, dist)
	LPTREE		tree;
	WORD		obj, new_pos;
	WORD		dist;
{
	GRECT		oc, t;

        pxl_rect(tree, obj, new_pos, &t);
	if (dist)
	  t.g_w += (dist - 1) * gl_wchar;
	else
	{
	  gsx_attr(FALSE, MD_XOR, BLACK);
	  t.g_y -= 3;
	  t.g_h += 6;
	}
						/* set the new clip rect*/
	gsx_gclip(&oc);
	gsx_sclip(&t);
						/* redraw the field	*/
	if (dist)
	  ob_draw(tree, obj, 0);
	else
	{
	  gsx_cline(t.g_x, t.g_y, t.g_x, t.g_y+t.g_h-1);
	  gsx_cline(t.g_x+1, t.g_y, t.g_x+1, t.g_y+t.g_h-1);
	}
						/* turn on cursor in	*/
						/*   new position	*/
        gsx_sclip(&oc);
}


/*
*	Routine to check to see if given character is in the desired 
*	range.  The character ranges are
*	stored as enumerated characters (xyz) or ranges (x..z)
*/
	WORD
instr(chr, str)
	REG BYTE	chr;
	REG BYTE	*str;
{
	REG BYTE	test1, test2;

	while(*str)
	{
	  test1 = test2 = *str++;
	  if ( (*str == '.') &&
	       (*(str+1) == '.') )
	  {
	    str += 2;
	    test2 = *str++;
	  }
	  if ( (chr >= test1) &&
	       (chr <= test2) )
	    return(TRUE);
	}
	return(FALSE);
}


/*
*	Routine to verify that the character matches the validation
*	string.  If necessary, upshift it.
*/
	WORD
check(in_char, valchar)
	REG BYTE	*in_char;
	BYTE		valchar;
{
	REG WORD	upcase;
	REG WORD	rstr;

	upcase = TRUE;
	rstr = -1;
	switch(valchar)
	{
	  case '9':				/* 0..9			*/
	    rstr = ST9VAL;
	    upcase = FALSE;
	    break;
	  case 'A':				/* A..Z, <space>	*/
	    rstr = STAVAL;
	    break;
	  case 'N':				/* 0..9, A..Z, <SPACE>	*/
	    rstr = STNVAL;
	    break;
	  case 'P':	    /* DOS pathname + '\', '?', '*', ':','.',','*/
	    rstr = STPVAL;
	    break;
	  case 'p':			/* DOS pathname + '\` + ':'	*/
	    rstr = STLPVAL;
	    break;
	  case 'F':		/* DOS filename + ':', '?' + '*'	*/
	    rstr = STFVAL;
	    break;
	  case 'f':				/* DOS filename */
	    rstr = STLFAVAL;
	    break;
	  case 'a':				/* a..z, A..Z, <SPACE>	*/
	    rstr = STLAVAL;
	    upcase = FALSE;
	    break;
	  case 'n':				/* 0..9, a..z, A..Z,<SP>*/
	    rstr = STLNVAL;
	    upcase = FALSE;
	    break;
	  case 'x':				/* anything, but upcase	*/
	    *in_char = toupper(*in_char);
	    return(TRUE);
	  case 'X':				/* anything		*/
	    return(TRUE);
	}
	if (rstr != -1)
	{
	  if ( instr(*in_char, rs_str(rstr)) )
	  {
	     if (upcase)
	       *in_char = toupper(*in_char);
	     return(TRUE);
	  }
	}

	return(FALSE);
}


/*
*	Find STart and FiNish of a raw string relative to the template
*	string.  The start is determined by the InDeX position given.
*/
	VOID
ob_stfn(idx, pstart, pfinish)
	WORD		idx;
	WORD		*pstart, *pfinish;
{
	*pstart = find_pos(&D.g_tmpstr[0], idx);
	*pfinish = find_pos(&D.g_tmpstr[0], strlen(&D.g_rawstr[0]) );
}


	WORD
ob_delit(idx)
	WORD		idx;
{
	if (D.g_rawstr[idx])
	{
	  strcpy(&D.g_rawstr[idx+1], &D.g_rawstr[idx]);
	  return(FALSE);
	}
	return(TRUE);
}

/*----------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
WORD ob_edit( REG LPTREE tree, REG WORD obj, WORD in_char, 
						    REG WORD * idx, WORD kind)
{
	WORD		pos, len;
	WORD		ii, no_redraw, start, finish, nstart, nfinish;
	REG WORD	dist, tmp_back, cur_pos;
	BYTE		bin_char;
#if DBCS
static  WORD		ctype = CT_ADE;
#endif /* DBCS */

	if ( (kind == EDSTART) ||
	     (obj <= 0) )
	  return(TRUE);
						/* copy TEDINFO struct	*/
						/*   to local struct	*/
	ob_getsp(tree, obj, &edblk);
						/* copy passed in strs	*/
						/*   to local strs	*/
	LSTCPY((BYTE FAR *)ad_tmpstr, (BYTE FAR *)edblk.te_ptmplt);
	LSTCPY((BYTE FAR *)ad_rawstr, (BYTE FAR *)edblk.te_ptext);
	len = ii = LSTRLEN( (BYTE FAR *)edblk.te_pvalid );
	LSTCPY((BYTE FAR *)ad_valstr, (BYTE FAR *)edblk.te_pvalid);
						/* expand out valid str	*/
	while ( (ii > 0) &&
		(len < edblk.te_tmplen) )
	  D.g_valstr[len++] = D.g_valstr[ii-1];
	D.g_valstr[len] = NULL;
						/* init formatted	*/
						/*   string		*/
	ob_format(edblk.te_just, &D.g_rawstr[0], &D.g_tmpstr[0], 
			&D.g_fmtstr[0]);
	switch(kind)
	{
	  case EDINIT:
		*idx = strlen(&D.g_rawstr[0]);
#if DBCS
		ctype = init_type(*idx);
#endif /* DBCS */
		break;
	  case EDCHAR:
		/* at this point, D.g_fmtstr has already been formatted--*/
		/*   it has both template & data. now update D.g_fmtstr	*/
		/*   with in_char; return it; strip out junk & update	*/
		/*   ptext string.					*/
		no_redraw = TRUE;
						/* find cursor & turn	*/
						/*   it off		*/
		ob_stfn(*idx, &start, &finish);
						/* turn cursor off	*/
		cur_pos = start;
		curfld(tree, obj, cur_pos, 0);
#if DBCS
		ctype = chkctype((UBYTE)in_char, ctype);
#endif /* DBCS */

		switch(in_char)
		{
		  case BACKSPACE:
			if (*idx > 0)
			{
			  *idx -= 1;
#if DBCS
	   		  if (D.g_typstr[*idx] == CT_DBC2)
			     *idx -= 1;
			  del_dummy(*idx);
#endif /* DBCS */
			  no_redraw = ob_delit(*idx);
#if DBCS
			  cur_pos = find_pos(&D.g_tmpstr[0], *idx);
			  ins_dummy(*idx, cur_pos, edblk.te_txtlen);
#endif /* DBCS */
			}
			break;
		  case CTL_BACKSPACE:
			*idx = 0;
			D.g_rawstr[0] = NULL;
			no_redraw = FALSE;
#if DBCS
			ctype = init_type(*idx);
#endif /* DBCS */
			break;
		  case DELETE:
			if (*idx <= (edblk.te_txtlen - 2))
#if DBCS
			{
			  del_dummy(*idx);
#endif /* DBCS */
			  no_redraw = ob_delit(*idx);
#if DBCS
			  ins_dummy(*idx, cur_pos, edblk.te_txtlen);
			}
#endif /* DBCS */
			break;
		  case LEFT:
			if (*idx > 0)
		  	  *idx -= 1;
#if DBCS
			if (D.g_typstr[*idx] == CT_DBC2)
			  *idx -= 1;
#endif /* DBCS */
			break;
		  case RIGHT:
			if ( *idx < strlen(&D.g_rawstr[0]) )
			  *idx += 1;
#if DBCS
			if (D.g_typstr[*idx] == CT_DBC2)
			  *idx += 1;
#endif /* DBCS */
			break;
		  case TAB:
		  case BACKTAB:
		  case RETURN:
		  	break;
		  default:
#if DBCS
			if (ctype == CT_DBC2 ) {
			    if ( D.g_typstr[*idx] == CT_DBC1) {
				*idx += 1;
				cur_pos++;
			    }
			    else
				break ;
			}
			tmp_back = 0;
			if ( *idx > 
		  	     (edblk.te_txtlen - ((ctype == CT_DBC1) ? 3 : 2)) )
			{
			  tmp_back++;
			  if ( (ctype == CT_DBC1) && 
			       (*idx > (edblk.te_txtlen-2)) )
			    tmp_back++;
			  if (D.g_typstr[*idx-tmp_back] == CT_DBC2)
			    tmp_back++;
			  if (*idx < tmp_back)
			      break ;
			  cur_pos -= tmp_back;
			  start = cur_pos;
			  *idx -= tmp_back;
			}
#else /* DBCS */
			tmp_back = FALSE;
			if (*idx > (edblk.te_txtlen - 2))
			{
			  cur_pos--;
			  start = cur_pos;
			  tmp_back = TRUE;
			  *idx -= 1;
			}
#endif /* DBCS */
			bin_char = in_char & 0x00ff;
			if (bin_char)
			{
						/* make sure char is	*/
						/*   in specified set	*/
#if DBCS
			  if ( check(&bin_char, D.g_valstr[*idx], ctype) )
			  {
			    if (ctype != CT_DBC2)
			        del_dummy(*idx);
			    ob_insrt(*idx, bin_char, ctype, edblk.te_txtlen);
			    if (ctype == CT_DBC1)
			        break;
			    ins_dummy(*idx, cur_pos, edblk.te_txtlen);
			    while (D.g_typstr[*idx] == DMY_SPC)
	    			*idx += 1;
			    *idx += 1;
			    if (D.g_typstr[*idx] == CT_DBC2)
			        *idx += 1;
#else /* DBCS */
			  if ( check(&bin_char, D.g_valstr[*idx]) )
			  {
			    ins_char(&D.g_rawstr[0], *idx, bin_char, 
					edblk.te_txtlen);
			    *idx += 1;
#endif /* DBCS */
			    no_redraw = FALSE;
			  }
		  	  else
			  {		/* see if we can skip ahead	*/
			    if (tmp_back)
			    {
#if DBCS
		  	      *idx += tmp_back;
			      cur_pos += tmp_back;
#else /* DBCS */
		  	      *idx += 1;
			      cur_pos++;
#endif /* DBCS */			      
			    }
#if DBCS
			    if (ctype != CT_ADE)
			      break;
#endif /* DBCS */			    
		 	    pos = scan_to_end(&D.g_tmpstr[0]+cur_pos, *idx,
								 bin_char);

		 	    if (pos < (edblk.te_txtlen - 2) )
			    {
			      bfill( pos - *idx, (SPACE & 0xFF), &D.g_rawstr[*idx]);
			      D.g_rawstr[pos] = NULL;
#if DBCS
			      bfill( pos - *idx, CT_ADE, &D.g_typstr[*idx]);
			      D.g_typstr[pos] = TYP_END;
#endif /* DBCS */
			      *idx = pos;
		 	      no_redraw = FALSE;
			    }
			  }
			}
			break;
		}

		LSTCPY((BYTE FAR *)edblk.te_ptext, (BYTE FAR *)ad_rawstr);

		if (!no_redraw)
		{
		  ob_format(edblk.te_just, &D.g_rawstr[0], &D.g_tmpstr[0],
				 &D.g_fmtstr[0]);
		  ob_stfn(*idx, &nstart, &nfinish);
	 	  start = min(start, nstart);
		  dist = max(finish, nfinish) - start;
		  if (dist)
		    curfld(tree, obj, start, dist);
		}
	        break;
	  case EDEND:
#ifdef DB_SPACE
		if (chk_dbsp())
		    LSTCPY(edblk.te_ptext, ad_rawstr);
#endif /* DB_SPACE */
		break;
	}
						/* draw/erase the cursor*/
	cur_pos = find_pos(&D.g_tmpstr[0], *idx);
	curfld(tree, obj, cur_pos, 0);
	return(TRUE);
}


