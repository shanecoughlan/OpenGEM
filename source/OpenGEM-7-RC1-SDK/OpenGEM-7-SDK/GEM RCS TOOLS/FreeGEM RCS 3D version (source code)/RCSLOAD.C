/*	RCSLOAD.C	7/3/85		Tim Oren		*/

/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
#include "ppdrcs.h"

#define CR 0x0D
#define LF 0x0A
#define CTRLZ 0x1A
#define ZERO '0'
#define SMX 'x'
#define SPACE ' '
#define COMMA ','
#define OPENBR '{'
#define CLOSEBR '}'
#define SEMICOLON ';'
#define F_READ 0
#define F_WRITE 1

/*	this module contains the file reading & writing routines	*/
/*	for ICONEDIT.							*/

/* The icon file format is:						*/
/* GEM Icon Definition: 						*/
/*#define ICON_W 0x0000 						*/
/*#define ICON_H 0x0000 						*/
/*#define DATA_SIZE 0x0000 						*/
/*UWORD iconname[DATA_SIZE] = 						*/
/*{ 0x8611, 0x482C, 0x8611, 0x8826, 					*/
/*  0xFF1F, 0xF8FF, 0x0000, 0x0000, 					*/
/*  ...									*/
/*  0xA106, 0xFF00, 0x0000, 0x0000					*/
/*};									*/
/*NOTE: All icon files have the extension .icn				*/
/*NOTE: As of 01/24/85, the written form of an icon has changed. Icons in */	
/*the new format have a comment as their first line. Old icons do not.	*/
/*READ_ICON determines which type of icon is being read; WRIT_ICON only*/
/*writes in the new form.						*/
/*In the written form, both mask & data are inverted and have their bytes*/
/*swapped.								*/



/************************************************************************/
/* j u s t _ n a m e							*/
/************************************************************************/
	VOID
just_name( pname)
	BYTE		 *pname;
{
/* pull the filename off the end of the path	*/
	BYTE		*ptmp;

	ptmp = &icn_file[0];
	while(*ptmp)
	  ptmp++;
	while(*ptmp != '\\')
	  ptmp--;
	ptmp++;
	strcpy(pname, ptmp);
	while(*pname)
	{				/* down-shift if necessary	*/
	  if (*pname >= 'A' && *pname <= 'Z')
	    *pname += 32;
	  pname++;
	} /* while */
} /* just_name */


/************************************************************************/
/* d i g _ t o _ c h a r						*/
/************************************************************************/
	BYTE
dig_to_char(number)
	WORD		number;
{
	BYTE		b_val, result;

	b_val = (BYTE) number;
	result = b_val + 48;
	if ( (result > 57) && (result < 65) )
	  result += 7;
	return(result);
} /* dig_to_char */


/************************************************************************/
/* w r i t e _ i c o n							*/
/************************************************************************/
WORD writ_icon(LPVOID praster, WORD icon_w, WORD icon_h, 
               WORD data_size, BOOLEAN data_field)
{
    WORD		count;
	WORD		ii, jj,  num_bytes;
	WORD		number, factor, digit;
	BYTE		chr, new_name[13];
	BYTE		*ptr;
	FILE		*fp;

	if (data_field) fp = fopen(icn_file, "w");
	else
		{
		fp = fopen(icn_file, "a"); /* go to end of file */
		}
	if (!fp) return FALSE;
	new_name[0] = 0;
	just_name(&new_name[0]);
	for (ii = 0; ii < MAPSIZE; c_obndx[ii++] = '\0');
	ptr = (char *) &c_obndx[0];

	ptr = strcat(ptr, "/* GEM Icon Definition: */");
	ptr--;
	*ptr++ = '\n';
	ptr = strcat(ptr, "#define ICON_W 0x");
	ptr--;
	factor = 12;
	for (jj = 1; jj <= 4; jj++)
	{  
	  digit = (icon_w >> factor) & 0x000F;
	  chr = dig_to_char(digit);
	  *ptr++ = chr;
	  factor -= 4;
	} /* for */
	*ptr++ = '\n';

	ptr = strcat(ptr, "#define ICON_H 0x");
	ptr--;
	factor = 12;
	for (jj = 1; jj <= 4; jj++)
	{  
	  digit = (icon_h >> factor) & 0x000F;
	  chr = dig_to_char(digit);
	  *ptr++ = chr;
	  factor -= 4;
	} /* for */
	*ptr++ = '\n';

	ptr = strcat(ptr, "#define DATASIZE 0x");
	ptr--;
	factor = 12;
	for (jj = 1; jj <= 4; jj++)
	{  
	  digit = (data_size >> factor) & 0x000F;
	  chr = dig_to_char(digit);
	  *ptr++ = chr;
	  factor -= 4;
	} /* for */
	*ptr++ = '\n';

	ptr = strcat(ptr, "UWORD ");
	ptr--;
	ii = 0;
	while (new_name[ii] != '.')
	  *ptr++ = new_name[ii++];
	ptr = strcat(ptr, "[DATASIZE] =");
	ptr--;
	*ptr++ = '\n';
	ptr = strcat(ptr, "{ ");
	ptr--;
	count = 0;
     
	ii = (data_size * 2) / icon_h;
	gsx_untrans(praster, ii, praster, ii, icon_h);

	for (ii = 0; ii < data_size; ii++)
	{

	  *ptr++ = ZERO;				/* '0'		*/
	  *ptr++ = SMX;					/* 'x'		*/

	  number = ((LPWORD)praster)[ii];
		/* now turn it into ASCII	*/
	  factor = 12;
	  for (jj = 1; jj <= 4; jj++)
	  {  
	    digit = (number >> factor) & 0x000F;
	    chr = dig_to_char(digit);
	    *ptr++ = chr;
	    factor -= 4;
	  } /* for */

	  *ptr++ = COMMA;
	  *ptr++ = SPACE;
	  count++;
	  if ( (count % 4) == 0 )
	  {
	    *ptr++ = '\n';
	    ptr = strcat(ptr, "  ");
	    ptr--;
	  } /* if count */
	} /* for ii */

	ii = (data_size * 2 ) / icon_h;
	gsx_trans(praster, ii, praster, ii, icon_h);


	ptr -= 5;
	*ptr++ = '\n';
	*ptr++ = CLOSEBR;
	*ptr++ = SEMICOLON;
	*ptr++ = '\n';
	*ptr = CTRLZ;

	num_bytes = ptr - (char *) &c_obndx[0];
	fwrite(c_obndx, 1, num_bytes, fp);
	fclose(fp);
	return(TRUE);
} /* writ _icon */


/************************************************************************/
/* g e t _ f i l e							*/
/************************************************************************/
VOID get_icnfile(BYTE *full_path)
{
	WORD		butn, ii;
	BYTE		tmp_name[13];
				    
	if( !sav_icnpath[0] )  get_path(full_path, "*.ICN"); 
	else                   strcpy(full_path, sav_icnpath);

	tmp_name[0] = 0;
	fsel_input(ADDR(full_path), ADDR(&tmp_name[0]), &butn);
	if (butn)
	{
	  if ( strcmp( sav_icnpath, full_path ))
		{			  
		strcpy( sav_icnpath, full_path);
		ii = 0;
		while(full_path[ii] != '*')
		   ii++;
       		while( full_path[ii] != '\\' )
		   ii--;		
		full_path[ii] = 0;
		dos_chdir( ADDR(full_path));
		strcat( full_path, "\\" );
		}
	   else
		{
		ii = 0;
		while(full_path[ii] != '*')
		   ii++;	    
		while(full_path[ii] != '\\')
		   ii--;	
		full_path[++ii] = 0;	      
		}
	  strcat(full_path, &tmp_name[0]);
	} /* if butn */
	else
	  full_path[0] = 0;
} /* get_icnfile */

/************************************************************************/
/* c h a r _ t o _ d i g						*/
/************************************************************************/
	WORD
char_to_dig(chr)
	BYTE		chr;
{
	WORD		result;

	result = chr - 48;
	if (result > 9)
	  result -= 7;
	return( (WORD) result );
} /* char_to_dig */

/************************************************************************/
/* g e t _ n u m b e r							*/
/************************************************************************/
	VOID
get_number(ptr, result)
	BYTE		*ptr;
	WORD		*result;
{
	WORD		ii, factor, digit;
	BYTE		chr;

	factor = 12;
	*result = 0x0;
	for (ii = 1; ii <= 4; ii++)
	{  
	  chr = *ptr++;
	  if (chr == 0x0D)
	    return;
	  digit = char_to_dig(chr);
	  *result |= digit << factor;
	  factor -= 4;
	} /* for */
} /* get_number */

/************************************************************************/
/* r e a d _ i c o n							*/
/************************************************************************/
	
LPVOID read_icon(buffer, icon_w, icon_h, data_size, icon_name, mask, icn_size)

	WORD		*icon_w, *icon_h, *data_size, *icn_size;
	BOOLEAN		mask;
	BYTE		*buffer, *icon_name;
{
	LPVOID		praster;
	WORD		count, ii, jj,  bytes_read, new_style;
	WORD		number, factor, digit;
	BYTE		chr, *ptr;
    FILE		*fp;
     

	fp = fopen(icon_name, "rb");
	
	
	if (!fp)
	{
	  *icon_w = -1;
	  return(FALSE);
	} /* if */
	if ( !mask  || *icn_size == -1 )
		bytes_read = dmcopy(fp,0x0L,ADDR(buffer),MAPSIZE);
	else
		bytes_read = dmcopy(fp, (LONG) *icn_size, ADDR(buffer),MAPSIZE);
	if (bytes_read <= 0)
	{
	  *icon_w = -1;
	  *icn_size = -1;	
	  return(FALSE);      
	} /* if */
	ptr = buffer;			/* initialize pointers */

					/* is this an new-style icon?	*/
		if (*ptr == '/')
		{
		  new_style = TRUE;
						/* skip the comment	*/
		  while (*ptr != '#')
		    ptr++;
		} /* if */
		else
		  new_style = FALSE;
						/* get ICON_W		*/
		while (*ptr != '0')
		  ptr++;
		ptr += 2;
		get_number(ptr, icon_w);
		ptr += 4;
						/* get ICON_H		*/
		while (*ptr != '0')
		  ptr++;
		ptr += 2;
		get_number(ptr, icon_h);
		ptr += 4;
						/* get DATASIZE		*/
		while (*ptr != '0')
		  ptr++;
		ptr += 2;
		get_number(ptr, data_size);		/* pick up DATA_SIZE	*/

		praster = get_mem(*data_size * 2);
					/* skip "UWORD filename..."	*/
		while (*ptr != OPENBR)
		  ptr++;
		ptr += 2;				/* skip '{ '		*/

		count = 0;
		for (ii = 0; ii < *data_size; ii++)
		{
		  ptr += 2;					/* skip "0x"	*/
		  factor = 12;
		  number = 0x0;
		  for (jj = 1; jj <= 4; jj++)
		  {  
		    chr = *ptr++;
		    digit = char_to_dig(chr);
		    number |= digit << factor;
		    factor -= 4;
		  } /* for */

		  ((LPWORD)praster)[ii] = number;
		  ptr += 2;					/* skip ", "	*/
		  count++;
		  if ( (count % 4) == 0 )
		    ptr += 4;				/* skip "CRLF  "	*/
		} /* for ii */
		if (new_style)
			{
			ii = (*data_size * 2) / *icon_h;
			gsx_trans(praster, ii, praster, ii, *icon_h);
			}
	*icn_size = ptr - buffer;
	fclose(fp);
	return (praster);
} /* read_icon */

WORD bb_get(LPBIT taddr, WORD *icon_w, WORD *icon_h)
	{
	LPVOID	where;
	WORD	icon_size, file_size;

	get_icnfile(&icn_file[0]);
	file_size = -1;
	if (icn_file[0])
	if (where = read_icon((BYTE *) &c_obndx[0], icon_w, icon_h, 
		&icon_size, &icn_file[0], TRUE ,&file_size ))
		{

		taddr->bi_pdata = where;
		taddr->bi_hl    = *icon_h;
		taddr->bi_wb    = 2 * icon_size / (*icon_h);
		icn_state = FILE_STATE;
		return (TRUE);
		}
	return(FALSE);
	}


WORD ib_get(LPICON taddr, WORD *icon_w, WORD *icon_h, BOOLEAN maskp, WORD *file_size)
	{
	LPVOID	where;
	WORD	icon_size;
/*	BYTE	mask_file[80];	*/

	if(*file_size == -1 && !maskp)  /* Need to get file name */
/*		if ( maskp )
			get_icnfile(&mask_file[0]);
		else	   
*/			{
			get_icnfile(&icn_file[0]);	 
			*file_size = 0;
			}
	if(!icn_file[0]) return(FALSE);
	where = 0x0L;
	if (icn_file[0])
		where = read_icon((BYTE *) &c_obndx[0], icon_w, icon_h, 
			&icon_size, &icn_file[0], maskp, file_size);
/*	else
		if(mask_file[0] && maskp && *file_size == -1)		
			where = read_icon((BYTE *) &c_obndx[0], icon_w,
					icon_h, &icon_size, &mask_file[0], 
					TRUE, file_size);
*/	if (where)		
		{
		if (!maskp)	taddr->ib_pdata = where;
		else		taddr->ib_pmask = where;
		taddr->ib_hicon = *icon_h;
		taddr->ib_wicon = *icon_w;
		icn_state = FILE_STATE;
		return (TRUE);
		}
	return (FALSE);
	}

VOID load_part(WORD obj)
	{
	LPTREE	tree, traddr;
	LPBIT   baddr;
	LPICON  iaddr;
	GRECT	p;
	WORD	type, sizeit,  icn_fsize, icn_w, icn_h;
				   
	if (iconedit_flag) traddr = save_tree;
	else               traddr = ad_view;
	obj_redraw(traddr, obj);
	baddr = (LPBIT)traddr[obj].ob_spec;
	iaddr = (LPICON)baddr;
	
	type = LLOBT(traddr[obj].ob_type);
	if (type == G_IMAGE)
		{
		if (bb_get(baddr, &p.g_w, &p.g_h))
			newsize_obj(traddr, obj, p.g_w, p.g_h, FALSE);
		if ( LHIBT(traddr[obj].ob_type ))
			{
			tree = traddr;
			tree[obj].ob_type = G_IMAGE & 0xFF;
			}
		}
	else
		{
		sizeit = FALSE;

		icn_fsize = -1;  /* display fsel */
		if (ib_get(iaddr, &p.g_w, &p.g_h, FALSE, &icn_fsize)) sizeit = TRUE;
		else if(!icn_file[0]) return;	/*cancel button in file sel*/
		icn_w = p.g_w;
		icn_h = p.g_h;
		if (ib_get(iaddr, &p.g_w, &p.g_h, TRUE, &icn_fsize))
			sizeit = TRUE;				  
		if (icn_fsize <= 0 )	      
			{
			icn_fsize = -1; /* display fsel to get mask */
		     	if(ib_get(iaddr, &p.g_w, &p.g_h, TRUE, &icn_fsize))
				sizeit = TRUE;			       
			}     
		if (icn_w != p.g_w || icn_h != p.g_h)
			{
			hndl_alert(1,string_addr(STSIZEP));
			sizeit = FALSE;
			p.g_w = -1;
			}
		rcs_edited = TRUE;
		if (sizeit)
			{
			icon_tfix(iaddr);
			icon_wh(iaddr, &p.g_w, &p.g_h);
			newsize_obj(traddr, obj, p.g_w, p.g_h, FALSE);
			}
		if ( LHIBT(traddr[obj].ob_type))
			{
			tree = traddr;
			tree[obj].ob_type = G_ICON & 0xFF;
			}
		}

	obj_redraw(traddr, obj); 
	}

