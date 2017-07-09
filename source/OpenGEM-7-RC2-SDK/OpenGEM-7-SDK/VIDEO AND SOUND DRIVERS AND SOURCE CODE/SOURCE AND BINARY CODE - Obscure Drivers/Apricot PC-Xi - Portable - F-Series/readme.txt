GEM/3 video drivers for the ACT Apricot PC / Xi / Portable / F-Series
======================================================================

  This archive contains six GEM drivers: 
* Four aimed at the Apricot Portable (FP).
* One for the F1 / F2 / F10 series.
* One (more experimental) for the PC / Xi.

Name in this archive	Machine	Resolution	display
==============================================================
SDFPM9.CGA		FP	640x200 mono	LCD
SDFP29.CGA		FP	640x256x4	CRT
SDFP39.CGA		FP	640x256x8	CRT
SDFP49.CGA		FP	640x256x16	CRT
SDAF19.CGA		F1	640x256x4	CRT 
SDAXI9.EGA		PC/Xi	800x400 mono	CRT

  All drivers other than the PC/Xi one are based on disassembling Digital
Research's GEM/1.2 drivers for these computers, then updated to GEM/3 using
the published GEM/3 driver source tree. The exception is the PC/Xi driver; 
since I didn't have an original one to disassemble, I created it from scratch.

Installation
~~~~~~~~~~~~

  If you don't have a working copy of GEM on the Apricot already, I suggest
installing on a real IBM-compatible PC. If you're targeting the PC / Xi, 
choose the EGA mono 640x350 driver; otherwise, the CGA mono 640x200 driver.
Then copy the installed GEM onto the Apricot, and make the following changes:

For GEM/1 or GEM/2:

* Rename your chosen driver (SDxxx9.xGA) to SDxxx9.SYS
* Edit ASSIGN.SYS. On the line beginning with 01, replace the driver name with
 SDxxx9. So, for example

	01 IBMEHFP5	  becomes	01 SDAXI9
or	01 IBMCHMP5	  becomes	01 SDFP49

For GEM/3 or FreeGEM:

* Delete any files with names beginning SD in \GEMAPPS\GEMSYS.
* Copy your chosen SDxxx9.xGA file into \GEMAPPS\GEMSYS.

Switchable screen resolution
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  The F1 and FP colour drivers support two screen resolutions: 640x200 (60Hz) 
and 640x256 (50Hz). In the normal course of events, this setting will be read
by MS-DOS at boot time, and can be changed using CONFIG.APP under GEM.

  If you want to override this setting, you can use VIDPATCH:

	VIDPATCH SDxxx9.CGA 50		-- Select 50Hz (640x256) screen
	VIDPATCH SDxxx9.CGA 60		-- Select 60Hz (640x256) screen
	VIDPATCH SDxxx9.CGA AUTO	-- Use the MS-DOS configuration table

FP / F1 drivers: What's New
~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Switchable resolutions, as mentioned above.

* These are GEM/3 drivers. This means, if I've got things right, they support
 GEM/3 functions like Bezier curves.

* Codepage support. However, all the versions of MS-DOS I've seen for the 
 Apricot are MS-DOS 2, and codepage support requires MS-DOS 3; so this code is
 untested.

* The 16-colour driver (SDFP49.CGA) is new. There may well have been a 
 16-colour DRI driver, but I don't have it, so I had to construct it from 
 the source code for the 8-colour driver.

* The screen palette is held in common across virtual workstations. In DRI 
 drivers, changes made to the palette in one virtual workstation will update 
 the palette registers, but will not be detected in other virtual 
 workstations.

* Bug fixes for various drawing problems:
	- Text drawn in TRAN mode not aligned to a byte boundary was being
	 drawn in REPLACE mode.
	- vro_cpyfm() drawing mode 2 was not drawing fringes correctly. 

* Various pockets of dead code have been removed.

PC / Xi driver: Notes
~~~~~~~~~~~~~~~~~~~~~

- The MicroScreen keys 1-5 are mapped to the 10 function keys that GEM 
expects: key 1 to F1/F2, key 2 to F3/F4, and so on. Use Shift to select the
even-numbered function key.

- MicroScreen key 6 toggles the keyboard mouse on or off. If keyboard mouse
 is on, use the cursor keys to move the pointer, HOME as the left mouse
 button, CLEAR as the right mouse button. Key 1 on the numeric keypad starts
 a mouse drag operation.

- Since there is no ALT key, CTRL+SHIFT+letter should have the same effect.

Known Bugs
~~~~~~~~~~

- PC / Xi driver: There is no keyboard repeat, which makes the keyboard 
 mouse somewhat trying to use. Sorry.

- PC / Xi driver: I don't actually know whether Caps Lock works, since I never
 use it.

- F1 / FP drivers: There appears to be no way to toggle the keyboard mouse.

Copying
~~~~~~~
  The drivers I disassembled contained only Digital Research copyright
messages. I am assuming that they were covered by the release of GEM under the
GNU GPL <http://www.simpits.org/pipermail/gem-dev/2007-March/001405.html>
and have accordingly made extensive use of the existing GEM/3 GPL source tree.


