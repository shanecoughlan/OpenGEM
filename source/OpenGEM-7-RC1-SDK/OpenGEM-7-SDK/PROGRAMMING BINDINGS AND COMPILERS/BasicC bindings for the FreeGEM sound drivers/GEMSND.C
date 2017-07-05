/*************************************************/
/* C bindings for Heinz Rath's GEM sound drivers */
/* Copyright (c) Owen Rudge 2000                 */
/* Last Updated: 19th November 2000              */
/*                                               */
/* Version 1.02                                  */
/*************************************************/

/* All functions up to version 0.4 are implemented. */
/* PlaySample has not been tested!!! */

#ifdef __WATCOMC__     // Watcom uses I86.H instead of DOS.H
	#include <i86.h>
#endif

#include <dos.h>
#include "gemsnd.h"

int cdecl vernum;        // Watcom wants them to all be declared cdecl.
int cdecl type;
int cdecl sbport;
int cdecl dma;

int cdecl stereo;
int cdecl output_16;
int cdecl hmver;

int DICalled = FALSE;

#ifndef __TURBOC__
	// not using Turbo C++ or Borland C++
	unsigned short _AX;
	unsigned short _BX;
	unsigned short _CX;
	unsigned short _DX;
	unsigned short _DI;
	unsigned short _SI;
	unsigned short _ES;

	void geninterrupt(int intr_num);
#endif

/* DriverInfo:  Returns information about the driver */
void DriverInfo()
{
	DICalled = TRUE;

	_AX = 0x2208;
	_BX = 1;

	geninterrupt(0xE1);

	vernum = _CX;
	type   = _DX;
	sbport = _AX;
	dma    = _SI;

	if (_DI && 0)
		stereo = TRUE;
	else
		stereo = FALSE;

	if (_DI && 1)
		output_16 = TRUE;
	else
		output_16 = FALSE;

	if (_DI && 2)
		hmver = TRUE;
	else
		hmver = FALSE;
}

/* UnloadDriver:  Unloads the sound driver */
void UnloadDriver()
{
	_AX = 0x2208;
	_BX = 2;

	geninterrupt(0xE1);
}

/* SpeakerOn:  Turns the speaker on */
void SpeakerOn()
{
	_AX = 0x2208;
	_BX = 3;

	geninterrupt(0xE1);
}

/* SpeakerOff:  Turns the speaker off */
void SpeakerOff()
{
	_AX = 0x2208;
	_BX = 4;

	geninterrupt(0xE1);
}

/* PlayWAV:  Plays a WAV file */
/*	   Returns TRUE if successful, FALSE otherwise */
int PlayWAV(char filename[])
{
	char *fn = filename;

	_AX = 0x2208;
	_BX = 5;
	_CX = FP_OFF(fn);
	_ES = FP_SEG(fn);

	geninterrupt(0xE1);

	if (_CX == 0)
		return(TRUE);
	else
		return(FALSE);
}

/* PlaySample: Plays an in-memory sample. NOT TESTED!!! */
void PlaySample(char sample[], int size, int khz)
{
	char *sm = sample;

	_AX = 0x2208;
	_BX = 6;
	_SI = FP_OFF(sm);
	_ES = FP_SEG(sm);
	_CX = size;
	_DX = khz;

	geninterrupt(0xE1);
}

/* WaitForDMA: Waits until playing ready (DMA ends) */
void WaitForDMA()
{
	_AX = 0x2208;
	_BX = 7;

	geninterrupt(0xE1);
}

/* DMAStop: Stops DMA */
void DMAStop()
{
	_AX = 0x2208;
	_BX = 8;

	geninterrupt(0xE1);
}

/* SetStereo: Turns stereo on or off (only exists in v0.2 or higher) */
/*	    If state = 1 then Stereo, if state = 0 then Mono */
void SetStereo(int state)
{
	if (!DICalled)
		DriverInfo();

	if (vernum >= 2)  // check if version 0.2 or higher
	{
		_AX = 0x2208;
		_BX = 9;
		_CX = state;

		geninterrupt(0xE1);
	}
}

/* GetStereo: Returns 1 if stereo, 0 if mono, -1 if incorrect version*/
int GetStereo()
{
	if (!DICalled)
		DriverInfo();

	if (vernum >= 2)  // check if version 0.2 or higher
	{
		_AX = 0x2208;
		_BX = 9;
		_DX = 1;

		geninterrupt(0xE1);
		return(_CX);
	}
	else
		return(-1);
}

/* Volume: Sets volume (0 - min, 255 - loudest) */
void Volume(int vol)
{
	if (!DICalled)
		DriverInfo();

	if (vernum >= 2)  // check if version 0.2 or higher
	{
		_AX = 0x2208;
		_BX = 10;
		_CX = vol;

		geninterrupt(0xE1);
	}
}

int PlayIWAV(char filename[])
{
	char *fn = filename;

	_AX = 0x2208;
		  _BX = 13;
	_CX = FP_OFF(fn);
	_ES = FP_SEG(fn);

	geninterrupt(0xE1);

	if (_CX == 0)
		return(TRUE);
	else
		return(FALSE);
}

void cdecl StopWAV()
{
	_AX = 0x2208;
   _BX = 15;

	geninterrupt(0xE1);
}

int cdecl isPlaying()
{
	_AX = 0x2208;
		  _BX = 14;

	geninterrupt(0xE1);

		  if (_CX == 1)
		return(TRUE);
	else
		return(FALSE);
}

#ifndef __TURBOC__
/* geninterrupt: Generates an interrupt - only for non-TC users.    */
/* NOTE: This version ONLY includes the registers needed by GEMSND! */
void geninterrupt(int intr_num)
{
	union REGS regs;
	struct SREGS sregs;

#ifdef __PACIFIC__       /* Pacific C is annoyingly different */
	regs.x.ax = _AX;
	regs.x.bx = _BX;
	regs.x.cx = _CX;
	regs.x.dx = _DX;
	regs.x.di = _DI;
	regs.x.si = _SI;
#else
	regs.w.ax = _AX;
	regs.w.bx = _BX;
	regs.w.cx = _CX;
	regs.w.dx = _DX;
	regs.w.di = _DI;
	regs.w.si = _SI;
#endif

	sregs.es = _ES;

	int86x(intr_num, &regs, &regs, &sregs);

#ifndef __PACIFIC__
	_AX = regs.w.ax;
	_BX = regs.w.bx;
	_CX = regs.w.cx;
	_DX = regs.w.dx;
	_DI = regs.w.di;
	_SI = regs.w.si;
#else
	_AX = regs.x.ax;
	_BX = regs.x.bx;
	_CX = regs.x.cx;
	_DX = regs.x.dx;
	_DI = regs.x.di;
	_SI = regs.x.si;
#endif
	_ES = sregs.es;
}
#endif
