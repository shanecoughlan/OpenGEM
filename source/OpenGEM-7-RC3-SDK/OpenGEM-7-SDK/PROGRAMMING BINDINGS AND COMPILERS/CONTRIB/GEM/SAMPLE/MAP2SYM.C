/* MAPSYM.C                                          */
/* Microsoft LINK "MAP" -to- GEMSID "SYM" converter     */
/* modified for LINK v3.0 by J. Weaver Jr. [71446,1362] */
/* 30 May 1985                                          */

extern char *gets();

main()
{
        int     seg, off, flg;
        char    buf[200];
        char    var[32];
        char    *p;
        int     done = 0;

        while (!done)
        {
                if ((p = gets(buf)) == 0)
                        exit(1);
                if (!strcmp(buf,
                 "  Address         Publics by Value"))
                        done = 1;
        }
        printf(" 0000 LABELS\n");
        flg = 1;
        while (scanf("%4x:%4x %s", &seg, &off, var) > 0)
        {
                if (flg && seg)
		{
                        flg = 0;
                        printf(" 0000 VARIABLES\n");
                }
                printf("%04x %s\n", off, var);
        }
        exit(0);
} /*  MAP2SYM.C  */


