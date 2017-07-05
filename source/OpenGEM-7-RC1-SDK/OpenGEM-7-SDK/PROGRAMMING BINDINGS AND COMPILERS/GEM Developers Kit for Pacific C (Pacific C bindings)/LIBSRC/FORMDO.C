/*****************************************************************************
 * form_do()
 *
 *	-   The source to form_do() is provided to allow for 
 *          simpler application tailoring of the AES's generalized 
 *          form fill-in capability. This source code refers to two 
 *          new calls:
 *
 *          form_keybd()
 *
 *          form_button()
 *
 *
 *    NOTE: Use this code only if the normal form_do code doesn't suit 
 *          your needs.  (_AND AT YOUR OWN RISK_)
 *
 *
 *
 *          form_do() is as follows:
 *
 *****************************************************************************/

#include "ppdgem.h"

#define FMD_FORWARD  0
#define FMD_BACKWARD 1
#define FMD_DEFLT    2
#define FMD_CONTROL  3
#define FMD_END      4

#define EXFL_CONTROL 0x0200
#define EXFL_END     0x0400

MLOCAL WORD find_obj    _((OBJECT FAR *tree, WORD start_obj, WORD which,
                           WORD extype));
MLOCAL WORD fm_inifld   _((OBJECT FAR *tree, WORD start_fld));

/*
 * routine to scan an object tree for a specified
 * ob_flags or for a given extended object type.
 */

MLOCAL WORD find_obj(tree, start_obj, which, extype)
REG OBJECT FAR *tree;
WORD start_obj;
WORD which;
WORD extype;
{
   REG WORD obj, flag, theflag, inc;

   obj = 0;
   flag = EDITABLE;
   inc = 1;
   switch(which)
   {
      case FMD_BACKWARD:
         inc = -1;
      /* fall thru */
      case FMD_FORWARD:
         obj = start_obj + inc;
         break;

      case FMD_DEFLT:
         flag = DEFAULT;
         break;

      case FMD_CONTROL:
         flag = EXFL_CONTROL;
         break;

      case FMD_END:
         flag = EXFL_END;
         break;
   }

   while ( obj >= 0 )
   {
      theflag = tree[obj].ob_flags;
      if (flag)
      {
         if (theflag & flag && !(theflag & HIDETREE))
         {
            if (!extype)
               return(obj);
            if ((tree[obj].ob_type >> 8) == extype)
               return(obj);
         }
      }

      if (theflag & LASTOB)
         obj = -1;
      else
         obj += inc;
   }

   return(start_obj);
}

MLOCAL WORD fm_inifld(tree, start_fld)

OBJECT FAR *tree;
WORD start_fld;
{
/*
 * position cursor on the starting field
 */
   if (start_fld == 0)
      start_fld = find_obj(tree, 0, FMD_FORWARD, 0);

   return( start_fld );
}

GLOBAL WORD form_exdo(LPTREE tree, WORD start_fld)
{
   REG WORD edit_obj;
       WORD next_obj;
       WORD which, cont;
       WORD idx;
       WORD mx, my, mb, ks, br;
       UWORD kr;

   wind_update(BEG_UPDATE);
   wind_update(BEG_MCTRL);
/*
 * set starting field to edit, if want
 * first editing field then walk tree
 */
   next_obj = fm_inifld(tree, start_fld);
   edit_obj = 0;
/*
 * interact with user
 */
   cont = TRUE;
   while(cont)
   {
/*
 * position cursor on the selected editting field
 */
      if ( (next_obj != 0) && (edit_obj != next_obj) )
      {
         edit_obj = next_obj;
         next_obj = 0;
         objc_edit(tree, edit_obj, 0, &idx, EDINIT);
      }
/*
 * wait for mouse or key
 */
      which = evnt_multi(MU_KEYBD | MU_BUTTON, 
                         0x02, 0x01, 0x01, 
                         0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0,
                         0x0L,
                         0, 0,
                         (UWORD *)&mx, (UWORD *)&my, (UWORD *)&mb, 
                         (UWORD *)&ks, &kr, (UWORD *)&br);
/*
 * handle keyboard event
 */
     if (which & MU_KEYBD)
     {
         switch( kr )
         {
/*
 * translate a return into a cursor down if it is not a shift-return
 */
            case 0x1c0d:
               if (!(ks & (K_RSHIFT|K_LSHIFT)))
                  kr = 0x5000;
               break;

            default:
               break;
         }

         if ((ks & K_CTRL) || kr == 0x4f00)
         {
/*
 * if its the end key, search for an object with the EXFL_END flag
 */
            if (kr == 0x4f00)
               next_obj = find_obj(tree, 0, FMD_END, 0);
/*
 * else search for an object containing the kr value in extended type
 */
            else
               next_obj = find_obj(tree, 0, FMD_CONTROL, kr & 0x1f);
            if (next_obj != -1)
            {
               br = (ks & (K_RSHIFT|K_LSHIFT)) ? 2 : 1;
               cont = form_button(tree, next_obj, br, &next_obj);
            }
         }
         else
         {
            cont = form_keybd(tree, edit_obj, next_obj, kr, &next_obj, (WORD *)&kr);
            if (kr)
               objc_edit(tree, edit_obj, kr, &idx, EDCHAR);
         }
     }
/*
 * handle button event
 */
     if (which & MU_BUTTON)
     {
         next_obj = objc_find(tree, ROOT, MAX_DEPTH, mx, my);
         if (next_obj == -1)
         {
/*
 * sound(TRUE, 440, 2);
 */
            next_obj = 0;
         }
         else
            cont = form_button(tree, next_obj, br, &next_obj);
      }
/*
 * handle end of field  clean up
 */
      if ( (!cont) || ((next_obj != 0) && (next_obj != edit_obj)) )
         objc_edit(tree, edit_obj, 0, &idx, EDEND);
   }

   wind_update(END_MCTRL);
   wind_update(END_UPDATE);
/*
 * return exit object hi bit may be set
 * if exit obj. was double-clicked
 */
   return(next_obj);
}

/*
 * NOTE:   This is the end of the source code for form_do.  Remember this 
 *         code was provided for you to use or modify in the writing of 
 *         your own code to perform similar functions.  BEWARE, check your 
 *         code carefully !!
 */
