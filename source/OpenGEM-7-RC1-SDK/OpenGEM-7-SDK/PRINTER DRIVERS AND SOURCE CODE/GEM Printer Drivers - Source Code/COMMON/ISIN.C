/*************************************************************************
**       Copyright 1999, Caldera Thin Clients, Inc.                     ** 
**       This software is licenced under the GNU Public License.        ** 
**       Please see LICENSE.TXT for further information.                ** 
**                                                                      ** 
**                  Historical Copyright                                ** 
**									**
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

#include "portab.h"

#define    HALFPI    900 
#define    PI	     1800
#define    TWOPI     3600

static	WORD	sin_tbl[92] =
		{    
		    0 , 572  , 1144 , 1716 , 2286 , 2856 , 3425 , 3993 , 
		 4560 , 5126 , 5690 , 6252 , 6813 , 7371 , 7927 , 8481 , 
		 9032 , 9580 ,10126 ,10668 ,11207 ,11743 ,12275 ,12803 ,
		13328 ,13848 ,14364 ,14876 ,15383 ,15886 ,16383 ,16876 ,
		17364 ,17846 ,18323 ,18794 ,19260 ,19720 ,20173 ,20621 ,
		21062 ,21497 ,21925 ,22347 ,22762 ,23170 ,23571 ,23964 ,
		24351 ,24730 ,25101 ,25465 ,25821 ,26169 ,26509 ,26841 ,
		27165 ,27481 ,27788 ,28087 ,28377 ,28659 ,28932 ,29196 ,
		29451 ,29697 ,29934 ,30162 ,30381 ,30591 ,30791 ,30982 ,
		31163 ,31335 ,31498 ,31650 ,31794 ,31927 ,32051 ,32165 ,
		32269 ,32364 ,32448 ,32523 ,32587 ,32642 ,32687 ,32722 ,
		32747 ,32762 ,32767 ,32767 
		};

WORD	isin( angle )
WORD angle;
{
  	WORD  index, quadrant, remainder, tmpsin;

  	while (angle > 3600) 
    	  angle -= 3600;
  	quadrant = angle / HALFPI;
  	switch ( quadrant )
    	{
      		case 0:
        	  break;

      		case 1:
		  angle = PI - angle;
		  break;
      
      		case 2:
		  angle -= PI;
		  break;

		case 3:
        	  angle = TWOPI - angle;
		  break;

      		case 4:
        	  angle -= TWOPI;
		  break;
    	};
    	index = angle / 10;
	remainder = angle % 10;
    	tmpsin = sin_tbl[ index ];
    	if (remainder)
	    tmpsin += ((sin_tbl[ index + 1 ] - tmpsin) * remainder) / 10;
    	if (quadrant > 1) 
	    tmpsin = -tmpsin;
    	return ( tmpsin );
	
}  /* end of isin() */


WORD	icos(angle) 
WORD angle;
{
  	if ( (angle += HALFPI) > TWOPI)
	    angle -= TWOPI;
  	return(isin(angle));
	
}  /* end of icos() */
