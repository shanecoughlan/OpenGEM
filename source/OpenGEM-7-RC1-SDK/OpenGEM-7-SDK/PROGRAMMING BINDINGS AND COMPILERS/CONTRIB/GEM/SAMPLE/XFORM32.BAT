copy smachine.h machine.h
masm tcs.asm,tcs.obj,NUL,NUL
masm prostart.asm,prostart.obj,NUL,NUL
lc -cw -v xform 
lc -cw -v xlib 
lc -cw -v gembind 
masm gemasm.asm,gemasm.obj,NUL,NUL
lc -cw -v vdibind 
masm vdiasm.asm,vdiasm.obj,NUL,NUL
masm longasm.asm,longasm.obj,NUL,NUL
lc -cw -v tcrtl 
masm tcrtlasm.asm,tcrtlasm.obj,NUL,NUL
masm crmuldiv.asm,crmuldiv.obj,NUL,NUL
masm proend.asm,proend.obj,NUL,NUL
del xform32.app
del xform32.sym
del xform32.map
link tcs+prostart+xform+xlib+gembind+gemasm+vdibind+vdiasm+longasm+tcrtl+tcrtlasm+crmuldiv+proend,xform32,xform32/map,,
map2sym <xform32.map >xform32.sym
ren xform32.exe xform32.app
