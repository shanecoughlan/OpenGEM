
copy lmachine.h machine.h
masm ltcl.asm,ltcl.obj,NUL,NUL
masm lprostrt.asm,lprostrt.obj,NUL,NUL
lc -B -cw -mls -v gembind
masm lgemasm.asm,lgemasm.obj,NUL,NUL
lc -B -cw -mls -v dosbind
masm ldosasm.asm,ldosasm.obj,NUL,NUL
lc -B -dLARGE -cw -mls -v vdibind
masm lvdiasm.asm,lvdiasm.obj,NUL,NUL
lc -B -cw -mls -v tcrtl
masm ltcrtlas.asm,ltcrtlas.obj,NUL,NUL
masm llongasm.asm,llongasm.obj,NUL,NUL
masm lproend.asm,lproend.obj,NUL,NUL
masm lfardraw.asm,lfardraw.obj,NUL,NUL
erase ldemo32.app
erase ldemo32.sym
erase ldemo32.map
lc -B -cw -mls -v demo
link @ldemolnk,ldemo32,ldemo32/map,,
ren ldemo32.exe ldemo32.app 
lmap2sym <ldemo32.map >ldemo32.sym
