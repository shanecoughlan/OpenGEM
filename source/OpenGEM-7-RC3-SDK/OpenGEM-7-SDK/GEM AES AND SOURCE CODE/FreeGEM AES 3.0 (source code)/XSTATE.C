
#include "aes.h"

	VOID
draw_hilite(obtype, t)
	WORD obtype;
	GRECT *t;
{
	WORD	puff_amt;

	vsl_udsty( 0xAAAA );
	vsl_type( 7 );
	gsx_attr(FALSE, MD_XOR, BLACK);
	switch( obtype ) {
          case G_ICON: 
	  case G_CLRICN:
		puff_amt = 1; break;
	  case G_STRING: 
		puff_amt = 3; break;
	  default: 
		puff_amt = 5; break;
         }
	 gr_box(t->g_x-puff_amt, t->g_y-puff_amt, 
		 t->g_w+2*puff_amt, t->g_h+2*puff_amt, 2);
	 vsl_type( 1 );
}


VOID wstate(LPTREE tr, WORD obj, GRECT *t, WORD left, WORD th, WORD cc)
{
	ICONBLK ib;

	LBCOPY((LPBYTE)&ib, (LPBYTE)tr[obj].ob_spec, sizeof(ICONBLK));

	ib.ib_yicon = t->g_y + (t->g_h / 2) - (ib.ib_hicon / 2); 

	if (left)
	{
		ib.ib_xicon = t->g_x + th;
	}
	else
	{
		ib.ib_xicon = t->g_x + t->g_w - ib.ib_wicon - th;
	}

	if (tr == gl_uicolour) 
	{
		ib.ib_char &= 0xFF;
		ib.ib_char |= ((cc_s[cc].cc_foreground << 8 ) & 0x0F00);
		ib.ib_char |= ((cc_s[cc].cc_background << 12) & 0xF000);
	}

	gr_gicon(0, ib.ib_pmask, ib.ib_pdata, ib.ib_ptext, ib.ib_char, 
			 ib.ib_xchar, ib.ib_ychar, (GRECT *)&ib.ib_xicon,
			 (GRECT *)&ib.ib_xtext);
}


VOID unstate(LPTREE tr, WORD obj, GRECT *t, WORD left, WORD th, WORD do3d)
{
	ICONBLK ib;

	LBCOPY((LPBYTE)&ib, (LPBYTE)tr[obj].ob_spec, sizeof(ICONBLK));

	ib.ib_yicon = t->g_y + (t->g_h / 2) - (ib.ib_hicon / 2); 

	if (left)
	{
		ib.ib_xicon = t->g_x + th;
	}
	else
	{
		ib.ib_xicon = t->g_x + t->g_w - ib.ib_wicon - th;
	}

	if (do3d) 
	{
		grcc_rect( CC_BUTTON, (GRECT *)&ib.ib_xicon );
	}
	else
	{
		gr_rect(WHITE, 0, (GRECT *)&ib.ib_xicon );
	}
}


#define SELCHECKED (CHECKED | SELECTED)

VOID xstate(WORD obtype, WORD state, WORD doit3d, WORD th, GRECT *t, WORD flags)
{
	LPTREE tr, tr3d;
	WORD do3d;
	
	if (state)
	{
	  if ( state & OUTLINED )
	  {
	      gsx_attr(FALSE, MD_REPLACE, BLACK);
	      gr_box(t->g_x-3, t->g_y-3, t->g_w+6, t->g_h+6, 1);
	      gsx_attr(FALSE, MD_REPLACE, WHITE);
	      gr_box(t->g_x-2, t->g_y-2, t->g_w+4, t->g_h+4, 2);
	  }

	  if (th > 0)			/* ensure t is inside the border */
	    gr_inside(t, th);
	  else 
	    th = -th;

	  if ( (state & SHADOWED) && th )
	  {
	    vsf_color(1);	/* All drop shadows cast a dgrey shadow */
	    bb_fill(MD_ERASE, FIS_PATTERN, 4, t->g_x+t->g_w+th, t->g_y+(4*th),
				4*th, t->g_h+th );
	    bb_fill(MD_ERASE, FIS_PATTERN, 4, t->g_x+(4*th), t->g_y+t->g_h+th, 
			t->g_w+th, 4*th);
	  }

	  /* GEM5-style checkboxes & radio buttons 
       *
       * nb: G_ICONs already use DRAW3D, so we don't let them use this.
	   */
	  
	  if ( (state & DRAW3D) && obtype != G_ICON)
	  {
		tr = gl_uicolour;
		if (gl_domono) tr = gl_uimono;
		tr3d = tr;
		
		/* If the object is not 3D, and WHITEBAK is set, force the mono
		 * version of the graphic. WHITEBAK has other uses with 3D controls. 
		 */
		do3d = (flags & FL3DACT);
		
		if (!do3d && (state & WHITEBAK)) tr3d = gl_uimono;

		do3d = (!(state & WHITEBAK));
		
		/* Show a "default" symbol on suitable buttons. */
		
		if (obtype == G_BUTTON)
		{
			if (flags & DEFAULT)
			{
				wstate(tr3d, CDFLT, t, 0, th, CC_3DSHADE);
			}
		}
		else if (flags & SELECTABLE)
		{
			/* Radio buttons are objects (not true buttons) that have 
			 * SELECTABLE and RBUTTON flags set */
			if (flags & RBUTTON)
			{
				if (!(state & SELCHECKED)) unstate(tr, CDOT, t, 1, th, do3d);
				wstate(tr3d, CCIRC, t, 1, th, CC_3DSHADE);
				if (state & SELCHECKED) wstate(tr, CDOT, t, 1, th, CC_RADIO);
			}
			/* Checkboxes are objects (not true buttons) that have 
			 * SELECTABLE set and RBUTTON not set */
			else 
			{
				if (!(state & SELCHECKED)) unstate(tr, CTICK, t, 1, th, do3d);
				wstate(tr3d, CSQUARE, t, 1, th, CC_3DSHADE);
				if (state & SELCHECKED) wstate(tr, CTICK, t, 1, th, CC_CHECK);
			}
		
			/* SELECTED has been drawn. Don't draw it again */	
			state &= ~SELECTED;
		}
	  }
	  else if ( state & CHECKED )
	  {
	    gsx_attr(TRUE, MD_TRANS, BLACK);
	    intin[0] = 0x10;				/* a check mark	*/
	    gsx_tblt(IBM, t->g_x+2, t->g_y, 1);
	  }

	  
	  
	  if ( state & CROSSED )
	  {
	    gsx_attr(FALSE, MD_TRANS, WHITE);
	    gsx_cline(t->g_x, t->g_y, t->g_x+t->g_w-1, t->g_y+t->g_h-1);
	    gsx_cline(t->g_x, t->g_y+t->g_h-1, t->g_x+t->g_w-1, t->g_y);
	  }
	  if ( state & DISABLED )
	  {
	    vsf_color(WHITE);
	    bb_fill(MD_TRANS, FIS_PATTERN, IP_4PATT, t->g_x, t->g_y,
			 t->g_w, t->g_h);
	  }

	  if (state & SELECTED)
	  {
	    if( doit3d && !gl_domono)
	    {
	      gr_3dpress( t, -th, !(state & WHITEBAK));		/*910326WHF*/
	    }
	    else
	      bb_fill( MD_XOR, FIS_SOLID, IP_SOLID, t->g_x, t->g_y, t->g_w, t->g_h);
	  }

	
	  if ( (state & HIGHLIGHTED || state & UNHIGHLIGHTED) 
	  	&& (obtype!=G_USERDEF) && (obtype!=G_DTMFDB)) /* New state to draw a field outline */
	  {
	      draw_hilite(obtype, t);
	  }
	}
}

MLOCAL	WORD exobj_num(LPTREE tree, WORD obj)
{

    return (tree[obj].ob_type >> 8) & 0xFF;
}



VOID ystate(WORD obtype, LONG spec, GRECT *t, LPTREE tree, WORD obj)
{
	WORD		tmpx, tmpy, len;
	WORD		n;
	
	if ( (obtype == G_STRING) ||
	     (obtype == G_TITLE) ||
             (obtype == G_BUTTON) )
	{
	  len = LBWMOV((LPWORD)ad_intin, (LPBYTE)spec);
	  if (len)
	  { 
	    gsx_attr(TRUE, MD_TRANS, BLACK);
	    tmpy = t->g_y + ((t->g_h-gl_hchar)/2);
	    if (obtype == G_BUTTON)
	      tmpx = t->g_x + ((t->g_w-(len*gl_wchar))/2);
	    else
	      tmpx = t->g_x;
#if DBCS
	    for( n=0 ; n<len && intin[n] && intin[n]!='_' ; n++ )
	      if (dbcs_lead(intin[n]) && intin[n+1])
		n++;
#else /* DBCS */
	    for( n=0 ; n<len && intin[n] && intin[n]!='_' ; n++ );
#endif /* DBCS */	
	    if( !intin[n] || n==len || exobj_num(tree, obj) )
	      gsx_tblt(IBM, tmpx, tmpy, len);
	    else{
#if RED_UNDERLINES		
	      intin[n]=0;	/* output string preceeding '_' char */
	      gsx_tblt(IBM, tmpx, tmpy, n );

	      if ( DISABLED & (tree+obj)->ob_state ) {
			/* draw greyed out items with grey underlines */
		gsx_attr(TRUE, MD_TRANS, BLACK ) ;
	      } else {	/* draw in red */
		gsx_attr(TRUE, MD_TRANS, RED ) ;
	      }

	      tmpx += (n*gl_wchar) ;
	      LBWMOV( (LPWORD)ad_intin, (LPBYTE)"_" ) ;
	      gsx_tblt(IBM, tmpx, tmpy, 1 );
	    
	      spec+=(n+1);	
	      gsx_attr(TRUE, MD_TRANS, BLACK); /* back to black */
				      /* ..and put the rest of the string */
	      LBWMOV( (LPWORD)ad_intin, (LPBYTE)spec ); 
	      gsx_tblt(IBM, tmpx, tmpy, len-(n+1) );

#else /* RED_UNDERLINES */
	      sav_ch=intin[n+1];
	      intin[n+1]=0;	/* Output string up to '_' character */
	      gsx_tblt(IBM, tmpx, tmpy, n+1 );
	      intin[n+1]=sav_ch;
	      spec+=(n+1);	/* Point past '_' and backspace 1 */
	      LBWMOV( ad_intin, spec ); /* ..and put the rest of the string */
	      gsx_tblt(IBM, tmpx+(n*gl_wchar), tmpy, len-(n+1) );
#endif /* RED_UNDERLINES */
	    }				/* ..starting on top of the '_' */
	  }
	}
}
