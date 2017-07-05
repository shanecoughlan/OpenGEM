; This file is a part of CuteMouse source
; Here are messages in Dutch
; Translated by Bernd Blaauw <bblnews@hotmail.com>

E_option	db 5,0dh,0ah,'Fout: Ongeldige optie',0dh,0ah
		db 'Voer /? in op opdrachtregel voor hulp',0dh,0ah,'$'
E_mousepresent	db 5,'Muis service reeds aanwezig',0dh,0ah,'$'
E_notfound	db 5,'Fout: apparaat niet gevonden',0dh,0ah,'$'

E_nocute	db 1,'CuteMouse stuurprogramma is niet geladen!',0dh,0ah,'$'
E_notunload	db 2,'ontladen stuurprogramma mislukt: interrupts onderbroken',0dh,0ah,'$'
S_unloaded	db 0,'Stuurprogramma is succesvol ontladen...',0dh,0ah,'$'

S_reset		db 'Residente gedeelte gereset naar $'
S_installed	db 'Geinstalleerd op $'
S_atPS2		db 'PS/2 poort$'
S_atCOM		db 'COM'
com_port	db ?,' ('
S_atIO		db '0000h/IRQ'
IRQno		db ?
S_in		db ') in $'
S_inMSYS	db 'Mouse Systems mode$'
S_inMS		db 'Microsoft mode$'
S_inLT		db 'Logitech mode$'
S_wheel		db ' (muiswiel aanwezig)'
S_CRLF		db 0dh,0ah,'$'

Copyright	db 0dh,0ah,'CuteMouse v',CTMRELEASE,' [FreeDOS]',0dh,0ah,'$'
Syntax	db 0,'Opties:',0dh,0ah
	db '  /P	   - dwing PS/2 muis-modus af',0dh,0ah
	db '  /S[c[i]] - dwing seriele muis-modus af op COM poort c (c=1-4, standaard is COM1)',0dh,0ah
	db '	     met IRQ lijn i (i=2-7, standaard is IRQ4 voor COM1/3 en IRQ3',0dh,0ah
	db '	     voor COM2/4)',0dh,0ah
;;+	db '  /Aia	   - dwing seriele muis-modus af op I/O adres a (hexadecimaal getal)',0dh,0ah
;;+	db '	     met IRQ lijn i (i=2-7)',0dh,0ah
	db '  /Y	   - voorkom Mouse Systems modus',0dh,0ah
	db '  /V	   - omgekeerd zoeken: PS/2 na seriele muis vinden',0dh,0ah
	db '  /3	   - dwing 3-knops modus af (alleen voor Microsoft en PS/2 muizen)',0dh,0ah
	db '  /R[h[v]] - horizontale/verticale resolutie (h,v=0-9; ontbrekende optie R, geen',0dh,0ah
	db '	     argumenten of 0 als argument na R betekent auto-resolutie, ontbrekend',0dh,0ah
	db '	     tweede argument houdt dezelfde waarde als voor het eerste argument in)',0dh,0ah
	db '  /L	   - linkshandige modus (standaard ingesteld op rechtshandig)',0dh,0ah
	db '  /B	   - niet laden als een muis-stuurprogramma al geladen is',0dh,0ah
	db '  /W	   - voorkomt het laden van deze TSR in UMBs',0dh,0ah
	db '  /U	   - stuurprogramma deactiveren',0dh,0ah
	db '  /?	   - toont dit helpscherm',0dh,0ah,'$'
