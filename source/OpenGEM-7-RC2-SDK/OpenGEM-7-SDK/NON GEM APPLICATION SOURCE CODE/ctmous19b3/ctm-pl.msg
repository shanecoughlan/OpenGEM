; This file is a part of CuteMouse source
; Here are messages in Polish
; Translated by Jerzy 'HARY' Klejnowski <ksionrze@poczta.wp.pl>

E_option	db 5,0dh,0ah,'Bˆ¥d: Niepoprawna opcja',0dh,0ah
		db 'Pomoc uzyskasz podaj¥c /? w wierszu poleceä',0dh,0ah,'$'
E_mousepresent	db 5,'Usˆugi myszy s¥ ju¾ dost©pne',0dh,0ah,'$'
E_notfound	db 5,'Bˆ¥d: nie znaleziono urz¥dzenia',0dh,0ah,'$'

E_nocute	db 1,'Sterownik CuteMouse nie zostaˆ zainstalowany!',0dh,0ah,'$'
E_notunload	db 2,'Nie mog© zdeinstalowa† sterownika: zmienione wektory przerwaä...',0dh,0ah,'$'
S_unloaded	db 0,'Sterownik zdeinstalowany...',0dh,0ah,'$'

S_reset		db 'Cz©˜† rezydentna ustawiona na $'
S_installed	db 'Zainstalowany na: $'
S_atPS2		db 'PS/2$'
S_atCOM		db 'COM'
com_port	db ?,' ('
S_atIO		db '0000h/IRQ'
IRQno		db ?
S_in		db '), protok¢ˆ $'
S_inMSYS	db 'Mouse Systems$'
S_inMS		db 'Microsoft$'
S_inLT		db 'Logitech$'
S_wheel		db 0dh,0ah,9,9,'(obsˆuga rolki wˆ¥czona)'
S_CRLF		db 0dh,0ah,'$'

Copyright	db 0dh,0ah,'CuteMouse v',CTMRELEASE,' [FreeDOS]',0dh,0ah,'$'
Syntax	db 0,'Opcje:',0dh,0ah
	db '  /P	   - wymusza tryb PS/2',0dh,0ah
	db '  /S[c[i]] - wymusza u¾ycie portu szeregowego COM nr c (c=1-4, domy˜lnie COM1)',0dh,0ah
	db '	     na przerwaniu i (i=2-7, domy˜lnie IRQ4 dla COM1/3 i IRQ3',0dh,0ah
	db '	     dla COM2/4)',0dh,0ah
;;+	db '  /Aia	   - wymusza u¾ycie portu szeregowego pod adresem a (szesnastkowo)',0dh,0ah
;;+	db '	     na przerwaniu i (i=2-7)',0dh,0ah
	db '  /Y	   - nie uruchamiaj w trybie Mouse Systems',0dh,0ah
	db '  /V	   - odwr¢† kolejno˜†: szukaj najpierw na COM, potem na PS/2',0dh,0ah
	db '  /3	   - wymu˜ tryb 3-przyciskowy (tylko dla myszy Microsoft i PS/2)',0dh,0ah
	db '  /R[h[v]] - rozdzielczo˜† pozioma/pionowa (h,v=0-9; brak opcji R, brak',0dh,0ah
	db '	     argument¢w lub podanie 0 oznacza auto resolution; brak drugiego',0dh,0ah
	db '	     argumentu oznacza tak¥ sam¥ warto˜† jak dla pierwszego argumentu)',0dh,0ah
	db '  /L	   - mysz dla lewor©cznych (domy˜lnie dla prawor©cznych)',0dh,0ah
	db '  /B	   - nie uruchamiaj, je˜li usˆugi myszy s¥ ju¾ dost©pne',0dh,0ah
	db '  /W	   - nie instaluj cz©˜ci rezydentnej w UMB',0dh,0ah
	db '  /U	   - zdeinstaluj sterownik',0dh,0ah
	db '  /?	   - poka¾ ten tekst',0dh,0ah,'$'
