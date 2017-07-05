FreeGEM video drivers
=====================

  Recent changes: A new driver has been added for the Hercules InColor card.

  This zipfile contains the following FreeGEM video drivers for nonstandard
hardware. I have tested them, at least to the point of starting GEM and 
checking that the Desktop appears.

SDU759.VGA: Supports the 720x540 mode present on some VGA chipsets including
           the Amstrad PC5086. If your VGA card has a 720x540 mode that this
           driver does not detect, you can set it using VIDPATCH:
		VIDPATCH SDPAR9.VGA <mode> -- Mode number to use (decimal)
		VIDPATCH SDPAR9.VGA AUTO   -- restores default mode.

SDATT9.EGA: Supports the "AT&T" 640x400 mono mode on such computers as the 
           AT&T 6300, Olivetti M24, Toshiba 3100 and DEC VAXmate.

SDAMS9.CGA: Supports the 640x200 16-colour mode on the Amstrad PC1512.

SDHIC9.EGA: Supports the 720x348 16-colour mode on the Hercules InColor card.

SDKHM9.EGA: Supports the 720x350 graphics mode on the IBM 3270 PC.

SDKLF9.EGA: Supports the 360x350 4-colour graphics mode on the IBM 3270 PC.

SDPLA9.CGA: Supports the 640x200 4-colour mode on the Plantronics ColorPlus 
           and compatible cards (such as the ATI Graphics Solution). 

SDPAR9.VGA: Supports 640x400 and 640x480 256-colour modes on Paradise VGA
	   chipsets. As supplied, it attempts to autodetect which mode to
	   use. To force one or the other, use VIDPATCH:

		VIDPATCH SDPAR9.VGA 95    -- forces 640x480 mode
		VIDPATCH SDPAR9.VGA 94    -- forces 640x400 mode
		VIDPATCH SDPAR9.VGA AUTO  -- restores autodetection.

	   Be warned that on an 8086 SDPAR9 is not particularly sprightly.


The included Desk Accessory (FIXPAL.ZIP) allows you to switch between the 
magenta/cyan/white and red/green/yellow palettes in the 4-colour drivers.

  Compared to the original GEM drivers, these drivers have the following 
changes:

* Loadable codepages, using code from ViewMAX II. To get this working, you 
  need to install GEM.CPI in the same place as GEMVDI.EXE, and then use CHCP 
  to select the correct codepage before starting GEM. SDKLF9.EGA does not 
  support loadable codepages and uses a hardcoded ISO-8859-15 font.

* There is one extra function, which I've called v_delay(). It's simply a 
  wrapper around the BIOS's microsecond delay functionality, and provides 
  correct window opening/closing effects on the latest FreeGEM AES.

* The mouse support in all drivers has been updated to match ViewMAX/1. There
 is one visible change to this: if you have the mouse type set to 0 (no mouse)
 then pressing CTRL and the right SHIFT key together toggles whether the 
 pointer is visible. It also tries to pick up COM port settings from the BIOS 
 rather than hardcoding them.

* Some of the other bits of code have also been updated to match ViewMAX/2. 
 In general the changes are minor.

* Some drivers which weren't present in GEM/3.1 have been reconstructed from 
 earlier GEM versions, or written from scratch.


320x360 driver notes
====================

* As mentioned above, the 320x360 driver lacks codepage support. This is 
 because it uses a 5-pixel font (based on the X11 font 

 -Misc-Fixed-Medium-R-Normal--7-70-75-75-C-50-ISO8859-15

 ) and GEM.CPI doesn't yet contain any 5-pixel fonts.

* The 5-pixel font has also simply been doubled in height (from 5x7 to 5x14)
 with no manual tidying up.

* Even with 5-pixel fonts, the screen is only 72 characters wide, when 
 ideally it would be 80.

* Bitmaps and the mouse pointers are much bigger than the text, which can
 upset things like 'about' boxes. The desktop icons are bigger again, because
 the Desktop uses the low-resolution set (which are intended for a low 
 _vertical_ resolution but a high _horizontal_ resolution).

* If you're using FreeGEM, it's best to turn off the 3D effects and use
  solid colour rather than textures, or things get very grainy.


