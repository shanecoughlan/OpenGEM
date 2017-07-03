@ECHO OFF
CLS
REM  OpenGEM Release 6
REM  Copyright (c) 2001, 2006 Opendawn
REM
REM  This is the OpenGEM main startup file.  It's not as difficult to use
REM  as it may appear, so take some time to read through it.  
REM
REM  Comments or suggestions about this file to info@opendawn.com please.
REM  Help us make OpenGEM (and FreeGEM) better!
CLS

REM STARTING THE MOUSE DRIVER
REM To start the mouse driver remove the REM command on all the lines in this block apart from this one
REM :SET-MOUSE
REM CD GEMAPPS\GEMSYS
REM CTMOUSE

REM CHECKING FOR NEW APPLICATIONS TO INSTALL
IF EXIST INSTALL.ZIP CALL C:\GEMAPPS\GEMSYS\INSTALL.BAT

REM CHECKING FOR NEW UPDATES TO INSTALL
IF EXIST UPDATE.ZIP CALL C:\GEMAPPS\GEMSYS\UPDATE.BAT

REM LOADING OPENGEM
:LOADGEM
CD GEMAPPS\GEMSYS
ECHO OFF
REM Load GEM
GEMVDI %1 %2 %3
CLS

REM THE OPENGEM SHUTDOWN MENU
:SHUTDOWN
C:
CLS
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * What would you like to do now?                 *
ECHO               *                                                *
ECHO               * 1 - Restart OpenGEM                            *
ECHO               *                                                *
ECHO               * 2 - Return to DOS                              *
ECHO               *                                                *
ECHO               * 3 - Reboot the computer                        *
ECHO               *                                                *
ECHO               * 4 - Shutdown the computer                      *
ECHO               *     (This only works on ATX computers)         *
ECHO               *                                                *
ECHO               *                                                *
ECHO               *                                                *
ECHO               * 5 - Help                                       *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
ECHO                           Please select an option...
CD GEMAPPS\GEMSYS\
choice /c:12345
if errorlevel 5 goto HELP
if errorlevel 4 goto 4
if errorlevel 3 goto 3
if errorlevel 2 goto 2
if errorlevel 1 goto 1
:1
CLS
ECHO OFF
GOTO LOADGEM
:2
CLS
ECHO OFF
GOTO END
:3
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
SHUTDOWN R
GOTO END
:4
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
SHUTDOWN S
GOTO END

REM OPENGEM STARTUP HELP
:HELP
CLS
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * OpenGEM has several options when you exit the  *
ECHO               * graphical user interface.                      *
ECHO               *                                                *
ECHO               * You can restart OpenGEM.  This is something    *
ECHO               * that you may want to do if the computer is     *
ECHO               * slow or you have changed the configuration.    *
ECHO               *                                                *
ECHO               * You can return to the DOS command prompt, and  *
ECHO               * leave OpenGEM altogether.                      *
ECHO               *                                                *
ECHO               * You can reboot the computer.  This can resolve *
ECHO               * many issues with program slowness and crashes. *
ECHO               *                                                *
ECHO               * You can shutdown your computer entirely.  This *
ECHO               * option only works with ATX computers, as it    *
ECHO               * requires their power management tools.         *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
PAUSE
GOTO SHUTDOWN

REM OPENGEM FINAL EXIT MESSAGE
:END
CLS
CD\
ECHO               **************************************************
ECHO               *           Thank you for using OpenGEM          *
ECHO               *                                                *
ECHO               *             Copyright (c) Opendawn             *
ECHO               *             http://gem.opendawn.com            *
ECHO               **************************************************
ECHO.