#
#	make.PRO  (IBM Proprinter)
#
#-----------------------
# Build the whole driver
#-----------------------
pdpro8.eps:	obj\devdep.obj		\
	    	obj\buf.obj		\
	    	obj\pentry.obj		\
	    	obj\textblt.obj		\
	    	obj\image.obj		\
	    	obj\jmptbl.obj		\
		obj\monoprin.obj	\
	    	obj\monout.obj		\
	    	obj\text.obj		\
	    	obj\alphtext.obj		\
	    	..\common\obj\devtext.obj	\
		..\common\obj\dvtasm.obj	\
	    	..\common\obj\io.obj		\
		..\common\obj\imageasm.obj	\
	    	..\common\obj\enddseg.obj	\
	    	..\common\obj\isin.obj		\
		..\styles\120x144.obj		\
		pdpro8.inp
	cd obj
	copy ..\pdpro8.inp/v
	copy ..\..\common\obj\*.*/v
	copy ..\..\styles\120x144.obj/v
	link /dosseg /map @pdpro8.inp, pdpro8.eps, pdpro8.sym;
	mv pdpro8.* ..
	del io.obj
	del imageasm.obj
	del enddseg.obj
	del isin.obj
	del 120x144.obj
	cd ..\..\common
	del externs.a86
	del devinfo.a86
	del dvtasm.a86
	del devtext.c
	del gsxdef.h
	cd ..\pro
	del *.a86

#------------------------
# Device dependent files 
#------------------------
obj\devdep.obj: devdep.pro	\
	    	devdata.ehi	\
		externs.ehi	\
		..\common\devind.a86
	copy devdep.pro *.a86/v
	copy devdata.ehi *.a86/v
	copy externs.ehi *.a86/v
	copy ..\common\devind.a86/v
	rasm86 DEVDEP $pz sz
	mv devdep.obj obj

..\common\obj\dvtasm.obj:	..\common\dvtasm.bit
	cd ..\common
	copy dvtasm.bit *.a86/v
	rasm86 DVTASM $pz sz
	mv dvtasm.obj obj
	cd ..\pro

..\styles\120x144.obj:	..\styles\120x144.a86
	cd ..\styles
	rasm86 120x144 $pz sz
	cd ..\pro

..\common\obj\devtext.obj: 	..\common\devtext.bit	\
		 		..\common\portab.h	\
				..\common\fontdef.h	\
				..\common\gsxextrn.h
	cd ..\common
	copy devtext.bit *.c/v
	hc devtext -mm Compact
	mv devtext.obj obj
	cd ..\pro

#------------------------
# Common code RASM files
#------------------------
..\common\obj\io.obj: ..\common\io.a86
	cd ..\common
	rasm86 io $pz sz
	mv io.obj obj
	cd ..\pro

obj\buf.obj: 	..\common\buf.a86	\
		externs.ehi
	cd ..\common
	copy ..\pro\externs.ehi *.a86/v
	rasm86 buf $pz sz
	mv buf.obj ..\pro\obj
	cd ..\pro

obj\pentry.obj: ..\common\pentry.a86	\
		externs.ehi	\
		devinfo.pro
	cd ..\common
	copy ..\pro\externs.ehi *.a86/v
	copy ..\pro\devinfo.pro *.a86/v
	rasm86 pentry $pz sz
	mv pentry.obj ..\pro\obj
	cd ..\pro

obj\textblt.obj: ..\common\textblt.a86	\
		..\common\fhinc.a86	\
		externs.ehi	
	cd ..\common
	copy ..\pro\externs.ehi *.a86/v
	rasm86 textblt $pz sz
	mv textblt.obj ..\pro\obj
	cd ..\pro

..\common\obj\imageasm.obj:	..\common\imageasm.a86
	cd ..\common
	rasm86 imageasm $pz sz
	mv imageasm.obj obj
	cd ..\pro

..\common\obj\enddseg.obj: ..\common\enddseg.a86
	cd ..\common
	rasm86 enddseg $pz sz
	mv enddseg.obj obj
	cd ..\pro

#---------------------
# Common code C files
#---------------------
obj\monoprin.obj:	..\common\monoprin.c	\
			..\common\portab.h	\
			gsxdef.pro
	cd ..\common
	copy ..\pro\gsxdef.pro gsxdef.h/v
	hc monoprin
	mv monoprin.obj ..\pro\obj
	cd ..\pro

..\common\obj\isin.obj: ..\common\isin.c	\
			..\common\portab.h
	cd ..\common
	hc isin
	mv isin.obj obj
	cd ..\pro

obj\jmptbl.obj:	..\common\jmptbl.c	\
		..\common\portab.h	\
		..\common\gsxextrn.h	\
		..\common\jmptbl.h	\
		gsxdef.pro
	cd ..\common
	copy ..\pro\gsxdef.pro gsxdef.h/v
	hc jmptbl
	mv jmptbl.obj ..\pro\obj
	cd ..\pro

obj\monout.obj:	..\common\monout.c	\
		..\common\portab.h	\
		..\common\gsxextrn.h	\
		gsxdef.pro
	cd ..\common
	copy ..\pro\gsxdef.pro gsxdef.h/v
	hc monout
	mv monout.obj ..\pro\obj
	cd ..\pro

obj\text.obj: 	..\common\text.c	\
		..\common\portab.h	\
		..\common\fontdef.h	\
		..\common\gsxextrn.h	\
		gsxdef.pro
	cd ..\common
	copy ..\pro\gsxdef.pro gsxdef.h/v
	hc text -mm Compact
	mv text.obj ..\pro\obj
	cd ..\pro

obj\image.obj: 	..\common\image.c	\
		..\common\portab.h	\
		gsxdef.pro
	cd ..\common
	copy ..\pro\gsxdef.pro gsxdef.h/v
	hc image -mm Compact
	mv image.obj ..\pro\obj
	cd ..\pro

obj\alphtext.obj:	..\common\alphtext.c	\
			..\common\portab.h	\
			gsxdef.pro
	cd ..\common
	copy ..\pro\gsxdef.pro gsxdef.h/v
	hc alphtext -mm Compact
	mv alphtext.obj ..\pro\obj
	cd ..\pro

