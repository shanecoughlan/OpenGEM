; This file is a part of CuteMouse source
; Here are messages in Italian
; Translated by Massimo Giussani <massimo@poboxes.com>

E_option	db 5,0dh,0ah,'Errore: Opzione invalida',0dh,0ah
		db "Per l'aiuto digitare /? sulla linea di comando",0dh,0ah,'$'
E_mousepresent	db 5,'Servizi di Mouse gi… presenti',0dh,0ah,'$'
E_notfound	db 5,'Errore: dispositivo non trovato',0dh,0ah,'$'

E_nocute	db 1,'Il driver CuteMouse non Š stato installato!',0dh,0ah,'$'
E_notunload	db 2,'Disinstallazione fallita: alcuni interrupts sono stati intercettati!',0dh,0ah,'$'
S_unloaded	db 0,'Driver disinstallato',0dh,0ah,'$'

S_reset		db 'Parte residente impostata a $'
S_installed	db 'Installato su $'
S_atPS2		db 'Porta PS/2$'
S_atCOM		db 'COM'
com_port	db ?,' ('
S_atIO		db '0000h/IRQ'
IRQno		db ?
S_in		db ') in $'
S_inMSYS	db 'Mouse Systems mode$'
S_inMS		db 'Microsoft mode$'
S_inLT		db 'Logitech mode$'
S_wheel		db ' (wheel present)'
S_CRLF		db 0dh,0ah,'$'

Copyright	db 0dh,0ah,'CuteMouse v',CTMRELEASE,' [FreeDOS]',0dh,0ah,'$'
Syntax	db 0,'Opzioni:',0dh,0ah
	db '  /P	   - forza modalit… PS/2',0dh,0ah
	db '  /S[c[i]] - forza modalit… seriale alla porta COM c (c=1-4, per',0dh,0ah
	db '	     default Š COM1) con linea IRQ i (i=2-7, per default Š IRQ4',0dh,0ah
	db '	     sulla COM1/3 e IRQ3 sulla COM2/4)',0dh,0ah
;;+	db "  /Aia	   - forza modalit… seriale all'indirizzo I/O a (in esadecimale)",0dh,0ah
;;+	db '	     con linea IRQ i (i=2-7)',0dh,0ah
	db '  /Y	   - impedisce modalit… Mouse Systems',0dh,0ah
	db '  /V	   - inverte la ricerca: cerca il mouse PS/2 dopo quello seriale',0dh,0ah
	db '  /3	   - forza modo 3-pulsanti (solo per mouse Microsoft e PS/2)',0dh,0ah
	db "  /R[h[v]] - risoluzione orizzontale/verticale (h,v=0-9; senza l'opzione R,",0dh,0ah
	db '	     senza argumenti o 0 come argumento dopo R significa risoluzione',0dh,0ah
	db '	     automatica, se il secondo argomento Š mancante significa',0dh,0ah
	db '	     che il suo valore Š lo stesso del primo argumento)',0dh,0ah
	db '  /L	   - modalit… per mancini (per default Š per non mancini)',0dh,0ah
	db '  /B	   - non installa se i servizi mouse sono gi… presenti',0dh,0ah
	db '  /W	   - impedisce il caricamento TSR negli UMB',0dh,0ah
	db '  /U	   - disinstalla il driver',0dh,0ah
	db '  /?	   - mostra questo aiuto',0dh,0ah,'$'
