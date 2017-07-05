/*
  Copyright (c) 1990-2001 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
//******************************************************************************
//
// File:        WINCE.H
//
// Description: This file declares all the Win32 APIs and C runtime functions
//              that the Info-ZIP code calls, but are not implemented natively
//              on Windows CE.  See WINCE.CPP for the implementation.
//
// Copyright:   All the source files for Pocket UnZip, except for components
//              written by the Info-ZIP group, are copyrighted 1997 by Steve P.
//              Miller.  The product "Pocket UnZip" itself is property of the
//              author and cannot be altered in any way without written consent
//              from Steve P. Miller.
//
// Disclaimer:  All project files are provided "as is" with no guarantee of
//              their correctness.  The authors are not liable for any outcome
//              that is the result of using this source.  The source for Pocket
//              UnZip has been placed in the public domain to help provide an
//              understanding of its implementation.  You are hereby granted
//              full permission to use this source in any way you wish, except
//              to alter Pocket UnZip itself.  For comments, suggestions, and
//              bug reports, please write to stevemil@pobox.com.
//
//
// Date      Name          History
// --------  ------------  -----------------------------------------------------
// 02/01/97  Steve Miller  Created (Version 1.0 using Info-ZIP UnZip 5.30)
//
//******************************************************************************

#ifndef __WINCE_H__
#define __WINCE_H__

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
//***** For all platforms - Our debug output function
//******************************************************************************

// If we are building for debug, we implement the DebugOut() function. If we are
// building for release, then we turn all calls to DebugOut() into no-ops.  The
// Microsoft compiler (and hopefully others) will not generate any code at all
// for the retail version of DebugOut() defined here.  This works much better
// than trying to create a variable argument macro - something C/C++ does not
// support cleanly.

#ifdef DEBUG
void DebugOut(LPCTSTR szFormat, ...);
#else
__inline void DebugOut(LPCTSTR szFormat, ...) {}
#endif


//******************************************************************************
//***** Windows NT Native
//******************************************************************************

#if !defined(_WIN32_WCE)
#ifndef UNICODE
#include <stdio.h>
#endif
#include <io.h>
#include <time.h>
#include <fcntl.h>
#include <sys\stat.h>
#endif

//******************************************************************************
//***** Windows CE Native
//******************************************************************************

#if defined(_WIN32_WCE)

#ifndef ZeroMemory
#define ZeroMemory(Destination,Length) memset(Destination, 0, Length)
#endif

// A few forgotten defines in Windows CE's TCHAR.H
#ifndef _stprintf
#define _stprintf wsprintf
#endif

#ifndef _vsntprintf
#define _vsntprintf(d,c,f,a) wvsprintf(d,f,a)
#endif


//******************************************************************************
//***** SYS\TYPES.H functions
//******************************************************************************

typedef long _off_t;
typedef long time_t;


//******************************************************************************
//***** CTYPE.H functions
//******************************************************************************

_CRTIMP int __cdecl isupper(int);
_CRTIMP int __cdecl tolower(int);


//******************************************************************************
//***** FCNTL.H functions
//******************************************************************************

#define _O_RDONLY 0x0000   // open for reading only
//#define _O_WRONLY  0x0001   // open for writing only
//#define _O_RDWR    0x0002   // open for reading and writing
//#define _O_APPEND  0x0008   // writes done at eof

//#define _O_CREAT   0x0100   // create and open file
//#define _O_TRUNC   0x0200   // open and truncate
//#define _O_EXCL    0x0400   // open only if file doesn't already exist


//#define _O_TEXT    0x4000   // file mode is text (translated)
#define _O_BINARY 0x8000   // file mode is binary (untranslated)


#define O_RDONLY  _O_RDONLY
//#define O_WRONLY   _O_WRONLY
//#define O_RDWR     _O_RDWR
//#define O_APPEND   _O_APPEND
//#define O_CREAT    _O_CREAT
//#define O_TRUNC    _O_TRUNC
//#define O_EXCL     _O_EXCL
//#define O_TEXT     _O_TEXT
#define O_BINARY  _O_BINARY
//#define O_RAW      _O_BINARY
//#define O_TEMPORARY   _O_TEMPORARY
//#define O_NOINHERIT   _O_NOINHERIT
//#define O_SEQUENTIAL  _O_SEQUENTIAL
//#define O_RANDOM   _O_RANDOM

//******************************************************************************
//***** IO.H functions
//******************************************************************************

_CRTIMP int __cdecl chmod(const char *, int);
_CRTIMP int __cdecl close(int);
_CRTIMP int __cdecl isatty(int);
_CRTIMP long __cdecl lseek(int, long, int);
_CRTIMP int __cdecl open(const char *, int, ...);
_CRTIMP int __cdecl read(int, void *, unsigned int);
_CRTIMP int __cdecl setmode(int, int);
_CRTIMP int __cdecl unlink(const char *);


//******************************************************************************
//***** STDIO.H functions
//******************************************************************************


//typedef struct _iobuf FILE;
typedef int FILE;

#define stdin  ((int*)-2)
#define stdout ((int*)-3)
#define stderr ((int*)-4)

#define EOF    (-1)

_CRTIMP int __cdecl fflush(FILE *);
_CRTIMP char * __cdecl fgets(char *, int, FILE *);
_CRTIMP int __cdecl fileno(FILE *);
_CRTIMP FILE * __cdecl fopen(const char *, const char *);
_CRTIMP int __cdecl fprintf(FILE *, const char *, ...);
_CRTIMP int __cdecl fclose(FILE *);
_CRTIMP int __cdecl putc(int, FILE *);
_CRTIMP int __cdecl sprintf(char *, const char *, ...);


//******************************************************************************
//***** STRING.H functions
//******************************************************************************

_CRTIMP int     __cdecl _stricmp(const char *, const char *);
_CRTIMP char *  __cdecl _strupr(char *);
_CRTIMP char *  __cdecl strrchr(const char *, int);


//******************************************************************************
//***** TIME.H functions
//******************************************************************************

#ifndef _TM_DEFINED
struct tm {
   int tm_sec;     // seconds after the minute - [0,59]
   int tm_min;     // minutes after the hour - [0,59]
   int tm_hour;    // hours since midnight - [0,23]
   int tm_mday;    // day of the month - [1,31]
   int tm_mon;     // months since January - [0,11]
   int tm_year;    // years since 1900
// int tm_wday;    // days since Sunday - [0,6]
// int tm_yday;    // days since January 1 - [0,365]
   int tm_isdst;   // daylight savings time flag
};
#define _TM_DEFINED
#endif

_CRTIMP struct tm * __cdecl localtime(const time_t *);


//******************************************************************************
//***** SYS\STAT.H functions
//******************************************************************************

struct stat {
// _dev_t st_dev;
// _ino_t st_ino;
   unsigned short st_mode;
// short st_nlink;
// short st_uid;
// short st_gid;
// _dev_t st_rdev;
   _off_t st_size;
// time_t st_atime;
   time_t st_mtime;
// time_t st_ctime;
};

#define _S_IFMT   0170000  // file type mask
#define _S_IFDIR  0040000  // directory
//#define _S_IFCHR   0020000  // character special
//#define _S_IFIFO   0010000  // pipe
#define _S_IFREG  0100000  // regular
#define _S_IREAD  0000400  // read permission, owner
#define _S_IWRITE 0000200  // write permission, owner
#define _S_IEXEC  0000100  // execute/search permission, owner

#define S_IFMT  _S_IFMT
#define S_IFDIR  _S_IFDIR
//#define S_IFCHR  _S_IFCHR
//#define S_IFREG  _S_IFREG
#define S_IREAD  _S_IREAD
#define S_IWRITE _S_IWRITE
#define S_IEXEC  _S_IEXEC

_CRTIMP int __cdecl stat(const char *, struct stat *);


//******************************************************************************

#endif // _WIN32_WCE

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __WINCE_H__
