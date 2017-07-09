/******************************************************************************
 *
 *    Modulname   :  EMSLIB.C
 *
 *    Author      :  Robert Schneider
 *    Version     :  1.0
 *    Created     :  May 3, 1989
 *
 *****************************************************************************
 *
 *	-------------------------------------------------------------
 *	GEM Desktop					  Version 3.1
 *	Serial No.  XXXX-0000-654321		  All Rights Reserved
 *	Copyright (C) 1985 - 1989		Digital Research Inc.
 *	-------------------------------------------------------------
 *
 *****************************************************************************/

#include <portab.h>
#include <string.h>

#define EMS_INT 0x67          /* interrupt of EM-mangager                     */

GLOBAL WORD  emm_error;       /* holds last ems-error                         */
GLOBAL UWORD emm_bx;          /* return code of bx                            */
GLOBAL UWORD emm_dx;          /* return code of dx                            */
GLOBAL UWORD emm_ax;          /* return code of ax                            */

MLOCAL BYTE *emm_name = "EMMXXXX0"; /* emm identification string              */

/***********************************************************************
 *    ems_int()
 *
 *    call EMS-manager EMM
 *
 *  Input   :  function-code, handle, logical page, physical-page
 *  Output  :  TRUE or FALSE
 ***********************************************************************/

#if HIGH_C

#include <msdos.cf>

GLOBAL WORD ems_int(fkt,dx,bx,al)
WORD fkt;
WORD dx;
WORD bx;
WORD al;
{
   Registers.AX.LH.H = fkt;
   Registers.AX.LH.L = al;
   Registers.BX.R    = bx;
   Registers.DX.R    = dx;

   callint(EMS_INT);

   emm_ax = Registers.AX.R;
   emm_bx = Registers.BX.R;
   emm_dx = Registers.DX.R;

   return ((emm_error = emm_ax) ? FALSE : TRUE);
}

#else /* HIGH_C */

#include <dos.h>

MLOCAL union REGS regs;       /* Prozessorregister fÅr den Interruptaufruf    */
MLOCAL struct SREGS sregs;    /* Segmentregister fÅr den Interrupt-Aufruf     */

GLOBAL WORD ems_int(fkt,dx,bx,al)
WORD fkt;
WORD dx;
WORD bx;
WORD al;
{
   regs.h.ah = fkt;
   regs.h.al = al;
   regs.x.bx = bx;
   regs.x.dx = dx;

   int86(EMS_INT, &regs, &regs);

   emm_ax = regs.x.ax;
   emm_bx = regs.x.bx;
   emm_dx = regs.x.dx;

   return ((emm_error = emm_ax) ? FALSE : TRUE);
}

#endif /* HIGH_C */

/*
 * strmcmp for small memory model
 */

MLOCAL WORD ems_strncmp(p1, p2, len)
BYTE FAR *p1;
BYTE *p2;
WORD len;
{
   while (len--)
   {
      if (*p1 != *p2)
         return(TRUE);
   }
   return(FALSE);
}

/***********************************************************************
 *    ems_inst();
 * 
 *    Check, whether EMS-manager is installed
 *
 * Input    :  none
 * Output   :  TRUE, EMM installed
 *             FALSE, EMM not installed
 ***********************************************************************/

#if HIGH_C

GLOBAL WORD ems_inst()
{
   REG BYTE FAR *p;

   Registers.AX.R = 0x3567;         /* Get interrupt-vector 0x67           */
   calldos();                       /* Call DOS                            */
   p = MKFP(Registers.ES.R, 10);         /* Create pointer                      */

   return((!ems_strncmp(p,emm_name,strlen(emm_name))) ? TRUE : FALSE);
 }

#else /* HIGH_C */

GLOBAL WORD ems_inst()
{
   REG BYTE far *p;

   regs.x.ax = 0x3567;              /* Get interrupt-vector 0x67           */
   intdosx(&regs, &regs, &sregs);   /* Call DOS                            */
   p = MKFP(sregs.es, 10);         /* Create pointer                      */

   return((!ems_strncmp(p,emm_name,strlen(emm_name))) ? TRUE : FALSE);
 }

#endif /* HIGH_C */

/***********************************************************************
 *    ems_errcode
 *
 *    return error-code of last EMS-operation
 *
 * Input    :  none
 * Output   :  error-code or 0 if none available
 ***********************************************************************/

GLOBAL WORD ems_errcode()
{
   return(emm_error);
}

