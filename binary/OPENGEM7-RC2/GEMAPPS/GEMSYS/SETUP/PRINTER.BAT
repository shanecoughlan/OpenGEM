@ECHO OFF
CLS

REM Change the printer driver
ECHO OFF
ECHO               **************************************************
ECHO               *                                                *
ECHO               * Please select the driver you want to use for   *
ECHO               * printing in OpenGEM:                           *
ECHO               *                                                *
ECHO               * A - IBM/Epson High Res Printers (default)      *
ECHO               * B - Epson LQ Series Printers                   *
ECHO               * C - Epson GQ-3500 Laser Printer                *
ECHO               * D - Hewlett Packard Deskjet, Deskjet+ or newer *
ECHO               * E - Hewlett Packard Laserjet II Printer        *
ECHO               * F - Hewlett Packard LaserJet+                  *
ECHO               * G - IBM Proprinter                             *
ECHO               * H - LaserWriter / Postscript Printers          *
ECHO               * I - Toshiba P321, P341, or P351 Printer        *
ECHO               * J - Xerox 4020 Colour Ink Jet                  *
ECHO               *                                                *
ECHO               * Z - Quit without making any changes            *
ECHO               *                                                *
ECHO               **************************************************
ECHO.
ECHO                           Please select an option...
CD GEMAPPS\GEMSYS
choice /c:ABCDEFGHIJZ
if errorlevel 255 goto END
if errorlevel  11 goto END
if errorlevel  10 goto J
if errorlevel   9 goto I
if errorlevel   8 goto H
if errorlevel   7 goto G
if errorlevel   6 goto F
if errorlevel   5 goto E
if errorlevel   4 goto D
if errorlevel   3 goto C
if errorlevel   2 goto B
if errorlevel   1 goto A
if errorlevel   0 goto END

:A
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.B30
DEL *.EPS
DEL *.ELQ
DEL *.HPH
DEL *.X20
DEL PDPST8.SYS
CD GEMAPPS\GEMSYS\SETUP\PRINTERS
COPY PDEHI9.EPS C:\GEMAPPS\GEMSYS
CD\
CLS
ECHO               **************************************************
ECHO               *             Printer Driver changed!            *
ECHO               **************************************************
PAUSE
GOTO END

:B
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.B30
DEL *.EPS
DEL *.ELQ
DEL *.HPH
DEL *.X20
DEL PDPST8.SYS
CD GEMAPPS\GEMSYS\SETUP\PRINTERS
COPY PDELQ8.ELQ C:\GEMAPPS\GEMSYS
CD\
CLS
ECHO               **************************************************
ECHO               *             Printer Driver changed!            *
ECHO               **************************************************
PAUSE
GOTO END

:C
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.B30
DEL *.EPS
DEL *.ELQ
DEL *.HPH
DEL *.X20
DEL PDPST8.SYS
CD GEMAPPS\GEMSYS\SETUP\PRINTERS
COPY PDEGH8.B30 C:\GEMAPPS\GEMSYS
CD\
CLS
ECHO               **************************************************
ECHO               *             Printer Driver changed!            *
ECHO               **************************************************
PAUSE
GOTO END

:D
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.B30
DEL *.EPS
DEL *.ELQ
DEL *.HPH
DEL *.X20
DEL PDPST8.SYS
CD GEMAPPS\GEMSYS\SETUP\PRINTERS
COPY /Y DJU.ATM C:\GEMAPPS\GEMSYS
COPY /Y DJU.DDF C:\GEMAPPS\GEMSYS
COPY /Y LESEMICH.DOK C:\GEMAPPS\GEMSYS
COPY /Y PDDJU9.B30 C:\GEMAPPS\GEMSYS
COPY /Y PRTRERR.STR C:\GEMAPPS\GEMSYS
CD\
CLS
ECHO               **************************************************
ECHO               *             Printer Driver changed!            *
ECHO               **************************************************
PAUSE
GOTO END

:E
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.B30
DEL *.EPS
DEL *.ELQ
DEL *.HPH
DEL *.X20
DEL PDPST8.SYS
CD GEMAPPS\GEMSYS\SETUP\PRINTERS
COPY PDHPU8.B30 C:\GEMAPPS\GEMSYS
CD\
CLS
ECHO               **************************************************
ECHO               *             Printer Driver changed!            *
ECHO               **************************************************
PAUSE
GOTO END

:F
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.B30
DEL *.EPS
DEL *.ELQ
DEL *.HPH
DEL *.X20
DEL PDPST8.SYS
CD GEMAPPS\GEMSYS\SETUP\PRINTERS
COPY PDHPH8.HPH C:\GEMAPPS\GEMSYS
CD\
CLS
ECHO               **************************************************
ECHO               *             Printer Driver changed!            *
ECHO               **************************************************
PAUSE
GOTO END

:G
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.B30
DEL *.EPS
DEL *.ELQ
DEL *.HPH
DEL *.X20
DEL PDPST8.SYS
CD GEMAPPS\GEMSYS\SETUP\PRINTERS
COPY PDPRO8.EPS C:\GEMAPPS\GEMSYS
CD\
CLS
ECHO               **************************************************
ECHO               *             Printer Driver changed!            *
ECHO               **************************************************
PAUSE
GOTO END

:H
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.B30
DEL *.EPS
DEL *.ELQ
DEL *.HPH
DEL *.X20
DEL PDPST8.SYS
CD GEMAPPS\GEMSYS\SETUP\PRINTERS
COPY PDPST8.SYS C:\GEMAPPS\GEMSYS
CD\
CLS
ECHO               **************************************************
ECHO               *             Printer Driver changed!            *
ECHO               **************************************************
PAUSE
GOTO END

:I
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.B30
DEL *.EPS
DEL *.ELQ
DEL *.HPH
DEL *.X20
DEL PDPST8.SYS
CD GEMAPPS\GEMSYS\SETUP\PRINTERS
COPY PDTSH8.ELQ C:\GEMAPPS\GEMSYS
CD\
CLS
ECHO               **************************************************
ECHO               *             Printer Driver changed!            *
ECHO               **************************************************
PAUSE
GOTO END

:J
CLS
ECHO OFF
CD GEMAPPS\GEMSYS
DEL *.B30
DEL *.EPS
DEL *.ELQ
DEL *.HPH
DEL *.X20
DEL PDPST8.SYS
CD GEMAPPS\GEMSYS\SETUP\PRINTERS
COPY PDX208.X20 C:\GEMAPPS\GEMSYS
CD\
CLS
ECHO               **************************************************
ECHO               *             Printer Driver changed!            *
ECHO               **************************************************
PAUSE
GOTO END

:END
CLS
CD GEMAPPS\GEMSYS\SETUP
SETUP