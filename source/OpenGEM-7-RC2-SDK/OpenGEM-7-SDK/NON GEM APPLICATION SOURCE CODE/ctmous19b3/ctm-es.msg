; This file is a part of CuteMouse source
; Here are messages in Spanish
; Translated by Diego Rodr¡guez <dieymir@navegalia.com>

E_option	db 5,0dh,0ah,'Error: opci¢n desconocida',0dh,0ah
		db 'Teclee "CTMOUSE /?" para obtener ayuda',0dh,0ah,'$'
E_mousepresent	db 5,'Controlador de rat¢n ya instalado',0dh,0ah,'$'
E_notfound	db 5,'Error: rat¢n no encontrado',0dh,0ah,'$'

E_nocute	db 1,'­CuteMouse no ha sido instalado!',0dh,0ah,'$'
E_notunload	db 2,'Imposible desinstalar Cutemouse: alguna interrupci¢n interceptada...',0dh,0ah,'$'
S_unloaded	db 0,'Cutemouse ha sido desinstalado correctamente...',0dh,0ah,'$'

S_reset		db 'Controlador residente en $'
S_installed	db 'Instalado en $'
S_atPS2		db 'puerto PS/2$'
S_atCOM		db 'COM'
com_port	db ?,' ('
S_atIO		db '0000h/IRQ'
IRQno		db ?
S_in		db ') usando protocolo $'
S_inMSYS	db 'Mouse Systems$'
S_inMS		db 'Microsoft$'
S_inLT		db 'Logitech$'
S_wheel		db ' (detectada rueda)'
S_CRLF		db 0dh,0ah,'$'

Copyright	db 0dh,0ah,'CuteMouse v',CTMRELEASE,' [FreeDOS]',0dh,0ah,'$'
Syntax	db 0,'Opciones:',0dh,0ah
	db '  /P	   - fuerza rat¢n PS/2',0dh,0ah
	db '  /S[c[i]] - fuerza rat¢n serie en puerto COM c (c=1-4, por defecto COM1)',0dh,0ah
	db '	     con IRQ = i (i=2-7, por defecto IRQ4 para COM1/3 e IRQ3',0dh,0ah
	db '	     para COM2/4)',0dh,0ah
;;+	db '  /Aia	   - fuerza rat¢n serie en direcci¢n de E/S = a (en hexadecimal)',0dh,0ah
;;+	db '	     con IRQ = i (i=2-7)',0dh,0ah
	db '  /Y	   - no instala modo Mouse Systems',0dh,0ah
	db '  /V	   - invertir orden de b£squeda: busca antes el rat¢n en puerto serie',0dh,0ah
	db '  /3	   - fuerza modo 3-botones (s¢lo para rat¢n Microsoft y PS/2)',0dh,0ah
	db '  /R[h[v]] - resoluci¢n horizontal/vertical (h,v=0-9; especificar R sin',0dh,0ah
	db '	     argumentos o argumento=0 indica resoluci¢n autom tica, sin segundo',0dh,0ah
	db '	     argumento indica valor igual a primer argumento)',0dh,0ah
	db '  /L	   - rat¢n para zurdos (por defecto para diestros)',0dh,0ah
	db '  /B	   - no se instala si ya hay otro controlador instalado',0dh,0ah
	db '  /W	   - no instalar en bloque UMB',0dh,0ah
	db '  /U	   - desinstalar controlador',0dh,0ah
	db '  /?	   - mostrar esta ayuda',0dh,0ah,'$'
