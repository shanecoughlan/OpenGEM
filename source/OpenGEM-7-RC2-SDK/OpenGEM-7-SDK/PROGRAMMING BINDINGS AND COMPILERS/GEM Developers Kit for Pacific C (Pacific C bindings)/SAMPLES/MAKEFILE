
include ../makedefs.i

TARGETS=demo-s.app   demo-l.app   \
	clock-s.app  clock-l.app  \
	xform-s.app  xform-l.app  \
	hello-s.app  hello-l.app  \
	hello-s.acc  hello-l.acc  \
	growbx-s.acc growbx-l.acc \
	fdtest-s.app fdtest-l.app \
	events-s.app events-l.app \
	spool-s.app  spool-l.app  \
	udef-s.app   udef-l.app   \
	fmem-s.acc   fmem-l.acc   \
	mgraf-s.app  mgraf-l.app  \
	mgraf-s.acc  mgraf-l.acc  \
	rscr-s.exe

CC = pacc
CFLAGS = -Zg $(STDINC) -I../include

MAIN = main-l.obj main-s.obj

all:	$(TARGETS)

main:	$(MAIN)

main-l.obj:	main.c
	$(CC) -Bl $(CFLAGS) -C $<
	copy main.obj $@
	erase main.obj

main-s.obj:	main.c
	$(CC) -Bs $(CFLAGS) -C $<
	copy main.obj $@
	erase main.obj

demo-s.app demo-l.app:	$(MAIN)
	cd demo
	make
	cd ..

clock-s.app clock-l.app: $(MAIN)
	cd clock
	make
	cd ..

events-s.app events-l.app: $(MAIN)
	cd events
	make
	cd ..

fdtest-s.app fdtest-l.app: $(MAIN)
	cd fdtest
	make
	cd ..

spool-s.app spool-l.app: $(MAIN)
	cd spooler
	make
	cd ..

udef-s.app udef-l.app: $(MAIN)
	cd userdef
	make
	cd ..


hello-s.app hello-l.app hello-s.acc hello-l.acc: $(MAIN)
	cd hello
	make
	cd ..

mgraf-s.app mgraf-l.app mgraf-s.acc mgraf-l.acc: 
	cd memgraf
	make
	cd ..

growbx-s.acc growbx-l.acc: $(MAIN)
	cd growbox
	make
	cd ..

fmem-s.acc fmem-l.acc: $(MAIN)
	cd freemem
	make
	cd ..

rscr-s.exe: $(MAIN)
	cd rscreate
	make
	cd ..

xform-s.app xform-l.app: $(MAIN)
	cd xform
	make
	cd ..

tidy:
	erase main-?.obj
	erase *.bak
	cd hello
	make tidy
	cd ..
	cd demo
	make tidy
	cd ..
	cd xform
	make tidy
	cd ..
	cd growbox
	make tidy
	cd ..
	cd clock
	make tidy
	cd ..
	cd events
	make tidy
	cd ..
	cd freemem
	make tidy
	cd ..
	cd memgraf
	make tidy
	cd ..
	cd fdtest
	make tidy
	cd ..
	cd spooler
	make tidy
	cd ..
	cd userdef
	make tidy
	cd ..
	cd rscreate
	make tidy
	cd ..

clean:
	erase main-?.obj
	erase *.rsc
	erase *.app
	erase *.acc
	erase *.exe
	erase *.bak
	cd hello
	make clean
	cd ..
	cd demo
	make clean
	cd ..
	cd xform
	make clean
	cd ..
	cd growbox
	make clean
	cd ..
	cd clock
	make clean
	cd ..
	cd events
	make clean
	cd ..
	cd freemem
	make clean
	cd ..
	cd memgraf
	make clean
	cd ..
	cd fdtest
	make clean
	cd ..
	cd spooler
	make clean
	cd ..
	cd userdef
	make clean
	cd ..
	cd rscreate
	make clean
	cd ..

