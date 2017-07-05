#include <stdlib.h>
#include "djgppgem.h"

/* 
 * This code allows big MFDBs to be stored in 32-bit memory and copied
 * into normal memory in chunks. This will not work for desktop images,
 * because they must be in memory at all times. It does work for
 * the BitBlit operations vro_cpyfm() and vrt_cpyfm(), and the 
 * transformation vr_trnfm().
 *
 */

MLOCAL int __inline__ min(int a, int b) { return (a < b) ? a : b; }
MLOCAL int __inline__ max(int a, int b) { return (a > b) ? a : b; }

#define MFDB_BUF_LEN 65536L

/*
 * mfdb_slice_size() works out the height of a chunk.
 */

static int mfdb_slice_size(MFDB *fdb, int bufsiz, int *height)
{
    int  datlen;
    int  nbuflen;

    if (!fdb->mp) /* It is the screen. There is no buffer. */
    {
        *height = fdb->fh;
        return 0;
    }
    datlen = fdb->np * fdb->fww * fdb->fh * 2;    /* Size of the data */

    if (datlen < bufsiz) return fdb->fh;    /* The whole lot fits */

    nbuflen =  (bufsiz / fdb->np);           /* Max. size of 1 plane */
    nbuflen /= (2 * fdb->fww);               /* Max. height of the block */

    *height = nbuflen;

    nbuflen *= (2 * fdb->fww * fdb->np);    /* Actual size of the resulting buffer */

    return nbuflen;
}

/*
 * 0 = mp
 * 4 = fwp
 * 6 = fh
 * 8 = fww
 */

static void copyin(MFDB *fdb, LPMFDB fdb16, int y, int height)
    {
    int pl;
    int plen   = fdb->fww * 2 * fdb->fh;
    int plen16 = fdb->fww * 2 * height;
    LPBYTE smp = LSGET(fdb16);

    LWSET(fdb16 + 6, height);

    for (pl = 0; pl < fdb->np; pl++)
        {
        BYTE  *sso = ((BYTE *)fdb->mp) + (y * fdb->fww * 2) + (pl * plen);
        LPBYTE dso = smp     + (pl * plen16);

        dosmemput(sso, height * fdb->fww * 2, dso); 
        }
    }


static void copyout(MFDB *fdb, LPMFDB fdb16, int y, int height)
    {
    int pl;
    int plen   = fdb->fww * 2 * fdb->fh;
    int plen16 = fdb->fww * 2 * LWGET(fdb16 + 6);
    LPBYTE smp = LSGET(fdb16);

    for (pl = 0; pl < fdb->np; pl++)
        {
        BYTE  *sso = ((BYTE *)fdb->mp) + (y * fdb->fww * 2) + (pl * plen);
        LPBYTE dso = smp     + (pl * plen16);

        dosmemget(dso, height * fdb->fww * 2, sso); 
        }
    }





void mfdb_2ops(MFDB *src, MFDB *dst, MFUNC cpyfunc, void *lParam)
    {
    int bufsiz = MFDB_BUF_LEN - 2 * sizeof(MFDB);
    int bs1, bs2, height1, height2, step, height, y, z;
    LPMFDB s2, d2;
    LPBYTE smp, dmp;
    LPBYTE buf = dos_alloc(MFDB_BUF_LEN);

    if (!buf) return;
    if (src->mp && dst->mp) bufsiz /= 2;

    s2 = buf + MFDB_BUF_LEN - sizeof(MFDB);
    d2 = buf + MFDB_BUF_LEN - 2*sizeof(MFDB);

    dosmemput(src, sizeof(MFDB), s2);
    dosmemput(dst, sizeof(MFDB), d2);

    smp = (src->mp) ? buf : 0; 
    dmp = (dst->mp) ? (smp ? buf + bufsiz : buf) : 0;        

    LSSET(s2, smp);
    LSSET(d2, dmp);

    height = max(src->fh, dst->fh); /* Is this correct, or should it be min() ? */

    if (!smp && !dmp)
        {
	height = 99999;
        (*cpyfunc)(s2, d2, 0, 0, height, height, lParam);
        dos_free(buf);
        return;
        }

    bs1 = mfdb_slice_size(src, bufsiz, &height1);
    bs2 = mfdb_slice_size(dst, bufsiz, &height2);

    if ((smp && height1 < 1) || (dmp && height2 < 1)) return;

// 0 = mp
// 4 = fwp
// 6 = fh
// 8 = fww

    if (smp && dmp) step = min(height1, height2);
    else if (dmp)   step = height2;
    else            step = height1;

    for (y = 0; y < height; y += step)
        {
        int sfh = min(height - y, step);

        if (smp) copyin(src, s2, y, sfh);

	for (z = 0; z < height; z += step)
	{
		int dfh = min(height - z, step);
		if (dmp) copyin(dst, d2, z, dfh);

	        (*cpyfunc)(s2, d2, y, z, sfh, dfh, lParam);

        	if (dmp) copyout(dst, d2, z, dfh);
        }
        if (smp) copyout(src, s2, y, sfh);
    }
    dos_free(buf);
    }

/* Functions that use this code */

extern LPLPTR pPblock;
extern LPWORD pContrl, pIntin, pIntout, pPtsin, pPtsout;
extern LPBYTE pGeneral;	/* General (4k) transfer area */

MLOCAL void __inline__ i_intin(LPVOID l)  { LSSET(pPblock + 4,  l); }
MLOCAL void __inline__ i_ptsin(LPVOID l)  { LSSET(pPblock + 8,  l); }
MLOCAL void __inline__ i_intout(LPVOID l) { LSSET(pPblock + 12, l); }
MLOCAL void __inline__ i_ptsout(LPVOID l) { LSSET(pPblock + 16, l); }
MLOCAL void __inline__ i_lptr1(LPVOID l)  { LSSET(pContrl + 14, l); }
MLOCAL void __inline__ i_lptr2(LPVOID l)  { LSSET(pContrl + 18, l); }

#define swap(a,b) { n = a; a = b; b = n; }

/* This function will present every slice of the source MFDB to every 
 slice of the destination MFDB. We have to work out the overlap. 

 So, if overall a rectangle between (y0, y1) must be copied to (y2, y3):

* 1. Work out source coordinates that are available.
* 2. Work out from that what the preferred destination coordinates are for
     that source area.
* 3. Take the intersection of the preferred destination with the 
     available destination.
* 4. Work out from that what the usable source coordinates are.
* 5. If the source and destination are non-empty, do the copy.
*/

MLOCAL VOID djro_cpyfm(LPMFDB mfsrc, LPMFDB mfdest, int SY, int DY, int SH, int DH, void *lParam)
{
    WORD xy[8], voffset;
    WORD n;

    memcpy(xy, lParam, 16);
    if (xy[0] > xy[2]) swap(xy[0], xy[2]);
    if (xy[1] > xy[3]) swap(xy[1], xy[3]);
    if (xy[4] > xy[6]) swap(xy[4], xy[6]);
    if (xy[5] > xy[7]) swap(xy[5], xy[7]);

    voffset = xy[5] - xy[1]; /* Vertical movement required between source
                              * and target rectangles */

/* 1. Work out source coordinates that are available in this chunk */

    for (n = 1; n < 5; n+=2)
    {
         if (xy[n] <        SY)  xy[n] = SY;
         if (xy[n] >= (SY + SH)) xy[n] = SY + SH - 1;
    }

    if (xy[1] == xy[3]) return; /* No overlap at all */

/* 2. Work out from that what the preferred destination coordinates are */

    xy[5] = xy[1] + voffset;
    xy[7] = xy[3] + voffset;

/* 3. Take the intersection of the preferred destination with the 
 *    destination chunk */
    for (n = 5; n < 9; n+=2)
    {
	 if (xy[n] <        DY)  xy[n] = DY;
         if (xy[n] >= (DY + DH)) xy[n] = DY + DH - 1;
    }
    if (xy[5] == xy[7]) return; /* No overlap again */

/* 4. Work out from that what the usable source coordinates are */

    xy[1] = xy[5] - voffset;
    xy[3] = xy[7] - voffset;

/* If not using screen MFDBs, coordinates must be relative to the slice, not
 * to the screen */

    if (LSGET(mfsrc))  { xy[1] -= SY; xy[3] -= SY; }
    if (LSGET(mfdest)) { xy[5] -= DY; xy[7] -= DY; }

    dosmemput(xy, 16, pPtsin);

    i_lptr1(mfsrc);
    i_lptr2(mfdest);

    vdi();
}


/* This function will present every slice of the source MFDB to every 
 slice of the destination MFDB. Since a transform works on the whole
 MFDB rather than little bits, we just go down the slices in lockstep. */


MLOCAL VOID djvr_trnfm(LPMFDB mfsrc, LPMFDB mfdest, int SY, int DY, int SH, int DH, void *lParam)
{
    i_lptr1(mfsrc);
    i_lptr2(mfdest);

    if (SY != DY) return;

    vdi();
}

/* VDI-style interfaces to the functionality */

VOID dj_vro_cpyfm( WORD handle, WORD wr_mode, WORD xy[], MFDB *srcMFDB, MFDB *desMFDB )
{
    LWSET(pIntin, wr_mode);

    LWSET(pContrl + 0, 109);
    LWSET(pContrl + 2,   4);
    LWSET(pContrl + 6,   1);
    LWSET(pContrl +12,   handle);
    mfdb_2ops(srcMFDB, desMFDB, djro_cpyfm, xy);
}



VOID dj_vrt_cpyfm( WORD handle, WORD wr_mode, WORD xy[], MFDB *srcMFDB, MFDB *desMFDB, WORD *index )
{
    LWSET(pIntin, wr_mode);
    LWSET(pIntin + 2, *index++);
    LWSET(pIntin + 4, *index);

    LWSET(pContrl + 0, 121);
    LWSET(pContrl + 2,   4);
    LWSET(pContrl + 6,   3);
    LWSET(pContrl +12,   handle);
    mfdb_2ops(srcMFDB, desMFDB, djro_cpyfm, xy);
}


VOID dj_vr_trnfm( WORD handle, MFDB *srcMFDB, MFDB *desMFDB )
{
    LWSET(pContrl + 0, 110);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);

    mfdb_2ops(srcMFDB, desMFDB, djvr_trnfm, NULL);
}

