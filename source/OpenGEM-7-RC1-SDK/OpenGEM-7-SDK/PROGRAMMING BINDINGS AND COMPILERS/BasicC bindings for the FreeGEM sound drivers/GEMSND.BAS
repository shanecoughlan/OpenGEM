'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' GEM Sound Drivers - QuickBASIC routines                               '
' Copyright (c) Owen Rudge 2000                                         '
'                                                                       '
' Test program                                                          '
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

DECLARE FUNCTION MAKETIME$ (HOUR AS INTEGER, MIN AS INTEGER, SEC AS INTEGER)
DECLARE FUNCTION TRUEFALSE$ (NUM AS INTEGER)

' Sound driver routines
DECLARE SUB StopWAV ()
DECLARE FUNCTION IsPlaying! ()
DECLARE FUNCTION PlayIWAV! (Filename AS STRING)
DECLARE FUNCTION GetStereo! ()
DECLARE SUB SpeakerOn ()
DECLARE SUB SpeakerOff ()
DECLARE FUNCTION PlayWAV (Filename AS STRING)
DECLARE SUB DriverInfo ()

' Registers type
TYPE RegType
   AX AS INTEGER
   BX AS INTEGER
   CX AS INTEGER
   DX AS INTEGER
   BP AS INTEGER
   SI AS INTEGER

   DI AS INTEGER
   FLAGS AS INTEGER
   DS AS INTEGER
   ES AS INTEGER
END TYPE

DIM SHARED VERNUM  AS INTEGER
DIM SHARED DRVTYPE AS INTEGER
DIM SHARED SBPORT AS INTEGER
DIM SHARED DMA AS INTEGER
DIM SHARED STEREO AS INTEGER
DIM SHARED OUTPUT16 AS INTEGER
DIM SHARED HMVER AS INTEGER

DIM SHARED DICALLED AS INTEGER

DIM BACKGROUND AS INTEGER
DIM SECONDS AS INTEGER
DIM MINUTES AS INTEGER
DIM HOURS AS INTEGER

CONST TRUE = 1
CONST FALSE = 0

SECONDS = 0

IF LTRIM$(RTRIM$(COMMAND$)) = "" THEN
   PRINT "Syntax: GEMSND [/B] filename"
   END
END IF

IF LEFT$(UCASE$(COMMAND$), 2) = "/B" THEN
   BACKGROUND = TRUE
ELSE
   BACKGROUND = FALSE
END IF

CLS

DriverInfo

PRINT "DRIVER INFORMATION"
PRINT "=================="
PRINT
PRINT "Version Number: "; VERNUM
PRINT "Driver Type:    "; DRVTYPE
PRINT "SB Port:         "; HEX$(SBPORT)
PRINT "DMA:            "; DMA
PRINT "Stereo?          "; TRUEFALSE$(STEREO)
PRINT "16-bit output?   "; TRUEFALSE$(OUTPUT16)
PRINT "High memory ver? "; TRUEFALSE$(HMVER)
PRINT
PRINT "Play in Background: "; TRUEFALSE$(BACKGROUND)

PRINT "Turning speaker on..."
SpeakerOn
PRINT "Playing "; LTRIM$(RTRIM$((RIGHT$(COMMAND$, LEN(COMMAND$) - 3)))); "..."

IF BACKGROUND = FALSE THEN
   PRINT "Return Value: "; PlayWAV(COMMAND$)
ELSE
   x% = PlayIWAV(LTRIM$(RTRIM$((RIGHT$(COMMAND$, LEN(COMMAND$) - 3)))))
   DO WHILE INKEY$ = ""
      SLEEP 1

      IF IsPlaying = FALSE THEN
         EXIT DO
      END IF

      SECONDS = SECONDS + 1
     
      IF SECONDS >= 60 THEN
         MINUTES = MINUTES + 1
         SECONDS = 0
      END IF

      IF MINUTES >= 60 THEN
         HOURS = HOURS + 1
         MINUTES = 0
      END IF

      OldPos = POS(0)
      OldLin = CSRLIN

      LOCATE 1, 60
      PRINT "Time: "; MAKETIME$(HOURS, MINUTES, SECONDS)
      LOCATE OldLin, OldPos
   LOOP

   StopWAV
END IF

PRINT "Turning speaker off..."
SpeakerOff

SUB DMAStop
   DIM REGS AS RegType

   REGS.AX = &H2208
   REGS.BX = 8

   CALL INTERRUPTX(&HE1, REGS, REGS)
END SUB

SUB DriverInfo
   DIM INREGS AS RegType
   DIM OUTREGS AS RegType

   INREGS.AX = &H2208
   INREGS.BX = 1

   CALL INTERRUPTX(&HE1, INREGS, OUTREGS)

   VERNUM = OUTREGS.CX
   DRVTYPE = OUTREGS.DX
   SBPORT = OUTREGS.AX
   DMA = OUTREGS.SI

   IF OUTREGS.DI AND O THEN
      STEREO = TRUE
   ELSE
      STEREO = FALSE
   END IF

   IF OUTREGS.DI AND 1 THEN
      OUTPUT16 = TRUE
   ELSE
      OUTPUT16 = FALSE
   END IF

   IF OUTREGS.DI AND 2 THEN
      HMVER = TRUE
   ELSE
      HMVER = FALSE
   END IF

   DICALLED = TRUE
END SUB

FUNCTION GetStereo
    DIM REGS AS RegType

    IF DICALLED = FALSE THEN
       DriverInfo
    END IF

    IF VERNUM >= 2 THEN  ' check if version 0.2 or higher
        REGS.AX = &H2208
        REGS.BX = 9

        CALL INTERRUPT(&HE1, REGS, REGS)
        GetStereo = REGS.CX
    ELSE
        GetStereo = -1  ' incorrect ver
    END IF
END FUNCTION

FUNCTION IsPlaying
    DIM REGS AS RegType
   
    REGS.AX = &H2208
    REGS.BX = 14

    CALL INTERRUPTX(&HE1, REGS, REGS)

    IF REGS.CX = 1 THEN
        IsPlaying = TRUE
    ELSE
        IsPlaying = FALSE
    END IF
END FUNCTION

FUNCTION MAKETIME$ (HOUR AS INTEGER, MIN AS INTEGER, SEC AS INTEGER)
   HRSSTR$ = LTRIM$(RTRIM$(STR$(HOUR)))
   MINSTR$ = LTRIM$(RTRIM$(STR$(MIN)))
   SECSTR$ = LTRIM$(RTRIM$(STR$(SEC)))

   IF LEN(MINSTR$) = 1 THEN
      MINSTR$ = "0" + MINSTR$
   END IF

   IF LEN(SECSTR$) = 1 THEN
      SECSTR$ = "0" + SECSTR$
   END IF

   IF LEN(HRSSTR$) = 1 THEN
      HRSSTR$ = "0" + HRSSTR$
   END IF

   MAKETIME$ = HRSSTR$ + ":" + MINSTR$ + ":" + SECSTR$
END FUNCTION

FUNCTION PlayIWAV (Filename AS STRING)
   DIM REGS AS RegType
   DIM FILNAM AS STRING * 250  ' max 250 chars in filename

   IF DICALLED = FALSE THEN
       DriverInfo
   END IF

   IF VERNUM >= 4 THEN  ' check if version 0.4 or higher
        FILNAM = Filename + CHR$(0)

        REGS.AX = &H2208
        REGS.BX = 13
        REGS.CX = VARPTR(FILNAM)
        REGS.ES = VARSEG(FILNAM)

        CALL INTERRUPTX(&HE1, REGS, REGS)

        IF REGS.CX = 0 THEN
           PlayIWAV = TRUE
        ELSE
           PlayIWAV = FALSE
        END IF
   END IF
END FUNCTION

SUB PlaySample (SAMPLE AS STRING, SIZE AS INTEGER, KHZ AS INTEGER)
   DIM REGS AS RegType
   DIM SMPL AS STRING * 16000

   SMPL = SAMPLE

   REGS.AX = &H2208
   REGS.BX = 5
   REGS.SI = VARPTR(FILNAM)
   REGS.ES = VARSEG(FILNAM)
   REGS.CX = SIZE
   REGS.DX = KHZ

   CALL INTERRUPTX(&HE1, REGS, REGS)
END SUB

FUNCTION PlayWAV (Filename AS STRING)
   DIM REGS AS RegType
   DIM FILNAM AS STRING * 250  ' max 250 chars in filename

   FILNAM = Filename + CHR$(0)

   REGS.AX = &H2208
   REGS.BX = 5
   REGS.CX = VARPTR(FILNAM)
   REGS.ES = VARSEG(FILNAM)

   CALL INTERRUPTX(&HE1, REGS, REGS)

   IF REGS.CX = 0 THEN
      PlayWAV = TRUE
   ELSE
      PlayWAV = FALSE
   END IF
END FUNCTION

SUB SetStereo (NewVal AS INTEGER)
    DIM REGS AS RegType

    IF DICALLED = FALSE THEN
       DriverInfo
    END IF

    IF VERNUM >= 2 THEN  ' check if version 0.2 or higher
        REGS.AX = &H2208
        REGS.BX = 9
        REGS.CX = NewVal

        CALL INTERRUPT(&HE1, REGS, REGS)
    END IF
END SUB

SUB SpeakerOff
   DIM INREGS AS RegType
   DIM OUTREGS AS RegType

   INREGS.AX = &H2208
   INREGS.BX = 4

   CALL INTERRUPTX(&HE1, INREGS, OUTREGS)
END SUB

SUB SpeakerOn
   DIM INREGS AS RegType
   DIM OUTREGS AS RegType

   INREGS.AX = &H2208
   INREGS.BX = 3

   CALL INTERRUPTX(&HE1, INREGS, OUTREGS)
END SUB

SUB StopWAV
   DIM REGS AS RegType

   REGS.AX = &H2208
   REGS.BX = 15

   CALL INTERRUPTX(&HE1, REGS, REGS)
END SUB

FUNCTION TRUEFALSE$ (NUM AS INTEGER)
   IF NUM = TRUE THEN
      TRUEFALSE = "TRUE"
   ELSE
      TRUEFALSE = "FALSE"
   END IF
END FUNCTION

SUB UnloadDriver
   DIM INREGS AS RegType
   DIM OUTREGS AS RegType

   INREGS.AX = &H2208
   INREGS.BX = 2

   CALL INTERRUPTX(&HE1, INREGS, OUTREGS)
END SUB

SUB Volume (NewVal AS INTEGER)
    DIM REGS AS RegType

    IF DICALLED = FALSE THEN
       DriverInfo
    END IF

    IF VERNUM >= 2 THEN  ' check if version 0.2 or higher
        REGS.AX = &H2208
        REGS.BX = 10
        REGS.CX = NewVal

        CALL INTERRUPT(&HE1, REGS, REGS)
    END IF

END SUB

SUB WaitForDMA
   DIM REGS AS RegType

   REGS.AX = &H2208
   REGS.BX = 7

   CALL INTERRUPTX(&HE1, REGS, REGS)
END SUB

