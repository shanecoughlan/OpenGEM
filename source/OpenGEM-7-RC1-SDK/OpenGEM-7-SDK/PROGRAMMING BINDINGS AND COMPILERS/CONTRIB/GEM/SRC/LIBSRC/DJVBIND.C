/*	LVDIBIND.C	7/18/84 - 4/19/85	John Grant		*/
/*	Modified VDIBIND.C for use with LARGE Model		*/
/* [JCE] modified for use with DJGPP */

#include "djgppgem.h"

extern LPLPTR pPblock;
extern LPWORD pContrl, pIntin, pIntout, pPtsin, pPtsout;
extern LPBYTE pGeneral;	/* General (4k) transfer area */

#define G_MFDB1 pGeneral
#define G_MFDB2 (pGeneral + sizeof(MFDB))

void __inline__ i_intin(LPVOID l)  { LSSET(pPblock + 4,  l); }
void __inline__ i_ptsin(LPVOID l)  { LSSET(pPblock + 8,  l); }
void __inline__ i_intout(LPVOID l) { LSSET(pPblock + 12, l); }
void __inline__ i_ptsout(LPVOID l) { LSSET(pPblock + 16, l); }
void __inline__ i_lptr1(LPVOID l)  { LSSET(pContrl + 14, l); }
void __inline__ i_lptr2(LPVOID l)  { LSSET(pContrl + 18, l); }

#define GENERIC_5(n) 			\
    LWSET(pContrl + 0, 5);		\
    LWSET(pContrl + 2, 0);		\
    LWSET(pContrl + 6, 0);		\
    LWSET(pContrl +10, n);		\
    LWSET(pContrl +12, handle);



/* Open Workstation is nearly always the first VDI call. Check that the 
 * arrays are allocated. */


WORD v_opnwk( WORD work_in[11], WORD *handle, WORD work_out[57] )
{
	if (!vdi_alloc_arrays()) return 0;

	dosmemput(work_in, 22, pGeneral);

	i_intin( pGeneral );
        i_intout( pGeneral + 22 );
        i_ptsout( pGeneral + 112 );

	LWSET(pContrl + 0, 1);
	LWSET(pContrl + 2, 0);
	LWSET(pContrl + 6, 11);
        vdi();

        *handle = LWGET(pContrl + 12);

	dosmemget(pGeneral + 22, 114, work_out);

        i_intin( pIntin );
        i_intout( pIntout );
        i_ptsout( pPtsout );
	return( TRUE ) ;
}


VOID v_clswk( WORD handle )
{
    LWSET(pContrl + 0, 2);
    LWSET(pContrl + 2, 0);
    LWSET(pContrl + 6, 0);
    LWSET(pContrl +12, handle);
    vdi();
}


VOID v_clrwk( WORD handle )
{
    LWSET(pContrl + 0, 3);
    LWSET(pContrl + 2, 0);
    LWSET(pContrl + 6, 0);
    LWSET(pContrl +12, handle);
    vdi();
}

VOID v_updwk( WORD handle )
{
    LWSET(pContrl + 0, 4);
    LWSET(pContrl + 2, 0);
    LWSET(pContrl + 6, 0);
    LWSET(pContrl +12, handle);
    vdi();
}


VOID vq_chcells( WORD handle, WORD *rows, WORD *columns )
{
    GENERIC_5(1)
    vdi();

    *rows    = LWGET(pIntout + 0);
    *columns = LWGET(pIntout + 2);
}

VOID v_exit_cur( WORD handle )
{
    GENERIC_5(2)
    vdi();
}

VOID v_enter_cur( WORD handle )
{
    GENERIC_5(3)
    vdi();
}

VOID v_curup( WORD handle )
{
    GENERIC_5(4)
    vdi();
}

VOID v_curdown( WORD handle )
{
    GENERIC_5(5)
    vdi();
}

VOID v_curright( WORD handle )
{
    GENERIC_5(6)
    vdi();
}

VOID v_curleft( WORD handle )
{
    GENERIC_5(7)
    vdi();
}

VOID v_curhome( WORD handle )
{
    GENERIC_5(8)
    vdi();
}

VOID v_eeos( WORD handle )
{
    GENERIC_5(9)
    vdi();
}

VOID v_eeol( WORD handle )
{
    GENERIC_5(10)
    vdi();
}

VOID vs_curaddress( WORD handle, WORD row, WORD column )
{
    LWSET(pIntin  + 0, row);
    LWSET(pIntin  + 2, column);

    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 0);		
    LWSET(pContrl + 6, 2);		
    LWSET(pContrl +10, 11);		
    LWSET(pContrl +12, handle);
    vdi();
}

VOID v_curtext( WORD handle, char *string )
{
    WORD i;

    i = dj_putb2w(string, pIntin);

    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 0);		
    LWSET(pContrl + 6, i);		
    LWSET(pContrl +10, 12);		
    LWSET(pContrl +12, handle);
    vdi();
}

VOID v_rvon( WORD handle )
{
    GENERIC_5(13)
    vdi();
}

VOID v_rvoff( WORD handle )
{
    GENERIC_5(14)
    vdi();
}


VOID vq_curaddress( WORD handle, WORD *row, WORD *column )
{
    GENERIC_5(15)
    vdi();

    *row    = LWGET(pIntout + 0);
    *column = LWGET(pIntout + 2);
}

WORD vq_tabstatus( WORD handle )
{
    GENERIC_5(16)
    vdi();
    return( LWGET(pIntout) );
}

VOID v_hardcopy( WORD handle )
{
    GENERIC_5(17)
    vdi();
}

VOID v_dspcur( WORD handle, WORD x, WORD y )
{
    LWSET(pPtsin + 0, x);
    LWSET(pPtsin + 2, y);

    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 1);		
    LWSET(pContrl + 6, 0);		
    LWSET(pContrl +10, 18);		
    LWSET(pContrl +12, handle);
    vdi();
}

VOID v_rmcur( WORD handle )
{
    GENERIC_5(19)
    vdi();
}

VOID v_form_adv( WORD handle )
{
    GENERIC_5(20)
    vdi();
}

VOID v_output_window( WORD handle, WORD xy[] )
{
    LWSET(pPtsin + 0, xy[0]);
    LWSET(pPtsin + 2, xy[1]);
    LWSET(pPtsin + 4, xy[2]);
    LWSET(pPtsin + 6, xy[3]);

    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 2);		
    LWSET(pContrl + 6, 0);		
    LWSET(pContrl +10, 21);		
    LWSET(pContrl +12, handle);
    vdi();
}

VOID v_clear_disp_list( WORD handle )
{
    GENERIC_5(22)
    vdi();
}

VOID v_bit_image( WORD handle, BYTE *filename, WORD aspect, WORD x_scale, 
                  WORD y_scale, WORD h_align, WORD v_align, WORD xy[] )
{
    WORD i;

    dosmemput(xy, 8, pPtsin);
    LWSET(pIntin + 0, aspect);
    LWSET(pIntin + 2, x_scale);
    LWSET(pIntin + 4, y_scale);
    LWSET(pIntin + 6, h_align);
    LWSET(pIntin + 8, v_align);
    i = 5 + dj_putb2w(filename, pIntin + 10);
    LWSET(pContrl +  0,  5);
    LWSET(pContrl +  2,  2);
    LWSET(pContrl +  6,  i);
    LWSET(pContrl + 10, 23);
    LWSET(pContrl + 12, handle);
    vdi();
}

VOID vq_scan( WORD handle, WORD *g_height, WORD *g_slice, WORD *a_height, 
                           WORD *a_slice,  WORD *factor )
{
    GENERIC_5(24)
    vdi();

    *g_height = LWGET(pIntout + 0);
    *g_slice  = LWGET(pIntout + 2);
    *a_height = LWGET(pIntout + 4);
    *a_slice  = LWGET(pIntout + 6);
    *factor   = LWGET(pIntout + 8);
}



VOID v_alpha_text( WORD handle, BYTE *string )
{
    WORD i;

    i = dj_putb2w(string, pIntin);

    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 0);		
    LWSET(pContrl + 6, i);		
    LWSET(pContrl +10, 25);		
    LWSET(pContrl +12, handle);
    vdi();
}

WORD vs_palette( WORD handle, WORD palette )
{
    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 0);		
    LWSET(pContrl + 6, 1);		
    LWSET(pContrl +10, 60);		
    LWSET(pContrl +12, handle);
    LWSET(pIntin  + 0, palette);    
    vdi();
    return( LWGET(pIntout));
}

VOID v_sound( WORD handle, WORD frequency, WORD duration )
{
    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 0);		
    LWSET(pContrl + 6, 2);		
    LWSET(pContrl +10, 61);		
    LWSET(pContrl +12, handle);

    LWSET(pIntin  + 0, frequency);
    LWSET(pIntin  + 2, duration);
    vdi();
}

WORD vs_mute( WORD handle, WORD action )
{
    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 0);		
    LWSET(pContrl + 6, 1);		
    LWSET(pContrl +10, 62);		
    LWSET(pContrl +12, handle);
    LWSET(pIntin  + 0, action);
    vdi();
    return( LWGET(pIntout));
}

VOID vsp_film( WORD handle, WORD index, WORD lightness )
{
    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 0);		
    LWSET(pContrl + 6, 2);		
    LWSET(pContrl +10, 91);		
    LWSET(pContrl +12, handle);
    LWSET(pIntin  + 0, index);
    LWSET(pIntin  + 2, lightness);
    vdi();
}

BOOLEAN vqp_filmname( WORD handle, WORD index, BYTE name[] )
{
    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 0);		
    LWSET(pContrl + 6, 1);		
    LWSET(pContrl +10, 92);		
    LWSET(pContrl +12, handle);
    LWSET(pIntin  + 0, index);
    vdi();

    if ( LWGET(pContrl + 8) != 0) 
    {
	dj_getw2b(pIntout, LWGET(pContrl + 8), name);
	return(TRUE);
    }
    return(FALSE);
}

VOID vsc_expose( WORD handle, WORD state )
{
    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 0);		
    LWSET(pContrl + 6, 2);	/* ??? Only one integer is actually set */		
    LWSET(pContrl +10, 93);		
    LWSET(pContrl +12, handle);
    LWSET(pIntin  + 0, state);
    vdi();
}

VOID v_meta_extents( WORD handle, WORD min_x, WORD min_y, WORD max_x, WORD max_y )
{
    LWSET(pPtsin + 0, min_x);
    LWSET(pPtsin + 2, min_y);
    LWSET(pPtsin + 4, max_x);
    LWSET(pPtsin + 6, max_y);

    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 2);		
    LWSET(pContrl + 6, 0);		
    LWSET(pContrl +10, 98);		
    LWSET(pContrl +12, handle);
    vdi();
}

VOID v_write_meta( WORD handle, WORD num_ints, LPWORD ints, WORD num_pts, LPWORD pts )
{
    i_intin( ints );
    i_ptsin( pts );

    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, num_pts);		
    LWSET(pContrl + 6, num_ints);		
    LWSET(pContrl +10, 99);		
    LWSET(pContrl +12, handle);
    vdi();
    i_intin( pIntin );
    i_ptsout( pPtsout );

}

VOID vm_filename( WORD handle, BYTE *filename )
{
    WORD i;

    i = dj_putb2w(filename, pIntin);
    LWSET(pContrl + 0, 5);		
    LWSET(pContrl + 2, 0);		
    LWSET(pContrl + 6, i);		
    LWSET(pContrl +10, 100);		
    LWSET(pContrl +12, handle);
    vdi();
}

VOID vm_pagesize( WORD handle, WORD pgwidth, WORD pgheight )
{
    LWSET(pContrl +  0, 5);
    LWSET(pContrl +  2, 0);
    LWSET(pContrl +  6, 3);
    LWSET(pContrl + 10, 99);
    LWSET(pContrl + 12, handle);
    LWSET(pIntin  +  0, 0);
    LWSET(pIntin  +  2, pgwidth);
    LWSET(pIntin  +  4, pgheight);
    vdi();
}

VOID vm_coords( WORD handle, WORD llx, WORD lly, WORD urx, WORD ury )
{
    LWSET(pContrl +  0, 5);
    LWSET(pContrl +  2, 0);
    LWSET(pContrl +  6, 5);
    LWSET(pContrl + 10, 99);
    LWSET(pContrl + 12, handle);
    LWSET(pIntin  +  0, 1);
    LWSET(pIntin  +  2, llx);
    LWSET(pIntin  +  4, lly);
    LWSET(pIntin  +  6, urx);
    LWSET(pIntin  +  8, ury);
    vdi();
}

VOID vt_resolution ( WORD handle, WORD xres, WORD yres, WORD *xset, WORD *yset )
{
    LWSET(pContrl +  0, 5);
    LWSET(pContrl +  2, 0);
    LWSET(pContrl +  6, 2);
    LWSET(pContrl + 10, 81);
    LWSET(pContrl + 12, handle);
    LWSET(pIntin  +  0, xres);
    LWSET(pIntin  +  2, yres);
    vdi();
    *xset = LWGET(pIntout + 0);
    *yset = LWGET(pIntout + 2);
}


VOID vt_axis ( WORD handle, WORD xres, WORD yres, WORD *xset, WORD *yset )
{
    LWSET(pContrl +  0, 5);
    LWSET(pContrl +  2, 0);
    LWSET(pContrl +  6, 2);
    LWSET(pContrl + 10, 82);
    LWSET(pContrl + 12, handle);
    LWSET(pIntin  +  0, xres);
    LWSET(pIntin  +  2, yres);
    vdi();
    *xset = LWGET(pIntout + 0);
    *yset = LWGET(pIntout + 2);
}

VOID vt_origin ( WORD handle, WORD xorigin, WORD yorigin )
{
    LWSET(pContrl +  0, 5);
    LWSET(pContrl +  2, 0);
    LWSET(pContrl +  6, 2);
    LWSET(pContrl + 10, 83);
    LWSET(pContrl + 12, handle);
    LWSET(pIntin  +  0, xorigin);
    LWSET(pIntin  +  2, yorigin);
    vdi();
}

VOID vq_tdimensions( WORD handle, WORD *xdimension, WORD *ydimension )
{
    GENERIC_5(84)
    vdi();
    *xdimension = LWGET(pIntout + 0);
    *ydimension = LWGET(pIntout + 2);
}


VOID vt_alignment( WORD handle, WORD dx, WORD dy )
{
    LWSET(pContrl +  0, 5);
    LWSET(pContrl +  2, 0);
    LWSET(pContrl +  6, 2);
    LWSET(pContrl + 10, 85);
    LWSET(pContrl + 12, handle);
    LWSET(pIntin  +  0, dx);
    LWSET(pIntin  +  2, dy);
    vdi();
}

VOID v_pline( WORD handle, WORD count, WORD xy[] )
{
    dosmemput(xy, 4*count, pPtsin);

    LWSET(pContrl +  0, 6);
    LWSET(pContrl +  2, count);
    LWSET(pContrl +  6, 0);
    LWSET(pContrl + 12, handle);

    vdi();
}

VOID v_pmarker( WORD handle, WORD count, WORD xy[] )
{
    dosmemput(xy, 4*count, pPtsin);

    LWSET(pContrl +  0, 7);
    LWSET(pContrl +  2, count);
    LWSET(pContrl +  6, 0);
    LWSET(pContrl + 12, handle);

    vdi();
}

VOID v_gtext( WORD handle, WORD x, WORD y, BYTE *string)
{
    WORD i;

    LWSET(pPtsin + 0, x);
    LWSET(pPtsin + 2, y);
    i = dj_putb2w(string, pIntin);
    LWSET(pContrl +  0, 8);
    LWSET(pContrl +  2, 1);
    LWSET(pContrl +  6, i);
    LWSET(pContrl + 12, handle);
    vdi();
}

VOID v_fillarea( WORD handle, WORD count, WORD xy[])
{
    dosmemput(xy, 4*count, pPtsin);

    LWSET(pContrl +  0, 9);
    LWSET(pContrl +  2, count);
    LWSET(pContrl +  6, 0);
    LWSET(pContrl + 12, handle);

    vdi();
}


VOID v_cellarray( WORD handle, WORD xy[4], WORD row_length, WORD el_per_row, 
                  WORD num_rows, WORD wr_mode, LPWORD colors )
{
    i_intin( colors );
    dosmemput(xy, 8, pPtsin);

    LWSET(pContrl +  0, 10);
    LWSET(pContrl +  2, 2);
    LWSET(pContrl +  6, row_length * num_rows);
    LWSET(pContrl + 12, handle);
    LWSET(pContrl + 14, row_length);
    LWSET(pContrl + 16, el_per_row);
    LWSET(pContrl + 18, num_rows);
    LWSET(pContrl + 20, wr_mode);
    vdi();

    i_intin( pIntin );
}

VOID v_bar( WORD handle, WORD xy[] )
{
    dosmemput(xy, 8, pPtsin);

    LWSET(pContrl +  0, 11);
    LWSET(pContrl +  2, 2);
    LWSET(pContrl +  6, 0);
    LWSET(pContrl + 10, 1);
    LWSET(pContrl + 12, handle);

    vdi();
}

VOID v_arc( WORD handle, WORD xc, WORD yc, WORD rad, WORD sang, WORD eang )
{
    LWSET(pPtsin,      xc);
    LWSET(pPtsin +  2, yc);
    LWSET(pPtsin +  4,  0);
    LWSET(pPtsin +  6,  0);
    LWSET(pPtsin +  8,  0);
    LWSET(pPtsin + 10,  0);
    LWSET(pPtsin + 12,  rad);
    LWSET(pPtsin + 14,  0);
    LWSET(pIntin,      sang);
    LWSET(pIntin +  2, eang);

    LWSET(pContrl +  0, 11);
    LWSET(pContrl +  2, 4);
    LWSET(pContrl +  6, 2);
    LWSET(pContrl + 10, 2);
    LWSET(pContrl + 12, handle);
    vdi();
}

VOID v_pieslice( WORD handle, WORD xc, WORD yc, WORD rad, WORD sang, WORD eang )
{
    LWSET(pPtsin,      xc);
    LWSET(pPtsin +  2, yc);
    LWSET(pPtsin +  4,  0);
    LWSET(pPtsin +  6,  0);
    LWSET(pPtsin +  8,  0);
    LWSET(pPtsin + 10,  0);
    LWSET(pPtsin + 12,  rad);
    LWSET(pPtsin + 14,  0);
    LWSET(pIntin,      sang);
    LWSET(pIntin +  2, eang);

    LWSET(pContrl +  0, 11);
    LWSET(pContrl +  2, 4);
    LWSET(pContrl +  6, 2);
    LWSET(pContrl + 10, 3);
    LWSET(pContrl + 12, handle);

    vdi();
}

VOID v_circle( WORD handle, WORD xc, WORD yc, WORD rad )
{
    LWSET(pPtsin,      xc);
    LWSET(pPtsin +  2, yc);
    LWSET(pPtsin +  4,  0);
    LWSET(pPtsin +  6,  0);
    LWSET(pPtsin +  8,  rad);
    LWSET(pPtsin + 10,  0);

    LWSET(pContrl +  0, 11);
    LWSET(pContrl +  2, 3);
    LWSET(pContrl +  6, 0);
    LWSET(pContrl + 10, 4);
    LWSET(pContrl + 12, handle);

    vdi();
}

VOID v_ellipse( WORD handle, WORD xc, WORD yc, WORD xrad, WORD yrad )
{
    LWSET(pPtsin,      xc);
    LWSET(pPtsin +  2, yc);
    LWSET(pPtsin +  4, xrad);
    LWSET(pPtsin +  6, yrad);

    LWSET(pContrl +  0, 11);
    LWSET(pContrl +  2, 2);
    LWSET(pContrl +  6, 0);
    LWSET(pContrl + 10, 5);
    LWSET(pContrl + 12, handle);
    vdi();
}

VOID v_ellarc( WORD handle, WORD xc, WORD yc, WORD xrad, WORD yrad, 
               WORD sang, WORD eang )
{
    LWSET(pPtsin,      xc);
    LWSET(pPtsin +  2, yc);
    LWSET(pPtsin +  4, xrad);
    LWSET(pPtsin +  6, yrad);
    LWSET(pIntin,      sang);
    LWSET(pIntin +  2, eang);

    LWSET(pContrl +  0, 11);
    LWSET(pContrl +  2, 2);
    LWSET(pContrl +  6, 2);
    LWSET(pContrl + 10, 6);
    LWSET(pContrl + 12, handle);
    vdi();
}

VOID v_ellpie( WORD handle, WORD xc, WORD yc, WORD xrad, WORD yrad, 
               WORD sang, WORD eang)
{
    LWSET(pPtsin,      xc);
    LWSET(pPtsin +  2, yc);
    LWSET(pPtsin +  4, xrad);
    LWSET(pPtsin +  6, yrad);
    LWSET(pIntin,      sang);
    LWSET(pIntin +  2, eang);

    LWSET(pContrl +  0, 11);
    LWSET(pContrl +  2, 2);
    LWSET(pContrl +  6, 2);
    LWSET(pContrl + 10, 7);
    LWSET(pContrl + 12, handle);
    vdi();
}

VOID v_rbox( WORD handle, WORD xy[] )
{
    dosmemput(xy, 8, pPtsin);

    LWSET(pContrl +  0, 11);
    LWSET(pContrl +  2, 2);
    LWSET(pContrl +  6, 0);
    LWSET(pContrl + 10, 8);
    LWSET(pContrl + 12, handle);
    vdi();
}


VOID v_rfbox( WORD handle, WORD xy[] )
{
    dosmemput(xy, 8, pPtsin);

    LWSET(pContrl +  0, 11);
    LWSET(pContrl +  2, 2);
    LWSET(pContrl +  6, 0);
    LWSET(pContrl + 10, 9);
    LWSET(pContrl + 12, handle);
    vdi();
}


VOID v_justified( WORD handle, WORD x, WORD y, BYTE string[], 
		  WORD length, WORD word_space, WORD char_space)
{
    int i;

    LWSET(pPtsin,     x);
    LWSET(pPtsin + 2, y);
    LWSET(pPtsin + 4, length);
    LWSET(pPtsin + 6, 0);
    LWSET(pIntin,     word_space);
    LWSET(pIntin + 2, char_space);

    i = 2 + dj_putb2w(string, pIntin + 4);
    LWSET(pContrl,     11);
    LWSET(pContrl + 2, 2);
    LWSET(pContrl + 6, i);
    LWSET(pContrl +10, 10);
    LWSET(pContrl +12, handle);

    vdi();
}

WORD vqt_justified( WORD handle, WORD x, WORD y, BYTE string[], 
		    WORD length, WORD word_space, WORD char_space, 
                    LPWORD offsets)
{
    int i;

    LWSET(pPtsin,     x);
    LWSET(pPtsin + 2, y);
    LWSET(pPtsin + 4, length);
    LWSET(pPtsin + 6, 0);
    LWSET(pIntin,     word_space);
    LWSET(pIntin + 2, char_space);

    i = 2 + dj_putb2w(string, pIntin + 4);
    LWSET(pContrl,     132);
    LWSET(pContrl + 2, 2);
    LWSET(pContrl + 6, i);
    LWSET(pContrl +12, handle);

    i_ptsout( offsets );
    vdi();
    i_ptsout( pPtsout );
    return LWGET(pContrl + 4);
}



VOID vst_height( WORD handle, WORD height, 
                 WORD *char_width, WORD *char_height, 
                 WORD *cell_width, WORD *cell_height )
{
    LWSET(pPtsin,     0);
    LWSET(pPtsin + 2, height);

    LWSET(pContrl,     12);
    LWSET(pContrl + 2,  1);
    LWSET(pContrl + 6,  0);
    LWSET(pContrl +12,  handle);

    vdi();

    *char_width  = LWGET(pPtsout);
    *char_height = LWGET(pPtsout + 2);
    *cell_width  = LWGET(pPtsout + 4);
    *cell_height = LWGET(pPtsout + 6);
}

WORD vst_rotation( WORD handle, WORD angle )
{
    LWSET(pIntin,      angle);
    LWSET(pContrl + 0, 13);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();
    return LWGET(pIntout);
}

VOID vs_color( WORD handle, WORD index, WORD *rgb )
{
    LWSET(pIntin,      index);
    dosmemput(rgb, 8, pIntin + 2);

    LWSET(pContrl + 0, 14);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  4);
    LWSET(pContrl +12,  handle);
    vdi();
}



WORD vsl_type( WORD handle, WORD style )
{
    LWSET(pIntin,      style);
    LWSET(pContrl + 0, 15);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();
    return LWGET(pIntout);
}

WORD vsl_width( WORD handle, WORD width )
{
    LWSET(pPtsin,      width);
    LWSET(pPtsin  + 2, 0);
    LWSET(pContrl + 0, 16);
    LWSET(pContrl + 2,  1);
    LWSET(pContrl + 6,  0);
    LWSET(pContrl +12,  handle);
    vdi();
    return LWGET(pPtsout);
}


WORD vsl_color( WORD handle, WORD index )
{
    LWSET(pIntin,      index);
    LWSET(pContrl + 0, 17);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();
    return LWGET(pIntout);
}

WORD vsm_type( WORD handle, WORD symbol )
{
    LWSET(pIntin,      symbol);
    LWSET(pContrl + 0, 18);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();
    return LWGET(pIntout);
}



WORD vsm_height( WORD handle, WORD height )
{
    LWSET(pPtsin,     0);
    LWSET(pPtsin + 2, height);

    LWSET(pContrl + 0, 19);
    LWSET(pContrl + 2,  1);
    LWSET(pContrl + 6,  0);
    LWSET(pContrl +12,  handle);
    vdi();
    return(LWGET(pPtsout + 2));
}

WORD vsm_color( WORD handle, WORD index )
{
    LWSET(pIntin, index);

    LWSET(pContrl + 0, 20);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();
    return( LWGET(pIntout ));
}

WORD vst_font( WORD handle, WORD font )
{
    LWSET(pIntin, font);

    LWSET(pContrl + 0, 21);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();
    return( LWGET(pIntout) );
}

WORD vst_color( WORD handle, WORD index )
{
    LWSET(pIntin, index);

    LWSET(pContrl + 0, 22);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();
    return( LWGET(pIntout) );
}
    
WORD vsf_interior( WORD handle, WORD style )
{
    LWSET(pIntin, style);

    LWSET(pContrl + 0, 23);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();
    return( LWGET(pIntout));
}

WORD vsf_style( WORD handle, WORD index )
{
    LWSET(pIntin, index);

    LWSET(pContrl + 0, 24);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();
    return( LWGET(pIntout));
}


WORD vsf_color( WORD handle, WORD index )
{
    LWSET(pIntin,      index);
    LWSET(pContrl + 0, 25);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();
    return( LWGET(pIntout));
}



VOID vq_color( WORD handle, WORD index, WORD set_flag, WORD rgb[] )
{
    LWSET(pIntin,      index);
    LWSET(pIntin  + 2, set_flag);
    LWSET(pContrl + 0, 26);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  2);
    LWSET(pContrl +12,  handle);
    vdi();

    rgb[0] = LWGET(pIntout + 2);
    rgb[1] = LWGET(pIntout + 4);
    rgb[2] = LWGET(pIntout + 6);
}


VOID vq_cellarray( WORD handle,   WORD xy[4], WORD row_len, WORD num_rows, 
		   WORD *el_used, WORD *rows_used, WORD *stat, LPVOID colors )
{
    dosmemput(xy, pPtsin, 8);
    i_intout( colors );

    LWSET(pContrl + 0, 27);
    LWSET(pContrl + 2,  2);
    LWSET(pContrl + 6,  0);
    LWSET(pContrl +12,  handle);
    LWSET(pContrl +14,  row_len);
    LWSET(pContrl +16,  num_rows);
    vdi();

    *el_used   = LWGET(pContrl + 18);
    *rows_used = LWGET(pContrl + 20);
    *stat      = LWGET(pContrl + 22);

    i_intout( pIntout );
}


VOID vrq_locator( WORD handle, WORD initx, WORD inity, 
		  WORD *xout,  WORD *yout, WORD *term )
{
    LWSET(pPtsin + 0, initx);
    LWSET(pPtsin + 2, inity);

    LWSET(pContrl + 0, 28);
    LWSET(pContrl + 2,  1);
    LWSET(pContrl + 6,  0);
    LWSET(pContrl +12,  handle);
    vdi();

    *xout = LWGET(pPtsout);
    *yout = LWGET(pPtsout + 2);
    *term = LWGET(pIntout);
}


WORD vsm_locator( WORD handle, WORD initx, WORD inity, 
                  WORD *xout,  WORD *yout, WORD *term )
{
    LWSET(pPtsin + 0, initx);
    LWSET(pPtsin + 2, inity);

    LWSET(pContrl + 0, 28);
    LWSET(pContrl + 2,  1);
    LWSET(pContrl + 6,  0);
    LWSET(pContrl +12,  handle);
    vdi();

    *xout = LWGET(pPtsout);
    *yout = LWGET(pPtsout + 2);
    *term = LWGET(pIntout);
    return( (LWGET(pContrl + 8) << 1) | LWGET(pContrl + 4) );
}



VOID vrq_valuator( WORD handle, WORD val_in, WORD *val_out, WORD *term )
{
    LWSET(pIntin, val_in);

    LWSET(pContrl + 0, 29);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();

    *val_out = LWGET(pIntout);
    *term    = LWGET(pIntout + 2);
}


VOID vsm_valuator( WORD handle, WORD val_in, WORD *val_out, 
                   WORD *term,  WORD *status )
{
    LWSET(pIntin, val_in);

    LWSET(pContrl + 0, 29);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();

    *val_out = LWGET(pIntout);
    *term    = LWGET(pIntout + 2);
    *status  = LWGET(pContrl + 8);
}



VOID vrq_choice( WORD handle, WORD in_choice, WORD *out_choice )
{
    LWSET(pIntin, in_choice);

    LWSET(pContrl + 0, 30);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();

    *out_choice = LWGET(pIntout);
}



WORD vsm_choice( WORD handle, WORD *choice )
{
    LWSET(pContrl + 0, 30);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  0);
    LWSET(pContrl +12,  handle);
    vdi();

    *choice = LWGET(pIntout);

    return LWGET(pContrl + 8);
}

VOID vrq_string( WORD handle, WORD length, WORD echo_mode, WORD echo_xy[],
                 BYTE *string)
{
    LWSET(pIntin,     length);
    LWSET(pIntin + 2, echo_mode);

    if (echo_mode)
    {
        LWSET(pPtsin,     echo_xy[0]);
        LWSET(pPtsin + 2, echo_xy[1]);
    }

    LWSET(pContrl + 0,  31);
    LWSET(pContrl + 2,  echo_mode);
    LWSET(pContrl + 6,  2);
    LWSET(pContrl +12,  handle);
    vdi();

    dj_getw2b(pIntout, LWGET(pContrl + 8), string);
    string[LWGET(pContrl + 8)] = 0;
}



WORD vsm_string( WORD handle, WORD length, WORD echo_mode, WORD echo_xy[],
		 BYTE *string )
{
    LWSET(pIntin,     length);
    LWSET(pIntin + 2, echo_mode);

    if (echo_mode)
    {
        LWSET(pPtsin,     echo_xy[0]);
        LWSET(pPtsin + 2, echo_xy[1]);
    }

    LWSET(pContrl + 0,  31);
    LWSET(pContrl + 2,  echo_mode);
    LWSET(pContrl + 6,  2);
    LWSET(pContrl +12,  handle);
    vdi();

    dj_getw2b(pIntout, LWGET(pContrl + 8), string);
    string[LWGET(pContrl + 8)] = 0;

    return (LWGET(pContrl + 8));
}


WORD vswr_mode( WORD handle, WORD mode )
{
    LWSET(pIntin,      mode);
    LWSET(pContrl + 0, 32);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  1);
    LWSET(pContrl +12,  handle);
    vdi();
    return LWGET(pIntout);
}


VOID vsin_mode( WORD handle, WORD dev_type, WORD mode )
{
    LWSET(pIntin,      dev_type);
    LWSET(pIntin + 2,  mode);
    LWSET(pContrl + 0, 33);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  2);
    LWSET(pContrl + 12, handle);
    vdi();
}



VOID vql_attributes( WORD handle, WORD attributes[] )
{
    i_intout(pGeneral);

    LWSET(pContrl + 0, 35);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  0);
    LWSET(pContrl + 12, handle);
    vdi();

    if (LWGET(pContrl + 8)) dosmemget(pGeneral, LWGET(pContrl + 8), attributes);
    i_intout(pIntout);

    attributes[3] = LWGET(pPtsout);
}



VOID vqm_attributes( WORD handle, WORD attributes[] )
{
    i_intout(pGeneral);

    LWSET(pContrl + 0, 36);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  0);
    LWSET(pContrl + 12, handle);
    vdi();

    if (LWGET(pContrl + 8)) dosmemget(pGeneral, LWGET(pContrl + 8), attributes);
    i_intout(pIntout);

    attributes[3] = LWGET(pPtsout + 2);
}



VOID vqf_attributes( WORD handle, WORD attributes[] )
{
    i_intout(pGeneral);

    LWSET(pContrl + 0, 37);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  0);
    LWSET(pContrl + 12, handle);
    vdi();

    if (LWGET(pContrl + 8)) dosmemget(pGeneral, LWGET(pContrl + 8), attributes);
    i_intout(pIntout);
}


/* Returns 0 if the VDI does not support this call (ViewMAX doesn't) */

BOOLEAN vqt_attributes( WORD handle, WORD attributes[10] )
{
    LWSET(pContrl + 0, 38);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  0);
    LWSET(pContrl + 12, handle);
    vdi();

    dosmemget(pIntout, 12,  attributes);
    dosmemget(pPtsout,  8, &attributes[6]);

    if (LWGET(pContrl + 8)) return 1;
    return 0;
}


VOID vst_alignment( WORD handle, WORD hor_in,    WORD vert_in, 
                                 WORD *hor_out,  WORD *vert_out )
{
    LWSET(pIntin + 0, hor_in);
    LWSET(pIntin + 2, vert_in);

    LWSET(pContrl + 0, 39);
    LWSET(pContrl + 2,  0);
    LWSET(pContrl + 6,  2);
    LWSET(pContrl + 12, handle);
    vdi();

    *hor_out  = LWGET(pIntout);
    *vert_out = LWGET(pIntout + 2);
}



VOID v_opnvwk( WORD work_in[11], WORD *handle, WORD work_out[57] )
{
    if (!vdi_alloc_arrays()) return;

    dosmemput(work_in, 22, pGeneral);

    i_intin( pGeneral );
    i_intout( pGeneral + 22 );
    i_ptsout( pGeneral + 112 );

    LWSET(pContrl +  0, 100);
    LWSET(pContrl +  2,   0);
    LWSET(pContrl +  6,  11);
    LWSET(pContrl + 12,  *handle);
    vdi();

    *handle = LWGET(pContrl + 12);

    dosmemget(pGeneral + 22, 114, work_out);

    i_intin( pIntin );
    i_intout( pIntout );
    i_ptsout( pPtsout );
}


VOID v_clsvwk( WORD handle )
{
    LWSET(pContrl + 0, 101);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);
    vdi();
}


VOID vq_extnd( WORD handle, WORD owflag, WORD work_out[57] )
{
    i_intout( pGeneral );
    i_ptsout( pGeneral + 90 );
    LWSET(pIntin, owflag);

    LWSET(pContrl + 0, 102);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   1);
    LWSET(pContrl +12,   handle);
    vdi();

    dosmemget( pGeneral, 114, work_out);
    i_intout( pIntout );
    i_ptsout( pPtsout );
}


VOID v_contourfill( WORD handle, WORD x, WORD y, WORD index )
{
    LWSET(pIntin, index);
    LWSET(pPtsin,     x);
    LWSET(pPtsin + 2, y);

    LWSET(pContrl + 0, 103);
    LWSET(pContrl + 2,   1);
    LWSET(pContrl + 6,   1);
    LWSET(pContrl +12,   handle);
    vdi();
}

WORD vsf_perimeter( WORD handle, WORD per_vis )
{
    LWSET(pIntin, per_vis);

    LWSET(pContrl + 0, 104);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   1);
    LWSET(pContrl +12,   handle);
    vdi();
    return( LWGET(pIntout));
}


BOOLEAN v_get_pixel( WORD handle, WORD x, WORD y, WORD *pel, WORD *index )
{
    LWSET(pPtsin,     x);
    LWSET(pPtsin + 2, y);

    LWSET(pContrl + 0, 105);
    LWSET(pContrl + 2,   1);
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);
    vdi();

    *pel   = LWGET(pIntout);
    *index = LWGET(pIntout + 2);

    if (LWGET(pContrl + 8)) return TRUE;
    return FALSE;
}



WORD vst_effects( WORD handle, WORD effect )
{
    LWSET(pIntin, effect);

    LWSET(pContrl + 0, 106);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   1);
    LWSET(pContrl +12,   handle);
    vdi();
    return( LWGET(pIntout));
}

WORD vst_point( WORD handle, WORD point, WORD *char_width, WORD *char_height, 
				         WORD *cell_width, WORD *cell_height )
{
    LWSET(pIntin, point);

    LWSET(pContrl + 0, 107);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   1);
    LWSET(pContrl +12,   handle);
    vdi();

    *char_width =  LWGET(pPtsout);
    *char_height = LWGET(pPtsout + 2);
    *cell_width =  LWGET(pPtsout + 4);
    *cell_height = LWGET(pPtsout + 6);
    return( LWGET(pIntout));
}

VOID vsl_ends( WORD handle, WORD beg_style, WORD end_style)
{
    LWSET(pIntin,     beg_style);
    LWSET(pIntin + 2, end_style);

    LWSET(pContrl + 0, 108);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   2);
    LWSET(pContrl +12,   handle);
    vdi();
}

/* Note: Because the MFDBs are in our address space, pointers are linear 
        ones and we have to convert them to seg:off */

VOID vro_cpyfm( WORD handle, WORD wr_mode, WORD xy[], MFDB *srcMFDB, MFDB *desMFDB )
{
    LWSET(pIntin, wr_mode);
    dosmemput(xy, 16, pPtsin);
    dosmemput(srcMFDB, sizeof(MFDB), G_MFDB1);
    dosmemput(desMFDB, sizeof(MFDB), G_MFDB2);

    LSSET(G_MFDB1, LDGET(G_MFDB1));	/* Convert pointers to seg:off     */
    LSSET(G_MFDB2, LDGET(G_MFDB2));	/* (they are at the start of the   */
					/*  struct, so no need to add      */
					/*  anything to G_MFDB1 / G_MFDB2) */
    i_lptr1(G_MFDB1);
    i_lptr2(G_MFDB2);

    LWSET(pContrl + 0, 109);
    LWSET(pContrl + 2,   4);
    LWSET(pContrl + 6,   1);
    LWSET(pContrl +12,   handle);
    vdi();

    LDSET(G_MFDB1, LSGET(G_MFDB1));	/* Convert pointers to linear  */
    LDSET(G_MFDB2, LSGET(G_MFDB2));	

    dosmemget(G_MFDB1, sizeof(MFDB), srcMFDB);
    dosmemget(G_MFDB2, sizeof(MFDB), desMFDB);
}



VOID vr_trnfm( WORD handle, MFDB *srcMFDB, MFDB *desMFDB )
{
    dosmemput(srcMFDB, sizeof(MFDB), G_MFDB1);
    dosmemput(desMFDB, sizeof(MFDB), G_MFDB2);

    LSSET(G_MFDB1, LDGET(G_MFDB1));	/* Convert pointers to seg:off */
    LSSET(G_MFDB2, LDGET(G_MFDB2));	

    i_lptr1(pGeneral);
    i_lptr2(pGeneral + sizeof(MFDB));

    LWSET(pContrl + 0, 110);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);
    vdi();

    LDSET(G_MFDB1, LSGET(G_MFDB1));	/* Convert pointers to linear */
    LDSET(G_MFDB2, LSGET(G_MFDB2));	

    dosmemget(G_MFDB1, sizeof(MFDB), srcMFDB);
    dosmemget(G_MFDB2, sizeof(MFDB), desMFDB);

}



VOID vsc_form( WORD handle, WORD cur_form[37] )
{
    dosmemput(cur_form, 74, pIntin);

    LWSET(pContrl + 0, 111);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,  37);
    LWSET(pContrl +12,   handle);
    vdi();
}


VOID vsf_udpat( WORD handle, WORD *fill_pat, WORD planes )
{
    dosmemput(fill_pat, 32*planes, pGeneral);
    i_intin(pGeneral);

    LWSET(pContrl + 0, 112);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   16*planes);
    LWSET(pContrl +12,   handle);
    vdi();
    i_intin( pIntin );	
}


VOID vsl_udsty( WORD handle, WORD pattern )
{
    LWSET(pIntin, pattern);

    LWSET(pContrl + 0, 113);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   1);
    LWSET(pContrl +12,   handle);
    vdi();
}


VOID vr_recfl( WORD handle, WORD *xy )
{
    dosmemput(xy, 8, pPtsin);

    LWSET(pContrl +  0, 114);
    LWSET(pContrl +  2, 2);
    LWSET(pContrl +  6, 0);
    LWSET(pContrl + 12, handle);
    vdi();

}


VOID vqin_mode( WORD handle, WORD dev_type, WORD *mode )
{
    LWSET(pIntin, dev_type);

    LWSET(pContrl + 0, 115);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   1);
    LWSET(pContrl +12,   handle);
    vdi();

    *mode = LWGET(pIntout);
}


VOID vqt_extent( WORD handle, BYTE *string, WORD *extent )
{
    WORD i;

    i = dj_putb2w(string, pIntin);

    LWSET(pContrl + 0, 116);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   i);
    LWSET(pContrl +12,   handle);
    vdi();

    dosmemget(pPtsout,  16, extent); /* Returns 4 points */
}


WORD vqt_width( WORD handle, BYTE character, 
                WORD *cell_width, WORD *left_delta, WORD *right_delta )
{
    LWSET(pIntin, character);

    LWSET(pContrl + 0, 117);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   1);
    LWSET(pContrl +12,   handle);
    vdi();

    *cell_width  = LWGET(pPtsout);
    *left_delta  = LWGET(pPtsout + 4);
    *right_delta = LWGET(pPtsout + 8);
    return( LWGET(pIntout) );
}


VOID vex_timv( WORD handle, LPVOID tim_addr, LPVOID *old_addr, WORD *scale )
{
    i_lptr1( tim_addr );

    LWSET(pContrl + 0, 118);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);
    vdi();

    *old_addr = LSGET(pContrl + 18);
    *scale = LWGET(pIntout);
}


WORD vst_load_fonts( WORD handle, WORD select )
{
    LWSET(pContrl + 0, 119);	/* select is not used, and this seems to */
    LWSET(pContrl + 2,   0);	/* be deliberate */
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);
    vdi();
    return( LWGET(pIntout));
}

VOID vst_unload_fonts( WORD handle, WORD select )
{
    LWSET(pContrl + 0, 120);	/* select is not used, and this seems to */
    LWSET(pContrl + 2,   0);	/* be deliberate */
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);
    vdi();
}



VOID vrt_cpyfm( WORD handle, WORD wr_mode, WORD xy[], MFDB *srcMFDB, MFDB *desMFDB, WORD *index )
{
    dosmemput(srcMFDB, sizeof(MFDB), G_MFDB1);
    dosmemput(desMFDB, sizeof(MFDB), G_MFDB2);
    dosmemput(xy, 16, pPtsin);
 
    LSSET(G_MFDB1, LDGET(G_MFDB1));	/* Convert pointers to seg:off */
    LSSET(G_MFDB2, LDGET(G_MFDB2));	

    LWSET(pIntin, wr_mode);
    LWSET(pIntin + 2, *index++);
    LWSET(pIntin + 4, *index);

    i_lptr1( pGeneral );
    i_lptr2( pGeneral + sizeof(MFDB) );

    LWSET(pContrl + 0, 121);
    LWSET(pContrl + 2,   4);
    LWSET(pContrl + 6,   3);
    LWSET(pContrl +12,   handle);
    vdi();

    LDSET(G_MFDB1, LSGET(G_MFDB1));	/* Convert pointers to linear */
    LDSET(G_MFDB2, LSGET(G_MFDB2));	

    dosmemget(G_MFDB1, sizeof(MFDB), srcMFDB);
    dosmemget(G_MFDB2, sizeof(MFDB), desMFDB);
}

VOID v_show_c( WORD handle, WORD reset )
{

    LWSET(pIntin,      reset);
    LWSET(pContrl + 0, 122);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   1);
    LWSET(pContrl +12,   handle);
    vdi();
}


VOID v_hide_c( WORD handle )
{
    LWSET(pContrl + 0, 123);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);
    vdi();
}

VOID vq_mouse( WORD handle, WORD *status, WORD *px, WORD *py )
{
    LWSET(pContrl + 0, 124);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);
    vdi();

    *status = LWGET(pIntout);
    *px     = LWGET(pPtsout);
    *py     = LWGET(pPtsout + 2);
}

VOID vex_butv( WORD handle, LPVOID usercode, LPVOID *savecode )
{
    i_lptr1( usercode );

    LWSET(pContrl + 0, 125);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);
    vdi();

    *savecode = LSGET(pContrl + 18);
}

VOID vex_motv( WORD handle, LPVOID usercode, LPVOID *savecode )
{
    i_lptr1( usercode );

    LWSET(pContrl + 0, 126);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);
    vdi();

    *savecode = LSGET(pContrl + 18);
}

VOID vex_curv( WORD handle, LPVOID usercode, LPVOID *savecode )
{
    i_lptr1( usercode );

    LWSET(pContrl + 0, 127);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);
    vdi();

    *savecode = LSGET(pContrl + 18);
}

VOID vq_key_s( WORD handle, WORD *status )
{
    LWSET(pContrl + 0, 128);
    LWSET(pContrl + 2,   0);
    LWSET(pContrl + 6,   0);
    LWSET(pContrl +12,   handle);
    vdi();

    *status = LWGET(pIntout);
}



VOID vs_clip( WORD handle, WORD clip_flag, WORD xy[] )
{
    dosmemput(xy, 8, pPtsin);

    LWSET(pIntin, clip_flag);

    LWSET(pContrl  + 0, 129);
    LWSET(pContrl  + 2, 2);
    LWSET(pContrl  + 6, 1);
    LWSET(pContrl  +12, handle);
    vdi();
}


WORD vqt_name( WORD handle, WORD element_num, BYTE name[] )
{
    WORD i;

    LWSET(pIntin, element_num);

    for (i = 0; i < 32; i++) LWSET(pIntout + 2 + 2*i, 0);

    LWSET(pContrl  + 0, 130);
    LWSET(pContrl  + 2, 0);
    LWSET(pContrl  + 6, 1);
    LWSET(pContrl  +12, handle);
    vdi();

    for (i = 0 ; i < 32 ; i++)
	name[i] = LWGET(pIntout + 2 + 2*i);
    return( LWGET(pIntout));
}

BOOLEAN vqt_font_info( WORD handle, WORD *minADE, WORD *maxADE, 
		    WORD distances[], WORD *maxwidth, WORD effects[] )
{
    LWSET(pContrl  + 0, 131);
    LWSET(pContrl  + 2, 0);
    LWSET(pContrl  + 6, 0);
    LWSET(pContrl  +12, handle);
    vdi();

    *minADE      = LWGET(pIntout);
    *maxADE      = LWGET(pIntout + 2);
    *maxwidth    = LWGET(pPtsout);
    distances[0] = LWGET(pPtsout + 2);
    distances[1] = LWGET(pPtsout + 6);
    distances[2] = LWGET(pPtsout + 10);
    distances[3] = LWGET(pPtsout + 14);
    distances[4] = LWGET(pPtsout + 18);
    effects[0]   = LWGET(pPtsout + 4);
    effects[1]   = LWGET(pPtsout + 8);
    effects[2]   = LWGET(pPtsout + 12);

    if (LWGET(pContrl + 8)) return TRUE;
    return FALSE;
}


/* The following functions are GEM/3 only*/

VOID v_copies(WORD handle, WORD count)
{
    LWSET(pIntin, count);
    LWSET(pContrl  + 0, 5);
    LWSET(pContrl  + 2, 0);
    LWSET(pContrl  + 6, 1);
    LWSET(pContrl  +10, 28);
    LWSET(pContrl  +12, handle);
    vdi();
}

VOID v_etext( WORD handle, WORD x, WORD y, UBYTE string[], WORD offsets[])
{
    WORD	intptr, ptsptr;
    
    intptr = 0;
    LWSET(pPtsin, x);
    LWSET(pPtsin + 2, y);

    intptr = dj_putb2w(string, pIntin);

    for (ptsptr = 0; ptsptr < intptr; ptsptr++)
    {
        LWSET(pPtsin + 4 + 4*ptsptr, *offsets++);
        LWSET(pPtsin + 6 + 4*ptsptr, *offsets++);
    }
    LWSET(pContrl  + 0, 1);
    LWSET(pContrl  + 2, ptsptr);
    LWSET(pContrl  + 6, intptr);
    LWSET(pContrl  +10, 11);
    LWSET(pContrl  +12, handle);
    vdi();
}

VOID v_orient( WORD handle, WORD orientation )
{
    LWSET(pIntin,       orientation);
    LWSET(pContrl  + 0, 5);
    LWSET(pContrl  + 2, 0);
    LWSET(pContrl  + 6, 1);
    LWSET(pContrl  +10, 27);
    LWSET(pContrl  +12, handle);
    vdi() ;
}


VOID v_tray( WORD handle, WORD tray )
{
    LWSET(pIntin,       tray);
    LWSET(pContrl  + 0, 5);
    LWSET(pContrl  + 2, 0);
    LWSET(pContrl  + 6, 1);
    LWSET(pContrl  +10, 29);
    LWSET(pContrl  +12, handle);
    vdi() ;
} 



WORD v_xbit_image( WORD handle, BYTE *filename, WORD aspect, WORD x_scale, 
                   WORD y_scale, WORD h_align, WORD v_align, WORD rotate,
		   WORD background, WORD foreground, WORD xy[] )
{
    WORD ii;

    dosmemput(xy, 8, pPtsin);
    LWSET(pIntin     , aspect);
    LWSET(pIntin +  2, x_scale);
    LWSET(pIntin +  4, y_scale);
    LWSET(pIntin +  6, h_align);
    LWSET(pIntin +  8, v_align);
    LWSET(pIntin + 10, rotate);
    LWSET(pIntin + 12, background);
    LWSET(pIntin + 14, foreground);
    ii = 8 + dj_putb2w(filename, pIntin + 16);
    LWSET(pContrl,      5);
    LWSET(pContrl +  2, 2);
    LWSET(pContrl +  6, ii);
    LWSET(pContrl + 10, 101);
    LWSET(pContrl + 12, handle);
    vdi();
    return( TRUE ) ;
}

WORD vst_ex_load_fonts( WORD handle, WORD select, WORD font_max, WORD font_free )
{
    LWSET(pIntin,       select);
    LWSET(pIntin  +  2, font_max);
    LWSET(pIntin  +  4, font_free);
    LWSET(pContrl,      119);
    LWSET(pContrl +  2, 0);
    LWSET(pContrl +  6, 3);
    LWSET(pContrl + 12, handle);
    vdi();
    return( LWGET(pIntout) );
}


VOID v_ps_halftone(WORD handle, WORD index, WORD angle, WORD frequency)
{
   LWSET(pContrl,      5);
   LWSET(pContrl +  2, 0);
   LWSET(pContrl +  6, 3);
   LWSET(pContrl + 10, 32);
   LWSET(pContrl + 12, handle);

   LWSET(pIntin,     index);
   LWSET(pIntin + 2, angle);
   LWSET(pIntin + 4, frequency);

   vdi();
}

VOID v_setrgbi(WORD handle, WORD primtype, WORD r, WORD g, WORD b, WORD i)
{
   LWSET(pContrl,      5);
   LWSET(pContrl +  2, 0);
   LWSET(pContrl +  6, 5);
   LWSET(pContrl + 10, 18500);
   LWSET(pContrl + 12, handle);

   LWSET(pIntin,     primtype);
   LWSET(pIntin + 2, r);
   LWSET(pIntin + 4, g);
   LWSET(pIntin + 6, b);
   LWSET(pIntin + 8, i);

   vdi();
}


VOID v_topbot(WORD handle, WORD height, WORD *char_width, WORD *char_height, 
                                        WORD *cell_width, WORD *cell_height)
{
   LWSET(pContrl,      5);
   LWSET(pContrl +  2, 1);
   LWSET(pContrl +  6, 0);
   LWSET(pContrl + 10, 18501);	
   LWSET(pContrl + 12, handle);

   LWSET(pPtsin,     0);
   LWSET(pPtsin + 2, height);

   vdi();

   *char_width  = LWGET(pIntout);
   *char_height = LWGET(pIntout + 2);
   *cell_width  = LWGET(pIntout + 4);
   *cell_height = LWGET(pIntout + 6);
}

VOID vs_bkcolor(WORD handle, WORD color)
{
   LWSET(pContrl,      5);
   LWSET(pContrl +  2, 0);
   LWSET(pContrl +  6, 1);
   LWSET(pContrl + 10, 102);
   LWSET(pContrl + 12, handle);
   LWSET(pIntin,       color);

   vdi();
}

VOID v_set_app_buff(LPVOID address, WORD nparagraphs)
{
   LWSET(pContrl,     -1);
   LWSET(pContrl +  2, 0);
   LWSET(pContrl +  6, 3);
   LWSET(pIntin,       LP_OFF(address));
   LWSET(pIntin  +  2, LP_SEG(address));
   LWSET(pIntin  +  4, nparagraphs);   

   vdi();
}

WORD v_bez_on(WORD handle)
{
   LWSET(pContrl,      11);
   LWSET(pContrl +  2,  1);
   LWSET(pContrl +  6,  0);
   LWSET(pContrl + 10, 13);
   LWSET(pContrl + 12, handle);

   vdi();

   return( LWGET(pIntout));
}


WORD v_bez_off(WORD handle)
{
   LWSET(pContrl,      11);
   LWSET(pContrl +  2,  0);
   LWSET(pContrl +  6,  0);
   LWSET(pContrl + 10, 13);
   LWSET(pContrl + 12, handle);

   vdi();

   return( LWGET(pIntout));
}

WORD v_bez_qual(WORD handle, WORD prcnt)
{
   LWSET(pContrl,       5);
   LWSET(pContrl +  2,  0);
   LWSET(pContrl +  6,  3);
   LWSET(pContrl + 10, 99);
   LWSET(pContrl + 12, handle);

   LWSET(pIntin,       32);
   LWSET(pIntin  +  2,  1);
   LWSET(pIntin  +  4, prcnt);

   vdi();

   return (LWGET(pIntout));
}

VOID v_pat_rotate(WORD handle, WORD angle)
{
   LWSET(pContrl,      134);
   LWSET(pContrl +  2,   0);
   LWSET(pContrl +  6,   1);
   LWSET(pContrl + 12, handle);

   LWSET(pIntin, angle);

   vdi();
}


VOID vs_grayoverride(WORD handle, WORD grayval)
{
   LWSET(pContrl,      133);
   LWSET(pContrl +  2,   0);
   LWSET(pContrl +  6,   1);
   LWSET(pContrl + 12, handle);

   LWSET(pIntin, grayval);

   vdi();
}


MLOCAL VOID v_bezier_all(WORD opcode, WORD handle, WORD count, LPWORD xyarr, 
                         LPVOID bezarr, WORD *minmax, WORD *npts, WORD *nmove)
{
   LWSET(pContrl,      opcode);
   LWSET(pContrl +  2, count);
   LWSET(pContrl +  6, (count + 1) / 2);
   LWSET(pContrl + 10, 13);
   LWSET(pContrl + 12, handle);

   i_intin(bezarr);
   i_ptsin(xyarr);

   vdi();

   i_intin(pIntin);
   i_ptsin(pPtsin);

   *npts     = LWGET(pIntout);
   *nmove    = LWGET(pIntout + 2);
   minmax[0] = LWGET(pPtsout);
   minmax[1] = LWGET(pPtsout + 2);
   minmax[2] = LWGET(pPtsout + 4);
   minmax[3] = LWGET(pPtsout + 6);
}

VOID v_bez(WORD handle, WORD count, LPWORD xyarr, LPVOID bezarr, WORD *minmax, WORD *npts, WORD *nmove)
{
   v_bezier_all(6, handle, count, xyarr, bezarr, minmax, npts, nmove);
}

VOID v_bezfill(WORD handle, WORD count, LPWORD xyarr, LPVOID bezarr, WORD *minmax, WORD *npts, WORD *nmove)
{
   v_bezier_all(9, handle, count, xyarr, bezarr, minmax, npts, nmove);
}


