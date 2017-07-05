# Macros for building, deleting

AS=tasm
ASFLAGS=-m @asmlib.cfg

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

all: ctmouse.exe

ctmouse.exe: ctmouse.obj com2exe.exe
	$(LINK) $(LINKCOM) $*,$*.exe
	com2exe -s512 $*.exe $*.exe

ctmouse.obj: ctmouse.asm ctmouse.msg asmlib\*.* asmlib\bios\*.* \
		asmlib\convert\*.* asmlib\dos\*.* asmlib\hard\*.*
ctmouse.msg: ctm-en.msg
	copy ctm-en.msg ctmouse.msg>nul

	
# Clean up:

clean:
	-$(RM) ctmouse.msg
	-$(RM) *.obj
