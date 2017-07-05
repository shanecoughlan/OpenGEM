/*	VDIBIND.H	7/18/84 - 4/19/85	John Grant		*/
/*************************************************************
 * Copyright 1999 by Caldera Thin Clients, Inc.              *
 * This software is licensed under the GNU Public License.   *
 * Please see LICENSE.TXT for further information.           *
 *************************************************************/
extern    WORD    v_opnwk();           /*  1   */
extern    WORD    v_clswk();           /*  2   */
extern    WORD    v_clrwk();           /*  3   */
extern    WORD    v_clswk();           /*  4   */

extern    WORD    vq_chcells();        /* 5 1  */
extern    WORD    v_exit_cur();        /* 5 2  */
extern    WORD    v_enter_cur();       /* 5 3  */
extern    WORD    v_curup();           /* 5 4  */
extern    WORD    v_curdown();         /* 5 5  */
extern    WORD    v_curright();        /* 5 6  */
extern    WORD    v_curleft();         /* 5 7  */
extern    WORD    v_curhome();         /* 5 8  */
extern    WORD    v_eeos();            /* 5 9  */
extern    WORD    v_eeol();            /* 5 10 */
extern    WORD    vs_curaddress();     /* 5 11 */
extern    WORD    v_curtext();         /* 5 12 */
extern    WORD    v_rvon();            /* 5 13 */
extern    WORD    v_rvoff();           /* 5 14 */
extern    WORD    vq_curaddress();     /* 5 15 */
extern    WORD    vq_tabstatus();      /* 5 16 */
extern    WORD    v_hardcopy ();       /* 5 17 */
extern    WORD    v_dspcur();          /* 5 18 */
extern    WORD    v_rmcur();           /* 5 19 */
extern    WORD    v_form_adv();        /* 5 20 */
extern    WORD    v_output_window();   /* 5 21 */
extern    WORD    v_clear_disp_list(); /* 5 22 */
extern    WORD    v_bit_image();       /* 5 23 */
extern    WORD    vq_scan();           /* 5 24 */
extern    WORD    v_alpha_text();      /* 5 25 */
extern    WORD    vs_palette();        /* 5 60 */
extern    WORD    v_meta_extents();    /* 5 98 */
extern    WORD    v_write_meta();      /* 5 99 */
extern    WORD    vm_filename();       /* 5 100*/

extern    WORD    v_pline();           /*  6   */
extern    WORD    v_pmarker();         /*  7   */
extern    WORD    v_gtext();           /*  8   */
extern    WORD    v_fillarea();        /*  9   */
extern    WORD    v_cellarray();       /*  10  */

extern    WORD    v_bar();	       /* 11  1 */
extern    WORD    v_arc();	       /* 11  2 */
extern    WORD    v_pieslice();	       /* 11  3 */
extern    WORD    v_circle();	       /* 11  4 */
extern    WORD    v_ellipse();	       /* 11  5 */
extern    WORD    v_ellarc();	       /* 11  6 */
extern    WORD    v_ellpie();	       /* 11  7 */
extern    WORD    v_rbox();            /* 11  8 */
extern    WORD    v_rfbox();           /* 11  9 */
extern    WORD    v_justified();       /* 11 10 */

extern    WORD    vst_height();        /*  12  */
extern    WORD    vst_rotation();      /*  13  */
extern    WORD    vs_color();	       /*  14  */
extern    WORD    vsl_type();	       /*  15  */
extern    WORD    vsl_width();	       /*  16  */
extern    WORD    vsl_color();	       /*  17  */
extern    WORD    vsm_type();	       /*  18  */
extern    WORD    vsm_height();	       /*  19  */
extern    WORD    vsm_color();	       /*  20  */
extern    WORD    vst_font();	       /*  21  */
extern    WORD    vst_color();	       /*  22  */
extern    WORD    vsf_interior();      /*  23  */
extern    WORD    vsf_style();	       /*  24  */
extern    WORD    vsf_color();	       /*  25  */
extern    WORD    vq_color();	       /*  26  */
extern    WORD    vq_cellarray();      /*  27  */

extern    WORD    vrq_locator();       /*  28  */
extern    WORD    vsm_locator();       /*  28  */
extern    WORD    vrq_valuator();      /*  29  */
extern    WORD    vsm_valuator();      /*  29  */
extern    WORD    vrq_choice();        /*  30  */
extern    WORD    vsm_choice();        /*  30  */
extern    WORD    vrq_string();        /*  31  */
extern    WORD    vsm_string();        /*  31  */
extern    WORD    vswr_mode();         /*  32  */
extern    WORD    vsin_mode();         /*  33  */

extern    WORD    vql_attributes();    /*  35  */
extern    WORD    vqm_attributes();    /*  36  */
extern    WORD    vqf_attributes();    /*  37  */
extern    WORD    vqt_attributes();    /*  38  */
extern    WORD    vst_alignment();     /*  39  */

extern    WORD    v_opnvwk();          /* 100  */
extern    WORD    v_clsvwk();          /* 101  */
extern    WORD    vq_extnd();          /* 102  */
extern    WORD    v_contourfill();     /* 103  */

extern    WORD    vsf_perimeter();     /* 104  */
extern    WORD    vst_background();    /* 105  */
extern    WORD    vst_effects();       /* 106  */
extern    WORD    vst_point();         /* 107  */
extern    WORD    vsl_end_style();     /* 108  */

extern    WORD    vro_cpyfm();         /* 109  */
extern    WORD    vr_trnfm();          /* 110  */
extern    WORD    vsc_form();          /* 111  */
extern    WORD    vsf_udpat();         /* 112  */
extern    WORD    vsl_udsty();         /* 113  */
extern    WORD    vr_recfl();          /* 114  */

extern    WORD    vqi_mode();          /* 115  */
extern    WORD    vqt_extent();        /* 116  */
extern    WORD    vqt_width();         /* 117  */
extern    WORD    vex_timv();          /* 118  */
extern	  WORD	  vst_load_fonts();    /* 119  */
extern	  WORD	  vst_unload_fonts();  /* 120  */
extern    WORD    vrt_cpyfm();         /* 121  */
extern    WORD    v_show_c();          /* 122  */
extern    WORD    v_hide_c();          /* 123  */
extern    WORD    vq_mouse();          /* 124  */
extern    WORD    vex_butv();          /* 125  */
extern    WORD    vex_motv();          /* 126  */
extern    WORD    vex_curv();          /* 127  */
extern    WORD    vq_key_s();          /* 128  */
extern    WORD    vs_clip();           /* 129  */

extern    WORD    vqt_name();          /* 130  */
extern    WORD    vqt_fontinfo();      /* 131  */
