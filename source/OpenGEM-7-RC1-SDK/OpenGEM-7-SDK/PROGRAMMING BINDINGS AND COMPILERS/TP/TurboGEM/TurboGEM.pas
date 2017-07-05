unit TurboGEM;

{ GEM Bindings for New Version TPs (Object Pascal) }
{ A tidied up version of the GEM/TP bindings }
{ Copied, pasted, and tidied by Rob Mitchelmore, 2001 }
{ All rites reversed }
{ Please see the README for details... scary :) }

interface
uses
    dos;

{ C String to Pascal String convertor types/procs}

type
    CARDINAL = 0.. MAXINT;
    WORD = INTEGER;
    ADDRESS = ^BYTE;
    CharString = String[127];
    MAXSTRING = String[255];
    LONG = record
      case Integer of
        1 : (ad : ADDRESS);
        2 : (o, s : INTEGER);             (* this is machine dependent ! *)
        3 : (i0, i1 : INTEGER);
        4 : (b0, b1, b2, b3 : BYTE);
        5 : (sp : ^CharString);
      end;

procedure MakeCString(var pString, cString);
procedure MakePString(var cString, pString);



{ AES Bindings }

TYPE
    ac_ARRAY = array [0..4] of INTEGER;
    ai_ARRAY = array [0..15] of INTEGER;
    ao_ARRAY = array [0..6] of INTEGER;
    ag_ARRAY = array [0..14] of INTEGER;
    aad_ARRAY = array [0..3] of ADDRESS;

VAR                               (* STD ARRAYS for GEM AES routines **)
    gcontrol : ac_ARRAY;
    global : ag_ARRAY;
    gintin : ai_ARRAY;
    gintout : ao_ARRAY;
    addr_in : aad_ARRAY;
    addr_out : aad_ARRAY;

PROCEDURE GAES(var a_addrout : aad_ARRAY; var a_addrin : aad_ARRAY;
               var aintout : ao_ARRAY; var aintin : ai_ARRAY;
               var aglobal : ag_ARRAY; var acontrol : ac_ARRAY);

FUNCTION GEMAES(opcode, c1, c2, c3, c4 : INTEGER) : INTEGER;

FUNCTION appl_init : INTEGER;
FUNCTION appl_exit : INTEGER;
FUNCTION appl_write(ap_wid, ap_wlength : INTEGER; ap_wpbuff : ADDRESS) : INTEGER;
FUNCTION appl_read(ap_rid, ap_rlength : INTEGER; ap_rpbuff : ADDRESS) : INTEGER;
FUNCTION appl_find(VAR ap_fpname) : INTEGER;
FUNCTION appl_tplay(ap_tpmem : ADDRESS; ap_tpnum, ap_tpscale : INTEGER) : INTEGER;
FUNCTION appl_trecord(ap_trmem : ADDRESS; ap_trcount : INTEGER) : INTEGER;
FUNCTION appl_bvset(bvdisk : INTEGER; bvhard : INTEGER) : INTEGER;
FUNCTION appl_yield : INTEGER;

FUNCTION evnt_keybd : INTEGER;
FUNCTION evnt_button(ev_bclicks, ev_bmask, ev_bstate : INTEGER; VAR ev_bmx,
                     ev_bmy, ev_bmb, ev_bks : INTEGER) : INTEGER;
FUNCTION evnt_mouse(ev_moflags, ev_mox, ev_moy, ev_mowidth, ev_moheight : INTEGER;
                    VAR ev_momx, ev_momy, ev_momb, ev_momks : INTEGER) : INTEGER;
FUNCTION evnt_mesag(ev_mgpbuff : ADDRESS) : INTEGER;
FUNCTION evnt_timer(ev_tlocount, ev_thicount : INTEGER) : INTEGER;
FUNCTION evnt_multi(ev_mflags, ev_mbclicks, ev_mbmask, ev_mbstate, ev_mm1flags,
                    ev_mm1x, ev_mm1y, ev_mm1width, ev_m1height, ev_mm2flags,
                    ev_mm2x, ev_mm2y, ev_mm2width, ev_mm2height : INTEGER;
                    ev_mmgpbuff : ADDRESS; ev_mtlocount, ev_mthicount : INTEGER;
                    VAR ev_mmox, ev_mmoy, ev_mmobutton, ev_mmokstate, ev_mkreturn,
                    ev_mbreturn : INTEGER) : INTEGER;
FUNCTION evnt_dclick(ev_dnew, ev_dgtest : INTEGER) : INTEGER;

FUNCTION menu_bar(me_btree : ADDRESS; me_bshow : INTEGER) : INTEGER;
FUNCTION menu_icheck(me_ctree : ADDRESS; me_citem, me_ccheck : INTEGER) : INTEGER;
FUNCTION menu_ienable(me_etree : ADDRESS; me_eitem, me_eenable : INTEGER) : INTEGER;
FUNCTION menu_tnormal(me_ntree : ADDRESS; me_ntitle, me_nnormal : INTEGER) : INTEGER;
FUNCTION menu_text(me_ttree : ADDRESS; me_titem : INTEGER; VAR me_ttext) : INTEGER;
FUNCTION menu_register(me_raccapid : INTEGER; VAR me_rpstring) : INTEGER;
FUNCTION menu_unregister(mu_mid : INTEGER) : INTEGER;

FUNCTION form_do(fo_dotree : ADDRESS; fo_dostartob : INTEGER) : INTEGER;
FUNCTION form_dial(fo_diflag, fo_dilx, fo_dily, fo_dilw, fo_dilh, fo_dibigx, fo_dibigy,
                   fo_dibigw, fo_dibigh : INTEGER) : INTEGER;
FUNCTION form_alert(fo_adefbttn : INTEGER; VAR fo_aastring) : INTEGER;
FUNCTION form_error(fo_enum : INTEGER) : INTEGER;
FUNCTION form_center(fo_ctree : ADDRESS; VAR fo_cx, fo_cy, fo_cw, fo_ch : INTEGER)
                     : INTEGER;
FUNCTION form_keybd(fo_key : ADDRESS; fo_obj, fo_nxt, fo_thechar : INTEGER; VAR fo_pnxt,
                    fo_pchar : INTEGER) : INTEGER;
FUNCTION form_button(fo_but : ADDRESS; fo_obj, fo_clks : INTEGER; VAR fo_pnxt : INTEGER)
                     : INTEGER;

FUNCTION graf_rubbox(gr_rx, gr_ry, gr_rminwidth, gr_rminheight : INTEGER;
                     VAR gr_rlastwidth, gr_rlastheight : INTEGER) : INTEGER;
FUNCTION graf_dragbox(gr_dwidth, gr_dheight, gr_dstx, gr_dsty, gr_dbndx, gr_dbndy,
                      gr_dbndw, gr_dbndh : INTEGER; VAR gr_dfinx, gr_dfiny : INTEGER)
                      : INTEGER;
FUNCTION graf_mbox(gr_mwidth, gr_mheight, gr_msrcex, gr_msrcey, gr_mdestx,
                   gr_mdesty : INTEGER) : INTEGER;
FUNCTION graf_watchbox(gr_wptree : ADDRESS; gr_wobject, gr_winstate,
                       gr_woutstate : INTEGER) : INTEGER;
FUNCTION graf_slidebox(gr_slptree : ADDRESS; gr_slparent, gr_slobject, gr_slvh : INTEGER)
                       : INTEGER;
FUNCTION graf_handle(VAR gr_hwchar, gr_hhchar, gr_hwbox, gr_hhbox : INTEGER) : INTEGER;
FUNCTION graf_mouse(gr_monumber : INTEGER; gr_mofaddr : ADDRESS) : INTEGER;
FUNCTION graf_mkstate(VAR gr_mkmx, gr_mkmy, gr_mkmstate, gr_mkkstate : INTEGER) : INTEGER;
FUNCTION scrp_read(VAR sc_rpath) : INTEGER;
FUNCTION scrp_write(VAR sc_pscrap ) : INTEGER;
FUNCTION scrp_clear : INTEGER;

FUNCTION wind_create(wi_crkind, wi_crwx, wi_crwy, wi_crww, wi_crwh : INTEGER) : INTEGER;
FUNCTION wind_open(wi_ohandle, wi_owx, wi_owy, wi_oww, wi_owh : INTEGER) : INTEGER;
FUNCTION wind_close(wi_clhandle : INTEGER) : INTEGER;
FUNCTION wind_delete(wi_dhandle : INTEGER) : INTEGER;
FUNCTION wind_get(wi_ghandle, wi_gfield : INTEGER; VAR wi_gw1, wi_gw2, wi_gw3,
                  wi_gw4 : INTEGER) : INTEGER;
FUNCTION wind_set(wi_shandle, wi_sfield, wi_sw1, wi_sw2, wi_sw3, wi_sw4 : INTEGER)
                  : INTEGER;
FUNCTION wind_find(wi_fmx, wi_fmy : INTEGER) : INTEGER;
FUNCTION wind_update(wi_ubegend : INTEGER) : INTEGER;
FUNCTION wind_calc(wi_ctype, wi_ckind, wi_cx, wi_cy, wi_cinwidth, wi_cinheight : INTEGER;
                   VAR wi_coutx, wi_couty, wi_coutwidth, wi_coutheight : INTEGER)
                   : INTEGER;

FUNCTION rsrc_load(VAR re_lpfname) : INTEGER;
FUNCTION rsrc_free : INTEGER;
FUNCTION rsrc_gaddr(re_gttype, re_gtindex : INTEGER; VAR re_gtaddr : ADDRESS) : INTEGER;
FUNCTION rsrc_saddr(re_sttype, re_stindex : INTEGER; re_staddr : ADDRESS) : INTEGER;
FUNCTION rsrc_obfix(re_otree : ADDRESS; re_oobject : INTEGER) : INTEGER;

FUNCTION shel_read(VAR sh_rpcmd, sh_rptail) : INTEGER;
FUNCTION shel_write(sh_wdoex, sh_wisgr, sh_wiscr : INTEGER; VAR sh_wpcmd, sh_wptail)
                    : INTEGER;
FUNCTION shel_find(VAR sh_ppath ) : INTEGER;
FUNCTION shel_envrn(sh_ppath : ADDRESS; VAR sh_psrch) : INTEGER;
FUNCTION shel_rdef(VAR sh_lpcmd, sh_lpdir) : INTEGER;
FUNCTION shel_wdef(VAR sh_lpcmd, sh_lpdir) : INTEGER;

FUNCTION xgrf_stepcalc(xg_orgw, xg_orgh, xg_xc, xg_yc, xg_w, xg_h : INTEGER;
                       VAR xg_pcx, xg_pcy, xg_pcnt, xg_pxstep, xg_pystep : INTEGER)
                       : INTEGER;
FUNCTION xgrf_2box(xg_xc, xg_yc, xg_w, xg_h, xg_corners, xg_cnt, xg_xstep, xg_ystep,
                   xg_doubled : INTEGER) : INTEGER;
FUNCTION graf_growbox(gr_gstx, gr_gsty, gr_gstwidth, gr_gstheight, gr_gfinx, gr_gfiny,
                      gr_gfinwidth, gr_gfinheight : INTEGER) : INTEGER;
FUNCTION graf_shrinkbox(gr_sstx, gr_ssty, gr_sstwidth, gr_sstheight, gr_sfinx, gr_sfiny,
                        gr_sfinwidth, gr_sfinheight : INTEGER) : INTEGER;

{ and that's the AES over with... :) }

{ The VDI type bindings are next, as they are neede for the debugger (MFDB &c.) }

  CONST
    cntl_max = 11;
        (* max sizes for arrays - relative 0 *)
    intin_max = 131;
    intout_max = 139;
    pts_max = 144;
        (* you may need this larger *)

    white = 0;
        (* std colors - not always true *)
    black = 1;
    red = 2;
    green = 3;
    blue = 4;
    cyan = 5;
    yellow = 6;
    magenta = 7;
    DWHITE =8;
    DBLACK =9;
    DRED =10;
    DGREEN =11;
    DBLUE =12;
    DCYAN =13;
    DYELLOW =14;
    DMAGENTA =15;

    hollow = 0;
        (* std fill interior styles *)
    solid = 1;
    PATTERN = 2;
    hatch = 3;

    longdash = 2;
        (* line styles, solid as above *)
    dshdot = 4;

    maxndc = 32767;
        (* max coord in NDC space *)

    normal = 0;
        (* graphic text styles - combinations ok as well *)
    bold = 1;
    light = 2;
    skew = 4;
    underline = 8;
    outline = 16;
    shadow = 32;

    request = 1;
        (* input modes *)
    sample = 2;

    replace = 1;
        (* write modes *)
    transparent = 2;
    xor_mode = 3;
    ERASE = 4;

  (**  Standard Types for GEM VDI Turbo Pascal programs         **)
  TYPE
    contrl_ARRAY = ARRAY[0..cntl_max] OF INTEGER;   (** All arrays relative **)
    intin_ARRAY = ARRAY[0..intin_max] OF INTEGER;   (** to zero here **)
    intout_ARRAY = ARRAY[0..intout_max] OF INTEGER;
    ptsin_ARRAY = ARRAY[0..pts_max] OF INTEGER;
    ptsout_ARRAY = ARRAY[0..pts_max] OF INTEGER;

    (* type defns for gemtools *)
    ARRAY_57 = ARRAY[0..56] OF INTEGER;        (* std arrray sizes *)
    ARRAY_3 = ARRAY[0..2] OF INTEGER;
    ARRAY_2 = ARRAY[0..1] OF INTEGER;
    ARRAY_4 = ARRAY[0..3] OF INTEGER;
    ARRAY_5 = ARRAY[0..4] OF INTEGER;
    ARRAY_6 = ARRAY[0..5] OF INTEGER;
    ARRAY_8 = ARRAY[0..7] OF INTEGER;
    ARRAY_10 = ARRAY[0..9] OF INTEGER;
    ARRAY_11 = ARRAY[0..10] OF INTEGER;
    ARRAY_16 = ARRAY[0..15] OF INTEGER;
    ARRAY_37 = ARRAY[0..36] OF INTEGER;

    MFDB = RECORD        (* MFDB layout *)
             mp  : ADDRESS;       (* words 1 & 2  - memory pointer *)
             fwp : INTEGER;       (* word 3 - width in pixels  *)
             fh  : INTEGER;       (* word 4 - height in lines  *)
             fww : INTEGER;       (* word 5 - width in words   *)
             ff  : INTEGER;       (* word 6 - format flag      *)
             np  : INTEGER;       (* word 7 - number of planes *)
             r1  : INTEGER;       (* reserved for future use *)
             r2  : INTEGER;
             r3  : INTEGER;
           END;               (* end MFDB record layout *)

    GRECT = RECORD      (* used to describe location of objects & windows *)
         g_x : Integer;
         g_y : Integer;
         g_w : Integer;
         g_h : Integer;
       END;

 (**  Standard Variables for GEM VDI Turbo Pascal programs         **)
  VAR
    contrl : contrl_ARRAY;        (* global arrays reqd by gemvdi *)
    intin : intin_ARRAY;
    intout : intout_ARRAY;
    ptsin : ptsin_ARRAY;
    ptsout : ptsout_ARRAY;


{ The debugger only has procedures/functions to deal with }
PROCEDURE Send_msg (VAR instrg);
PROCEDURE Tell_Addr (VAR instrg; anyPtr  : ADDRESS);
PROCEDURE Tell_Byte (VAR instrg, inPtr);
PROCEDURE Tell_Int (VAR instrg; anyInt : Integer);
PROCEDURE Tell_Real (VAR instrg; anyReal : Real);
PROCEDURE Tell_Mfdb (VAR instrg; anyMFDB : MFDB);
PROCEDURE Tell_Grect (VAR instrg, inGrect);
PROCEDURE Tell_Contrl (VAR instrg; anyContrl : Contrl_ARRAY);
PROCEDURE KeyCont;

{ The VDI headers come next.  Enjoy them. }

PROCEDURE GVDI(VAR gptsout : ptsout_ARRAY; VAR gintout : intout_ARRAY;
               VAR gptsin : ptsin_ARRAY; VAR gintin : intin_ARRAY;
               VAR gcontrl : contrl_ARRAY);
FUNCTION gemvdif(opcode, handle : INTEGER) : INTEGER;
FUNCTION v_opnwk(workin : ARRAY_11; VAR handle : INTEGER; VAR workout : ARRAY_57)
                 : INTEGER;
FUNCTION v_clswk(handle : INTEGER) : INTEGER;
FUNCTION v_clrwk(handle : INTEGER) : INTEGER;
FUNCTION v_updwk(handle : INTEGER) : INTEGER;
FUNCTION v_opnvwk(workin : ARRAY_11; VAR handle : INTEGER; VAR workout : ARRAY_57)
                  : INTEGER;
FUNCTION v_clsvwk(handle : INTEGER) : INTEGER;
FUNCTION vst_load_fonts(handle, select : INTEGER) : INTEGER;
FUNCTION vst_unload_fonts(handle, select : INTEGER) : INTEGER;
FUNCTION vs_clip(handle : INTEGER; clipflag : INTEGER; pxyarray : ARRAY_4) : INTEGER;
FUNCTION v_pline(handle, count : INTEGER; VAR pxyarray) : INTEGER;
FUNCTION v_pmarker(handle, count : INTEGER; VAR pxyarray ) : INTEGER;
FUNCTION v_gtext(handle, x, y : INTEGER; VAR chstring ) : INTEGER;
FUNCTION v_fillarea(handle, count : INTEGER; VAR pxyarray) : INTEGER;
FUNCTION v_cellarray(handle : INTEGER; pxyarray : ARRAY_4; rowlength, elused, numrows,
                     wrtmode : INTEGER; VAR col_array) : INTEGER;
FUNCTION v_bar(handle : INTEGER; pxyarray : ARRAY_4) : INTEGER;
FUNCTION v_arc(handle, x, y, radius, begang, endang : INTEGER) : INTEGER;
FUNCTION v_pieslice(handle, x, y, radius, begang, endang : INTEGER) : INTEGER;
FUNCTION v_circle(handle, x, y, radius : INTEGER) : INTEGER;
FUNCTION v_ellipse(handle, x, y, xradius, yradius : INTEGER) : INTEGER;
FUNCTION v_ellarc(handle, x, y, xradius, yradius, begang, endang : INTEGER) : INTEGER;
FUNCTION v_ellpie(handle, x, y, xradius, yradius, begang, endang : INTEGER) : INTEGER;
FUNCTION v_rbox(handle : INTEGER; xyarray : ARRAY_4) : INTEGER;
FUNCTION v_rfbox(handle : INTEGER; xyarray : ARRAY_4) : INTEGER;
FUNCTION v_justified(handle, x, y : INTEGER; VAR gstring; jlength : INTEGER;
                     wordspace, charspace : INTEGER) : INTEGER;
FUNCTION v_contourfill(handle, x, y, index : INTEGER) : INTEGER;
FUNCTION vr_recfl(handle : INTEGER; pxyarray : ARRAY_4) : INTEGER;
FUNCTION genset(opcode, handle, param : INTEGER) : INTEGER;
FUNCTION vswr_mode(handle, mode : INTEGER) : INTEGER;
FUNCTION vs_color(handle, index : INTEGER; rgbin : ARRAY_3) : INTEGER;
FUNCTION vsl_type(handle, style : INTEGER) : INTEGER;
FUNCTION vsl_udsty(handle, PATTERN : INTEGER) : INTEGER;
FUNCTION vsl_width(handle, width : INTEGER) : INTEGER;
FUNCTION vsl_color(handle, colindex : INTEGER) : INTEGER;
FUNCTION vsl_ends(handle, begstyle, endstyle : INTEGER) : INTEGER;
FUNCTION vsm_type(handle, symbol : INTEGER) : INTEGER;
FUNCTION vsm_height(handle, height : INTEGER) : INTEGER;
FUNCTION vsm_color(handle, colindex : INTEGER) : INTEGER;
FUNCTION vst_height(handle, height : INTEGER; VAR charwidth, charheight,
                    cellwidth, cellheight : INTEGER) : INTEGER;
FUNCTION vst_point(handle, point : INTEGER; VAR charwidth, charheight,
                   cellwidth, cellheight : INTEGER) : INTEGER;
FUNCTION vst_rotation(handle, angle : INTEGER) : INTEGER;
FUNCTION vst_font(handle, font : INTEGER) : INTEGER;
FUNCTION vst_color(handle, colindex : INTEGER) : INTEGER;
FUNCTION vst_effects(handle, effects : INTEGER) : INTEGER;
FUNCTION vst_alignment(handle, horin, vertin : INTEGER; VAR horout, vertout
                       : INTEGER) : INTEGER;
FUNCTION vsf_interior(handle, style : INTEGER) : INTEGER;
FUNCTION vsf_style(handle, styleindex : INTEGER) : INTEGER;
FUNCTION vsf_color(handle, colorindex : INTEGER) : INTEGER;
FUNCTION vsf_perimeter(handle, pervis : INTEGER) : INTEGER;
FUNCTION vsf_updat(handle : INTEGER; VAR pfill_pat; planes : INTEGER) : INTEGER;
FUNCTION v_get_pixel(handle, v_gx, v_gy : INTEGER; VAR v_pel, v_index : INTEGER)
                     : INTEGER;
FUNCTION vro_cpyfm (handle, wrmode : INTEGER; pxyarray : ARRAY_8; psrcMFDB,
                    pdesMFDB : MFDB) : INTEGER;
FUNCTION vr_trnfm (handle : INTEGER; psrcMFDB : MFDB; VAR pdesMFDB : MFDB) : INTEGER;
FUNCTION vrt_cpyfm (handle, wrmode : INTEGER; pxyarray : ARRAY_8; psrcMFDB,
                    pdesMFDB : MFDB; color : ARRAY_2) : INTEGER;
FUNCTION vsin_mode(handle, devtype, mode : INTEGER) : INTEGER;
FUNCTION vrq_locator(handle, x, y : INTEGER; VAR xout, yout : INTEGER; VAR term
                     : CHAR) : INTEGER;
FUNCTION vsm_locator(handle, x, y : INTEGER; VAR xout, yout, term : INTEGER) : INTEGER;
FUNCTION vrq_valuator(handle, valin : INTEGER; VAR valout : INTEGER; VAR term : CHAR)
                      : INTEGER;
FUNCTION vsm_valuator(handle, valin : INTEGER; VAR valout : INTEGER; VAR term : CHAR;
                      VAR status : INTEGER) : INTEGER;
FUNCTION vrq_choice(handle, in_choice : INTEGER; VAR out_choice : INTEGER) : INTEGER;
FUNCTION vsm_choice(handle : INTEGER; VAR choice : INTEGER) : INTEGER;
FUNCTION vrq_string(handle, maxlen, echomode : INTEGER; echoxy : ARRAY_2; VAR instring)
                    : INTEGER;
FUNCTION vsm_string(handle, maxlen, echomode : INTEGER; echoxy : ARRAY_2; VAR instring;
                      VAR status : INTEGER) : INTEGER;
FUNCTION vsc_form(handle : INTEGER; pcurform : ARRAY_37) : INTEGER;
FUNCTION vex_timv (handle : INTEGER; timaddr : ADDRESS; VAR otimaddr : ADDRESS;
                   VAR timconv : INTEGER) : INTEGER;
FUNCTION v_show_c(handle, RESET : INTEGER) : INTEGER;
FUNCTION v_hide_c(handle : INTEGER) : INTEGER;
FUNCTION vq_mouse(handle : INTEGER; VAR pstatus, x, y : INTEGER) : INTEGER;
FUNCTION vex_butv (handle : INTEGER; pusrcode : ADDRESS; VAR psavcode : ADDRESS)
                   : INTEGER;
FUNCTION vex_motv (handle : INTEGER; pusrcode : ADDRESS; VAR psavcode : ADDRESS)
                   : INTEGER;
FUNCTION vex_curv (handle : INTEGER; pusrcode : ADDRESS; VAR psavcode : ADDRESS)
                   : INTEGER;
FUNCTION vq_key_s(handle : INTEGER; VAR pstatus : INTEGER) : INTEGER;
FUNCTION vq_color(handle, colorindex, setflag : INTEGER; VAR rgb : ARRAY_3) : INTEGER;
FUNCTION vq_cellarray(handle : INTEGER; pxyarray : ARRAY_4; rowlen, numrows : INTEGER;
                      VAR elused, rowsused, status : INTEGER; VAR colarray
                      : intout_ARRAY) : INTEGER;
FUNCTION vql_attributes(handle : INTEGER; VAR attrib : ARRAY_6) : INTEGER;
FUNCTION vqm_attributes(handle : INTEGER; VAR attrib : ARRAY_5) : INTEGER;
FUNCTION vqf_attributes(handle : INTEGER; VAR attrib : ARRAY_5) : INTEGER;
FUNCTION vqt_attributes(handle : INTEGER; VAR attrib : ARRAY_10) : INTEGER;
FUNCTION vq_extnd(handle, owflag : INTEGER; VAR workout : ARRAY_57) : INTEGER;
FUNCTION vqin_mode(handle, devtype : INTEGER;VAR inputmode : INTEGER) : INTEGER;
FUNCTION vqt_extent(handle : INTEGER; VAR chstring; VAR extent : ARRAY_8) : INTEGER;
FUNCTION vqt_width(handle : INTEGER; character : CHAR; VAR cellwidth, leftdelta,
                   rightdelta : INTEGER) : INTEGER;
FUNCTION vqt_name(handle, elementnum : INTEGER; VAR name ) : INTEGER;
FUNCTION vqt_font_info(handle : INTEGER; VAR minADE, maxADE : INTEGER; VAR distances
                       : ARRAY_5; VAR maxwidth : INTEGER; effects : ARRAY_3) : INTEGER;
FUNCTION vqt_justified(handle, x, y : INTEGER; VAR gstring; jlength, wordspace,
                       charspace : INTEGER; VAR offsets : ptsout_ARRAY) : INTEGER;
FUNCTION vq_chcells(handle : INTEGER; VAR rows, columns : INTEGER) : INTEGER;
FUNCTION genescape(fid, handle : INTEGER) : INTEGER;
FUNCTION v_exit_cur(handle : INTEGER) : INTEGER;
FUNCTION v_enter_cur(handle : INTEGER) : INTEGER;
FUNCTION v_curup(handle : INTEGER) : INTEGER;
FUNCTION v_curdown(handle : INTEGER) : INTEGER;
FUNCTION v_curright(handle : INTEGER) : INTEGER;
FUNCTION v_curleft(handle : INTEGER) : INTEGER;
FUNCTION v_curhome(handle : INTEGER) : INTEGER;
FUNCTION v_eeos(handle : INTEGER) : INTEGER;
FUNCTION v_eeol(handle : INTEGER) : INTEGER;
FUNCTION vs_curaddress(handle, row, column : INTEGER) : INTEGER;
FUNCTION v_curtext(handle : INTEGER; VAR chstring ) : INTEGER;
FUNCTION v_rvon(handle : INTEGER) : INTEGER;
FUNCTION v_rvoff(handle : INTEGER) : INTEGER;
FUNCTION vq_curaddress(handle : INTEGER; VAR row, column : INTEGER) : INTEGER;
FUNCTION vq_tabstatus(handle : INTEGER) : INTEGER;
FUNCTION v_hardcopy(handle : INTEGER) : INTEGER;
FUNCTION v_dspcur(handle, x, y : INTEGER) : INTEGER;
FUNCTION v_rmcur(handle : INTEGER) : INTEGER;
FUNCTION v_form_adv(handle : INTEGER) : INTEGER;
FUNCTION v_output_window(handle : INTEGER; xyarray : ARRAY_4) : INTEGER;
FUNCTION v_clear_display_list(handle : INTEGER) : INTEGER;
FUNCTION v_bit_image(handle : INTEGER; VAR filename; aspect, xscale, yscale,
                     halign, valign : INTEGER; xyarray : ARRAY_4) : INTEGER;
FUNCTION vq_scan(handle : INTEGER; VAR gheight, gslices, aheight, aslices,
                 factor : INTEGER) : integer;
FUNCTION v_alpha_text(handle : INTEGER; VAR tstring) : INTEGER;
FUNCTION vs_palette(handle, PALETTE : INTEGER) : INTEGER;
FUNCTION v_sound(handle, frequency, duration : INTEGER) : INTEGER;
FUNCTION v_mute(handle, action : INTEGER) : INTEGER;
FUNCTION vt_resolution(handle, xres, yres : INTEGER; VAR xset, yset : INTEGER)
                       : INTEGER;
FUNCTION vt_axis(handle, xres, yres : INTEGER; VAR xset, yset : INTEGER) : INTEGER;
FUNCTION vt_origin(handle, xorigin, yorigin : INTEGER) : INTEGER;
FUNCTION vq_tdimensions(handle : INTEGER; VAR xdim, ydim : INTEGER) : INTEGER;
FUNCTION vt_alignment(handle, dx, dy : INTEGER) : INTEGER;
FUNCTION vsp_film(handle, index, lightness : INTEGER) : INTEGER;
FUNCTION vqp_filmname(handle, index : INTEGER; VAR name ) : INTEGER;
FUNCTION vsc_expose(handle, state : INTEGER) : INTEGER;
FUNCTION v_meta_extents(handle, minx, miny, maxx, maxy : INTEGER) : INTEGER;
FUNCTION v_write_meta(handle, numintin : INTEGER; VAR var_int_in; numptsin : INTEGER;
                      VAR var_pts_in ) : INTEGER;
FUNCTION vm_filename(handle : INTEGER; VAR filename) : INTEGER;
FUNCTION vm_pagesize(handle, pgwidth, pgheight : INTEGER) : INTEGER;
FUNCTION vm_coords(handle, llx, lly, urx, ury : INTEGER) : INTEGER;

{ OK, VDI done, and object definitions to follow }

Const
 ROOT =0;
 CNIL =-1;
                          { keybd states         }
 K_RSHIFT =$0001;
 K_LSHIFT =$0002;
 K_CTRL =$0004;
 K_ALT =$00008;
                         { max string length    }
 MAX_LEN =81;
                         { max depth of search  }
                         {   or draw for objects}
 MAX_DEPTH =8;
                         { inside patterns      }
 IP_HOLLOW =0;
 IP_1PATT =1;
 IP_2PATT =2;
 IP_3PATT =3;
 IP_4PATT =4;
 IP_5PATT =5;
 IP_6PATT =6;
 IP_SOLID =7;
                         { system foreground and}
                         {   background rules   }
                         {   but transparent    }
 SYS_FG =$1100;

 WTS_FG =$11a1;          { window title selected}
                         {   using pattern =2 &  }
                         {   replace mode text  }
 WTN_FG =$1100;          { window title normal  }

                         { gsx modes            }
 MD_REPLACE =1;
 MD_TRANS =2;
 MD_XOR =3;
 MD_ERASE =4;
                         { gsx styles           }
 FIS_HOLLOW =0;
 FIS_SOLID =1;
 FIS_PATTERN =2;
 FIS_HATCH =3;
 FIS_USER =4;
                         { bit blt rules        }
 ALL_WHITE =0;
 S_AND_D =1;
 S_ONLY =3;
 NOTS_AND_D =4;
 S_XOR_D =6;
 S_OR_D =7;
 D_INVERT =10;
 NOTS_OR_D =13;
 ALL_BLACK =15;
                          { font types           }
 IBM =3;
 SMALL =5;

{ Object Drawing Types }
                          { Graphic types of obs }
 G_BOX =20;
 G_TEXT =21;
 G_BOXTEXT =22;
 G_IMAGE =23;
 G_USERDEF =24;
 G_PROGDEF =24;
 G_IBOX =25;
 G_BUTTON =26;
 G_BOXCHAR =27;
 G_STRING =28;
 G_FTEXT =29;
 G_FBOXTEXT =30;
 G_ICON =31;
 G_TITLE =32;
                          { Object flags          }
 NONE =$0;
 SELECTABLE =$1;
 DEFAULT =$2;
 EXIT =$4;
 EDITABLE =$8;
 RBUTTON =$10;
 LASTOB =$20;
 TOUCHEXIT =$40;
 HIDETREE =$80;
 INDIRECT =$100;
                          { Object states        }
 SELECTED =$1;
 CROSSED =$2;
 CHECKED =$4;
 DISABLED =$8;
 OUTLINED =$10;
 SHADOWED =$20;
 WHITEBAK =$40;
 DRAW3D =$80;
{ colors are defined in bindings}


type

   UIOBJECT = record
        ob_next : integer;    { -> object's next sibling     }
        ob_head : integer;    { -> head of object's children }
        ob_tail : integer;    { -> tail of object's children }
        ob_type : integer;    { type of object- BOX, CHAR,...}
        ob_flags: integer;    { flags                        }
        ob_state: integer;    { state- SELECTED, OPEN, ...   }
        ob_spec : address; { "out"- -> anything else      }
        ob_x    : integer;    { upper left corner of object  }
        ob_y    : integer;    { upper left corner of object  }
        ob_width: integer;    { width of obj                 }
       ob_height: integer;    { height of obj                }
      end;

(*  ORECT struct orect is defined in bindings *)


(*  GRECT defined in VDIDEF.INC *)


   TEDINFO = record
       te_ptext : Address;       { ptr to text (must be =1st)    }
       te_ptmplt: address;       { ptr to template              }
       te_pvalid: Address;       { ptr to validation chrs.      }
       te_font  : integer;          { font                         }
       te_junk1 : integer;          { junk integer                    }
       te_just  : integer;          { justification- left, right...}
       te_color : integer;          { color index integer       }
       te_junk2 : integer;          { junk integer                    }
    te_thickness: integer;          { border thickness             }
       te_txtlen: integer;          { length of text string        }
       te_tmplen: integer;          { length of template string    }
    end;


   ICONBLK = record
       ib_pmask : Address;
       ib_pdata : Address;
       ib_ptext : Address;
       ib_char  : integer;
       ib_xchar : integer;
       ib_ychar : integer;
       ib_xicon : integer;
       ib_yicon : integer;
       ib_wicon : integer;
       ib_hicon : integer;
       ib_xtext : integer;
       ib_ytext : integer;
       ib_wtext : integer;
       ib_htext : integer;
     end;


   BITBLK = record
       bi_pdata : Address;               { ptr to bit forms data        }
       bi_wb    : integer;                  { width of form in bytes       }
       bi_hl    : integer;                  { height in lines              }
       bi_x     : integer;                  { source x in bit form         }
       bi_y     : integer;                  { source y in bit form         }
       bi_color : integer;                  { color index of block         }
   end;

   APPLBLK = record                     { same as USERBLK in Balma & Fitler }
     ab_code : address;
     ab_parm : address;
   end;

   PARMBLK = record
      pb_tree  : address;               {ptr to object tree            }
      pb_obj    : integer;               { index of object              }
      pb_prevstate : integer;             { previous state of object     }
      pb_currstate  : integer;               { current state of object      }
      pb_x  : integer;               { object's x coord             }
      pb_y  : integer;               { object's y coord             }
      pb_w  : integer;               { object's width               }
      pb_h  : integer;               { object's height              }
      pb_xc : integer;               { clip area's x coord          }
      pb_yc : integer;               { clip area's y coord          }
      pb_wc : integer;               { clip area's width            }
      pb_hc : integer;               { clip area's height           }
      parameter : address;               { optional long value          }
   end;

(*  MFDB structure is defined in bindings *)

Function Fobadr(a,x,y:integer):ADDRESS;  { used by rest of functions }
Function Fpbadr(x:ADDRESS;y:integer):ADDRESS;  { used by rest of functions }
Function Fob_next (x:integer):ADDRESS;
Function Fob_head (x:integer):ADDRESS;
Function Fob_tail (x:integer):ADDRESS;
Function Fob_type (x:integer):ADDRESS;
Function Fob_flags(x:integer):ADDRESS;
Function Fob_state(x:integer):ADDRESS;
Function Fob_spec(x:integer):ADDRESS;
Function Fob_X(x:integer):ADDRESS;
Function Fob_Y(x:integer):ADDRESS;
Function Fob_Width(x:integer):ADDRESS;
Function Fob_Height(x:integer):ADDRESS;
Function FTE_PTEXT(x:integer):ADDRESS;
Function FTE_TXTLEN(x:integer):ADDRESS;
Function FBI_PDATA(x:ADDRESS):ADDRESS; (* BITBLK - image data *)
Function FBI_WB(x:ADDRESS):ADDRESS;
Function FBI_HL(x:ADDRESS):ADDRESS;
Function FIB_PMASK(x:ADDRESS):ADDRESS;
Function FIB_PDATA(x:ADDRESS):ADDRESS;
Function FIB_WICON(x:ADDRESS):ADDRESS;
Function FIB_HICON(x:ADDRESS):ADDRESS;

{ Machine-specific bindings time }

Function LLDS : ADDRESS;
Function LLCS : ADDRESS;
Function LBGET(inptr : ADDRESS) : Byte;
Procedure LBSET (inptr : ADDRESS; invalue : Byte);
Function LWGET(inptr : ADDRESS) : Integer;
Procedure LWSET (inptr : ADDRESS; invalue : Integer);
Function LLGET (inptr : ADDRESS) : ADDRESS;
Procedure LLSET (inptr, invalue : ADDRESS);
Function LBYTE0(inptr  : ADDRESS) : Byte;
Function LBYTE1(inptr : ADDRESS) : Byte;
Function LBYTE2(inptr : ADDRESS) : Byte;
Function LBYTE3(inptr : ADDRESS) : Byte;
FUNCTION umul (ifactor1, ifactor2 : Integer) : Integer;
FUNCTION udiv (idividend, idivisor : Integer) : Integer;
FUNCTION umul_div (ifactor1, ifactor2, idivisor : Integer) : Integer;


const

{ redefine all structure offsets as constants }

    { OBJECT offsets }
   Kob_size = 24;  { size of object in words}
   Kob_next  = 0;
   Kob_head  = 2;
   Kob_tail  = 4;
   Kob_type  = 6;
   Kob_flags = 8;
   Kob_state = 10;
   Kob_spec  = 12;
   Kob_x     = 16;
   Kob_y     = 18;
   Kob_w     = 20;
   Kob_h     = 22;

   { TEDINFO offsets }
   Kte_size   = 28; {size of TEDINFO}
   Kte_ptext  = 0;
   Kte_ptmplt = 4;
   Kte_pvalid = 8;
   Kte_font   = 12;
   Kte_junk1  = 14;
   Kte_just   = 16;
   Kte_color  = 18;
   Kte_junk2  = 20;
   Kte_thickness = 22;
   Kte_txtlen  = 24;
   Kte_tmplen  = 26;

   { ICONBLK offsets }
   KIB_size = 36; {size of ICONBLK}
   Kib_pmask =0;
   Kib_pdata =4;
   Kib_ptext =8;
   Kib_char  =12;
   Kib_xchar =14;
   Kib_ychar =16;
   Kib_xicon =18;
   Kib_yicon =20;
   Kib_wicon =22;
   Kib_hicon =24;
   Kib_xtext =26;
   Kib_ytext =28;
   Kib_wtext =30;
   Kib_htext =32;

   { BITBLK offsets}
   Kbi_size = 14;
   Kbi_pdata =0;
   Kbi_wb    =4;
   Kbi_hl    =6;
   Kbi_x     =8;
   Kbi_y     =10;
   Kbi_color =12;

   { APPLBLK offsets }
   Kab_size =4;
   Kab_code =0;
   Kab_parm =2;

   { PARMBLK offsets }
   Kpb_size  =15;
   Kpb_tree =0;
   Kpb_obj   =2;
   Kpb_prevstate=3;
   Kpb_currstate =4;
   Kpb_x =5;
   Kpb_y =6;
   Kpb_w =7;
   Kpb_h =8;
   Kpb_xc =9;
   Kpb_yc =10;
   Kpb_wc =11;
   Kpb_hc =12;
   Kpb_param=13;

 EDSTART =0;
 EDINIT =1;
 EDCHAR =2;
 EDEND =3;

 TE_LEFT =0;
 TE_RIGHT =1;
 TE_CNTR =2;

(* constants for use with resource library calls in AES *)

 R_TREE =0;
 R_OBJECT =1;
 R_TEDINFO =2;
 R_ICONBLK =3;
 R_BITBLK =4;
 R_STRING =5;        (* gets pointer to free strings *)
 R_IMAGEDATA =6;    (* gets pointer to free images *)
 R_OBSPEC =7;
 R_TEPTEXT =8;      (* sub ptrs in TEDINFO *)
 R_TEPTMPLT =9;
 R_TEPVALID =10;
 R_IBPMASK =11;      (* sub ptrs in ICONBLK *)
 R_IBPDATA =12;
 R_IBPTEXT =13;
 R_BIPDATA =14;      (* sub ptrs in BITBLK *)
 R_FRSTR =15;        (* gets addr of ptr to free strings *)
 R_FRIMG =16;        (* gets addr of ptr to free images *)

 Var
  tree : ADDRESS; { global variable }

{ DOS Bindings }
TYPE
    dta = record
      dos_reserve : ARRAY [0..20] OF BYTE;
      fileattr : BYTE;
      time : Integer;
      date : Integer;
      sizelo : Integer;
      sizehi : Integer;
      filename : ARRAY [0..12] OF CHAR;      { C style string }
    END;

VAR
    dos_reg : registers;
    dos_err : Boolean;

PROCEDURE call_dos;
PROCEDURE dos_func (ax, lodsdx, hidsdx : Integer);
FUNCTION dos_chdir(VAR pdrvpath ) : Integer;
FUNCTION dos_gdir(drive : Integer; VAR pdrvpath ) : Integer;
FUNCTION dos_gdrv : Integer;
FUNCTION dos_sdrv (newdrv : Integer) : Integer;
FUNCTION dos_sdta (VAR ldta) : Integer;
FUNCTION dos_gdta : ADDRESS;
FUNCTION dos_sfirst (VAR pspec; attr : Integer) : Integer;
FUNCTION dos_snext : Integer;
FUNCTION dos_open (VAR pname; access : Integer) : Integer;
FUNCTION dos_close (handle : Integer) : Integer;
FUNCTION block_read (handle, icnt : Integer; buff_ptr : LONG) : Integer;
FUNCTION dos_read (handle : Integer; cnt : Real; buff_loc : ADDRESS) : Real;
FUNCTION dos_lseek (handle, smode : Integer; sofst : LONG) : ADDRESS;
FUNCTION dos_wait : Integer;
FUNCTION dos_alloc (nbytes : Real) : ADDRESS;
FUNCTION dos_avail : REAL;
FUNCTION dos_free (VAR maddr) : Integer;
FUNCTION dos_space (drv : Integer; VAR ptotal, pAvail : Real) : Integer;
FUNCTION dos_rmdir (VAR ppath) : Integer;
FUNCTION dos_create (VAR pname; attr : Integer) : Integer;
FUNCTION dos_mkdir (VAR ppath) : Integer;
FUNCTION dos_delete (VAR pname) : Integer;
FUNCTION dos_rename (VAR poname, pnname) : Integer;
FUNCTION block_write (handle, icnt : Integer; buff_ptr : LONG): Integer;
FUNCTION dos_write (handle : Integer; cnt : Real; buff_loc : ADDRESS) : Real;
FUNCTION dos_chmod(VAR pname; func, attr : Integer) : Integer;
FUNCTION dos_setdt(handle, date, time : Integer) : Integer;
FUNCTION dos_getdt(handle : Integer; VAR date, time : Integer) : Integer;

implementation

{ *** C/Pascal Strings *** }

PROCEDURE MakeCString(VAR pString, cString);
  (* make a Pascal style string into a C style string *)

  VAR
    ps : MaxString ABSOLUTE pString;
    cs : MaxString ABSOLUTE cString;
    index, len : INTEGER;

  BEGIN        (* MakeCString *)
    IF (POS(CHR(0),ps) = 0) THEN   (* check that it isn't already a cstring *)
      BEGIN
        len := LENGTH(ps);

        IF (len > 0) THEN
          FOR index := 1 TO len DO
            cs[index-1] := ps[index];

        cs[len] := CHR(0);
      END;
  END;        (* MakeCString *)



PROCEDURE MakePString(VAR cString, pString);
  (* make a C style string into a Pascal style string *)

  VAR
    ps : MaxString ABSOLUTE pString;
    cs : MaxString ABSOLUTE cString;
    index : INTEGER;

  BEGIN        (* MakePString *)
    IF (POS(CHR(0),cs)<>0) THEN      (* check that cs is a cstring *)
      BEGIN
        index := 0;

        WHILE (cs[index] <> CHR(0)) DO
          BEGIN
            ps[index+1] := cs[index];
            index := index+1;
          END;   (* WHILE *)

        ps[0] := CHR(index);
      END;
  END; (* MakePString *)

{ AES, here we come... }

PROCEDURE GAES(VAR a_addrout : aad_ARRAY;
                 VAR a_addrin : aad_ARRAY;
                 VAR aintout : ao_ARRAY;
                 VAR aintin : ai_ARRAY;
                 VAR aglobal : ag_ARRAY;
                 VAR acontrol : ac_ARRAY);

    CONST
      AESinterruptVector = $EF;
      AESmagicConstant = 200;

    VAR
      parameterBlock : RECORD
                         controlArray : ADDRESS;
                         globalArray : ADDRESS;
                         inputParameterArray : ADDRESS;
                         outputParameterArray : ADDRESS;
                         inputAddressArray : ADDRESS;
                         outputAddressArray : ADDRESS;
                       END (* RECORD *) ;
      { registers : RECORD

                    ax, bx, cx, dx, bp, si, di, ds, es, flags : INTEGER;

                  END ; }

      _registers : registers;

      ltemp : LONG;

    BEGIN
        (* GAES *)

      parameterBlock.controlArray := ADDR(acontrol);
      parameterBlock.globalArray := ADDR(aglobal);
      parameterBlock.inputParameterArray := ADDR(aintin);
      parameterBlock.outputParameterArray := ADDR(aintout);
      parameterBlock.inputAddressArray := ADDR(a_addrin);
      parameterBlock.outputAddressArray := ADDR(a_addrout);

      _registers.cx := AESmagicConstant;
      ltemp.ad := ADDR(parameterBlock);
      _registers.es := ltemp.s;
      _registers.bx := ltemp.o;

      INTR(AESinterruptvector, _registers);

    END (* GAES *) ;


(*---------------------------------------------------------------------*)
(** Common call to the GEM AES routines - used by all other functions *)

  FUNCTION GEMAES(opcode, c1, c2, c3, c4 : INTEGER) : INTEGER;
    BEGIN
      gcontrol[0] := opcode;
      gcontrol[1] := c1;
          (* number of params in gintin *)
      gcontrol[2] := c2;
          (* number of params in gintout *)
      gcontrol[3] := c3;
          (* number of params in addrin *)
      gcontrol[4] := c4;
          (* number of params in addrout *)
      GAES(addr_out, addr_in, gintout, gintin, global, gcontrol);
      GEMAES := gintout[0];
          (* return value if any *)
    END;

(***********************************************************************)
(*** APPLICATION LIBRARY ***)

(*---------------------------------------------------------------------*)

  FUNCTION appl_init : INTEGER;

    VAR ap_id : INTEGER;
    BEGIN
      ap_id := GEMAES(10, 0, 1, 0, 0);
      global[2] := ap_id;
      appl_init := ap_id;
    END;

(*---------------------------------------------------------------------*)

  FUNCTION appl_exit : INTEGER;
    BEGIN
      appl_exit := GEMAES(19, 0, 1, 0, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION appl_write(ap_wid, ap_wlength : INTEGER;
                      ap_wpbuff : ADDRESS) : INTEGER;
    BEGIN
      gintin[0] := ap_wid;
      gintin[1] := ap_wlength;
      addr_in[0] := ap_wpbuff;
      appl_write := GEMAES(12, 2, 1, 1, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION appl_read(ap_rid, ap_rlength : INTEGER;
                     ap_rpbuff : ADDRESS) : INTEGER;
    BEGIN
      gintin[0] := ap_rid;
      gintin[1] := ap_rlength;
      addr_in[0] := ap_rpbuff;
      appl_read := GEMAES(11, 2, 1, 1, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION appl_find(VAR ap_fpname) : INTEGER;
    (* NOTE ap_fpname is limited to 8 char, padded with trailing blanks *)
    VAR
      p_name : string[8] ABSOLUTE ap_fpname;
      c_name : string[8];
    BEGIN
      MakeCstring(p_name,c_name);
      addr_in[0] := ADDR(c_name);
      appl_find := GEMAES(13, 0, 1, 1, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION appl_tplay(ap_tpmem : ADDRESS;
                      ap_tpnum, ap_tpscale : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := ap_tpnum;
      gintin[1] := ap_tpscale;
      addr_in[0] := ap_tpmem;
      appl_tplay := GEMAES(14, 2, 1, 1, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION appl_trecord(ap_trmem : ADDRESS;
                        ap_trcount : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := ap_trcount;
      addr_in[0] := ap_trmem;
      appl_trecord := GEMAES(15, 1, 1, 1, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION appl_bvset(bvdisk : INTEGER;
                      bvhard : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := bvdisk;
      gintin[1] := bvhard;
      appl_bvset := GEMAES(16, 2, 1, 0, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION appl_yield : INTEGER;
    BEGIN
      appl_yield := GEMAES(17, 0, 1, 0, 0);
    END;

(***********************************************************************)
(** EVENT LIBRARY **)

(*---------------------------------------------------------------------*)

  FUNCTION evnt_keybd : INTEGER;
    BEGIN
      evnt_keybd := GEMAES(20, 0, 1, 0, 0);
    END;
(*---------------------------------------------------------------------*)

  FUNCTION evnt_button(ev_bclicks, ev_bmask, ev_bstate : INTEGER;
                       VAR ev_bmx, ev_bmy, ev_bmb, ev_bks : INTEGER) : INTEGER
                           ;
    BEGIN
      gintin[0] := ev_bclicks;
      gintin[1] := ev_bmask;
      gintin[2] := ev_bstate;
      evnt_button := GEMAES(21, 3, 5, 0, 0);
      ev_bmx := gintout[1];
      ev_bmy := gintout[2];
      ev_bmb := gintout[3];
      ev_bks := gintout[4];
    END;

(*---------------------------------------------------------------------*)

  FUNCTION evnt_mouse(ev_moflags, ev_mox, ev_moy, ev_mowidth,
                      ev_moheight : INTEGER;
                      VAR ev_momx, ev_momy, ev_momb, ev_momks : INTEGER) :
      INTEGER;
    BEGIN
      gintin[0] := ev_moflags;
      gintin[1] := ev_mox;
      gintin[2] := ev_moy;
      gintin[3] := ev_mowidth;
      gintin[4] := ev_moheight;
      evnt_mouse := GEMAES(22, 5, 5, 0, 0);
      ev_momx := gintout[1];
      ev_momy := gintout[2];
      ev_momb := gintout[3];
      ev_momks := gintout[4];
    END;

(*---------------------------------------------------------------------*)

  FUNCTION evnt_mesag(ev_mgpbuff : ADDRESS) : INTEGER;
    BEGIN
      addr_in[0] := ev_mgpbuff;
      evnt_mesag := GEMAES(23, 0, 1, 1, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION evnt_timer(ev_tlocount, ev_thicount : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := ev_tlocount;
      gintin[1] := ev_thicount;
      evnt_timer := GEMAES(24, 2, 1, 0, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION evnt_multi(ev_mflags, ev_mbclicks, ev_mbmask, ev_mbstate,
                      ev_mm1flags, ev_mm1x, ev_mm1y, ev_mm1width, ev_m1height,
                      ev_mm2flags, ev_mm2x, ev_mm2y, ev_mm2width, ev_mm2height
                      : INTEGER;
                      ev_mmgpbuff : ADDRESS;
                      ev_mtlocount, ev_mthicount : INTEGER;

                      VAR ev_mmox, ev_mmoy, ev_mmobutton, ev_mmokstate,
                      ev_mkreturn, ev_mbreturn : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := ev_mflags;
      gintin[1] := ev_mbclicks;
      gintin[2] := ev_mbmask;
      gintin[3] := ev_mbstate;
      gintin[4] := ev_mm1flags;
      gintin[5] := ev_mm1x;
      gintin[6] := ev_mm1y;
      gintin[7] := ev_mm1width;
      gintin[8] := ev_m1height;
      gintin[9] := ev_mm2flags;
      gintin[10] := ev_mm2x;
      gintin[11] := ev_mm2y;
      gintin[12] := ev_mm2width;
      gintin[13] := ev_mm2height;
      addr_in[0] := ev_mmgpbuff;
      gintin[14] := ev_mtlocount;
      gintin[15] := ev_mthicount;
      EVNT_MULTI := GEMAES(25, 16, 7, 1, 0);
      ev_mmox := gintout[1];
      ev_mmoy := gintout[2];
      ev_mmobutton := gintout[3];
      ev_mmokstate := gintout[4];
      ev_mkreturn := gintout[5];
      ev_mbreturn := gintout[6];
    END;

(*----------------------------------------------------------------------*)

  FUNCTION evnt_dclick(ev_dnew, ev_dgtest : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := ev_dnew;
      gintin[1] := ev_dgtest;
      evnt_dclick := GEMAES(26, 2, 1, 0, 0);
    END;

(************************************************************************)
(** MENU LIBRARY **)

(*---------------------------------------------------------------------*)

  FUNCTION menu_bar(me_btree : ADDRESS;
                    me_bshow : INTEGER) : INTEGER;
    BEGIN
      addr_in[0] := me_btree;
      gintin[0] := me_bshow;
      menu_bar := GEMAES(30, 1, 1, 1, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION menu_icheck(me_ctree : ADDRESS;
                       me_citem, me_ccheck : INTEGER) : INTEGER;
    BEGIN
      addr_in[0] := me_ctree;
      gintin[0] := me_citem;
      gintin[1] := me_ccheck;
      menu_icheck := GEMAES(31, 2, 1, 1, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION menu_ienable(me_etree : ADDRESS;
                        me_eitem, me_eenable : INTEGER) : INTEGER;
    BEGIN
      addr_in[0] := me_etree;
      gintin[0] := me_eitem;
      gintin[1] := me_eenable;
      menu_ienable := GEMAES(32, 2, 1, 1, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION menu_tnormal(me_ntree : ADDRESS;
                        me_ntitle, me_nnormal : INTEGER) : INTEGER;
    BEGIN
      addr_in[0] := me_ntree;
      gintin[0] := me_ntitle;
      gintin[1] := me_nnormal;
      menu_tnormal := GEMAES(33, 2, 1, 1, 0);
    END;


(*---------------------------------------------------------------------*)

  FUNCTION menu_text(me_ttree : ADDRESS;
                     me_titem : INTEGER;
                     VAR me_ttext) : INTEGER;

    VAR pstrg : MaxString ABSOLUTE me_ttext;
        cstrg : MaxString;

    BEGIN
      addr_in[0] := me_ttree;
      gintin[0] := me_titem;
      MakeCstring(pstrg,cstrg);
      addr_in[1] := ADDR(cstrg);
      menu_text := GEMAES(34, 1, 1, 2, 0);
    END;

(*----------------------------------------------------------------------*)

  FUNCTION menu_register(me_raccapid : INTEGER;
                         VAR me_rpstring) : INTEGER;

    VAR pstrg : MaxString ABSOLUTE me_rpstring;
        cstrg : MaxString;

    BEGIN
      gintin[0] := me_raccapid;
      MakeCstring(pstrg,cstrg);
      addr_in[0] := ADDR(cstrg);
      menu_register := GEMAES(35, 1, 1, 1, 0);
    END;

(*----------------------------------------------------------------------*)

  FUNCTION menu_unregister(mu_mid : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := mu_mid;
      menu_unregister := GEMAES(36, 1, 1, 0, 0);
    END;

(**********************************************************************)
(** FORM LIBRARY **)

(*---------------------------------------------------------------------*)

  FUNCTION form_do(fo_dotree : ADDRESS;
                   fo_dostartob : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := fo_dostartob;
      addr_in[0] := fo_dotree;
      form_do := GEMAES(50, 1, 1, 1, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION form_dial(fo_diflag, fo_dilx, fo_dily, fo_dilw,
                     fo_dilh, fo_dibigx, fo_dibigy,
                     fo_dibigw, fo_dibigh : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := fo_diflag;
      gintin[1] := fo_dilx;
      gintin[2] := fo_dily;
      gintin[3] := fo_dilw;
      gintin[4] := fo_dilh;
      gintin[5] := fo_dibigx;
      gintin[6] := fo_dibigy;
      gintin[7] := fo_dibigw;
      gintin[8] := fo_dibigh;
      form_dial := GEMAES(51, 9, 1, 0, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION form_alert(fo_adefbttn : INTEGER;
                      VAR fo_aastring) : INTEGER;
    VAR pstrg : MaxString ABSOLUTE fo_aastring;
        cstrg : MaxString;

    BEGIN
      MakeCstring(pstrg,cstrg);
      addr_in[0] := ADDR(cstrg);
      gintin[0] := fo_adefbttn;
      form_alert := GEMAES(52, 1, 1, 1, 0);
    END;

(*-------------------------------------------------------------------*)

  FUNCTION form_error(fo_enum : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := fo_enum;
      form_error := GEMAES(53, 1, 1, 0, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION form_center(fo_ctree : ADDRESS;
                       VAR fo_cx, fo_cy, fo_cw, fo_ch : INTEGER) : INTEGER;
    BEGIN
      addr_in[0] := fo_ctree;
      form_center := GEMAES(54, 0, 5, 1, 0);
      fo_cx := gintout[1];
      fo_cy := gintout[2];
      fo_cw := gintout[3];
      fo_ch := gintout[4];
    END;

(*---------------------------------------------------------------------*)

  FUNCTION form_keybd(fo_key : ADDRESS;
                      fo_obj, fo_nxt, fo_thechar : INTEGER;
                      VAR fo_pnxt, fo_pchar : INTEGER) : INTEGER;
    BEGIN
      addr_in[0] := fo_key;
      gintin[0] := fo_obj;
      gintin[2] := fo_nxt;
      gintin[1] := fo_thechar;
      form_keybd := GEMAES(55, 3, 3, 1, 0);
      fo_pnxt := gintout[1];
      fo_pchar := gintout[2];
    END;

(*---------------------------------------------------------------------*)

  FUNCTION form_button(fo_but : ADDRESS;
                       fo_obj, fo_clks : INTEGER;
                       VAR fo_pnxt : INTEGER) : INTEGER;
    BEGIN
      addr_in[0] := fo_but;
      gintin[0] := fo_obj;
      gintin[1] := fo_clks;
      form_button := GEMAES(56, 2, 2, 1, 0);
      fo_pnxt := gintout[1];
    END;

(**********************************************************************)
(** GRAPHICS LIBRARY **)

(*--------------------------------------------------------------------*)

  FUNCTION graf_rubbox(gr_rx, gr_ry, gr_rminwidth, gr_rminheight : INTEGER;
                       VAR gr_rlastwidth, gr_rlastheight : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := gr_rx;
      gintin[1] := gr_ry;
      gintin[2] := gr_rminwidth;
      gintin[3] := gr_rminheight;
      graf_rubbox := GEMAES(70, 4, 3, 0, 0);
      gr_rlastwidth := gintout[1];
      gr_rlastheight := gintout[2];
    END;

(*-------------------------------------------------------------------*)

  FUNCTION graf_dragbox(gr_dwidth, gr_dheight, gr_dstx, gr_dsty,
                        gr_dbndx, gr_dbndy, gr_dbndw, gr_dbndh : INTEGER;

                        VAR gr_dfinx, gr_dfiny : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := gr_dwidth;
      gintin[1] := gr_dheight;
      gintin[2] := gr_dstx;
      gintin[3] := gr_dsty;
      gintin[4] := gr_dbndx;
      gintin[5] := gr_dbndy;
      gintin[6] := gr_dbndw;
      gintin[7] := gr_dbndh;
      graf_dragbox := GEMAES(71, 8, 3, 0, 0);
      gr_dfinx := gintout[1];
      gr_dfiny := gintout[2];
    END;

(*--------------------------------------------------------------------*)
    { formerly: graf_movebox }
  FUNCTION graf_mbox(gr_mwidth, gr_mheight, gr_msrcex, gr_msrcey,
                     gr_mdestx, gr_mdesty : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := gr_mwidth;
      gintin[1] := gr_mheight;
      gintin[2] := gr_msrcex;
      gintin[3] := gr_msrcey;
      gintin[4] := gr_mdestx;
      gintin[5] := gr_mdesty;
      graf_mbox := GEMAES(72, 6, 1, 0, 0);
    END;

(*--------------------------------------------------------------------*)

  FUNCTION graf_watchbox(gr_wptree : ADDRESS;
                         gr_wobject, gr_winstate, gr_woutstate : INTEGER) :
      INTEGER;
    BEGIN
      gintin[1] := gr_wobject;
      gintin[2] := gr_winstate;
      gintin[3] := gr_woutstate;
      addr_in[0] := gr_wptree;
      graf_watchbox := GEMAES(75, 4, 1, 1, 0);
    END;

(*-------------------------------------------------------------------*)

  FUNCTION graf_slidebox(gr_slptree : ADDRESS;
                         gr_slparent, gr_slobject, gr_slvh : INTEGER) : 
                             INTEGER;
    BEGIN
      gintin[0] := gr_slparent;
      gintin[1] := gr_slobject;
      gintin[2] := gr_slvh;
      graf_slidebox := GEMAES(76, 3, 1, 1, 0);
    END;

(*-------------------------------------------------------------------*)

  FUNCTION graf_handle(VAR gr_hwchar, gr_hhchar, gr_hwbox,
                       gr_hhbox : INTEGER) : INTEGER;
    BEGIN
      graf_handle := GEMAES(77, 0, 5, 0, 0);
      gr_hwchar := gintout[1];
      gr_hhchar := gintout[2];
      gr_hwbox := gintout[3];
      gr_hhbox := gintout[4];
    END;

(*------------------------------------------------------------------*)

  FUNCTION graf_mouse(gr_monumber : INTEGER;
                      gr_mofaddr : ADDRESS) : INTEGER;
    BEGIN
      gintin[0] := gr_monumber;
      addr_in[0] := gr_mofaddr;
      graf_mouse := GEMAES(78, 1, 1, 1, 0);
    END;

(*------------------------------------------------------------------*)

  FUNCTION graf_mkstate(VAR gr_mkmx, gr_mkmy,
                        gr_mkmstate, gr_mkkstate : INTEGER) : INTEGER;
    BEGIN
      graf_mkstate := GEMAES(79, 0, 5, 0, 0);
      gr_mkmx := gintout[1];
      gr_mkmy := gintout[2];
      gr_mkmstate := gintout[3];
      gr_mkkstate := gintout[4];
    END;

(*********************************************************************)
(** SCRAP LIBRARY **)

(*---------------------------------------------------------------------*)

  FUNCTION scrp_read(VAR sc_rpath) : INTEGER;
    VAR
      p_path : MaxString ABSOLUTE sc_rpath;
      c_path : MaxString;
    BEGIN
      MAkeCstring(p_path,c_path);
      addr_in[0] := ADDR(c_path);
      scrp_read := GEMAES(80, 0, 1, 1, 0);
      MakePstring(c_path,p_path);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION scrp_write(VAR sc_pscrap ) : INTEGER;
    VAR
      p_path : MaxString ABSOLUTE sc_pscrap;
      c_path : MaxString;
    BEGIN
      MAkeCstring(p_path,c_path);
      addr_in[0] := ADDR(c_path);
      scrp_write := GEMAES(81, 0, 1, 1, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION scrp_clear : INTEGER;
    BEGIN
      scrp_clear := GEMAES(82, 0, 1, 0, 0);
    END;

(************************************************************************)
(** WINDOW LIBRARY **)

(*---------------------------------------------------------------------*)

  FUNCTION wind_create(wi_crkind, wi_crwx, wi_crwy,
                       wi_crww, wi_crwh : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := wi_crkind;
      gintin[1] := wi_crwx;
      gintin[2] := wi_crwy;
      gintin[3] := wi_crww;
      gintin[4] := wi_crwh;
      wind_create := GEMAES(100, 5, 1, 0, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION wind_open(wi_ohandle, wi_owx, wi_owy,
                     wi_oww, wi_owh : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := wi_ohandle;
      gintin[1] := wi_owx;
      gintin[2] := wi_owy;
      gintin[3] := wi_oww;
      gintin[4] := wi_owh;
      wind_open := GEMAES(101, 5, 1, 0, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION wind_close(wi_clhandle : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := wi_clhandle;
      wind_close := GEMAES(102, 1, 1, 0, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION wind_delete(wi_dhandle : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := wi_dhandle;
      wind_delete := GEMAES(103, 1, 1, 0, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION wind_get(wi_ghandle, wi_gfield : INTEGER;
                    VAR wi_gw1, wi_gw2, wi_gw3, wi_gw4 : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := wi_ghandle;
      gintin[1] := wi_gfield;
      wind_get := GEMAES(104, 2, 5, 0, 0);
      wi_gw1 := gintout[1];
      wi_gw2 := gintout[2];
      wi_gw3 := gintout[3];
      wi_gw4 := gintout[4];
    END;

(*---------------------------------------------------------------------*)

  FUNCTION wind_set(wi_shandle, wi_sfield,
                    wi_sw1, wi_sw2, wi_sw3, wi_sw4 : INTEGER) : INTEGER;
    VAR
      pstrng, cstrng : CharString;
      ltemp : LONG;
    BEGIN
      gintin[0] := wi_shandle;
      gintin[1] := wi_sfield;
      IF (wi_sfield in [2,3]) THEN       (* setting window name or info field*)
        BEGIN
          ltemp.o := wi_sw1;             (* wi_sw1 = offset of string *)
          ltemp.s := wi_sw2;             (* wi_sw2 = segment of string *)
          pstrng := ltemp.sp^;           (* ltemp.sp points to pstring *)
          MakeCstring(pstrng,cstrng);    (* convert pstring to cstring *)
          ltemp.ad := ADDR(cstrng);
          gintin[2] := ltemp.o;
          gintin[3] := ltemp.s;
        END
      ELSE
        BEGIN
          gintin[2] := wi_sw1;
          gintin[3] := wi_sw2;
        END;
      gintin[4] := wi_sw3;
      gintin[5] := wi_sw4;
      wind_set := GEMAES(105, 6, 1, 0, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION wind_find(wi_fmx, wi_fmy : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := wi_fmx;
      gintin[1] := wi_fmy;
      wind_find := GEMAES(106, 2, 1, 0, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION wind_update(wi_ubegend : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := wi_ubegend;
      wind_update := GEMAES(107, 1, 1, 0, 0);
    END;


(*---------------------------------------------------------------------*)

  FUNCTION wind_calc(wi_ctype, wi_ckind,
                     wi_cx, wi_cy, wi_cinwidth, wi_cinheight : INTEGER;
                     VAR wi_coutx, wi_couty, wi_coutwidth, wi_coutheight :
                     INTEGER) :
      INTEGER;
    BEGIN
      gintin[0] := wi_ctype;
      gintin[1] := wi_ckind;
      gintin[2] := wi_cx;
      gintin[3] := wi_cy;
      gintin[4] := wi_cinwidth;
      gintin[5] := wi_cinheight;
      wind_calc := GEMAES(108, 6, 5, 0, 0);
      wi_coutx := gintout[1];
      wi_couty := gintout[2];
      wi_coutwidth := gintout[3];
      wi_coutheight := gintout[4];
    END;

(************************************************************************)
(** RESOURCE LIBRARY **)

(*---------------------------------------------------------------------*)

  FUNCTION rsrc_load(VAR re_lpfname) : INTEGER;
    VAR
      p_name : MaxString ABSOLUTE re_lpfname;
      c_name : MaxString;
    BEGIN
      MAkeCstring(p_name,c_name);
      addr_in[0] := ADDR(c_name);
      rsrc_load := GEMAES(110, 0, 1, 1, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION rsrc_free : INTEGER;
    BEGIN
      rsrc_free := GEMAES(111, 0, 1, 0, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION rsrc_gaddr(re_gttype, re_gtindex : INTEGER;
                      VAR re_gtaddr : ADDRESS) : INTEGER;
    BEGIN
      gintin[0] := re_gttype;
      gintin[1] := re_gtindex;
      rsrc_gaddr := GEMAES(112, 2, 1, 0, 1);
      re_gtaddr := addr_out[0];
    END;

(*---------------------------------------------------------------------*)

  FUNCTION rsrc_saddr(re_sttype, re_stindex : INTEGER;
                      re_staddr : ADDRESS) : INTEGER;
    BEGIN
      gintin[0] := re_sttype;
      gintin[1] := re_stindex;
      addr_in[0] := re_staddr;
      rsrc_saddr := GEMAES(113, 2, 1, 1, 0);
    END;

(*----------------------------------------------------------------------*)

  FUNCTION rsrc_obfix(re_otree : ADDRESS;
                      re_oobject : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := re_oobject;
      addr_in[0] := re_otree;
      rsrc_obfix := GEMAES(114, 1, 1, 1, 0);
    END;

(***********************************************************************)
(** SHELL LIBRARY **)

(*---------------------------------------------------------------------*)

  FUNCTION shel_read(VAR sh_rpcmd, sh_rptail) : INTEGER;
    VAR
      p_cmd: MaxString ABSOLUTE sh_rpcmd;
      p_tail: MaxString ABSOLUTE sh_rptail;
      c_cmd, c_tail : MaxString;
    BEGIN
      MAkeCstring(p_cmd,c_cmd);
      MAkeCstring(p_tail,c_tail);
      addr_in[0] := ADDR(c_cmd);
      addr_in[1] := ADDR(c_tail);
      shel_read := GEMAES(120, 0, 1, 2, 0);
    END;


(*---------------------------------------------------------------------*)

  FUNCTION shel_write(sh_wdoex, sh_wisgr, sh_wiscr : INTEGER;
                      VAR sh_wpcmd, sh_wptail) : INTEGER;
    VAR
      p_cmd: MaxString ABSOLUTE sh_wpcmd;
      p_tail: MaxString ABSOLUTE sh_wptail;
      c_cmd, c_tail : MaxString;
    BEGIN
      MAkeCstring(p_cmd,c_cmd);
      MAkeCstring(p_tail,c_tail);
      gintin[0] := sh_wdoex;
      gintin[1] := sh_wisgr;
      gintin[2] := sh_wiscr;
      addr_in[0] := ADDR(c_cmd);
      addr_in[1] := ADDR(c_tail);
      shel_write := GEMAES(121, 3, 1, 2, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION shel_find(VAR sh_ppath ) : INTEGER;
    VAR
      p_path : MaxString ABSOLUTE sh_ppath;
      c_path : MaxString;
    BEGIN
      MAkeCstring(p_path,c_path);
      addr_in[0] := ADDR(c_path);
      shel_find := GEMAES(124, 0, 1, 1, 0);
      MakePstring(c_path,p_path);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION shel_envrn(sh_ppath : ADDRESS; VAR sh_psrch) : INTEGER;
    VAR
      p_srch : MaxString ABSOLUTE sh_psrch;
      c_srch : MaxString;
    BEGIN
      addr_in[0] := sh_ppath;
      MakeCstring(p_srch,c_srch);
      addr_in[1] := ADDR(c_srch);
      shel_envrn := GEMAES(125, 0, 1, 2, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION shel_rdef(VAR sh_lpcmd, sh_lpdir) : INTEGER;
    VAR
      p_cmd: MaxString ABSOLUTE sh_lpcmd;
      p_dir: MaxString ABSOLUTE sh_lpdir;
      c_cmd, c_dir : MaxString;
    BEGIN
      MAkeCstring(p_cmd,c_cmd);
      MAkeCstring(p_dir,c_dir);
      addr_in[0] := ADDR(c_cmd);
      addr_in[1] := ADDR(c_dir);
      shel_rdef := GEMAES(126, 0, 1, 2, 0);
      MakePstring(c_cmd,p_cmd);
      MakePstring(c_dir,p_dir);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION shel_wdef(VAR sh_lpcmd, sh_lpdir) : INTEGER;
    VAR
      p_cmd: MaxString ABSOLUTE sh_lpcmd;
      p_dir: MaxString ABSOLUTE sh_lpdir;
      c_cmd, c_dir : MaxString;
    BEGIN
      MAkeCstring(p_cmd,c_cmd);
      MAkeCstring(p_dir,c_dir);
      addr_in[0] := ADDR(c_cmd);
      addr_in[1] := ADDR(c_dir);
      shel_wdef := GEMAES(127, 0, 0, 2, 0);
    END;

(*---------------------------------------------------------------------*)

  FUNCTION xgrf_stepcalc(xg_orgw, xg_orgh, xg_xc, xg_yc, xg_w, xg_h : INTEGER;
                         VAR xg_pcx, xg_pcy, xg_pcnt,
                         xg_pxstep, xg_pystep : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := xg_orgw;
      gintin[1] := xg_orgh;
      gintin[2] := xg_xc;
      gintin[3] := xg_yc;
      gintin[4] := xg_w;
      gintin[5] := xg_h;
      xgrf_stepcalc := GEMAES(130, 6, 6, 0, 0);
      xg_pcx := gintout[1];
      xg_pcy := gintout[2];
      xg_pcnt := gintout[3];
      xg_pxstep := gintout[4];
      xg_pystep := gintout[5];
    END;

(*---------------------------------------------------------------------*)

  FUNCTION xgrf_2box(xg_xc, xg_yc, xg_w, xg_h,
                     xg_corners, xg_cnt, xg_xstep,
                     xg_ystep, xg_doubled : INTEGER) : INTEGER;
    BEGIN
      gintin[0] := xg_cnt;
      gintin[1] := xg_xstep;
      gintin[2] := xg_ystep;
      gintin[3] := xg_doubled;
      gintin[4] := xg_corners;
      gintin[5] := xg_xc;
      gintin[6] := xg_yc;
      gintin[7] := xg_w;
      gintin[8] := xg_h;
      xgrf_2box := GEMAES(131, 9, 1, 0, 0);
    END;

(*-------------------------------------------------------------------*)

  FUNCTION graf_growbox(gr_gstx, gr_gsty, gr_gstwidth, gr_gstheight,
                        gr_gfinx, gr_gfiny, gr_gfinwidth, gr_gfinheight :
                        INTEGER) : INTEGER;
    BEGIN
    END;

(*---------------------------------------------------------------------*)

  FUNCTION graf_shrinkbox(gr_sstx, gr_ssty, gr_sstwidth, gr_sstheight,
                          gr_sfinx, gr_sfiny, gr_sfinwidth, gr_sfinheight :
                          INTEGER) : INTEGER;
    BEGIN
    END;

{ end of AESBIND.PAS ... next the DEBUG! :¬) }

PROCEDURE Send_msg (VAR instrg);
  VAR
    msg : CharString ABSOLUTE instrg;
    pstrg : CharString;
    Status  : Integer;
  BEGIN
    pstrg:='[3]['+msg+'][Continue]';
    status:=form_alert(1,pstrg);
  END;

PROCEDURE Tell_Addr (VAR instrg; anyPtr  : ADDRESS);
  VAR
    msg : CharString ABSOLUTE instrg;
    nstrg : CharString;
    ltemp : LONG;
  BEGIN
    ltemp.ad := anyPTR;
    Str(ltemp.o,Nstrg);
    msg:= msg+'|ofs= '+Nstrg;
    str(ltemp.s,Nstrg);
    msg:=msg+'|seg= '+nstrg;
    Send_msg(msg);
  END;

PROCEDURE Tell_Byte (VAR instrg, inPtr);
  VAR
    Ltemp : Long ABSOLUTE inPtr;
    msg : CharString ABSOLUTE instrg;
    nstrg : CharString;
  BEGIN
    Str(ltemp.b0,Nstrg);
    msg:= msg+'|byte 0 = '+Nstrg;
    Str(ltemp.b1,Nstrg);
    msg:= msg+'|byte 1 = '+Nstrg;
    Str(ltemp.b2,Nstrg);
    msg:= msg+'|byte 2 = '+Nstrg;
    Str(ltemp.b3,Nstrg);
    msg:= msg+'|byte 3 = '+Nstrg;
    Send_msg(msg);
  END;

PROCEDURE Tell_Int (VAR instrg; anyInt : Integer);
  VAR
    msg : CharString ABSOLUTE instrg;
    nstrg : CharString;
  BEGIN
    Str(anyInt,nstrg);
    msg:= msg+nstrg;
    Send_msg(msg);
  END;

PROCEDURE Tell_Real (VAR instrg; anyReal : Real);
  VAR
    msg : CharString ABSOLUTE instrg;
    nstrg : CharString;
  BEGIN
    Str(anyReal,nstrg);
    msg:= msg+nstrg;
    Send_msg(msg);
  END;

PROCEDURE Tell_Mfdb (VAR instrg; anyMFDB : MFDB);
  VAR
    title : CharString ABSOLUTE instrg;
    nstrg, msg : CharString;
  BEGIN
    msg:=title+'|MFDB points to';
    Tell_Addr(msg,anyMFDB.mp);
    Str(anyMfdb.fwp,nstrg);
    msg:=title+'|MFDB dimensions are|'+nstrg+' pixels by ';
    Str(anyMfdb.fh,nstrg);
    msg := msg+nstrg+' lines';
    Send_msg(msg);
    msg:=title+'|MFDB width in words is ';
    Tell_Int(msg,anyMFDB.fww);
    msg:=title+'|MFDB format flag is ';
    Tell_Int(msg,anyMFDB.ff);
    msg:=title+'|MFDB memory planes are ';
    Tell_Int(msg,anyMFDB.np);
  END;

PROCEDURE Tell_Grect (VAR instrg, inGrect);
  VAR
    title : CharString ABSOLUTE instrg;
    anyGrect : ARRAY_4 ABSOLUTE inGrect;
    status : Integer;
    nstrg,msg : CharString;
  BEGIN
    Str(anyGrect[0],nstrg);
    msg := title+'| x coord = '+nstrg;
    Str(anyGrect[1],nstrg);
    msg := msg+'| y coord = '+nstrg;
    Str(anyGrect[2],nstrg);
    msg := msg+'| width   = '+nstrg;
    Str(anyGrect[3],nstrg);
    msg := msg+'| height  = '+nstrg;
    Send_Msg(msg);
  END;

PROCEDURE Tell_Contrl (VAR instrg; anyContrl : Contrl_ARRAY);
  VAR
    title : CharString ABSOLUTE instrg;
    k, status : Integer;
    nstrg, msg  : CharString;
  BEGIN
    FOR k := 0 TO cntl_Max DO
      BEGIN
        Str(k,nstrg);
        msg:=title+'|Contrl('+nstrg+') = ';
        Tell_Int(msg,anyContrl[k]);
      END;
  END;

PROCEDURE KeyCont;
  BEGIN
    WriteLn;
    WriteLn(' hit ENTER key to continue ');
    Readln;
  END;

{ OK, here come the biggies: VDI bindings :) }

    (*****************************************************************)

  PROCEDURE GVDI(VAR gptsout : ptsout_ARRAY;
                 VAR gintout : intout_ARRAY;
                 VAR gptsin : ptsin_ARRAY;
                 VAR gintin : intin_ARRAY;
                 VAR gcontrl : contrl_ARRAY);

    CONST
      VDIinterruptVector = $EF;
      VDImagicConstant = $0473;

    TYPE
      ADDRESS = ^BYTE;

    VAR
      parameterBlock : RECORD
                         controlArray : ADDRESS;
                         inputParameterArray : ADDRESS;
                         inputPointCoordinateArray : ADDRESS;
                         outputParameterArray : ADDRESS;
                         outputPointCoordinateArray : ADDRESS;
                       END (* RECORD *) ;

      { registers : RECORD
                    ax, bx, cx, dx, bp, si, di, ds, es, flags : INTEGER;
                  END ; }

      _registers : registers;

      ltemp : LONG;

    BEGIN
        (* GVDI *)

      parameterBlock.controlArray := ADDR(gcontrl);
      parameterBlock.inputParameterArray := ADDR(gintin);
      parameterBlock.inputPointCoordinateArray := ADDR(gptsin);
      parameterBlock.outputParameterArray := ADDR(gintout);
      parameterBlock.outputPointCoordinateArray := ADDR(gptsout);

      _registers.cx := VDImagicConstant;
      ltemp.ad := ADDR(parameterBlock);
      _registers.ds := ltemp.s;
      _registers.dx := ltemp.o;

      INTR(VDIinterruptvector, _registers);

    END (* GVDI *) ;

    (**************************************************************)
    (* general (and only) call to GEM VDI *)

  FUNCTION gemvdif(opcode, handle : INTEGER) : INTEGER;
    BEGIN
      contrl[0] := opcode;
      contrl[6] := handle;
      (* in gempcall - gdos interrupt*)
      GVDI(ptsout, intout, ptsin, intin, contrl);
      gemvdif := intout[0];
    END;

    (********************************************************************)
    (*** CONTROL FUNCTIONS ***)
    (*****************************************************************)
    (** open workstation **)

  FUNCTION v_opnwk(workin : ARRAY_11;
                   VAR handle : INTEGER;
                   VAR workout : ARRAY_57) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 11;
      FOR i := 0 TO contrl[3]-1 DO
        intin[i] := workin[i];
      v_opnwk := gemvdif(1, handle);
          (* opcode = 1 *)
      handle := contrl[6];
      FOR i := 0 TO 44 DO
        workout[i] := intout[i];
      FOR i := 0 TO 11 DO
        workout[i+45] := ptsout[i];
    END;

    (****************************************************************)
    (** close workstation **)

  FUNCTION v_clswk(handle : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      v_clswk := gemvdif(2, handle);
    END;

    (*****************************************************************)
    (** clear workstation **)

  FUNCTION v_clrwk(handle : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      v_clrwk := gemvdif(3, handle);
    END;

    (******************************************************************)
    (** update workstation **)

  FUNCTION v_updwk(handle : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      v_updwk := gemvdif(4, handle);
    END;

    (*****************************************************************)
    (** open virtual workstation **)

  FUNCTION v_opnvwk(workin : ARRAY_11;
                    VAR handle : INTEGER;
                    VAR workout : ARRAY_57) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 0;
          (* no of input vertices *)
      contrl[3] := 11;
          (* length of intin *)
      FOR i := 0 TO contrl[3]-1 DO
        intin[i] := workin[i];
      v_opnvwk := gemvdif(100, handle);
      (* handle from previously opened screen device *)
      handle := contrl[6];
      FOR i := 0 TO 44 DO
        workout[i] := intout[i];
      FOR i := 0 TO 11 DO
        workout[i+45] := ptsout[i];
    END;

    (****************************************************************)
    (** close virtual workstation **)

  FUNCTION v_clsvwk(handle : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      v_clsvwk := gemvdif(101, handle);
    END;

    (****************************************************************)
    (** Load extra fonts into memory - caller must free some memory space **)

  FUNCTION vst_load_fonts(handle, select : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      intin[0] := select;
      vst_load_fonts := gemvdif(119, handle);
    END;

    (********************************************************************)
    (** Unload those extra fonts **)

  FUNCTION vst_unload_fonts(handle, select : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      intin[0] := select;
      vst_unload_fonts := gemvdif(120, handle);
    END;

    (************************************************************************)
    (** set clipping rectangle **)

  FUNCTION vs_clip(handle : INTEGER;
                   clipflag : INTEGER;
                   pxyarray : ARRAY_4) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 2;
      contrl[3] := 1;
      intin[0] := clipflag;
      FOR i := 0 TO 3 DO
        ptsin[i] := pxyarray[i];
      vs_clip := gemvdif(129, handle);
    END;

    (************************)
    (**  OUTPUT FUNCTIONS  **)
    (***************************************************************)
    (** polyline **)

  FUNCTION v_pline(handle, count : INTEGER;
                   VAR pxyarray) : INTEGER;

    VAR
      pxy : Ptsin_ARRAY ABSOLUTE pxyarray;
      i, n : INTEGER;
    BEGIN
      contrl[1] := count;    (* number of vertices to follow *)
      contrl[3] := 0;
      n := count*2-1;        (* twice as many numbers as there are coords *)
      FOR i := 0 TO n DO
        ptsin[i] := pxy[i];
      v_pline := gemvdif(6, handle);
    END;

    (****************************************************************)
    (** polymarker **)

  FUNCTION v_pmarker(handle, count : INTEGER;
                     VAR pxyarray ) : INTEGER;

    VAR
      pxy : Ptsin_ARRAY ABSOLUTE pxyarray;
      i, n : INTEGER;
    BEGIN
      contrl[1] := count;          (* number of markers *)
      contrl[3] := 0;
      n := count*2-1;
      FOR i := 0 TO n DO
        ptsin[i] := pxy[i];
      v_pmarker := gemvdif(7, handle);
    END;

    (**************************************************************)
    (** text **)

  FUNCTION v_gtext(handle, x, y : INTEGER;
                   VAR chstring ) : INTEGER;

    VAR
      pstrg : MaxString ABSOLUTE chstring;
      i : INTEGER;
    BEGIN
      contrl[1] := 1;
      contrl[3] := LENGTH(pstrg);
      ptsin[0] := x;
      ptsin[1] := y;
      FOR i := 1 TO LENGTH(pstrg) DO
        intin[i-1] := ORD(pstrg[i]);
      intin[LENGTH(pstrg)] := 0;
      v_gtext := gemvdif(8, handle);
    END;

    (***************************************************************)
    (** filled area **)

  FUNCTION v_fillarea(handle, count : INTEGER;
                      VAR pxyarray) : INTEGER;

    VAR
      pxy : Ptsin_ARRAY ABSOLUTE pxyarray;
      i, n : INTEGER;
    BEGIN
      contrl[1] := count;
      contrl[3] := 0;
      n := count*2-1;
      FOR i := 0 TO n DO
        ptsin[i] := pxy[i];
      v_fillarea := gemvdif(9, handle);
    END;

    (**************************************************************)
    (** cell array **)

  FUNCTION v_cellarray(handle : INTEGER;
                       pxyarray : ARRAY_4;
                       rowlength, elused, numrows, wrtmode : INTEGER;
                       VAR col_array) : INTEGER;

    VAR
      colarray : intin_ARRAY ABSOLUTE col_array;
      i, j : INTEGER;
    BEGIN
      contrl[1] := 2;
      contrl[3] := rowlength*numrows;
      contrl[7] := rowlength;
      contrl[8] := elused;
      contrl[9] := numrows;
      contrl[10] := wrtmode;
      FOR i := 0 TO 3 DO
        ptsin[i] := pxyarray[i];
      j := contrl[3]-1;
      FOR i := 0 TO j DO
        intin[i] := colarray[i];
      v_cellarray := gemvdif(10, handle);
    END;

    (*****************)
    (**   GDP 's    **)
    (****************************************************************)
    (** gdp - bar **)

  FUNCTION v_bar(handle : INTEGER;
                 pxyarray : ARRAY_4) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 2;
      contrl[3] := 0;
      contrl[5] := 1;
      FOR i := 0 TO 3 DO
        ptsin[i] := pxyarray[i];
      v_bar := gemvdif(11, handle);
    END;

    (****************************************************************)
    (** GDP - arc **)

  FUNCTION v_arc(handle, x, y, radius, begang, endang : INTEGER) : INTEGER;
    VAR
      j : INTEGER;
    BEGIN
      contrl[1] := 4;
      contrl[3] := 2;
      contrl[5] := 2;
      intin[0] := begang;
      intin[1] := endang;
      ptsin[0] := x;
      ptsin[1] := y;
      ptsin[6] := radius;
      FOR j := 2 TO 5 DO ptsin[j] := 0;
      v_arc := gemvdif(11, handle);
    END;

    (********************************************************************)
    (** GDP - pieslice **)

  FUNCTION v_pieslice(handle, x, y, radius, begang, endang : INTEGER) : 
      INTEGER;
    VAR
      j : INTEGER;
    BEGIN
      contrl[1] := 4;
      contrl[3] := 2;
      contrl[5] := 3;
      intin[0] := begang;
      intin[1] := endang;
      ptsin[0] := x;
      ptsin[1] := y;
      ptsin[6] := radius;
      FOR j := 2 TO 5 DO ptsin[j] := 0;
      v_pieslice := gemvdif(11, handle);
    END;

    (**********************************************************************)
    (** GDP - circle **)

  FUNCTION v_circle(handle, x, y, radius : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 3;
      contrl[3] := 0;
      contrl[5] := 4;
      ptsin[0] := x;
      ptsin[1] := y;
      ptsin[4] := radius;
      ptsin[2] := 0;
      ptsin[3] := 0;
      ptsin[5] := 0;
      v_circle := gemvdif(11, handle);
    END;

    (*****************************************************************)
    (** GDP - Ellipse **)

  FUNCTION v_ellipse(handle, x, y, xradius, yradius : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 2;
      contrl[3] := 0;
      contrl[5] := 5;
      ptsin[0] := x;
      ptsin[1] := y;
      ptsin[2] := xradius;
      ptsin[3] := yradius;
      v_ellipse := gemvdif(11, handle);
    END;

    (********************************************************************)
    (** GDP - elliptical arc **)

  FUNCTION v_ellarc(handle, x, y, xradius, yradius,
                    begang, endang : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 2;
      contrl[3] := 2;
      contrl[5] := 6;
      intin[0] := begang;
      intin[1] := endang;
      ptsin[0] := x;
      ptsin[1] := y;
      ptsin[2] := xradius;
      ptsin[3] := yradius;
      v_ellarc := gemvdif(11, handle);
    END;

    (*****************************************************************)
    (** GDP - elliptical pie **)

  FUNCTION v_ellpie(handle, x, y, xradius, yradius,
                    begang, endang : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 2;
      contrl[3] := 2;
      contrl[5] := 7;
      intin[0] := begang;
      intin[1] := endang;
      ptsin[0] := x;
      ptsin[1] := y;
      ptsin[2] := xradius;
      ptsin[3] := yradius;
      v_ellpie := gemvdif(11, handle);
    END;

    (****************************************************************)
    (** GDP rounded rectangle **)

  FUNCTION v_rbox(handle : INTEGER;
                  xyarray : ARRAY_4) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 2;
      contrl[3] := 0;
      contrl[5] := 8;
      FOR i := 0 TO 3 DO
        ptsin[i] := xyarray[i];
      v_rbox := gemvdif(11, handle);
    END;

    (****************************************************************)
    (** GDP Filled rounded rectangle **)

  FUNCTION v_rfbox(handle : INTEGER;
                   xyarray : ARRAY_4) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 2;
      contrl[3] := 0;
      contrl[5] := 9;
      FOR i := 0 TO 3 DO
        ptsin[i] := xyarray[i];
      v_rfbox := gemvdif(11, handle);
    END;

    (**************************************************************)
    (**  Justified graphics text **)

  FUNCTION v_justified(handle, x, y : INTEGER;
                       VAR gstring;
                       jlength : INTEGER;
                       wordspace, charspace : INTEGER) : INTEGER;

    VAR
      pstrg : MaxString ABSOLUTE gstring;
      i : INTEGER;
    BEGIN
      contrl[1] := 2;
      contrl[3] := LENGTH(pstrg)+2;
      contrl[5] := 10;
      FOR i := 1 TO LENGTH(pstrg) DO
        intin[i+1] := ORD(pstrg[i]);
      intin[LENGTH(pstrg)] := 0;
      intin[0] := wordspace;
      intin[1] := charspace;
      ptsin[0] := x;
      ptsin[1] := y;
      ptsin[2] := jlength;
      ptsin[3] := 0;
      v_justified := gemvdif(10, handle);
    END;

    (***************************************************************)
    (** contour fill **)

  FUNCTION v_contourfill(handle, x, y, index : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 1;
      contrl[3] := 1;
      ptsin[0] := x;
      ptsin[1] := y;
      intin[0] := index;
      v_contourfill := gemvdif(103, handle);
    END;

    (***********************************************************)
    (** fill rectangle **)

  FUNCTION vr_recfl(handle : INTEGER;
                    pxyarray : ARRAY_4) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 2;
      contrl[3] := 0;
      FOR i := 0 TO 3 DO
        ptsin[i] := pxyarray[i];
      vr_recfl := gemvdif(114, handle);
    END;

    (*******************************)
    (*** SET ATTRIBUTE FUNCTIONS ***)
    (**********************************************************)
    (** general set routine, called by many procedures below **)

  FUNCTION genset(opcode, handle, param : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      intin[0] := param;
      genset := gemvdif(opcode, handle); 
          (* return value suggested *)
    END;

    (***************************************************************)
    (** set writing mode **)

  FUNCTION vswr_mode(handle, mode : INTEGER) : INTEGER;
    BEGIN
      vswr_mode := genset(32, handle, mode);
    END;

    (**************************************************************)
    (** set color representation **)

  FUNCTION vs_color(handle, index : INTEGER;
                    rgbin : ARRAY_3) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 4;
      intin[0] := index;
      intin[1] := rgbin[0];
      intin[2] := rgbin[1];
      intin[3] := rgbin[2];
      vs_color := gemvdif(14, handle);
    END;


    (***********************************************************)
    (** set polyline line type **)

  FUNCTION vsl_type(handle, style : INTEGER) : INTEGER;
    BEGIN
      vsl_type := genset(15, handle, style);
    END;

    (***********************************************************)
    (** set user defined line style pattern **)

  FUNCTION vsl_udsty(handle, PATTERN : INTEGER) : INTEGER;
    BEGIN
      vsl_udsty := genset(113, handle, PATTERN);
    END;

    (**********************************************************)
    (** set polyline linewidth **)

  FUNCTION vsl_width(handle, width : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 1;
      contrl[3] := 0;
      ptsin[0] := width;
      ptsin[1] := 0;
      vsl_width := gemvdif(16, handle);
      vsl_width := ptsout[0];
    END;

    (***********************************************************)
    (** set polyline color index **)

  FUNCTION vsl_color(handle, colindex : INTEGER) : INTEGER;
    BEGIN
      vsl_color := genset(17, handle, colindex);
    END;

    (*************************************************************)
    (** set polyline end style **)

  FUNCTION vsl_ends(handle, begstyle, endstyle : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 2;
      intin[0] := begstyle;
      intin[1] := endstyle;
      vsl_ends := gemvdif(108, handle);
    END;

    (**************************************************************)
    (** set polymarker type **)

  FUNCTION vsm_type(handle, symbol : INTEGER) : INTEGER;
    BEGIN
      vsm_type := genset(18, handle, symbol);
    END;

    (***************************************************************)
    (** set polymarker height **)

  FUNCTION vsm_height(handle, height : INTEGER) : INTEGER;
    BEGIN
      contrl[0] := 19;
      contrl[1] := 1;
      contrl[3] := 0;
      contrl[6] := handle;
      ptsin[0] := 0;
      ptsin[1] := height;
      vsm_height := gemvdif(19, handle);
      vsm_height := ptsout[1];
    END;

    (*************************************************************)
    (** set polymarker color index **)

  FUNCTION vsm_color(handle, colindex : INTEGER) : INTEGER;
    BEGIN
      vsm_color := genset(20, handle, colindex);
    END;

    (***************************************************************)
    (** set character height, absolute mode **)

  FUNCTION vst_height(handle, height : INTEGER;

                      VAR charwidth, charheight,
                      cellwidth, cellheight : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 1;
      contrl[3] := 0;
      ptsin[0] := 0;
      ptsin[1] := height;
      vst_height := gemvdif(12, handle);
      charwidth := ptsout[0];
      charheight := ptsout[1];
      cellwidth := ptsout[2];
      cellheight := ptsout[3];
    END;

    (**********************************************************************)
    (** set character cell height, points mode **)

  FUNCTION vst_point(handle, point : INTEGER;

                     VAR charwidth, charheight,
                     cellwidth, cellheight : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      intin[0] := point;
      vst_point := gemvdif(107, handle);
      charwidth := ptsout[0];
      charheight := ptsout[1];
      cellwidth := ptsout[2];
      cellheight := ptsout[3];
    END;

    (*********************************************************************)
    (** set text character baseline vector - rotation **)

  FUNCTION vst_rotation(handle, angle : INTEGER) : INTEGER;
    BEGIN
      vst_rotation := genset(13, handle, angle);
    END;

    (******************************************************************)
    (** set text font **)

  FUNCTION vst_font(handle, font : INTEGER) : INTEGER;
    BEGIN
      vst_font := genset(21, handle, font);
    END;

    (******************************************************************)
    (** set text color **)

  FUNCTION vst_color(handle, colindex : INTEGER) : INTEGER;
    BEGIN
      vst_color := genset(22, handle, colindex);
    END;

    (*****************************************************************)
    (** set text special effects **)

  FUNCTION vst_effects(handle, effects : INTEGER) : INTEGER;
    BEGIN
      vst_effects := genset(106, handle, effects);
    END;

    (****************************************************************)
    (** set graphics text alignment **)

  FUNCTION vst_alignment(handle, horin, vertin : INTEGER;
                         VAR horout, vertout : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 2;
      intin[0] := horin;
      intin[1] := vertin;
      vst_alignment := gemvdif(39, handle);
      horout := intout[0];
      vertout := intout[1];
    END;


    (*****************************************************************)
    (** set fill interior style **)

  FUNCTION vsf_interior(handle, style : INTEGER) : INTEGER;
    BEGIN
      vsf_interior := genset(23, handle, style);
    END;

    (****************************************************************)
    (** set fill style index **)

  FUNCTION vsf_style(handle, styleindex : INTEGER) : INTEGER;
    BEGIN
      vsf_style := genset(24, handle, styleindex);
    END;

    (***************************************************************)
    (** set fill color index **)

  FUNCTION vsf_color(handle, colorindex : INTEGER) : INTEGER;
    BEGIN
      vsf_color := genset(25, handle, colorindex);
    END;

    (****************************************************************)
    (** set fill perimeter visibility **)

  FUNCTION vsf_perimeter(handle, pervis : INTEGER) : INTEGER;
    BEGIN
      vsf_perimeter := genset(104, handle, pervis);
    END;

    (**********************************************************************)
    (** Exchange fill pattern **)

  FUNCTION vsf_updat(handle : INTEGER;
                     VAR pfill_pat;
                     planes : INTEGER) : INTEGER;
    VAR
      pfillpat : intin_ARRAY ABSOLUTE pfill_pat;
      i : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 16*planes;
      FOR i := 0 TO contrl[3] DO
        intin[i] := pfillpat[i];
      vsf_updat := gemvdif(112, handle);
    END;

    (***********************************************************************)
    (** Get pixel value **)

  FUNCTION v_get_pixel(handle, v_gx, v_gy : INTEGER;
                       VAR v_pel, v_index : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 1;
      contrl[3] := 0;
      ptsin[0] := v_gx;
      ptsin[1] := v_gy;
      v_get_pixel := gemvdif(105, handle);
      v_pel := intout[0];
      v_index := intout[1];
    END;
(****************************************************************)
(** Copy rastor , Opaque **)

FUNCTION vro_cpyfm (handle, wrmode : INTEGER;
                        pxyarray : ARRAY_8;
                        psrcMFDB, pdesMFDB : MFDB) : INTEGER;

VAR
        i :     INTEGER;
        sm, dm : MFDB;
        ltemp : LONG;
BEGIN
        contrl[1] := 4;
        contrl[3] := 1;
        (* ensure MFDB is local to get right segment address *)
        sm := psrcMFDB;
        ltemp.ad := ADDR(sm);
        contrl[7] := ltemp.o;           (* offset of MFDB *)
        contrl[8] := ltemp.s;           (* segemnt of MFDB *)
        dm := pdesMFDB;
        ltemp.ad := ADDR(dm);
        contrl[9] := ltemp.o;
        contrl[10] := ltemp.s;
        intin[0] := wrmode;               (* logic operation write mode *)
        FOR i:=0 TO 7 DO
                ptsin[i] := pxyarray[i];
        vro_cpyfm := gemvdif(109,handle);
END;

(*************************************************************)
(** Transform Form **)

FUNCTION vr_trnfm (handle : INTEGER; psrcMFDB : MFDB;
                     VAR pdesMFDB : MFDB) : INTEGER;

VAR
        sm, dm : MFDB;
        ltemp : LONG;
BEGIN
        contrl[1] := 0;
        contrl[3] := 0;
        sm := psrcMFDB;
        ltemp.ad := ADDR(sm);
        contrl[7] := ltemp.o;
        contrl[8] := ltemp.s;
        dm := pdesMFDB;
        ltemp.ad := ADDR(dm);
        contrl[9] := ltemp.o;
        contrl[10] := ltemp.s;
        vr_trnfm := gemvdif(110,handle);
        pdesMFDB:=dm;
END;

                   (*************************)
                   (**** INPUT FUNCTIONS ****)
(****************************************************************)
(** Copy rastor , Transparent **)

FUNCTION vrt_cpyfm (handle, wrmode : INTEGER;
                    pxyarray : ARRAY_8;
                    psrcMFDB, pdesMFDB : MFDB;
                    color : ARRAY_2) : INTEGER;

VAR     i :     INTEGER;
        sm, dm : MFDB;
        ltemp : LONG;
BEGIN
        contrl[1] := 4;
        contrl[3] := 3;
        sm := psrcMFDB;                   (* local MFDB *)
        ltemp.ad := ADDR(sm);             (* put local address in temp ptr *)
        contrl[7] := ltemp.o;             (* hi order word of address ptr *)
        contrl[8] := ltemp.s;             (* lo order word *)
        dm := pdesMFDB;
        ltemp.ad := ADDR(dm);
        contrl[9] := ltemp.o;
        contrl[10] := ltemp.s;
        intin[0] := wrmode;               (* logic operation write mode *)
        intin[1] := color[0];
        intin[2] := color[1];
        FOR i:=0 TO 7 DO
                ptsin[i] := pxyarray[i];
        vrt_cpyfm := gemvdif(121,handle);
END;

    (*************************)
    (**** INPUT FUNCTIONS ****)
    (*********************************************************************)
    (** Set Input Mode **)

  FUNCTION vsin_mode(handle, devtype, mode : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 2;
      intin[0] := devtype;
      intin[1] := mode;
      vsin_mode := gemvdif(33, handle);
    END;

    (************************************************************************)
    (**  Input locator, request mode **)
    (**********************************)

  FUNCTION vrq_locator(handle, x, y : INTEGER;
                       VAR xout, yout : INTEGER;
                       VAR term : CHAR) : INTEGER;
    BEGIN
      contrl[1] := 1;
      contrl[3] := 0;
      ptsin[0] := x;
      ptsin[1] := y;
      vrq_locator := gemvdif(28, handle);
      xout := ptsout[0];
      yout := ptsout[1];
      term := CHR(intout[0]); 
          (* return single byte character *)
    END;

    (******************************************************************)
    (** Input Locator , Sample mode **)

  FUNCTION vsm_locator(handle, x, y : INTEGER;
                       VAR xout, yout, term : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 1;
      contrl[3] := 0;
      ptsin[0] := x;
      ptsin[1] := y;
      vsm_locator := gemvdif(28, handle);
      xout := ptsout[0];
      yout := ptsout[1];
      term := intout[0];
      IF contrl[4] < 1 THEN vsm_locator := contrl[2]
      ELSE vsm_locator := contrl[4];
    END;

    (********************************************************************)
    (**  Input Valuator, Request Mode **)

  FUNCTION vrq_valuator(handle, valin : INTEGER;
                        VAR valout : INTEGER;
                        VAR term : CHAR) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      intin[0] := valin;
      vrq_valuator := gemvdif(29, handle);
      valout := intout[0];
      term := CHR(intout[1]);
    END;

    (*******************************************************************)
    (** Input Valuator, Sample Mode **) 

  FUNCTION vsm_valuator(handle, valin : INTEGER;
                        VAR valout : INTEGER;
                        VAR term : CHAR;
                        VAR status : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      intin[0] := valin;
      vsm_valuator := gemvdif(29, handle);
      valout := intout[0];
      status := contrl[4];
      term := CHR(intout[1]);
    END;

    (*****************************************************************)
    (** Input Choice, request Mode **)

  FUNCTION vrq_choice(handle, in_choice : INTEGER;
                      VAR out_choice : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      intin[0] := in_choice;
      vrq_choice := gemvdif(30, handle);
      out_choice := intout[0];
    END;

    (*******************************************************************)
    (** Input Choice, Sample Mode **)

  FUNCTION vsm_choice(handle : INTEGER;
                      VAR choice : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      vsm_choice := gemvdif(30, handle);
      choice := intout[0];
    END;

    (******************************************************************)
    (** Input String, Request Mode **)

  FUNCTION vrq_string(handle, maxlen, echomode : INTEGER;
                      echoxy : ARRAY_2;
                      VAR instring) : INTEGER;

    VAR
      pstrg : MaxString ABSOLUTE instring;
      i : INTEGER;
    BEGIN
      contrl[1] := 1;
      contrl[3] := 2;
      intin[0] := 0-maxlen;
          (* force standard keyboard input *)
      intin[1] := echomode;
      ptsin[0] := echoxy[0];
      ptsin[1] := echoxy[1];
      vrq_string := gemvdif(31, handle);
      pstrg := '';
          (* null string *)
      IF contrl[4] > 0 THEN
        FOR i := 1 TO contrl[4] DO
          pstrg := CONCAT(pstrg, CHR(intout[i-1]));
      (* into string char form *)
    END;

    (****************************************************************)
    (** Input String, Sample Mode **)

  FUNCTION vsm_string(handle, maxlen, echomode : INTEGER;
                      echoxy : ARRAY_2;
                      VAR instring;
                      VAR status : INTEGER) : INTEGER;

    VAR
      pstrg : MaxString ABSOLUTE instring;
      I : INTEGER;
    BEGIN
      contrl[1] := 1;
      contrl[3] := 2;
      intin[0] := 0-maxlen;
          (* force standard keyboard input  *)
      intin[1] := echomode;
      ptsin[0] := echoxy[0];
      ptsin[1] := echoxy[1];
      vsm_string := gemvdif(31, handle);
      pstrg := '';
          (* null string *)
      IF contrl[4] > 0 THEN
        FOR i := 1 TO contrl[4] DO
          pstrg := CONCAT(pstrg, CHR(intout[i-1]));
      (* into string char form *)
      status := contrl[4];
      vsm_string := status;
    END;

    (*******************************************************************)
    (** Set Mouse Form **)

  FUNCTION vsc_form(handle : INTEGER;
                    pcurform : ARRAY_37) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 37;
      FOR i := 0 TO 36 DO
        intin[i] := pcurform[i];
      vsc_form := gemvdif(111, handle);
    END;


(********************************************************************)
(** Exchange Timer Interrupt Vector **)

FUNCTION vex_timv (handle : INTEGER;
                  timaddr : ADDRESS;
                  VAR otimaddr : ADDRESS;
                  VAR     timconv : INTEGER) : INTEGER;
VAR
  ltemp : LONG;
BEGIN
        contrl[1] := 0;
        contrl[3] := 0;
        ltemp.ad := timaddr;
        contrl[7] := ltemp.o;
        contrl[8] := ltemp.s;
        vex_timv := gemvdif(118,handle);
        ltemp.o := contrl[9];
        ltemp.s := contrl[10];
        otimaddr := ltemp.ad;
        timconv := intout[0];
END;

    (****************************************************************)
    (*** show graphic cursor ****)

  FUNCTION v_show_c(handle, RESET : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      intin[0] := RESET;
      v_show_c := gemvdif(122, handle);
    END;

    (*******************************************************)
    (** hide graphic cursor **)

  FUNCTION v_hide_c(handle : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      v_hide_c := gemvdif(123, handle);
    END;

    (********************************************************************)
    (** Sample Mouse Button State **)

  FUNCTION vq_mouse(handle : INTEGER;
                    VAR pstatus, x, y : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      vq_mouse := gemvdif(124, handle);
      pstatus := intout[0];
      x := ptsout[0];
      y := ptsout[1];
    END;

(********************************************************************)
(** Exchange Button Change Vector **)

FUNCTION vex_butv (handle : INTEGER;
                        pusrcode : ADDRESS;
                        VAR psavcode : ADDRESS) : INTEGER;
VAR
  ltemp : LONG;
BEGIN
        contrl[1] := 0;
        contrl[3] := 0;
        ltemp.ad := pusrcode;
        contrl[7] := ltemp.o;
        contrl[8] := ltemp.s;
        vex_butv := gemvdif(125,handle);
        ltemp.o := contrl[9];
        ltemp.s := contrl[10];
        psavcode := ltemp.ad;
END;

(********************************************************************)
(** Exchange Mouse Movement Vector **)

FUNCTION vex_motv (handle : INTEGER;
                   pusrcode : ADDRESS;
                   VAR psavcode : ADDRESS) : INTEGER;
VAR
  ltemp : LONG;
BEGIN
        contrl[1] := 0;
        contrl[3] := 0;
        ltemp.ad := pusrcode;
        contrl[7] := ltemp.o;
        contrl[8] := ltemp.s;
        vex_motv := gemvdif(126,handle);
END;

(**********************************************************************)
(** Exchange Cursor Change  Vector **)

FUNCTION vex_curv (handle : INTEGER;
                        pusrcode : ADDRESS;
                        VAR psavcode : ADDRESS) : INTEGER;
VAR
  ltemp : LONG;
BEGIN
        contrl[1] := 0;
        contrl[3] := 0;
        ltemp.ad := pusrcode;
        contrl[7] := ltemp.o;
        contrl[8] := ltemp.s;
        vex_curv := gemvdif(127,handle);
        ltemp.o := contrl[9];
        ltemp.s := contrl[10];
        psavcode := ltemp.ad;
END;

    (***********************************************************************)
    (** Sample Keyboard State Information **)

  FUNCTION vq_key_s(handle : INTEGER;
                    VAR pstatus : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      vq_key_s := gemvdif(128, handle);
      pstatus := intout[0];
    END;

    (*********************************************************************)


    (**** INQUIRE FUNCTIONS *******)
    (*********************************************************************)
    (** Inquire color representation **)
    {                                               *!*      }

  FUNCTION vq_color(handle, colorindex, setflag : INTEGER;
                    VAR rgb : ARRAY_3) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 2;
      intin[0] := colorindex;
      intin[1] := setflag;
      vq_color := gemvdif(26, handle);
      FOR i := 0 TO 2 DO
        rgb[i] := intout[i+1];
    END;

    (**********************************************************************)
    (** Inquire Cell Array **)

  FUNCTION vq_cellarray(handle : INTEGER;
                        pxyarray : ARRAY_4;
                        rowlen, numrows : INTEGER;
                        VAR elused, rowsused, status : INTEGER;
                        VAR colarray : intout_ARRAY) : INTEGER;

    VAR i, n : INTEGER;
    BEGIN
      contrl[1] := 2;
      contrl[7] := rowlen;
      contrl[8] := numrows;
      FOR i := 0 TO 3 DO ptsin[i] := pxyarray[i];
      vq_cellarray := gemvdif(27, handle);
      elused := contrl[9];
      rowsused := contrl[10];
      status := contrl[11];
      n := elused*rowsused-1;
      FOR i := 0 TO n DO colarray[i] := intout[i];
    END;

    (********************************************************)
    (** Inquire polyline attributes *)
    {                                               *!*      }

  FUNCTION vql_attributes(handle : INTEGER;
                          VAR attrib : ARRAY_6) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      vql_attributes := gemvdif(35, handle);
      FOR i := 0 TO 4 DO
        attrib[i] := intout[i];
      attrib[5] := ptsout[0];
    END;

    (*********************************************************)
    (** Inquire polymarker attributes **)
    {                                               *!*      }

  FUNCTION vqm_attributes(handle : INTEGER;
                          VAR attrib : ARRAY_5) : INTEGER;

    VAR
      I : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      vqm_attributes := gemvdif(36, handle);
      FOR i := 0 TO 2 DO
        attrib[i] := intout[i];
      attrib[3] := ptsout[0];
      attrib[4] := ptsout[1];
    END;

    (***********************************************************)
    (** Inquire fill area attributes **)
    {                                               *!*      }

  FUNCTION vqf_attributes(handle : INTEGER;
                          VAR attrib : ARRAY_5) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      vqf_attributes := gemvdif(37, handle);
      FOR i := 0 TO 4 DO
        attrib[i] := intout[i];
    END;

    (******************************************************************)
    (** Inquire current Graphic text attributes **)

  FUNCTION vqt_attributes(handle : INTEGER;
                          VAR attrib : ARRAY_10) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      vqt_attributes := gemvdif(38, handle);
      FOR i := 0 TO 5 DO
        attrib[i] := intout[i];
      FOR i := 6 TO 9 DO
        attrib[i] := ptsout[i-6];
    END;

    (**********************************************************************)
    (** Extended Inquire Function **)

  FUNCTION vq_extnd(handle, owflag : INTEGER;
                    VAR workout : ARRAY_57) : INTEGER;

    VAR
      i : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      intin[0] := owflag;
      vq_extnd := gemvdif(102, handle);
      FOR i := 0 TO 44 DO
        workout[i] := intout[i];
      FOR i := 45 TO 56 DO
        workout[i] := ptsout[i-45];
    END;


    (*******************************************************************)
    (** inquire current input mode **)

  FUNCTION vqin_mode(handle, devtype : INTEGER;
                     VAR inputmode : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      intin[0] := devtype;
      vqin_mode := gemvdif(115, handle);
      inputmode := intout[0];
    END;

    (*******************************************************************)
    (** Inquire Text Extent **)

  FUNCTION vqt_extent(handle : INTEGER;
                      VAR chstring;
                      VAR extent : ARRAY_8) : INTEGER;

    VAR
      pstrg : MaxString ABSOLUTE chstring;
      I : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := LENGTH(pstrg);
      FOR i := 1 TO LENGTH(pstrg) DO intin[i-1] := ORD(pstrg[i]);
      intin[LENGTH(pstrg)] := 0;
      vqt_extent := gemvdif(116, handle);
      FOR i := 0 TO 7 DO extent[i] := ptsout[i];
    END;

    (**********************************************************************)
    (** Inquire character cell width **)

  FUNCTION vqt_width(handle : INTEGER;
                     character : CHAR;
                     VAR cellwidth, leftdelta, rightdelta : INTEGER) : INTEGER
                         ;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      intin[0] := ORD(character);
      vqt_width := gemvdif(117, handle);
      cellwidth := ptsout[0];
      leftdelta := ptsout[2];
      rightdelta := ptsout[4];
    END;

    (********************************************************************)
    (*** Inquire font name and index **)

  FUNCTION vqt_name(handle, elementnum : INTEGER;
                    VAR name ) : INTEGER;

    VAR
      pstrg : MaxString ABSOLUTE name;
      i : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      intin[0] := elementnum;
      vqt_name := gemvdif(130, handle);
      pstrg := '';
          (* initialize string to null *)
      FOR i := 1 TO 32 DO
        pstrg := CONCAT(pstrg, CHR(intout[i]));
    END;

    (********************************************************************)
    (** Inquire Current Font Information **)

  FUNCTION vqt_font_info(handle : INTEGER;
                        VAR minADE, maxADE : INTEGER;
                        VAR distances : ARRAY_5;
                        VAR maxwidth : INTEGER;
                        effects : ARRAY_3) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      vqt_font_info := gemvdif(131, handle);
      minADE := intout[0];
      maxADE := intout[1];
      distances[0] := ptsout[1];
      distances[1] := ptsout[3];
      distances[2] := ptsout[5];
      distances[3] := ptsout[7];
      distances[4] := ptsout[9];
      maxwidth := ptsout[0];
      effects[0] := ptsout[2];
      effects[1] := ptsout[4];
      effects[2] := ptsout[6];
    END;

    (********************************************************************)
    (** Inquire Justified Graphics Text  **)

  FUNCTION vqt_justified(handle, x, y : INTEGER;
                         VAR gstring;
                         jlength, wordspace, charspace : INTEGER;
                         VAR offsets : ptsout_ARRAY) : INTEGER;

    VAR
      pstrg : MaxString ABSOLUTE gstring;
      i, n : INTEGER;

    BEGIN
      contrl[1] := 2;
      contrl[3] := LENGTH(pstrg)+2;
      intin[0] := wordspace;
      intin[1] := charspace;
      FOR i := 1 TO LENGTH(pstrg) DO
        intin[i+1] := ORD(pstrg[i]);
      intin[LENGTH(pstrg)+2] := 0;
      ptsin[0] := x;
      ptsin[1] := y;
      ptsin[2] := jlength;
      ptsin[3] := 0;
      vqt_justified := gemvdif(132, handle);
      n := 2*LENGTH(pstrg)-1;
      IF n > 0 THEN
        FOR i := 0 TO n DO
          offsets[i] := ptsout[i];
    END;


    (*********************)
    (******  ESCAPES *****)
    (********************************************************************)
    (** escape : inquire addressable alpha char cells **)

  FUNCTION vq_chcells(handle : INTEGER;
                      VAR rows, columns : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      contrl[5] := 1;
      vq_chcells := gemvdif(5, handle);
      rows := intout[0];
      columns := intout[1];
    END;

    (************************************************************)
    (*** general escape routine..called by many of those below **)

  FUNCTION genescape(fid, handle : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      contrl[5] := fid; 
          (* function id *)
      genescape := gemvdif(5, handle);
    END;

    (**************************************************************)

  FUNCTION v_exit_cur(handle : INTEGER) : INTEGER;
    BEGIN
      v_exit_cur := genescape(2, handle);
    END;

  FUNCTION v_enter_cur(handle : INTEGER) : INTEGER;
    BEGIN
      v_enter_cur := genescape(3, handle);
    END;

  FUNCTION v_curup(handle : INTEGER) : INTEGER;
    BEGIN
      v_curup := genescape(4, handle);
    END;

  FUNCTION v_curdown(handle : INTEGER) : INTEGER;
    BEGIN
      v_curdown := genescape(5, handle);
    END;

  FUNCTION v_curright(handle : INTEGER) : INTEGER;
    BEGIN
      v_curright := genescape(6, handle);
    END;

  FUNCTION v_curleft(handle : INTEGER) : INTEGER;
    BEGIN
      v_curleft := genescape(7, handle);
    END;

  FUNCTION v_curhome(handle : INTEGER) : INTEGER;
    BEGIN
      v_curhome := genescape(8, handle);
    END;

  FUNCTION v_eeos(handle : INTEGER) : INTEGER;
    BEGIN
      v_eeos := genescape(9, handle);
    END;

  FUNCTION v_eeol(handle : INTEGER) : INTEGER;
    BEGIN
      v_eeol := genescape(10, handle);
    END;

    (*******************************************************************)
    (** direct alpha cursor address **)

  FUNCTION vs_curaddress(handle, row, column : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 2;
      contrl[5] := 11;
      intin[0] := row;
      intin[1] := column;
      vs_curaddress := gemvdif(5, handle);
    END;

    (**************************************************************)
    (** output cursor addressable text **)

  FUNCTION v_curtext(handle : INTEGER;
                     VAR chstring ) : INTEGER;

    VAR
      pstrg : MaxString ABSOLUTE chstring;
      i : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := LENGTH(pstrg);
      contrl[5] := 12;
      FOR i := 1 TO LENGTH(pstrg) DO intin[i-1] := ORD(pstrg[i]);
      intin[LENGTH(pstrg)] := 0;
      v_curtext := gemvdif(5, handle);
    END;

    (**************************************************************)

  FUNCTION v_rvon(handle : INTEGER) : INTEGER;
    BEGIN
      v_rvon := genescape(13, handle);
    END;

  FUNCTION v_rvoff(handle : INTEGER) : INTEGER;
    BEGIN
      v_rvoff := genescape(14, handle);
    END;

    (***************************************************************)
    (** inquire current alpha cursor address **)

  FUNCTION vq_curaddress(handle : INTEGER;
                         VAR row, column : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      contrl[5] := 15;
      vq_curaddress := gemvdif(5, handle);
      row := intout[0];
      column := intout[1];
    END;

    (**************************************************************)
    (** inquire tablet status **)

  FUNCTION vq_tabstatus(handle : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      contrl[5] := 16;
      vq_tabstatus := gemvdif(5, handle);
    END;

    (***************************************************************)
    (** Hard Copy **)

  FUNCTION v_hardcopy(handle : INTEGER) : INTEGER;
    BEGIN
      v_hardcopy := genescape(17, handle);
    END;

    (******************************************************************)
    (** place a graphic cursor at the specifeid location **)

  FUNCTION v_dspcur(handle, x, y : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 1;
      contrl[3] := 0;
      contrl[5] := 18;
      ptsin[0] := x;
      ptsin[1] := y;
      v_dspcur := gemvdif(5, handle);
    END;

    (**************************************************************)

  FUNCTION v_rmcur(handle : INTEGER) : INTEGER;
    BEGIN
      v_rmcur := genescape(19, handle);
    END;

    (**************************************************************)
    (***    Form advance **)

  FUNCTION v_form_adv(handle : INTEGER) : INTEGER;
    BEGIN
      v_form_adv := genescape(20, handle);
    END;

    (**************************************************************)
    (** Output Window **)

  FUNCTION v_output_window(handle : INTEGER;
                           xyarray : ARRAY_4) : INTEGER;

    VAR i : INTEGER;
    BEGIN
      contrl[1] := 2;
      contrl[3] := 0;
      contrl[5] := 21;
      FOR i := 0 TO 3 DO ptsin[i] := xyarray[i];
      v_output_window := gemvdif(5, handle);
    END;

    (***************************************************************)
    (** Clear display list **)

  FUNCTION v_clear_display_list(handle : INTEGER) : INTEGER;
    BEGIN
      v_clear_display_list := genescape(22, handle);
    END;

    (*******************************************************************)
    (** process a bit-image file **)

  FUNCTION v_bit_image(handle : INTEGER;
                       VAR filename;
                       aspect, xscale, yscale, halign, valign : INTEGER;
                       xyarray : ARRAY_4) : INTEGER;

    VAR
      pstrg : MaxString ABSOLUTE filename;
      i : INTEGER;

    BEGIN
      contrl[1] := 2;
      contrl[3] := LENGTH(pstrg)+5;
      contrl[5] := 23;
      intin[0] := aspect;
      intin[1] := xscale;
      intin[2] := yscale;
      intin[3] := halign;
      intin[4] := valign;
      FOR i := 1 TO LENGTH(pstrg) DO
        intin[i+4] := ORD(pstrg[i]);
      intin[LENGTH(pstrg)+5] := 0;
      FOR i := 0 TO 3 DO
        ptsin[i] := xyarray[i];
      v_bit_image := gemvdif(5, handle);
    END;

    (*********************************************************************)
    (** return printer scan heights **)

  FUNCTION vq_scan(handle : INTEGER;
                   VAR gheight, gslices, aheight, aslices, factor : INTEGER)
      : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      contrl[5] := 24;
      vq_scan := gemvdif(5, handle);
      gheight := intout[0];
      gslices := intout[1];
      aheight := intout[2];
      aslices := intout[3];
      factor := intout[4];
    END;

    (******************************************************************)
    (** print alpha text **)

  FUNCTION v_alpha_text(handle : INTEGER;
                        VAR tstring) : INTEGER;

    VAR
      pstrg : MaxString ABSOLUTE tstring;
      i : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := LENGTH(pstrg);
      contrl[5] := 25;
      FOR i := 1 TO LENGTH(pstrg) DO
        intin[i-1] := ORD(pstrg[i]);
      intin[LENGTH(pstrg)] := 0;
      v_alpha_text := gemvdif(5, handle);
    END;

    (**************************************************************)
    (** selection of IBM color palette 0 = red,green,yellow
                                       1=cyan,blue,magenta *)

  FUNCTION vs_palette(handle, PALETTE : INTEGER) : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      contrl[5] := 60;
      intin[0] := PALETTE;
      vs_palette := gemvdif(5, handle);
    END;

    (****************************************************************)
    (** generate tone **)

  FUNCTION v_sound(handle, frequency, duration : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 2;
      contrl[5] := 61;
      intin[0] := frequency;
      intin[1] := duration;
      v_sound := gemvdif(5, handle);
    END;

    (*********************************************************************)
    (** set/return state of mute flag **)

  FUNCTION v_mute(handle, action : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      contrl[5] := 62;
      intin[0] := action;
      v_mute := gemvdif(5, handle);
          (* v_mute is set to status *)
    END;

    (**********************************************************************)
    (** set tablet x and y resolution **)

  FUNCTION vt_resolution(handle, xres, yres : INTEGER;
                         VAR xset, yset : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 2;
      contrl[5] := 81;
      intin[0] := xres;
      intin[1] := yres;
      vt_resolution := gemvdif(5, handle);
      intout[0] := xset;
      intout[1] := yset;
    END;

    (******************************************************************)
    (** set tablet axis resolution **)

  FUNCTION vt_axis(handle, xres, yres : INTEGER;
                   VAR xset, yset : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 2;
      contrl[5] := 82;
      intin[0] := xres;
      intin[1] := yres;
      vt_axis := gemvdif(5, handle);
      intout[0] := xset;
      intout[1] := yset;
    END;

    (**********************************************************************)
    (** set tablet origin **)

  FUNCTION vt_origin(handle, xorigin, yorigin : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 2;
      contrl[5] := 83;
      intin[0] := xorigin;
      intin[1] := yorigin;
      vt_origin := gemvdif(5, handle);
    END;

    (***********************************************************************)
    (** inquire tablet dimensions **)

  FUNCTION vq_tdimensions(handle : INTEGER;
                          VAR xdim, ydim : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 0;
      contrl[5] := 84;
      vq_tdimensions := gemvdif(5, handle);
      intout[0] := xdim;
      intout[1] := ydim;
    END;

    (****************************************************************)
    (** set tablet alignment **)

  FUNCTION vt_alignment(handle, dx, dy : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 2;
      contrl[5] := 83;
      intin[0] := dx;
      intin[1] := dy;
      vt_alignment := gemvdif(5, handle);
    END;

    (*******************************************************************)
    (** Select film type and exposure time **)

  FUNCTION vsp_film(handle, index, lightness : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 2;
      contrl[5] := 91;
      intin[0] := index;
      intin[1] := lightness;
      vsp_film := gemvdif(5, handle);
    END;

    (*********************************************************************)
    (** inquire filmname **)

  FUNCTION vqp_filmname(handle, index : INTEGER;
                        VAR name ) : INTEGER;

    VAR
      pstrg : MaxString ABSOLUTE name;
      i : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      contrl[5] := 92;
      intin[0] := index;
      vqp_filmname := gemvdif(5, handle);
      pstrg := '';
      IF contrl[4] > 0 THEN
        FOR i := 0 TO contrl[4] DO
          pstrg := CONCAT(pstrg, CHR(intout[i]));
      vqp_filmname := contrl[4];
          (* return =0 for false status *)
      (* return <>0 for true status *)
    END;

    (*******************************************************************)
    (** enable/disable exposure **)

  FUNCTION vsc_expose(handle, state : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 1;
      contrl[5] := 93;
      intin[0] := state;
      vsc_expose := gemvdif(5, handle);
    END;

    (*********************************************************************)
    (** v_meta_extents **)

  FUNCTION v_meta_extents(handle, minx, miny, maxx, maxy : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 2;
      contrl[3] := 0;
      contrl[5] := 98;
      ptsin[0] := minx;
      ptsin[1] := miny;
      ptsin[2] := maxx;
      ptsin[3] := maxy;
      v_meta_extents := gemvdif(5, handle);
    END;
    (*******************************************************************)
    (*** write gsx metafile ***)

  FUNCTION v_write_meta(handle, numintin : INTEGER;
                        VAR var_int_in;
                        numptsin : INTEGER;
                        VAR var_pts_in ) : INTEGER;

    VAR
      int_in : intin_ARRAY ABSOLUTE var_int_in;
      pts_in : ptsin_ARRAY ABSOLUTE var_pts_in;
      i, k : INTEGER;
    BEGIN
      contrl[1] := numptsin;
      k := numptsin * 2 - 1;
      for i := 0 TO k DO
        ptsin[i] := pts_in[i];
      contrl[3] := numintin;
      k := numintin - 1;
      for i := 0 TO k DO
        intin[i] := int_in[i];
      contrl[5] := 99;
      contrl[6] := handle;
      v_write_meta := gemvdif(5, handle);
    END;

    (******************************************************************)
    (** change gsx metafile filename from  gsxfile.gsx **)

  FUNCTION vm_filename(handle : INTEGER;
                       VAR filename) : INTEGER;

    VAR
      pstrg : MaxString ABSOLUTE filename;
      i : INTEGER;
    BEGIN
      contrl[1] := 0;
      contrl[3] := LENGTH(pstrg);
      contrl[5] := 100;
      FOR i := 1 TO LENGTH(pstrg) DO intin[i-1] := ORD(pstrg[i]);
      intin[LENGTH(pstrg)] := 0;
      vm_filename := gemvdif(5, handle);
    END;

    (********************************************************************)
    (** NOTE : the following two fcns are included in the C bindings **)
    (**        but are actually specialized versions of v_write_meta **)
    (**        that use metafile sub-opcodes reserved for GEM output **)
    (**        and modify the metafile header, NOT the body          **)
    (**        see Appendix C of the GEM VDI Ref. Guide              **)

    (*********************************************************************)
    (** set metafile page size **)

  FUNCTION vm_pagesize(handle, pgwidth, pgheight : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 3;
      contrl[5] := 99;
      intin[0] := 0;
      intin[1] := pgwidth;
          (** dimensions are in tenths of   **)
      intin[2] := pgheight; 
          (**    millimeters                **)
      vm_pagesize := gemvdif(5, handle);
    END;

    (******************************************************************)
    (** set metafile window coordinates **)

  FUNCTION vm_coords(handle, llx, lly, urx, ury : INTEGER) : INTEGER;

    BEGIN
      contrl[1] := 0;
      contrl[3] := 5;
      contrl[5] := 99;
      intin[0] := 1;
      intin[1] := llx;
          (** coords of lower left corner  **)
      intin[2] := lly;
          (**    of window                 **)
      intin[3] := urx;
          (** coords of upper right corner **)
      intin[4] := ury;
          (**    of window                 **)
      vm_coords := gemvdif(5, handle);
    END;

    { Next are the object definitions... OBDEF.INC }

    Function Fobadr(a,x,y:integer):ADDRESS;  { used by rest of functions }
var
  i,j : integer;
  ltemp : LONG;
begin
  ltemp.ad := tree;
  ltemp.o := ltemp.o + (x*a) + y;
  Fobadr := ltemp.ad;
end;

Function Fpbadr(x:ADDRESS;y:integer):ADDRESS;  { used by rest of functions }
var
  i,j : integer;
  ltemp : LONG;
begin
  ltemp.ad := x;
  ltemp.o := ltemp.o + y;
  Fpbadr := ltemp.ad;
end;

Function Fob_next (x:integer):ADDRESS;
begin
   Fob_next:=Fobadr(kob_size,x,kob_next);
end;

Function Fob_head (x:integer):ADDRESS;
begin
   Fob_head:=Fobadr(kob_size,x,kob_head);
end;

Function Fob_tail (x:integer):ADDRESS;
begin
   Fob_tail:=Fobadr(kob_size,x,kob_tail);
end;

Function Fob_type (x:integer):ADDRESS;
begin
   Fob_type:=Fobadr(kob_size,x,kob_type);
end;

Function Fob_flags(x:integer):ADDRESS;
begin
   Fob_flags:=Fobadr(kob_size,x,kob_flags);
end;

Function Fob_state(x:integer):ADDRESS;
begin
   Fob_state:=Fobadr(kob_size,x,kob_state);
end;

Function Fob_spec(x:integer):ADDRESS;
begin
   Fob_spec:=Fobadr(kob_size,x,kob_spec);
end;

Function Fob_X(x:integer):ADDRESS;
begin
   Fob_X:=Fobadr(kob_size,x,kob_x);
end;

Function Fob_Y(x:integer):ADDRESS;
begin
   Fob_Y:=Fobadr(kob_size,x,kob_y);
end;

Function Fob_Width(x:integer):ADDRESS;
begin
   Fob_Width:=Fobadr(kob_size,x,kob_w);
end;

Function Fob_Height(x:integer):ADDRESS;
begin
   Fob_Height:=Fobadr(kob_size,x,kob_h);
end;

Function FTE_PTEXT(x:integer):ADDRESS;
begin
  FTE_PTEXT:=Fobadr(kte_size,x,kte_ptext);  (* TEDINFO text pointer  *)
end;

Function FTE_TXTLEN(x:integer):ADDRESS;
begin
  FTE_TXTLEN:=Fobadr(kte_size,x,kte_txtlen);  (* TEDINFO text length  *)
end;

Function FBI_PDATA(x:ADDRESS):ADDRESS; (* BITBLK - image data *)
begin
  FBI_PDATA:=Fpbadr(x,kbi_pdata);
end;

Function FBI_WB(x:ADDRESS):ADDRESS;        (*  width in bytes *)
begin
  FBI_WB:=Fpbadr(x,kbi_wb);
end;

Function FBI_HL(x:ADDRESS):ADDRESS;         (*  height - scan lines  *)
begin
  FBI_HL:=Fpbadr(x,kbi_hl);
end;

Function FIB_PMASK(x:ADDRESS):ADDRESS;      (* ICONBLK - icon mask *)
begin
  FIB_PMASK:=Fpbadr(x,kib_pmask);
end;

Function FIB_PDATA(x:ADDRESS):ADDRESS;      (*  icon data          *)
begin
  FIB_PDATA:=Fpbadr(x,kib_pdata);
end;

Function FIB_WICON(x:ADDRESS):ADDRESS;         (*  width in pixels    *)
begin
  FIB_Wicon:=Fpbadr(x,kib_wicon);
end;

Function FIB_HICON(x:ADDRESS):ADDRESS;         (*  height - scan lines *)
begin
  FIB_Hicon:=Fpbadr(x,kib_hicon);
end;

Function LLDS : ADDRESS;
{returns ADDRESS of data segment}
VAR
  ltemp : LONG;
begin
  ltemp.o := 0;
  ltemp.s := Dseg;             (* dseg is defined by Turbo PASCAL *)
  LLDS := ltemp.ad;
end;

Function LLCS : ADDRESS;
{returns ADDRESS of code segment}
VAR
  ltemp : LONG;
begin
  ltemp.o := 0;
  ltemp.s := Cseg;             (* cseg is defined by Turbo PASCAL *)
  LLCS := ltemp.ad;
end;

Function LBGET(inptr : ADDRESS) : Byte;
{returns the byte value pointed to by inptr}
VAR
  lptr : LONG ABSOLUTE inptr;
begin
  LBGET := Mem[lptr.s:lptr.o];
end;

Procedure LBSET (inptr : ADDRESS; invalue : Byte);
{sets the byte pointed to by inptr to invalue}
VAR
  lptr : LONG ABSOLUTE inptr;
begin
  Mem[lptr.s:lptr.o]:= invalue;
end;

Function LWGET(inptr : ADDRESS) : Integer;
{returns the integer value pointed to by inptr}
VAR
  lptr : LONG ABSOLUTE inptr;
begin
  LWGET := MemW[lptr.s:lptr.o];
end;

Procedure LWSET (inptr : ADDRESS; invalue : Integer);
{sets the integer pointed to by inptr to invalue}
VAR
  lptr : LONG ABSOLUTE inptr;
begin
  MemW[lptr.s:lptr.o]:= invalue;
end;

Function LLGET (inptr : ADDRESS) : ADDRESS;
{returns the ADDRESS pointed to by inptr}
VAR
  lptr : LONG ABSOLUTE inptr;
  ltemp : LONG;
begin
  ltemp.o := MemW[lptr.s:lptr.o];
  ltemp.s := MemW[lptr.s:lptr.o+2];
  LLGET := ltemp.ad;
end;

Procedure LLSET (inptr, invalue : ADDRESS);
{sets the ADDRESS pointed to by inptr to invalue}
VAR
  lptr : LONG ABSOLUTE inptr;
  lvalue : LONG ABSOLUTE invalue;
begin
  MemW[lptr.s:lptr.o] := lvalue.o;
  MemW[lptr.s:lptr.o+2]:= lvalue.s;
end;

Function LBYTE0(inptr  : ADDRESS) : Byte;
{returns the zero/base byte of the ADDRESS pointed to by inptr}
VAR
  lptr : LONG ABSOLUTE inptr;
BEGIN
  LBYTE0:=Mem[lptr.s:lptr.o];
END;

Function LBYTE1(inptr : ADDRESS) : Byte;
{returns the first byte of the ADDRESS pointed to by inptr}
VAR
  lptr : LONG ABSOLUTE inptr;
BEGIN
  LBYTE1:=Mem[lptr.s:lptr.o+1];
END;

Function LBYTE2(inptr : ADDRESS) : Byte;
{returns the second byte of the ADDRESS pointed to by inptr}
VAR
  lptr : LONG ABSOLUTE inptr;
BEGIN
  LBYTE2:=Mem[lptr.s:lptr.o+2];
END;

Function LBYTE3(inptr : ADDRESS) : Byte;
{returns the third byte of the ADDRESS pointed to by inptr}
VAR
  lptr : LONG ABSOLUTE inptr;
BEGIN
  LBYTE3:=Mem[lptr.s:lptr.o+3];
END;


(**************************************************************************)
(**                                                                      **)
(**  unsigned multiplication and divsion are done with real values in    **)
(**    Turbo Pascal to prevent or detect overflow problems               **)
(**                                                                      **)
(**************************************************************************)

FUNCTION umul (ifactor1, ifactor2 : Integer) : Integer;
  VAR
    factor1, factor2, product : Real;
    msg : CharString;
    status : Integer;
  BEGIN
    factor1 := ifactor1;
    factor2 := ifactor2;
    product := factor1 * factor2;
    IF product<MAXINT THEN
      BEGIN
        IF product>(1-MAXINT) THEN
          umul := Round(product)
        ELSE
          BEGIN
            umul := 1-MAXINT;
            msg := '[3][multiplication underflow][Continue]';
            status := form_alert(1,msg);
          END;
      END
    ELSE
      BEGIN
        umul := MAXINT;
        msg := '[3][multiplication overflow][Continue]';
        status := form_alert(1,msg);
      END;
  END;

FUNCTION udiv (idividend, idivisor : Integer) : Integer;
  VAR
    dividend, divisor, result : Real;
    msg : charString;
    status : Integer;
  BEGIN
    dividend := idividend;
    divisor := idivisor;
    result := dividend / divisor;
    IF result<MAXINT THEN
      BEGIN
        IF result>(1-MAXINT) THEN
          udiv := Round(result)
        ELSE
          BEGIN
            udiv := 1-MAXINT;
            msg := '[3][division underflow][Continue]';
            status := form_alert(1,msg);
          END;
      END
    ELSE
      BEGIN
        udiv := MAXINT;
        msg := '[3][division overflow][Continue]';
        status := form_alert(1,msg);
      END;
  END;

FUNCTION umul_div (ifactor1, ifactor2, idivisor : Integer) : Integer;
  VAR
    factor1, factor2, divisor, result : Real;
    status : Integer;
    msg : CharString;
  BEGIN
    factor1 := ifactor1;
    factor2 := ifactor2;
    divisor := idivisor;
    result := factor1 * factor2 / divisor;
    IF result<MAXINT THEN
      BEGIN
        IF result>(1-MAXINT) THEN
          umul_div := ROUND(result)
        ELSE
          BEGIN
            umul_div := 1-MAXINT;
            msg := 'mul_div underflow';
            status := form_alert(1,msg);
          END;
      END
    ELSE
      BEGIN
        umul_div := MAXINT;
        msg := 'mul_div overflow';
        status := form_alert(1,msg);
      END;
  END;

PROCEDURE call_dos;
  {general DOS function call using dos_reg and then setting dos_err}
  {assumes dos_reg preset by the calling proc }
  BEGIN
    MSDOS(dos_reg);   {generates BIOS interrupt 21H }
    IF ((dos_reg.flags mod 2) = 0) THEN    {set dos_err based on carry flag }
      dos_err := FALSE                     {there was no carry required}
    ELSE
      dos_err := TRUE;                     {carry was required}
  END;

PROCEDURE dos_func (ax, lodsdx, hidsdx : Integer);
  { general DOS function call  }
  BEGIN
    dos_reg.ax := ax;
    dos_reg.dx := lodsdx;
    dos_reg.ds := hidsdx;
    call_dos;
  END;


FUNCTION dos_chdir(VAR pdrvpath ) : Integer;    { NOTE untyped parameter }
  { change the current directory }
  VAR
    cstrg : CharString;
    pstrg : CharString ABSOLUTE pdrvpath;
    ltemp : LONG;
  BEGIN
    MakeCstring(pstrg,cstrg);
    ltemp.ad := ADDR(cstrg);
    dos_func($3B00,ltemp.o,ltemp.s);
    MakePstring(cstrg,pstrg);
    dos_chdir := dos_reg.ax;          {return error codes, if any}
  END;

FUNCTION dos_gdir(drive : Integer; VAR pdrvpath ) : Integer;
  { get current directory        NOTE untyped parameter }
  VAR
    cstrg : CharString;
    pstrg : CharString ABSOLUTE pdrvpath;
    ltemp : LONG;
  BEGIN
    dos_reg.ax := $4700;
    dos_reg.dx := drive;  {0= default drive; 1= A:, etc}
    ltemp.ad := ADDR(cstrg);
    dos_reg.si := ltemp.o;
    dos_reg.ds := ltemp.s;
    call_dos;
    MakePstring(cstrg,pstrg);
    dos_gdir:=1;     {always returns TRUE}
  END;

FUNCTION dos_gdrv : Integer;
  { get current drive }
  BEGIN
    dos_reg.ax := $1900;
    call_dos;
    dos_gdrv := dos_reg.al;   {0 = A:, 1 = B:, etc }
  END;

FUNCTION dos_sdrv (newdrv : Integer) : Integer;
  { select new drive, new drive becomes default }
  BEGIN
    dos_reg.ax := $0E00;
    dos_reg.dx := newdrv;   {0 = A:, 1 = B:, etc }
    call_dos;
    dos_sdrv := dos_reg.al; { return number of drives allowed on system }
  END;

FUNCTION dos_sdta (VAR ldta) : Integer;
  { set the Disk Transfer Address (DTA) }
  VAR
    ltemp : LONG ABSOLUTE ldta;
  BEGIN
    dos_func($1A00,ltemp.o,ltemp.s);
    dos_sdta := dos_reg.ax;      {return error codes, if any}
  END;

FUNCTION dos_gdta : ADDRESS;
  { get the current Disk Transfer Address (DTA) }
  VAR
    ltemp : LONG;
  BEGIN
    dos_reg.ax := $2F00;
    call_dos;
    ltemp.o := dos_reg.bx;
    ltemp.s := dos_reg.es;
    dos_gdta := ltemp.ad;
  END;

FUNCTION dos_sfirst (VAR pspec; attr : Integer) : Integer;
  { search for first matching file     NOTE untyped parameter }
  VAR
    cstrg : CharString;
    pstrg : CharString ABSOLUTE pspec;
    ltemp : LONG;
  BEGIN
    MakeCstring(pstrg,cstrg);
    ltemp.ad := ADDR(cstrg);
    dos_reg.cx := attr;   {file attributes}
    dos_func($4E00,ltemp.o,ltemp.s);
    MakePstring(cstrg,pstrg);
    IF dos_err THEN
      dos_sfirst := 0
    ELSE
      dos_sfirst := 1;
  END;

FUNCTION dos_snext : Integer;
  { search for next matching file }
  { dos_sfirst must be used just before this one }
  BEGIN
    dos_reg.ax := $4F00;
    call_dos;
    IF dos_err THEN
      dos_snext := 0
    ELSE
      dos_snext := 1;
  END;

FUNCTION dos_open (VAR pname; access : Integer) : Integer;
  { open file with name pname      NOTE use of untype parameter }
  { access =0 for read, =1 for write, =2 for both }
  VAR
    cstrg : CharString;
    pstrg : CharString ABSOLUTE pname;
    ltemp : LONG;
  BEGIN
    MakeCstring(pstrg,cstrg);
    ltemp.ad := ADDR(cstrg);
    dos_reg.ah := $3D;
    dos_reg.al := access;
    dos_reg.dx := ltemp.o;
    dos_reg.ds := ltemp.s;
    call_dos;
    dos_open := dos_reg.ax;    {return the file handle }
  END;

FUNCTION dos_close (handle : Integer) : Integer;
  { close file }
  BEGIN
    dos_reg.ax := $3E00;
    dos_reg.bx := handle;
    call_dos;
    IF dos_err THEN
      dos_close := 0
    ELSE
      dos_close := 1;
  END;

FUNCTION block_read (handle, icnt : Integer; buff_ptr : LONG) : Integer;
  { read icnt bytes from file indicated by handle into memory at buff_ptr }
  BEGIN
    dos_reg.cx := icnt;  { set number of bytes to read in }
    dos_reg.bx := handle; { set which file to read from }
    dos_func($3F00,buff_ptr.o,buff_ptr.s);
    block_read := dos_reg.ax;        {return number of bytes actually read }
  END;

FUNCTION dos_read (handle : Integer; cnt : Real; buff_loc : ADDRESS) : Real;
  { read complete file into memory at buff_loc  }
  { it is the user's responsibility to be sure that allocated buffer is large }
  {   enough to hold cnt bytes of data }
  VAR
    block_size : Integer;
    buff_ptr : LONG;
    rd_cnt : Real;
  BEGIN
    buff_ptr.ad := buff_loc;
    block_size := MAXINT;                   { use largest possible blocks }
    rd_cnt := 0.0;
    dos_err := FALSE;
    WHILE (cnt>0.0) AND (NOT dos_err) DO
      BEGIN                               { NOTE:                          }
        IF cnt> block_size THEN           { cnt is only variable locally   }
          cnt := cnt - block_size         { the actual number of bytes     }
        ELSE                              {   read in, rd_cnt, is returned }
          BEGIN                           {   by this function             }
            block_size := trunc(cnt);
            cnt := 0.0;
          END;
        rd_cnt := rd_cnt + block_read(handle, block_size, buff_ptr);
        buff_ptr.s := buff_ptr.s + (block_size div 16);
        buff_ptr.o := buff_ptr.o + (block_size mod 16);
      END;
    dos_read := rd_cnt;
  END;

FUNCTION dos_lseek (handle, smode : Integer; sofst : LONG) : ADDRESS;
  { move file read/write pointer }
  VAR
    ltemp : LONG;
  BEGIN
    dos_reg.ah := $42;
    dos_reg.al := smode;      {mode =0 from beginning, =1 from current, }
    dos_reg.bx := handle;     {     =2 from EOF plus offset             }
    dos_reg.cx := sofst.i1;   {set hi word of desired offset }
    dos_reg.dx := sofst.i0;   {set low word of desired offset }
    call_dos;
    ltemp.o := dos_reg.ax;
    ltemp.s := dos_reg.dx;
    dos_lseek := ltemp.ad;   { return ptr to new location}
  END;

FUNCTION dos_wait : Integer;
  {get return code of a sub-process }
  { =$00 for normal termination, =$01 for control break termination  }
  { =$02 for critical device error, =$03 for terminate and stay resident }
  BEGIN
    dos_reg.ax := $4D00;
    call_dos;
    dos_wait := dos_reg.ax;
  END;

FUNCTION dos_alloc (nbytes : Real) : ADDRESS;
  {allocate memory }
  VAR
    maddr : LONG;
  BEGIN
    dos_reg.ax := $4800;
    IF (nbytes < 0) THEN                     { force insufficient memory by }
        dos_reg.bx := $FFFF                  {  requesting max # paragraphs }
    ELSE                                          { convert number of bytes }
      dos_reg.bx := TRUNC((nbytes + 15) / 16.0);  {  to paragraphs          }
    call_dos;
    maddr.ad := NIL;
    IF NOT dos_err THEN
      maddr.s := dos_reg.ax;        { allocated memory block starts at AX:0 }
    dos_alloc := maddr.ad;
  END;

FUNCTION dos_avail : REAL;
  { returns amount of memory available in bytes }
  { do NOT check dos_err after this call, it will always be true }
  VAR
    temp : real;
  BEGIN
    dos_reg.ax := $4800;     { call to allocate memory }
    dos_reg.bx := $FFFF;     { ask for larger block than possible }
    call_dos;                { this forces failure, ie dos_err = TRUE}
    temp := dos_reg.bh;      { and bx = largest available block in paragraphs}
    dos_avail := (temp * 4096.0)+(dos_reg.bl * 16.0);
  END;

FUNCTION dos_free (VAR maddr) : Integer;
  { free the memory that was allocated via dos_alloc }
  {NOTE use of untyped variable parameter }
  VAR
    laddr : LONG absolute maddr;
  BEGIN
    dos_reg.ax := $4900;
    dos_reg.es := laddr.s;
    call_dos;
    dos_free := dos_reg.ax;     {return error codes, if any}
  END;

FUNCTION dos_space (drv : Integer; VAR ptotal, pAvail : Real) : Integer;
  { get disk free space expressed in bytes}
  { call_dos returns with bx= avail clusters, dx = clusters per drive, }
  {   cx =  bytes per sector, and ax = sectors per cluster             }
  { if the drive requested was invalid ax = $FFFF                      }
  VAR
    clusb : real;
  BEGIN
    dos_reg.ax := $3600;
    dos_reg.dx := drv;       {0=default, 1=A:, etc}
    call_dos;
    IF (dos_reg.ax=$FFFF) THEN
      BEGIN
        ptotal := 0.0;
        pavail := 0.0;
      END
    ELSE
      BEGIN
        clusb := dos_reg.ax * dos_reg.cx; {calc size of cluster in bytes }
        ptotal := clusb * dos_reg.dx;     {calc total space in bytes }
        pavail := clusb * dos_reg.bx;     {calc total available space in bytes}
      END;
    dos_space := dos_reg.ax;             {return error codes, if any}
  END;

FUNCTION dos_rmdir (VAR ppath) : Integer;
  { remove directory entry   NOTE use of untyped parameter }
  VAR
    cstrg : CharString;
    pstrg : CharString ABSOLUTE ppath;
    ltemp : LONG;
  BEGIN
    MakeCstring(pstrg,cstrg);
    ltemp.ad := ADDR(cstrg);
    dos_func($3A00, ltemp.o, ltemp.s);
    IF dos_err THEN
      dos_rmdir := 0
    ELSE
      dos_rmdir := 1;
  END;

FUNCTION dos_create (VAR pname; attr : Integer) : Integer;
  { create file    NOTE use of untyped parameter }
  VAR
    cstrg : CharString;
    pstrg : CharString ABSOLUTE pname;
    ltemp : LONG;
  BEGIN
    MakeCstring(pstrg,cstrg);
    ltemp.ad := ADDR(cstrg);
    dos_reg.cx := attr;
    dos_func($3C00, ltemp.o, ltemp.s);
    dos_create := dos_reg.ax;       {return error codes, if any}
  END;

FUNCTION dos_mkdir (VAR ppath) : Integer;
  { create a sub-directory    NOTE use of untyped parameter }
  VAR
    cstrg : CharString;
    pstrg : CharString ABSOLUTE ppath;
    ltemp : LONG;
  BEGIN
    MakeCstring(pstrg,cstrg);
    ltemp.ad := ADDR(cstrg);
    dos_func($3900, ltemp.o, ltemp.s);
    IF dos_err THEN
      dos_mkdir := 0
    ELSE
      dos_mkdir := 1;
  END;

FUNCTION dos_delete (VAR pname) : Integer;
  { delete file    NOTE use of untyped parameter }
  VAR
    cstrg : CharString;
    pstrg : CharString ABSOLUTE pname;
    ltemp : LONG;
  BEGIN
    MakeCstring(pstrg,cstrg);
    ltemp.ad := ADDR(cstrg);
    dos_func($4100, ltemp.o, ltemp.s);
    dos_delete := dos_reg.ax;     {return error codes, if any}
  END;

FUNCTION dos_rename (VAR poname, pnname) : Integer;
  { rename file   NOTE use of untyped parameters }
  VAR
    costrg, cnstrg : CharString;
    postrg : CharString ABSOLUTE poname;
    pnstrg : CharString ABSOLUTE pnname;
    ltemp : LONG;
  BEGIN
    MakeCstring(pnstrg,cnstrg);
    ltemp.ad := ADDR(cnstrg);
    dos_reg.di := ltemp.o;
    dos_reg.es := ltemp.s;
    MakeCstring(postrg,costrg);
    ltemp.ad := ADDR(costrg);
    dos_func($5600, ltemp.o, ltemp.s);
    dos_rename := dos_reg.ax;       {return error codes, if any}
  END;

FUNCTION block_write (handle, icnt : Integer; buff_ptr : LONG): Integer;
  { write to a file }
  BEGIN
    dos_reg.cx := icnt;
    dos_reg.bx := handle;
    dos_func($4000,buff_ptr.o,buff_ptr.s);
    block_write := dos_reg.ax;         { return bytes actually written }
  END;

FUNCTION dos_write (handle : Integer; cnt : Real; buff_loc : ADDRESS) : Real;
  {write cnt bytes from memory at buff_loc to file indicated by handle }
  VAR
    block_size : Integer;
    buff_ptr : LONG;
    wt_cnt : Real;
  BEGIN
    buff_ptr.ad := buff_loc;
    block_size := MAXINT;
    wt_cnt := 0.0;
    dos_err := FALSE;
    WHILE (cnt>0) AND (NOT dos_err) DO
      BEGIN
        IF cnt>block_size THEN
          cnt := cnt - block_size
        ELSE
          BEGIN
            block_size := trunc(cnt);    { write last piece < 32K }
            cnt := 0.0;
          END;
        wt_cnt := wt_cnt + block_write(handle, block_size, buff_ptr);
        buff_ptr.s := buff_ptr.s + (block_size div 16);
        buff_ptr.o := buff_ptr.o + (block_size mod 16);
      END;
    dos_write := wt_cnt;
  END;

FUNCTION dos_chmod(VAR pname; func, attr : Integer) : Integer;
  { change file mode   }
  { func =$01 to set file attr,  =$00 to get file attr }
  { NOTE can't change volume label or subdirectory bits }
  VAR
    cstrg : CharString;
    pstrg : CharString ABSOLUTE pname;
    ltemp : LONG;
  BEGIN
    MakeCstring(pstrg,cstrg);
    ltemp.ad := ADDR(cstrg);
    dos_reg.cx := attr;
    dos_reg.ah := $43;
    dos_reg.al := func;
    dos_reg.dx := ltemp.o;
    dos_reg.ds := ltemp.s;
    call_dos;
    IF dos_err THEN
      dos_chmod := dos_reg.ax       {return error flags }
    ELSE
      dos_chmod := dos_reg.cx;      {return file's attr }
  END;

FUNCTION dos_setdt(handle, date, time : Integer) : Integer;
  { set file's date and time }
  BEGIN
    dos_reg.ax := $5701;
    dos_reg.bx := handle;
    dos_reg.cx := time;
    dos_reg.dx := date;
    call_dos;
    dos_setdt := dos_reg.ax;    {return error codes, if any}
  END;

FUNCTION dos_getdt(handle : Integer; VAR date, time : Integer) : Integer;
  { get file's date and time }
  BEGIN
    dos_reg.ax := $5700;
    dos_reg.bx := handle;
    call_dos;
    time := dos_reg.cx;
    date := dos_reg.dx;
    dos_getdt := dos_reg.ax;    {return error codes, if any}
  END;


end.

{ This is kinda naff, but I'm working on the principal that if anyone's }
{ got down this far they are prolly gonna be pretty knackered, and not  }
{ bother reading this.  I just want to say thanks to three people who   }
{ between them have made my life worth living over the past year...     }
{ They are known as doofus, TheNerd, and ognord, respectively, and they }
{ know who they are.  Thanks.                                           }
{                -- Cheese (Rob Mitchelmore)                            }