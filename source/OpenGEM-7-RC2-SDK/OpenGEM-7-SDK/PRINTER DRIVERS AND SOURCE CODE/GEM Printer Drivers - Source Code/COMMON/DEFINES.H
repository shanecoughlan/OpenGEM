/*************************************************************************
**       Copyright 1999, Caldera Thin Clients, Inc.                     ** 
**       This software is licenced under the GNU Public License.        ** 
**       Please see LICENSE.TXT for further information.                ** 
**                                                                      ** 
**                  Historical Copyright                                ** 
**									**
**  Copyright (c) 1987, Digital Research, Inc. All Rights Reserved.	**
**  The Software Code contained in this listing is proprietary to	**
**  Digital Research, Inc., Monterey, California and is covered by U.S.	**
**  and other copyright protection.  Unauthorized copying, adaptation,	**
**  distribution, use or display is prohibited and may be subject to 	**
**  civil and criminal penalties.  Disclosure to others is prohibited.	**
**  For the terms and conditions of software code use refer to the 	**
**  appropriate Digital Research License Agreement.			**
**									**
*************************************************************************/

/* Device independent equivalences. */
#define xres      dev_tab[0]		/* jmptbl, monout, monoprin */
#define yres      dev_tab[1]		/* jmptbl, monout, monoprin */
#define xsize     dev_tab[3]		/* image, monout, monoprin */
#define ysize     dev_tab[4]		/* image, monout, monoprin */
#define iptscnt   CONTRL[1]		/* monout */
#define optscnt   CONTRL[2]		/* NOT USED IN COMMON */
#define iintcnt   CONTRL[3]		/* NOT USED IN COMMON */
#define ointcnt   CONTRL[4]		/* NOT USED IN COMMON */
#define gdp_code  CONTRL[5]		/* monout */
#define	DEF_CHWT  siz_tab[0]		/* NOT USED IN COMMON */
#define	DEF_CHHT  siz_tab[1]		/* NOT USED IN COMMON */
#define	DEF_LWID  siz_tab[4]		/* monout */
#define DEF_MKWD  siz_tab[8]		/* monout, monoprin */
#define DEF_MKHT  siz_tab[9]		/* monout, monoprin */
#define MAX_MKWD  siz_tab[10]		/* NOT USED IN COMMON */
#define MAX_MKHT  siz_tab[11]		/* monoprin */

/* Device independent enumeration values. */
#define SQUARED 0	/* monout */
#define ARROWED 1	/* monout, monoprin */
#define CIRCLED 2	/* monout */

#define LLUR    0	/* monoprin */
#define ULLR    1	/* monoprin, jmptbl.c */
