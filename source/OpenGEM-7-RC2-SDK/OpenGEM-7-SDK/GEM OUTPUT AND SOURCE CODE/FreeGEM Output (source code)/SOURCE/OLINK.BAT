erase output.app
erase output.lst
set OBJ=..\obj
plink86 @output > olink.err
type olink.err
msym < output.lst > output.sym