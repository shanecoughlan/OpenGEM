/************************************************************************
*
*     GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*     Filename: META.H
*
*     Date: 08/01/89
*
*************************************************************************/

/*----- VDI ------------ Opcode ----------------------------------------*/
#define MAX_META_SIZE	 15000L
#define METAFILE_KENNUNG -1
#define MAX_POINTS	 128

/*--------------------------- VDI-Function numbers ---------------------*/
#define	V_OPNWK		  1
#define V_CLSWK		  2
#define V_CLRWK		  3
#define V_UPDWK		  4

#define ESCAPE		  5 

#define VQ_CHCELLS	  1
#define V_EXIT_CUR	  2
#define V_ENTER_CUR	  3
#define V_CURUP		  4
#define V_CURDOWN	  5
#define V_CURRIGHT	  6
#define V_CURLEFT	  7
#define V_CURHOME	  8
#define V_EEOS		  9
#define V_EEOL		  10
#define VS_CURADDRESS	  11
#define V_CURTEXT	  12
#define V_RVON		  13
#define V_RVOFF		  14
#define VQ_CURADDRESS	  15
#define VQ_TABSTATUS	  16
#define V_HARDCOPY	  17
#define V_DSPCUR	  18
#define V_RMCUR		  19
#define V_FORM_ADV	  20
#define V_OUTPUT_WINDOW	  21
#define V_CLEAR_DISP_LIST 22
#define V_BIT_IMAGE	  23
#define VQ_SCAN		  24
#define V_ALPHA_TEXT	  25
#define VS_PALETTE	  60
#define V_META_EXTENTS	  98
#define V_WRITE_META	  99
#define VM_FILENAME	  100

#define V_PLINE		  6
#define V_PMARKER	  7
#define V_GTEXT	 	  8
#define V_FILLAREA	  9
#define V_CELLARRAY	  10

#define GDP		  11

#define V_BAR		  1
#define V_ARC		  2
#define V_PIESLICE	  3
#define V_CIRCLE	  4
#define V_ELLIPSE	  5
#define V_ELLARC	  6
#define V_ELLPIE	  7
#define V_RBOX		  8
#define V_RFBOX		  9
#define V_JUSTIFIED	  10

#define VST_HEIGHT	  12
#define VST_ROTATION	  13
#define VS_COLOR	  14
#define VSL_TYPE	  15
#define VSL_WIDTH	  16
#define VSL_COLOR	  17
#define VSM_TYPE	  18
#define VSM_HEIGHT	  19
#define VSM_COLOR	  20
#define VST_FONT	  21
#define VST_COLOR	  22
#define VSF_INTERIOR	  23
#define VSF_STYLE	  24
#define VSF_COLOR	  25
#define VQ_COLOR	  26
#define VQ_CELLARRAY	  27

#define VRQ_LOCATOR	  28
#define VSM_LOCATOR	  28
#define VRQ_VALUATOR	  29
#define VSM_VALUATOR	  29
#define VRQ_CHOICE	  30
#define VSM_CHOICE	  30
#define VRQ_STRING	  31
#define VSM_STRING	  31
#define VSWR_MODE	  32
#define VSIN_MODE	  33

#define VQL_ATTRIBUTES	  35
#define VQM_ATTRIBUTES	  36
#define VQF_ATTRIBUTES	  37
#define VQT_ATTRIBUTES	  38
#define VST_ALIGNMENT	  39

#define V_OPNVWK	  100
#define V_CLSVWK	  101
#define VQ_EXTND	  102
#define V_CONTOURFILL	  103

#define VSF_PERIMETER	  104
#define VST_BACKGROUND	  105
#define VST_EFFECTS	  106
#define VST_POINT	  107
#define VSL_END_STYLE	  108

#define VRO_CPYFM	  109
#define VR_TRNFM	  110
#define VSC_FORM 	  111
#define VSF_UDPAT	  112
#define VSL_UDSTY	  113
#define VR_RECFL	  114

#define VQI_MODE	  115
#define VQT_EXTENT	  116
#define VQT_WIDTH	  117
#define VEX_TIMV	  118
#define VST_LOAD_FONTS	  119
#define VST_UNLOAD_FONTS  120
#define VRT_CPYFM	  121
#define V_SHOW_C	  122
#define V_HIDE_C	  123
#define VQ_MOUSE	  124
#define VEX_BUTV	  125
#define VEX_MOTV	  126
#define VEX_CURV	  127
#define VQ_KEY_S	  128
#define VS_CLIPP	  129

#define VQT_NAME	  130
#define VQT_FONTINFO	  131

/*-------------------- Patterns -----------------------------------------*/
#define SOLID			1
#define SOLID_PATTERN		0xFFFF
#define LONG_DASH		2
#define LONG_DASH_PATTERN	0xFFF0
#define DOT			3
#define DOT_PATTERN		0xE0E0
#define DASH_DOT		4
#define DASH_DOT_PATTERN	0xFE38
#define DASH			5
#define DASH_PATTERN		0xFF00
#define DASH_DOT_DOT		6
#define DASH_DOT_DOT_PATTERN	0xF198
#define USER_DEFINE		7


/*-------- Definitions for "meta_find_objekts" --------------------------*/

#define	KLICKFELD		1
#define	EINGABE_DIALOGBOX	2
#define	OBJEKT			3

