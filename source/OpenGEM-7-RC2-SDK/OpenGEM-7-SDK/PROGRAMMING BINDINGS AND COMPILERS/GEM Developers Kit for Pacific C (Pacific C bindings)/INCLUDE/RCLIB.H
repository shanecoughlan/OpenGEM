/*****************************************************************************/
/*   RCLIB.H Common RCLIB definitions                                        */
/*****************************************************************************/
/*                                                                           */
/*    GEM/3 Programmers Toolkit - Digital Research, Inc.                     */
/*                                                                           */
/*    Author:  Robert Schneider                                              */
/*                                                                           */
/*****************************************************************************/

#ifndef __RCLIB__
#define __RCLIB__

WORD  rc_equal             _((GRECT FAR *p1, GRECT FAR *p2));
VOID  rc_copy              _((GRECT FAR *psbox, GRECT FAR *pdbox));
WORD  rc_intersect         _((GRECT FAR *p1, GRECT FAR *p2));
WORD  rc_inside            _((WORD x, WORD y, GRECT FAR *pt));
VOID  rc_grect_to_array    _((GRECT FAR *area, WORD FAR *array));

#endif /* __RCLIB__ */
