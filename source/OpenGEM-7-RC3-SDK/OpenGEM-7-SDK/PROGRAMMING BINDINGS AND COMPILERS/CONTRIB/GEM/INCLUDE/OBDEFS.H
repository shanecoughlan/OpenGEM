/*	OBDEFS.H		03/15/84 - 02/08/85	Gregg Morris	*/
/*	GEM DEVELOPER KIT 	06/07/86		Lowell Webster	*/
/*      DJGPP                   28/09/98                John Elliott */

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
#define SYS_FG 0x1100

#define WTS_FG 0x11a1				/* window title selected*/
						/*   using pattern 2 &	*/
						/*   replace mode text 	*/
#define WTN_FG 0x1100				/* window title normal	*/
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
#define G_PROGDEF 24
#define G_IBOX 25
#define G_BUTTON 26
#define G_BOXCHAR 27
#define G_STRING 28
#define G_FTEXT 29
#define G_FBOXTEXT 30
#define G_ICON 31
#define G_TITLE 32
#define G_CLRICN 33	/* ViewMAX/3 colour icon */
						/* Object flags		 */
#define NONE 		0x0
#define SELECTABLE 	0x1
#define DEFAULT 	0x2
#define EXIT 		0x4
#define EDITABLE	0x8
#define RBUTTON 	0x10
#define LASTOB 		0x20
#define TOUCHEXIT	0x40
#define HIDETREE 	0x80
#define INDIRECT 	0x100
#define	ESCCANCEL	0x0200
#define BITBUTTON	0x0400
#define SCROLLER	0x0800
#define	FLAG3D		0x1000
#define	USECOLORCAT	0x2000
#define VMAX_3D     	FLAG3D		/* For compatibility with prev. */
#define VMAX_SYSCOL 	USECOLORCAT	/* PTK versions */

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
#define NORMAL 		0x0000
#define SELECTED 	0x0001
#define CROSSED 	0x0002
#define CHECKED 	0x0004
#define DISABLED 	0x0008
#define OUTLINED 	0x0010
#define SHADOWED 	0x0020
#define WHITEBAK 	0x0040
#define DRAW3D 		0x0080
#define HIGHLIGHTED 	0x0100
#define UNHIGHLIGHTED 	0x0200 
						/* Object colors	*/
#define WHITE 0
#define BLACK 1
#define RED 2
#define GREEN 3
#define BLUE 4
#define CYAN 5
#define YELLOW 6
#define MAGENTA 7
#define DWHITE 8
#define DBLACK 9
#define DRED 10
#define DGREEN 11
#define DBLUE 12
#define DCYAN 13
#define DYELLOW 14
#define DMAGENTA 15

#define GPK __attribute__((packed));

#ifdef _cplusplus
#pragma pack(1)
#endif

typedef struct object 
{
	WORD		ob_next   GPK	/* 0 -> object's next sibling	*/
	WORD		ob_head   GPK	/* 2 -> head of object's children */
	WORD		ob_tail   GPK	/* 4 -> tail of object's children */
	UWORD		ob_type   GPK	/* 6 type of object- BOX, CHAR,...*/
	UWORD		ob_flags  GPK	/* 8 flags			*/
	UWORD		ob_state  GPK	/* a state- SELECTED, OPEN, ...	*/
	LONG		ob_spec   GPK	/* c "out"- -> anything else	*/
	UWORD		ob_x      GPK	/* 10 upper left corner of object	*/
	UWORD		ob_y      GPK	/* 12 upper left corner of object	*/
	UWORD		ob_width  GPK	/* 14 width of obj			*/
	UWORD		ob_height GPK	/* 16 height of obj		*/
} OBJECT  GPK



typedef struct orect
{
	WORD	o_link   GPK	/* Short pointer to next */
	WORD	o_x   	 GPK
	WORD	o_y   	 GPK
	WORD	o_w  	 GPK
	WORD	o_h   	 GPK
} ORECT  GPK


typedef struct grect
{
	WORD	g_x   GPK
	WORD	g_y   GPK
	WORD	g_w   GPK
	WORD	g_h   GPK
} GRECT  GPK


typedef struct text_edinfo
{
	BYTE		*te_ptext    GPK /*  0 ptr to text (must be 1st)	*/
	BYTE		*te_ptmplt   GPK /*  4 ptr to template		*/
	BYTE		*te_pvalid   GPK /*  8 ptr to validation chrs.	*/
	WORD		te_font      GPK /*  C font				*/
	WORD		te_junk1     GPK /*  E junk word			*/
	WORD		te_just      GPK /* 10 justification- left, right...*/
	WORD		te_color     GPK /* 12 color information word	*/
	WORD		te_junk2     GPK /* 14 junk word			*/
	WORD		te_thickness GPK /* 16 border thickness		*/
	WORD		te_txtlen    GPK /* 18 length of text string	*/
	WORD		te_tmplen    GPK /* 1A length of template string	*/
} TEDINFO  GPK


typedef struct icon_block
{
	VOID	*ib_pmask  GPK	/* 0 */
	VOID	*ib_pdata  GPK	/* 4 */
	BYTE 	*ib_ptext  GPK	/* 8 */
	WORD	ib_char    GPK	/* C */
	WORD	ib_xchar   GPK	/* E */
	WORD	ib_ychar   GPK	/* 10 */
	WORD	ib_xicon   GPK	/* 12 */
	WORD	ib_yicon   GPK	/* 14 */
	WORD	ib_wicon   GPK	/* 16 */
	WORD	ib_hicon   GPK	/* 18 */
	WORD	ib_xtext   GPK	/* 1A */
	WORD	ib_ytext   GPK	/* 1C */
	WORD	ib_wtext   GPK	/* 1E */
	WORD	ib_htext   GPK	/* 20 */
} ICONBLK  GPK



typedef struct bit_block
{
	VOID	*bi_pdata  GPK		/* ptr to bit forms data	*/
	WORD	bi_wb      GPK			/* width of form in bytes	*/
	WORD	bi_hl      GPK			/* height in lines		*/
	WORD	bi_x       GPK			/* source x in bit form		*/
	WORD	bi_y       GPK			/* source y in bit form		*/
	WORD	bi_color   GPK		/* fg color of blt 		*/
} BITBLK  GPK


typedef struct user_blk
{
	LONG	ub_code   GPK
	LONG	ub_parm   GPK
} USERBLK __attribute__((packed))   GPK

typedef struct appl_blk
{
	LONG	ab_code   GPK
	LONG	ab_parm   GPK
} APPLBLK __attribute__((packed))   GPK

typedef struct parm_blk
{
	LONG	pb_tree        GPK
	WORD	pb_obj         GPK
	WORD	pb_prevstate   GPK
	WORD	pb_currstate   GPK
	WORD	pb_x, pb_y, pb_w, pb_h       GPK
	WORD	pb_xc, pb_yc, pb_wc, pb_hc   GPK
	LONG	pb_parm                      GPK
} PARMBLK  GPK


typedef struct memform
{
    LPVOID	mp    GPK	/* Note: LINEAR address if the MFDB is in our */
    WORD	fwp   GPK	/*      address space (which it usually is). SEG:OFF */
    WORD	fh    GPK	/*      address if the MFDB is in GEM's address space */
    WORD	fww   GPK
    WORD	ff    GPK
    WORD	np    GPK
    WORD	r1    GPK
    WORD	r2    GPK
    WORD	r3    GPK
} MFDB  GPK


typedef struct patarray
{
    WORD	patword[16]   GPK

} FILLPAT  GPK

						/* used in RSCREATE.C	*/
typedef struct rshdr
{
	WORD	rsh_vrsn	GPK	/* File version */
	WORD	rsh_object	GPK	/* list of object trees */
	WORD	rsh_tedinfo	GPK	/* list of TEDINFOS */
	WORD	rsh_iconblk	GPK	/* list of ICONBLKS */
	WORD	rsh_bitblk	GPK	/* list of BITBLKS */
	WORD	rsh_frstr	GPK
	WORD	rsh_string	GPK
	WORD	rsh_imdata	GPK	/* image data */
	WORD	rsh_frimg	GPK
	WORD	rsh_trindex	GPK
	WORD	rsh_nobs	GPK	/* counts of various structs	*/
	WORD	rsh_ntree	GPK
	WORD	rsh_nted	GPK
	WORD	rsh_nib		GPK
	WORD	rsh_nbb		GPK
	WORD	rsh_nstring	GPK
	WORD	rsh_nimages	GPK
	WORD	rsh_rssize	GPK	/* total bytes in resource	*/
} RSHDR GPK


/* [JCE 25 May 1999] Mouse event structure for ev_evnt() */

typedef struct mevent
{
  UWORD e_flags;
  UWORD e_bclk;
  UWORD e_bmsk;
  UWORD e_bst;
  UWORD e_m1flags;
  GRECT e_m1;
  UWORD e_m2flags;
  GRECT e_m2;
  WORD FAR *e_mepbuf;
  ULONG e_time;
  WORD  e_mx;       
  WORD  e_my;
  UWORD e_mb;
  UWORD e_ks;
  UWORD e_kr;
  UWORD e_br;
  UWORD e_m3flags;
  GRECT e_m3;
  WORD  e_xtra0;
  WORD FAR *e_smepbuf;
  ULONG e_xtra1;
  ULONG e_xtra2;
} MEVENT;

typedef MEVENT FAR *LPMEV;


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
	WORD	cc_foreground	GPK
	WORD	cc_background	GPK
	WORD	cc_style	GPK
	WORD	cc_pattern	GPK
} CLRCAT	GPK

typedef struct x_buf_v1
{
	WORD	buf_len GPK	/* Size of structure, so it can grow. 
				 * Set by calling application. */
	WORD	arch 	GPK	/* 16 for 16-bit, 32 for hypothetical 
				 * 32-bit version. */
	LONG    cc 	GPK	/* Address of an array of 16 CLRCAT 
				 * structures.
                               	 * This is so that they can be read by
                               	 * a program without having to fiddle
                               	 * with ViewMAX.INI */
	LONG	w_active GPK	/* Address of the tree (NUM_ELEM elements)
                                 * that is used to draw window frames and 
                                 * controls. The intention is that the 
                                 * glyphs on the buttons could be changed 
                                 * by a program. */
	LONG	info GPK	/* ASCII information such as a version 
				 * string. This should be max. 40 characters 
				 * with no newlines */
	LONG	abilities GPK	/* AES abilities: see ABLE_* below */
				 
} X_BUF_V2 GPK

#define ABLE_GETINFO  1		/* bit 0 : xapp_getinfo() supported */
#define ABLE_PROP     2		/* bit 1 : prop_get(), prop_put() supported */
#define ABLE_WTREE    4		/* bit 2 : wind_set() can change glyphs */
#define ABLE_X3D      8		/* bit 3 : GEM/5 3D using DRAW3D */
#define ABLE_XSHL    16		/* bit 4 : xshl_getshell() & xshl_putshell() */
#define ABLE_PROP2   32		/* bit 5 : prop_gui_get(), prop_gui_set() */
#define ABLE_EMSDESK 64		/* bit 6 : xgrf_dtimage() supports EMS */
#define ABLE_XBVSET 128         /* bit 7 : supports 32 disc drives */
/* >> JCE */





#undef GPK
#ifdef _cplusplus
#pragma pack()
#endif


#define EDSTART 0
#define EDINIT 1
#define EDCHAR 2
#define EDEND 3

#define TE_LEFT 0
#define TE_RIGHT 1
#define TE_CNTR 2

/* JCE 8 Jan 1998 ViewMAX UI elements (for the xgrf_colour command)
 *
 * These have been worked out by executing xgrf_colour() with different
 * parameters and seeing what changed colour.
 *
 */
#define VMAX_ATITLE	 8	/* Active titlebar */
#define	VMAX_SLIDER	 9	/* Slider (scrollbar) */
#define VMAX_DESK	10	/* Desktop */
#define	VMAX_BUTTON	11	/* Buttons */
#define	VMAX_INFO	12	/* Info bar (giving free space on drive) */
#define VMAX_ALERT  	13	/* Alert box */
#define	VMAX_ITITLE	14	/* Inactive titlebar */

/* Official ViewMAX names for the above parameters */
#define	CC_NAME		8
#define	CC_SLIDER	9
#define	CC_DESKTOP	10
#define	CC_BUTTON	11
#define	CC_INFO		12
#define	CC_ALERT	13
#define	CC_SLCTDNAME	14

#define CC_3DSHADE	16
#define CC_RADIO	17
#define CC_CHECK	18

/* >> JCE */


/* Desktop Image Modes for ViewMAX/3 */
#define DT_CENTER	1
#define DT_TILE		2

