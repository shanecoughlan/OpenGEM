#
# Location of Pacific libraries
#
include ../makedefs.i

LIBDIR=$(CCDIR)\LIB

SOURCE = ems_all.c ems_fp.c ems_free.c ems_fs.c ems_map.c ems_np.c \
	 ems_rmap.c ems_smap.c ems_vers.c emslib.c ev_event.c formdo.c \
	 ob_dofl.c ob_dost.c ob_dr_di.c ob_g_txt.c ob_isfl.c ob_isst.c \
	 ob_s_txt.c ob_ud_di.c ob_udfl.c ob_udst.c ob_xywh.c ppdd000.c \
	 ppdd001.c ppdd002.c ppdd003.c ppdd004.c ppdd005.c ppdd006.c \
	 ppdd007.c ppdd008.c ppdd009.c ppdd010.c ppdd011.c ppdd012.c \
	 ppdd013.c ppdd014.c ppdd015.c ppdd016.c ppdd017.c ppdd018.c \
	 ppdd019.c ppdd020.c ppdd021.c ppdd022.c ppdd023.c ppdd024.c \
	 ppdd025.c ppdd026.c ppdd027.c ppdd028.c ppdfdraw.c ppdg000.c \
	 ppdg001.c ppdg002.c ppdg003.c ppdg004.c ppdg005.c ppdg006.c \
	 ppdg007.c ppdg008.c ppdg009.c ppdg010.c ppdg011.c ppdg012.c \
	 ppdg013.c ppdg014.c ppdg015.c ppdg016.c ppdg017.c ppdg018.c \
	 ppdg019.c ppdg020.c ppdg021.c ppdg022.c ppdg023.c ppdg024.c \
	 ppdg025.c ppdg026.c ppdg027.c ppdg028.c ppdg029.c ppdg030.c \
	 ppdg031.c ppdg032.c ppdg033.c ppdg034.c ppdg035.c ppdg036.c \
	 ppdg037.c ppdg038.c ppdg039.c ppdg040.c ppdg041.c ppdg042.c \
	 ppdg043.c ppdg044.c ppdg045.c ppdg046.c ppdg047.c ppdg048.c \
	 ppdg049.c ppdg050.c ppdg051.c ppdg052.c ppdg053.c ppdg054.c \
	 ppdg055.c ppdg056.c ppdg057.c ppdg058.c ppdg059.c ppdg060.c \
	 ppdg061.c ppdg062.c ppdg063.c ppdg064.c ppdg065.c ppdg066.c \
	 ppdg067.c ppdg068.c ppdg069.c ppdg070.c ppdg071.c ppdg072.c \
	 ppdg073.c ppdg074.c ppdg075.c ppdg076.c ppdg077.c ppdg078.c \
	 ppdg079.c ppdg080.c ppdg081.c ppdg082.c ppdg083.c ppdg084.c \
	 ppdg085.c ppdg086.c ppdg087.c ppdg088.c ppdg089.c ppdg090.c \
	 ppdgcrt.c ppdgem.c ppdhook.c ppdldraw.as ppdlptr.c ppdsdraw.as \
	 ppdv000.c ppdv001.c ppdv002.c ppdv003.c ppdv004.c ppdv005.c \
	 ppdv006.c ppdv007.c ppdv008.c ppdv009.c ppdv010.c ppdv011.c \
	 ppdv012.c ppdv013.c ppdv014.c ppdv015.c ppdv016.c ppdv017.c \
	 ppdv018.c ppdv019.c ppdv020.c ppdv021.c ppdv022.c ppdv023.c \
	 ppdv024.c ppdv025.c ppdv026.c ppdv027.c ppdv028.c ppdv029.c \
	 ppdv030.c ppdv031.c ppdv032.c ppdv033.c ppdv034.c ppdv035.c \
	 ppdv036.c ppdv037.c ppdv038.c ppdv039.c ppdv040.c ppdv041.c \
	 ppdv042.c ppdv043.c ppdv044.c ppdv045.c ppdv046.c ppdv047.c \
	 ppdv048.c ppdv049.c ppdv050.c ppdv051.c ppdv052.c ppdv053.c \
	 ppdv054.c ppdv055.c ppdv056.c ppdv057.c ppdv058.c ppdv059.c \
	 ppdv060.c ppdv061.c ppdv062.c ppdv063.c ppdv064.c ppdv065.c \
	 ppdv066.c ppdv067.c ppdv068.c ppdv069.c ppdv070.c ppdv071.c \
	 ppdv072.c ppdv073.c ppdv074.c ppdv075.c ppdv076.c ppdv077.c \
	 ppdv078.c ppdv079.c ppdv080.c ppdv081.c ppdv082.c ppdv083.c \
	 ppdv084.c ppdv085.c ppdv086.c ppdv087.c ppdv088.c ppdv089.c \
	 ppdv090.c ppdv091.c ppdv092.c ppdv093.c ppdv094.c ppdv095.c \
	 ppdv096.c ppdv097.c ppdv098.c ppdv099.c ppdv100.c ppdv101.c \
	 ppdv102.c ppdv103.c ppdv104.c ppdv105.c ppdv106.c ppdv107.c \
	 ppdv108.c ppdv109.c ppdv110.c ppdv111.c ppdv112.c ppdv113.c \
	 ppdv114.c ppdv115.c ppdv116.c ppdv117.c ppdv118.c ppdv119.c \
	 ppdv120.c ppdv121.c ppdv122.c ppdv123.c ppdv124.c ppdv125.c \
	 ppdv126.c ppdv127.c ppdv128.c ppdv129.c ppdv130.c ppdv131.c \
	 ppdv132.c ppdv133.c ppdv134.c ppdv135.c ppdv136.c ppdv137.c \
	 ppdv138.c ppdvdi.c rc_copy.c rc_equal.c rc_gr2ar.c \
	 rc_insid.c rc_inter.c trimage.c x_dux_sc.c x_dux_xf.c x_duy_sc.c \
	 x_duy_xf.c x_mul_di.c x_saspec.c x_sxform.c x_udx_sc.c x_udx_xf.c \
	 x_udx_yf.c x_udy_sc.c x_ytox.c xfmlib.c


all:	../lib/ppdgem-l.lib ../lib/ppdgem-s.lib

../lib/ppdgem-l.lib: large/ppdgem-l.lib 
	copy large\ppdgem-l.lib ..\lib\ppdgem-l.lib

../lib/ppdgem-s.lib: small/ppdgem-s.lib
	copy small\ppdgem-s.lib ..\lib\ppdgem-s.lib

large/ppdgem-l.lib: $(SOURCE)
	cd large
	make
	copy ppdgem-l.lib ..\..\lib
	cd ..

small/ppdgem-s.lib: $(SOURCE)
	cd small
	make
	copy ppdgem-s.lib ..\..\lib
	cd ..

tidy:
	erase *.bak
	cd large
	make tidy
	cd ..
	cd small
	make tidy
	cd ..

clean:	
	erase *.bak
	cd large
	make clean
	cd ..
	cd small
	make clean
	cd ..
