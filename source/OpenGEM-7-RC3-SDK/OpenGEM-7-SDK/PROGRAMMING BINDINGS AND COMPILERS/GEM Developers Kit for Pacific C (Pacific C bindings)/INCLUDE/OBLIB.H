/*****************************************************************************/
/*   OBLIB.H Common OBLIB definitions                                        */
/*****************************************************************************/
/*                                                                           */
/*    GEM/3 Programmers Toolkit - Digital Research, Inc.                     */
/*                                                                           */
/*    Author:  Robert Schneider                                              */
/*                                                                           */
/*****************************************************************************/

#ifndef __OBLIB__
#define __OBLIB__

VOID  ob_dostate        _((OBJECT FAR *tree, WORD index, WORD state));
VOID  ob_undostate      _((OBJECT FAR *tree, WORD index, WORD state));
WORD  ob_isstate        _((OBJECT FAR *tree, WORD index, WORD state));
VOID  ob_doflag         _((OBJECT FAR *tree, WORD index, WORD flag));
VOID  ob_undoflag       _((OBJECT FAR *tree, WORD index, WORD flag));
WORD  ob_isflag         _((OBJECT FAR *tree, WORD index, WORD flag));
VOID  ob_xywh           _((OBJECT FAR *tree, WORD index, GRECT FAR *rec));
BYTE FAR *ob_get_text   _((OBJECT FAR *tree, WORD index, WORD clear));
VOID  ob_set_text       _((OBJECT FAR *tree, WORD index, BYTE FAR *p));
VOID  ob_draw_dialog    _((OBJECT FAR *tree, WORD xl, WORD yl,
                           WORD wl, WORD hl));
VOID  ob_undraw_dialog  _((OBJECT FAR *tree, WORD xl, WORD yl,
                           WORD wl, WORD hl));

#endif /* __OBLIB__ */
