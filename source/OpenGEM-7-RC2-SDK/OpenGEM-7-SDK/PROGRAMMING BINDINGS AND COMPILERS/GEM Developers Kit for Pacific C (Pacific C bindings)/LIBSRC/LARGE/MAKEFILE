include ../../makedefs.i
#
CC = pacc
CFLAGS = -Bl -Zg -I../../INCLUDE -I.. $(STDINC)


OBJECTS = ems_all.obj ems_fp.obj ems_free.obj ems_fs.obj ems_map.obj ems_np.obj ems_rmap.obj \
	  ems_smap.obj ems_vers.obj emslib.obj ob_dofl.obj ob_dost.obj ob_dr_di.obj \
	  ob_g_txt.obj ob_isfl.obj ob_isst.obj ob_s_txt.obj ob_ud_di.obj ob_udfl.obj \
          ob_udst.obj ob_xywh.obj rc_copy.obj rc_equal.obj rc_gr2ar.obj rc_insid.obj \
          rc_inter.obj x_dux_sc.obj x_dux_xf.obj x_duy_sc.obj x_duy_xf.obj x_mul_di.obj \
          x_saspec.obj x_sxform.obj x_udx_sc.obj x_udx_xf.obj x_udx_yf.obj x_udy_sc.obj \
          x_ytox.obj xfmlib.obj formdo.obj ev_event.obj trimage.obj \
	  ppdd000.obj ppdd001.obj ppdd002.obj ppdd003.obj ppdd004.obj ppdd005.obj \
	  ppdd006.obj ppdd007.obj ppdd008.obj ppdd009.obj ppdd010.obj ppdd011.obj \
	  ppdd012.obj ppdd013.obj ppdd014.obj ppdd015.obj ppdd016.obj ppdd017.obj \
	  ppdd018.obj ppdd019.obj ppdd020.obj ppdd021.obj ppdd022.obj ppdd023.obj \
	  ppdd024.obj ppdd025.obj ppdd026.obj ppdd027.obj ppdd028.obj ppdfdraw.obj \
	  ppdg000.obj ppdg001.obj ppdg002.obj ppdg003.obj ppdg004.obj ppdg005.obj \
	  ppdg006.obj ppdg007.obj ppdg008.obj ppdg009.obj ppdg010.obj ppdg011.obj \
	  ppdg012.obj ppdg013.obj ppdg014.obj ppdg015.obj ppdg016.obj ppdg017.obj \
	  ppdg018.obj ppdg019.obj ppdg020.obj ppdg021.obj ppdg022.obj ppdg023.obj \
	  ppdg024.obj ppdg025.obj ppdg026.obj ppdg027.obj ppdg028.obj ppdg029.obj \
	  ppdg030.obj ppdg031.obj ppdg032.obj ppdg033.obj ppdg034.obj ppdg035.obj \
	  ppdg036.obj ppdg037.obj ppdg038.obj ppdg039.obj ppdg040.obj ppdg041.obj \
	  ppdg042.obj ppdg043.obj ppdg044.obj ppdg045.obj ppdg046.obj ppdg047.obj \
	  ppdg048.obj ppdg049.obj ppdg050.obj ppdg051.obj ppdg052.obj ppdg053.obj \
	  ppdg054.obj ppdg055.obj ppdg056.obj ppdg057.obj ppdg058.obj ppdg059.obj \
	  ppdg060.obj ppdg061.obj ppdg062.obj ppdg063.obj ppdg064.obj ppdg065.obj \
	  ppdg066.obj ppdg067.obj ppdg068.obj ppdg069.obj ppdg070.obj ppdg071.obj \
	  ppdg072.obj ppdg073.obj ppdg074.obj ppdg075.obj ppdg076.obj ppdg077.obj \
	  ppdg078.obj ppdg079.obj ppdg080.obj ppdg081.obj ppdg082.obj ppdg083.obj \
	  ppdg084.obj ppdg085.obj ppdg086.obj ppdg087.obj ppdg088.obj ppdg089.obj \
	  ppdg090.obj \
          ppdgcrt.obj ppdgem.obj  ppdhook.obj \
	  ppdldraw.obj ppdv000.obj ppdv001.obj ppdv002.obj ppdv003.obj ppdv004.obj \
	  ppdv005.obj ppdv006.obj ppdv007.obj ppdv008.obj ppdv009.obj ppdv010.obj \
	  ppdv011.obj ppdv012.obj ppdv013.obj ppdv014.obj ppdv015.obj ppdv016.obj \
	  ppdv017.obj ppdv018.obj ppdv019.obj ppdv020.obj ppdv021.obj ppdv022.obj \
	  ppdv023.obj ppdv024.obj ppdv025.obj ppdv026.obj ppdv027.obj ppdv028.obj \
	  ppdv029.obj ppdv030.obj ppdv031.obj ppdv032.obj ppdv033.obj ppdv034.obj \
	  ppdv035.obj ppdv036.obj ppdv037.obj ppdv038.obj ppdv039.obj ppdv040.obj \
	  ppdv041.obj ppdv042.obj ppdv043.obj ppdv044.obj ppdv045.obj ppdv046.obj \
	  ppdv047.obj ppdv048.obj ppdv049.obj ppdv050.obj ppdv051.obj ppdv052.obj \
	  ppdv053.obj ppdv054.obj ppdv055.obj ppdv056.obj ppdv057.obj ppdv058.obj \
	  ppdv059.obj ppdv060.obj ppdv061.obj ppdv062.obj ppdv063.obj ppdv064.obj \
	  ppdv065.obj ppdv066.obj ppdv067.obj ppdv068.obj ppdv069.obj ppdv070.obj \
	  ppdv071.obj ppdv072.obj ppdv073.obj ppdv074.obj ppdv075.obj ppdv076.obj \
	  ppdv077.obj ppdv078.obj ppdv079.obj ppdv080.obj ppdv081.obj ppdv082.obj \
	  ppdv083.obj ppdv084.obj ppdv085.obj ppdv086.obj ppdv087.obj ppdv088.obj \
	  ppdv089.obj ppdv090.obj ppdv091.obj ppdv092.obj ppdv093.obj ppdv094.obj \
	  ppdv095.obj ppdv096.obj ppdv097.obj ppdv098.obj ppdv099.obj ppdv100.obj \
	  ppdv101.obj ppdv102.obj ppdv103.obj ppdv104.obj ppdv105.obj ppdv106.obj \
	  ppdv107.obj ppdv108.obj ppdv109.obj ppdv110.obj ppdv111.obj ppdv112.obj \
	  ppdv113.obj ppdv114.obj ppdv115.obj ppdv116.obj ppdv117.obj ppdv118.obj \
	  ppdv119.obj ppdv120.obj ppdv121.obj ppdv122.obj ppdv123.obj ppdv124.obj \
	  ppdv125.obj ppdv126.obj ppdv127.obj ppdv128.obj ppdv129.obj ppdv130.obj \
	  ppdv131.obj ppdv132.obj ppdv133.obj ppdv134.obj ppdv135.obj ppdv136.obj \
          ppdv137.obj ppdv138.obj ppdvdi.obj ppdlptr.obj

all:	ppdgem-l.lib
#
# Because of the short DOS command line, I can't just say "libr r $@ $(OBJECTS)"
#
ppdgem-l.lib:	$(OBJECTS)
	erase ppdgem-l.lib
	libr r ppdgem-l.lib ems_all.obj ems_fp.obj ems_free.obj ems_fs.obj ems_map.obj ems_np.obj ems_rmap.obj 
	libr r ppdgem-l.lib ems_smap.obj ems_vers.obj emslib.obj ob_dofl.obj ob_dost.obj ob_dr_di.obj 
	libr r ppdgem-l.lib ob_g_txt.obj ob_isfl.obj ob_isst.obj ob_s_txt.obj ob_ud_di.obj ob_udfl.obj 
	libr r ppdgem-l.lib ob_udst.obj ob_xywh.obj rc_copy.obj rc_equal.obj rc_gr2ar.obj rc_insid.obj 
	libr r ppdgem-l.lib rc_inter.obj x_dux_sc.obj x_dux_xf.obj x_duy_sc.obj x_duy_xf.obj x_mul_di.obj 
	libr r ppdgem-l.lib x_saspec.obj x_sxform.obj x_udx_sc.obj x_udx_xf.obj x_udx_yf.obj x_udy_sc.obj 
	libr r ppdgem-l.lib x_ytox.obj xfmlib.obj formdo.obj ev_event.obj trimage.obj 
	libr r ppdgem-l.lib ppdd000.obj ppdd001.obj ppdd002.obj ppdd003.obj ppdd004.obj ppdd005.obj ppdd006.obj ppdd007.obj
	libr r ppdgem-l.lib ppdd008.obj ppdd009.obj ppdd010.obj ppdd011.obj ppdd012.obj ppdd013.obj ppdd014.obj ppdd015.obj
	libr r ppdgem-l.lib ppdd016.obj ppdd017.obj ppdd018.obj ppdd019.obj ppdd020.obj ppdd021.obj ppdd022.obj ppdd023.obj
	libr r ppdgem-l.lib ppdd024.obj ppdd025.obj ppdd026.obj ppdd027.obj ppdd028.obj ppdfdraw.obj ppdg000.obj ppdg001.obj
	libr r ppdgem-l.lib ppdg002.obj ppdg003.obj ppdg004.obj ppdg005.obj ppdg006.obj ppdg007.obj ppdg008.obj
	libr r ppdgem-l.lib ppdg009.obj ppdg010.obj ppdg011.obj ppdg012.obj ppdg013.obj ppdg014.obj ppdg015.obj ppdg016.obj
	libr r ppdgem-l.lib ppdg017.obj ppdg018.obj ppdg019.obj ppdg020.obj ppdg021.obj ppdg022.obj ppdg023.obj ppdg024.obj
	libr r ppdgem-l.lib ppdg025.obj ppdg026.obj ppdg027.obj ppdg028.obj ppdg029.obj ppdg030.obj ppdg031.obj ppdg032.obj
	libr r ppdgem-l.lib ppdg033.obj ppdg034.obj ppdg035.obj ppdg036.obj ppdg037.obj ppdg038.obj ppdg039.obj ppdg040.obj
	libr r ppdgem-l.lib ppdg041.obj ppdg042.obj ppdg043.obj ppdg044.obj ppdg045.obj ppdg046.obj ppdg047.obj ppdg048.obj
	libr r ppdgem-l.lib ppdg049.obj ppdg050.obj ppdg051.obj ppdg052.obj ppdg053.obj ppdg054.obj ppdg055.obj ppdg056.obj
	libr r ppdgem-l.lib ppdg057.obj ppdg058.obj ppdg059.obj ppdg060.obj ppdg061.obj ppdg062.obj ppdg063.obj ppdg064.obj
	libr r ppdgem-l.lib ppdg065.obj ppdg066.obj ppdg067.obj ppdg068.obj ppdg069.obj ppdg070.obj ppdg071.obj ppdg072.obj
	libr r ppdgem-l.lib ppdg073.obj ppdg074.obj ppdg075.obj ppdg076.obj ppdg076.obj ppdg077.obj ppdg078.obj
	libr r ppdgem-l.lib ppdg079.obj ppdg080.obj ppdg081.obj ppdg082.obj ppdg083.obj ppdg084.obj ppdg085.obj
	libr r ppdgem-l.lib ppdg086.obj ppdg087.obj ppdg088.obj ppdg089.obj ppdg090.obj 
	libr r ppdgem-l.lib ppdgcrt.obj ppdgem.obj ppdhook.obj ppdldraw.obj
	libr r ppdgem-l.lib ppdlptr.obj ppdv000.obj ppdv001.obj ppdv002.obj ppdv003.obj ppdv004.obj ppdv005.obj ppdv006.obj
	libr r ppdgem-l.lib ppdv007.obj ppdv008.obj ppdv009.obj ppdv010.obj ppdv011.obj ppdv012.obj ppdv013.obj ppdv014.obj
	libr r ppdgem-l.lib ppdv015.obj ppdv016.obj ppdv017.obj ppdv018.obj ppdv019.obj ppdv020.obj ppdv021.obj ppdv022.obj
	libr r ppdgem-l.lib ppdv023.obj ppdv024.obj ppdv025.obj ppdv026.obj ppdv027.obj ppdv028.obj ppdv029.obj ppdv030.obj
	libr r ppdgem-l.lib ppdv031.obj ppdv032.obj ppdv033.obj ppdv034.obj ppdv035.obj ppdv036.obj ppdv037.obj ppdv038.obj
	libr r ppdgem-l.lib ppdv039.obj ppdv040.obj ppdv041.obj ppdv042.obj ppdv043.obj ppdv044.obj ppdv045.obj ppdv046.obj
	libr r ppdgem-l.lib ppdv047.obj ppdv048.obj ppdv049.obj ppdv050.obj ppdv051.obj ppdv052.obj ppdv053.obj ppdv054.obj
	libr r ppdgem-l.lib ppdv055.obj ppdv056.obj ppdv057.obj ppdv058.obj ppdv059.obj ppdv060.obj ppdv061.obj ppdv062.obj
	libr r ppdgem-l.lib ppdv063.obj ppdv064.obj ppdv065.obj ppdv066.obj ppdv067.obj ppdv068.obj ppdv069.obj ppdv070.obj
	libr r ppdgem-l.lib ppdv071.obj ppdv072.obj ppdv073.obj ppdv074.obj ppdv075.obj ppdv076.obj ppdv077.obj ppdv078.obj
	libr r ppdgem-l.lib ppdv079.obj ppdv080.obj ppdv081.obj ppdv082.obj ppdv083.obj ppdv084.obj ppdv085.obj ppdv086.obj
	libr r ppdgem-l.lib ppdv087.obj ppdv088.obj ppdv089.obj ppdv090.obj ppdv091.obj ppdv092.obj ppdv093.obj ppdv094.obj
	libr r ppdgem-l.lib ppdv095.obj ppdv096.obj ppdv097.obj ppdv098.obj ppdv099.obj ppdv100.obj ppdv101.obj ppdv102.obj
	libr r ppdgem-l.lib ppdv103.obj ppdv104.obj ppdv105.obj ppdv106.obj ppdv107.obj ppdv108.obj ppdv109.obj ppdv110.obj
	libr r ppdgem-l.lib ppdv111.obj ppdv112.obj ppdv113.obj ppdv114.obj ppdv115.obj ppdv116.obj ppdv117.obj ppdv118.obj
	libr r ppdgem-l.lib ppdv119.obj ppdv120.obj ppdv121.obj ppdv122.obj ppdv123.obj ppdv124.obj ppdv125.obj ppdv126.obj
	libr r ppdgem-l.lib ppdv127.obj ppdv128.obj ppdv129.obj ppdv130.obj ppdv131.obj ppdv132.obj ppdv133.obj ppdv134.obj
	libr r ppdgem-l.lib ppdv135.obj ppdv136.obj ppdv137.obj ppdv138.obj ppdvdi.obj 

%.obj:	../%.c
	$(CC) -O$@ -C $(CFLAGS) $<

%.obj:	../%.as
	$(CC) -O$@ -C $(CFLAGS) $<

tidy:
	erase *.bak
	erase *.obj
	erase ppdgem-l.lib

clean:	tidy
