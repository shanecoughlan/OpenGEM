@ECHO OFF
CLS

REM Change the video driver
:MENU
ECHO OFF
CLS
ECHO               **************************************************
ECHO               *                                                *
ECHO               * Please select the driver you want to use for   *
ECHO               * video in OpenGEM:                              *
ECHO               *                                                *
ECHO               * 1 - VGA 640x480 16 colours (default)           *
ECHO               * 2 - VGA 640x480 monochrome                     *
ECHO               * 3 - EGA 640x350 16 colours                     *
ECHO               * 4 - EGA/Mono Display 640x350                   *
ECHO               * 5 - CGA/Colour Display 640x200 monochrome      *
ECHO               * 6 - Super VGA 800x600 16 colours               *
ECHO               * 7 - ATT DEB 640x480 16 colours                 *
ECHO               * 8 - ATT Monochrome 640x480                     *
ECHO               *                                                *
ECHO               * 9 - Additional video driver options...         *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
ECHO                           Please select an option...
CD GEMAPPS\GEMSYS
choice /c:123456789
if errorlevel 9 goto 9
if errorlevel 8 goto 8
if errorlevel 7 goto 7
if errorlevel 6 goto 6
if errorlevel 5 goto 5
if errorlevel 4 goto 4
if errorlevel 3 goto 3
if errorlevel 2 goto 2
if errorlevel 1 goto 1
:1
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDPSC9.VGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:2
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDPSM8.VGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:3
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDEHF8.EGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:4
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDEH18.EGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:5
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDCGA8.CGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:6
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDPSC8.VGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:7
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDDEB8.EGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:8
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDATT8.EGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:9
GOTO ADDITIONAL MENU

:ADDITIONAL MENU
ECHO OFF
CLS
ECHO               **************************************************
ECHO               *                                                *
ECHO               * Please select the driver you want to use for   *
ECHO               * video in OpenGEM:                              *
ECHO               *                                                *
ECHO               * 1 - Hercules/Monochrome PC Display 720x348     *
ECHO               * 2 - GEM/5-palette VGA 640x480 16 colours       *
ECHO               * 3 - MDS THE GENIUS 728x1008                    *
ECHO               * 4 - Video 7 VEGA Deluxe 640x480 16 colours     *
ECHO               * 5 - Quadram ProSync 640x480 16 colours         *
ECHO               * 6 - Video 7 VEGA Deluxe 752x410 16 colours     *
ECHO               * 7 - Quadram ProSync 752x410 16 colours         *
ECHO               * 8 - Wyse/Amdek Monochrome 1200x800             *
ECHO               *                                                *
ECHO               * 9 - Back to main video driver options...       *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
ECHO                           Please select an option...
CD GEMAPPS\GEMSYS
choice /c:123456789
if errorlevel 9 goto MENU
if errorlevel 8 goto S
if errorlevel 7 goto T
if errorlevel 6 goto U
if errorlevel 5 goto V
if errorlevel 4 goto W
if errorlevel 3 goto X
if errorlevel 2 goto Y
if errorlevel 1 goto Z
:Z
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDHRC8.EGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:Y
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDGEM5.VGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:X
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDGEN8.VGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:W
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDP648.VGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:V
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDP648.VGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:U
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDP758.VGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:T
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDP758.VGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END
:S
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.CGA
DEL *.EGA
DEL *.VGA
CD GEMAPPS\GEMSYS\SETUP\VIDEO
COPY SDWYS8.VGA C:\GEMAPPS\GEMSYS
CLS
ECHO               **************************************************
ECHO               *              Video Driver changed!             *
ECHO               **************************************************
PAUSE > NUL
GOTO END

:END
CLS
CD GEMAPPS\GEMSYS\SETUP
SETUP