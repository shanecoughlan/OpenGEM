; This file is a part of CuteMouse source
; Here are messages in Polish
; Translated by Jerzy 'HARY' Klejnowski <ksionrze@poczta.wp.pl>

E_error		db nl,'Bˆ¥d: niepoprawn',eos
E_option	db 'a opcja',eos
E_argument	db 'y argument',eos
E_help		db 5,nl,'Pomoc uzyskasz podaj¥c /? w wierszu poleceä',eos

E_mousepresent	db 5,'Usˆugi myszy s¥ ju¾ dost©pne',eos
E_notfound	db 5,'Bˆ¥d: nie znaleziono urz¥dzenia',eos
;;+E_noIRQ	db 5,'Bˆ¥d: IRQ line not detected',eos

E_nocute	db 1,'Sterownik CuteMouse nie zostaˆ zainstalowany!',eos
E_notunload	db 2,'Nie mog© zdeinstalowa† sterownika: zmienione wektory przerwaä...',eos
S_unloaded	db 0,'Sterownik zdeinstalowany...',eos

S_reset		db 'Cz©˜† rezydentna ustawiona na ',eos
S_installed	db 'Zainstalowany na: ',eos
S_atPS2		db 'PS/2',eos
S_atCOM		db 'COM'
com_port	db ?,' ('
S_atIO		db '0000h/IRQ'
IRQno		db ?
S_in		db '), protok¢ˆ ',eos
S_inMSYS	db 'Mouse Systems',eos
S_inMS		db 'Microsoft',eos
S_inLT		db 'Logitech',eos
S_wheel		db nl,9,9,'(obsˆuga rolki wˆ¥czona)'
S_CRLF		db nl,eos

Copyright	db nl,'CuteMouse v',CTMRELEASE,' [FreeDOS]',nl,eos
Syntax		label
    db 0,'Opcje:',nl
    db '  /P	   - wymusza tryb PS/2',nl
    db '  /S[c[i]] - wymusza u¾ycie portu szeregowego COM nr c (1-4) na przerwaniu',nl
    db '	     i (1-7)',nl
;;+ db '  /S$a[,i] - wymusza u¾ycie portu szeregowego pod adresem a (szesnastkowo)',nl
;;+ db '	     na przerwaniu i (1-7)',nl
    db '  /Y	   - nie uruchamiaj w trybie Mouse Systems',nl
    db '  /V	   - odwr¢† kolejno˜†: szukaj najpierw na COM, potem na PS/2',nl
    db '  /3	   - wymu˜ tryb 3-przyciskowy (tylko dla myszy Microsoft i PS/2)',nl
    db '  /R[h[v]] - rozdzielczo˜† pozioma/pionowa (h,v=0-9; brak opcji R, brak',nl
    db '	     argument¢w lub podanie 0 oznacza auto resolution; brak drugiego',nl
    db '	     argumentu oznacza tak¥ sam¥ warto˜† jak dla pierwszego argumentu)',nl
    db '  /L	   - mysz dla lewor©cznych (domy˜lnie dla prawor©cznych)',nl
    db '  /B	   - nie uruchamiaj, je˜li usˆugi myszy s¥ ju¾ dost©pne',nl
    db '  /N	   - instaluj, nawet je˜li sterownik CuteMouse jest ju¾ zainstalowany',nl
    db '	     (u¾yteczne w .BAT, kt¢re deinstaluj¥ CuteMouse na koäcu)',nl
    db '  /W	   - nie instaluj cz©˜ci rezydentnej w UMB',nl
    db '  /U	   - zdeinstaluj sterownik',nl
    db '  /?	   - poka¾ ten tekst',eos
