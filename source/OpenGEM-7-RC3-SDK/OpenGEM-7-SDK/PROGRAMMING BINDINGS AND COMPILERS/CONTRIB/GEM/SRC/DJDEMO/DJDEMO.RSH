#define T0OBJ 0
#define T1OBJ 26
#define T2OBJ 39
#define T3OBJ 45
#define FREEBB 7
#define FREEIMG 7
#define FREESTR 48

BYTE *rs_strings[] = {
" Desk ",
" _File ",
" _Options ",
"  About GEM Demo... ",
"--------------------",
"1",
"2",
"3",
"4",
"5",
"6",
"  _Load...    ",
"  _Save       ",
"  Save _As... ",
"  _Abandon    ",
"  _Quit       ",
"  _Pen / Eraser Selection... ",
"-----------------------------",
"  _Erase Picture             ",
" _OK",
"GEM Demo ",
"GEM Sample Application",
"Authors",
"-------",
"Tom Rolander",
"Version 2.1   April 1986",
"Digital Research, Inc.",
"Tim Oren",
" About this program ",
"",
"",
" _OK",
" _Cancel",
" Save this picture as: ",
"",
"",
"",
"________.DOO",
"ANNNNNNN",
" _OK",
" _Cancel",
" Pen / Eraser selection ",
"",
"",
"Pens:",
"Pen Colors:",
"Selected:",
"Erasers:",
"[2][You are about|to write over|an existing file.][ _OK| _Cancel]",
"[3][ GEM does not have| any windows left,| GEM Demo aborting.][ _Sorr\
y ]"};

WORD IMAG0[] = {
0x0, 0x0, 0x3FFF, 0xFFF8, 
0x7FFF, 0xFFFC, 0xE000, 0xE, 
0xC000, 0x6, 0xC000, 0x6, 
0xC3F8, 0x3F86, 0xC7FC, 0x7FC6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x60C6, 0xC7FC, 0x60C6, 
0xC7FC, 0x7FC6, 0xC3F8, 0x3F86, 
0xC000, 0x6, 0xC000, 0x6, 
0xE000, 0xE, 0x7FFF, 0xFFFC, 
0x3FFF, 0xFFF8, 0x0, 0x0};

WORD IMAG1[] = {
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x3, 0x8000, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG2[] = {
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x7, 0xC000, 
0xF, 0xE000, 0x7, 0xC000, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG3[] = {
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x7, 0xC000, 0x1F, 0xF000, 
0x3F, 0xF800, 0x1F, 0xF000, 
0x7, 0xC000, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG4[] = {
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x7, 0xC000, 0x4, 0x4000, 
0x7, 0xC000, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG5[] = {
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x1F, 0xF000, 
0x10, 0x1000, 0x10, 0x1000, 
0x10, 0x1000, 0x1F, 0xF000, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG6[] = {
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x7F, 0xFC00, 0x40, 0x400, 
0x40, 0x400, 0x40, 0x400, 
0x40, 0x400, 0x40, 0x400, 
0x7F, 0xFC00, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

LONG rs_frstr[] = {
48L,
49L};

BITBLK rs_bitblk[] = {
0L, 4, 32, 0, 0, 1,
1L, 4, 16, 0, 0, 1,
2L, 4, 16, 0, 0, 1,
3L, 4, 16, 0, 0, 1,
4L, 4, 16, 0, 0, 1,
5L, 4, 16, 0, 0, 1,
6L, 4, 16, 0, 0, 1};

LONG rs_frimg[] = {
0};

ICONBLK rs_iconblk[] = {
0};

TEDINFO rs_tedinfo[] = {
28L, 29L, 30L, 3, 6, 2, 0x1191, 0x0, -1, 21,1,
33L, 34L, 35L, 3, 6, 2, 0x1191, 0x0, -1, 24,1,
36L, 37L, 38L, 3, 6, 0, 0x1180, 0x0, -1, 1,13,
41L, 42L, 43L, 3, 6, 2, 0x1191, 0x0, -1, 25,1};

OBJECT rs_object[] = {
-1, 1, 6, G_IBOX, NONE, NORMAL, 0x0L, 0,0, 80,25,
6, 2, 2, G_BOX, NONE, NORMAL, 0x1100L, 0,0, 80,513,
1, 3, 5, G_IBOX, NONE, NORMAL, 0x0L, 2,0, 23,769,
4, -1, -1, G_TITLE, NONE, NORMAL, 0x0L, 0,0, 6,769,
5, -1, -1, G_TITLE, NONE, NORMAL, 0x1L, 6,0, 7,769,
2, -1, -1, G_TITLE, NONE, NORMAL, 0x2L, 13,0, 10,769,
0, 7, 22, G_IBOX, NONE, NORMAL, 0x0L, 0,769, 80,19,
16, 8, 15, G_BOX, NONE, NORMAL, 0xFF1100L, 2,0, 20,8,
9, -1, -1, G_STRING, SELECTABLE, NORMAL, 0x3L, 0,0, 20,1,
10, -1, -1, G_STRING, NONE, DISABLED, 0x4L, 0,1, 20,1,
11, -1, -1, G_STRING, NONE, NORMAL, 0x5L, 0,2, 20,1,
12, -1, -1, G_STRING, NONE, NORMAL, 0x6L, 0,3, 20,1,
13, -1, -1, G_STRING, NONE, NORMAL, 0x7L, 0,4, 20,1,
14, -1, -1, G_STRING, NONE, NORMAL, 0x8L, 0,5, 20,1,
15, -1, -1, G_STRING, NONE, NORMAL, 0x9L, 0,6, 20,1,
7, -1, -1, G_STRING, NONE, NORMAL, 0xAL, 0,7, 20,1,
22, 17, 21, G_BOX, NONE, NORMAL, 0xFF1100L, 8,0, 13,5,
18, -1, -1, G_STRING, SELECTABLE, NORMAL, 0xBL, 0,0, 13,1,
19, -1, -1, G_STRING, SELECTABLE, DISABLED, 0xCL, 0,1, 13,1,
20, -1, -1, G_STRING, SELECTABLE, NORMAL, 0xDL, 0,2, 13,1,
21, -1, -1, G_STRING, SELECTABLE, DISABLED, 0xEL, 0,3, 13,1,
16, -1, -1, G_STRING, SELECTABLE, NORMAL, 0xFL, 0,4, 13,1,
6, 23, 25, G_BOX, NONE, NORMAL, 0xFF1100L, 15,0, 29,3,
24, -1, -1, G_STRING, SELECTABLE, NORMAL, 0x10L, 0,0, 29,1,
25, -1, -1, G_STRING, NONE, DISABLED, 0x11L, 0,1, 29,1,
22, -1, -1, G_STRING, 0x21, NORMAL, 0x12L, 0,2, 29,1,
-1, 1, 12, G_BOX, 0x3000, NORMAL, 0x3110BL, 8,9, 39,18,
2, -1, -1, G_BUTTON, 0x7, NORMAL, 0x13L, 27,16, 8,1,
12, 3, 11, G_BOX, NONE, SHADOWED, 0xFF1100L, 3,3, 34,12,
4, -1, -1, G_STRING, NONE, NORMAL, 0x14L, 13,1, 9,1,
5, -1, -1, G_STRING, NONE, NORMAL, 0x15L, 7,2, 22,1,
6, -1, -1, G_IMAGE, NONE, NORMAL, 0x0L, 1,4, 4,4,
7, -1, -1, G_STRING, NONE, NORMAL, 0x16L, 14,4, 7,1,
8, -1, -1, G_STRING, NONE, NORMAL, 0x17L, 14,5, 7,1,
9, -1, -1, G_STRING, NONE, NORMAL, 0x18L, 12,6, 12,1,
10, -1, -1, G_STRING, NONE, NORMAL, 0x19L, 2,9, 24,1,
11, -1, -1, G_STRING, NONE, NORMAL, 0x1AL, 2,10, 22,1,
2, -1, -1, G_STRING, NONE, NORMAL, 0x1BL, 14,7, 8,1,
0, -1, -1, G_BOXTEXT, LASTOB, NORMAL, 0x0L, 2,1, 36,1,
-1, 1, 4, G_BOX, 0x3000, NORMAL, 0x3110BL, 0,0, 45,9,
2, -1, -1, G_BUTTON, 0x7, NORMAL, 0x1FL, 35,7, 8,1,
3, -1, -1, G_BUTTON, 0x205, NORMAL, 0x20L, 25,7, 8,1,
4, -1, -1, G_BOXTEXT, NONE, NORMAL, 0x1L, 1,1, 43,1,
0, 5, 5, G_BOX, NONE, SHADOWED, 0xFF1100L, 2,3, 40,3,
4, -1, -1, G_FTEXT, 0x28, NORMAL, 0x2L, 15,1, 12,1,
-1, 1, 8, G_BOX, 0x3000, NORMAL, 0x2110BL, 8,9, 36,15,
2, -1, -1, G_IBOX, NONE, NORMAL, 0x1100L, 11,2, 22,7,
3, -1, -1, G_BUTTON, 0x7, NORMAL, 0x27L, 27,13, 7,1,
4, -1, -1, G_BUTTON, 0x205, NORMAL, 0x28L, 17,13, 8,1,
7, 5, 6, G_IBOX, NONE, NORMAL, 0x100L, 4,8, 29,5,
6, -1, -1, G_IBOX, NONE, NORMAL, 0x1100L, 15,1, 12,2,
4, -1, -1, G_IBOX, NONE, NORMAL, 0x1100L, 17,1, 8,1,
8, -1, -1, G_BOXTEXT, NONE, NORMAL, 0x3L, 1,1, 34,1,
0, 9, 25, G_BOX, NONE, SHADOWED, 0xFF1100L, 2,3, 32,9,
10, -1, -1, G_STRING, NONE, NORMAL, 0x2CL, 1,1, 5,1,
11, -1, -1, G_IMAGE, 0x11, NORMAL, 0x1L, 12,1, 4,2,
12, -1, -1, G_IMAGE, 0x11, NORMAL, 0x2L, 19,1, 4,2,
13, -1, -1, G_IMAGE, 0x11, NORMAL, 0x3L, 26,1, 4,2,
14, -1, -1, G_IMAGE, 0x11, NORMAL, 0x4L, 12,3, 4,2,
15, -1, -1, G_IMAGE, 0x11, NORMAL, 0x5L, 19,3, 4,2,
16, -1, -1, G_IMAGE, 0x11, NORMAL, 0x6L, 26,3, 4,2,
17, -1, -1, G_STRING, NONE, NORMAL, 0x2DL, 1,5, 11,1,
18, -1, -1, G_BOXCHAR, 0x3040, NORMAL, 0x4FF110BL, 15,5, 2,1,
19, -1, -1, G_BOXCHAR, TOUCHEXIT, NORMAL, 0x31FF1071L, 17,5, 2,1,
20, -1, -1, G_BOXCHAR, TOUCHEXIT, NORMAL, 0x32FF1072L, 19,5, 2,1,
21, -1, -1, G_BOXCHAR, TOUCHEXIT, NORMAL, 0x33FF1073L, 21,5, 2,1,
22, -1, -1, G_BOXCHAR, TOUCHEXIT, NORMAL, 0x34FF1074L, 23,5, 2,1,
23, -1, -1, G_BOXCHAR, 0x3040, NORMAL, 0x3FF110BL, 25,5, 2,1,
24, -1, -1, G_STRING, NONE, NORMAL, 0x2EL, 1,7, 9,1,
25, -1, -1, G_BOXCHAR, NONE, NORMAL, 0x31FF1071L, 15,7, 2,1,
8, -1, -1, G_STRING, LASTOB, NORMAL, 0x2FL, 1,3, 8,1};

LONG rs_trindex[] = {
0L,
26L,
39L,
45L};

struct foobar {
	WORD	dummy;
	WORD	*image;
	} rs_imdope[] = {
0, &IMAG0[0],
0, &IMAG1[0],
0, &IMAG2[0],
0, &IMAG3[0],
0, &IMAG4[0],
0, &IMAG5[0],
0, &IMAG6[0]};

#define NUM_STRINGS 50
#define NUM_FRSTR 2
#define NUM_IMAGES 7
#define NUM_BB 7
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_TI 4
#define NUM_OBS 71
#define NUM_TREE 4

BYTE pname[] = "DEMO.RSC";