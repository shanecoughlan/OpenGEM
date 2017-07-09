GEM/3 video drivers for the Wang Professional Computer
======================================================

  This archive contains two video drivers for the Wang Professional Computer,
allowing GEM or FreeGEM to be used with the PM-001 monochrome graphics card
or the 40x25 colour video card.

Installation
~~~~~~~~~~~~

  Because the Wang PC is not IBM-compatible, the installation method I 
recommend is to install GEM on a real PC (using the CGA mono 640x200 driver), 
and then copy the resulting program directories onto the Wang. Then edit
the resulting system as follows:

  For GEM/1 or GEM/2: 

1. Choose your driver: SDWHM9.CGA for the monochrome card, SDWM49.CGA for 
  the colour card. In the following instructions, replace 'SDxxx' with the
  name of your chosen driver.
2. Copy SDxxx9.CGA to /GEMSYS/SDxxx9.SYS
2. Edit ASSIGN.SYS and edit the line starting 01. Replace the existing
  driver name with SDxxx9. For example, change:

	01 IBMCHMP5 
to
	01 SDWHM9
or
	01 SDWM49


  For GEM/3 or FreeGEM: 

1. Choose your driver, as for GEM/1 or GEM/2.
1. Delete any SD* files in /GEMAPPS/GEMSYS. 
2. Copy your chosen driver file to /GEMAPPS/GEMSYS/

In use
~~~~~~

  Once GEM is up and running with the driver, it ought to operate as normal
with any well-behaved GEM program.

  By default, the driver is set up for a Microsoft-type serial mouse. To 
change this, use the provided SETMOUSE utility:

	SETMOUSE SDxxx9.CGA 0 		- no mouse
	SETMOUSE SDxxx9.CGA 1 		- Mouse Systems mouse on serial port
	SETMOUSE SDxxx9.CGA 2 		- External mouse driver using INT 33h
					 (eg, AEMOUSE.COM under QDAE)
	SETMOUSE SDxxx9.CGA 3 		- Microsoft serial mouse on serial port

Known Issues
~~~~~~~~~~~~

* There is no repeat on function keys or cursor keys, which makes the keyboard 
 mouse somewhat trying to use. Sorry.

* Sound support is untested.
