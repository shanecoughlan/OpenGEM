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

#include <portab.h>
#include <aes.h>
#include <vdi.h>

GLOBAL VOID fix_icon       _((WORD vdi_handle, OBJECT FAR *tree));
GLOBAL VOID trans_gimage   _((WORD vdi_handle, OBJECT FAR *tree, WORD obj));
MLOCAL VOID vdi_trans      _((WORD vdi_handle, VOID FAR *saddr, WORD swb,
                              VOID FAR *daddr, WORD dwb, WORD h));
MLOCAL VOID vdi_fix        _((MFDB FAR *pfd, VOID FAR *theaddr, WORD wb,
                              WORD h));


/*----------------------------------*/
/*          fix_icon                */
/* Runs through a tree and converts,*/
/* and converts icons and images    */
/*----------------------------------*/
GLOBAL VOID fix_icon(vdi_handle, tree)

WORD vdi_handle;
OBJECT FAR *tree;
{
   WORD obj = 0;
   WORD type;

   do
   {
      type = tree[obj].ob_type & 0xff;
      if (type == G_ICON || type == G_IMAGE)
         trans_gimage(vdi_handle, tree, obj);
   } while (!(tree[obj++].ob_flags & LASTOB));
}

/*--------------------------------*/
/*        trans_gimage()          */
/* Transform bit images and icons */
/*--------------------------------*/
GLOBAL VOID trans_gimage(vdi_handle, tree, obj)

WORD vdi_handle;
OBJECT FAR *tree;
WORD obj;
{
   ICONBLK FAR *ic;
   BITBLK  FAR *ib;
   LPVOID  taddr;
   WORD         wb, hl;

   ic = (ICONBLK FAR *)tree[obj].ob_spec;
   ib = (BITBLK FAR *)tree[obj].ob_spec;

   if ((tree[obj].ob_type & 0xff) == G_ICON)
   {
      taddr = ic->ib_pmask;
      wb    = ic->ib_wicon;
      wb    = wb >> 3;                   /* pixels to bytes */
      hl    = ic->ib_hicon;
      vdi_trans(vdi_handle, taddr, wb, taddr, wb, hl); /* transform mask */

      taddr = ic->ib_pdata;
   }
   else if ((tree[obj].ob_type & 0xff) == G_IMAGE)
   {
      taddr = ib->bi_pdata;
      wb    = ib->bi_wb;
      hl    = ib->bi_hl;
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
MLOCAL VOID vdi_trans(vdi_handle, saddr, swb, daddr, dwb, h)

WORD vdi_handle;
VOID FAR *saddr;
WORD swb;
VOID FAR *daddr;
WORD dwb;
WORD h;
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

MLOCAL VOID vdi_fix(pfd, theaddr, wb, h)

MFDB FAR *pfd;
VOID FAR *theaddr;
WORD wb, h;
{
   pfd->fww = wb >> 1;             /* # of bytes to words          */
   pfd->fwp = wb << 3;             /* # of bytes to to pixels      */
   pfd->fh = h;                    /* height in scan lines         */
   pfd->np = 1;                    /* number of planes             */
   pfd->mp = theaddr;              /* memory pointer               */
}
