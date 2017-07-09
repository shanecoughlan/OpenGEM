/************************************************************************/
/*	File:	xform.c							*/
/************************************************************************/
/*									*/
/*			  +--------------------------+			*/
/*			  | Digital Research, Inc.   |			*/
/*			  | 60 Garden Court	     |			*/
/*			  | Monterey, CA.     93940  |			*/
/*			  +--------------------------+			*/
/*									*/
/*   The  source code  contained in  this listing is a non-copyrighted	*/
/*   work which  can be  freely used.  In applications of  this source	*/
/*   code you  are requested to  acknowledge Digital Research, Inc. as	*/
/*   the originator of this code.					*/
/*									*/
/*   Author:	Chris Keith, Tom Rolander, Tim Oren			*/
/*   PRODUCT:	GEM Sample Application					*/
/*   Module:	xform,  Version 0.1					*/
/*   Version:	March 28, 1985						*/
/*									*/
/*   Update:	Oct. 1, 1985 for 68K by Lowell Webster			*/
/*									*/ 
/************************************************************************/

#include "djgppgem.h"				/* portable coding conv	*/
#include "xform.h"				/* xform apl resource	*/

/* user or page coordinate system is IN_COORDS points per inch, with 	*/
/* (0,0) in the upper left corner.					*/

#define IN_COORDS	1024		/* number of user coordinate points */
					/* per inch */
#define PG_WIDTH	10240		/* 8 x 10 inch page */
#define PG_HEIGHT	8192 

#define	ARROW		0
#define	HOUR_GLASS	2
#define	DESK		0
#define END_UPDATE	0
#define	BEG_UPDATE	1

#define WAIT_OUT	1		/* waiting for mouse to go out */
#define WAIT_IN		0		/* waiting for mouse to go in  */

#define MX_FONTS	4
#define MX_FSIZES	8
#define MX_COORDS 	256
/*

Page*/
/*----------------------------------------------------------------------*/

GLOBAL WORD	wtype = 0x002d ;	/* window type.			*/
GLOBAL WORD	gl_wchar;		/* character width		*/
GLOBAL WORD	gl_hchar;		/* character height		*/
GLOBAL WORD	gl_wbox;		/* box (cell) width		*/
GLOBAL WORD	gl_hbox;		/* box (cell) height		*/
GLOBAL WORD	gl_hspace;		/* height of space between lines*/
GLOBAL WORD	gem_handle;		/* GEM vdi handle		*/
GLOBAL WORD	vdi_handle;		/* xform vdi handle		*/
GLOBAL WORD	work_out[57];		/* open virt workstation values	*/
GLOBAL GRECT	scrn_area;		/* whole screen area		*/
GLOBAL GRECT	work_area;		/* drawing area of main window  */
GLOBAL WORD	gl_rmsg[8];		/* message buffer		*/
GLOBAL LONG	gl_menu;		/* menu tree address		*/
GLOBAL WORD	gl_apid;		/* application ID		*/
GLOBAL WORD	gl_xfull;		/* full window 'x'		*/
GLOBAL WORD	gl_yfull;		/* full window 'y'		*/
GLOBAL WORD	gl_wfull;		/* full window 'w'		*/
GLOBAL WORD	gl_hfull;		/* full window 'h'		*/
GLOBAL WORD	scrn_width;		/* screen width in pixels	*/
GLOBAL WORD	scrn_height;		/* screen height in pixels	*/
GLOBAL WORD	scrn_xsize;		/* width of one pixel		*/
GLOBAL WORD	scrn_ysize;		/* height of one pixel		*/
GLOBAL WORD	ev_which;		/* event multi return state(s)	*/
GLOBAL UWORD	mousex, mousey;		/* mouse x,y position		*/
GLOBAL UWORD	bstate, bclicks;	/* button state, & # of clicks	*/
GLOBAL UWORD	kstate, kreturn;	/* key state and keyboard char	*/
GLOBAL WORD	monumber = 5;		/* mouse form number		*/
GLOBAL LONG	mofaddr = 0x0L;		/* mouse form address		*/
					/* xform window title		*/
GLOBAL BYTE	*wdw_title = " Transform Window ";

GLOBAL WORD	sk_length = 0 ;		/* # of points in xform		*/
GLOBAL WORD	sk_data[MX_COORDS];	/* xform data points		*/
GLOBAL WORD	xf_whndl;		/* xform window handle		*/
GLOBAL WORD	cur_fit = FULLITEM ;	/* type of transformation in effect */

GLOBAL WORD	evnts ;			/* for sketch event multi	*/
GLOBAL WORD   	m_out ;

GLOBAL WORD	avail_text[ MX_FONTS][ MX_FSIZES ] ;
GLOBAL WORD	nfonts ;

extern LPWORD   pPtsin;	 /* We will need to create a new, larger ptsin */
extern LPWORD   pPblock; /*  and change the pointer to it in pPblock */

/* Prototypes */

VOID add_pt(VOID);
VOID set_xform(VOID);
VOID rdr_mesg(VOID);
VOID do_redraw(WORD wh, GRECT *area);

/*

Page*/

/*----------------------------------------------------------------------*/
    WORD
min(a, b)			/* return min of two values */
    WORD		a, b;
{
	return( (a < b) ? a : b );
}



/*----------------------------------------------------------------------*/
    WORD
max(a, b)			/* return max of two values */
    WORD		a, b;
{
	return( (a > b) ? a : b );
}




/*----------------------------------------------------------------------*/
    VOID
do_open(wh, org_x, org_y, x, y, w, h)	/* grow and open specified wdw	*/
    WORD	wh;
    WORD	org_x, org_y;
    WORD	x, y, w, h;
{
	graf_growbox(org_x, org_y, 21, 21, x, y, w, h);
	wind_open(wh, x, y, w, h);
}


/*----------------------------------------------------------------------*/
    VOID
do_close(wh, org_x, org_y)	/* close and shrink specified window	*/
    WORD	wh;
    WORD	org_x, org_y;
{
	WORD	x, y, w, h;

	wind_get(wh, WF_CXYWH, &x, &y, &w, &h);
	wind_close(wh);
	graf_shrinkbox(org_x, org_y, 21, 21, x, y, w, h);
}


/*----------------------------------------------------------------------*/
    VOID
get_work()
{
	wind_get(xf_whndl, WF_WXYWH,
		&work_area.g_x, &work_area.g_y,
		&work_area.g_w, &work_area.g_h);
} /* get_work*/


/*----------------------------------------------------------------------*/
    VOID
do_full(wh)	/* depending on current window state, either make window*/
    WORD	wh; /*   full size -or- return to previous shrunken size */
{
	GRECT	prev;
	GRECT	curr;
	GRECT	full;

	wind_get(wh, WF_CXYWH, &curr.g_x, &curr.g_y, &curr.g_w, &curr.g_h);
	wind_get(wh, WF_PXYWH, &prev.g_x, &prev.g_y, &prev.g_w, &prev.g_h);
	wind_get(wh, WF_FXYWH, &full.g_x, &full.g_y, &full.g_w, &full.g_h);
	if ( rc_equal(&curr, &full) )
	{					/* is full now so change*/
		/**/				/*   to previous	*/
		graf_shrinkbox(prev.g_x, prev.g_y, prev.g_w, prev.g_h,
			full.g_x, full.g_y, full.g_w, full.g_h);
		wind_set(wh, WF_CXYWH, prev.g_x, prev.g_y, prev.g_w, prev.g_h);
	}
	else
	{					/* is not full so make	*/
		/**/				/*   it full		*/
		graf_growbox(curr.g_x, curr.g_y, curr.g_w, curr.g_h,
			full.g_x, full.g_y, full.g_w, full.g_h);
		wind_set(wh, WF_CXYWH, full.g_x, full.g_y, full.g_w, full.g_h);
	}
    get_work() ;
    set_xform() ;
}


/*----------------------------------------------------------------------*/
VOID set_clip(WORD clip_flag, GRECT *s_area)	/* set clip to specified area	*/
{
	WORD	pxy[4];

	rc_grect_to_array(s_area, pxy);
	vs_clip(vdi_handle, clip_flag, pxy);
}


/*----------------------------------------------------------------------*/
    VOID
draw_rect(area)
    GRECT	*area;
{
    	WORD	pxy[10];

    	pxy[0] = area->g_x;
    	pxy[1] = area->g_y;
    	pxy[2] = area->g_x + area->g_w - 1;
    	pxy[3] = area->g_y + area->g_h - 1;
    	pxy[4] = pxy[2];
    	pxy[5] = pxy[3];
    	pxy[3] = pxy[1];
    	pxy[6] = pxy[0];
    	pxy[7] = pxy[5];
    	pxy[8] = pxy[0];
    	pxy[9] = pxy[1];
    	v_pline(vdi_handle, 5, pxy);
}



/*----------------------------------------------------------------------*/
VOID hndl_dial(LPTREE tree, WORD def, WORD x, WORD y, WORD w, WORD h)
{
	WORD	xdial, ydial, wdial, hdial ;

	form_center(tree, &xdial, &ydial, &wdial, &hdial);
	form_dial(0, scrn_width/2, scrn_height/2, 0, 0, xdial, ydial, wdial, hdial);
	form_dial(1, scrn_width/2, scrn_height/2, 0, 0, xdial, ydial, wdial, hdial);
	objc_draw(tree, ROOT, MAX_DEPTH, xdial, ydial, wdial, hdial);
	form_do(tree, def) ;
	form_dial(2, scrn_width/2, scrn_height/2, 0, 0, xdial, ydial, wdial, hdial);
	form_dial(3, scrn_width/2, scrn_height/2, 0, 0, xdial, ydial, wdial, hdial);
}

/*------------------------------*/
/*	string_addr		*/
/*------------------------------*/
LONG
string_addr(which)		/* returns a tedinfo LONG string addr	*/
WORD	which;
{
	LONG	where;

	rsrc_gaddr(R_STRING, which, &where);
	return (where);
} 

/*

Page*/
/************************************************************************/
/****			    Object Tree Manipulation		     ****/
/************************************************************************/



/*----------------------------------------------------------------------*/
    VOID
do_obj(tree, which, bit)	/* set specified bit in object state	*/
    LONG	tree;
    WORD	which, bit;
{
	WORD	state;

	state = LWGET(OB_STATE(which));
	LWSET(OB_STATE(which), state | bit);
}



/*----------------------------------------------------------------------*/
    VOID
undo_obj(tree, which, bit)	/* clear specified bit in object state	*/
    LONG	tree;
    WORD	which, bit;
{
	WORD	state;

	state = LWGET(OB_STATE(which));
	LWSET(OB_STATE(which), state & ~bit);
}


/*----------------------------------------------------------------------*/
    VOID
sel_obj(tree, which)		/* turn on selected bit of spcfd object	*/
    LONG	tree;
    WORD	which;
{
	do_obj(tree, which, SELECTED);
}


/*----------------------------------------------------------------------*/
    VOID
desel_obj(tree, which)		/* turn off selected bit of spcfd object*/
    LONG	tree;
    WORD	which;
{
	undo_obj(tree, which, SELECTED);
}


/*----------------------------------------------------------------------*/
    VOID
objc_xywh(tree, obj, p)		/* get x,y,w,h for specified object	*/
    LONG	tree;
    WORD	obj;
    GRECT	*p;
{
	objc_offset(tree, obj, &p->g_x, &p->g_y);
	p->g_w = LWGET(OB_WIDTH(obj));
	p->g_h = LWGET(OB_HEIGHT(obj));
}


/*

Page*/
/************************************************************************/
/****			    Menu Handling			     ****/
/************************************************************************/


/*----------------------------------------------------------------------*/
    VOID
do_about()			/* display xform Info... 	*/
{
	LONG	tree;
	GRECT	box;

	objc_xywh(gl_menu, DESK, &box);
	rsrc_gaddr(R_TREE, ABOUDIAL, &tree);
	hndl_dial(tree, 0, box.g_x, box.g_y, box.g_w, box.g_h);
	desel_obj(tree, ABOUOK);
}

/*----------------------------------------------------------------------*/
    VOID
do_view()
{
	LONG	tree;
	GRECT	box;

	objc_xywh(gl_menu, DESK, &box);
	rsrc_gaddr(R_TREE, FITDIAL, &tree);
	sel_obj(tree, cur_fit);
	hndl_dial(tree, 0, box.g_x, box.g_y, box.g_w, box.g_h);
	desel_obj(tree, FITOK);
	for ( cur_fit = FULLITEM ; 
	      (!(LWGET( OB_STATE( cur_fit ) ) & SELECTED) ) ; 
	      cur_fit++ )
	    ;
	set_xform() ;
	set_clip( TRUE, &work_area ) ;
	rdr_mesg() ;
}


/*----------------------------------------------------------------------*/
WORD hndl_mnu(WORD title, WORD item)
{
	WORD	done;

	done = FALSE;
	switch (item)
	{
	case ABOUITEM :
		do_about() ;
		break;
	case QUITITEM :
		done = TRUE;
		break;
	case VIEWITEM :
		do_view() ;
		break ;
	}
	menu_tnormal(gl_menu,title,TRUE);
	return (done);
}




/*

Page*/
/************************************************************************/
/****			    Message Handling			     ****/
/************************************************************************/



/*----------------------------------------------------------------------*/
    VOID
st_curr( wdw_hndl )
    WORD	wdw_hndl ;
{
    wind_set(wdw_hndl, WF_CXYWH, gl_rmsg[4],gl_rmsg[5], gl_rmsg[6], gl_rmsg[7]);
    get_work() ;
    set_xform() ;
} /* st_curr */


/*----------------------------------------------------------------------*/
    /*MLOCAL*/	
BOOLEAN hndl_mesag(VOID)
{
	BOOLEAN	done; 
	WORD	wdw_hndl;

	done = FALSE;
	wdw_hndl = gl_rmsg[3];
	switch( gl_rmsg[0] )
	{
	case MN_SELECTED:
		done = hndl_mnu(wdw_hndl, gl_rmsg[4]);
		break;
	case WM_REDRAW:
		do_redraw(wdw_hndl, (GRECT *) &gl_rmsg[4]);
		break;
	case WM_TOPPED:
		wind_set(wdw_hndl, WF_TOP, 0, 0, 0, 0);
		break;
	case WM_FULLED:
		do_full(wdw_hndl);
		break;
	case WM_SIZED:
	case WM_MOVED:
		st_curr( wdw_hndl ) ;
		if ( (cur_fit == SVASITEM) || (cur_fit == IGASITEM) )
		    rdr_mesg() ;
		break;
	} /* switch */
	return(done);
} /* hndl_mesag */


/*----------------------------------------------------------------------*/
/* display background */
VOID background()
{
    WORD	array[ 16 ] ;

    vsf_perimeter( vdi_handle, FALSE ) ;
    vsf_color( vdi_handle, 0 ) ;
    vsf_style( vdi_handle, 1 ) ;
    rc_grect_to_array( &work_area, array ) ;
    v_bar( vdi_handle, array ) ;
}

/*----------------------------------------------------------------------*/
/* set line attribs for drawing grid and sketch.			*/
    VOID
line_atts()
{
    vswr_mode( vdi_handle, 1 ) ;
    vsl_width( vdi_handle, 1 ) ;
    vsl_type( vdi_handle, 1 ) ;
    vsl_color( vdi_handle, 1 ) ;
    vsl_ends( vdi_handle, 0, 0 ) ;
} /* line_atts */


/*----------------------------------------------------------------------*/
/* draw a 1 inch square (in user units) grid, and the sketch, if any.	*/
VOID do_rdraw(VOID)
{
    WORD	pts[ MX_COORDS ] ;
    WORD	i ;
    WORD	end ;

    background() ;
    line_atts() ;
    vsl_type( vdi_handle, 3 ) ;
    pts[ 1 ] = x_udy_xform( 0 ) ;
    pts[ 3 ] = x_udy_xform( PG_HEIGHT ) ;
    for ( i = 0 ; i <= PG_WIDTH ; i += IN_COORDS )
    {
	pts[ 0 ] = pts[ 2 ] = x_udx_xform( i ) ;
	v_pline( vdi_handle, 2, pts ) ;
    }   
    pts[ 0 ] = x_udx_xform( 0 ) ;
    pts[ 2 ] = x_udx_xform( PG_WIDTH ) ;
    for ( i = 0 ; i <= PG_HEIGHT ; i += IN_COORDS )
    {
	pts[ 1 ] = pts[ 3 ] = x_udy_xform( i ) ;
	v_pline( vdi_handle, 2, pts ) ;
    }   
    if (sk_length)
    {
	vsl_type( vdi_handle, 1 ) ;
	i = 0 ;
	end = sk_length * 2 ;
	while (i < end)
	{
	    pts[ i ] = x_udx_xform( sk_data[ i ] ) ;
	    i++ ;
	    pts[ i ] = x_udy_xform( sk_data[ i ] ) ;
	    i++ ;
	}      
	v_pline( vdi_handle, sk_length, pts ) ;
    }
} /* do_rdraw */



#define MS_LENGTH	8
/*----------------------------------------------------------------------*/
VOID rdr_mesg(VOID)
{
    WORD	ms_buf[ MS_LENGTH ] ;

    ms_buf[ 0 ] = 20 ;
    ms_buf[ 1 ] = gl_apid ;
    ms_buf[ 2 ] = 0 ;
    ms_buf[ 3 ] = xf_whndl ;
    ms_buf[ 4 ] = work_area.g_x ;
    ms_buf[ 5 ] = work_area.g_y ;
    ms_buf[ 6 ] = work_area.g_w ;
    ms_buf[ 7 ] = work_area.g_h ;
    appl_write( gl_apid, MS_LENGTH * 2, ms_buf ) ;
} /* rdr_mesg */


/*----------------------------------------------------------------------*/
VOID do_redraw(WORD wh, GRECT *area)
{
	GRECT	box;

	if (wh != xf_whndl)
	    return;
	graf_mouse(M_OFF, 0x0L);

	wind_get(wh, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	while ( box.g_w && box.g_h )
	{
		if (rc_intersect(area, &box))
		{
		    set_clip( TRUE, &box ) ;
		    do_rdraw() ;
		}
		wind_get(wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	}
	graf_mouse(M_ON, 0x0L);
}


/*

Page*/
/************************************************************************/
/****			    transformation procs		     ****/

/*----------------------------------------------------------------------*/
    VOID
full_view( zoom )
    WORD	zoom ;
{
    WORD	w, h ;
    WORD	x, y ;

    wind_calc( 1, wtype, gl_xfull, gl_yfull, gl_wfull, gl_hfull, 
			 &x,   &y,    &w,   &h ) ;
    x_sxform( 0, 0, PG_WIDTH / zoom, PG_HEIGHT / zoom, x, y, w, h ) ;
} /* full_view */


/*----------------------------------------------------------------------*/
/* set transformation according to current fit and work area.		*/
VOID set_xform(VOID)
{
    WORD	w, h ;

    switch( cur_fit )
    {   
	default :
	case FULLITEM : 			/* page to full size window */
	    full_view( 1 ) ;
	    break ;
	case ZOM2ITEM :
	    full_view( 2 ) ;
	    break ;
	case ZOM3ITEM :
	    full_view( 3 ) ;
	    break ;
	case SVASITEM :			/* page to work area, preserve aspect */
	    w = work_area.g_w ;
	    h = work_area.g_h ;
	        x_saspect( PG_WIDTH, PG_HEIGHT, &w, &h, work_out[3], work_out[4] ) ;   
	        x_sxform( 0, 0, PG_WIDTH, PG_HEIGHT,
			work_area.g_x, work_area.g_y, w, h);
	    rdr_mesg();
	    break ;
	case IGASITEM :			/* page to work area, ignore aspect */
	    x_sxform( 0, 0, PG_WIDTH, PG_HEIGHT,
			work_area.g_x, work_area.g_y, 
			work_area.g_w, work_area.g_h ) ;   
            rdr_mesg() ;
	    break ;
    }
} /* set_xform */

/*

Page*/
/************************************************************************/
/****			    Button Handling			     ****/
/************************************************************************/


/*----------------------------------------------------------------------*/
    WORD
sket_evnt_multi()
{
    WORD	ev ;

    wind_update(BEG_UPDATE);
    ev = evnt_multi( evnts,
	1, 1, 0,          
	1, mousex-4, mousey-4, 8, 8, 
	m_out,
	(UWORD) work_area.g_x, (UWORD) work_area.g_y,
	(UWORD) work_area.g_w, (UWORD) work_area.g_h,
	gl_rmsg,         		
	0, 0,				
	&mousex, &mousey, &bstate, &kstate,
	&kreturn, &bclicks);
    wind_update(END_UPDATE);
    return( ev ) ;
} /* sket_evnt_multi */


/*----------------------------------------------------------------------*/
/* mouse went in or out of window, change evnt_multi params accordingly. */
/* if outside, don't wait on small mouse movement for portion of sketch. */
    VOID
swap_evnt()
{
    if (evnts & MU_M1)
    {
	evnts &= ~MU_M1 ;
	m_out = WAIT_IN ;;
    }
    else
    {
	evnts |= MU_M1 ;
	m_out = WAIT_OUT ;
    }
} /* swap_evnt */


/*----------------------------------------------------------------------*/
    VOID
setup_sketch()
{
    line_atts() ;
    vsl_type( vdi_handle, 1 ) ;
    evnts = MU_BUTTON | MU_M1 | MU_M2 ;
    m_out = WAIT_OUT ;
    if (sk_length != 0)
    {
    	sk_length = 0 ;
	do_rdraw() ;
    }
    add_pt() ;
} /* setup_sketch */


/*----------------------------------------------------------------------*/
VOID add_pt(VOID)  
{
    WORD	index ;

    sk_length++ ; 
    index = (sk_length - 1) * 2 ;
    sk_data[ index ] = mousex ;
    sk_data[ index + 1 ] = mousey ;
} /* add_pt */


/*----------------------------------------------------------------------*/
/* transform sketch data into user coordinates.				*/
    VOID
xfm_sketch()
{
    WORD	i ;
    WORD	end ;

    i = 0 ;
    end = sk_length * 2 ;
    while (i < end)
    {
	sk_data[ i ] = x_dux_xform( sk_data[ i ] ) ;
	i++ ;
	sk_data[ i ] = x_duy_xform( sk_data[ i ] ) ;
	i++ ;
    }      
} /* xfm_sketch */


/*----------------------------------------------------------------------*/
WORD hndl_button(VOID )
{
    BOOLEAN  	done = FALSE ;
    BOOLEAN	no_ink = FALSE ;
    WORD	ev_which  ;
    WORD	j ;

    if (!rc_inside(mousex, mousey, &work_area))
	return(FALSE);
    setup_sketch() ;
    graf_mouse(M_OFF, 0x0L);
    while (!done) 
    {
	ev_which = sket_evnt_multi()  ;  
	if (ev_which & MU_M2)
	    swap_evnt() ;
	else if (ev_which & MU_M1)
	{
    	    add_pt() ;
	    v_pline( vdi_handle, 2, &sk_data[ (sk_length - 2) * 2 ] ) ;
	    done = no_ink = (sk_length * 2 >= MX_COORDS) ;
	}
	if (ev_which & MU_BUTTON)
	    done = TRUE ;
    }
    graf_mouse(M_ON, 0x0L);
    if (no_ink)					/* wait for mouse up - dont */
	evnt_button( 1, 1, 0, &j, &j, &j, &j ) ;/* fall into next sketch */
    xfm_sketch() ;

    return TRUE;
} /* hndl_button */


/*

Page*/
/************************************************************************/
/****			    xform Event Handler		     ****/
/************************************************************************/


/*----------------------------------------------------------------------*/
VOID xf(VOID)
{
	BOOLEAN	done;

	done = FALSE;
	do
	{
		wind_update(END_UPDATE);
		ev_which = evnt_multi(MU_BUTTON | MU_MESAG,
		0x02, 0x01, 0x01, 
		0,
		(UWORD) work_area.g_x, (UWORD) work_area.g_y,
		(UWORD) work_area.g_w, (UWORD) work_area.g_h,
		0, 0, 0, 0, 0,
		gl_rmsg, 0, 0, 
		&mousex, &mousey, &bstate, &kstate,
		&kreturn, &bclicks);
		wind_update(BEG_UPDATE);
		if (ev_which & MU_BUTTON)
		    hndl_button() ;
		if (ev_which & MU_MESAG)
		    done = hndl_mesag() ;
	} while (!done) ;
}
/*

Page*/
/************************************************************************/
/****			    Termination				     ****/
/************************************************************************/



/*----------------------------------------------------------------------*/
VOID xf_term(WORD term_type)
{
	switch (term_type)	/* NOTE: all cases fall through		*/
	{
		case (0 /* normal termination */):
			do_close(xf_whndl, gl_wfull/2, gl_hfull/2);
			wind_delete(xf_whndl);
		case (3):
			menu_bar(gl_menu, FALSE);
		case (2):
			v_clsvwk( vdi_handle );
		case (1):
			wind_update(END_UPDATE);
			appl_exit();
		case (4):
			break;
	}
}

/*

Page*/
/************************************************************************/
/****			    Initialization			     ****/
/************************************************************************/


/*----------------------------------------------------------------------*/
/* set the full width and height to produce a window with an work area	*/
/* aspect ratio of PG_WIDTH x PG_HEIGHT. Use wind_calc to find the 	*/
/* inside of the	*/
/* maximum window and pass those values to set_aspect (along with the	*/
/* screen attibutes) to calculate new inside window values. Then use	*/
/* wind_calc again to get the outside values of the window for		*/
/* wind_set.								*/

VOID ini_full(VOID)
{
    WORD	out_w, out_h ;
    WORD	out_x, out_y ;

    wind_calc( 1, wtype, gl_xfull, gl_yfull, gl_wfull, gl_hfull, 
			 &out_x,   &out_y,    &out_w,   &out_h ) ;
    x_saspect( PG_WIDTH, PG_HEIGHT, &out_w, &out_h, work_out[3], work_out[4] ) ;
    wind_calc( 0, wtype, out_x,     out_y,     out_w,     out_h,
			 &gl_xfull, &gl_yfull, &gl_wfull, &gl_hfull ) ;

} /* ini_full */


/*----------------------------------------------------------------------*/

WORD xf_init(VOID)
{
	WORD	work_in[11];
	WORD	i;
	LPWORD  pNewPtsin;

	gl_apid = appl_init(NULL);		/* initialize libraries	*/
	if (gl_apid == -1)
		return(4);
	wind_update(BEG_UPDATE);
	graf_mouse(HOUR_GLASS, 0x0L);

	/* Reallocate ptsin so it can hold 256 coordinate pairs */
	pNewPtsin = dos_alloc(MX_COORDS * 4);
	if (pNewPtsin) 
	{
		pPtsin = pNewPtsin;
		LSSET(pPblock + 8, pPtsin);
	}

	if (!rsrc_load( "xform.RSC" ))
	{
		graf_mouse(ARROW, 0x0L);
		dj_form_alert(1, "[3][Fatal Error !|xform.RSC|File Not Found][ Abort ]");
		return(1);
	}
	/* open virtual workstation */
	for (i=0; i<10; i++)
	{
		work_in[i]=1;
	}
	work_in[10]=2;

	gem_handle = graf_handle(&gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);
	vdi_handle = gem_handle;
	v_opnvwk(work_in,&vdi_handle,work_out);

	if (vdi_handle == 0)
		return(1);

	scrn_width = work_out[0] + 1;
	scrn_height = work_out[1] + 1;
	scrn_xsize = work_out[3];
	scrn_ysize = work_out[4];

	scrn_area.g_x = 0;
	scrn_area.g_y = 0;
	scrn_area.g_w = scrn_width;
	scrn_area.g_h = scrn_height;

	wind_get(DESK, WF_WXYWH, &gl_xfull, &gl_yfull, &gl_wfull, &gl_hfull);

	/* initialize menu	*/
	rsrc_gaddr(R_TREE, MAINMENU, &gl_menu);
	/* show menu		*/
	menu_bar(gl_menu, TRUE);	

	ini_full() ;
	xf_whndl = wind_create(wtype, gl_xfull, gl_yfull, gl_wfull, gl_hfull);
	if (xf_whndl == -1)
	{
		form_alert(1, string_addr(NWINDOW));
		return(3);
	}

	graf_mouse(HOUR_GLASS, 0x0L);
	dj_wind_setl(xf_whndl, WF_NAME, dj_string_addr(wdw_title), 0);
	do_open(xf_whndl, gl_wfull/2, gl_hfull/2, gl_xfull,
		gl_yfull, gl_wfull, gl_hfull);
	graf_mouse(ARROW,0x0L);
	get_work() ;
	set_xform() ;

	return(0);
}


/*----------------------------------------------------------------------*/
WORD GEMAIN(WORD ARGC, BYTE *ARGV[])
{
	WORD	term_type;

	if (!(term_type = xf_init()))
		xf();
	xf_term(term_type);
	return 0;
}
