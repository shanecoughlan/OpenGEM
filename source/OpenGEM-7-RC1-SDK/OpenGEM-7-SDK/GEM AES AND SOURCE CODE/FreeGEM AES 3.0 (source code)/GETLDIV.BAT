@echo off
rem
rem Extract LDIV.OBJ and LMUL.OBJ from the Pacific C runtime. The 
rem code generator uses these modules for 32-bit arithmetic.
rem
rem Alter your LIBDIR to taste.
rem
set LIBDIR=D:\TOOLS\C\LIB
libr x %LIBDIR%\86--dsc.lib ldiv.obj lmul.obj
