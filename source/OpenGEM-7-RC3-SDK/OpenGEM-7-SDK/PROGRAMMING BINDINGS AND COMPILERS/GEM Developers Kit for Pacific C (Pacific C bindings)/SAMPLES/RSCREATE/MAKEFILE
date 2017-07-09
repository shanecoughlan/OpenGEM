include ../../makedefs.i

#
# nb: Low warning level, so that the RCS-generated file EXAMPLE.RSH 
# doesn't cause warnings
#
CFLAGS = -Zg -W5 $(STDINC)
CC = pacc

all:	../rscr-s.exe

../%.exe:	%.exe
	copy $< ..

rscr-s.exe:	rscreate.c
	$(CC) -Bs $(CFLAGS) -O$@ $< ../../lib/ppdgem-s.lib

tidy:
	erase rscr-?.exe
	erase *.bak
	erase *.sdb
	erase *.sym
	erase *.map
	erase *.obj

clean:	tidy
