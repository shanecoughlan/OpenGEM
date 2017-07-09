@echo off
if exist ctm-%1.msg goto makeone
if not "%1"=="" goto syntax
for %%i in (BR DE ES FR HU IT LV NL PL PT SK EN) do call %0 %%i
goto done

:makeone
echo building ctm-%1.exe...
make clean
copy ctm-%1.msg ctmouse.msg
make ctmouse.exe
del ctm-%1.exe
ren ctmouse.exe ctm-%1.exe
goto done

:syntax
echo Usage: %0 LANGUAGE builds ctm-LANGUAGE.exe using ctm-LANGUAGE.msg
echo Running %0 without options builds all default language ctm-*.exe

:done
