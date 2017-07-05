/*	GEMFMALT.C		09/01/84 - 06/20/85	Lee Lorenzen	*/
/*	merge High C vers. w. 2.2 & 3.0		8/20/87		mdf	*/ 

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

#define MSG_OFF 2
#define BUT_OFF 7
#define NUM_ALOBJS 10
#define NUM_ALSTRS 8 
#define MAX_MSGLEN 40
#define INTER_WSPACE 1
#define INTER_HSPACE 0
						/* in GSXIF.C		*/
EXTERN	VOID	bb_restore();
EXTERN	VOID	bb_save();
						/* in OBLIB.C		*/
EXTERN	VOID	ob_draw();


/* ---------- added for metaware compiler ---------- */
EXTERN WORD	max();
EXTERN VOID	r_set();
EXTERN VOID 	ob_setxywh();
EXTERN WORD	rs_gaddr();
EXTERN VOID	gsx_mfset();
EXTERN VOID	ob_center();
EXTERN VOID 	wm_update();
EXTERN VOID	gsx_gclip();
EXTERN VOID 	gsx_sclip();
EXTERN WORD	fm_do();
/* -------------------------------------------------- */


GLOBAL UBYTE	gl_nils[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/*
*	Routine to break a string into smaller strings.  Breaks occur
*	whenever an | or a ] is encountered.
*/
	VOID
fm_strbrk(tree, palstr, stroff, pcurr_id, pnitem, pmaxlen)
	LPTREE		tree;
	LPBYTE		palstr;
	WORD		stroff;
	WORD		*pcurr_id;
	WORD		*pnitem;
	WORD		*pmaxlen;
{
	REG WORD	nitem, curr_id;
	REG WORD	len, maxlen;
	REG BYTE	tmp;
	FAR BYTE	*pstr;
	REG BYTE	nxttmp;

	nitem = maxlen = 0; 
	curr_id = *pcurr_id;
	tmp = NULL;
	while( tmp != ']')
	{
	  pstr = (LPBYTE) tree[stroff + nitem].ob_spec;
	  len = 0;
						/* get 1st char of new	*/
						/*   string		*/
	  do
	  {
	    tmp = palstr[curr_id];
	    curr_id++;
	    nxttmp = palstr[curr_id];
	    if ( (tmp == ']') ||
		 (tmp == '|') )
	    {
	      if (tmp == nxttmp)
		curr_id++;
	      else
	      {
		nxttmp = tmp;
	        tmp = NULL;
	      }
	    }
	    *(pstr + len++) = tmp;
	  } while ( tmp != NULL );
	  tmp = nxttmp;
	  maxlen = max(len - 1, maxlen);
	  nitem++;
	}
	*pcurr_id = curr_id;
	*pnitem = nitem;
	*pmaxlen = maxlen;
}


/*
*	Routine to parse a string into an icon #, multiple message
*	strings, and multiple button strings.  For example,
*
*		[0][This is some text|for the screen.][Ok|Cancel]
*		0123456
*
*	becomes:
*		icon# = 0;
*		1st msg line = This is some text
*		2nd msg line = for the screen.
*		1st button = Ok
*		2nd button = Cancel
*/

        VOID
fm_parse(tree, palstr, picnum, pnummsg, plenmsg, pnumbut, plenbut)
	REG LPTREE	tree;
	LPBYTE		palstr;
	WORD		*picnum;
	WORD		*pnummsg, *plenmsg;
	WORD		*pnumbut, *plenbut;
{
	WORD		curr_id;

	*picnum = palstr[1] - '0';

	curr_id = 4;
	fm_strbrk(tree, palstr, MSG_OFF, &curr_id, pnummsg, plenmsg);

	curr_id++;
	fm_strbrk(tree, palstr, BUT_OFF, &curr_id, pnumbut, plenbut);
	*plenbut += 1;
}

        VOID
fm_build(tree, haveicon, nummsg, mlenmsg, numbut, mlenbut)
	REG LPTREE	tree;
	WORD		haveicon;
	WORD		nummsg, mlenmsg;
	WORD		numbut, mlenbut;
{
	REG WORD	i, k;
	GRECT		al, ic, bt, ms;

	r_set(&al, 0, 0, 1+INTER_WSPACE, 1+INTER_HSPACE);
	r_set(&ms, 1 + INTER_WSPACE, 1 + INTER_HSPACE, mlenmsg, 1);

	if (haveicon)
	{
	  r_set(&ic, 1+INTER_WSPACE, 1+INTER_HSPACE, 4, 4);
	  al.g_w += ic.g_w + INTER_WSPACE;
	  al.g_h += ic.g_h + INTER_HSPACE + 1;
	  ms.g_x = ic.g_x + ic.g_w + INTER_WSPACE + 1;
	}

	al.g_w += ms.g_w + INTER_WSPACE + 1;
	r_set(&bt, al.g_w, 1 + INTER_HSPACE, mlenbut, 1);

	al.g_w += bt.g_w + INTER_WSPACE + 1;
	al.g_h = max(al.g_h, 2 + (2 * INTER_HSPACE) + nummsg );
	al.g_h = max(al.g_h, 2 + INTER_HSPACE + (numbut * 2) - 1);
	al.g_h |= -0x300; //0xfd00;	// [JCE] avoid compiler warnings
						/* init. root object	*/
	ob_setxywh(tree, ROOT, &al);
	ad_nils = (UBYTE FAR *)ADDR(&gl_nils[0]);
	for(i=0; i<NUM_ALOBJS; i++)
	  LBCOPY(&tree[i].ob_next, ad_nils, 6);
						/* add icon object	*/
	if (haveicon)
	{
	  ob_setxywh(tree, 1, &ic);
	  ob_add(tree, ROOT, 1);
	}
						/* add msg objects	*/
	for(i=0; i<nummsg; i++)
	{
	  ob_setxywh(tree, MSG_OFF+i, &ms);
	  ms.g_y++;
	  ob_add(tree, ROOT, MSG_OFF+i);
	}
						/* add button objects	*/
	for(i=0; i<numbut; i++)
	{
	  k = BUT_OFF+i;
	  tree[k].ob_flags = SELECTABLE | EXIT;
	  tree[k].ob_state = NORMAL;
	  ob_setxywh(tree, k, &bt);
	  bt.g_y++;
	  bt.g_y++;
	  ob_add(tree, ROOT, k);
	}
						/* set last object flag	*/
	tree[BUT_OFF + numbut - 1].ob_flags = SELECTABLE | EXIT | LASTOB;
}


fm_alert(defbut, palstr)
	WORD		defbut;
	LPBYTE		palstr;
{
	REG WORD	i;
	WORD		inm, nummsg, mlenmsg, numbut, button_id, mlenbut;
	LPTREE		tree;
	LPVOID		plong;
	GRECT		d, t;

						/* init tree pointer	*/
	rs_gaddr(ad_sysglo, R_TREE, 1, (LPVOID *)&tree);
	gsx_mfset(ad_armice);

	fm_parse(tree, palstr, &inm, &nummsg, &mlenmsg, &numbut, &mlenbut);

	fm_build(tree, (inm != 0), nummsg, mlenmsg, numbut, mlenbut);

	/* ViewMAX allows alerts to have Cancel buttons too */
	if (defbut)
	{
	    button_id = BUT_OFF + (defbut&0xFF) - 1 ;
	    (tree+button_id)->ob_flags |= DEFAULT ;
	    if( defbut>>8 ){
		button_id = BUT_OFF + (defbut>>8) - 1 ;
		(tree+button_id)->ob_flags |= ESCCANCEL ;
	    } 
	}

	if (inm != 0)
	{
	  rs_gaddr(ad_sysglo, R_BITBLK, inm-1, &plong);
	  tree[1].ob_spec = (LONG)plong;
	}
						/* convert to pixels	*/
	for(i=0; i<NUM_ALOBJS; i++)
	  rs_obfix(tree, i);
						/* fix up icon, 32x32	*/
	tree[1].ob_type = G_IMAGE;
	tree[1].ob_width  = 
	tree[1].ob_height = 32;
						/* center tree on screen*/
	ob_center(tree, &d);
						/* save screen under-	*/
						/*   neath the alert	*/
	wm_update(TRUE);
	gsx_gclip(&t);
	bb_save(&d);
						/* draw the alert	*/
	gsx_sclip(&d);
	ob_draw(tree, ROOT, MAX_DEPTH);
						/* let user pick button	*/
	i = fm_do(tree, 0);
						/* restore saved screen	*/
	gsx_sclip(&d);
	bb_restore(&d);
	gsx_sclip(&t);
	wm_update(FALSE);
						/* return selection	*/
	return( i - BUT_OFF + 1 );
}


