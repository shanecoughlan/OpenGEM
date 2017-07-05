# Macros for building, deleting ########################################

AS=tasm -m @asmlib.cfg
LINKEXE=tlink /x
LINKCOM=tlink /x /t

RM=del


# Rules to build files #################################################

.asm.obj:
	$(AS) $*
.obj.com:
	$(LINKCOM) $*
.obj.exe:
	$(LINKEXE) $*


# Targets ##############################################################

all: ctmouse.exe

ctmouse.exe: ctmouse.obj com2exe.exe
	$(LINKCOM) $*,$*.exe
	com2exe -s512 $*.exe $*.exe

ctmouse.obj: ctmouse.asm ctmouse.msg asmlib\*.* asmlib\bios\*.* \
		asmlib\convert\*.* asmlib\dos\*.* asmlib\hard\*.*

ctmouse.msg: ctm-en.msg
	copy ctm-en.msg ctmouse.msg>nul


# Clean up #############################################################

clean:
	-$(RM) ctmouse.msg
	-$(RM) *.obj
