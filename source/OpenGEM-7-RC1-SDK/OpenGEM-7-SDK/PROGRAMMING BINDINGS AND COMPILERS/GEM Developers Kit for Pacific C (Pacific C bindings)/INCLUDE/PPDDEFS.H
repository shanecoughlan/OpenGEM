

typedef struct gemblkstr
{
	LPUWORD		gb_pcontrol;
	LPUWORD		gb_pglobal;
	LPUWORD		gb_pintin;
	LPUWORD		gb_pintout;
	LPLPTR		gb_padrin;
	LPLPTR		gb_padrout;		
} GEMBLK;

typedef GEMBLK FAR * LPGEMBLK;


typedef struct gsx_parameters {
    LPWORD contrl;
    LPWORD intin;
    LPWORD ptsin;
    LPWORD intout;
    LPWORD ptsout;
} GSXPAR;

typedef GSXPAR FAR * LPGSXPAR;


WORD  MUL_DIV (WORD m1, UWORD m2, WORD d1);
UWORD UMUL_DIV(UWORD m1, UWORD m2, UWORD d1);


