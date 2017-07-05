

SRCZIP=font.a86 cgafont.a86 cgaregs.a86 \
	 entry.a86 dummy.a86 debug.a86 \
	 jmptbl.a86 monobj.a86 monout.a86 monobjy.a86 \
	 imouse.a86 externs.a86 \
	 whmmdvsp.a86 wm4mdvsp.a86 \
	 monmmre1.a86 whmmmre2.a86 \
	 wm4mmre1.a86 wm4mmre2.a86 \
	 monspblt.a86 rastop.a86 \
	 whmmblit.a86 whmblit.a86 wm4mblit.a86 wm4blit.a86 \
	 copttext.a86 \
	 opttext.a86 opttxt1.a86 opttxt2.a86 opttdraw.a86  \
	 whpttxt1.a86 whpttxt2.a86 whptdraw.a86 whmpat.a86 \
	 wmpttxt1.a86 wmpttxt2.a86 wmptdraw.a86 wm4pat.a86 \
	 whm_zyxg.a86 wm4_zyxg.a86 codepage.a86 cgacp.a86 \
	 whm_drv.a86  wm4_drv.a86 \
	 whmregs.a86 whmdev.a86 \
	 wm4regs.a86 wm4dev.a86 \
	 isin.a86 enddseg.a86 date.a86 copyrt.a86 \
	 lorespat.a86 \
	 sdwhm9.inp sdwm49.inp \
	 readme.txt \
	 setmouse.c \
	 copying.txt Makefile

BINZIP = sdwhm9.cga sdwm49.cga setmouse.exe

all:	$(BINZIP)

#
# Ensure that the linker INP file is big enough - if it isn't you get 
# strange "Syntax error" messages. To be safe add 128 extra spaces to the
# end; LINK86 only reads up to the last multiple of 128 bytes.
#
sdwhm9.cga:	entry.obj jmptbl.obj monobjy.obj  \
		whm_zyxg.obj cgacp.obj monout.obj cgafont.obj \
		copttext.obj isin.obj dummy.obj whm_drv.obj \
		whmmblit.obj whmblit.obj whpttxt1.obj whpttxt2.obj \
		whptdraw.obj whmpat.obj enddseg.obj
	link86 sdwhm9[i

sdwm49.cga:	entry.obj jmptbl.obj monobjy.obj  \
		wm4_zyxg.obj cgacp.obj monout.obj cgafont.obj \
		copttext.obj isin.obj dummy.obj wm4_drv.obj \
		wm4mblit.obj wm4blit.obj wmpttxt1.obj wmpttxt2.obj \
		wmptdraw.obj wm4pat.obj enddseg.obj
	link86 sdwm49[i

%.obj:	%.a86 externs.a86
	rasm86 $< $$pzsz

whm_drv.obj:	whm_drv.a86 whmregs.a86 externs.a86 whmmdvsp.a86 imouse.a86 \
		monmmre1.a86 whmmmre2.a86 whmdev.a86 date.a86 copyrt.a86 
wm4_drv.obj:	wm4_drv.a86 wm4regs.a86 externs.a86 wm4mdvsp.a86 imouse.a86 \
		wm4mmre1.a86 wm4mmre2.a86 wm4dev.a86 date.a86 copyrt.a86 

monobjy.obj:	monobjy.a86 monobj.a86 externs.a86

whmmblit.obj:	whmmblit.a86 monspblt.a86
whmblit.obj:	whmblit.a86 rastop.a86
whpttxt1.obj:	whpttxt1.a86 opttxt1.a86
whpttxt2.obj:	whpttxt2.a86 opttxt2.a86
whptdraw.obj:	whptdraw.a86 opttdraw.a86
whmpat.obj:	whmpat.a86 lorespat.a86

wm4mblit.obj:	wm4mblit.a86
wm4blit.obj:	wm4blit.a86 
wmpttxt1.obj:	wmpttxt1.a86 opttxt1.a86
wmpttxt2.obj:	wmpttxt2.a86 opttxt2.a86
wmptdraw.obj:	wmptdraw.a86 
wm4pat.obj:	wm4pat.a86 lorespat.a86

copttext.obj:	copttext.a86 opttext.a86

cgacp.obj:	cgacp.a86 codepage.a86

setmouse.exe:	setmouse.c
	pacc -Bs $<

zip:	$(SRCZIP) $(BINZIP)
	zip wang.zip $(SRCZIP) $(BINZIP)
