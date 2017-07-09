copy lmachine.h machine.h
masm ltcl.asm,ltcl.obj,NUL,NUL
masm laccstrt.asm,laccstrt.obj,NUL,NUL
lc -mLs -cw -v gembind
masm lgemasm.asm,lgemasm.obj,NUL,NUL
lc -dLARGE -mLs -cw -v vdibind
masm lvdiasm.asm,lvdiasm.obj,NUL,NUL
masm ltcrtlas.asm,ltcrtlas.obj,NUL,NUL
masm llongasm.asm,llongasm.obj,NUL,NUL
masm lproend.asm,lproend.obj,NUL,NUL
del lheloa32.exe
del lheloa32.map
lc -dDESKACC=1 -mLs -cw -v hello
link ltcl+laccstrt+hello+gembind+lgemasm+vdibind+lvdiasm+ltcrtlas+llongasm+lproend,lheloa32,lheloa32/map,,
copy lheloa32.exe \gemapps\gemsys\lhello32.acc
