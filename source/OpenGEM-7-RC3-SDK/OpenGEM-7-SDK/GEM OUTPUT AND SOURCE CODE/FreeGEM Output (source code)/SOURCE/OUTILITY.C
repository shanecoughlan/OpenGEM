/*******************************************************************/
/*                                                                 */
/*      Copyright 1999, Caldera Thin Clients, Inc.                 */
/*      This software is licenced under the GNU Public License.    */
/*      Please see LICENSE.TXT for further information.            */
/*                                                                 */
/*                 Historical Copyright                            */
/*******************************************************************/
/* Copyright (c) 1987 Digital Research Inc.		           */
/* The software contained in this listing is proprietary to        */
/* Digital Research Inc., Pacific Grove, California and is         */
/* covered by U.S. and other copyright protection.  Unauthorized   */
/* copying, adaptation, distribution, use or display is prohibited */
/* and may be subject to civil and criminal penalties.  Disclosure */
/* to others is prohibited.  For the terms and conditions of soft- */
/* ware code use refer to the appropriate Digital Research         */
/* license agreement.						   */
/*******************************************************************/
 
#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "gembind.h"
#include "vdibind.h"
#include "dosbind.h"
#include "xgembind.h"
#include "rsrclib.h"

#include "output.h"
#include "odefs.h"
#include "ovar.h"

EXTERN BOOLEAN	rc_intersect() ;		/* ooptimz.c */
EXTERN VOID	r_set() ;
EXTERN VOID	strcpy() ;
EXTERN VOID	strcat() ;
EXTERN WORD	strlen() ;
EXTERN BOOLEAN 	strcmp() ;
EXTERN WORD	min() ;
EXTERN WORD	max() ;

EXTERN VOID	send_redraw() ;			/* owindow.c */

EXTERN VOID	b_stuff() ;			/* util86.asm */

#if I8086
/*--------------------------------------------------------------*/
	LONG
dos_avail()
{
LONG	mlen ;

    DOS_AX = 0x4800 ;
    DOS_BX = 0xffff ;
    __DOS() ;
    mlen = ( (LONG)(DOS_BX-1) ) << 4 ;
    return( mlen ) ;
} /* dos_avail */
#endif

/*--------------------------------------------------------------*/
	VOID
vopen( first )
BOOLEAN	first ;
{
    WORD work_out[ 57 ], work_in[ 11 ] ;
    WORD i ;
    WORD	minADE, maxADE, distances[ 5 ], maxwidth, effects[ 3 ] ;

    for ( i=0 ; i<10 ; i++ )
	work_in[ i ]=1 ;
    work_in[ 10 ]=2 ;
    if ( first )
	{
	gem_hndl = graf_handle( &gl_wcell, &gl_hcell, &gl_wbox, &gl_hbox ) ;
	}
    scr_hndl = gem_hndl ;
    v_opnvwk( work_in, &scr_hndl, work_out ) ;
    if ( first )
	{
    	vqt_font_info( scr_hndl, &minADE, &maxADE, distances, 
							&maxwidth, effects);
	gl_baseoff = gl_hcell - distances[ 0 ] ;
	}
} /* vopen */

/*--------------------------------------------------------------*/
	WORD
align_h( h )
WORD 	h ;
{
    return( ( ( h - 2*gl_hbox)/ gl_hcell ) * gl_hcell + (2 * gl_hbox) ) ;
} /* align_h */

/*--------------------------------------------------------------*/
	WORD
abs(num)
	WORD num;
{
	return((num < 0) ? -num : num);
} /* abs */


/*--------------------------------------------------------------*/
/* moves num bytes from s to d.					*/
	VOID
movb(num, ps, pd)
	WORD		num;
	BYTE		*ps, *pd;
{
    if ( num > 0 )
	{
	do
	  *pd++ = *ps++;
	while ( --num ) ;
	}
} /* movb */

/*--------------------------------------------------------------*/
/* returns TRUE iff file1 and file2 have the same extension.	*/
/* Also, if file2 ends with no extension, file1's extension is  */
/* copied onto file2.						*/
	BOOLEAN
extension( file1, file2 )
	BYTE *file1, *file2 ;
{
BYTE	*p1, *p2 ;
WORD	ii ;

    p1 = &file1[ pos( PERIOD, file1 ) ] ;   /* searches from end to beginning */
    ii = pos( PERIOD, file2 ) ;   
    if ( ii >= 0 )
    	p2 = &file2[ ii ] ;   
    else 
	p2 = &file2[ strlen( file2 ) ] ;
    if ( ( *p2 == NULL) || ( *(p2+1) == NULL) ) /* no ext on second name */
	strcpy( p1, p2 ) ;
    return( strcmp( p1, p2 ) );
} /* extension */

/*--------------------------------------------------------------*/
/* returns TRUE iff file1 and file2 have the same extension.	*/
	BOOLEAN
comp_ext( file1, file2 )
	BYTE *file1, *file2 ;
{
BYTE	*p1, *p2 ;
WORD	ii ;

    ii = pos( PERIOD, file2 ) ;   
    if ( ii < 0 )
	return( FALSE ) ;
    p2 = &file2[ ii ] ;   
    p1 = &file1[ pos( PERIOD, file1 ) ] ;   /* searches from end to beginning */
    return( strcmp( p1, p2 ) );
} /* comp_ext */


/*--------------------------------------------------------------*/
    VOID
new_ext( file, ext )
	BYTE *file, *ext ;
{
    WORD	p1, p2 ;

    p1 = pos( PERIOD, file ) ;
    p2 = pos( PERIOD, ext ) ;
    strcpy( &ext[ p2 ], &file[ p1 ] ) ;
} /* new_ext */


/*--------------------------------------------------------------*/
/* returns value clamped to between minimum and maximum.	*/
	WORD
minmax( minimum, value, maximum)
	WORD minimum, value, maximum;
{
	return ( min( max( minimum, value), maximum) );
} /* minmax */


/*--------------------------------------------------------------*/
    VOID
bld_dir(dir, wc, dir_wc)
	BYTE *dir, *wc;
	BYTE *dir_wc;
{
	strcpy(dir, dir_wc);
	strcat(wc, dir_wc);
} /* bld_dir */

/*----------------------------------------------------------------------*/
/* returns 1 if drive id is for hard disk, -1 if floppy, and 0 if invalid */
    WORD
valid_drive( drive_id )
BYTE	drive_id ;
{
WORD	mask ;

    for ( mask = 0x8000; drive_id > UCASE_A; drive_id-- )
        mask >>= 1 ;
    if ( mask & global.ap_harddrives )
        return( 1 ) ;
    if ( mask & global.ap_drives )
        return( -1 ) ;
    return( 0 ) ;
} /* valid_drive() */

/*----------------------------------------------------------------------*/
/* remove path from filename, return filename minus disk and dir.	*/
    VOID
del_path( name, name_only )
    BYTE	*name, *name_only ;
{
    UWORD	ii ;
    BOOLEAN	ok ;

    ii = strlen( name ) ;
    ok = TRUE ;
    while ( ok )
	{
	--ii ;
	if ( ii == 0 ) 		/* no path found */
	    ok = FALSE ;
	if ( ok )
	    {
	    if ( (name[ ii ] == COLON) || (name[ ii ] == BSLASH) ) 
	    	{ 
	    	++ii ;
	    	ok = FALSE ;
	    	}
	    } /* if ok */
	} /* while ok */
    strcpy( &name[ ii ], name_only ) ;
} /* del_path */

/*----------------------------------------------------------------------*/
/* remove filename from path, return disk and dir minus filename. 	*/
    VOID
del_fname( name, path_only )
    BYTE	*name, *path_only ;
{
    UWORD	ii ;
    BOOLEAN	ok ;

    strcpy( name, path_only ) ;
    ii = strlen( path_only ) ;
    ok = TRUE ;
    while ( ok )
	{
	--ii ;
	if ( ii == 0 ) 		/* no path found */
	    ok = FALSE ;
	if ( ok )
	    {
	    if ( (path_only[ ii ] == COLON) || (path_only[ ii ] == BSLASH) ) 
	    	{ 
	    	++ii ;
	    	ok = FALSE ;
	        }
	    } /* if ok */
	} /* while ok */
    path_only[ ii ] = NULL ;
} /* del_fname */

/*--------------------------------------------------------------*/
/* returns TRUE iff p2 wholly contained by p1.			*/
	WORD
rc_inside(p1, p2)
	GRECT *p1, *p2;
{
    return( ( p2->g_x >= p1->g_x ) && 
	    ( p2->g_y >= p1->g_y ) &&
	    ( p2->g_x + p2->g_w <= p1->g_x + p1->g_w ) && 
	    ( p2->g_y + p2->g_h <= p1->g_y + p1->g_h ) ) ;
} /* rc_inside */

/*--------------------------------------------------------------*/
	WORD
rc_equal(p1, p2)
    WORD	*p1, *p2;
{
WORD i ;

    for ( i=0 ; i<4; i++ )
	{
	if ( *p1++ != *p2++ )
	   return( FALSE );
	}
    return( TRUE ) ;
} /* rc_equal */


/*--------------------------------------------------------------*/
    VOID
reverse( s )
	BYTE s[];
{
	WORD c, i, j;

	for(i=0, j=strlen(s)-1; i<j; i++, j--)
	{
	    c = s[i];
	    s[i] = s[j];
	    s[j] = c;
	}
} /* reverse */

/*--------------------------------------------------------------*/
/* converts integer to ascii: s[] had best be long enough.	*/
    VOID
itoa( i, s )
	WORD i;
	BYTE s[];
{
	WORD cnt, sign;

	if ( ( sign = i ) < 0 )
	    i = -i ;
	cnt = 0 ;
	do
	{
	    s[ cnt++ ] = i % 10 + '0' ;
	} while ( (i /= 10 ) > 0 ) ;
	if ( sign < 0 )
	    s[ cnt++ ] = '-' ;
	s[ cnt ] = NULL ;
	reverse( s ) ;
} /* itoa */

/*--------------------------------------------------------------*/
/* pads strings out to n places. For example, if n = 3, this	*/
/* function returns "100" for 100, "050" for 50 and "000" for 0.*/
    VOID
pad_n_itoa( n, w, s )
WORD n, w ;
BYTE *s ;
{
WORD	factor, ii ;
BYTE	*dst, *src ;

    factor = 1 ;
    for ( ii = 1; ii < n; ii++ )
        factor *= 10 ;
    itoa( w, s ) ;
    while ( factor > 1 )
        {
	if ( factor > w )
	    {		/* shift chars one place to the right and add '0' */
	    dst = s + strlen( s ) ;	/* start at one past end of string */
	    *( dst + 1 ) = NULL ;	/* new end of string is set to NULL */
	    src = dst - 1 ;		/* last char in string */
	    while ( src >= s )
		*dst-- = *src-- ;
	    *s = ZERO ;
	    }
	factor /= 10 ;
	}
} /* pad_n_itoa */

/*--------------------------------------------------------------*/
/* converts ascii to integer.					*/
	WORD
atoi(s)
	BYTE s[];
{
	WORD i, n, sign;

	for(i=0; s[i]==SP||s[i]=='\n'||s[i]=='\t'; i++)
	    ;
	sign = 1;
	if(s[i]=='+' || s[i]=='-')
	    sign = (s[i++]=='+') ? 1 : -1;
	for(n=0; s[i] >= '0' && s[i] <= '9'; i++)
	    n = 10 * n + s[i] - '0';
	return(sign*n);
} /* atoi */

/*--------------------------------------------------------------*/
/* takes strings in the form "123.45" and returns 100x integer  */
/* equivalent.							*/
	UWORD
dec_to_int( ptr )
FBYTE	*ptr ;
{
WORD	ii, factor ;
BYTE	num[ 6 ] ;

    factor = 100 ;
    ii = 0 ;
    while ( *ptr == SP || *ptr == BEG_PAREN || 
    			*ptr == LCASE_X || *ptr == UCASE_X )
        ++ptr ;
    while ( *ptr != PERIOD && *ptr != COMMA && *ptr != LCASE_X 
    		&& *ptr != UCASE_X && *ptr != END_PAREN && *ptr != SP )
	num[ ii++ ] = *ptr++ ;
    if ( *ptr == PERIOD || *ptr == COMMA )
        {
	++ptr ;
    	while ( *ptr != LCASE_X && *ptr != UCASE_X 
				&& *ptr != END_PAREN && *ptr != SP )
	    {
	    factor /= 10 ;
	    num[ ii++ ] = *ptr++ ;
	    }
	}
    num[ ii ] = NULL ;
    factor = max( 1, factor ) ;
    return( factor * (UWORD)(atoi( num ) ) ) ;
} /* dec_to_int() */

/*--------------------------------------------------------------*/
	WORD
align_x(x)		/* forces word alignment */
	WORD x;
{
	return((x & 0xfff0) + ((x & 0x000c) ? 0x0010 : 0));
} /* align_x */


/*--------------------------------------------------------------*/
/* swaps longs a and b.						*/
	VOID
lswap( a, b )
	LONG	*a, *b ;
{
	LONG	t ;

    t = *a ;
    *a = *b ;
    *b = t ;
} /* lswap */


/*--------------------------------------------------------------*/
/* swaps words a and b.						*/
	VOID
wswap( a, b )
	WORD	*a, *b ;
{
	WORD 	t ;

    t = *a ;
    *a = *b ;
    *b = t ;
} /* wswap */


/*--------------------------------------------------------------*/	
/* swaps word pointers a and b.					*/
    VOID
pswap( a, b )
	WORD	**a, **b ;
{
	WORD 	*t ;

    t = *a ;
    *a = *b ;
    *b = t ;
} /* pswap */

/*--------------------------------------------------------------*/
/* Returns position of character c in string s, in the range	*/
/* [0..strlen(s)-1]. Returns -1 if character not found.		*/
    WORD
pos( c, s ) 
    BYTE	c, *s ;
{
WORD	i ;
   
    i = strlen( s ) ;
    while ( i )
	{
	i-- ;
	if ( c == s[ i ] )
	    return( i ) ;
	}
    return( (-1) ) ;
} /* pos */


/*--------------------------------------------------------------*/
/* return string s with spaces removed.				*/
    VOID
rm_spaces( s ) 
    BYTE *s ;
{
BYTE	*i, *p ;
   
    p = i = s ;
    while ( *p )
	{
	if ( *p != SP )
	    {
	    *i = *p ;
	    i++ ;
	    }
	p++ ;
	}
    *i = NULL ;
} /* rm_spaces */


/*--------------------------------------------------------------*/
/* called from o_trayut.c and o_dialog.c.			*/
	WORD
get_name( old_wc, new_wc, new_file )
	BYTE *old_wc, *new_wc, *new_file ;
{
	LONG d_addr, f_addr ;
	WORD okcan ;

	new_file[ 0 ] = NULL ;
	strcpy( old_wc, new_wc ) ;
	d_addr = ADDR( new_wc ) ;
	f_addr = ADDR( new_file ) ;
	fsel_input( d_addr, f_addr, &okcan ) ;
	return( okcan != CANCEL ) ;
} /* get_name */

get_name_xtnd(old_wc, new_wc, new_file, title)
	BYTE *old_wc, *new_wc, *new_file, *title;
{
#ifdef FREEGEM_STUFF
	LONG d_addr, f_addr ;
	WORD okcan ;
	
	if (!FreeGEM)
 	   return(get_name(old_wc, new_wc, new_file));

	new_file[ 0 ] = NULL ;
	strcpy( old_wc, new_wc ) ;
	d_addr = ADDR( new_wc ) ;
	f_addr = ADDR( new_file ) ;
	fsel_exinput( d_addr, f_addr, &okcan, title ) ;
	return( okcan != CANCEL ) ;
#else
	return(get_name(old_wc, new_wc, new_file));
#endif
}

/*--------------------------------------------------------------*/
    VOID
sidebar_ienable( item, enable, str0_item, str1_item )
WORD	item ;
BOOLEAN	enable ;
WORD	str0_item, str1_item ;
{
WORD	old_flags, new_flags, new_strst, old_strst, item_state ;
WORD	x, y, w, h ;
GRECT	r1, r2 ;

    new_flags = enable ? SELECTABLE : NONE ;
    old_flags = get_flags( cntl_tr, item ) ;
    new_strst = enable ? NORMAL : DISABLED ;
    old_strst = ( item == STRTITEM ) ? new_strst : get_state( cntl_tr, str0_item ) ;
    item_state = get_state( cntl_tr, item ) ;
    if ( ( old_flags != new_flags ) || ( new_strst != old_strst )
			|| ( SELECTED & item_state ) )
	{
	set_state( cntl_tr, item, NORMAL ) ; /* need to unselect if selected */
	set_flags( cntl_tr, item, new_flags ) ;
	if ( item != STRTITEM )		
	    {
	    set_state( cntl_tr, str0_item, new_strst ) ;
	    set_state( cntl_tr, str1_item, new_strst ) ;
	    }
	get_obloc( cntl_tr, item, &x, &y, &w, &h ) ;
	r_set( &r1, x-1, y-1, w+2, h+2 ) ;
	r_set( &r2, gl_win.g_x, gl_work.g_y, gl_win.g_w, gl_work.g_h ) ;
	if ( rc_intersect( &r1, &r2 ) )	
	    send_redraw( &r2.g_x ) ;
	}
} /* sidebar_ienable */


/*--------------------------------------------------------------*/
    WORD
f_alert( tree_num, def_but )
WORD	tree_num, def_but ;
{
LONG al_addr ;

    rsrc_gaddr( GAD_STRING, tree_num, &al_addr ) ;
    return( form_alert( def_but, al_addr ) ) ;
} /* f_alert */


#if DEBUG
#define MX_MESG_LEN	40
/*----------------------------------------------------------------------*/
/* b comes in the range [0..15d] and is returned in the range [30..39h] */
/* and [41..46h] for the ascii characters [`0'..`F'].			*/
    VOID
i_to_hex( b )
BYTE	*b ;
{
    if ( ( *b < 0 ) || ( *b > 15 ) )
	*b = ASTERISK ;	
    else
	{
	if ( *b < 10 )
	    *b += '0' ;
	else 
	    *b += ('A' - 10) ;
	}
} /* i_to_hex */


/*----------------------------------------------------------------------*/
/* takes a string of the form ["hello"] and returns the string ["68.65. */
/* 6c.6c.6f.00.00.00.00....."].	Notice the new string is three times the*/
/* length of the original.						*/
    VOID
byte_to_hex( b )
BYTE	*b ;
{
BYTE	nib1, nib2, bite ;
WORD	ii ;
BYTE	tmp[ MX_MESG_LEN ] ;

    b_stuff( ADDR( tmp ), MX_MESG_LEN, PERIOD ) ;
    for ( ii = 0; 3*ii < MX_MESG_LEN; ii++ )
	{
	bite = b[ ii ] ;
	nib1 = bite >> 4 ;
	nib2 = bite & 0x0F ;
	i_to_hex( &nib1 ) ;
  	i_to_hex( &nib2 ) ;
	tmp[ ii*3 ] = nib1 ;
	tmp[ ii*3 + 1 ] = nib2 ;
	}
    tmp[ MX_MESG_LEN -1 ] = NULL ;
    strcpy( tmp, b ) ;
} /* byte_to_hex */

/*----------------------------------------------------------------------*/
/* puts up a form alert with the 'stop' icon and one exit button.	*/
    VOID
do_falert( line1, line2, line3, line4, line5 )
BYTE	*line1, *line2, *line3, *line4, *line5 ;
{
BYTE	tmp[ 256 ] ;

    strcpy( "[3][" , tmp ) ;
    strcat( line1, tmp ) ;
    strcat( "|", tmp ) ;
    strcat( line2, tmp ) ;
    strcat( "|", tmp ) ;
    strcat( line3, tmp ) ;
    strcat( "|", tmp ) ;
    strcat( line4, tmp ) ;
    strcat( "|", tmp ) ;
    strcat( line5, tmp ) ;
    strcat( "][ Exit ]" , tmp ) ;
    form_alert( 1, ADDR( tmp ) ) ;
} /* do_falert */


/*----------------------------------------------------------------------*/
    VOID
show_hex( mesg )
BYTE	*mesg ;
{
BYTE	lines[ 5 ][ MX_MESG_LEN ] ;
WORD	inc, ii ;

    b_stuff( ADDR( lines ), sizeof( lines ), NULL ) ;
    inc = MX_MESG_LEN / 3 ;
    for ( ii = 0; ii < 5; ii++ )
	{
	movb( inc, &mesg[ ii*inc ], &lines[ ii ] ) ;
	byte_to_hex( &lines[ ii ] ) ;
	}
    do_falert( &lines[ 0 ], &lines[ 1 ], &lines[ 2 ], &lines[ 3 ], &lines[ 4 ] ) ;
} /* show_hex */

#endif

/*--------------------------------------------------------------*/
/* "FILENAME123" becomes "FILENAME.123", "FILE<sp><sp><sp><sp>123" */
/* becomes "FILE.123", and "FILENAME12" becomes "FILENAME.12"	*/
    VOID
etext_to_fname( etext, fname )
FBYTE	*etext, *fname ;
{
FBYTE	*dst ;
WORD	ii ;

    b_stuff( fname, NAME_LENGTH, NULL ) ;    
    for ( ii = 0, dst = fname; ii < 11; ii++ )	/* get filename */
        {
	if ( ii == 8 )
	    *dst++ = PERIOD ;		/* start extension */
	if ( etext[ ii ] != SP )	/* remove imbedded spaces */
	    *dst++ = etext[ ii ] ;
	}
} /* etext_to_fname() */

/*--------------------------------------------------------------*/
/* "FILENAME.123" becomes "FILENAME123", "FILE.123" becomes	*/
/* "FILE<sp><sp><sp><sp>123" and "FILENAME.12" becomes 		*/
/* "FILENAME12"							*/
    VOID
fname_to_etext( fname, etext )
FBYTE	*fname, *etext ;
{
FBYTE	*src, *dst ;

    b_stuff( etext, 11, SP ) ;    
    for ( src = fname, dst = etext; *src != NULL; src++, dst++ )
        {
	if ( *src != PERIOD )
	    *dst = *src ;
	else
	    dst = ( etext + 7 ) ;	/* last char before extension */
	}
} /* fname_to_etext() */

/* end of oututil.c */


