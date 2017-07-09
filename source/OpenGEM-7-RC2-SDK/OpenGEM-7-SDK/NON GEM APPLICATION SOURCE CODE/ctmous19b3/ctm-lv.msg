; This file is a part of CuteMouse source
; Here are messages in Latvian (for codepage 775)
; Translated by Kristaps Kaupe, Latvia <neons@digital.lv>

E_option	db 5,0dh,0ah,'Kë×da: Nepareiza opcija',0dh,0ah
		db 'Ievadiet /? komandrindƒ lai ieg×tu palŒdzŒbu',0dh,0ah,'$'
E_mousepresent	db 5,'Cits peles draiveris jau ir atmiìƒ',0dh,0ah,'$'
E_notfound	db 5,'Kë×da: iekƒrta nav atrasta',0dh,0ah,'$'

E_nocute	db 1,'CuteMouse draiveris netika ielƒd‰ts!',0dh,0ah,'$'
E_notunload	db 2,'Kë×da izlƒd‰jot draiveri: daØi pƒrtraukumi aizìemti...',0dh,0ah,'$'
S_unloaded	db 0,'Draiveris veiksmŒgi izlƒd‰ts...',0dh,0ah,'$'

S_reset		db 'Rezidentƒ daëa pƒrlƒd‰ta uz $'
S_installed	db 'Ielƒd‰ts uz $'
S_atPS2		db 'PS/2 porta$'
S_atCOM		db 'COM'
com_port	db ?,' ('
S_atIO		db '0000h/IRQ'
IRQno		db ?
S_in		db ') $'
S_inMSYS	db 'Mouse Systems reØŒmƒ$'
S_inMS		db 'Microsoft reØŒmƒ$'
S_inLT		db 'Logitech reØŒmƒ$'
S_wheel		db ' (pele ar ritenŒti)'
S_CRLF		db 0dh,0ah,'$'

Copyright	db 0dh,0ah,'CuteMouse v',CTMRELEASE,' [FreeDOS]',0dh,0ah,'$'
Syntax	db 0,'Opcijas:',0dh,0ah
	db '  /P	   - PS/2 peles reØŒms',0dh,0ah
	db '  /S[c[i]] - seriƒlƒs peles reØŒms uz COM porta c (c=1-4, noklus‰tais ir COM1)',0dh,0ah
	db '	     ar IRQ lŒniju i (i=2-7, noklus‰tais ir IRQ4 priekÕ COM1/3 un IRQ3',0dh,0ah
	db '	     priekÕ COM2/4)',0dh,0ah
;;+	db '  /Aia	   - seriƒlƒs peles reØŒms I/O adres‰ a (heksadecimƒlais numurs)',0dh,0ah
;;+	db '	     ar IRQ lŒniju i (i=2-7)',0dh,0ah
	db '  /Y	   - Mouse Systems reØŒms',0dh,0ah
	db '  /V	   - apgrieztƒ mekl‰Õana: mekl‰t PS/2 p‰c seriƒlƒs peles',0dh,0ah
	db '  /3	   - 3 pogu reØŒms (tikai Microsoft un PS/2 pel‰m)',0dh,0ah
	db '  /R[h[v]] - horizontƒlƒ/vertikƒlƒ izÕéirtsp‰ja (h,v=0-9; ja nav opcijas R, nav',0dh,0ah
	db '	     argumentu vai arguments p‰c R ir 0, tiek lietota automƒtiska',0dh,0ah
	db '	     izÕéirtsp‰ja, ja tr×kst otrƒ argumentƒ tam tiek pieÕéirta tƒda',0dh,0ah
	db '	     pati v‰rtŒba kƒ pirmajam)',0dh,0ah
	db '  /L	   - kreisƒs rokas reØŒms (noklus‰tais ir labƒs rokas reØŒms)',0dh,0ah
	db '  /B	   - pƒrtraukt darbu, ja cits peles draiveris jau ir atmiìƒ',0dh,0ah
	db '  /W	   - nelƒd‰t draivera rezidento daëu UMB',0dh,0ah
	db '  /U	   - izlƒd‰t draiveri',0dh,0ah
	db '  /?	   - parƒdŒt Õo palŒdzŒbu',0dh,0ah,'$'
