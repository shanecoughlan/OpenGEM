/* PMAP2SYM	                                                */
/* PLINK module map file to GEMSID "SYM file			*/
/* 12 JULY 1985							*/
/*                                                              */
/* This program creates a large model symbol file for use with  */  
/* GEMSID  from  a  map file that was created  using  the  "m"  */
/* option in PLINK86.                                           */
/*                                                              */
/* This  program assumes that the substring "DATA" is part  of  */ 
/* the segment name or the class name for data segments.        */
/*                                                              */ 
/*        (use PMAPSYM.LNK to link this file)                   */
/*                                                              */


extern char *gets();

main()
{
	int done;
	char buf[200];
	int data_flg, code_flg;
	char name[13], seg_val[5], off_val[5];
	char *buf_ptr, *buf_end;

	done = 0;
	while ( !done )
	{
	    if( gets(buf) == 0 )
	    {
		exit(1);
	    }
	    if( !strcmp(buf, "Modules:") )
		done = 1;
	}
	while( gets(buf) )
	{
	    if( !strncmp( buf, "        Segment ", 16 ) )
	    {
		if( find_str( buf, "DATA") )
		{
		    code_flg = 0;
		    data_flg = 1;
		}
		else
		{
		    code_flg = 1;
		    data_flg = 0;
		}
	    }
	    else if ( !strncmp( buf, "            ", 12 ) )
	    {
		buf_ptr = &buf[12];
		buf_end = &buf[0] + strlen(buf);
		while( buf_ptr < buf_end )
		{
		    strncpy(name, buf_ptr, 8);
		    buf_ptr += 11;
		    strncpy(seg_val, buf_ptr, 4);
		    buf_ptr += 5;
		    strncpy(off_val, buf_ptr, 4);
		    buf_ptr += 7;
		    drop_blnk( name );
		    blnktozero( seg_val );
		    blnktozero( off_val );
		    if( data_flg )
		    {
			printf("0000 VARIABLES\t%s DATA\n",seg_val);
			data_flg = 0;
		    }
		    if( code_flg )
		    {
			printf("0000 LABELS\t%s CODE\n",seg_val);
			code_flg = 0;
		    }
		    printf("%s %s\n",off_val,name);
		}
	    }
	}
}


find_str( str1, str2 )
	char *str1, *str2;
{
	int length;

	length = strlen(str2);
	while( *str1 )
	{
	    if( !strncmp( str1, str2, length ) )
		return(1);
	    str1++;
	}
	return(0);
}

blnktozero( str )
	char *str;
{
	while( *str )
	{
	    if( *str == ' ' )
	        *str = '0';
	    str++;
	}
}

drop_blnk( str)
	char *str;
{
	char *send;

	send = str + strlen( str );
	while( *(--send) == ' ');
	*(send+1) = 0;
}
ÿ