FreeGEM video drivers
=====================

  This zipfile contains the following FreeGEM video drivers, which I have 
been able to compile but not test. They may work; they may fail; they
may crash your system. If you have suitable hardware and are able to test 
any of them, let me know how you get on.

Driver     | Resolution | Colours | Supported hardware
===========+============+=========+=========================================
SDDEB9.EGA | 640x400    | 16      | AT&T 6300 Display Enhancement Board
SDGEN9.VGA | 1008x728   |  2      | Genius card
SDGSV9.VGA | 800x600    | 16      | Genoa SuperVGA card
SDP649.VGA | 640x480    | 16      | Quadram Prosync
SDP759.VGA | 752x358    | 16      | Quadram Prosync
SDWYS9.VGA | 1280x800   |  2      | Wyse 700 (tested under emulation, but
           |            |         | not on real hardware).
===========+============+=========+=========================================

  Compared to the original GEM drivers, these drivers have the following 
changes:

* Loadable codepages, using code from ViewMAX II. To get this working, you 
  need to install GEM.CPI in the same place as GEMVDI.EXE, and then use CHCP 
  to select the correct codepage before starting GEM.

* There is one extra function, which I've called v_delay(). It's simply a 
  wrapper around the BIOS's microsecond delay functionality, and provides 
  correct window opening/closing effects on the latest FreeGEM AES.

* The mouse support in all drivers has been updated to match ViewMAX/1. There
 is one visible change to this: if you have the mouse type set to 0 (no mouse)
 then pressing CTRL and the right SHIFT key together toggles whether the 
 pointer is visible. It also tries to pick up COM port settings from the BIOS 
 rather than hardcoding them.

* Some of the other bits of code have also been updated to match ViewMAX/2. 
 In general the changes are minor, and relate to support for the 'universal'
 EGA/VGA and CGA/AT&T drivers.

* Some drivers which weren't present in GEM/3.1 have been reconstructed from 
 earlier GEM versions.
