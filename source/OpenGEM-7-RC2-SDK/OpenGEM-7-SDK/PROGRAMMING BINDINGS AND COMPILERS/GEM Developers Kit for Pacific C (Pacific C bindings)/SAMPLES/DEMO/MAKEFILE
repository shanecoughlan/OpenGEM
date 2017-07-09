include ../../makedefs.i

CFLAGS = -Zg $(STDINC)
CC = pacc

all:	../demo-l.app ../demo-s.app ../demo.rsc

../%.app:	%.app
	copy $< ..

../demo.rsc:	demo.rsc
	copy demo.rsc ..

demo-l.app:	demo.c demo.h 
	$(CC) -Bl $(CFLAGS) -O$@ $< ../main-l.obj ../../lib/ppdgem-l.lib

demo-s.app:	demo.c demo.h
	$(CC) -Bs $(CFLAGS) -O$@ $< ../main-s.obj ../../lib/ppdgem-s.lib

tidy:
	erase *.bak
	erase *.sdb
	erase *.sym
	erase *.map
	erase *.obj
	erase demo-?.app

clean:	tidy
