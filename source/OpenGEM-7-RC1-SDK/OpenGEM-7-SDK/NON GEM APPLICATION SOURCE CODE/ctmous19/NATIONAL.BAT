@echo off
if "%1"=="" goto syntax
if "%1"=="@" goto compile
if not "%2"=="" goto syntax
if exist ctm-%1.msg %0 @ %1 ctm-%1

echo Invalid language code!
echo.

:syntax
echo Syntax: national "lang code"
echo.
echo Where "lang code" is an "EN" for English edition or suffix
echo of one of other CTM-*.MSG file. Example:
echo.
echo %0 DE
goto end

:compile
if not exist %2\nul md %2
cd %2

copy ..\%3.msg ctmouse.msg>nul
call tasm -m @..\asmlib.cfg ..\ctmouse.asm
if not errorlevel 1 call tlink /t /x ctmouse.obj,ctmouse.exe
if not errorlevel 1 ..\com2exe -s512 ctmouse.exe ctmouse.exe
if exist ctmouse.msg del ctmouse.msg>nul
if exist ctmouse.obj del ctmouse.obj>nul

cd ..

:end
