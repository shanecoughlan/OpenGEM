; This file is a part of CuteMouse source
; Here are messages in Latvian (for codepage 775)
; Translated by Kristaps Kaupe, Latvia <neons@digital.lv>

E_option	db 5,0dh,0ah,'K��da: Nepareiza opcija',0dh,0ah
		db 'Ievadiet /? komandrind� lai ieg�tu pal�dz�bu',0dh,0ah,'$'
E_mousepresent	db 5,'Cits peles draiveris jau ir atmi�',0dh,0ah,'$'
E_notfound	db 5,'K��da: iek�rta nav atrasta',0dh,0ah,'$'

E_nocute	db 1,'CuteMouse draiveris netika iel�d�ts!',0dh,0ah,'$'
E_notunload	db 2,'K��da izl�d�jot draiveri: da�i p�rtraukumi aiz�emti...',0dh,0ah,'$'
S_unloaded	db 0,'Draiveris veiksm�gi izl�d�ts...',0dh,0ah,'$'

S_reset		db 'Rezident� da�a p�rl�d�ta uz $'
S_installed	db 'Iel�d�ts uz $'
S_atPS2		db 'PS/2 porta$'
S_atCOM		db 'COM'
com_port	db ?,' ('
S_atIO		db '0000h/IRQ'
IRQno		db ?
S_in		db ') $'
S_inMSYS	db 'Mouse Systems re،m�$'
S_inMS		db 'Microsoft re،m�$'
S_inLT		db 'Logitech re،m�$'
S_wheel		db ' (pele ar riten�ti)'
S_CRLF		db 0dh,0ah,'$'

Copyright	db 0dh,0ah,'CuteMouse v',CTMRELEASE,' [FreeDOS]',0dh,0ah,'$'
Syntax	db 0,'Opcijas:',0dh,0ah
	db '  /P	   - PS/2 peles re،ms',0dh,0ah
	db '  /S[c[i]] - seri�l�s peles re،ms uz COM porta c (c=1-4, noklus�tais ir COM1)',0dh,0ah
	db '	     ar IRQ l�niju i (i=2-7, noklus�tais ir IRQ4 priek� COM1/3 un IRQ3',0dh,0ah
	db '	     priek� COM2/4)',0dh,0ah
;;+	db '  /Aia	   - seri�l�s peles re،ms I/O adres� a (heksadecim�lais numurs)',0dh,0ah
;;+	db '	     ar IRQ l�niju i (i=2-7)',0dh,0ah
	db '  /Y	   - Mouse Systems re،ms',0dh,0ah
	db '  /V	   - apgriezt� mekl��ana: mekl�t PS/2 p�c seri�l�s peles',0dh,0ah
	db '  /3	   - 3 pogu re،ms (tikai Microsoft un PS/2 pel�m)',0dh,0ah
	db '  /R[h[v]] - horizont�l�/vertik�l� iz��irtsp�ja (h,v=0-9; ja nav opcijas R, nav',0dh,0ah
	db '	     argumentu vai arguments p�c R ir 0, tiek lietota autom�tiska',0dh,0ah
	db '	     iz��irtsp�ja, ja tr�kst otr� argument� tam tiek pie��irta t�da',0dh,0ah
	db '	     pati v�rt�ba k� pirmajam)',0dh,0ah
	db '  /L	   - kreis�s rokas re،ms (noklus�tais ir lab�s rokas re،ms)',0dh,0ah
	db '  /B	   - p�rtraukt darbu, ja cits peles draiveris jau ir atmi�',0dh,0ah
	db '  /W	   - nel�d�t draivera rezidento da�u UMB',0dh,0ah
	db '  /U	   - izl�d�t draiveri',0dh,0ah
	db '  /?	   - par�d�t �o pal�dz�bu',0dh,0ah,'$'
