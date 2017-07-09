/*
 * Utility functions which will shift entire object trees between 
 * 16-bit and 32-bit memory spaces
 *
 * Important assumptions:
 *
 * 1. USERDEF (program-drawn) objects have a USERBLK as their object 
 *   spec, and this is always in low memory. This is because the 
 *   USERBLK is managed by dj_userdef().
 *
 * 2. Objects with the INDIRECT flag set point to a struct INDBLOCK:
 *
 */
#include <djgppgem.h>
#include "debug.h"

/* This is necessary because otherwise the indirect data cannot be moved
 * between low & normal memory; DJGPP doesn't know how big it is.
 *
 */


/* Round up a value to the nearest multiple of 4 */
#define ALLOW4(x) (((x) + 3L) & (~3L))



/* Find the size of an MFDB and its planes. */

LONG dj_mfdb_size32(MFDB *fdb)
{
    long plane_size = fdb->fww * 2 * fdb->fh;

    if (!fdb->mp) return ALLOW4(sizeof(MFDB));

    return ALLOW4(plane_size * fdb->np) + ALLOW4(sizeof(MFDB));
}

/* Find the size of an object tree and every object it refers to. There are 
 * versions for 32-bit and 16-bit trees.
 *
 * We allow space for all objects to be aligned on a 4-byte boundary. 
 */



LONG dj_tree_size32(OBJECT *tree)
{
    LONG size  = 0;
    INDBLOCK *iblock;
    LONG ospec; 
    TEDINFO *tedi;
    BITBLK  *blki;
    ICONBLK *icon;

    do
    {
        size += ALLOW4(sizeof(OBJECT));	  /* Size of object itself */
        ospec = tree->ob_spec;            /* Its spec (polymorphic) */

        /* If indirect, add its INDBLOCK, & follow it */
        if (tree->ob_flags & INDIRECT)
        {
            LONG isize;

            iblock = (INDBLOCK *)ospec;
            ospec = iblock->ind_spec;
            isize = iblock->ind_len;
            if (isize < 8) isize = 8;    /* Block size must be */
                                         /* at least 8 bytes */
            size += ALLOW4(isize);
        }

        switch(tree->ob_type & 0xFF)
        {
            case G_STRING:	/* Space for 1 string */
            case G_TITLE:
            case G_BUTTON:
                size += ALLOW4(1 + strlen((char *)ospec));
                break;

            case G_BOXTEXT:	/* Space for 1 TEDINFO & 3 strings */
            case G_TEXT:
            case G_FTEXT:
            case G_FBOXTEXT:
                size += ALLOW4(sizeof(TEDINFO));
                tedi = (TEDINFO *)ospec;
                
		size += ALLOW4(1 + tedi->te_txtlen);
                size += ALLOW4(1 + strlen(tedi->te_ptmplt));
                size += ALLOW4(1 + strlen(tedi->te_pvalid));
                break;

            case G_IMAGE:	/* Space for 1 BITBLK & 1 image */
                size += ALLOW4(sizeof(BITBLK));
                blki = (BITBLK *)ospec;
                size += ALLOW4(blki->bi_wb * blki->bi_hl);
                break;

            case G_ICON:	/* Space for 1 ICONBLK, 1 string, 2 images */
                size += ALLOW4(sizeof(ICONBLK));
                icon = (ICONBLK *)ospec;
                size += ALLOW4(1 + strlen((char *)icon->ib_ptext));
                size += ALLOW4( ((icon->ib_wicon + 15) & ~15) * icon->ib_hicon * 2);
                break;

            case G_CLRICN:	/* Space for 1 ICONBLK, 1 string, 2 MFDBs */
                size += ALLOW4(sizeof(ICONBLK));
                icon = (ICONBLK *)ospec;
                size += ALLOW4(1 + strlen((char *)icon->ib_ptext));
                size += ALLOW4(dj_mfdb_size32((MFDB *)icon->ib_pmask));
                size += ALLOW4(dj_mfdb_size32((MFDB *)icon->ib_pdata));
                break;
        }
	if (tree->ob_flags & LASTOB) break;
	++tree;
    } while (1);

    return size;
}


LONG dj_mfdb_size16(LPWORD mfdb)
{
    long plane_size = LWGET(mfdb+8) * 2 * LWGET(mfdb+6); /* fww * fh * 2 */

    if (LDGET(mfdb) == 0) return ALLOW4(sizeof(MFDB));

    return ALLOW4(plane_size * LWGET(mfdb+12)) + ALLOW4(sizeof(MFDB)); 
	/* (plane_size * np) + sizeof(MFDB) */ 
}


/* Find the space occupied by a 16-bit object tree */

LONG dj_tree_size16(LPTREE tree)
{
    LONG size  = 0;
    LPLONG iblock;
    LONG ospec; 

    do
    {
        size += ALLOW4(sizeof(OBJECT));        /* Size of object */
        ospec = LSGET(tree + 0x0C);        /* ob_spec 
                                                         * as pointer */

        /* If indirect, add its INDBLOCK, & follow it */
        if (LWGET(tree + 0x08) & INDIRECT)    /* ob_flags */
        {
            LONG isize;

            iblock = ospec;
            ospec = LSGET(iblock);
            isize = LDGET(iblock+4);
            if (isize < 8) isize = 8;    /* Block size must be */
                            /* at least 8 bytes */
            size += ALLOW4(isize);
        }

        switch(LWGET(tree + 0x06) & 0xFF) /* ob_type */
        {
            case G_STRING:
            case G_TITLE:
            case G_BUTTON:
                size += ALLOW4(1 + LSTRLEN(ospec));
                break;

            case G_BOXTEXT:
            case G_TEXT:
            case G_FTEXT:
            case G_FBOXTEXT:
//                zsize = size;

                size += ALLOW4(sizeof(TEDINFO));
		size += ALLOW4(1 + LWGET(ospec + 0x18)); /* te_txtlen */
                size += ALLOW4(1 + LSTRLEN(LSGET(ospec+4)));    /* te_ptmplt */
                size += ALLOW4(1 + LSTRLEN(LSGET(ospec+8)));    /* te_pvalid */
                break;

            case G_IMAGE:
                size += ALLOW4(sizeof(BITBLK));
                /*             bi_wb            bi_hl */
                size += ALLOW4(LWGET(ospec+4) * LWGET(ospec + 6));
                break;

            case G_ICON:
                size += ALLOW4(sizeof(ICONBLK));
                size += ALLOW4(1 + LSTRLEN(LSGET(ospec+8)));    /* ib_ptext */
                size += ALLOW4( ((LWGET(ospec + 0x16) + 15) & ~15) * 
                                  LWGET(ospec + 0x18) * 2);
                break;

            case G_CLRICN:
                size += ALLOW4(sizeof(ICONBLK));
                size += ALLOW4(1 + LSTRLEN(LSGET(ospec+8)));    /* ib_ptext */
                size += ALLOW4(dj_mfdb_size16(LSGET(ospec)));
                size += ALLOW4(dj_mfdb_size16(LSGET(ospec+4)));
                break;
        }
        if (LWGET(tree + 8) & LASTOB) break;
        tree += sizeof(OBJECT);
    } while(1);

    return size;
}


static LPBYTE string_32to16(char *str, LPBYTE *dstptr)
{
    LONG size = ALLOW4(1 + strlen(str));

    dosmemput(str, size, *dstptr);
    (*dstptr) += size;

    return (*dstptr) - size;    
}

LPBYTE dj_mfdb_32to16(MFDB *fdb, LPBYTE *dstptr)
{
    LONG size  = fdb->fww * fdb->fh * 2 * fdb->np;
    LONG obadd = *dstptr;

    dosmemput(fdb, sizeof(MFDB), obadd);

    (*dstptr) += ALLOW4(sizeof(MFDB));
    if (fdb->mp)
    {
        LSSET(obadd, (*dstptr));	/* 1st plane */
        dosmemput((void *)(fdb->mp), size, (*dstptr));
        (*dstptr) += ALLOW4(size);
    }
    return obadd;
}





void dj_tree_32to16(OBJECT *tree, LPTREE tree16)
{
    INDBLOCK *iblock;
    TEDINFO *tedi;
    BITBLK  *blki;
    ICONBLK *icon;
    LONG trptr = tree16;
    LONG size;
    LONG dstptr, obptr;
    LONG treelen = 0;
    LONG pospec, ospec;
    WORD n = 0;

    /* 1. Copy in the tree entries themselves, not fixing-up pointers */

    do
    {
	treelen += sizeof(OBJECT);        

    } while (!(tree[n++].ob_flags & LASTOB));

    dosmemput(tree, treelen, tree16); /* Tree itself is in */
    dstptr = ALLOW4(tree16 + treelen);

    /* 2. Work down the tree copying in data */

    do
    {
        ospec  = tree->ob_spec;            /* Its spec (polymorphic) */
        pospec = trptr + 12;               /* Address of 16-bit spec */

        /* If indirect, add its INDBLOCK, & follow it */
        if (tree->ob_flags & INDIRECT)
        {
            LONG isize;

            iblock = (INDBLOCK *)ospec;
            ospec = iblock->ind_spec;
            isize = iblock->ind_len;
            if (isize < 8) isize = 8;    /* Block size must be */
                                         /* at least 8 bytes */
            dosmemput(iblock, isize, dstptr);
            LSSET(pospec, dstptr);
            pospec = dstptr;	         /* Write real spec to indirect block */
                                         /* Real spec defaults to 32-bit int */
            dstptr += ALLOW4(isize);
        }

        switch(tree->ob_type & 0xFF)
        {
            case G_STRING:	/* Space for 1 string */
            case G_TITLE:
            case G_BUTTON:
                LSSET(pospec, string_32to16((char *)ospec, &dstptr));
                break;

            case G_BOXTEXT:	/* Space for 1 TEDINFO & 3 strings */
            case G_TEXT:
            case G_FTEXT:
            case G_FBOXTEXT:
                obptr = dstptr;
                tedi  = (TEDINFO *)ospec;
                dosmemput(tedi, sizeof(TEDINFO), obptr);
                dstptr += ALLOW4(sizeof(TEDINFO));

                LSSET(pospec,   obptr);
		dosmemput(tedi->te_ptext, 1 + tedi->te_txtlen, dstptr);
		LSSET(obptr,    dstptr);
		dstptr += ALLOW4(1 + tedi->te_txtlen);
                LSSET(obptr+4,  string_32to16((char *)tedi->te_ptmplt, &dstptr));
                LSSET(obptr+8,  string_32to16((char *)tedi->te_pvalid, &dstptr));
                break;

            case G_IMAGE:	/* Space for 1 BITBLK & 1 image */
                obptr = dstptr;
                blki = (BITBLK *)ospec;
                dosmemput(blki, sizeof(BITBLK), obptr);
                dstptr += ALLOW4(sizeof(BITBLK));
                size = blki->bi_wb * blki->bi_hl;
                dosmemput((void *)blki->bi_pdata, size, dstptr);
                LSSET(obptr,    dstptr);
                LSSET(pospec,   obptr);
                dstptr += ALLOW4(size);
                break;

            case G_ICON:	/* Space for 1 ICONBLK, 1 string, 2 images */
                obptr = dstptr;
                icon = (ICONBLK *)ospec;
                dosmemput(icon, sizeof(ICONBLK), obptr);
                dstptr += ALLOW4(sizeof(ICONBLK));
                LSSET(obptr + 8, string_32to16((char *)icon->ib_ptext, &dstptr));
                size  = ((icon->ib_wicon + 15) & ~15) * icon->ib_hicon;
                LSSET(obptr,     dstptr);
                LSSET(obptr + 4, dstptr + size);
                LSSET(pospec,    obptr);
                dosmemput((void *)icon->ib_pmask, size, dstptr); 
                dstptr += ALLOW4(size);
                dosmemput((void *)icon->ib_pdata, size, dstptr); 
                dstptr += ALLOW4(size);
                break;

            case G_CLRICN:	/* Space for 1 ICONBLK, 1 string, 2 MFDBs */
                obptr = dstptr;
                icon = (ICONBLK *)ospec;
                dosmemput(icon, sizeof(ICONBLK), obptr);
                dstptr += ALLOW4(sizeof(ICONBLK));
                LSSET(obptr + 8, string_32to16 ((char *)icon->ib_ptext, &dstptr));
                LSSET(obptr,     dj_mfdb_32to16((MFDB *)icon->ib_pmask, &dstptr));
                LSSET(obptr + 4, dj_mfdb_32to16((MFDB *)icon->ib_pdata, &dstptr));
                LSSET(pospec, obptr);
                break;
        }
	if (tree->ob_flags & LASTOB) break;
	++tree;
	trptr += sizeof(OBJECT);
    } while (1);

}


static UBYTE *string_16to32(LPBYTE str, UBYTE **dstptr)
{
    LONG size = ALLOW4(1 + LSTRLEN(str));
    UBYTE *dest = (*dstptr);

    dosmemget(str, size, dest);
    (*dstptr) += size;

    return dest;
}

MFDB *dj_mfdb_16to32(LPMFDB obadd, UBYTE **dstptr)
{
    LONG size;
    MFDB *fdb = (MFDB *)(*dstptr);

    dosmemget(obadd, sizeof(MFDB), fdb);
    size = fdb->fww * fdb->fh * 2 * fdb->np;

    (*dstptr) += ALLOW4(sizeof(MFDB));
    if (fdb->mp)
    {
        fdb->mp = (LONG)(*dstptr);
        dosmemget(LSGET(obadd), size, dstptr);
        (*dstptr) += ALLOW4(size);
    }
    return fdb;
}



void dj_tree_16to32(LPTREE tree16, OBJECT *tree)
{
    TEDINFO *tedi;
    BITBLK  *blki;
    ICONBLK *icon;
    LONG size = 0;
    UBYTE *dstptr, *dstmax;
    LONG treelen = 0;
    LONG ospec;
    LONG *pospec;
    WORD n = 0;

//    WORD tr_n = 0;
//    UBYTE *begin = (UBYTE *)tree;
    /* 1. Copy in the tree entries themselves, not fixing-up pointers */

    do
    {
	treelen += sizeof(OBJECT);        

    } while (!(LWGET(tree16 + 24*(n++) + 8) & LASTOB));

    dosmemget(tree16, treelen, tree); /* Tree itself is in */
    dstptr = ((UBYTE *)tree) + ALLOW4(treelen);
    dstmax = ((UBYTE *)tree) + dj_tree_size16(tree16);

    /* 2. Work down the tree copying in data */

    do
    {
        ospec  = LSGET(tree16 + 0x0C);     /* Spec as linear pointer */
        pospec = &(tree->ob_spec);         /* Destination spec */

        /* If indirect, add its INDBLOCK, & follow it */
        if (tree->ob_flags & INDIRECT)
        {
            LONG isize;

            isize = LSGET(ospec + 4);  /* size of INDIRECT block */
            if (isize < 8) isize = 8;
            
            dosmemget(ospec, isize, dstptr);
            ospec = LSGET(ospec);       /* Real spec, as pointer */
            *pospec = (LONG)dstptr;	/* Address of 32-bit INDIRECT block */
            pospec  = (LONG *)dstptr;   /* Address of real ob. spec */
            dstptr += ALLOW4(isize);
        }

        switch(tree->ob_type & 0xFF)
        {
            case G_STRING:	/* Space for 1 string */
            case G_TITLE:
            case G_BUTTON:
                *pospec = (LONG)string_16to32(ospec, &dstptr);
                break;

            case G_BOXTEXT:	/* Space for 1 TEDINFO & 3 strings */
            case G_TEXT:
            case G_FTEXT:
            case G_FBOXTEXT:
                dosmemget(ospec, sizeof(TEDINFO), dstptr);
                tedi = (TEDINFO *)dstptr;
                dstptr += ALLOW4(sizeof(TEDINFO));
                tedi->te_ptext  = dstptr;
		dosmemget(LSGET(ospec), 1 + tedi->te_txtlen, dstptr);
		dstptr += ALLOW4(1 + tedi->te_txtlen);

                tedi->te_ptmplt = string_16to32(LSGET(ospec+4), &dstptr);
                tedi->te_pvalid = string_16to32(LSGET(ospec+8), &dstptr);
               *pospec = (LONG)tedi;
                break;

            case G_IMAGE:	/* Space for 1 BITBLK & 1 image */
                dosmemget(ospec, sizeof(BITBLK), dstptr);
                blki = (BITBLK *)dstptr;
                dstptr += ALLOW4(sizeof(BITBLK));
                size = blki->bi_wb * blki->bi_hl;
                dosmemget(LSGET(ospec), size, dstptr);
                blki->bi_pdata = dstptr;
                dstptr += ALLOW4(size);
                *pospec = (LONG)blki;
                break;

            case G_ICON:	/* Space for 1 ICONBLK, 1 string, 2 images */
                dosmemget(ospec, sizeof(ICONBLK), dstptr);
                icon = (ICONBLK *)dstptr;
                dstptr += ALLOW4(sizeof(ICONBLK));
                size  = ((icon->ib_wicon + 15) & ~15) * icon->ib_hicon;
                icon->ib_ptext = string_16to32(LSGET(ospec + 8), &dstptr);
		dosmemget(LSGET(ospec), size, dstptr);
                icon->ib_pmask = dstptr;
                dstptr += ALLOW4(size);
                dosmemget(LSGET(ospec+4), size, dstptr);
                icon->ib_pdata = dstptr;
                dstptr += ALLOW4(size);
                *pospec = (LONG)icon;
                break;

            case G_CLRICN:	/* Space for 1 ICONBLK, 1 string, 2 MFDBs */
                dosmemget(ospec, sizeof(ICONBLK), dstptr);
                icon = (ICONBLK *)dstptr;
                dstptr += ALLOW4(sizeof(ICONBLK));
                icon->ib_ptext = string_16to32 (LSGET(ospec + 8), &dstptr);
		icon->ib_pmask = dj_mfdb_16to32(LSGET(ospec    ), &dstptr);
		icon->ib_pdata = dj_mfdb_16to32(LSGET(ospec + 4), &dstptr);
                *pospec = (LONG)icon;   
                break;
        }
	if (tree->ob_flags & LASTOB) break;
	++tree;
        tree16 += sizeof(OBJECT);
    } while (1);
}



/* Copy a tree back, but not reallocating at all, and only copying back
 * data that may have been changed by (say) a form_do() call or by
 * image format changes */

void dj_tree_16to32s(LPTREE tree16, OBJECT *tree)
{
    LONG spec_orig;
    TEDINFO *tedi32;
    ICONBLK *icon32;
    BITBLK  *bit32;
    LPTEDI   tedi16;
    LPICON   icon16;
    LPBIT    bit16;


	/* 1. Copy in the tree entries themselves, not fixing-up pointers */

    do
    {
        spec_orig = tree->ob_spec;
	dosmemget(tree16, sizeof(OBJECT), tree);
	tree->ob_spec = spec_orig;

	if (tree->ob_flags & INDIRECT)
	{
		spec_orig = *(LONG *)spec_orig;
	}
	switch(tree->ob_type & 0xFF)
	{
            case G_BOXTEXT:	/* If it's a TEDINFO get the text */
            case G_TEXT:
            case G_FTEXT:
            case G_FBOXTEXT:
		tedi16 = LSGET(tree16 + 0x0C);
		tedi32 = (TEDINFO *)spec_orig;
		dosmemget(LSGET(tedi16), 1 + tedi32->te_txtlen, tedi32->te_ptext);
		break;
            /* If it's a BITBLK or ICONBLK, set the image */
	    case G_IMAGE:
		bit16 = LSGET(tree16 + 0x0C);
		bit32 = (BITBLK *)spec_orig;
		dosmemget(LSGET(bit16), bit32->bi_wb * bit32->bi_hl, bit32->bi_pdata);
		break;
	    case G_ICON:		
		icon16 = LSGET(tree16 + 0x0C);
		icon32 = (ICONBLK *)spec_orig;
		dosmemget(LSGET(icon16),   ((icon32->ib_wicon + 7)/8) * icon32->ib_hicon, icon32->ib_pmask);
		dosmemget(LSGET(icon16+4), ((icon32->ib_wicon + 7)/8) * icon32->ib_hicon, icon32->ib_pdata);
		break;
	}
	tree16 += sizeof(OBJECT);
	tree++;
    } while (!(tree[-1].ob_flags & LASTOB));
}


/* Copy a tree back, but not reallocating at all, and only copying back
 * data that may have been changed by (say) a form_do() call or by image
 * format changes */

void dj_tree_32to16s(OBJECT *tree, LPTREE tree16)
{
    LONG spec_orig;
    TEDINFO *tedi32;
    ICONBLK *icon32;
    BITBLK  *bit32;
    LPTEDI   tedi16;
    LPICON   icon16;
    LPBIT    bit16;


	/* 1. Copy in the tree entries themselves, not fixing-up pointers */

    do
    {
        spec_orig = LSGET(tree16 + 0x0C);
	dosmemput(tree, sizeof(OBJECT), tree16);
	LSSET(tree16 + 0x0C, spec_orig);

	if (tree->ob_flags & INDIRECT)
	{
		spec_orig = LSGET(spec_orig);
	}
	switch(tree->ob_type & 0xFF)
	{
            case G_BOXTEXT:	/* If it's a TEDINFO get the text */
            case G_TEXT:
            case G_FTEXT:
            case G_FBOXTEXT:
		tedi16 = LSGET(tree16 + 0x0C);
		tedi32 = (TEDINFO *)(tree->ob_spec);
		dosmemput(tedi32->te_ptext, 1 + tedi32->te_txtlen, LSGET(tedi16));
		break;
            /* If it's a BITBLK or ICONBLK, set the image */
	    case G_IMAGE:
		bit16 = LSGET(tree16 + 0x0C);
		bit32 = (BITBLK *)(tree->ob_spec);
		dosmemput(bit32->bi_pdata, bit32->bi_wb * bit32->bi_hl, LSGET(bit16));
		break;
	    case G_ICON:		
		icon16 = LSGET(tree16 + 0x0C);
		icon32 = (ICONBLK *)(tree->ob_spec);
		dosmemput(icon32->ib_pmask, ((icon32->ib_wicon + 7)/8) * icon32->ib_hicon, LSGET(icon16));
		dosmemput(icon32->ib_pdata, ((icon32->ib_wicon + 7)/8) * icon32->ib_hicon, LSGET(icon16+4));
		break;
	}
	tree16 += sizeof(OBJECT);
	tree++;
    } while (!(tree[-1].ob_flags & LASTOB));
}
