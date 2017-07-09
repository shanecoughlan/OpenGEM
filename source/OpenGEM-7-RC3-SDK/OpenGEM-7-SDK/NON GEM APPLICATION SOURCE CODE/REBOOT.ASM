 
; Code by BiTByte
assume cs:code,ds:code;
assume es:nothing,ss:nothing;


code segment use16



org 100h
start:
          mov ax,40h
          mov ds,ax
          mov word ptr ds:[72],1234h
          db 0eah,0f0h,0ffh,00h,0f0h
code ends
      end start
