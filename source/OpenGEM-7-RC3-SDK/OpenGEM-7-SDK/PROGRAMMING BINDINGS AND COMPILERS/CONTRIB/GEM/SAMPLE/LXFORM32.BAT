copy lmachine.h machine.h
masm ltcl.asm,ltcl.obj,NUL,NUL
masm lprostrt.asm,lprostrt.obj,NUL,NUL
lc -cw -mls -v xform 
lc -cw -mls -v xlib 
lc -cw -mls -v gembind 
masm lgemasm.asm,lgemasm.obj,NUL,NUL;
lc -dLARGE -cw -mls -v vdibind 
masm lvdiasm.asm,lvdiasm.obj,NUL,NUL;
masm llongasm.asm,llongasm.obj,NUL,NUL
lc -cw -mls -v tcrtl 
masm ltcrtlas.asm,ltcrtlas.obj,NUL,NUL
masm lcmuldiv.asm,lcmuldiv.obj,NUL,NUL
masm lproend.asm,lproend.obj,NUL,NUL
del lxform32.app
del lxform32.sym
del lxform32.map 
link @lxfrmlnk,lxform32,lxform32/m,,
lmap2sym <lxform32.map >lxform32.sym 
ren lxform32.exe lxform32.app