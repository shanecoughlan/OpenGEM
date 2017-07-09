

WORD  MUL_DIV (WORD m1, UWORD m2, WORD d1);
UWORD UMUL_DIV(UWORD m1, UWORD m2, UWORD d1);



typedef WORD (*VDIFUNC)(LPLPTR x);
typedef WORD (*AESFUNC)(LPLPTR x);
typedef WORD (*DRAWFUNC)(PARMBLK *PB);


typedef struct dj_user_blk
{
	DRAWFUNC ub_code;	/* Callback function */
	LONG	 ub_parm;	/* Parameter. If you change the      */
				/* parameter, update the 16-bit copy */
	LONG	 ub_16bit;	/* -> 16-bit copy of this userblk    */
} DJUSERBLK;

