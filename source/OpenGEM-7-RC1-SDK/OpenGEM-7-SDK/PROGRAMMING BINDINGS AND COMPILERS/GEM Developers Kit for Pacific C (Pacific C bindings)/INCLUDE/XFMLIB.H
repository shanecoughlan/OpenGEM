/*****************************************************************************/
/*   XLIB.H   -  Common XLIB definitions                                     */
/*                                                                           */
/*   GEM/3 Programmers Toolkit - Digital Research, Inc.                      */
/*                                                                           */
/*****************************************************************************/

#ifndef __XLIB__
#define __XLIB__

WORD  x_ytox         _((WORD y));
WORD  x_udx_xform    _((WORD user_x));
WORD  x_udy_xform    _((WORD user_y));
WORD  x_dux_xform    _((WORD x));
WORD  x_duy_xform    _((WORD y));
WORD  x_udx_scale    _((WORD user_dx));
WORD  x_udy_scale    _((WORD user_dy));
WORD  x_dux_scale    _((WORD dev_x));
WORD  x_duy_scale    _((WORD dev_y));
WORD  x_sxform       _((WORD user_x, WORD user_y, WORD user_w, WORD user_h, 
                        WORD dev_x, WORD dev_y, WORD dev_w, WORD dev_h));
VOID  x_saspect      _((WORD user_w, WORD user_h, WORD FAR *dev_w,
                        WORD FAR *dev_h, WORD w_microns, WORD h_microns));
WORD  x_mul_div      _((WORD m1, WORD m2, WORD d1));

#endif /* __XLIB__ */
