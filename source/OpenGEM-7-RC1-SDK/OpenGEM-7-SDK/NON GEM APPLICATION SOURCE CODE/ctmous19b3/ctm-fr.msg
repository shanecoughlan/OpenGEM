; This file is a part of CuteMouse source
; Here are messages in French
; Translated by Frederic Pasteleurs <masterfrd@freegates.be>

E_option	db 5,0dh,0ah,'Erreur: Option invalide.',0dh,0ah
		db "Entrez /? sur la ligne de commande pour l'aide.",0dh,0ah,'$'
E_mousepresent	db 5,'Un driver souris est d‚j… pr‚sent.',0dh,0ah,'$'
E_notfound	db 5,'Erreur: p‚riph‚rique non trouv‚.',0dh,0ah,'$'

E_nocute	db 1,"Le driver CuteMouse n'est pas install‚ !",0dh,0ah,'$'
E_notunload	db 2,'Echec du d‚chargement du driver: des interruptions sont intercept‚es...',0dh,0ah,'$'
S_unloaded	db 0,'Le driver CuteMouse est enlev‚.',0dh,0ah,'$'

S_reset		db 'Portion r‚sidante initialis‚e … $'
S_installed	db 'Install‚ sur $'
S_atPS2		db 'port PS/2$'
S_atCOM		db 'COM'
com_port	db ?,' ('
S_atIO		db '0000h/IRQ'
IRQno		db ?
S_in		db ') en mode $'
S_inMSYS	db '"Mouse Systems"$'
S_inMS		db 'Microsoft$'
S_inLT		db 'Logitech$'
S_wheel		db ' (roulette pr‚sente)'
S_CRLF		db 0dh,0ah,'$'

Copyright	db 0dh,0ah,'CuteMouse v',CTMRELEASE,' [FreeDOS]',0dh,0ah,'$'
Syntax	db 0,'Options:',0dh,0ah
	db '  /P	   - force le mode souris PS/2.',0dh,0ah
	db '  /S[c[i]] - force le mode souris s‚rie sur le port COM c',0dh,0ah
	db "	     (c=1-4, COM1 par d‚faut) avec l'IRQ num‚ro i",0dh,0ah
	db '	     (i=2-7, IRQ4 par d‚faut pour COM1/3 et IRQ3 pour COM2/4).',0dh,0ah
;;+	db "  /Aia	   - force le mode souris s‚rie sur le port d'E/S a (nombre hexa)",0dh,0ah
;;+	db '	     avec la ligne IRQ i (i=2-7).',0dh,0ah
	db '  /Y	   - Empˆche le mode "Mouse Systems".',0dh,0ah
	db '  /V	   - Recherche invers‚e: trouver la souris PS/2 aprŠs la souris s‚rie.',0dh,0ah
	db '  /3	   - force le mode 3-boutons (Souris Microsoft et PS/2 seulement).',0dh,0ah
	db '  /R[h[v]] - R‚solution horizontal/vertical',0dh,0ah
	db "	     (h,v=0-9; Si l'option R manque, si il n'y a pas d'arguments",0dh,0ah
	db '	     ou si il y a z‚ro aprŠs R, la r‚solution automatique sera',0dh,0ah
	db '	     utilis‚e; si le deuxiŠme argument est manquant, la mˆme valeur',0dh,0ah
	db "	     que l'argument pr‚c‚dent sera utilis‚).",0dh,0ah
	db '  /L	   - Mode gaucher (Le driver est en mode droitier par d‚faut).',0dh,0ah
	db '  /B	   - Empˆche le d‚marrage du driver si les services sont d‚j… pr‚sents.',0dh,0ah
	db '  /W	   - Empˆche le chargement du driver dans les UMBs',0dh,0ah
	db '  /U	   - EnlŠve le driver CuteMouse.',0dh,0ah
	db '  /?	   - Montre cette aide.',0dh,0ah,'$'
