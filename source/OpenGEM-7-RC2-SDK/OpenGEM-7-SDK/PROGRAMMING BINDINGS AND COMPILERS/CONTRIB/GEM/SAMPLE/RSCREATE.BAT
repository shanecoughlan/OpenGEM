copy smachine.h machine.h
masm tcs.asm,tcs.obj,NUL,NUL
lc -v -cw dosbind
masm dosasm.asm,dosasm.obj,NUL,NUL
lc -cw -v tcrtl
masm tcrtlasm.asm,tcrtlasm,NUL,NUL
masm longasm.asm,longasm.obj,NUL,NUL
masm proend.asm,proend.obj,NUL,NUL
lc -cw -v rscreate
erase rscreate.exe
link tcs+rscreate+tcrtl+tcrtlasm+dosbind+dosasm+longasm+proend,rscreate,rscreate/m,,
map2sym <rscreate.map >rscreate.sym
rscreate
