@ECHO OFF
CLS
REM  OpenGEM Update Release 3
REM
REM  Copyright (c) 2005-2017 Shane Coughlan (Opendawn)
REM
REM  Comments or suggestions about this file to shane@opendawn.com please.
REM  Help us make OpenGEM (and FreeGEM) better!
CLS

:UPDATE
CLS
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * OpenGEM has detected that an update is         *
ECHO               * waiting to be installed.                       *
ECHO               *                                                *
ECHO               *                                                *
ECHO               * Would you like to install this update now?     *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
ECHO                           Please choose an option...
ECHO.
ECHO                           [ Y = Yes ]     [ N = No ]
CD GEMAPPS\GEMSYS\
choice /c:YN
if errorlevel 2 goto 2
if errorlevel 1 goto 1
:1
UNZIP -o C:\UPDATE.ZIP -d C:\
DEL C:\UPDATE.ZIP
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * OpenGEM has installed the update.              *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
PAUSE
GOTO END
:2
GOTO SKIP

:SKIP
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * OpenGEM has skipped the update. You can choose *
ECHO               * to install it the next time OpenGEM starts.    *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
PAUSE

:END