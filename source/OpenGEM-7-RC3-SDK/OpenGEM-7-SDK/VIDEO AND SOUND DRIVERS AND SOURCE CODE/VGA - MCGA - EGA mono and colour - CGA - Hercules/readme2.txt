FreeGEM video drivers
=====================

  Recent changes: 

* Two new drivers added: VGA 4-colour and VGA 8-colour (intended for 
 software developers wanting to test their software against less-capable
 hardware).

  This zipfile contains 14 'alternative' FreeGEM video drivers. These 
are intended for use in situations where, for some reason, the 'preferred' 
drivers do not work. Possible situations include:

* The 'preferred' driver has not correctly detected the screen resolution
 or colour setting.
* You are using an EGA card with limited memory or with a non-EGA monitor.
* You are using a laptop with a dark-on-light LCD.

  The drivers are:

  SDPSC9.VGA: VGA 640x480, 16-colour. 
  SDPS89.VGA: VGA 640x480,  8-colour.
  SDPS49.VGA: VGA 640x480,  4-colour.
  SDPSM9.VGA: VGA or MCGA, 640x480 mono.
  SDEHF9.EGA: EGA 640x350 colour.
  SDEH19.EGA: EGA 640x350 mono. Use this if you have an EGA card with a
             monochrome monitor.
  SDELF9.EGA: EGA 640x200 colour. Use this if you have an EGA card with a
             CGA monitor, or an EGA card with only 64k RAM.
  SDEH89.EGA: EGA 640x350, reduced colour. 
  SDEL89.EGA: EGA 640x200, reduced colour. 
  SDCGA9.CGA: CGA 640x200 mono.
  SDRCG9.CGA: CGA 640x200, reverse video. Use this on systems with liquid
             crystal displays (such as the IBM Convertible or the Amstrad
             PPC512) which display in dark on light.
  SDVLF9.VGA: VGA 320x200 in 256 colours. See the notes on this driver below.
  SDCLF9.VGA: CGA 320x200 in four colours. 
  SDCLR9.VGA: CGA 320x200 in four colours, reverse video.

The included Desk Accessory (FIXPAL.ZIP) allows you to switch between the 
magenta/cyan/white and red/green/yellow palettes in the 4-colour drivers.
 
  Compared to the original GEM drivers, these drivers have the following 
changes:

* Loadable codepages, using code from ViewMAX II. To get this working, you 
  need to install GEM.CPI in the same place as GEMVDI.EXE, and then use CHCP 
  to select the correct codepage before starting GEM. The 320x200 drivers
  don't support loadable codepages; they have a hardcoded ISO-8859-15 font.

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


320x200 driver notes
====================

* As mentioned above, the 320x200 drivers lack codepage support. This is 
 because they use 5-pixel fonts (based on the X11 font 

 -Misc-Fixed-Medium-R-Normal--7-70-75-75-C-50-ISO8859-15

 ) and GEM.CPI doesn't yet contain any 5-pixel fonts.

* Even with 5-pixel fonts, the screen is only 64 characters wide; so there
 isn't enough room for the BASIC2 menu bar, for example.

* Bitmaps and the mouse pointers are much bigger than the text, which can
 upset things like 'about' boxes. The desktop icons are bigger again, because
 the Desktop uses the low-resolution set (which are intended for a low 
 _vertical_ resolution but a high _horizontal_ resolution).

* Paint won't run, because it reckons the screen is too small.

* If you're using FreeGEM, it's best to turn off the 3D effects and use
  solid colour rather than textures, or things get very grainy.

