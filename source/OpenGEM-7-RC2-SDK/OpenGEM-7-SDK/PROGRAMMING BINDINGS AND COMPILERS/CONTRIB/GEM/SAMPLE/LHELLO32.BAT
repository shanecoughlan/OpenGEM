copy lmachine.h machine.h
masm ltcl.asm,ltcl.obj,NUL,NUL
masm lprostrt.asm,lprostrt.obj,NUL,NUL
lc -mLs -cw -v gembind
masm lgemasm.asm,lgemasm.obj,NUL,NUL
lc -dLARGE -mLs -cw -v vdibind
masm lvdiasm.asm,lvdiasm.obj,NUL,NUL
masm ltcrtlas.asm,ltcrtlas.obj,NUL,NUL
masm llongasm.asm,llongasm.obj,NUL,NUL
masm lproend.asm,lproend.obj,NUL,NUL
del lhello32.app
del lhello32.sym
del lhello32.map
lc -dDESKACC=0 -mLs -cw -v hello
link ltcl+lprostrt+hello+gembind+lgemasm+vdibind+lvdiasm+ltcrtlas+llongasm+lproend,lhello32,lhello32/map,,
ren lhello32.exe lhello32.app
lmap2sym <lhello32.map >lhello32.sym

