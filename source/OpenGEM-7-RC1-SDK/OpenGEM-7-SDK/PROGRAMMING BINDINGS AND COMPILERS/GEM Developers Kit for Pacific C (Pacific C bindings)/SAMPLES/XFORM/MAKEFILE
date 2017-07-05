include ../../makedefs.i

CFLAGS = -Zg $(STDINC)
CC = pacc

all:	../xform-l.app ../xform-s.app ../xform.rsc

../%.app:	%.app
	copy $< ..

../xform.rsc:	xform.rsc
	copy xform.rsc ..

xform-l.app:	xform.c xform.h
	$(CC) -Bl $(CFLAGS) -O$@ xform.c ../main-l.obj ../../lib/ppdgem-l.lib

xform-s.app:	xform.c xform.h
	$(CC) -Bs $(CFLAGS) -O$@ xform.c ../main-s.obj ../../lib/ppdgem-s.lib

tidy:
	erase *.obj
	erase *.bak
	erase *.sdb
	erase *.sym
	erase *.map
	erase *.obj
	erase xform-?.app

clean:	tidy
