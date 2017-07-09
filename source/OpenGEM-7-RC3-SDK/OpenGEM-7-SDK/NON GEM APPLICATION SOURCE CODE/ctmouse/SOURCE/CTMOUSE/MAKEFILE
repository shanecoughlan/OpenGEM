# Macros for building, deleting ########################################

# AS=tasm -m @asmlib.cfg
# the following would require that you run SET ASMLIB=... first in a .bat:
# AS=jwasmd -mt @asmlib ... so we just hardcode the asmlib/ for includes
AS=jwasmd -mt 
LINKEXE=tlink /x
# using tlink /x /t would create COM but fails on jwasm made OBJ:
# it says that there would be data defined below initial CS:IP...
# *** LINKCOM=tlink /x /t

RM=del


# Rules to build files #################################################

.asm.obj:
	$(AS) $*
.obj.exe:
	$(LINKEXE) $*


# Targets ##############################################################

all: ctmouse.exe

ctmouse.exe: ctmouse.obj com2exe.exe
	$(LINKEXE) $*,$*.exe
	exe2bin $*.exe $*.bin
	com2exe -s512 $*.bin $*.exe

ctmouse.obj: ctmouse.asm ctmouse.msg asmlib\*.* asmlib\bios\*.* \
		asmlib\convert\*.* asmlib\dos\*.* asmlib\hard\*.*

# ctmouse.msg: ctm-en.msg
#	copy ctm-en.msg ctmouse.msg


# Clean up #############################################################

clean:
	-$(RM) ctmouse.msg
	-$(RM) *.obj
	-$(RM) ctmouse.bin
# -$(RM) ctmouse.com

