/*	TREEADDR.H	04/11/84 - 09/11/84	Gregg Morris		*/
/*              5 Jan 1998 John Elliott: Type casts added so that */
/*                         PACIFIC C pointer arithmetic works */ 


/* Portability macros. These were written with 'tree' being long, so 
 * for Pacific C, typecasts were needed to force sizeof(*tree)==1.
 *
 */ 

#define OB_NEXT(x)   ((LPWORD) ((LPBYTE)tree + (x) * sizeof(OBJECT) +  0))
#define OB_HEAD(x)   ((LPWORD) ((LPBYTE)tree + (x) * sizeof(OBJECT) +  2))
#define OB_TAIL(x)   ((LPWORD) ((LPBYTE)tree + (x) * sizeof(OBJECT) +  4))
#define OB_TYPE(x)   ((LPUWORD)((LPBYTE)tree + (x) * sizeof(OBJECT) +  6))
#define OB_FLAGS(x)  ((LPUWORD)((LPBYTE)tree + (x) * sizeof(OBJECT) +  8))
#define OB_STATE(x)  ((LPUWORD)((LPBYTE)tree + (x) * sizeof(OBJECT) + 10))
#define OB_SPEC(x)   ((LPLPTR) ((LPBYTE)tree + (x) * sizeof(OBJECT) + 12))
#define OB_X(x)      ((LPUWORD)((LPBYTE)tree + (x) * sizeof(OBJECT) + 16))
#define OB_Y(x)      ((LPUWORD)((LPBYTE)tree + (x) * sizeof(OBJECT) + 18))
#define OB_WIDTH(x)  ((LPUWORD)((LPBYTE)tree + (x) * sizeof(OBJECT) + 20))
#define OB_HEIGHT(x) ((LPUWORD)((LPBYTE)tree + (x) * sizeof(OBJECT) + 22))



