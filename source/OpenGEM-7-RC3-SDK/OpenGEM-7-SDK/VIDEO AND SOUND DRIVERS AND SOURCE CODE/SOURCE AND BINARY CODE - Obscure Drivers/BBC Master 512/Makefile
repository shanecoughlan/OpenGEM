

SRCZIP=font.a86 cgafont.a86 cgaregs.a86 bbcfont.a86 \
	 muentry.a86 dummy.a86 debug.a86 \
	 jmptbl.a86 monobj.a86 monout.a86 monobjy.a86 \
	 bbcmouse.a86 externs.a86 update.a86 \
	 ibmmdvsp.a86 \
	 monmmre1.a86 monmmre2.a86 \
	 monspblt.a86 rastop.a86 \
	 bbmmblit.a86 bbmblit.a86 \
	 bbcmblit.a86 bbcblit.a86 \
	 copttext.a86 \
	 opttext.a86 opttxt1.a86 opttxt2.a86 opttdraw.a86 lorespat.a86 \
	 bmpttxt1.a86 bmpttxt2.a86 bmptdraw.a86 bbmpat.a86 \
	 bcpttxt1.a86 bcpttxt2.a86 bcptdraw.a86 bbcpat.a86 \
	 bbm_zyxg.a86 bbmcp.a86 bbc_zyxg.a86 \
	 bbm_drv.a86 bbmregs.a86 bbmdev.a86 \
	 bbc_drv.a86 bbcregs.a86 bbcdev.a86 \
	 isin.a86 enddseg.a86 date.a86 copyrt.a86 \
	 sdbbm9.inp \
	 sdbbc9.inp \
	 readme.txt \
	 copying.txt Makefile

BINZIP = sdbbm9.cga sdbbc9.cga 

all:	$(BINZIP)

#
# Ensure that the linker INP file is big enough - if it isn't you get 
# strange "Syntax error" messages. To be safe add 128 extra spaces to the
# end; LINK86 only reads up to the last multiple of 128 bytes.
#
sdbbm9.cga:	muentry.obj jmptbl.obj monobjy.obj  \
		bbm_zyxg.obj bbmcp.obj monout.obj cgafont.obj \
		copttext.obj isin.obj dummy.obj bbm_drv.obj \
		bbmmblit.obj bbmblit.obj bmpttxt1.obj bmpttxt2.obj \
		bmptdraw.obj bbmpat.obj enddseg.obj
	link86 sdbbm9[i

sdbbc9.cga:	muentry.obj jmptbl.obj monobjy.obj  \
		bbc_zyxg.obj bbmcp.obj monout.obj bbcfont.obj \
		copttext.obj isin.obj dummy.obj bbc_drv.obj \
		bbcmblit.obj bbcblit.obj bcpttxt1.obj bcpttxt2.obj \
		bcptdraw.obj bbcpat.obj enddseg.obj
	link86 sdbbc9[i

%.obj:	%.a86 externs.a86
	rasm86 $< $$pzsz

bbm_drv.obj:	bbm_drv.a86 bbmregs.a86 externs.a86 ibmmdvsp.a86 bbcmouse.a86 \
		monmmre1.a86 monmmre2.a86 bbmdev.a86 date.a86 copyrt.a86 \
		update.a86
bbc_drv.obj:	bbc_drv.a86 bbcregs.a86 externs.a86 ibmmdvsp.a86 bbcmouse.a86 \
		monmmre1.a86 monmmre2.a86 bbcdev.a86 date.a86 copyrt.a86 \
		update.a86

monobjy.obj:	monobjy.a86 monobj.a86 externs.a86

bbmmblit.obj:	bbmmblit.a86 monspblt.a86
bbmblit.obj:	bbmblit.a86 rastop.a86
bmpttxt1.obj:	bmpttxt1.a86 opttxt1.a86
bmpttxt2.obj:	bmpttxt2.a86 opttxt2.a86
bmptdraw.obj:	bmptdraw.a86 opttdraw.a86
bbmpat.obj:	bbmpat.a86 lorespat.a86

bbcmblit.obj:	bbcmblit.a86 monspblt.a86
bbcblit.obj:	bbcblit.a86 rastop.a86
bcpttxt1.obj:	bcpttxt1.a86 opttxt1.a86
bcpttxt2.obj:	bcpttxt2.a86 opttxt2.a86
bcptdraw.obj:	bcptdraw.a86 opttdraw.a86
bbcpat.obj:	bbcpat.a86 lorespat.a86

copttext.obj:	copttext.a86 opttext.a86

copttext.obj:	copttext.a86 opttext.a86
popttext.obj:	popttext.a86 opttext.a86

zip:	$(SRCZIP) $(BINZIP)
	zip acorn3.zip $(SRCZIP) $(BINZIP)
