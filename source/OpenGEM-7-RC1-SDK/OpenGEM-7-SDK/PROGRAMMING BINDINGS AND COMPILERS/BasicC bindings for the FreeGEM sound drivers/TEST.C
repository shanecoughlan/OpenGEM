/***********************************************/
/* GEM Sound Driver - C Bindings Test Program  */
/* Copyright (c) Owen Rudge 2000               */
/***********************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>

#ifdef __WATCOMC__
    #include <graph.h>

    void clrscr();
    void gotoxy(int x, int y);
    int wherex();
    int wherey();
    void textcolor();

    void ctrlbrk(void (*func)(void) );
#endif

#ifdef __TURBOC__
   #define PRINTTIME
#endif

#include "gemsnd.h"

int background = FALSE;

int c_break(void);

int main(int argc, char *argv[])
{
        int ino, i;
#ifdef PRINTTIME
        int j, oldx, oldy;
#endif
        
        if (argc < 2)
        {
                printf("GEM Sound Drivers Test Program\n");
                printf("Copyright (c) Owen Rudge 2000\n");
                printf("\n");
                printf("Syntax: %s [/b] file1 [file2] [file3] ...\n", argv[0]);
                printf("\n");
                printf("/b      Plays the selected files in the background\n");
                exit(1);
        }

#ifndef __PACIFIC__
        clrscr();
#endif
        printf("\n\n");

        DriverInfo();

        if (strstr(argv[1], "/B") != NULL)
                background = TRUE;
        else if (strstr(argv[1], "/b") != NULL)
                background = TRUE;

        printf("Driver Version:      %d\n", vernum);
        printf("SB Port:             %x\n", sbport);
        printf("DMA:                 %d\n", dma);
        printf("Stereo?:             %s\n", stereo == TRUE ? "Yes" : "No");
        printf("16-bit Output?       %s\n", output_16 == TRUE ? "Yes" : "No");
        printf("Play in Background?  %s\n\n", background == TRUE ? "Yes" : "No");

        SpeakerOn();
#ifndef __PACIFIC__
        ctrlbrk(c_break);
#endif
        
        if (background == TRUE)
        {
                printf("Press any key to move on to the next file\n\n");
                ino = 2;
        }
        else
                ino = 1;

        for (i = ino; i < argc; i++)
        {
                printf("Playing: %s\n", argv[i]);

                if (background == TRUE)
                {
                        PlayIWAV(argv[i]);

#ifdef PRINTTIME
                        oldx = wherex();
                        oldy = wherey();

                        gotoxy(1, 1);
                        for (j = 0; j < 80; j++)
                            printf(" ");

                        gotoxy(1, 1);
                        printf("Time:");

                        j = 0;

                        gotoxy(oldx, oldy);
#endif
                        while(!kbhit())
                        {
                                if (!isPlaying())
                                        break;

#ifdef PRINTTIME
                                if (j >= 32767) j = 0;
                                j++;
                                delay(1000);

                                oldx = wherex();
                                oldy = wherey();

                                gotoxy(5, 1);
                                printf("%3d:%02d", j/60, j);
                                gotoxy(oldx, oldy);
#endif
                        }

                        if (kbhit()) getch();  // if there is a key pressed, get rid of it

                        StopWAV();
                }
                else
                        PlayWAV(argv[i]);
        }

        SpeakerOff();

        printf("\nDone!\n");
        return(0);
}

#ifndef __PACIFIC__
int c_break(void)
{
        int i;

        for (i = 0; i < 80; i++)
                printf(" ");

        gotoxy(1, 1);
        textcolor(14);
#ifdef __WATCOMC__
        printf("Bye...\n");
#else
        cprintf("CONTROL+BREAK pressed. Aborting.\n");
#endif   
        textcolor(15);

        if (background == TRUE) StopWAV();
        SpeakerOff();

        return(0);
}
#endif

#ifdef __WATCOMC__

void clrscr()
{
    _clearscreen(_GCLEARSCREEN);
}

void gotoxy(int x, int y)
{
    _settextposition(y, x);
}

int wherex()
{
    struct rccoord posstruct;

    posstruct = _gettextposition();
    return(posstruct.row);
}

int wherey()
{
    struct rccoord posstruct;

    posstruct = _gettextposition();
    return(posstruct.col);
}

void textcolor(int newcolour)
{
    _settextcolor(newcolour);
}

/* Watcom doesn't have a ctrlbrk, so use atexit() */
void ctrlbrk(void (*func)(void) )
{
    atexit(func);
}
#endif
