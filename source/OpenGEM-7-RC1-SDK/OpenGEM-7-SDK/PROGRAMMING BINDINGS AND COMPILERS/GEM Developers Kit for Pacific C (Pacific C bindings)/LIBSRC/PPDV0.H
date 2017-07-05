extern WORD contrl[], intin[], ptsin[], intout[], ptsout[];

extern GSXPAR pblock;

#define i_ptsin(ptr) pblock.ptsin = ptr
#define i_intin(ptr) pblock.intin = ptr
#define i_intout(ptr) pblock.intout = ptr
#define i_ptsout(ptr) pblock.ptsout = ptr

extern LPVOID *lptr1;
extern LPVOID *lptr2;

#define i_ptr(ptr)   *lptr1 = ptr
#define i_lptr1(ptr) *lptr1 = ptr
#define i_ptr2(ptr)  *lptr2 = ptr
#define m_lptr2(ptr) *ptr   = *lptr2

