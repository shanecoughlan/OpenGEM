# Macros for building, deleting

AS=tasm
ASFLAGS=-m @..\asmlib.cfg

LINK=tlink
LINKEXE=/x
LINKCOM=/x /t

RM=del


# Rules to build files

.asm.obj:
	$(AS) $(ASFLAGS) $*
.obj.com:
	$(LINK) $(LINKCOM) $*
.obj.exe:
	$(LINK) $(LINKEXE) $*


# Targets:

all: comtest.com protocol.com

comtest.com: comtest.obj
protocol.com: protocol.obj

comtest.obj: comtest.asm ..\asmlib\*.* ..\asmlib\bios\area0.def \
		..\asmlib\convert\digit.mac \
		..\asmlib\dos\io.mac ..\asmlib\dos\mem.mac \
		..\asmlib\hard\pic8259a.def ..\asmlib\hard\uart.def
protocol.obj: protocol.asm ..\asmlib\*.* ..\asmlib\bios\area0.def \
		..\asmlib\convert\digit.mac ..\asmlib\convert\byte2xa.inc \
		..\asmlib\dos\io.mac ..\asmlib\dos\mem.mac \
		..\asmlib\hard\pic8259a.def ..\asmlib\hard\uart.def


# Clean up:

clean:
	-$(RM) *.obj
