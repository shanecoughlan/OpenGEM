; This file is a part of CuteMouse source
; Here are messages in German
; Translated by Frank PrÅfer <fp666@t-online.de>
;
; Um CuteMouse mit den deutschen Meldungen zu kompilieren, einfach diese Datei
; in CTMOUSE.MSG umbenennen und CTMOUSE.ASM gemÑ· den Hinweisen in der Datei
; CTMOUSE.TXT neu kompilieren. Dabei mÅssen sich CTMOUSE.ASM und CTMOUSE.MSG
; in demselben Verzeichnis befinden.

E_option	db 5,0dh,0ah,'FEHLER: UngÅltige Option! Geben Sie /? auf der',0dh,0ah
		db 'Kommandozeilenebene ein, um Hilfe zu erhalten.',0dh,0ah,'$'
E_mousepresent	db 5,'Mausfunktionen sind bereits aktiv.',0dh,0ah,'$'
E_notfound	db 5,'FEHLER: Keine Maus gefunden!',0dh,0ah,'$'

E_nocute	db 1,'Treiber wurde nicht installiert!',0dh,0ah,'$'
E_notunload	db 2,'Treiber konnte nicht entfernt werden: einige Interrupts',0dh,0ah
		db 'sind von spÑter installierten Programmen belegt...',0dh,0ah,'$'
S_unloaded	db 0,'Treiber wurde erfolgreich entfernt...',0dh,0ah,'$'

S_reset		db 'Residenter Teil zurÅckgesetzt auf $'
S_installed	db 'Installiert an $'
S_atPS2		db 'PS/2-Port$'
S_atCOM		db 'COM'
com_port	db ?,' ('
S_atIO		db '0000h/IRQ'
IRQno		db ?
S_in		db ') im $'
S_inMSYS	db 'Mouse Systems Modus$'
S_inMS		db 'Microsoft Modus$'
S_inLT		db 'Logitech Modus$'
S_wheel		db ' (mit Rad)'
S_CRLF		db 0dh,0ah,'$'

Copyright	db 0dh,0ah,'CuteMouse v',CTMRELEASE,' [FreeDOS]',0dh,0ah,'$'
Syntax	db 0,'Optionen:',0dh,0ah
	db '  /P	   - PS/2-Mausmodus erzwingen',0dh,0ah
	db '  /S[c[i]] - seriellen Mausmodus erzwingen am COM-Port c (c=1...4, Vorgabe',0dh,0ah
	db '	     ist COM1) mit IRQ i (i=2...7, Vorgabe ist IRQ4 fÅr COM1/3',0dh,0ah
	db '	     sowie IRQ3 fÅr COM2/4)',0dh,0ah
;;+	db '  /Aia	   - seriellen Mausmodus erzwingen an I/O-Adresse a (hexadezimal)',0dh,0ah
;;+	db '	     mit IRQ i (i=2...7)',0dh,0ah
	db '  /Y	   - Mouse Systems Modus verbieten',0dh,0ah
	db '  /V	   - Suche umkehren: PS/2 nach serieller Maus finden',0dh,0ah
	db '  /3	   - 3-Tasten-Modus erzwingen (nur fÅr Microsoft- und PS/2-MÑuse)',0dh,0ah
	db '  /R[h[v]] - horizontale/vertikale Auflîsung (h,v=0...9; fehlende Option R,',0dh,0ah
	db '	     bzw. keine Argumente oder 0 als Argument nach R, ergeben',0dh,0ah
	db '	     automatische Auflîsung; ein fehlendes zweites Argument bedeutet',0dh,0ah
	db '	     denselben Wert wie fÅr das erste Argument)',0dh,0ah
	db '  /L	   - LinkshÑndermodus (Vorgabe ist RechtshÑndermodus)',0dh,0ah
	db '  /B	   - Programm abbrechen, wenn Mausfunktionen bereits aktiv sind',0dh,0ah
	db '  /W	   - TSR nicht automatisch in den UMB laden',0dh,0ah
	db '  /U	   - CuteMouse Treiber deinstallieren',0dh,0ah
	db '  /?	   - diese Hilfe anzeigen',0dh,0ah,'$'
