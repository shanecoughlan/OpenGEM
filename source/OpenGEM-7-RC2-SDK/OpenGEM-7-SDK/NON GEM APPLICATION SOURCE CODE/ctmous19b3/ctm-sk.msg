; This file is a part of CuteMouse source
; Here are messages in Slovak
; Translated by Henrich Fukna <henrich.fukna@kiwwi.sk>

E_option	db 5,0dh,0ah,'Chyba: Nezn ma vo–ba',0dh,0ah
		db 'Nap¡çte /? do pr¡kazov‚ho riadku pre n poveÔ',0dh,0ah,'$'
E_mousepresent	db 5,'Slu§ba myçi u§ je pr¡tomn ',0dh,0ah,'$'
E_notfound	db 5,'Chyba: zariadenie nen jden‚',0dh,0ah,'$'

E_nocute	db 1,'CuteMouse ovl daŸ nie je nainçtalovanì!',0dh,0ah,'$'
E_notunload	db 2,'Ovl daŸ nemo§no odinçtalovaœ: niektor‚ preruçenia s£ zmenen‚...',0dh,0ah,'$'
S_unloaded	db 0,'Ovl daŸ £speçne odinçtalovanì...',0dh,0ah,'$'

S_reset		db 'Rezidentn  Ÿasœ nastaven  na $'
S_installed	db 'Inçtalovanì na $'
S_atPS2		db 'porte PS/2$'
S_atCOM		db 'COM'
com_port	db ?,' ('
S_atIO		db '0000h/IRQ'
IRQno		db ?
S_in		db ') pre $'
S_inMSYS	db 'Mouse Systems m¢d$'
S_inMS		db 'Microsoft m¢d$'
S_inLT		db 'Logitech m¢d$'
S_wheel		db ' (podpora kolieska)'
S_CRLF		db 0dh,0ah,'$'

Copyright	db 0dh,0ah,'CuteMouse v',CTMRELEASE,' [FreeDOS]',0dh,0ah,'$'
Syntax	db 0,'Mo§nosti:',0dh,0ah
	db '  /P	   - vyn£ti PS/2 m¢d',0dh,0ah
	db '  /S[c[i]] - vyn£ti s‚riovì m¢d na COM porte c (c=1-4, predvolenì je COM1)',0dh,0ah
	db '	     s IRQ i (i=2-7, predvolen‚ je IRQ4 pre COM1/3 a IRQ3',0dh,0ah
	db '	     pre COM2/4)',0dh,0ah
;;+	db '  /Aia	   - vyn£ti s‚riovì m¢d na I/O adrese a (hexadecim lne Ÿ¡slo)',0dh,0ah
;;+	db '	     s IRQ i (i=2-7)',0dh,0ah
	db '  /Y	   - zak §e Mouse Systems',0dh,0ah
	db '  /V	   - opaŸn‚ h–adanie: PS/2 po s‚riovìch myçiach',0dh,0ah
	db '  /3	   - vyn£ti 3-tlaŸ¡tkovì m¢d (pre Microsoft a PS/2 myçi)',0dh,0ah
	db '  /R[h[v]] - horizont lne/vertik lne rozl¡çenie (h,v=0-9; chìbaj£ca vo–ba R,',0dh,0ah
	db '	     §iadne argumenty alebo 0 ako argument po R znamen  automatick‚',0dh,0ah
	db '	     rozl¡çenie, chìbaj£ci druhì argument znamen  rovnak£ hodnotu',0dh,0ah
	db '	     ako pre prvì argument)',0dh,0ah
	db '  /L	   - m¢d pre –av£ ruku (prednastaven‚ pre prav£ ruku)',0dh,0ah
	db '  /B	   - preruç¡ vykon vanie ak slu§ba myçi je u§ pr¡tomn ',0dh,0ah
	db '  /W	   - zabr ni nahratiu TSR do UMB',0dh,0ah
	db '  /U	   - odinçtaluje ovl daŸ',0dh,0ah
	db '  /?	   - zobraz¡ n vod na pou§itie',0dh,0ah,'$'
