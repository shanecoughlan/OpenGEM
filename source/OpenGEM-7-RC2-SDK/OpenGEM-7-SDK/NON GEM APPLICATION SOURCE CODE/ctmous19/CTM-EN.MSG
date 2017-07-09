; This file is a part of CuteMouse source
; Here are messages in English

E_error		db nl,'Error: Invalid option'
E_option	db eos
E_argument	db ' argument',eos
E_help		db 5,nl,'Enter /? on command line for help',eos

E_mousepresent	db 5,'Mouse services already present',eos
E_notfound	db 5,'Error: device not found',eos
;;+E_noIRQ	db 5,'Error: IRQ line not detected',eos

E_nocute	db 1,'CuteMouse driver is not installed!',eos
E_notunload	db 2,'Driver unload failed: some interrupts intercepted...',eos
S_unloaded	db 0,'Driver successfully unloaded...',eos

S_reset		db 'Resident part reset to ',eos
S_installed	db 'Installed at ',eos
S_atPS2		db 'PS/2 port',eos
S_atCOM		db 'COM'
com_port	db ?,' ('
S_atIO		db '0000h/IRQ'
IRQno		db ?
S_in		db ') in ',eos
S_inMSYS	db 'Mouse Systems mode',eos
S_inMS		db 'Microsoft mode',eos
S_inLT		db 'Logitech mode',eos
S_wheel		db ' (wheel present)'
S_CRLF		db nl,eos

Copyright	db nl,'CuteMouse v',CTMRELEASE,' [FreeDOS]',nl,eos
Syntax		label
    db 0,'Options:',nl
    db '  /P	   - force PS/2 mouse mode',nl
    db '  /S[c[i]] - force serial mouse mode at COM port c (1-4) with IRQ line i (1-7)',nl
;;+ db '  /S$a[,i] - force serial mouse mode at I/O address a (hexadecimal number)',nl
;;+ db '	     with IRQ line i (1-7)',nl
    db '  /Y	   - prevent Mouse Systems mode',nl
    db '  /V	   - reverse search: find PS/2 after serial mouse',nl
    db '  /3	   - force 3-button mode (for Microsoft and PS/2 mice only)',nl
    db '  /R[h[v]] - horizontal/vertical resolution (h,v=0-9; missing option R, no',nl
    db '	     arguments or 0 as argument after R mean auto resolution, missing',nl
    db '	     second argument means same value as for first argument)',nl
    db '  /L	   - left hand mode (default is right hand mode)',nl
    db '  /B	   - cancel run if mouse services are already present',nl
    db '  /N	   - load CuteMouse as new TSR, even if CuteMouse is already loaded',nl
    db '	     (useful for batch files which unload CuteMouse at end)',nl
    db '  /W	   - prevent loading TSR into UMB',nl
    db '  /U	   - uninstall driver',nl
    db '  /?	   - show this help',eos
