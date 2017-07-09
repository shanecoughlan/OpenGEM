RASMOPT = $szpz

clean:
	del *.obj
	del *.sym
	del *.lst
	del *.map

gemvdi.map:	gemvdi.sym
		sortmap <gemvdi.sym >e:\t1.tmp
		sort <e:\t1.tmp >e:\t2.tmp
		twocol <e:\t2.tmp >gemvdi.map
update:
	copy gemvdi.exe d:\gem\gemsys

gemvdi.exe:	entry.obj escape.obj control.obj fonts.obj headers.obj \
		driver.obj utility.obj dosutil.obj data.obj loader.obj \
		command.obj decode.obj stack.obj gemvdi.inp
	link86 gemvdi[nop]=gemvdi[i]
entry.obj:	entry.a86 equates.a86 externs.a86
	rasm86 entry $(RASMOPT)
escape.obj:	escape.a86 equates.a86 externs.a86
	rasm86 escape $(RASMOPT)
control.obj:	control.a86 equates.a86 externs.a86
	rasm86 control $(RASMOPT)
fonts.obj:	fonts.a86 equates.a86 externs.a86
	rasm86 fonts $(RASMOPT)
headers.obj:	headers.a86 equates.a86 externs.a86
	rasm86 headers $(RASMOPT)
driver.obj:	driver.a86 equates.a86 externs.a86
	rasm86 driver $(RASMOPT)
utility.obj:	utility.a86 equates.a86 externs.a86
	rasm86 utility $(RASMOPT)
dosutil.obj:	dosutil.a86 equates.a86 externs.a86
	rasm86 dosutil $(RASMOPT)
data.obj:	data.a86 equates.a86
	rasm86 data $(RASMOPT)
loader.obj:	loader.a86 equates.a86 externs.a86
	rasm86 loader $(RASMOPT)
command.obj:	command.a86 equates.a86 externs.a86
	rasm86 command $(RASMOPT)
decode.obj:	decode.a86 equates.a86 externs.a86
	rasm86 decode $(RASMOPT)
stack.obj:	stack.a86 equates.a86
	rasm86 stack $(RASMOPT)

gemvdi.xrf:	gemvdi.l86
	lib86 gemvdi.l86 [xref]
gemvdi.l86:	entry.obj escape.obj control.obj fonts.obj headers.obj \
		driver.obj utility.obj dosutil.obj data.obj loader.obj \
		command.obj stack.obj
	lib86 gemvdi.l86 =	entry.obj,escape.obj,control.obj,fonts.obj
	lib86 gemvdi.l86 = gemvdi.l86,headers.obj,driver.obj,utility.obj
	lib86 gemvdi.l86 = gemvdi.l86,dosutil.obj,data.obj,loader.obj
	lib86 gemvdi.l86 = gemvdi.l86,command.obj,stack.obj
