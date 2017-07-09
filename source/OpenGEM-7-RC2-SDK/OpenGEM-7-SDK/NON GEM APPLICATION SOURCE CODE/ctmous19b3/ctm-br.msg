; This file is a part of CuteMouse source
; Here are messages in Brazilian Portugese
; Translated by Alain Mouette <alainm@pobox.com>
;
; Note: „” was used instead of Æä (ao-tilde)
;       this allows display both in codepage 437 and 850

E_option	db 5,0dh,0ah,'Erro: op‡„o inv lida',0dh,0ah
		db 'Digite "ctmouse /?" para ajuda',0dh,0ah,'$'
E_mousepresent	db 5,'Driver de mouse j  instalado',0dh,0ah,'$'
E_notfound	db 5,'Erro: dispositivo n„o encontrado',0dh,0ah,'$'

E_nocute	db 1,'Driver CuteMouse n„o foi instalado!',0dh,0ah,'$'
E_notunload	db 2,'N„o pode retirar driver da mem¢ria: alguma interup‡„o foi interceptada...',0dh,0ah,'$'
S_unloaded	db 0,'Driver retirado da mem¢ria com sucesso...',0dh,0ah,'$'

S_reset		db 'Parte resident reconfigurada para $'
S_installed	db 'Instalado em $'
S_atPS2		db 'porta PS/2$'
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
Syntax	db 0,'Op‡”es:',0dh,0ah
	db '  /P	   - for‡a modo mouse PS/2',0dh,0ah
	db '  /S[c[i]] - for‡a modo mouse serial em COM port c (c=1-4, default ‚ COM1)',0dh,0ah
	db '	     com IRQ = i (i=2-7, default ‚ IRQ4 para COM1/3 e IRQ3',0dh,0ah
	db '	     para COM2/4)',0dh,0ah
;;+	db '  /Aia	   - for‡a modo mouse serial em I/O = a (em hexadecimal)',0dh,0ah
;;+	db '	     com IRQ = i (i=2-7)',0dh,0ah
	db '  /Y	   - n„o instala modo Mouse Systems',0dh,0ah
	db '  /V	   - busca reversa: procura PS/2 depois do mouse serial',0dh,0ah
	db '  /3	   - for‡a modo 3-bot”es (s¢ para mouses Microsoft e PS/2)',0dh,0ah
	db '  /R[h[v]] - resolu‡Æo horizontal/vertical (h,v=0-9; op‡„o R faltando, sem',0dh,0ah
	db '	     argumentos ou argumento=0 indica resolu‡„o autom tica, segundo',0dh,0ah
	db '	     argumento faltando indica igual a primeiro argumento)',0dh,0ah
	db '  /L	   - modo canhoto (default ‚ modo dextro)',0dh,0ah
	db '  /B	   - n„o instala se outro driver j  instalado',0dh,0ah
	db '  /W	   - n„o permite carregar TSR na UMB',0dh,0ah
	db '  /U	   - desinstala driver',0dh,0ah
	db '  /?	   - mostra esta ajuda',0dh,0ah,'$'
