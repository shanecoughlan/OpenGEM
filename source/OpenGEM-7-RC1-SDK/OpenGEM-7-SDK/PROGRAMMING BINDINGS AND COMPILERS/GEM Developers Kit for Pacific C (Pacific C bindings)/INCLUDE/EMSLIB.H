/*****************************************************************************/
/*   EMSLIB.H  -  Common EMSLIB definitions                                  */
/*****************************************************************************/
/*                                                                           */
/*    GEM/3 Programmers Toolkit - Digital Research, Inc.                     */
/*                                                                           */
/*    Author:  Robert Schneider                                              */
/*                                                                           */
/*****************************************************************************/

#ifndef __EMSLIB__
#define __EMSLIB__

WORD  ems_int           _((WORD fkt, WORD dx, WORD bx, WORD al));
WORD  ems_inst          _((VOID));
WORD  ems_errcode       _((VOID));
WORD  ems_num_page      _((VOID));
WORD  ems_free_page     _((VOID));
WORD  ems_frame_seg     _((VOID));
WORD  ems_alloc         _((WORD pages));
WORD  ems_map           _((WORD handle, WORD logp, WORD physp));
WORD  ems_free          _((WORD handle));
WORD  ems_version       _((VOID));
WORD  ems_save_map      _((WORD handle));
WORD  ems_restore_map   _((WORD handle));

#endif /* __EMSLIB__ */
