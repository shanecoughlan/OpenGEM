/*
 * Utility functions which will shift entire object trees between 
 * 16-bit and 32-bit memory spaces
 *
 * Important assumptions:
 *
 * 1. USERDEF (program-drawn) objects have a USERBLK as their object 
 *   spec, and this is always in low memory. This is because the 
 *   USERBLK is managed by dj_userdef().
 *
 * 2. Objects with the INDIRECT flag set point to a struct INDBLOCK:
 *
 */

typedef struct indblock
    {
    LONG    ind_spec;    /* Real object spec */
    LONG    ind_len;     /* Length of this block in bytes */
    /* ... additional data ... */
    } INDBLOCK;

/* This is necessary because otherwise the indirect data cannot be moved
 * between low & normal memory; the bindings don't know how much to copy.
 *
 */

/* Find the size of an MFDB and its planes. */

LONG dj_mfdb_size32(MFDB *fdb);
LONG dj_mfdb_size16(LPWORD mfdb);

/* Find the size of an object tree and every object it refers to. There are 
 * versions for 32-bit and 16-bit trees.
 *
 * We allow space for all objects to be aligned on a 4-byte boundary. 
 */
LONG dj_tree_size32(OBJECT *tree);
LONG dj_tree_size16(LPTREE tree);

/* Push an object tree between GEM memory & DJGPP memory */

void dj_tree_32to16(OBJECT *tree, LPTREE tree16);
void dj_tree_16to32(LPTREE tree16, OBJECT *tree);

/* A "shallow" copy, only doing tree objects themselves, the contents of 
 * text controls and image bitmaps */

void dj_tree_16to32s(LPTREE tree16, OBJECT *tree);
void dj_tree_32to16s(OBJECT *tree, LPTREE tree16);

/* Find the size of an MFDB and its planes. */
LONG dj_mfdb_size32(MFDB  *mfdb);
LONG dj_mfdb_size16(LPMFDB mfdb);

/* Move an MFDB between GEM memory & DJGPP memory
 * 
 * These are passed the address of a pointer for the destination MFDB. 
 * On return, the pointer will point at the first character after the
 * destination; while the address of the destination itself is returned
 * from the function.
 */

LPMFDB dj_mfdb_32to16(MFDB  *mfdb,   LPMFDB *dest);
MFDB  *dj_mfdb_16to32(LPMFDB mfdb16, BYTE  **dest);

/* mf2b_2ops() is designed to handle operations on MFDBs (no matter how big)
 *             by splitting them up into pieces small enough to copy into 
 *             low memory, and then operating on the bits.
 */

typedef void (*MFUNC)(LPMFDB mfsrc, LPMFDB mfdst, int sy, int dy, int sh, int dh, void *lParam);

void mfdb_2ops(MFDB *src, MFDB *dst, MFUNC cpyfunc, void *lParam);
