/**********************************************************************
*
*     GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*     Filename: PROTO.H
*
*     Date: 08/01/89
*
***********************************************************************/

EXTERN	MEVENT	mevent;
EXTERN	WORD	gl_rmsg[];
EXTERN	WORD	x_mouse_pos;
EXTERN	WORD	y_mouse_pos;
EXTERN	WORD	ev_which;

EXTERN  VOID init_event _((MEVENT FAR *e, WORD *msg));
EXTERN  VOID init_window _((VOID));
EXTERN  VOID handle_button _((MEVENT FAR *mevent));
EXTERN  VOID handle_keyboard _((MEVENT FAR *mevent));
EXTERN  VOID handle_timer _((MEVENT FAR *mevent));
EXTERN  VOID handle_m1 _((MEVENT FAR *mevent));
EXTERN  VOID handle_m2 _((MEVENT FAR *mevent));
EXTERN  VOID handle_message _((MEVENT FAR *mevent));
EXTERN  VOID handle_menu _((MEVENT FAR *mevent));
EXTERN  VOID draw_event_return _((WORD ev_which, MEVENT FAR *mevent));
EXTERN  WORD handle_event _((WORD ev_which, MEVENT FAR *mevent));
EXTERN  WORD meta_find_objekts _((WORD w_nr, WORD FAR *meta_buffer));
EXTERN  WORD FAR *meta_read _((BYTE *meta_file_name));
EXTERN  WORD meta_trans _((WORD FAR *meta_buffer,WORD v_x,WORD v_y,WORD v_w,WORD v_h));
EXTERN  WORD window_open _((WORD w_nr));
EXTERN  VOID window_close _((WORD w_nr));
EXTERN  VOID draw _((WORD w_nr, WORD x, WORD y, WORD w, WORD h));
EXTERN  VOID redraw _((WORD w_nr, WORD x, WORD y, WORD w, WORD h));
EXTERN  WORD dialog_box _((WORD dialogbox_nr));
EXTERN  WORD find_window _((WORD handle));
EXTERN  WORD find_click_area _((VOID));
EXTERN  VOID handle_window _((MEVENT FAR *mevent));
EXTERN  WORD init _((VOID));
EXTERN  VOID init_trans _((VOID));
EXTERN  VOID init_klickfelder _((K_TYP *k_, WORD n));
EXTERN  VOID term _((VOID));
EXTERN  VOID dialog _((WORD window_nr));
EXTERN  VOID main _((VOID));
EXTERN  WORD meta_out _((WORD vdi_handle,WORD FAR *meta_buffer,WORD x_off,WORD y_off));
