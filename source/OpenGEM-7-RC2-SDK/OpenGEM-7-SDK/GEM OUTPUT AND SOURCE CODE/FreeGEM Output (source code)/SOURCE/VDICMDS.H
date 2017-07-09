/* VDIOPCOD.H :		op codes for vdi functions	*/

#define     OPENCMD		1 
#define     CLOSECMD		2 
#define     CLEARCMD        	3 
#define     UPDATECMD		4 
#define     ESCAPECMD		5 
			/* output prims */
#define     PLINECMD		6 
#define     PMARKERCMD		7 
#define     TEXTCMD         	8 
#define     FILAREACMD		9 
#define     CELLARRCMD		10 
#define     GDPCMD		11 

			/* attributes */
#define     TEXTHGTCMD		12 
#define     TEXTUPCMD		13 
#define     COLREPCMD		14 
#define     PLINTYPCMD      	15 
#define     PLINWIDCMD		16 
#define     PLINCOLCMD		17 
#define     PMRKTYPCMD		18 
#define     PMRKSCLCMD		19 
#define     PMRKCOLCMD		20 
#define     FONTCMD		21 
#define     TEXTCOLCMD		22 
#define     FILSTYLCMD		23 
#define     FILINDXCMD		24 
#define     FILCOLCMD		25 
#define     INQCOLCMD		26 
#define     INQCELCMD		27 
#define     INPLOCCMD		28 
#define     INPVALCMD		29 
#define     INPCHOCCMD		30 
#define     INPSTRCMD		31 
#define     WRITMODCMD		32 
#define     INPMODECMD		33 
#define     METAWRITCMD		99
#define	    OPENVIRTUAL		100
#define	    CLOSEVIRTUAL	101
#define     EXTENDEDINQ		102 
#define	    CONTOURFILL		103
#define	    TEXTEFFCMD		106
#define     PTS_CHAR_HGT	107
#define     PLINENDCMD		108 
#define     FILLRECTCMD		114 
#define     SHWCURSCMD		122 
#define     HIDCURSCMD		123 
#define     SETCLIPCMD		129 

				/* GDP id numbers */
#define     CIRCLE		4
#define     ARC			2
#define     PIE			3
#define     ELLIPSE		5
#define     ELLIP_ARC		6
#define     ELLIP_PIE		7
#define     LINRRECTCMD		8 
#define     FILRRECTCMD		9 
#define     FORMT_TEXT		10

				/* ESCAPE sub-opcodes */
#define	    BITIMAGE		0x17
#define	    ALPHATEXT		0x19
#define	    IMGFCMD		23		/* same as BITIMAGE, above */
#define	    XIMGFCMD		26		/* rotated bit image */

				/* writing modes */
#define     REPLACE		1 
#define     TRANSPARENT		2 
#define     COMPLEMENT    	3 
#define     ERASE		4 

#define     HOLLOW		0
#define     SOLID		1
#define	    PATTERN		2
#define	    HATCH		3

				/* sub opcodes for v_metawrite() cmd */
#define	    PHYSPGSIZE		0
#define	    COORDRANGE		1

				/* user def sub opcodes for v_metawrite() */
#define	    BEGGRPCMD		10
#define     ENDGRPCMD		11
#define     SETNOLNSTY		49
#define     SHADONCMD		50
#define     SHADOFFCMD		51
#define     BEGPRIMCMD		80
#define	    ENDPRIMCMD		81
