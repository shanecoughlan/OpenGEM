include makedefs.i

TARGETS=lib/ppdgem-l.lib lib/ppdgem-s.lib bin/exe2acc.exe samples

all:	$(TARGETS)

samples: bin/exe2acc.exe
	cd samples
	make
	cd ..

lib/ppdgem-l.lib lib/ppdgem-s.lib:
	cd libsrc
	make
	cd ..

bin/exe2acc.exe:
	cd exe2acc
	make
	cd ..

install:
	cd include
	make install
	cd ..
	cd lib
	make install
	cd ..
	cd bin
	make install
	cd ..

clean:
	cd libsrc
	make $@
	cd ..
	cd samples
	make $@
	cd ..
	cd exe2acc
	make $@
	cd ..
	cd bin
	make $@
	cd ..
	cd lib
	make $@ 
	cd ..

tidy:
	cd libsrc
	make $@
	cd ..
	cd samples
	make $@
	cd ..
	cd exe2acc
	make $@
	cd ..
	cd bin
	make $@
	cd ..
	cd lib
	make $@ 
	cd ..

zip:
	erase ppdgem.zip
	pkzip -rP ppdgem.zip *.*