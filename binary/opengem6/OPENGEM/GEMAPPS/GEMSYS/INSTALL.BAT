@ECHO OFF
CLS
REM  OpenGEM Install Release 2
REM
REM  Copyright (c) 2005, 2006 Opendawn
REM
REM  Comments or suggestions about this file to info@opendawn.com please.
REM  Help us make OpenGEM (and FreeGEM) better!
CLS

:INSTALL
CLS
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * OpenGEM has detected an application that is    *
ECHO               * waiting to be installed.                       *
ECHO               *                                                *
ECHO               *                                                *
ECHO               * Would you like to install the application now? *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
ECHO                           Please select an option...
CD GEMAPPS\GEMSYS\
choice /c:YN
if errorlevel 2 goto 2
if errorlevel 1 goto 1
:1
UNZIP -o C:\INSTALL.ZIP -d C:\
CD\
DEL INSTALL.ZIP
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * OpenGEM has installed the application.         *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
ECHO                       Please press any key to continue...
PAUSE
GOTO END
:2
GOTO SKIP

:SKIP
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * OpenGEM has not installed the application, but *
ECHO               * you can install it next time OpenGEM starts.   *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
ECHO                       Please press any key to continue...
PAUSE

:END