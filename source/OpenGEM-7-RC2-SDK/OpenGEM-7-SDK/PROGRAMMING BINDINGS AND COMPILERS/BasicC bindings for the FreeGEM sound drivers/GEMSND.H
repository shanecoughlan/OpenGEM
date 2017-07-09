/*********************************************/
/* GEM Sound Driver C Bindings               */
/* Copyright (c) Owen Rudge 2000             */
/*********************************************/

#ifndef TRUE
	#define FALSE               0
   #define TRUE                1
#endif

#ifndef cdecl
   #define cdecl
#endif

extern int cdecl vernum;
extern int cdecl type;
extern int cdecl sbport;
extern int cdecl dma;

extern int cdecl stereo;
extern int cdecl output_16;
extern int cdecl hmver;

extern void cdecl DriverInfo();
extern void cdecl UnloadDriver();
extern void cdecl SpeakerOn();
extern void cdecl SpeakerOff();
extern int  cdecl PlayWAV(char filename[]);
extern void cdecl PlaySample(char sample[], int size, int khz);
extern void cdecl WaitForDMA();
extern void cdecl DMAStop();
extern void cdecl SetStereo(int state);
extern int  cdecl GetStereo();
extern void cdecl Volume(int vol);
extern int cdecl PlayIWAV(char filename[]);
extern void cdecl StopWAV();
extern int cdecl isPlaying();
