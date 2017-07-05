/************************************************************************/
/*	File:	hello.c							*/
/************************************************************************/
/*									*/
/*		     GGGGG        EEEEEEEE     MM      MM		*/
/*		   GG             EE           MMMM  MMMM		*/
/*		   GG   GGG       EEEEE        MM  MM  MM		*/
/*		   GG   GG        EE           MM      MM		*/
/*		     GGGGG        EEEEEEEE     MM      MM		*/
/*									*/
/************************************************************************/
/*									*/
/*			  +--------------------------+			*/
/*			  | Digital Research, Inc.   |			*/
/*			  | 60 Garden Court	     |			*/
/*			  | Monterey, CA.    93940   |			*/
/*			  +--------------------------+			*/
/*									*/
/*   The  source code  contained in  this listing is a non-copyrighted	*/
/*   work which  can be  freely used.  In applications of  this source	*/
/*   code you  are requested to  acknowledge Digital Research, Inc. as	*/
/*   the originator of this code.					*/
/*									*/
/*   Author:	Tom Rolander						*/
/*   PRODUCT:	GEM Sample Desk Top Accessory				*/
/*   Module:	HELLO							*/
/*   Version:	February 15, 1985					*/
/*									*/
/************************************************************************/


/*

Page*/
/*------------------------------*/
/*	includes		*/
/*------------------------------*/
/* #include "deskacc.h"				 #define DESKACC x	*/

#include "djgppgem.h"

/*------------------------------*/
/*	defines			*/
/*------------------------------*/

#define	ARROW		0
#define	HOUR_GLASS	2			

#define	DESK		0

#define END_UPDATE	0
#define	BEG_UPDATE	1


/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Data Structures			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	Global			*/
/*------------------------------*/
/* Not necessary in 32-bit
GLOBAL WORD	contrl[11];		// control inputs		  
GLOBAL WORD	intin[80];		// max string length		  
GLOBAL WORD	ptsin[256];		// polygon fill points		  
GLOBAL WORD	intout[45];		// open workstation output	  
GLOBAL WORD	ptsout[12];
*/

/*------------------------------*/
/*	Local			*/
/*------------------------------*/

WORD	gl_wchar;			/* character width		*/
WORD	gl_hchar;			/* character height		*/
WORD	gl_wbox;			/* box (cell) width		*/
WORD	gl_hbox;			/* box (cell) height		*/
WORD	gem_handle;			/* GEM vdi handle		*/
WORD	vdi_handle;			/* hello vdi handle		*/
WORD	work_out[57];			/* open virt workstation values	*/
GRECT	work_area;			/* current window work area	*/
WORD	gl_apid;			/* application ID		*/
WORD	gl_rmsg[8];			/* message buffer		*/
WORD	gl_itemhello = 0;		/* hello menu item		*/
WORD	gl_xfull;			/* full window 'x'		*/
WORD	gl_yfull;			/* full window 'y'		*/
WORD	gl_wfull;			/* full window 'w' width	*/
WORD	gl_hfull;			/* full window 'h' height	*/
WORD	ev_which;			/* event message returned value	*/
WORD	hello_whndl = 0;		/* hello window handle		*/
WORD	type_size;			/* system font cell size	*/
#define	MESS_NLINES	2		/* maximum lines in message	*/
#define	MESS_WIDTH	7		/* maximum width of message	*/
BYTE	*message[] =			/* message for window		*/
{
	" Hello ",
	" World ",
	0				/* null pointer terminates input*/
};
BYTE	*wdw_title =	"";		/* blank window title		*/
LPBYTE	str_title;

/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Local Procedures			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	min			*/
/*------------------------------*/
WORD
min(a, b)			/* return min of two values	*/
WORD		a, b;
{
	return( (a < b) ? a : b );
}


/*------------------------------*/
/*	max			*/
/*------------------------------*/
WORD
max(a, b)			/* return max of two values	*/
WORD		a, b;
{
	return( (a > b) ? a : b );
}



/*------------------------------*/
/*	do_open			*/
/*------------------------------*/
WORD
do_open(wh, org_x, org_y, x, y, w, h)	/* grow and open specified wdw	*/
WORD	wh;
WORD	org_x, org_y;
WORD	x, y, w, h;
{
	WORD	ret_code;

	graf_mouse(2,0x0L);
	graf_growbox(org_x, org_y, 21, 21, x, y, w, h);
	ret_code = wind_open(wh, x, y, w, h);
	graf_mouse(ARROW,0x0L);
	return(ret_code);
}


/*------------------------------*/
/*	do_close		*/
/*------------------------------*/
VOID
do_close(wh, org_x, org_y)	/* close and shrink specified window	*/
WORD	wh;
WORD	org_x, org_y;
{
	WORD	x, y, w, h;

	graf_mouse(2,0x0L);
	wind_get(wh, WF_CXYWH, &x, &y, &w, &h);
	wind_close(wh);
	graf_shrinkbox(org_x, org_y, 21, 21, x, y, w, h);
	graf_mouse(ARROW,0x0L);
}

/*------------------------------*/
/*	set_clip		*/
/*------------------------------*/
VOID
set_clip(clip_flag, s_area)	/* set clip to specified area		*/
WORD	clip_flag;
GRECT	*s_area;
{
	WORD	pxy[4];

	rc_grect_to_array(s_area, pxy);
	vs_clip(vdi_handle, clip_flag, pxy);
}

/*------------------------------*/
/*	align_x			*/
/*------------------------------*/
WORD
align_x(x)		/* forces word alignment for column positon,	*/
WORD	x;		/*   rounding to nearest word			*/
{
	return((x & 0xfff0) + ((x & 0x000c) ? 0x0010 : 0));
}	

/*------------------------------*/
/*	wdw_size		*/
/*------------------------------*/
VOID
wdw_size(box, w, h)	/* compute window size for given w * h chars	*/
GRECT	*box;
WORD	w, h;
{
	WORD	pw, ph;

	vst_height(vdi_handle, type_size,
	&gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);

	pw = w * gl_wbox + 1;
	ph = h * gl_hbox + 1;

	wind_calc(WC_BORDER, 0x000B, gl_wfull/2-pw/2, gl_hfull/2-ph/2, pw, ph, &box->g_x, &box->g_y, &box->g_w, &box->g_h);
	box->g_x = align_x(box->g_x) - 1;
}


/*------------------------------*/
/*	disp_message		*/
/*------------------------------*/
VOID
disp_mesag(strptr, clip_area)	/* display message applying input clip	*/
BYTE	**strptr;
GRECT	*clip_area;
{
	WORD	pxy[4];
	WORD	ycurr;

	set_clip(TRUE, clip_area);
	vsf_interior(vdi_handle, 1);
	vsf_color(vdi_handle, WHITE);
	rc_grect_to_array(&work_area, pxy);
	graf_mouse(M_OFF, 0x0L);
	vr_recfl(vdi_handle, pxy);	/* clear entire message area	*/

	vsl_color(vdi_handle,BLACK);
	vswr_mode(vdi_handle,MD_REPLACE);
	vsl_type (vdi_handle,FIS_SOLID);
	vswr_mode(vdi_handle, 1);
	ycurr = work_area.g_y - 1;
	while (*strptr)			/* loop through text strings	*/
	{
		ycurr += gl_hbox;
		v_gtext(vdi_handle, work_area.g_x, ycurr, *strptr++);
	}
	graf_mouse(M_ON, 0x0L);
	set_clip(FALSE, clip_area);
}



/*------------------------------*/
/*	do_redraw		*/
/*------------------------------*/
VOID
do_redraw(wh, area)		/* redraw message applying area clip	*/
WORD	wh;
GRECT	*area;
{
	GRECT	box;

	graf_mouse(M_OFF, 0x0L);
	wind_get(wh, WF_FIRSTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	while ( box.g_w && box.g_h )
	{
		if (rc_intersect(area, &box))
		{
			if (wh == hello_whndl)
			{
				disp_mesag(message, &box);
			}
		}
		wind_get(wh, WF_NEXTXYWH, &box.g_x, &box.g_y, &box.g_w, &box.g_h);
	}
	graf_mouse(M_ON, 0x0L);
}



/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Message Handling			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	hndl_mesag		*/
/*------------------------------*/
BOOLEAN	hndl_mesag()
{
	GRECT	box;
	BOOLEAN	done; 
	WORD	wdw_hndl;

	done = FALSE;
	wdw_hndl = gl_rmsg[3];			/* wdw handle of mesag	*/
	switch( gl_rmsg[0] )			/* switch on type of msg*/
	{
	case AC_OPEN:				/* do accessory open	*/
		if ( (gl_rmsg[4] == gl_itemhello) && 
		    (!hello_whndl) )		/* unless already open	*/
		{
			graf_mouse(HOUR_GLASS, 0x0L);
			/* 0x0B = NAME | CLOSER | MOVER	*/
			hello_whndl = wind_create(0x000B, align_x(gl_xfull)-1, gl_yfull, gl_wfull, gl_hfull);
			if (hello_whndl == -1)
			{ 
				graf_mouse(ARROW, 0x0L);
				dj_form_alert(1,
	"[3][Fatal Error !|Window not available|for Hello.][ Abort ]");
				hello_whndl = 0;
				return(TRUE); 
			}

			dj_wind_setl(hello_whndl, WF_NAME, str_title, 0L);
			wdw_size(&box, MESS_WIDTH, MESS_NLINES);
#if	DESKACC					/* open from menu area	*/
			do_open(hello_whndl, gl_wbox*4, gl_hbox/2, box.g_x, box.g_y, box.g_w, box.g_h);
#else						/* open from screen cntr*/
			do_open(hello_whndl, gl_wfull/2, gl_hfull/2, box.g_x, box.g_y, box.g_w, box.g_h);
#endif
			wind_get(hello_whndl, WF_WXYWH,	&work_area.g_x, &work_area.g_y, &work_area.g_w, &work_area.g_h);
			disp_mesag(message, &work_area);
			graf_mouse(ARROW,0x0L);
		}
		else   
		{
			graf_mouse(ARROW, 0x0L);
			wind_set(hello_whndl, WF_TOP, 0, 0, 0, 0); 
		}
		break;

	case AC_CLOSE:				/* do accessory close	*/
		if ( (gl_rmsg[3] == gl_itemhello) &&
		    (hello_whndl) )
		{
			hello_whndl = 0;	/* reset window handle	*/
		}
		break;

	case WM_REDRAW:				/* do redraw wdw contnts*/
		do_redraw(wdw_hndl, (GRECT *) &gl_rmsg[4]);
		break;

	case WM_TOPPED:				/* do window topped	*/
		wind_set(wdw_hndl, WF_TOP, 0, 0, 0, 0);
		break;

	case WM_CLOSED:				/* do window closed	*/
#if	DESKACC					/* close to menu bar	*/
		do_close(hello_whndl, gl_wbox*4, gl_hbox/2);
#else						/* close to screen cntr	*/
		do_close(hello_whndl, gl_wfull/2, gl_hfull/2);
#endif
		wind_delete(hello_whndl);
		hello_whndl = 0;
		done = TRUE;
		break;

	case WM_MOVED:				/* do window move	*/
		wind_set(wdw_hndl, WF_CXYWH, align_x(gl_rmsg[4])-1, gl_rmsg[5], gl_rmsg[6], gl_rmsg[7]);
		wind_get(hello_whndl, WF_WXYWH,	&work_area.g_x, &work_area.g_y, &work_area.g_w, &work_area.g_h);
		break;

	default:
		break;
	} /* switch */
	return(done);
} /* hndl_mesag */



/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Hello Event Handler			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	hello			*/
/*------------------------------*/
VOID hello(VOID)
{
	BOOLEAN	done;

	/**/					/* loop handling user	*/
	/**/					/*   input until done	*/
	done = FALSE;				/*   -or- if DESKACC	*/
	while( !done )				/*   then forever	*/
	{
		ev_which = evnt_mesag(gl_rmsg);	/* wait for message	*/
		wind_update(BEG_UPDATE);	/* begin window update	*/
		done = hndl_mesag();		/* handle event message	*/
		wind_update(END_UPDATE);	/* end window update	*/
#if	DESKACC
		done = FALSE;	/* never exit loop for desk accessory	*/
#endif
	}
}


/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Termination				     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	hello_term		*/
/*------------------------------*/  
VOID hello_term(VOID) 
{
#if	DESKACC
	return;				/* Desk Accessory never ends	*/
#else
	v_clsvwk( vdi_handle );		/* close virtual work station	*/
	appl_exit();			/* application exit		*/ 
#endif
}

/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Initialization			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/

/*------------------------------*/
/*	hello_init		*/
/*------------------------------*/
WORD
hello_init()
{
	WORD	i;
	WORD	work_in[11];
	WORD	attributes[10];

	gl_apid = appl_init(NULL);		/* initialize libraries	*/

/* Move the title into DOS space */

	str_title = dj_string_addr(wdw_title);

#if	DESKACC
	wind_update(BEG_UPDATE);
#endif
	for (i=0; i<10; i++) work_in[i] = 1;
	work_in[10] = 2;

	gem_handle = graf_handle(&gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);
	vdi_handle = gem_handle;
	v_opnvwk(work_in,&vdi_handle,work_out);	/* open virtual work stn*/

	if (!vqt_attributes(vdi_handle, attributes))	/* get text attributes	*/
	{
		dj_form_alert(1, "[3][HELLO cannot continue.|"
                                     "This version of GEM does not|"
                                     "support the vqt__attributes call.][ OK ]");
#ifdef DESKACC
		type_size = 0;
#else
		return FALSE;
#endif
	}
	else type_size = attributes[7];

	
#if	DESKACC					/* enter hello in menu	*/
	/* [JCE] DAs aren't allowed to abort. So if vqt_attributes() didn't */
        /*      work, just don't register our menu item. */
	if (type_size)
		gl_itemhello = menu_register(gl_apid, dj_string_addr("  Hello") );
#else

	if (vdi_handle == 0)
		return(FALSE);
#endif

						/* init. message address*/
	wind_get(DESK, WF_WXYWH, &gl_xfull, &gl_yfull, &gl_wfull, &gl_hfull);

	return(TRUE);
}

/*

Page*/
/************************************************************************/
/************************************************************************/
/****								     ****/
/****			    Main Program			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/


/*------------------------------*/
/*	GEMAIN			*/
/*------------------------------*/
WORD GEMAIN(WORD argc, BYTE *ARGV[])
{
	if (hello_init())			/* initialization	*/
	{
#if	DESKACC
		wind_update(END_UPDATE);
		hello();
#else						/* simulate AC_OPEN	*/
		gl_rmsg[0] = AC_OPEN;
		gl_rmsg[4] = gl_itemhello;
		hndl_mesag();

		hello();
	
		hello_term();			/* termination		*/
#endif
	}
	return 0;
}

