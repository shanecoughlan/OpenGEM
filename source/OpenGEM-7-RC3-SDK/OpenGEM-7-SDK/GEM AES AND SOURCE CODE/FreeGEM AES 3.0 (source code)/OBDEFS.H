/*	OBDEFS.H	03/15/84 - 02/08/85	Gregg Morris		*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1986			Digital Research Inc.
*	-------------------------------------------------------------
*/

#define ROOT 0
#define NIL -1
						/* keybd states		*/
#define K_RSHIFT 0x0001
#define K_LSHIFT 0x0002
#define K_CTRL 0x0004
#define K_ALT 0x00008
						/* max string length	*/
#define MAX_LEN 81
						/* max depth of search	*/
						/*   or draw for objects*/
#define MAX_DEPTH 8
						/* inside patterns	*/
#define IP_HOLLOW 0
#define IP_1PATT 1
#define IP_2PATT 2
#define IP_3PATT 3
#define IP_4PATT 4
#define IP_5PATT 5
#define IP_6PATT 6
#define IP_SOLID 7
						/* system foreground and*/
						/*   background rules	*/
						/*   but transparent	*/
/* #define SYS_FG 0x1100 now a variable, not a constant */

/* ViewMAX colour rules */
#define WTS_FG (sys_fg+sys_tr+CC_NAME)		/* window title selected*/
						/* use ColorCat CC_NAME */

#define WTN_FG (syn_fg+syn_tr+CC_SLCTDNAME)	/* window title normal	*/
						/* use CC_SLCTDNAME	*/

#define WTI_FG (syi_fg+syi_tr+CC_INFO)     	/* info bar normal	*/
						/* use CC_INFO	*/

//#define WTS_FG 0x11a1				/* window title selected*/
//						/*   using pattern 2 &	*/
//						/*   replace mode text 	*/
//#define WTN_FG 0x1100				/* window title normal	*/
						/* gsx modes		*/
#define MD_REPLACE 1
#define MD_TRANS 2
#define MD_XOR 3
#define MD_ERASE 4
						/* gsx styles		*/
#define FIS_HOLLOW 0
#define FIS_SOLID 1
#define FIS_PATTERN 2
#define FIS_HATCH 3
#define FIS_USER 4
						/* bit blt rules	*/
#define ALL_WHITE 0
#define S_AND_D 1
#define S_ONLY 3
#define NOTS_AND_D 4
#define S_XOR_D 6
#define S_OR_D 7
#define D_INVERT 10
#define NOTS_OR_D 13
#define ALL_BLACK 15
						/* font types		*/
#define IBM 3
#define SMALL 5

/* Object Drawing Types */
						/* Graphic types of obs	*/
#define G_BOX 20
#define G_TEXT 21
#define G_BOXTEXT 22
#define G_IMAGE 23
#define G_USERDEF 24
#define G_IBOX 25
#define G_BUTTON 26
#define G_BOXCHAR 27
#define G_STRING 28
#define G_FTEXT 29
#define G_FBOXTEXT 30
#define G_ICON 31
#define G_TITLE 32
#define G_CLRICN 33	/* [JCE] From ViewMAX beta. Incompatible with */
                        /*      Atari colour icons.                   */
#define G_DTMFDB 34	/* for internal AES use only: desktop image   */

						/* Object flags		 */
#define NONE 0x0
#define SELECTABLE 0x1
#define DEFAULT 0x2
#define EXIT 0x4
#define EDITABLE 0x8
#define RBUTTON 0x10
#define LASTOB 0x20
#define TOUCHEXIT 0x40
#define HIDETREE 0x80
#define INDIRECT 0x100
#define ESCCANCEL 0x200		/* [JCE] << From ViewMAX beta... */
#define BITBUTTON 0x400
#define SCROLLER 0x800
#define FLAG3D 0x1000
#define USECOLOURCAT 0x2000
#define USECOLORCAT  0x2000	/* >> ViewMAX */

/* [JCE 3-8-1999] Flags for Atari GEM compatibility. These are not 
 *               implemented; it's just to show that all 16 flags are
 *               taken!
 */
#define FL3DIND 0x1000	/* = FLAG3D */
#define FL3DBAK 0x4000
#define FL3DACT 0x5000	/* For compatibility, FL3DACT is the one with */
                      	/* both bits set */
#define SUBMENU 0x8000
						/* Object states	*/
#define NORMAL 0x0
#define SELECTED 0x1
#define CROSSED 0x2
#define CHECKED 0x4
#define DISABLED 0x8
#define OUTLINED 0x10
#define SHADOWED 0x20
#define WHITEBAK 0x40
#define DRAW3D 0x80
#define HIGHLIGHTED 0x100	/* [JCE] << From ViewMAX beta... */
#define UNHIGHLIGHTED 0x200	/* >> ViewMAX */
						/* Object colors	*/
#define WHITE 0
#define BLACK 1
#define RED 2
#define GREEN 3
#define BLUE 4
#define CYAN 5
#define YELLOW 6
#define MAGENTA 7
#define LWHITE 8
#define LBLACK 9
#define LRED 10
#define LGREEN 11
#define LBLUE 12
#define LCYAN 13
#define LYELLOW 14
#define LMAGENTA 15


typedef struct object
{
	WORD		ob_next;	/* -> object's next sibling	*/
	WORD		ob_head;	/* -> head of object's children */
	WORD		ob_tail;	/* -> tail of object's children */
	UWORD		ob_type;	/* type of object- BOX, CHAR,...*/
	UWORD		ob_flags;	/* flags			*/
	UWORD		ob_state;	/* state- SELECTED, OPEN, ...	*/
	LONG   		ob_spec;	/* "out"- -> anything else	*/
	WORD		ob_x;		/* upper left corner of object	*/
	WORD		ob_y;		/* upper left corner of object	*/
	WORD		ob_width;	/* width of obj			*/
	WORD		ob_height;	/* height of obj		*/
} OBJECT;

typedef OBJECT FAR *LPOBJ;
typedef OBJECT FAR *LPTREE;


typedef struct orect
{
	struct orect	*o_link;
	WORD		o_x;
	WORD		o_y;
	WORD		o_w;
	WORD		o_h;
} ORECT;
typedef ORECT FAR *LPORECT;


typedef struct grect
{
	WORD	g_x;
	WORD	g_y;
	WORD	g_w;
	WORD	g_h;
} GRECT;
typedef GRECT FAR *LPRECT;


typedef struct text_edinfo
{
	LPBYTE		te_ptext;	/* ptr to text (must be 1st)	*/
	LPBYTE		te_ptmplt;	/* ptr to template		*/
	LPBYTE		te_pvalid;	/* ptr to validation chrs.	*/
	WORD		te_font;	/* font				*/
	WORD		te_junk1;	/* junk word			*/
	WORD		te_just;	/* justification- left, right...*/
	WORD		te_color;	/* color information word	*/
	WORD		te_junk2;	/* junk word			*/
	WORD		te_thickness;	/* border thickness		*/
	WORD		te_txtlen;	/* length of text string	*/
	WORD		te_tmplen;	/* length of template string	*/
} TEDINFO;
typedef TEDINFO FAR *LPTEDI;


typedef struct icon_block
{
	LPBYTE	ib_pmask;
	LPBYTE	ib_pdata;
	LPBYTE	ib_ptext;
	WORD	ib_char;
	WORD	ib_xchar;
	WORD	ib_ychar;
	WORD	ib_xicon;
	WORD	ib_yicon;
	WORD	ib_wicon;
	WORD	ib_hicon;
	WORD	ib_xtext;
	WORD	ib_ytext;
	WORD	ib_wtext;
	WORD	ib_htext;
} ICONBLK;
typedef ICONBLK FAR *LPICON;


typedef struct bit_block
{
	LPBYTE	bi_pdata;		/* ptr to bit forms data	*/
	WORD	bi_wb;			/* width of form in bytes	*/
	WORD	bi_hl;			/* height in lines		*/
	WORD	bi_x;			/* source x in bit form		*/
	WORD	bi_y;			/* source y in bit form		*/
	WORD	bi_color;		/* fg color of blt 		*/
} BITBLK;
typedef BITBLK FAR *LPBBLK;



typedef struct parm_blk
{
	LPTREE	pb_tree;
	WORD	pb_obj;
	WORD	pb_prevstate;
	WORD	pb_currstate;
	WORD	pb_x, pb_y, pb_w, pb_h;
	WORD	pb_xc, pb_yc, pb_wc, pb_hc;
	LONG	pb_parm;
} PARMBLK;
typedef PARMBLK FAR *LPPARM;



typedef struct user_blk
{
	LONG	ub_code;
	LONG	ub_parm;
} USERBLK;
typedef USERBLK FAR *LPUSER;



#define EDSTART 0
#define EDINIT 1
#define EDCHAR 2
#define EDEND 3

#define TE_LEFT 0
#define TE_RIGHT 1
#define TE_CNTR 2

/* << ViewMAX */

/* Color categories used by Window Manager */
#define	CC_NAME		8
#define	CC_SLIDER	9
#define	CC_DESKTOP	10
#define	CC_BUTTON	11
#define	CC_INFO		12
#define	CC_ALERT	13
#define	CC_SLCTDNAME	14

#define CC_3DSHADE	 16
#define CC_RADIO	 17
#define CC_CHECK	 18
#define CC_NAMETEXT	 19	/* Text of active titlebar */
#define CC_NNAMETEXT 20	/* Text of inactive titlebar */
#define CC_INFOTEXT	 21	/* Text of info bar */

/* Desktop Image Modes */
#define DT_CENTER	1
#define DT_TILE		2

/* >> ViewMAX */


/* << [JCE] Enhancements post-DRI. This structure is returned by 
 *    appl_init() if addr_in[0] is supplied. DR GEM/ViewMAX will 
 *    ignore this parameter but FreeGEM or whatever we call it 
 *    will fill in the members. 
 *
 *    A calling program should set buf_len to sizeof(X_BUF) and the
 *    rest of the members to zero. GEM will set buf_len to the size
 *    of the data returned, and fill in the remaining fields.
 *
 **/

/* Colour category: public declaration */

typedef struct clrcat
{
	WORD	cc_foreground;
	WORD	cc_background;
	WORD	cc_style;
	WORD	cc_pattern;
} CLRCAT;

typedef CLRCAT FAR *LPCCAT;

typedef struct x_buf_v2
{
	UWORD	buf_len;			/* Size of structure, so it can grow. 
				 				 * Set by calling application. */
	WORD	arch;				/* 16 for 16-bit, 32 for hypothetical 
				 				 * 32-bit version. */
	LPCCAT	cc;					/* Address of an array of 16 CLRCAT 
				 				 * structures.
                                 * This is so that they can be read by
                                 * a program without having to fiddle
                                 * with ViewMAX.INI */
	LPTREE	w_active;	/* Address of the tree (NUM_ELEM elements)
                                 * that is used to draw window frames and 
                                 * controls. The intention is that the 
                                 * glyphs on the buttons could be changed 
                                 * by a program. */
	LPBYTE	info;		/* ASCII information such as a version 
				 * string. This should be max. 40 characters 
				 * with no newlines */
	LONG	abilities;	/* AES abilities: 
				 * bit 0: supports appl_getinfo()
                                 * bit 1: supports prop_get(), prop_put(), 
                                 * prop_del()
				 * bit 2: supports dynamic changing of
				 *       window glyphs
                                 * bit 3: supports GEM/5 style 3D.
				 				 */
} X_BUF_V2;

#define ABLE_GETINFO  1		/* bit 0 */
#define ABLE_PROP     2		/* bit 1 */
#define ABLE_WTREE    4		/* bit 2 */
#define ABLE_X3D      8 	/* bit 3 */
#define ABLE_XSHL    16		/* bit 4 */
#define ABLE_PROP2   32		/* bit 5 */
#define ABLE_EMSDESK 64		/* bit 6 */
#define ABLE_XBVSET 128		/* bit 7 */

typedef struct gui_opts
{
	WORD	opt_count;		/* No. of GUI options */
	WORD	rounded;		/* 1 Draw buttons rounded? */
	WORD	frame3d;		/* 2 3D frames */
	WORD	gui_compat;		/* 3 Compatibility level */
	WORD	menu_compat;		/* 4 Menu options */
	WORD	rsrc_compat;		/* 5 Resource load options */
} GUI_OPTS;

/* >> JCE */
