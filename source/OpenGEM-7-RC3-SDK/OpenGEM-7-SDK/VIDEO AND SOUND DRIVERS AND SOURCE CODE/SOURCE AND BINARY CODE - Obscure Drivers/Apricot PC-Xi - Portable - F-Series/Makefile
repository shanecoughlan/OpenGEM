

SRCZIP=font.a86 \
	 cgafont.a86 cgaregs.a86 cgacp.a86 \
	 egafont.a86 egaregs.a86 egacp.a86 \
	 dummy.a86 \
	 jmptbl.a86 monobj.a86 monout.a86 monobjy.a86 \
	 fpmouse.a86 ximouse.a86 externs.a86 detect.a86 scanxlt.a86 \
	 aprfdvsp.a86 aximdvsp.a86 entry.a86 \
	 aprfmre1.a86 aprfmre2.a86 aximmre1.a86 aximmre2.a86 \
	 monspblt.a86 rastop.a86 \
	 fp4entry.a86 fp4mblit.a86 fp4blit.a86 \
	 fp3entry.a86 fp3mblit.a86 fp3blit.a86 \
	 fp2entry.a86 fp2mblit.a86 fp2blit.a86 \
	 fpmentry.a86 fpmmblit.a86 fpmblit.a86 \
	 af1mblit.a86 af1blit.a86 \
	 aximblit.a86 axiblit.a86 \
	 copttext.a86 codepage.a86 \
	 opttext.a86 opttxt1.a86 opttxt2.a86 opttdraw.a86 \
	 lorespat.a86 hirespat.a86 \
	 f4pttxt1.a86 f4pttxt2.a86 f4ptdraw.a86 fp4pat.a86 \
	 f3pttxt1.a86 f3pttxt2.a86 f3ptdraw.a86 fp3pat.a86 \
	 f2pttxt1.a86 f2pttxt2.a86 f2ptdraw.a86 fp2pat.a86 \
	 fmpttxt1.a86 fmpttxt2.a86 fmptdraw.a86 fpmpat.a86 \
	 a1pttxt1.a86 a1pttxt2.a86 a1ptdraw.a86 af1pat.a86 \
	 axpttxt1.a86 axpttxt2.a86 axptdraw.a86 axipat.a86 \
	 fp4_zyxg.a86 fp4_drv.a86 fp4regs.a86 fp4dev.a86 \
	 fp3_zyxg.a86 fp3_drv.a86 fp3regs.a86 fp3dev.a86 \
	 fp2_zyxg.a86 fp2_drv.a86 fp2regs.a86 fp2dev.a86 \
	 fpm_zyxg.a86 fpm_drv.a86 fpmregs.a86 fpmdev.a86 \
	 af1_zyxg.a86 af1_drv.a86 af1regs.a86 af1dev.a86 \
	 axi_zyxg.a86 axi_drv.a86 axiregs.a86 axidev.a86 \
	 isin.a86 enddseg.a86 date.a86 copyrt.a86 gem.cpi \
	 sdfp49.inp sdfp39.inp sdfp29.inp sdfpm9.inp sdaf19.inp sdaxi9.inp \
	 readme.txt \
	 vidpatch.exe vidpatch.c \
	 copying.txt Makefile

BINZIP = sdfp49.cga sdfp39.cga sdfp29.cga sdfpm9.cga sdaf19.cga sdaxi9.ega

all:	$(BINZIP)

#
# Ensure that the linker INP file is big enough - if it isn't you get 
# strange "Syntax error" messages. To be safe add 128 extra spaces to the
# end; LINK86 only reads up to the last multiple of 128 bytes.
#
sdfp49.cga:	fp4entry.obj jmptbl.obj monobjy.obj  \
		fp4_zyxg.obj cgacp.obj monout.obj cgafont.obj \
		copttext.obj isin.obj dummy.obj fp4_drv.obj \
		fp4mblit.obj fp4blit.obj f4pttxt1.obj f4pttxt2.obj \
		f4ptdraw.obj fp4pat.obj enddseg.obj
	link86 sdfp49[i

sdfp39.cga:	fp3entry.obj jmptbl.obj monobjy.obj  \
		fp3_zyxg.obj cgacp.obj monout.obj cgafont.obj \
		copttext.obj isin.obj dummy.obj fp3_drv.obj \
		fp3mblit.obj fp3blit.obj f3pttxt1.obj f3pttxt2.obj \
		f3ptdraw.obj fp3pat.obj enddseg.obj
	link86 sdfp39[i

sdfp29.cga:	fp2entry.obj jmptbl.obj monobjy.obj  \
		fp2_zyxg.obj cgacp.obj monout.obj cgafont.obj \
		copttext.obj isin.obj dummy.obj fp2_drv.obj \
		fp2mblit.obj fp2blit.obj f2pttxt1.obj f2pttxt2.obj \
		f2ptdraw.obj fp2pat.obj enddseg.obj
	link86 sdfp29[i

sdfpm9.cga:	fpmentry.obj jmptbl.obj monobjy.obj  \
		fpm_zyxg.obj cgacp.obj monout.obj cgafont.obj \
		copttext.obj isin.obj dummy.obj fpm_drv.obj \
		fpmmblit.obj fpmblit.obj fmpttxt1.obj fmpttxt2.obj \
		fmptdraw.obj fpmpat.obj enddseg.obj
	link86 sdfpm9[i

sdaf19.cga:	fp2entry.obj jmptbl.obj monobjy.obj  \
		af1_zyxg.obj cgacp.obj monout.obj cgafont.obj \
		copttext.obj isin.obj dummy.obj af1_drv.obj \
		af1mblit.obj af1blit.obj a1pttxt1.obj a1pttxt2.obj \
		a1ptdraw.obj af1pat.obj enddseg.obj
	link86 sdaf19[i

sdaxi9.ega:	entry.obj jmptbl.obj monobjy.obj  \
		axi_zyxg.obj egacp.obj monout.obj egafont.obj \
		copttext.obj isin.obj dummy.obj axi_drv.obj \
		aximblit.obj axiblit.obj axpttxt1.obj axpttxt2.obj \
		axptdraw.obj axipat.obj enddseg.obj
	link86 sdaxi9[i


%.obj:	%.a86 externs.a86
	rasm86 $< $$pzsz

fp4_drv.obj:	fp4_drv.a86 fp4regs.a86 externs.a86 aprfdvsp.a86 fpmouse.a86 \
		aprfmre1.a86 aprfmre2.a86 fp4dev.a86 date.a86 copyrt.a86 \
		detect.a86 scanxlt.a86
fp3_drv.obj:	fp3_drv.a86 fp3regs.a86 externs.a86 aprfdvsp.a86 fpmouse.a86 \
		aprfmre1.a86 aprfmre2.a86 fp3dev.a86 date.a86 copyrt.a86 \
		detect.a86 scanxlt.a86
fp2_drv.obj:	fp2_drv.a86 fp2regs.a86 externs.a86 aprfdvsp.a86 fpmouse.a86 \
		aprfmre1.a86 aprfmre2.a86 fp2dev.a86 date.a86 copyrt.a86 \
		detect.a86 scanxlt.a86
fpm_drv.obj:	fpm_drv.a86 fpmregs.a86 externs.a86 aprfdvsp.a86 fpmouse.a86 \
		aprfmre1.a86 aprfmre2.a86 fpmdev.a86 date.a86 copyrt.a86 \
		detect.a86 scanxlt.a86
af1_drv.obj:	af1_drv.a86 af1regs.a86 externs.a86 aprfdvsp.a86 fpmouse.a86 \
		aprfmre1.a86 aprfmre2.a86 af1dev.a86 date.a86 copyrt.a86 \
		detect.a86 scanxlt.a86
axi_drv.obj:	axi_drv.a86 axiregs.a86 externs.a86 aximdvsp.a86 ximouse.a86 \
		aximmre1.a86 aximmre2.a86 axidev.a86 date.a86 copyrt.a86 


monobjy.obj:	monobjy.a86 monobj.a86 externs.a86

fp4mblit.obj:	fp4mblit.a86 monspblt.a86
fp4blit.obj:	fp4blit.a86 rastop.a86
f4pttxt1.obj:	f4pttxt1.a86 opttxt1.a86
f4pttxt2.obj:	f4pttxt2.a86 opttxt2.a86
f4ptdraw.obj:	f4ptdraw.a86 opttdraw.a86
fp4pat.obj:	fp4pat.a86 lorespat.a86

fp3mblit.obj:	fp3mblit.a86 monspblt.a86
fp3blit.obj:	fp3blit.a86 rastop.a86
f3pttxt1.obj:	f3pttxt1.a86 opttxt1.a86
f3pttxt2.obj:	f3pttxt2.a86 opttxt2.a86
f3ptdraw.obj:	f3ptdraw.a86 opttdraw.a86
fp3pat.obj:	fp3pat.a86 lorespat.a86

fp2mblit.obj:	fp2mblit.a86 monspblt.a86
fp2blit.obj:	fp2blit.a86 rastop.a86
f2pttxt1.obj:	f2pttxt1.a86 opttxt1.a86
f2pttxt2.obj:	f2pttxt2.a86 opttxt2.a86
f2ptdraw.obj:	f2ptdraw.a86 opttdraw.a86
fp2pat.obj:	fp2pat.a86 lorespat.a86

fpmmblit.obj:	fpmmblit.a86 monspblt.a86
fpmblit.obj:	fpmblit.a86 rastop.a86
fmpttxt1.obj:	fmpttxt1.a86 opttxt1.a86
fmpttxt2.obj:	fmpttxt2.a86 opttxt2.a86
fmptdraw.obj:	fmptdraw.a86 opttdraw.a86
fpmpat.obj:	fpmpat.a86 lorespat.a86

axipat.obj:	axipat.a86 hirespat.a86


copttext.obj:	copttext.a86 opttext.a86
popttext.obj:	popttext.a86 opttext.a86

zip:	$(SRCZIP) $(BINZIP)
	zip apricot3.zip $(SRCZIP) $(BINZIP)
