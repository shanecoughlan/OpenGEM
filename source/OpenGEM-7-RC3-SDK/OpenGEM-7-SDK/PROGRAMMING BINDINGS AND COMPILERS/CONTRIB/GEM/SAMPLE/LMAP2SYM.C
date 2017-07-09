/* MAPSYM.C                                          */
/* Microsoft LINK "MAP" -to- GEMSID "SYM" converter     */
/* modified for LINK v3.0 by J. Weaver Jr. [71446,1362] */
/* 30 May 1985                                          */
/* Feb 22, 1988 modified by Mei chung to handle large model symbol name */
extern char *gets();

main()
{
        int     off;
	unsigned int seg, old_seg, data_seg;
        char    buf[80];
        char    var[39];
        char    *p, *seg_name;
        int     done = 0;

        while (!done)
        {
                if ((p = gets(buf)) == 0)
                        exit(1);
		if (!strncmp(buf, " Start", 6)) {
		    while(!done){
			gets(buf);
			seg_name = buf + 22;
			if(!strncmp(seg_name, "DATA", 4)){
			    sscanf(buf, "%4x",&data_seg);
			    done = 1;
			}
		    }
		}
	}
	done = 0;
	while(!done)
	{
	    gets(buf);
            if (!strcmp(buf, "  Address         Publics by Value"))
                        done = 1;
        }
	old_seg =0xffff;
        while (scanf("%4x:%4x %s", &seg, &off, var) > 0)
        {
	    if (seg != old_seg){
	    	if (seg < data_seg)
	        	printf(" 0000 LABELS\t%04X CODE\n", seg);
	        else
	        	printf(" 0000 VARIABLES\t%04X DATA\n", seg);
		old_seg = seg;
	    }
            printf("%04X %s\n", off, var);
        }
        exit(0);
} /*  LMAP2SYM.C  */

