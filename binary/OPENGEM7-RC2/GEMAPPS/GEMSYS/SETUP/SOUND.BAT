@ECHO OFF
CLS

REM Change the sound driver
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * Please select the driver you want to use for   *
ECHO               * sound in OpenGEM:                              *
ECHO               *                                                *
ECHO               * 1 - Sound Blaster (default)                    *
ECHO               * 2 - Sound Blaster 16                           *
ECHO               * 3 - Sound Blaster Pro                          *
ECHO               * 4 - CS 4265                                    *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
ECHO                           Please select an option...
CD GEMAPPS\GEMSYS
choice /c:1234
if errorlevel 4 goto 4
if errorlevel 3 goto 3
if errorlevel 2 goto 2
if errorlevel 1 goto 1
:1
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL SOUND.EXE
CD GEMAPPS\GEMSYS\SETUP\SOUND
COPY BDSB.EXE C:\GEMAPPS\GEMSYS
CD GEMAPPS\GEMSYS
REN BDSB.EXE SOUND.EXE
DEL BDSB.EXE
CLS
ECHO               **************************************************
ECHO               *              Sound Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:2
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL SOUND.EXE
CD\
CD GEMAPPS\GEMSYS\SETUP\SOUND
COPY BDSB16.EXE C:\GEMAPPS\GEMSYS
CD GEMAPPS\GEMSYS
REN BDSB16.EXE SOUND.EXE
DEL BDSB16.EXE
CLS
ECHO               **************************************************
ECHO               *              Sound Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:3
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL SOUND.EXE
CD\
CD GEMAPPS\GEMSYS\SETUP\SOUND
COPY BDSBPRO.EXE C:\GEMAPPS\GEMSYS
CD GEMAPPS\GEMSYS
REN BDSBPRO.EXE SOUND.EXE
DEL BDSBPRO.EXE
CLS
ECHO               **************************************************
ECHO               *              Sound Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:4
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL SOUND.EXE
CD\
CD GEMAPPS\GEMSYS\SETUP\SOUND
COPY Bdcs4235.exe C:\GEMAPPS\GEMSYS
CD GEMAPPS\GEMSYS
REN Bdcs4235.exe SOUND.EXE
DEL Bdcs4235.exe
CLS
ECHO               **************************************************
ECHO               *              Sound Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END

:END
CLS
CD GEMAPPS\GEMSYS\SETUP
SETUP