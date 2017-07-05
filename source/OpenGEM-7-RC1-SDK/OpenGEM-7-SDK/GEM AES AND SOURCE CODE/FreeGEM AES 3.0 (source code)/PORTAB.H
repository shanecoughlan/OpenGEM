#define mc68k 0
/****************************************************************************
*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright     
*                            
*	  C P / M   C   R U N   T I M E   L I B   H E A D E R   F I L E
*	  -------------------------------------------------------------
*	Copyright 1982 by Digital Research Inc.  All rights reserved.
*
*	This is an include file for assisting the user to write portable
*	programs for C.
*
****************************************************************************/

#ifndef __PACIFIC__
#error This file is set up for the Pacific C compiler
#endif


#define UCHARA 0				/* if char is unsigned     */
/*
 *	Standard type definitions
 */
#define METAWARE   0				/* Metaware High C compiler*/

typedef char         BYTE;			/* Signed byte 		   */
typedef int          BOOLEAN;			/* 2 valued (true/false)   */
typedef int          WORD;			/* Signed 16-bit word      */
typedef unsigned int UWORD;			/* Unsigned word           */

typedef long          LONG;			/* Signed 32-bit long      */
typedef unsigned long ULONG;			/* Unsigned long           */

#define	REG	register			/* register variable	   */
#define	LOCAL	auto				/* Local var on 68000	   */
#define	EXTERN	extern				/* External variable	   */
#define	MLOCAL	static				/* Local to module	   */
#define	GLOBAL	/**/				/* Global variable	   */

#define VOID void
#define FAR  far
#define NEAR near

#ifndef METAWARE
#define	DEFAULT	int				/* Default size		   */
#endif

#if UCHARA
#define UBYTE	char				/* Unsigned byte 	   */
#else
#define	UBYTE	unsigned char			/* Unsigned byte	   */
#endif

/****************************************************************************/
/*	Miscellaneous Definitions:					    */
/****************************************************************************/
#define	FAILURE	(-1)			/*	Function failure return val */
#define SUCCESS	(0)			/*	Function success return val */
#define	YES	1			/*	"TRUE"			    */
#define	NO	0			/*	"FALSE"			    */
#define	FOREVER	for(;;)			/*	Infinite loop declaration   */
#define	NULL	0			/*	Null pointer value	    */
#define NULLPTR (char *) 0		/*				    */
#define	EOF	(-1)			/*	EOF Value		    */
#define	TRUE	(1)			/*	Function TRUE  value	    */
#define	FALSE	(0)			/*	Function FALSE value	    */

/* [JCE] Pacific C far pointer types */
#include "ppdptr.h"

