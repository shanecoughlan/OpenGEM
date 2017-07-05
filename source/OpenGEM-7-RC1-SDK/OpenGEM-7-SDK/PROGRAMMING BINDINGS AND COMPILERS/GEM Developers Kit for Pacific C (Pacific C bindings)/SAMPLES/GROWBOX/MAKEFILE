include ../../makedefs.i

CFLAGS = -Zg $(STDINC)
CC = pacc

all:	../growbx-l.acc ../growbx-s.acc

../%.acc:	%.exe
	../../bin/exe2acc $< $@


growbx-l.exe:	growbox.c 
	$(CC) -Bl -DDESKACC $(CFLAGS) -Hgrowbx-l.sym -O$@ $< ../main-l.obj ../../lib/ppdgem-l.lib

growbx-s.exe:	growbox.c
	$(CC) -Bs -DESKACC $(CFLAGS) -Hgrowbx-s.sym -O$@ $< ../main-s.obj ../../lib/ppdgem-s.lib

tidy:
	erase growbx-?.exe
	erase *.bak
	erase *.sdb
	erase *.sym
	erase *.map
	erase *.obj

clean:	tidy
