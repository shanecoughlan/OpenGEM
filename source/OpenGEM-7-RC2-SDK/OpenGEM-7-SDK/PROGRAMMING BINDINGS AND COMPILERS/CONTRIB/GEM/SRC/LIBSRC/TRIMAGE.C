/******************************************************************************
 *
 *    TRGIMAGE.C
 *
 *    Procedures, that convert icons and images of an object tree
 *    into device specific format.
 *
 * Author   :  Robert Schneider
 * created  :  12/12/88
 * Version  :  1.00
 *
 *****************************************************************************/

#include "djgppgem.h"

GLOBAL VOID fix_icon       (WORD vdi_handle, LPTREE tree);
GLOBAL VOID trans_gimage   (WORD vdi_handle, LPTREE tree, WORD obj);
MLOCAL VOID vdi_trans      (WORD vdi_handle, LPVOID saddr, WORD swb,
                              LPVOID daddr, WORD dwb, WORD h);
MLOCAL VOID vdi_fix        (MFDB FAR *pfd, LPVOID theaddr, WORD wb,
                              WORD h);


/*----------------------------------*/
/*          fix_icon                */
/* Runs through a tree and converts,*/
/* and converts icons and images    */
/*----------------------------------*/
GLOBAL VOID fix_icon(WORD vdi_handle, LPTREE tree)
{
   WORD obj = -1;
   WORD type;
   REG LPWORD addr;

   do
   {

      ++obj;
      addr = tree + (obj * sizeof(OBJECT)) + 6;

      type = LWGET(addr) & 0xff;
      if (type == G_ICON || type == G_IMAGE)
         trans_gimage(vdi_handle, tree, obj);
   } while (!(LWGET(addr + 2) & LASTOB));
}

/*--------------------------------*/
/*        trans_gimage()          */
/* Transform bit images and icons */
/*--------------------------------*/
GLOBAL VOID trans_gimage(WORD vdi_handle, LPTREE tree, WORD obj)
{
   LPICON  ic;
   LPBIT   ib;
   LPVOID  taddr;
   WORD    wb, hl;
   LPWORD  addr = tree + (obj * sizeof(OBJECT)) + 6;

   ic = LSGET(addr+6);
   ib = LSGET(addr+6);

   if ((LWGET(addr) & 0xff) == G_ICON)
   {
      taddr = LSGET(ic);
      wb    = LWGET(ic + 0x16) >> 3;	/* pixels to bytes */
      hl    = LWGET(ic + 0x18);
      vdi_trans(vdi_handle, taddr, wb, taddr, wb, hl); /* transform mask */

      taddr = LSGET(ic+4);

   }
   else if ((LWGET(addr) & 0xff) == G_IMAGE)
   {
      taddr = LSGET(ib);
      wb    = LWGET(ib + 4);
      hl    = LWGET(ib + 6);
   }
   else
      return;

   vdi_trans(vdi_handle, taddr, wb, taddr, wb, hl);   /* transform image or  */
                                                      /* icon data           */
}

/*------------------------------*/
/*        vdi_trans()           */
/*   'on the fly' transform     */
/*------------------------------*/
MLOCAL VOID vdi_trans(WORD vdi_handle, LPVOID saddr, WORD swb, LPVOID daddr, WORD dwb, WORD h)
{
   MFDB src, dst;                    /* local MFDB                   */

   vdi_fix(&src, saddr, swb, h);
   src.ff = TRUE;                    /* standard format              */

   vdi_fix(&dst, daddr, dwb, h);
   dst.ff = FALSE;                   /* transform to device          */
                                     /* specific format              */   
   vr_trnfm(vdi_handle, &src, &dst ); 
}

/*------------------------------*/
/*          vdi_fix()           */
/* set up MFDB for transform    */
/*------------------------------*/

MLOCAL VOID vdi_fix(MFDB *pfd, LPVOID theaddr, WORD wb, WORD h)
{
   pfd->fww = wb >> 1;             /* # of bytes to words          */
   pfd->fwp = wb << 3;             /* # of bytes to to pixels      */
   pfd->fh = h;                    /* height in scan lines         */
   pfd->np = 1;                    /* number of planes             */
   pfd->mp = theaddr;              /* memory pointer               */
}
