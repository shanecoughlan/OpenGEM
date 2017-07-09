The GSANS font                                   John Elliott, 4 September 2006
===============================================================================

  This file contains the following three codepage files:

GSANS.CPX
~~~~~~~~~
  GSANS.CPX is a UPX-compressed DRFONT codepage containing 25 codepages: 
437, 850, 852, 857, 858, 860, 863, 865, 866, 819, 912, 913, 914, 919, 920,
28591, 28592, 28593, 28594, 28595, 28597, 28599, 28600, 28603 and 28605.
Note that some of these are aliases -- codepage 819 is the same as 28591, 
91x is the same as 2859x, and 920 is the same as 28600.

  At the time of writing, no known version of MODE.COM can handle a 
UPX-compressed DRFONT (though there is a possibility that this support may be 
added to the FreeDOS MODE.COM in the future). I suggest instead using CPPREP 
<http://www.seasip.info/DOS/CPI/> either to replace the MODE CON CP PREPARE 
command:

	CPPREP 437=GSANS.CPX 850=GSANS.CPX 819=GSANS.CPX

 or to create a standard-format CPI that your version of MODE can use:

	CPPREP -Dmy.cpi 437=GSANS.CPX 850=GSANS.CPX 819=GSANS.CPX
	MODE CON CP PREPARE=((437,850,819) my.cpi)

  You'll still have to do the MODE CON CP SELECT afterwards, as normal.

GSANS.CPI
~~~~~~~~~
  This file has the same codepages as GSANS.CPX, but is not UPX compressed. 
You may find this useful if:

1. You're using DRDOS. In this case you can just copy GSANS.CPI over the 
  default EGA.CPI and CPPREP is not needed. If renamed to EGA.CPI, it will
  also be used by ViewMAX if the system codepage is not 437.

2. You're using Windows NT. If you install PSFTOOLS 
  <http://www.seasip.info/Unix/PSF/> then it's possible to convert GSANS.CPI
  to Windows NT format:
	
	cpidcomp --nt gsans.cpi ega.cpi

  and then copy the resulting EGA.CPI file to your \WINNT\SYSTEM32 directory.

3. You're using other utilities (like PSFTOOLS) which understand DRFONTs but
  not UPX compression.

GEM.CPI
~~~~~~~
  The original reason this font was created. It contains a subset of the 
fonts in GSANS.CPI (13 codepages) with the first 32 characters replaced by
the window gadgets used by GEM. If you use the latest video drivers 
(SDPSC9.VGA or SD256.VGA) this file is used by them for codepage support. 
The codepages included are 437, 850, 858, 852, 857, 860, 863, 865, 866, 819,
28591, 28605 and the original GEM font (which has been included as 
codepage 65280; codepage numbers 65280-65533 are allocated for custom 
codepages by IBM).

A size comparison
~~~~~~~~~~~~~~~~~

  This compares the number of files, and their sizes, needed to store 
25 codepages in various formats:

File format  | Number of files | File size | Notes
=============+=================+===========+===========================
Standard CPI |               5 |     58870 | Total size therefore ~290k
Windows NT   |               1 |    283075 |
DRFONT       |               1 |     38540 | As supplied in GSANS.CPI
DRFONT + UPX |               1 |     11926 | As supplied in GSANS.CPX
=============+=================+===========+===========================


How to make altered versions
============================

  It's entirely possible that you'll want to change the fonts I've provided,
either because I got the shape of one or more characters wrong, or because
I got the Unicode mapping wrong.

  Firstly, you'll need the source code for the CPI files, which should be
downloadable from <http://www.seasip.info/DOS/CPI/>.

  If you want to change the character shapes, it's quite easy - edit the
appropriate .txt file (gsans*.txt), and rerun the build process described
below.
  If you want to change the Unicode mappings for GEM.CPI, this is also 
quite easy - each codepage has a matching .UNI file, saying which Unicode 
character should be put in each slot. 
  Changing the mapping for GSANS.CPI is more difficult, because the codepages 
are builtin to PSFTOOLS. What you have to do is one of:

  1. Create a new .UNI file and change the input to psfs2cpi to use it. For 
    example, if you wanted to change the mapping in codepage 863, you'd change
    the entries in Makefile and the .inp files from "+863 gsans16.psf ..." to 
    "+863=my863.uni gsans16.psf ..."

  2. Change the mapping tables in the PSFTOOLS source, rebuild PSFTOOLS, and
    use the updated version of PSFS2CPI in the build. 

Build process
~~~~~~~~~~~~~

  Under UNIX: Install psftools-1.0.4 or later, from
<http://www.seasip.info/Unix/PSF/>. Type 'make'.

  Under DOS: Run mkcpi.bat. You will need the GO32 DOS extender, because the
build programs were compiled with DJGPP.

Where the fonts came from
=========================

  The 8x14 and 8x8 fonts originally came from GEM. The 8x16 font was created
by vertically stretching the 8x14 font. The 6x6 font has been altered so
much I'm no longer sure if it was based on the GEM 6x6 font or created from
scratch. In any case, all four fonts have had lots and lots of characters 
designed and added by me.
  In jurisdictions where bitmap fonts can be copyrighted, these are therefore

    Copyright 2006, John Elliott
    Copyright 1999, Caldera Thin Clients, Inc. 

