/************************************************************************
*
*     GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*     Filename: META_T.C
*
*     Date: 08/01/89
*
*************************************************************************/

/*-------------------- GEM Include Files -------------------------------*/
#include "portab.h"			/* portable coding conventions	*/
#include "aes.h"

#include "meta.h"
#include "types.h"
#include "proto.h"

/*----------------------------------------------------------------------*/
/* meta_trans:		                                                */
/*----------------------------------------------------------------------*/

WORD meta_trans(meta_buffer, v_x, v_y, v_w, v_h)
WORD FAR *meta_buffer;			/* Pointer to Metafile buffer	*/
WORD v_x, v_y, v_w, v_h;
{
   WORD i;
   WORD x, y;
   LONG m_w, m_h;
   WORD FAR *mb;

   mb = meta_buffer;

   x=-meta_buffer[4];
   y=-meta_buffer[5];
   m_w=(LONG)meta_buffer[6]-(LONG)meta_buffer[4];
   m_h=(LONG)meta_buffer[7]-(LONG)meta_buffer[5];

   meta_buffer+=meta_buffer[1];

   while(meta_buffer[0]!=METAFILE_KENNUNG)
   {
      switch(meta_buffer[0])
      {
       case V_PLINE:
          if (meta_buffer[1]< MAX_POINTS)
            {
             for (i=0; i <(meta_buffer[1]* 2); i+=2)
             {
        meta_buffer[4+i]=(WORD)(((LONG)(meta_buffer[4+i]+x)*(LONG)v_w)/m_w)+v_x;
        meta_buffer[5+i]=(WORD)(((LONG)(meta_buffer[5+i]+y)*(LONG)v_h)/m_h)+v_y;
              }
            }
            else
               return(FALSE);
            break;
         case V_PMARKER:
          if (meta_buffer[1]< MAX_POINTS)
            {
           for (i=0; i <(meta_buffer[1]* 2); i+=2)
              {
        meta_buffer[4+i]=(WORD)(((LONG)(meta_buffer[4+i]+x)*(LONG)v_w)/m_w)+v_x;
        meta_buffer[5+i]=(WORD)(((LONG)(meta_buffer[5+i]+y)*(LONG)v_h)/m_h)+v_y;
              }
            }
            else
               return(FALSE);
            break;
         case V_GTEXT:
            meta_buffer[4]=(WORD)(((LONG)(meta_buffer[4]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[5]=(WORD)(((LONG)(meta_buffer[5]+y)*(LONG)v_h)/m_h)+v_y;
            break;
         case V_FILLAREA:
            if (meta_buffer[1]< MAX_POINTS)
              {
               for (i=0; i <(meta_buffer[1]* 2); i+=2)
                  {
        meta_buffer[4+i]=(WORD)(((LONG)(meta_buffer[4+i]+x)*(LONG)v_w)/m_w)+v_x;
        meta_buffer[5+i]=(WORD)(((LONG)(meta_buffer[5+i]+y)*(LONG)v_h)/m_h)+v_y;
                  }
              }
            else
               return(FALSE);
            break;
         case GDP:
          switch(meta_buffer[3])
            {
          case V_BAR:
            meta_buffer[4]=(WORD)(((LONG)(meta_buffer[4]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[5]=(WORD)(((LONG)(meta_buffer[5]+y)*(LONG)v_h)/m_h)+v_y;
            meta_buffer[6]=(WORD)(((LONG)(meta_buffer[6]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[7]=(WORD)(((LONG)(meta_buffer[7]+y)*(LONG)v_h)/m_h)+v_y;
              break;
          case V_ARC:
            meta_buffer[4]=(WORD)(((LONG)(meta_buffer[4]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[5]=(WORD)(((LONG)(meta_buffer[5]+y)*(LONG)v_h)/m_h)+v_y;
            meta_buffer[10]=(WORD)(((LONG)meta_buffer[10]*(LONG)v_w)/m_w);
              break;
          case V_PIESLICE:
            meta_buffer[4]=(WORD)(((LONG)(meta_buffer[4]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[5]=(WORD)(((LONG)(meta_buffer[5]+y)*(LONG)v_h)/m_h)+v_y;
            meta_buffer[10]=(WORD)(((LONG)meta_buffer[10]*(LONG)v_w)/m_w);
              break;
          case V_CIRCLE:
            meta_buffer[4]=(WORD)(((LONG)(meta_buffer[4]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[5]=(WORD)(((LONG)(meta_buffer[5]+y)*(LONG)v_h)/m_h)+v_y;
            meta_buffer[8]=(WORD)(((LONG)meta_buffer[8]*(LONG)v_w)/m_w);
              break;
          case V_ELLARC:
            meta_buffer[4]=(WORD)(((LONG)(meta_buffer[4]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[5]=(WORD)(((LONG)(meta_buffer[5]+y)*(LONG)v_h)/m_h)+v_y;
            meta_buffer[6]=(WORD)(((LONG)meta_buffer[6]*(LONG)v_w)/m_w);
            meta_buffer[7]=(WORD)(((LONG)meta_buffer[7]*(LONG)v_h)/m_h);
              break;
          case V_ELLPIE:
            meta_buffer[4]=(WORD)(((LONG)(meta_buffer[4]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[5]=(WORD)(((LONG)(meta_buffer[5]+y)*(LONG)v_h)/m_h)+v_y;
            meta_buffer[6]=(WORD)(((LONG)meta_buffer[6]*(LONG)v_w)/m_w);
            meta_buffer[7]=(WORD)(((LONG)meta_buffer[7]*(LONG)v_h)/m_h);
              break;
          case V_ELLIPSE:
            meta_buffer[4]=(WORD)(((LONG)(meta_buffer[4]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[5]=(WORD)(((LONG)(meta_buffer[5]+y)*(LONG)v_h)/m_h)+v_y;
            meta_buffer[6]=(WORD)(((LONG)meta_buffer[6]*(LONG)v_w)/m_w);
            meta_buffer[7]=(WORD)(((LONG)meta_buffer[7]*(LONG)v_h)/m_h);
              break;
          case V_RBOX:
            meta_buffer[4]=(WORD)(((LONG)(meta_buffer[4]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[5]=(WORD)(((LONG)(meta_buffer[5]+y)*(LONG)v_h)/m_h)+v_y;
            meta_buffer[6]=(WORD)(((LONG)(meta_buffer[6]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[7]=(WORD)(((LONG)(meta_buffer[7]+y)*(LONG)v_h)/m_h)+v_y;
              break;
          case V_RFBOX:
            meta_buffer[4]=(WORD)(((LONG)(meta_buffer[4]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[5]=(WORD)(((LONG)(meta_buffer[5]+y)*(LONG)v_h)/m_h)+v_y;
            meta_buffer[6]=(WORD)(((LONG)(meta_buffer[6]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[7]=(WORD)(((LONG)(meta_buffer[7]+y)*(LONG)v_h)/m_h)+v_y;
              break;
          case V_JUSTIFIED:
            meta_buffer[4]=(WORD)(((LONG)(meta_buffer[4]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[5]=(WORD)(((LONG)(meta_buffer[5]+y)*(LONG)v_h)/m_h)+v_y;
            meta_buffer[6]=(WORD)(((LONG)meta_buffer[6]*(LONG)v_w)/m_w);
              break;
            }
            break;
         case VST_HEIGHT:
            meta_buffer[5]=(WORD)(((LONG)meta_buffer[5]*(LONG)v_h)/m_h);
            if (meta_buffer[5]<1)
               meta_buffer[5]=1;
            break;
         case VSL_WIDTH:
            meta_buffer[4]=(WORD)(((LONG)meta_buffer[4]*(LONG)v_w)/m_w);
            if (meta_buffer[4]<1)
               meta_buffer[4]=1;
            break;
         case VSM_HEIGHT:
            meta_buffer[4]=(WORD)(((LONG)meta_buffer[4]*(LONG)v_h)/m_h);
            if (meta_buffer[4]<1)
               meta_buffer[4]=1;
            break;
         case VST_POINT:
            meta_buffer[4]=(WORD)(((LONG)meta_buffer[4]*(LONG)v_h)/m_h);
            break;
         case VS_CLIPP:
            meta_buffer[4]=(WORD)(((LONG)(meta_buffer[4]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[5]=(WORD)(((LONG)(meta_buffer[5]+y)*(LONG)v_h)/m_h)+v_y;
            meta_buffer[6]=(WORD)(((LONG)(meta_buffer[6]+x)*(LONG)v_w)/m_w)+v_x;
            meta_buffer[7]=(WORD)(((LONG)(meta_buffer[7]+y)*(LONG)v_h)/m_h)+v_y;
            break;
      }
      meta_buffer+=4+meta_buffer[1]*2+meta_buffer[2];
   }

   mb[4]=v_x;
   mb[5]=v_y;
   mb[6]=v_x+v_w;
   mb[7]=v_y+v_h;

   return(TRUE);
}
