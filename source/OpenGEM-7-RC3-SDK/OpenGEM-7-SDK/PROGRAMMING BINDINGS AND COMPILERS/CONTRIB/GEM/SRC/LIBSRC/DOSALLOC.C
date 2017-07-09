/*
 * [JCE 13-4-1998] Manage DOS memory from 32-bit code.
 * 
 * This is a simple malloc/free for the area of memory addressable by 
 * 16-bit GEM. Ideally it should only be used to allocate/free a few large 
 * areas. 
 */

#include <stdlib.h>
#include <string.h>
#include <dpmi.h>
#include "djgppgem.h"

typedef struct		/* There is one of these for each DOS block allocated. */
{			/* It gives the base (as an LPVOID, ie an absolute */
			/* offset from the DOS_DS), the length of the block, */
	LPVOID base;	/* and the selector to use to deallocate it. */
	WORD len;
	WORD selector;
} BLOCK;

static int    initialised = 0;
static int    block_count = 32;
static BLOCK *blocklist;

static void deinitialise(void);	/* Forward declaration */

static int initialise(void)	/* Create the list of blocks */
{
	int n;

	if (!(blocklist = (BLOCK *)malloc(32 * sizeof(BLOCK)))) return 0;

	for (n = 0; n < 32; n++)
	{
		memset(&blocklist[n], 0, sizeof(BLOCK));
	}

	atexit(deinitialise);
	initialised = 1;
	return 1;
}

static void deinitialise(void)
{
	/* Free the blocks we have used */

	int n;

	for (n = 0; n < block_count; n++)
	{
		if (blocklist[n].len) __dpmi_free_dos_memory(blocklist[n].selector);
	}

	free(blocklist);
	initialised = 0;
}

/* The blocklist array needs more entries. Double its size. */

static int grow_blocklist(void)
{
	BLOCK *newlist;
	int n;
	int max = 2 * block_count;

	newlist = (BLOCK *)malloc(max * sizeof(BLOCK));
	if (newlist == NULL) return -1;

	memcpy(newlist, blocklist, block_count * sizeof(BLOCK));

	for (n = block_count; n < max; n++)
	{
		memset(&newlist[n], 0, sizeof(BLOCK));
	}

	n = block_count;
	block_count = max;
	free(blocklist);
	blocklist = newlist;
	return n;
}

/* Find a free block descriptor */

static int find_free_block(void)	/* This is a wretched linear algorithm */
{
	int n;

	for (n = 0; n < block_count; n++) if (!blocklist[n].len) return n;

	return grow_blocklist();
}

LPVOID dos_alloc(LONG size)	/* Allocate memory in the DOS area */
{
	int    selector;
	int    segment;
	int    nblock;

	if ((!initialised) && (!initialise())) return 0L;

	if (!size) return 0L;

	nblock = find_free_block();
	if (nblock < 0) return 0L;

	segment = __dpmi_allocate_dos_memory( ((size + 15)>>4), &selector);

	if (segment == -1) return 0L;

	blocklist[nblock].selector = selector;
	blocklist[nblock].len      = size;
	blocklist[nblock].base     = segment << 4;

	return blocklist[nblock].base;
}


void dos_free(LPVOID memory)
{
	int n;

	if ((!initialised) && (!initialise())) return;

	/* This is a wretched linear algorithm */

	for (n = 0; n < block_count; n++) if (blocklist[n].base == memory)
	{
		blocklist[n].len = 0;
		__dpmi_free_dos_memory(blocklist[n].selector);
		return;
	}
}
