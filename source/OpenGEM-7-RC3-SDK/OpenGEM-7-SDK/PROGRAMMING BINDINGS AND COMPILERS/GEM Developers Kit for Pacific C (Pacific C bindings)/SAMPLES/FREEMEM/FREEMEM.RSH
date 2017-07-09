#define T0OBJ 0
#define FREEBB 0
#define FREEIMG 0
#define FREESTR 21

BYTE *rs_strings[] = {
"VerfÅgbarer Speicher",
"",
"",
"112341234",
"",
"",
"Datum und Zeit",
"",
"",
"Mo, 12.12.1988 - 13:57:56",
"",
"",
"Betriebssystemversion",
"",
"",
"DRDOS - V3.40",
"",
"",
"(C)1988 Digital Research GmbH",
"",
""};

LONG rs_frstr[] = {
0};

BITBLK rs_bitblk[] = {
0};

LONG rs_frimg[] = {
0};

ICONBLK rs_iconblk[] = {
0};

TEDINFO rs_tedinfo[] = {
0L, 1L, 2L, 5, 6, 2, 0x1180, 0x0, -1, 21,1,
3L, 4L, 5L, 3, 6, 2, 0x1181, 0x0, -1, 10,1,
6L, 7L, 8L, 5, 6, 2, 0x1180, 0x0, -1, 15,1,
9L, 10L, 11L, 3, 6, 2, 0x1181, 0x0, -1, 26,1,
12L, 13L, 14L, 5, 6, 2, 0x1180, 0x0, -1, 22,1,
15L, 16L, 17L, 3, 6, 2, 0x1181, 0x0, -1, 14,1,
18L, 19L, 20L, 5, 6, 2, 0x1180, 0x0, -1, 30,1};

OBJECT rs_object[] = {
-1, 1, 7, G_BOX, NONE, NORMAL, 0x11100L, 0,0, 29,7,
2, -1, -1, G_TEXT, NONE, NORMAL, 0x0L, 0,0, 29,1,
3, -1, -1, G_TEXT, NONE, NORMAL, 0x1L, 0,1, 29,1,
4, -1, -1, G_TEXT, NONE, NORMAL, 0x2L, 0,2, 29,1,
5, -1, -1, G_TEXT, NONE, NORMAL, 0x3L, 0,3, 29,1,
6, -1, -1, G_TEXT, NONE, NORMAL, 0x4L, 0,4, 29,1,
7, -1, -1, G_TEXT, NONE, NORMAL, 0x5L, 0,5, 29,1,
0, -1, -1, G_TEXT, LASTOB, NORMAL, 0x6L, 0,6, 29,1};

LONG rs_trindex[] = {
0L};

struct foobar {
	WORD	dummy;
	WORD	*image;
	} rs_imdope[] = {
0};

#define NUM_STRINGS 21
#define NUM_FRSTR 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_TI 7
#define NUM_OBS 8
#define NUM_TREE 1

BYTE pname[] = "FREEMEM.RSC";