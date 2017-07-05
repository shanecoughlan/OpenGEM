; This file is a part of CuteMouse source
; Here are messages in English

E_option	db 5,0dh,0ah,'Error: Invalid option',0dh,0ah
		db 'Enter /? on command line for help',0dh,0ah,'$'
E_mousepresent	db 5,'Mouse services already present',0dh,0ah,'$'
E_notfound	db 5,'Error: device not found',0dh,0ah,'$'

E_nocute	db 1,'CuteMouse driver is not installed!',0dh,0ah,'$'
E_notunload	db 2,'Driver unload failed: some interrupts intercepted...',0dh,0ah,'$'
S_unloaded	db 0,'Driver successfully unloaded...',0dh,0ah,'$'

S_reset		db 'Resident part reset to $'
S_installed	db 'Installed at $'
S_atPS2		db 'PS/2 port$'
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
Syntax	db 0,'Options:',0dh,0ah
	db '  /P	   - force PS/2 mouse mode',0dh,0ah
	db '  /S[c[i]] - force serial mouse mode at COM port c (c=1-4, default is COM1)',0dh,0ah
	db '	     with IRQ line i (i=2-7, default is IRQ4 for COM1/3 and IRQ3',0dh,0ah
	db '	     for COM2/4)',0dh,0ah
;;+	db '  /Aia	   - force serial mouse mode at I/O address a (hexadecimal number)',0dh,0ah
;;+	db '	     with IRQ line i (i=2-7)',0dh,0ah
	db '  /Y	   - prevent Mouse Systems mode',0dh,0ah
	db '  /V	   - reverse search: find PS/2 after serial mouse',0dh,0ah
	db '  /3	   - force 3-button mode (for Microsoft and PS/2 mice only)',0dh,0ah
	db '  /R[h[v]] - horizontal/vertical resolution (h,v=0-9; missing option R, no',0dh,0ah
	db '	     arguments or 0 as argument after R mean auto resolution, missing',0dh,0ah
	db '	     second argument means same value as for first argument)',0dh,0ah
	db '  /L	   - left hand mode (default is right hand mode)',0dh,0ah
	db '  /B	   - cancel run if mouse services already present',0dh,0ah
	db '  /W	   - prevent loading TSR into UMB',0dh,0ah
	db '  /U	   - uninstall driver',0dh,0ah
	db '  /?	   - show this help',0dh,0ah,'$'
