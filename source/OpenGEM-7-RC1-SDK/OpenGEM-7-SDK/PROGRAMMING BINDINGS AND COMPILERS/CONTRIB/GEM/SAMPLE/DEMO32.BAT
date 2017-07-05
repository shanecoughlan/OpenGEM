copy smachine.h machine.h
masm tcs.asm,tcs.obj,NUL,NUL
masm prostart.asm,prostart.obj,NUL,NUL
lc -B -cw -v gembind
masm gemasm.asm,gemasm.obj,NUL,NUL
lc -B -cw -v dosbind
masm dosasm.asm,dosasm.obj,NUL,NUL
lc -B -cw -v vdibind
masm vdiasm.asm,vdiasm.obj,NUL,NUL
lc -B -cw -v tcrtl
masm tcrtlasm.asm,tcrtlasm.obj,NUL,NUL
masm longasm.asm,longasm.obj,NUL,NUL
masm proend.asm,proend.obj,NUL,NUL
masm fardraw.asm,fardraw.obj,NUL,NUL
erase demo.obj
lc -B -cw -v demo    
erase demo32.app 
erase demo32.map 
erase demo32.sym 
link tcs+prostart+fardraw+demo+gembind+gemasm+dosbind+dosasm+vdibind+vdiasm+tcrtl+tcrtlasm+longasm+proend,demo32,demo32/m,,
map2sym <demo32.map >demo32.sym
ren demo32.exe demo32.app
