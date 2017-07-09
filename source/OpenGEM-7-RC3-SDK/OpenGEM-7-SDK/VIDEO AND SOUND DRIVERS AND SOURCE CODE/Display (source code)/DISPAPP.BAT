copy dispapp.h dispver.h
lc1 display -s
lc2 display  -scode -v
erase display.exe
link86 dispapp[i]
copy display.exe \gemapps\display.app
copy display.rsc \gemapps
