include ../../makedefs.i

CFLAGS = -Zg $(STDINC)
CC = pacc

all:	../fmem-l.acc ../fmem-s.acc ../freemem.rsc

../%.acc:	%.exe
	../../bin/exe2acc $< $@

../%.rsc:	%.rsc
	copy $< ..


fmem-l.exe:	freemem.c 
	$(CC) -Bl -DDESKACC $(CFLAGS) -Hfmem-l.sym -O$@ $<  ../../lib/ppdgem-l.lib

fmem-s.exe:	freemem.c
	$(CC) -Bs -DESKACC $(CFLAGS) -Hfmem-s.sym -O$@ $< ../../lib/ppdgem-s.lib

tidy:
	erase fmem-?.exe
	erase *.bak
	erase *.sdb
	erase *.sym
	erase *.map
	erase *.obj

clean:	tidy
