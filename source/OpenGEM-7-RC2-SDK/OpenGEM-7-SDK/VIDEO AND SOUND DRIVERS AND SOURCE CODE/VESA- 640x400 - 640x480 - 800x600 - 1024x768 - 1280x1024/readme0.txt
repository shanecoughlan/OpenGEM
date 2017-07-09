FreeGEM video drivers
=====================

  Recent changes: 

* New VESA drivers for 640x480x256, 800x600x256 and 1280x1024x256 resolutions 

* All 256-colour drivers now use a packed-pixel format for native bitmaps,
 which speeds up blitting by a factor of 8 or so.

  This zipfile contains seven FreeGEM video drivers for systems with video
cards that support a VESA BIOS. If your PC is so old that it lacks VESA 
support, then you should try one of the other driver sets, which should be
available wherever you got this one.

  The seven drivers are:

Name       | Resolution | Colours | Notes
===========|============|=========|===========================================
SDV869.VGA | 800x600    | 16      |
           |            |         |
SDV109.VGA | 1024x768   | 16      | This mode is not widely supported. Known 
           |            |         | to work on a Cirrus Logic CL-GD5436 card 
           |            |         | and an ATI Mach32 chipset.
           |            |         |
SDW649.VGA | 640x480    | 256     | Also supports 640x400 resolution.
           |            |         |
SDW869.VGA | 800x600    | 256     |
           |            |         |
SDW109.VGA | 1024x768   | 256     |
           |            |         |
SDW129.VGA | 1280x1024  | 256     |
           |            |         |
SDW6X9.VGA | 640x480    | 256     | Alternative to SDW649 that may have 
           |            |         | slightly better performance, but requires 
           |            |         | a chipset with 4k memory window granularity
           |            |         | - eg, Paradise or Cirrus Logic chipsets.  
============================================================================== 

  The two 640x480 drivers (SDW649 and SDW6X9) also support 640x400 resolution.
By default, they will only select this if 640x480 is not available. It can be
selected manually by using VIDPATCH.EXE:

                VIDPATCH SDW649.VGA 95    -- forces 640x480 mode
                VIDPATCH SDW649.VGA 94    -- forces 640x400 mode
                VIDPATCH SDW649.VGA AUTO  -- restores autodetection.

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

256-colour driver compatibility issues
======================================

* Most programs (including, I'm afraid, anything I've written that uses 
  colour) won't accept that there are more than 16 colours available. However,
  recent versions of the GEM settings program (SETTINGS.APP) do support 
  256-colour drivers. Developers should also be aware that AES object 
  definitions only allow 16 colours, since they use 4-bit colour numbers. 
  If you want any more, you'll have to use G_USERDEF objects or ViewMAX-style 
  colour categories.

* High resolutions make GEM very memory-hungry (because all the bitmaps it
  needs to store are suddenly twice as big, or bigger, than in VGA mode). The
  drawing programs (Doodle and Paint) try to allocate large areas of memory,
  fail, and refuse to run.

* Some programs were written with the assumption that GEM would never have
  more than 16 colours. These will either crash or fail to run when a 
  256-colour driver is in operation.
