/************************************************************************
*
*     GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*     Filename: META_OUT.C
*
*     Date: 08/01/89
*
*************************************************************************/

/*-------------------- XGEM Include - Data -----------------------------*/
#include "portab.h"
#include "aes.h"
#include "vdi.h"

/*-------------------- KURVE Include - Data ----------------------------*/
#include "meta.h"                /* Definitions for Meta-Functions      */
#include "types.h"
#include "proto.h"

/*----------------------------------------------------------------------*/
/* meta_out:
/*----------------------------------------------------------------------*/

WORD meta_out ( vdi_handle, meta_buffer, x_off, y_off )
WORD vdi_handle;        /* VDI Handle  */
WORD FAR *meta_buffer;
WORD x_off, y_off;      /* X  Y - Offset Positions  */
{
   WORD xy [ MAX_POINTS ];
   WORD i;
   WORD dummy;
   BYTE str[128];

   meta_buffer += meta_buffer[1];

   while ( meta_buffer[0] != METAFILE_KENNUNG )
   {
      switch ( meta_buffer[0] )
      {
         case V_PLINE:
            if ( meta_buffer[1] < MAX_POINTS )
            {
               for ( i=0; i < meta_buffer[1]*2; i+=2 )
               {
                  xy[i]   = meta_buffer[4+i] + x_off;
                  xy[i+1] = meta_buffer[5+i] + y_off;
               }
               v_pline ( vdi_handle, meta_buffer[1], xy );
            }
            else
               return ( FALSE );
            break;

         case V_PMARKER:
            if ( meta_buffer[1] < MAX_POINTS )
            {
               for ( i=0; i < meta_buffer[1]*2; i+=2 )
               {
                  xy[i]   = meta_buffer[4+i] + x_off;
                  xy[i+1] = meta_buffer[5+i] + y_off;
               }
               v_pmarker ( vdi_handle, meta_buffer[1], xy );
            }
            else
               return ( FALSE );
            break;
         case V_GTEXT:
            LBCOPY(ADDR(str), ADDR(&meta_buffer[6]), LSTRLEN((LPBYTE)(&meta_buffer[6]))); 
            v_gtext( vdi_handle, meta_buffer[4]+x_off, meta_buffer[5]+y_off,
                                                str );
            break;
         case V_FILLAREA:
            if ( meta_buffer[1] < MAX_POINTS )
            {
               for ( i=0; i < meta_buffer[1]*2; i+=2 )
               {
                  xy[i]   = meta_buffer[4+i] + x_off;
                  xy[i+1] = meta_buffer[5+i] + y_off;
               }
               v_fillarea ( vdi_handle, meta_buffer[1], xy );
            }
            else
               return ( FALSE );
            break;
         case GDP:
            switch ( meta_buffer[3] )
            {
               case V_BAR:
                  xy[0] = meta_buffer[4] + x_off;
                  xy[1] = meta_buffer[5] + y_off;
                  xy[2] = meta_buffer[6] + x_off;
                  xy[3] = meta_buffer[7] + y_off;
                  v_bar ( vdi_handle, xy );
                  break;
               case V_ARC:
                  v_arc ( vdi_handle, meta_buffer[4]+x_off,
                           meta_buffer[5]+y_off, meta_buffer[10],
                           meta_buffer[12], meta_buffer[13] );
                  break;
               case V_PIESLICE:
                  v_pieslice ( vdi_handle, meta_buffer[4]+x_off,
                               meta_buffer[5]+y_off, meta_buffer[10],
                               meta_buffer[12], meta_buffer[13] );
                  break;
               case V_CIRCLE:
                  v_circle ( vdi_handle, meta_buffer[4]+x_off,
                                meta_buffer[5]+y_off, meta_buffer[8] );
                  break;
               case V_ELLARC:
                  v_ellarc ( vdi_handle, meta_buffer[4]+x_off,
                             meta_buffer[5]+y_off, meta_buffer[6],
                             meta_buffer[7], meta_buffer[8], meta_buffer[9] );
                  break;
               case V_ELLPIE:
                  v_ellpie ( vdi_handle, meta_buffer[4]+x_off,
                               meta_buffer[5]+y_off, meta_buffer[6],
                               meta_buffer[7], meta_buffer[8], meta_buffer[9] );
                  break;
               case V_ELLIPSE:
                  v_ellipse ( vdi_handle, meta_buffer[4]+x_off,
                                meta_buffer[5]+y_off, meta_buffer[6],
                                meta_buffer[7] );
                  break;
               case V_RBOX:
                  xy[0] = meta_buffer[4] + x_off;
                  xy[1] = meta_buffer[5] + y_off;
                  xy[2] = meta_buffer[6] + x_off;
                  xy[3] = meta_buffer[7] + y_off;
                  v_rbox ( vdi_handle, xy );
                  break;
               case V_RFBOX:
                  xy[0] = meta_buffer[4] + x_off;
                  xy[1] = meta_buffer[5] + y_off;
                  xy[2] = meta_buffer[6] + x_off;
                  xy[3] = meta_buffer[7] + y_off;
                  v_rfbox ( vdi_handle, xy );
                  break;
               case V_JUSTIFIED:
                  i=0;
                  while (str[i] = (BYTE)meta_buffer[i+10]) 
                  i++;
                  v_gtext ( vdi_handle, meta_buffer[4]+x_off,
                             meta_buffer[5]+y_off, str );
                  break;
               }
               break;

            case VST_HEIGHT:
               vst_height ( vdi_handle, meta_buffer[5], &dummy, &dummy,
                                                            &dummy, &dummy );
               break;
            case VST_ROTATION:
               vst_rotation ( vdi_handle, meta_buffer[4] );
               break;
            case VS_COLOR:
			   for (i = 0; i < 3; i++) xy[i] = meta_buffer[5+i];
               vs_color ( vdi_handle, meta_buffer[4], xy );
               break;
            case VSL_TYPE:
               vsl_type ( vdi_handle, meta_buffer[4] );
               break;
            case VSL_WIDTH:
               vsl_width ( vdi_handle, meta_buffer[4] );
               break;
            case VSL_COLOR:
               vsl_color ( vdi_handle, meta_buffer[4] );
               break;
            case VSM_TYPE:
               vsm_type ( vdi_handle, meta_buffer[4] );
               break;
            case VSM_HEIGHT:
               vsm_height ( vdi_handle, meta_buffer[4] );
               break;
            case VSM_COLOR:
               vsm_color ( vdi_handle, meta_buffer[4] );
               break;
            case VST_FONT:
               vst_font ( vdi_handle, meta_buffer[4] );
               break;
            case VST_COLOR:
               vst_color ( vdi_handle, meta_buffer[4] );
               break;
            case VSF_INTERIOR:
               vsf_interior ( vdi_handle, meta_buffer[4] );
               break;
            case VSF_STYLE:
               vsf_style ( vdi_handle, meta_buffer[4] );
               break;
            case VSF_COLOR:
               vsf_color ( vdi_handle, meta_buffer[4] );
               break;
            case VSWR_MODE:
               vswr_mode ( vdi_handle, meta_buffer[4] );
               break;
            case VST_ALIGNMENT:
               vst_alignment ( vdi_handle, meta_buffer[4], meta_buffer[5],
                                                           &dummy, &dummy );
               break;
            case VSF_PERIMETER:
               vsf_perimeter ( vdi_handle, meta_buffer[4] );
               break;
            case VST_EFFECTS:
               vst_effects ( vdi_handle, meta_buffer[4] );
               break;
            case VST_POINT:
               vst_point ( vdi_handle, meta_buffer[4], &dummy, &dummy,
                                                             &dummy, &dummy );
               break;
            case VSL_END_STYLE:
               vsl_ends ( vdi_handle, meta_buffer[4], meta_buffer[5] );
               break;
            case VSF_UDPAT:
               for (i = 0; i < 16 * meta_buffer[2]; i++) xy[i] = meta_buffer[4+i];
               vsf_udpat ( vdi_handle, xy, meta_buffer[2] );
               break;
            case VSL_UDSTY:
               vsl_udsty ( vdi_handle, meta_buffer[4] );
               break;
            case VST_LOAD_FONTS:
               vst_load_fonts ( vdi_handle, meta_buffer[4] );
               break;
            case VST_UNLOAD_FONTS:
               vst_unload_fonts ( vdi_handle, meta_buffer[4] );
               break;
            case VS_CLIPP:
			   for (i = 0; i < 4; i++) xy[i] = meta_buffer[4+i];
               vs_clip ( vdi_handle, meta_buffer[8], xy );
               break;
         } /* end of switch */

         meta_buffer += 4 + meta_buffer[1]*2 + meta_buffer[2];

   } /* end of while   */

   return ( TRUE );
}

