copy smachine.h machine.h
masm tcs.asm,tcs.obj,NUL,NUL
masm accstart.asm,accstart.obj,NUL,NUL
lc -cw -v gembind
masm gemasm.asm,gemasm.obj,NUL,NUL
lc -cw -v vdibind
masm vdiasm.asm,vdiasm.obj,NUL,NUL
masm tcrtlasm.asm,tcrtlasm.obj,NUL,NUL
masm longasm.asm,longasm.obj,NUL,NUL
masm proend.asm,proend.obj,NUL,NUL
lc -dDESKACC=1 -cw -v hello
erase heloa32.exe
erase heloa32.map
erase heloa32.sym
link tcs+accstart+hello+gembind+gemasm+vdibind+vdiasm+tcrtlasm+longasm+proend,heloa32,heloa32/map,,
map2sym <heloa32.map >heloa32.sym
copy heloa32.exe \gemapps\gemsys\hello32.acc
