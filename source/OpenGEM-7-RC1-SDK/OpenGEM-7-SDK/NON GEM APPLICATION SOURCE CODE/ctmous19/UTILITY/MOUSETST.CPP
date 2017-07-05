#include <dos.h>
#include <stdio.h>
#include <stdlib.h>

#pragma option -N-

/*===================================================================*/

#ifdef __STDC__

#  define far
#  define near
#  define cdecl

#  define SMALL_
#  define FAST_
#  define STD_
#  define NSMALL_
#  define NFAST_
#  define NSTD_
#  define FSMALL_
#  define FFAST_
#  define FSTD_

#else

#  define SMALL_	_pascal
#  define FAST_		__fastcall
#  define STD_		cdecl
#  define NSMALL_	near _pascal
#  define NFAST_	near __fastcall
#  define NSTD_		near cdecl
#  define FSMALL_	far _pascal
#  define FFAST_	far __fastcall
#  define FSTD_		far cdecl

#endif

typedef unsigned char	byte;
typedef unsigned short	word;
typedef const char	CStr[], *PCStr;

/*-------------------------------------------------------------------*/

#if defined(__LARGE__) || defined(__HUGE__) || defined(__COMPACT__)
#define FARDATA
#endif

enum CPU_FLAGS { Carry = 1, Parity = 4, Auxcarry = 0x10,
		 Zero = 0x40, Equal = Zero, Sign = 0x80, Trap = 0x100,
		 IEnable = 0x200, Direct = 0x400, Overflow = 0x800
};

#define isFLAG(mask) (_FLAGS & (mask))

/*-------------------------------------------------------------------*/

volatile const word far &NSTD_ _wscreen = *((const word far*)MK_FP(0, 0x44A));
volatile const byte far &NSTD_ _hscreen = *((const byte far*)MK_FP(0, 0x484));

#define scrwidth	(_wscreen)
#define scrheight	(_hscreen ? _hscreen + 1 : 25)

/*===================================================================*/

word AX, BX, CX, DX, SI, DI;
word cnt = 0;

void _loadds NSMALL_ handler_(word AX_){
	AX = AX_, BX = _BX, CX = _CX, DX = _DX, SI = _SI, DI = _DI; cnt++;
}

void far handler() { handler_(_AX); }

void NSMALL_ sethandler(word mask){
#ifdef FARDATA
	_ES = FP_SEG(handler),
#else
	_ES = _CS,
#endif
	_DX = FP_OFF(handler), _CX = mask, _AX = 0xC; asm int 33h;
}

/*-------------------------------------------------------------------*/
void NSMALL_ gotorc(byte row, byte col){
	_DH = row, _DL = col, _BH = 0, _AH = 2; asm int 10h;
}

void NSMALL_ fillscreen(){
	for(word row = 1; row < scrheight; row++){
		for(word col = 0, color = 2; col < scrwidth; col++){
			gotorc(row, col);
			_AL = (row + col) % 10u + byte('0'), _BH = 0,
			_BL = color, _CX = 1, _AH = 9; asm int 10h;
			color++, color &= 0x0F;
}	}	}

char butstatus[3*8] = "lmrLmrlmRLmRlMrLMrlMRLMR";

void NSMALL_ showstatus(word BX, word CX, word DX){
	gotorc(0, scrwidth - 14); printf("%-14.3s", &butstatus[(BX&7)*3]);
	gotorc(0, scrwidth - 10); printf("%3d,%-d", CX, DX);
}

void NFAST_ pause(CStr s){
	gotorc(0,0); printf(s);
	_AX = 3; asm int 33h; showstatus(_BX, _CX, _DX);
	for(word oldcnt = cnt;;){
		asm hlt;
		if(oldcnt != cnt) { showstatus(BX, CX, DX); oldcnt = cnt; }
		_AH = 1; asm int 16h; if(!isFLAG(Zero)) break;
	}
	_AH = 0; asm int 16h;
}

void NFAST_ font_pause(word fno, CStr s){
	/**/ fno = fno;
	_AX = fno, _AX += 0x1110; _BL = 0; asm int 10h; fillscreen();
	_AX = 0x21; asm int 33h; sethandler(0xFFFF);
	_AX = 1; asm int 33h; pause(s);
}

/*-------------------------------------------------------------------*/
int main(int argc, char *argv[]){
	if(argc < 2){
		printf("Syntax: mousetst <decimal video mode>\n"
			"Standard text modes: 0-3, 7\n"
			"Standard graphics modes: 4-6, 13-19\n"
			"Note: mode 7 only for monochrome systems\n");
		return 1;
	}
				sethandler(0xFFFF);
				pause("start");
	_AX = 1; asm int 33h;	pause("start/show");
	_AX = 0xd; _AX = (word)atol(argv[1]); asm int 10h;
	fillscreen();		pause("setmode   ");

	_CX = -40, _DX = 680, _AX = 7; asm int 33h;
	_CX = -40, _DX = 500, _AX = 8; asm int 33h;
	_AX = 1; asm int 33h;	pause("setmode/show");

/*	_CX = 21*8 + 5, _DX = 10*8 + 5, _SI = 41*8 - 4, _DI = 15*8 - 4,	*/
/*	_AX = 0x10; asm int 33h; pause("/region 173-85/324-116");	*/

	if((word)atol(argv[1]) < 4){
		font_pause(2, "setmode/show 8x8      ");
		font_pause(1, "setmode/show 8x14");
		font_pause(4, "setmode/show 8x16");
	}

	_AX = 0; asm int 33h;	sethandler(0xFFFF);
				pause("reset                 ");
	_AX = 1; asm int 33h;	pause("reset/show");
	_AX = 3; asm int 10h;	sethandler(0);
	return 0;
}
