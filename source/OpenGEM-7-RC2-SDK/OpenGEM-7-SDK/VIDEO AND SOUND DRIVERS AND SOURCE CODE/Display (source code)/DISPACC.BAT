copy dispacc.h dispver.h
lc1 display -s
lc2 display -scode -v
erase display.acc
link86 dispacc[i]
copy display.acc \gemboot
copy display.rsc \gemboot
