/*************************************************************************
*
*     GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*     Filename: META_R.C
*
*     Date: 08/01/89
*
**************************************************************************/

/*-------------------- XGEM Include - Data ------------------------------*/
#include "portab.h"
#include "aes.h"

/*-------------------- KURVE Include - Data ----------------------------*/
#include "window.h"		/* Definitions for Window-Tools	        */
#include "meta.h"		/* Definitions for Meta-Functions	*/
#include "types.h"
#include "proto.h"


/*-------------------- External XGEMBIND Data --------------------------*/
EXTERN UWORD DOS_ERR;

/*----------------------------------------------------------------------*/
/* meta_read():                                                         */
/*----------------------------------------------------------------------*/
WORD FAR *meta_read( meta_file_name )
BYTE *meta_file_name;
{
   WORD file_handle;
   WORD FAR *meta_buffer;

   meta_buffer = dos_alloc ( MAX_META_SIZE );
   if ( meta_buffer == 0l )
   {
      graf_mouse ( ARROW, 0L );
      form_alert ( 1, "[3][No more memory][ Ok ]" );
   }
   else
   {
      file_handle = dos_open ( meta_file_name, 0 );
      if ( DOS_ERR )
      {
         graf_mouse ( ARROW, 0L );
         form_alert ( 1,"[3][File not open][ Ok ]");
      }
      else
      {
         dos_read ( file_handle, MAX_META_SIZE, ADDR(meta_buffer) );
         if ( DOS_ERR )
         {
            graf_mouse ( ARROW, 0L );
            form_alert ( 1,"[3][File read error][ Ok ]");
         }
         else
         {
            dos_close ( file_handle );
            if ( DOS_ERR )
            {
               graf_mouse ( ARROW, 0L );
               form_alert ( 1,"[3][File not closed][ Ok ]");
            }
            else
            {
               return ( meta_buffer );
            }
         }
      }
   }
   return ( (WORD *) 0xFFFFFFFF );
}

