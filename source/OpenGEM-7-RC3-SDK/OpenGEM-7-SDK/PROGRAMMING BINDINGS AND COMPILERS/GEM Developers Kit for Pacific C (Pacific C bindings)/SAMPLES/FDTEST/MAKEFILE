include ../../makedefs.i

CFLAGS = -Zg $(STDINC)
CC = pacc

all:	../fdtest-l.app ../fdtest-s.app ../fdtest.rsc

../%.app:	%.app
	copy $< ..

../fdtest.rsc:	fdtest.rsc
	copy fdtest.rsc ..

fdtest-l.app:	fdtest.c fdtest.h 
	$(CC) -Bl $(CFLAGS) -O$@ $< ../../lib/ppdgem-l.lib

fdtest-s.app:	fdtest.c fdtest.h
	$(CC) -Bs $(CFLAGS) -O$@ $< ../../lib/ppdgem-s.lib

tidy:
	erase *.bak
	erase *.sdb
	erase *.sym
	erase *.map
	erase *.obj
	erase fdtest-?.app

clean:	tidy
