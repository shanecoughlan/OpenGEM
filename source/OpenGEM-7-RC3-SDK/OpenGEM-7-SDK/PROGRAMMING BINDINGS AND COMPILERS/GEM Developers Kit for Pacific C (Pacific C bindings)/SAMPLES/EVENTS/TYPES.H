/*************************************************************************
*
*     GEM/3 Programmers Toolkit - Digital Research, Inc.
*
*     Filename: TYPES.H
*
*     Date: 08/01/89
*
**************************************************************************/

typedef struct

{
  WORD    x_min;
  WORD    y_min;
  WORD    x_max;
  WORD    y_max;

  WORD    init;       
		
  WORD    e_nr;
	
  WORD    invers;
		
} K_TYP;


typedef struct

{
  WORD    handle;			/* Window handle		*/
  WORD    kind;
  BYTE    *name;			/* Pointer to Window name	*/

  BYTE    *meta_name;			/* Pointer to Meta data name	*/
  WORD FAR *meta_buffer;		/* Pointer to Metabuffer	*/

  WORD    klick_feld_anzahl;
  K_TYP   *klick_felder;
  WORD    k_nr;
  WORD    alt_k_nr;

  WORD    init;
	
  WORD    redrawed;
		

  WORD    x_npos;	/* relative Window position */
  WORD    y_npos;
  WORD    nwidth;	/* relative Window width */
  WORD    nheight;

  WORD    x_fpos;	/* Window coordinates */
  WORD    y_fpos;
  WORD    fwidth;
  WORD    fheight;

  WORD    x_wfpos;
  WORD    y_wfpos;
  WORD    wfwidth;
  WORD    wfheight;

  WORD    x_wpos;
  WORD    y_wpos;
  WORD    wwidth;
  WORD    wheight;

  WORD    x_move;
  WORD    y_move;

  WORD    h_slider_pos;			/* x - Slider position		*/
  WORD    v_slider_pos;			/* y - Slider position		*/
  WORD    y_slider_size;		/* Vertical slider size   	*/
  WORD    x_slider_size;		/* Horizontal slider size	*/

  WORD    full;
	

  WORD    x_alt;
  WORD    y_alt;
  WORD    w_alt;
  WORD    h_alt;

  WORD    x_move_alt;
  WORD    y_move_alt;

  WORD    v_ss_alt;
  WORD    h_ss_alt;
  WORD    v_sp_alt;
  WORD    h_sp_alt;

} W_TYP;

