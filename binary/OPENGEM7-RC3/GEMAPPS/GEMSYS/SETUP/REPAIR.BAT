@ECHO OFF
CLS

REM Repair OpenGEM
:MAIN
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * OpenGEM can repair its core code and restore   *
ECHO               * the default desktop.  This is useful if your   *
ECHO               * copy of OpenGEM is not working properly.       *
ECHO               *                                                *
ECHO               * Would you like OpenGEM to repair itself now?   *
ECHO               *                                                *
ECHO               * 1 - Yes, please repair yourself and restore    *
ECHO               *     the default OpenGEM desktop                *
ECHO               *                                                *
ECHO               * 2 - No, let me go back to the main setup menu  *
ECHO               *                                                *
ECHO               *                                                *
ECHO               * 3 - Get Help                                   *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
ECHO                           Please select an option...
CD GEMAPPS\GEMSYS
choice /c:123
if errorlevel 3 goto 3
if errorlevel 2 goto 2
if errorlevel 1 goto 1

:1
CLS
ECHO OFF
CD GEMAPPS\GEMSYS\
UNZIP -o c:\GEMAPPS\GEMSYS\SETUP\REPAIR\REPAIR.ZIP -d C:\
CLS
ECHO               **************************************************
ECHO               *          OpenGEM has repaired itself!          *
ECHO               **************************************************
PAUSE
GOTO END

:2
GOTO END

:3
CLS
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * OpenGEM can repair its core code and restore   *
ECHO               * the default desktop by using a zip archive     *
ECHO               * located in C:\GEMAPPS\GEMSYS\SETUP\REPAIR.     *
ECHO               *                                                *
ECHO               * This is useful if your GEM system freezes or   *
ECHO               * hangs after you have changed a system file.    *
ECHO               *                                                *
ECHO               * Please remember that if you repair OpenGEM,    *
ECHO               * all the changes you made to your desktop since *
ECHO               * installing will disappear.  Repair takes you   *
ECHO               * back to the original install settings of your  *
ECHO               * copy of OpenGEM.                               *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
PAUSE
GOTO MAIN

:END
CLS
CD GEMAPPS\GEMSYS\SETUP
SETUP