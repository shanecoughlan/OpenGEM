
#include <stdio.h>
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
WORD	type_size = 8;			/* system font cell size	*/
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
/****			    Hello Event Handler			     ****/
/****								     ****/
/************************************************************************/
/************************************************************************/



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
	v_clsvwk( vdi_handle );		/* close virtual work station	*/
	appl_exit();			/* application exit		*/ 
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

	gl_apid = appl_init(NULL);		/* initialize libraries	*/

/* Move the title into DOS space */

	str_title = dj_string_addr(wdw_title);

	for (i=0; i<10; i++) work_in[i] = 1;
	work_in[10] = 2;

	gem_handle = graf_handle(&gl_wchar,&gl_hchar,&gl_wbox,&gl_hbox);
	vdi_handle = gem_handle;
	v_opnvwk(work_in,&vdi_handle,work_out);	/* open virtual work stn*/


	if (vdi_handle == 0)
		return(FALSE);

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

	/* The actual property test */
		{
			char s[240];
			char buf[50];

			sprintf(s,"[0][prop__del(ex2) returns %d][ OK ]", 
				prop_del("Example", "ex2", 0));
			dj_form_alert(1, s);

			sprintf(s,"[0][prop__get(ex1) returns %d|%s][ OK ]", 
				prop_get("Example", "ex1", buf, 50, 0), buf);
			dj_form_alert(1, s);

			sprintf(s,"[0][prop__get(ex2) returns %d|%s][ OK ]", 
				prop_get("Example", "ex2", buf, 50, 0), buf);
			dj_form_alert(1, s);

			sprintf(s,"[0][prop__get(ex3) returns %d|%s][ OK ]", 
				prop_get("Example", "ex3", buf, 50, 0), buf);
			dj_form_alert(1, s);

			sprintf(s,"[0][prop__get(ex4) returns %d|%s][ OK ]", 
				prop_get("Example", "ex4", buf, 50, 0), buf);
			dj_form_alert(1, s);

			sprintf(s,"[0][prop__put(ex3,plover) returns %d][ OK ]", 
				prop_put("EXAMPLE", "EX3", "plover", 0));

			dj_form_alert(1, s);
			sprintf(s,"[0][prop__put(ex0,fish) returns %d][ OK ]", 
				prop_put("example", "ex0", "fish", 0));
			dj_form_alert(1, s);

		}
		hello_term();			/* termination		*/
	}
	return 0;
}

