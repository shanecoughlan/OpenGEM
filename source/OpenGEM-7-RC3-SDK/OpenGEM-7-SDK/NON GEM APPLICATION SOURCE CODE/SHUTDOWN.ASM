;*****************************;
;                             ;
; Code by BiTByte             ;
; [Elmar Hanlhofer]           ;
;                             ;
;-----------------------------;
;                             ;
; SHUTDOWN PC IN DOS          ;
;                             ;
; - Sourcecode is FREE -      ;
;                             ;
;-----------------------------;
;                             ;
; assembling:                 ;
;                             ;
;   tasm shutdown             ;
;   tlink /t shutdown         ;
;                             ;
;-----------------------------;
;                             ;
; http://plop.at              ;
;                             ;
;*****************************;

assume cs:code,ds:code
assume es:nothing,ss:nothing
code segment
.286
org 100h
start:
        push cs
        pop ds




        call ProcessCommandline


        cmp parameter,WRONGPARAMETER
        jne @main1
        mov ah,9
        lea dx,WrongTxt
        int 21h
        jmp @exit
@main1:
        cmp parameter,NOOUTPUT
        je @main2
        mov ah,9
        lea dx,TitleTxt
        int 21h
@main2:
        cmp parameter,0
        je @main30
        cmp parameter,HELP
        jne @main3
@main30:
        mov ah,9
        lea dx,HelpTxt
        int 21h
        int 20h
@main3:
        mov ah,2ch
        int 21h
        call Print
@again:
        mov countw,dh

        cmp ignore,1
        je @again2
        mov ah,1
        int 16h
        jne @exit
@again2:
        mov ah,2ch
        int 21h        
        cmp countw,dh
        je @again

        call Print
        cmp seconds,0
        je @chknxt1
        dec seconds
        jmp @again
@chknxt1:
        cmp minutes,0
        je @nl
        mov seconds,59
        dec minutes
        jmp @again
@nl:
        call Poweroff
@exit:
        xor ax,ax
        int 16h
        mov ah,9
        lea dx,aborttxt
        int 21h
        int 20h


Print:
        cmp parameter,NOOUTPUT
        jne @prn
        ret
@prn:
        pusha       
          mov ax,seconds
          lea di,outpsec
          mov byte ptr ds:[di-1]," "
          call integer
          mov ax,minutes
          lea di,outpmin
          mov byte ptr ds:[di-1]," "
          call integer

          lea dx,outputtxt
          mov ah,9
          int 21h
         popa
ret
Integer:
        pusha
        mov bx,10       ; set base (10 values)
        mov byte ptr ds:[di],"0"  ; write "0" 
@decagain:
        or ax,ax        ; check if ax=0
	je @decexit
        xor dx,dx       ; clear dx for div
        div bx
	add dl,"0"      ; in dx=rest & add value "0"
        mov ds:[di],dl  ; write it
        dec di          ; sub pointer 1
	jmp @decagain
@decexit:
        popa
ret

ProcessCommandline:
        pusha
          mov si,81h
@proces1:
          call RemoveSpaces
          cmp byte ptr ds:[si],0dh
          je @procescmdexit
          cmp word ptr ds:[si],"m-"
          jne @proces2
          add si,2
          call RemoveSpaces
          call convert
          mov minutes,ax
          jmp @proces1
@proces2:
          cmp word ptr ds:[si],"s-"
          jne @proces3
          add si,2
          call RemoveSpaces
          call convert
          mov seconds,ax
          jmp @proces1
@proces3:
          cmp word ptr ds:[si],"h-"
          jne @proces4
          add si,2
          mov parameter,HELP
          call RemoveSpaces
          cmp byte ptr ds:[si],0dh
          je @procescmdexit
          mov parameter,TOOMANYPARAMETERS
          jmp @procescmdexit

@proces4:
          cmp word ptr ds:[si],"n-"
          jne @proces5
          add si,2
          call RemoveSpaces
          mov parameter,NOOUTPUT
          jmp @proces1
@proces5:
          cmp word ptr ds:[si],"i-"
          jne @proces6
          add si,2
          call RemoveSpaces
          mov ignore,1
          jmp @proces1
@proces6:
          mov parameter,WRONGPARAMETER
@procescmdexit:
        popa
ret

Convert:
          mov parameter,OK
          mov di,si
          push si
          call ToStringEnd
          sub si,di
          mov cx,si
          pop si
          push cx
          call rinteger
          pop cx
          add si,cx
ret

RemoveSpaces:
          cmp byte ptr ds:[si],20h
          jne @removeexit
          inc si
          jmp removespaces
@removeexit:
ret

ToStringEnd:
          cmp byte ptr ds:[si],20h
          je @tostrexit
          cmp byte ptr ds:[si],0dh
          je @tostrexit
          inc si
          jmp tostringend
@tostrexit:
ret

rinteger:
        push bx dx si
        xor ax,ax
        xor dx,dx
        mov bx,10      ; set base 10
@rdecagain:
        mul bx         ; x10
        mov dx,ax     ; save old eax
	lodsb           ; load next value
	cmp al,"0"      ; value valid?
	jb @rdecexit
	cmp al,"9"
	ja @rdecexit
        and ax,0ffh
	sub al,"0"      ; yes -> sub ascii value
        add dx,ax     ; add new to old
        mov ax,dx     ; save new value
	loop @rdecagain ; loop during the string
@rdecexit:
        pop si dx bx
ret

PowerOff:
        mov ax,5300h
        mov bx,0
        int 15h
        push ax

	mov ax,5308h
	mov bx,1
	mov cx,1
	int 15h
	mov ax,5308h
	mov bx,0ffffh
	mov cx,1
	int 15h

	mov ax,5301h
	mov bx,0
	int 15h
	mov ax,530Eh
	mov bx,0
        pop cx

	int 15h
	mov ax,530Dh
	mov bx,1
	mov cx,1
	int 15h
	mov ax,530Fh
	mov bx,1
	mov cx,1
	int 15h
	mov ax,5307h
	mov bx,1
	mov cx,3
	int 15h
        int 20h


OK                      = 1
NOOUTPUT                = 2
HELP                    = 3
TOOMANYPARAMETERS       = 5
WRONGPARAMETER          = 6

TitleTxt        db "PLOP Shutdown program   v1.1 Coded by Elmar Hanlhofer   "
                db "http://plop.at",0dh,0ah,0ah,"$"
WrongTxt        db "Error: wrong parameter use -h for help",0dh,0ah,"$"

Param1Txt       db "-m"
Param2Txt       db "-s"
Param3Txt       db "-h"
Param4Txt       db "-n"

HelpTxt         db "shutdown [-s value] [-m value] [-n] [-i] [-h]",0dh,0ah,0ah
                db " turns the pc off"
                db 0dh,0ah,0ah
                db " -s   seconds",0dh,0ah
                db " -m   minutes",0dh,0ah
                db " -n   no output",0dh,0ah
                db " -i   ignore key press to abort",0dh,0ah
                db " -h   help",0dh,0ah,"$"

OutputTxt       db 0dh,"Shutdown in    Minutes:  "
outpmin         db "   Seconds:  "
outpsec         db " $"

ignore db 0
aborttxt db 0dh,"Abort                                                ",0dh,0ah,"$"

parameter       db 0
minutes         dw 0
seconds         dw 0
count           db ?
countw          db ?


code ends
        end start


