/* DOSBIND.H	extern declarations from dos binding. */

#define F_RDONLY 0x01
#define F_HIDDEN 0x02
#define F_SYSTEM 0x04
#define F_VOLUME 0x08
#define F_SUBDIR 0x10
#define F_ARCHIVE 0x20

#define DTA_LEN 128		

EXTERN UWORD	DOS_ERR;

#if I8086
EXTERN WORD	__DOS();
EXTERN UWORD	DOS_AX;
EXTERN UWORD	DOS_BX;
EXTERN BYTE	_dta[ DTA_LEN ] ;
#endif

EXTERN 	WORD dos_label();
EXTERN 	WORD dos_space();
EXTERN 	WORD dos_rmdir();
EXTERN 	WORD dos_chdir();
EXTERN 	WORD dos_gdir();
EXTERN 	WORD dos_sdta();
EXTERN 	WORD dos_sfirst();
EXTERN 	WORD dos_snext();
EXTERN 	WORD dos_gdrv();
EXTERN 	WORD dos_sdrv();
EXTERN 	WORD dos_create();
EXTERN 	WORD dos_mkdir();
EXTERN 	WORD dos_open();
EXTERN 	WORD dos_delete();
EXTERN 	WORD dos_rename();
EXTERN 	WORD dos_close();
EXTERN 	LONG dos_read();
EXTERN 	LONG dos_write();
EXTERN 	LONG dos_lseek();
EXTERN 	VOID dos_exec();
EXTERN 	WORD dos_wait();
EXTERN 	LONG dos_envir();
EXTERN 	WORD dos_chmod();
EXTERN 	LONG dos_alloc();
EXTERN 	WORD dos_free();
EXTERN 	WORD dos_setdt();
EXTERN 	WORD dos_find();
EXTERN  LONG dos_gint();
EXTERN  VOID dos_sint();
EXTERN	VOID dos_fdup();
EXTERN  LONG dos_avail() ;
