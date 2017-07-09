/*	RCSALERT.C	 1/17/85 - 1/17/85	Tim Oren		*/
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
#include "ppdrcs.h"

 

VOID pt_parse(LPTREE tree, WORD obj0, WORD nobj, LPBYTE where, WORD *here)
	{
	WORD	iobj;
	BYTE	tmp;

	tmp = 0;
	for (iobj = 0; iobj < nobj; iobj++) tree[obj0 + iobj].ob_spec = ((LPVOID)-1);
	
	for (iobj = 0; tmp != ']' && iobj < nobj; iobj++)
		{
		tree[obj0 + iobj].ob_spec = &where[*here];
		tmp = where[*here];
		while (tmp != ']' && tmp != '|')
			{
			(*here)++;
			tmp = where[*here];
			}

		where[*here] = 0;
		(*here)++;
		}
	}

	
VOID parse_alert(LPTREE tree, LPBYTE where)
	{
	LPTREE	stree;
	LPVOID	obspec;
	WORD	icon, curr_id;

	icon = where[1] - '0';
	if (icon < 1 || icon > 3) obspec = (LPVOID)-1;
	else
		{
		ini_tree(&stree, ALRTPBX);
		obspec = stree[APBXIMG0 + icon - 1].ob_spec;
		}
	tree[ALRTIMG].ob_spec = obspec;
	curr_id = 4;
	pt_parse(tree, ALRTSTR0, 5, where, &curr_id);
	curr_id++;
	pt_parse(tree, ALRTBTN0, 3, where, &curr_id);
	}

WORD is_alert(LPBYTE where)
	{
	return (where[0] == '[' && where[2] == ']');
	}


	
VOID comp_alerts(LPRSHDR hdr)
	{
	LPLTREE	trindex;
	WORD	nalert, nfrstr, istr, ndex;
	BYTE	name[9];
	LPTREE	tree, maddr;
	LPBYTE	where;
	LPLSTR	frstr;
	
	ini_tree(&maddr, NEWALRT);
	frstr = (LPLSTR)(((LPBYTE)hdr) + LW( hdr->rsh_frstr) );
	nfrstr = hdr->rsh_nstring;
	for (nalert = istr = 0; istr < nfrstr; istr++)
		{
		where = frstr[istr];
		if (is_alert(where)) nalert++;
		}
	if (!nalert) return;
	trindex = mak_trindex(nalert);

	for (istr = 0; istr < hdr->rsh_nstring; istr++)
		{
		where = frstr[istr];
		if (is_alert(where))
			{
			tree = copy_tree(maddr, ROOT, TRUE);
			parse_alert(tree, where);
			frstr[istr] = (LPBYTE)-1;
			fix_alert(tree);
			*(trindex++) = tree;
			if ((ndex = find_value(where)) != NIL)
				set_value(ndex, (LPBYTE)tree);
			else
				{
				unique_name(&name[0], "ALERT%W", istr);
				ndex = new_index((LPBYTE) tree, ALRT, name);
				}
			}
		}
	if (nalert == hdr->rsh_nstring) hdr->rsh_nstring = 0;
	} 


	
VOID pt_unparse(LPTREE tree, WORD obj0, WORD nobj, LPBYTE where, WORD *here)
	{
	LPVOID	obspec;
	WORD	iobj, lstr;

	where[(*here)++] = '[';
	for (iobj = 0; iobj < nobj; iobj++)
		{
		if ( (obspec = tree[obj0 + iobj].ob_spec) == (LPVOID)-1) break;
		if ( !(lstr = LSTRLEN(obspec)))  break;
		if (iobj) where[(*here)++] = '|';
		LLSTRCPY(obspec, where + *here);
		*here += lstr;
		}
	where [(*here)++] = ']';
	}

	
VOID unparse_alert(LPTREE tree, LPBYTE where)
	{
	LPTREE	stree;
	LPVOID  obspec;
	WORD	picnum, here;

	obspec = tree[ALRTIMG].ob_spec;
	if (obspec == (LPVOID)-1) picnum = 0;
	else
		{
		ini_tree(&stree, ALRTPBX);
		for (picnum = 2; picnum >= 0; picnum--)
			if (obspec == stree[APBXIMG0 + picnum].ob_spec) break;
		picnum++;
		}
	where[0] = '[';
	where[1] = '0' + picnum;
	where[2] = ']';
	here = 3;
	pt_unparse(tree, ALRTSTR0, 5, where, &here);
	pt_unparse(tree, ALRTBTN0, 3, where, &here);
	where[here] = '\0';
	get_mem(here + 1);
	}

	
VOID dcomp_alerts()
	{
	LPLSTR	frstr;
	LPBYTE	where;
	LPTREE	tree;
	WORD	nalert, ntrind, nfrstr, itree, ntree, ndex;

	ntrind = head->rsh_ntree;
	for (nalert = itree = 0; itree < ntrind; itree++)
		{
		ndex = find_value((LPBYTE) tree_addr(itree));
		if (ndex != NIL && get_kind(ndex) == ALRT)
			nalert++;
		}

	if (!nalert) return;
	nfrstr = head->rsh_nstring;
	frstr = mak_frstr(nalert);

	for (ntree = itree = 0; itree < ntrind; itree++)
		{
		tree = tree_addr(itree);
		ndex = find_value((LPBYTE) tree);
		if (ndex == NIL || get_kind(ndex) != ALRT)
			{
			if (ntree != itree) *tree_ptr(ntree) =  tree_addr(itree);
			ntree++;
			}
		else
			{
			where = get_mem(0);
			unparse_alert(tree, where);
			*(frstr++) = where;
			set_value(ndex, (BYTE *) nfrstr++);
			}
		}

	head->rsh_ntree = ntree;
	}

	
WORD pt_roomp(LPTREE tree, WORD obj0, WORD nobj)
	{
	WORD	iobj;
	LPVOID	taddr;

	for (iobj = 0; iobj < nobj; iobj++)
		{
		taddr = tree[obj0 + iobj].ob_spec;
		if ( taddr == (LPVOID)-1) return (obj0 + iobj);
		if (!LSTRLEN(taddr)) return (obj0 + iobj);
		}
	return (0);
	} 

VOID pt_count(LPTREE tree, WORD obj0, WORD nobj, WORD *n, WORD *mxl)
	{
	LPVOID	taddr;
	WORD	iobj, slen;

	*n = *mxl = 0;
	for (iobj = 0; iobj < nobj; iobj++)
		{
		taddr = tree[obj0 + iobj].ob_spec;
		if (taddr != (LPVOID)-1)
			if (slen = LSTRLEN(taddr))
				{
				if (*n != iobj)
					{
					tree[obj0 + *n  ].ob_spec = taddr;
					tree[obj0 + iobj].ob_spec = (LPVOID)-1;
					}
				*n += 1;
				*mxl = max(*mxl, slen);
				}
		hide_obj(tree, obj0 + iobj);
		}
	}

	
VOID al_count(LPTREE tree, WORD *pic, WORD *nmsg, WORD *mxlmsg, WORD *nbut, WORD *mxlbut)
	{
	*pic = (tree[ALRTIMG].ob_spec != (LPVOID)-1);
	hide_obj(tree, ALRTIMG);
	pt_count(tree, ALRTSTR0, 5, nmsg, mxlmsg);
	pt_count(tree, ALRTBTN0, 3, nbut, mxlbut);
	} 

	
VOID al_space(LPTREE tree, WORD haveicon, WORD nummsg, WORD mlenmsg, 
                           WORD numbut,   WORD mlenbut)
{
	WORD		i;
	GRECT		al, ic, bt, ms;

	r_set(&al, 0, 0, 1+INTER_WSPACE, 1+INTER_HSPACE);
	r_set(&ms, 1 + INTER_WSPACE, 1 + INTER_HSPACE, mlenmsg, 1);

	if (haveicon)
	{
	  r_set(&ic, 1+INTER_WSPACE, 1+INTER_HSPACE, 4, 4);
	  al.g_w += ic.g_w + INTER_WSPACE;
	  al.g_h += ic.g_h + INTER_HSPACE + 1;
	  ms.g_x = ic.g_x + ic.g_w + INTER_WSPACE;
	}

	al.g_w += ms.g_w + INTER_WSPACE + 1;
	r_set(&bt, al.g_w, 1 + INTER_HSPACE, mlenbut, 1);

	al.g_w += bt.g_w + INTER_WSPACE + 1;
	al.g_h = max(al.g_h, 2 + (2 * INTER_HSPACE) + nummsg );
	al.g_h = max(al.g_h, 2 + INTER_HSPACE + (numbut * 2) - 1);
	ob_setxywh(tree, ROOT, &al);		/* init. root object	*/
						/* add icon object	*/
	if (haveicon)
	{
	  ob_setxywh(tree, 1, &ic);
	  unhide_obj(tree, ALRTIMG);
	}
						/* add msg objects	*/
	for(i=0; i<nummsg; i++)
	{
	  ob_setxywh(tree, ALRTSTR0+i, &ms);
	  ms.g_y++;
	  unhide_obj(tree, ALRTSTR0+i);
	}
						/* add button objects	*/
	for(i=0; i<numbut; i++)
	{
		
	  tree[ALRTBTN0 + i].ob_flags = SELECTABLE | EXIT;
	  tree[ALRTBTN0 + i].ob_state = NORMAL;
	  ob_setxywh(tree, ALRTBTN0+i, &bt);
	  bt.g_y += 2;
	}
						/* set last object flag	*/
	tree[ALRTBTN0 + numbut - 1].ob_flags = SELECTABLE | EXIT | LASTOB;
}



VOID fix_alert(LPTREE tree)
	{
	WORD	pic, pnummsg, plenmsg, pnumbut, plenbut;

	al_count(tree, &pic, &pnummsg, &plenmsg, &pnumbut, &plenbut);
	al_space(tree, pic, pnummsg, plenmsg, pnumbut, plenbut);
	map_tree(tree, ROOT, NIL, rs_obfix);
	tree[ROOT].ob_x = view.g_x;
	tree[ROOT].ob_y = view.g_y;
	}

