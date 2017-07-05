; This file is a part of CuteMouse source
; Here are messages in Hungarian
; Translated by Nagy Daniel <nagyd@users.sourceforge.net>

E_option	db 5,0dh,0ah,'Hiba: helytelen opci¢',0dh,0ah
		db 'A /? parancs ad tov†bbi seg°tsÇget',0dh,0ah,'$'
E_mousepresent	db 5,'Az egÇrkezelÇs m†r telep°tve van',0dh,0ah,'$'
E_notfound	db 5,'Hiba: az eszkîz nem tal†lhat¢',0dh,0ah,'$'

E_nocute	db 1,'CuteMouse meghajt¢ nincs telep°tve!',0dh,0ah,'$'
E_notunload	db 2,'EgÇrmeghajt¢ nem t†vol°that¢ el: a megszak°t†sokat m†s program haszn†lja...',0dh,0ah,'$'
S_unloaded	db 0,'EgÇrmeghajt¢ sikeresen elt†vol°tva...',0dh,0ah,'$'

S_reset		db 'Rezidens rÇsz †t†ll°tva: $'
S_installed	db 'Telep°tve: $'
S_atPS2		db 'PS/2 port$'
S_atCOM		db 'COM'
com_port	db ?,' ('
S_atIO		db '0000h/IRQ'
IRQno		db ?
S_in		db ') in $'
S_inMSYS	db 'Mouse Systems m¢d$'
S_inMS		db 'Microsoft m¢d$'
S_inLT		db 'Logitech m¢d$'
S_wheel		db ' (gîrgetìgomb t†mogatva)'
S_CRLF		db 0dh,0ah,'$'

Copyright	db 0dh,0ah,'CuteMouse v',CTMRELEASE,' [FreeDOS]',0dh,0ah,'$'
Syntax	db 0,'Opci¢k:',0dh,0ah
	db '  /P	   - PS/2 egÇr m¢d',0dh,0ah
	db '  /S[c[i]] - soros egÇr m¢d a c COM porton (c=1-4, COM1 az alapbe†ll°t†s)',0dh,0ah
	db '	     az i IRQ megszak°t†son (i=2-7, az alapbe†ll°t†s COM1/3 esetÇn IRQ4',0dh,0ah
	db '	     Çs COM2/4 esetÇn IRQ3)',0dh,0ah
;;+	db '  /Aia	   - soros egÇr m¢d az a I/O c°men (hexadecim†lis sz†m)',0dh,0ah
;;+	db '	     Çs i IRQ megszak°t†son (i=2-7)',0dh,0ah
	db '  /Y	   - Mouse Systems m¢d tilt†sa',0dh,0ah
	db '  /V	   - ford°tott keresÇs: soros portok majd PS/2 port vizsg†lata',0dh,0ah
	db '  /3	   - 3 gombos m¢d (Microsoft Çs PS/2 egÇr esetÇn)',0dh,0ah
	db '  /R[h[v]] - horizont†lis/vertik†lis felbont†s (h,v=0-9; hi†nyz¢ R opci¢,',0dh,0ah
	db '	     hi†nyz¢ argumentum vagy 0 argumentum esetÇn automatikus felbont†s;',0dh,0ah
	db '	     hi†nyz¢ m†sodik argumentum esetÇn horizont†lis ÇrtÇk haszn†lata)',0dh,0ah
	db '  /L	   - balkezes m¢d (jobbkezes m¢d az alapbe†ll°t†s)',0dh,0ah
	db '  /B	   - telep°tÇs megakad†lyoz†sa, ha m†sik egÇrmeghajt¢ telep°tve van',0dh,0ah
	db '  /W	   - UMB haszn†lat†nak tilt†sa',0dh,0ah
	db '  /U	   - egÇrmeghajt¢ elt†vol°t†sa',0dh,0ah
	db '  /?	   - ez a help Åzenet',0dh,0ah,'$'
