/*
 * Handle USERDEF callbacks (16bit -> 32bit)
 * 
 */

#include <stdlib.h>
#include <dpmi.h>
#include "djgppgem.h"

static int callback_on = 0, callback_count = 0;
static _go32_dpmi_registers  paint_regs;
static _go32_dpmi_seginfo    paint_sinfo;

static LPVOID prealloc;
static int precount = 0;

static PARMBLK draw_pb;

static void paint_callback(_go32_dpmi_registers *regs)
{
	/* Entered with: ax:bx -> PARMBLK */
	LPPARM lp = (((LONG)(regs->x.ax)) << 4) + regs->x.bx;
	DJUSERBLK *djb;

	/* lp is linear pointer to PARMBLK */
	dosmemget(lp, sizeof(PARMBLK), &draw_pb);

	/* pb.pb_parm is address of 32-bit USERBLK */
	djb = (DJUSERBLK *)(draw_pb.pb_parm);
	
	/* Pass in the parameter as stored in the DJUSERBLK */
	draw_pb.pb_parm = djb->ub_parm;	
	draw_pb.pb_tree = LINEAR(draw_pb.pb_tree);
	
	regs->x.ax = (*djb->ub_code)(&draw_pb);
}



static void dj_userdef_off(void)
{
	if (callback_on) _go32_dpmi_free_real_mode_callback(&paint_sinfo);
	callback_on = 0;
}



#define DARC _go32_dpmi_allocate_real_mode_callback_retf

WORD dj_userdef(LPTREE tree, WORD object, DJUSERBLK *djb)
{

	if (!callback_on)
	{
		paint_sinfo.pm_offset = (LONG)paint_callback;
	
		if (!DARC(&paint_sinfo, &paint_regs))
		{
			callback_on = 1;
			atexit(dj_userdef_off);
		}
		else return 0;
	}
	if (callback_on)
	{
		WORD ob_type;

		if (precount)	/* Pre-allocated blocks available? */
		{
			djb->ub_16bit = prealloc;
			prealloc += 8;
			--precount;
		}
		else djb->ub_16bit = dos_alloc(8);	/* Allocate a 16-bit USERBLK */
		if (!djb->ub_16bit) return 0;
		LWSET(djb->ub_16bit,     paint_sinfo.rm_offset);	/* Callback code */
		LWSET(djb->ub_16bit + 2, paint_sinfo.rm_segment);
		LDSET(djb->ub_16bit + 4, (LONG)djb);		/* Address of 32-bit DJUSERBLK */

		/* Set object to USERDEF type */
		ob_type = LWGET(OB_TYPE(object));
		ob_type = (ob_type & 0xFF00) | G_USERDEF; 
		LWSET(OB_TYPE(object), ob_type);
		LSSET(OB_SPEC(object), djb->ub_16bit);
		++callback_count;
		return 1;
	}
	return 0;
}

#undef DARC

void dj_unuserdef(DJUSERBLK *djb)
{
	dos_free(djb->ub_16bit);	
	--callback_count;		
	if (!callback_count) dj_userdef_off();
}



WORD dj_userdef_prealloc(WORD count)	/* Allocate USERBLKs in advance */
{
	LPVOID ptr = dos_alloc(8*count);

	if (!ptr) return 0;

	prealloc = ptr;
	precount = count;

	return 1;
}
