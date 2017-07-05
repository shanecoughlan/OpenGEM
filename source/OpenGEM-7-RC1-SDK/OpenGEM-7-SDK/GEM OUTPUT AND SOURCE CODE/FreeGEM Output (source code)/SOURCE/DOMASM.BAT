@echo off

echo. >> MASM.ERR
echo ****** %1 ****** >> MASM.ERR
echo (command: MASM %1;) >> MASM.ERR
echo ****** %1 ****** >> MASM.ERR

echo. >> MASM.ERR

masm %1; >> MASM.ERR
                       
