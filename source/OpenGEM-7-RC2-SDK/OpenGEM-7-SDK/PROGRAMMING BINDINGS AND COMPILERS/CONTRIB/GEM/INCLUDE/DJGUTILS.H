/* The following functions take an object tree in normal 32-bit space, 
 * thrust it down into 16-bit space, allow GEM to alter it, and pull it
 * back.
 *
 * Some important caveats are:
 *
 *   No lines of text longer than 79 characters (80 including the 
 *   terminating 0).
 *   No INDIRECT objects. If INDIRECT objects are necessary, then
 *   ob_spec must point at a structure starting:
 *       LONG real_ob_spec;
 *       LONG structure_len;
 *       (other words if necessary).
 *   
 *
 */

WORD dj_menu_bar(OBJECT *tree, WORD showit);
WORD dj_menu_icheck(WORD itemnum, WORD checkit);
WORD dj_menu_ienable(WORD itemnum, WORD enableit);
WORD dj_menu_tnormal(WORD titlenum, WORD normalit);
WORD dj_menu_text(WORD inum, LPBYTE ptext);
WORD dj_objc_add(OBJECT *tree, WORD parent, WORD child);
WORD dj_objc_delete(OBJECT *tree, WORD delob);
WORD dj_objc_draw(OBJECT *tree, WORD drawob, WORD depth, WORD xc, WORD yc, WORD wc, WORD hc);
WORD dj_objc_find(OBJECT *tree, WORD startob, WORD depth, WORD mx, WORD my);
WORD dj_objc_offset(OBJECT *tree, WORD obj, WORD *poffx, WORD *poffy);
WORD dj_objc_edit(OBJECT *tree, WORD obj, WORD inchar, WORD *idx, WORD kind);
WORD dj_objc_change(OBJECT *tree, WORD drawob, WORD depth, WORD xc, WORD yc, 
		 WORD wc, WORD hc, WORD newstate, WORD redraw);
WORD dj_form_do(OBJECT *tree, WORD start);
WORD dj_form_center(OBJECT *tree, WORD *pcx, WORD *pcy, WORD *pcw, WORD *pch);
WORD dj_form_keybd(OBJECT *tree, WORD obj, WORD nxt_obj, WORD thechar, 
		WORD *pnxt_obj, WORD *pchar);
WORD dj_form_button(OBJECT *tree, WORD obj, WORD clks, WORD *pnxt_obj);
WORD dj_rsrc_treeaddr(WORD rsid, OBJECT **paddr);
WORD dj_rsrc_obfix(OBJECT *tree, WORD obj);

VOID dj_vro_cpyfm( WORD handle, WORD wr_mode, WORD xy[], MFDB *srcMFDB, MFDB *desMFDB );
VOID dj_vr_trnfm(  WORD handle, MFDB *srcMFDB, MFDB *desMFDB );
VOID dj_vrt_cpyfm( WORD handle, WORD wr_mode, WORD xy[],
                MFDB *srcMFDB, MFDB *desMFDB, WORD *index );
