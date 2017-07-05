/*
*       PPDPTR.H: True "far pointer" types for Pacific C
*
*       Copyright 1999, John Elliott
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*/

#ifdef __PACIFIC__
typedef FAR VOID   *LPVOID;
typedef FAR LPVOID *LPLPTR;
typedef FAR BYTE   *LPBYTE;
typedef FAR WORD   *LPWORD;
typedef FAR LONG   *LPLONG;
typedef FAR UWORD  *LPUWORD;
typedef FAR ULONG  *LPULONG;
#else
#error This requires Pacific C!
#endif

