copy smachine.h machine.h
masm tcs.asm,tcs.obj,NUL,NUL
masm prostart.asm,prostart.obj,NUL,NUL
lc -cw -v gembind
masm gemasm.asm,gemasm.obj,NUL,NUL
lc -cw -v vdibind
masm vdiasm.asm,vdiasm.obj,NUL,NUL
masm tcrtlasm.asm,tcrtlasm.obj,NUL,NUL
masm longasm.asm,longasm.obj,NUL,NUL
masm proend.asm,proend.obj,NUL,NUL
lc -dDESKACC=0 -cw -v hello
erase hello32.app
erase hello32.map
erase hello32.sym
link tcs+prostart+hello+gembind+gemasm+vdibind+vdiasm+tcrtlasm+longasm+proend,hello32,hello32/map,,
map2sym <hello32.map >hello32.sym
ren hello32.exe hello32.app
