include ../../makedefs.i

CFLAGS = -Zg $(STDINC)
CC = pacc

all:	../udef-l.app ../udef-s.app ../userdef.rsc

../%.app:	%.app
	copy $< ..

../userdef.rsc:	userdef.rsc
	copy userdef.rsc ..

udef-l.app:	userdef.c userdef.h 
	$(CC) -Bl $(CFLAGS) -O$@ $< ../../lib/ppdgem-l.lib

udef-s.app:	userdef.c userdef.h
	$(CC) -Bs $(CFLAGS) -O$@ $< ../../lib/ppdgem-s.lib

tidy:
	erase *.bak
	erase *.sdb
	erase *.sym
	erase *.map
	erase *.obj
	erase udef-?.app

clean:	tidy
