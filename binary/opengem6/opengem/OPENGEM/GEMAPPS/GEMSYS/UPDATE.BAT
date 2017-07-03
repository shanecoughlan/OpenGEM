@ECHO OFF
CLS
REM  OpenGEM Update Release 2
REM
REM  Copyright (c) 2005, 2006 Opendawn
REM
REM  Comments or suggestions about this file to info@opendawn.com please.
REM  Help us make OpenGEM (and FreeGEM) better!
CLS

:UPDATE
CLS
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * OpenGEM has detected an update that is         *
ECHO               * waiting to be installed.                       *
ECHO               *                                                *
ECHO               *                                                *
ECHO               * Would you like to install this update now?     *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
ECHO                           Please select an option...
CD GEMAPPS\GEMSYS\
choice /c:YN
if errorlevel 2 goto 2
if errorlevel 1 goto 1
:1
UNZIP -o C:\UPDATE.ZIP -d C:\
CD\
DEL UPDATE.ZIP
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * OpenGEM has installed the update.              *
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
ECHO               * OpenGEM has skipped the update, though you can *
ECHO               * install it next time OpenGEM starts.           *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
ECHO                       Please press any key to continue...
PAUSE

:END