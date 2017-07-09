/********************************************************************
*
*     GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*     Filename: META_F.C
*
*     Date: 08/01/89
*
**********************************************************************/

/*------------ XGEM Include - Data ----------------------------------*/
#include "portab.h"              /* portable coding conversion       */
#include "aes.h"

/*------------ KURVE Include - Data ---------------------------------*/
#include "window.h"     /* Definitions for Window-Tools              */
#include "meta.h"       /* Definitions for Meta-Functions            */
#include "types.h"      
#include "proto.h"

/*-------------------- External Data -----------------------------------*/
EXTERN W_TYP w_[];

BYTE  *klick_name   = "KLICK";

#define MAX_XY_WERTE  128
#define TEST            0             
#define NO_KLICK        0
#define KLICKFELD       1

/*----------------------------------------------------------------------*/
/* meta_find_objekts:                                                    */
/*----------------------------------------------------------------------*/
WORD meta_find_objekts (w_nr, meta_buffer)
WORD w_nr;
WORD FAR *meta_buffer;
{ 
   WORD  opcode, subopcode;      
   WORD  n, m;                   
   WORD  i,j;                    
   K_TYP *k_;
   WORD  klick_nr;
   WORD  ziffer1, ziffer2;
   WORD  status;
   WORD  pos;

   if (meta_buffer[0] != METAFILE_KENNUNG) /* Metafile - Kennung Ok ?   */
      return (FALSE);
   meta_buffer += meta_buffer [1];
   status = NO_KLICK;
   k_ = w_[w_nr].klick_felder;
   while (meta_buffer[0] != METAFILE_KENNUNG)
   {
      opcode = *meta_buffer; meta_buffer ++;
      n = *meta_buffer; meta_buffer ++;
      m = *meta_buffer; meta_buffer ++;
      subopcode   = *meta_buffer; meta_buffer ++;
      if ((n > MAX_XY_WERTE) || (m > MAX_XY_WERTE))
         return (FALSE);  
      if ((opcode == GDP) && (subopcode == V_JUSTIFIED))
      {
        if (status == NO_KLICK)
         {
            i = 0;
            j = 6; /* 1 + n*2 + 1 */
          while ((meta_buffer [i+j] == klick_name [i]) && (klick_name [i] != 0))
               i++;
            ziffer1 = (BYTE) meta_buffer [i + j] - 48;
            ziffer2 = (BYTE) meta_buffer [i + 1 + j] - 48;
           if ((klick_name[i] == 0) && ((ziffer1 >= 0) || (ziffer1 < 10))
                                          && ((ziffer2 >= 0) || (ziffer2 < 10)))
            {
               klick_nr = ziffer1 * 10 + ziffer2;
               if (w_[w_nr].klick_feld_anzahl > klick_nr)
                  status = KLICKFELD;
            }
         }
      }
      else
      {
         if ((opcode == V_PLINE) && (n == 5)) 
         {
            if (status == KLICKFELD)
            {
               k_[klick_nr].x_min = meta_buffer[0];
               k_[klick_nr].y_min = meta_buffer[5];
               k_[klick_nr].x_max = meta_buffer[2];
               k_[klick_nr].y_max = meta_buffer[1];
               k_[klick_nr].init = TRUE;
               if (k_[klick_nr].x_min > k_[klick_nr].x_max) 
               {
                  pos = k_[klick_nr].x_min;
                  k_[klick_nr].x_min = k_[klick_nr].x_max;
                  k_[klick_nr].x_max = pos;
               }
               if (k_[klick_nr].y_min > k_[klick_nr].y_max)
               {
                  pos = k_[klick_nr].y_min;
                  k_[klick_nr].y_min = k_[klick_nr].y_max;
                  k_[klick_nr].y_max = pos;
               }
               status = NO_KLICK;
            }
         }
      }
      meta_buffer += n * 2 + m;
   }    /* end of while */
   return (TRUE);
}
