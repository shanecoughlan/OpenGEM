GEM/3 video driver for the BBC Master 512
=========================================

  This archive contains two video drivers for the BBC Master 512, allowing 
GEM/3 to use the same video modes as the supplied drivers for GEM/2 (rather
than just emulated CGA). The drivers may also work under Concurrent DOS on 
an ABC 300/310, if any such machines still exist.

  These drivers (like most of the rest of BBC Master DOS Plus) are 
temperamental -- see the list of known issues further down.

Installation
~~~~~~~~~~~~

 Install GEM/3, and then go to \GEMAPPS\GEMSYS. Delete the existing CGA driver 
(SDCGA*.CGA) and replace it with the appropriate driver: 

SDBBM9.CGA -- 640x256 mono
SDBBC9.CGA -- 320x256 colour

  The drivers support only the Master 512's built-in mouse. Therefore it  
is not necessary to load an additional mouse driver from DOS.

Known Issues
~~~~~~~~~~~~

* GEM/3 itself appears only to work on DOS Plus 2.1.

* The drivers have not been tested on a real Master 512 or ABC 300/310. The
only tests were done on the BeebEm emulator with 896k of emulated RAM.

* Although these drivers are based on the FreeGEM driver codebase, there is
no codepage support. This is because no known operating system for the 
80x86 processors in the Master 512 or ABC 300/310 supports codepages, so 
there would be no point.

* The colour driver attempts to allocate its framebuffer at runtime. This fails
unless ADDMEM was previously run with a memory amount of 300 or greater 
(eg: ADDMEM 333). It was not possible to incorporate the framebuffer in the 
driver itself (as was done in GEM/2) because something (either the GEM/3 VDI, 
or DOS Plus itself) cannot handle drivers exceeding 64k in size.

* The copy of GEM Paint supplied with DOS Plus crashes when run under GEM/3 
with either of these drivers, as soon as any menu is dropped down.

Off-Screen Bitmap Storage
~~~~~~~~~~~~~~~~~~~~~~~~~
  When GEM/3 drops down a menu, it saves a copy of what was behind it. Normally
this is saved in conventional memory, but if the driver supports it the bitmap
can be saved in spare video card memory, increasing the amount of main memory 
available to programs. 

  Both the supplied drivers support this facility, using the video RAM at
B8000h. The mono driver only needs 20k of the 32k area for the screen, so 
it allows the remaining 12k to be used for bitmap storage. The colour driver
has its own framebuffer, so it allows the whole 32k area at B8000h to be used
for bitmap storage.

  These buffers are accessed by doing a bit-blit operation to/from a memory
form with a memory pointer of FFFF:FFFF.

Copyright
~~~~~~~~~
  These two drivers have been largely based on the GEM/3 source tree, 
released under the GPL. Roughly 2% of the lines of code are unique to the
BBC Master - in particular, the mouse driver and the logic that works out 
which areas of the screen have been updated and squirts them across the Tube.
This code has been added from a disassembly of the existing GEM/1 driver
ACORNBW.SYS, for which no source exists. The copyright message reads Digital 
Research and it is my understanding that all DRI GEM is covered by the
GPL whether or not source exists. Thus:
<http://www.simpits.org/pipermail/gem-dev/2007-March/001405.html>

