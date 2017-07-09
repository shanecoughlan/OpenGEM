FreeGEM video drivers
=====================

  This zipfile contains four 'preferred' FreeGEM video drivers. These should
work correctly on the great majority of PC-compatible hardware. If you cannot
get a usable display out of any of these drivers, you may like to try the
'alternative drivers' zipfile, which should be available wherever you got 
this one.

  If your video card has a VESA-compatible BIOS and you want a driver that
supports 256 colours or a higher resolution than 800x600, download the 
'VESA drivers' zipfile (which, again, should be available wherever you found i
this file).

  The four drivers in this pack are: 

Name       | Resolution | Colours | Notes
===========|============|=========|===========================================
SDU869.VGA | 800x600    | 16      | Supports VESA-compatible, Trident and 
           |            |         | Paradise video cards.
           |            |         |
SDUNI9.VGA | 640x480    | 16      | When used with a VGA card.
           | 640x480    | mono    | When used with an MCGA chipset.
           | 640x350    | 16      | When used with an EGA card and colour EGA
           |            |         | monitor.
           | 640x350    | mono    | When used with an EGA card and mono EGA
           |            |         | monitor.
           |            |         | 
SDCAT9.EGA | 640x200    | mono    | When used on a CGA or EGA chipset.
           | 640x400    | mono    | When used on AT&T 6300, Olivetti M24, 
           |            |         | Toshiba 3100e or DEC VAXmate.
           |            |         | 
SDHRC9.EGA | 720x348    | mono    | For use on a Hercules card.
============================================================================== 

  If you have an EGA with only 64k (or one that's using a non-EGA monitor), 
then although you can use SDCAT9.EGA you'd almost certainly get better results
with one of the EGA drivers from the alternative set.

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

Manual mode override
====================

  VIDPATCH.EXE has been supplied in case the automatic video mode probes 
in SDUNI9.VGA or SDCAT9.EGA do not work as expected. The following commands
override the probe:

VIDPATCH SDUNI9.VGA 0	  -- force EGA mono
VIDPATCH SDUNI9.VGA 1	  -- force EGA colour
VIDPATCH SDUNI9.VGA 2	  -- force VGA/MCGA mono
VIDPATCH SDUNI9.VGA 3 	  -- force VGA colour
VIDPATCH SDUNI9.VGA AUTO  -- go back to automatic probe

VIDPATCH SDCAT9.EGA 6	  -- force CGA mode
VIDPATCH SDCAT9.EGA 64	  -- force AT&T 6300 mode
VIDPATCH SDCAT9.EGA 116	  -- force Toshiba 3100e mode
VIDPATCH SDCAT9.VGA AUTO  -- go back to automatic probe

  VIDPATCH can also be used to override the mode detection in SDU869.VGA --
for example, if your video card provides an 800x600 mode that is not detected 
automatically. 

VIDPATCH SDU869.VGA <mode>   

where <mode> is a decimal number. For example, 

VIDPATCH SDU869.VGA 82

selects the 800x600 mode that Oak chipsets use.

  If you're using these driver in ViewMAX, you can achieve the same effect by
adding a VidPatch= line to ViewMAX.INI.




