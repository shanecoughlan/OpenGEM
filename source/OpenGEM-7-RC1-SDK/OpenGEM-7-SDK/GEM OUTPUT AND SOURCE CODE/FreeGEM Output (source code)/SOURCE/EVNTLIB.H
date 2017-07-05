/*	EVNTLIB.H	03/15/84 - 05/16/84	Lee Lorenzen		*/

						/* mu_flags		*/
#define MU_KEYBD 0x0001	
#define MU_BUTTON 0x0002
#define MU_M1 0x0004
#define MU_M2 0x0008
#define MU_MESAG 0x0010
#define MU_TIMER 0x0020

typedef struct moblk
{
	WORD		m_out;
	WORD		m_x;
	WORD		m_y;
	WORD		m_w;
	WORD		m_h;
} MOBLK ;