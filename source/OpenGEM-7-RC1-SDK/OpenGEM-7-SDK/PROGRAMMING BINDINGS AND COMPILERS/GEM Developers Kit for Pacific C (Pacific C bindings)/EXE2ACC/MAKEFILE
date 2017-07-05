include ../makedefs.i

CFLAGS = -Zg -O $(STDINC)

all:	../bin/exe2acc.exe

../bin/exe2acc.exe:	exe2acc.exe
	copy exe2acc.exe ..\bin

exe2acc.exe:	exe2acc.c
	pacc $(CFLAGS) $<

tidy:
	erase exe2acc.exe

clean:	tidy

