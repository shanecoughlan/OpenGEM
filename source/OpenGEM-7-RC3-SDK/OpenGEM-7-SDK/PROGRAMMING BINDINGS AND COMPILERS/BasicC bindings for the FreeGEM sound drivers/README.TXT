BASIC/C BINDINGS FOR HEINZ RATH'S SOUND DRIVERS
Version 1.1

Copyright (c) Owen Rudge 2000

INTRODUCTION - C
================

See below for BASIC documentation.

I have written some C bindings that interface with Heinz Rath's sound drivers
for GEM (www.geocities.com/heinz_rath/). These have implementations of all
the functions included in every version of the drivers up to 0.4. The PlayIWAV
(play in background) functions are working.

I have compiled and run it sucessfully under Watcom C++ 10.6 (and it was a third
of the BC4.51 size too...). I have also compiled and again sucessfully run it under
Pacific C (it had to be different about the REGS structure...). I also compiled it
under Turbo C++ 1.01, and that worked, and what's more, the EXE was tiny - just 8KB!

Please note that the sample program works best under Borland/Turbo C++, as Watcom
C++ doesn't seem to do the gotoxy() functions too well, and Pacific doesn't support
them, and I can't be bothered to write more interrupts to move the cursor!

Please note that the executable MUST be large model, or it will not work. (However,
it seems to work under QuickBasic, which is small model I think...)

USING THE ROUTINES FROM C
=========================

To play a WAV file, you need to turn the speaker on, use PlayWAV to play the
file, and then turn the speaker off. The following code will do this:

SpeakerOn();
PlayWAV("TEST.WAV");
SpeakerOff();

Documentation on all these functions are available in the GEMSND.C file and
in the SND.TXT file distributed with the drivers.

To play a WAV file in the background, you need to turn the speaker on, and use
PlayIWAV to play the file. You can then do whatever you want. When you are finished,
it is best to use StopWAV to stop the file, and then turn the speaker off. The
following code will do this:

SpeakerOn();
PlayIWAV("TEST.WAV");
/* do various things */
StopWAV();
SpeakerOff();

USING THE C TEST PROGRAMS
=========================

The test application plays every file on the command line (unless, of course,
it isn't a WAVE file!). Just type TEST, and a list of files. If you type /B
before the list of files, it will play them in the background, and under
Borland C++, you will have a time counter.

INTRODUCTION - BASIC
====================

I have also written bindings for QuickBASIC. (When I first started, I wondered
why I kept getting 0 for various DriverInfo calls. I only discovered earlier
today that it was because the info variables weren't SHARED...). Again, all the
functions in Heinz's drivers up to v0.4 have been implemented. You must specify
the Quick Library QB.QLB on the QuickBASIC command line (QB GEMSND.BAS /L will
do the trick).

I wrote, tested and compiled these bindings under QuickBASIC 4.5. It may or may
not work on earlier versions (versions under 4.x probably won't work). They do
not work with QBASIC.

USING THE ROUTINES FROM QUICKBASIC
==================================

The syntax is almost exactly the same as is the C syntax. To play a WAV file,
you need to turn the speaker on, use PlayWAV to play the file, and then turn
the speaker off. The following code will do this:

SpeakerOn
x% = PlayWAV("TEST.WAV")
SpeakerOff

Documentation on all these functions are available in the GEMSND.BAS file and
in the SND.TXT file distributed with the drivers.

To play a WAV file in the background, you need to turn the speaker on, and use
PlayIWAV to play the file. You can then do whatever you want. When you are finished,
it is best to use StopWAV to stop the file, and then turn the speaker off. The
following code will do this:

SpeakerOn
x% = PlayIWAV("TEST.WAV")
' do various things
StopWAV
SpeakerOff

USING THE BASIC TEST PROGRAM
============================

The QuickBASIC test program is pretty similar to the C test program, except you
can only put one file on the command line. Playing in the background is supported,
with, in my opinion, a better clock! Please note that pressing PAUSE to pause the
program will mess up the time!

CONTACT INFORMATION
===================

Thanks for downloading these bindings. If you have any problems, e-mail me at
orudge@freeuk.com.

Owen Rudge
19th November 2000

E-mail:         orudge@freeuk.com
Web site:       http://www.owenrudge.co.uk/
Web site (GEM): http://www.owenrudge.co.uk/GEM/