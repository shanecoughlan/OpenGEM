include ../../makedefs.i

CFLAGS = -Zg $(STDINC)
CC = pacc

all:	../clock-l.app ../clock-s.app ../clock-l.acc ../clock-s.acc 

../%.app:	%.app
	copy $< ..

../%.acc:	%.exe
	../../bin/exe2acc $< $@


clock-l.app:	clock.c
	$(CC) -Bl $(CFLAGS) -O$@ $<  ../../lib/ppdgem-l.lib

clock-s.app:	clock.c
	$(CC) -Bs $(CFLAGS) -O$@ $< ../../lib/ppdgem-s.lib


clock-l.exe:	clock.c 
	$(CC) -Bl -DDESKACC $(CFLAGS) -Hclock-l.sym -O$@ $<  ../../lib/ppdgem-l.lib

clock-s.exe:	clock.c
	$(CC) -Bs -DESKACC $(CFLAGS) -Hclock-s.sym -O$@ $< ../../lib/ppdgem-s.lib

tidy:
	erase clock-?.exe
	erase clock-?.app
	erase *.bak
	erase *.sdb
	erase *.sym
	erase *.map
	erase *.obj

clean:	tidy
