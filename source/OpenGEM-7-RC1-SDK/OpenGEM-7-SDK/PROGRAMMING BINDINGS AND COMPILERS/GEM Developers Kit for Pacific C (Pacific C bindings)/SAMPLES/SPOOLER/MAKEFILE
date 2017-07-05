include ../../makedefs.i

CFLAGS = -Zg $(STDINC)
CC = pacc

all:	../spool-l.app ../spool-s.app ../spooler.rsc

../%.app:	%.app
	copy $< ..

../spooler.rsc:	spooler.rsc
	copy spooler.rsc ..

spool-l.app:	spooler.c spooler.h 
	$(CC) -Bl $(CFLAGS) -O$@ $< ../../lib/ppdgem-l.lib

spool-s.app:	spooler.c spooler.h
	$(CC) -Bs $(CFLAGS) -O$@ $< ../../lib/ppdgem-s.lib

tidy:
	erase *.bak
	erase *.sdb
	erase *.sym
	erase *.map
	erase *.obj
	erase spool-?.app

clean:	tidy
