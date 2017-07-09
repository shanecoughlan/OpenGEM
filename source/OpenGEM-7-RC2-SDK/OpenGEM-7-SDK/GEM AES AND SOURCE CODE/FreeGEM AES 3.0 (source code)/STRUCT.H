/*	STRUCT.H	1/28/84 - 01/18/85	Lee Jay Lorenzen	*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1986			Digital Research Inc.
*	-------------------------------------------------------------
*/

/* [JCE] For Pacific C, use typedefs rather than #defines, and replace 
 * LONGs by real far pointers */
typedef UWORD	EVSPEC;

#define NUM_PDS (NUM_ACCS + 2)		/* acc's + ctrlpd + dos appl.	*/
#define NUM_IEVBS (2 * 5)		/* 5 * the number of internalPDs*/
#define NUM_EEVBS (NUM_ACCS * 5)	/* 5 * the number of externalPDs*/
#define KBD_SIZE 8
#define QUEUE_SIZE 128
#define STACK_SIZE 448
#define NFORKS 32

typedef struct cqueue
{
	WORD	c_buff[KBD_SIZE];
	WORD	c_front;
	WORD	c_rear;
	WORD	c_cnt;
} CQUEUE;


typedef struct mfstr
{
	WORD	mf_xhot;
	WORD	mf_yhot;
	WORD	mf_nplanes;
	WORD	mf_fg;
	WORD	mf_bg;
	WORD	mf_mask[16];
	WORD	mf_data[16];
} MFORM;
typedef MFORM FAR *LPMFORM;


#define C_KOWNER 0x0001
#define C_MOWNER 0x0002

typedef struct cda
{
	UWORD		c_flags;
	struct evb	*c_iiowait;	/* Waiting for Input		*/
	struct evb	*c_msleep;	/* wait for mouse rect		*/
	struct evb	*c_bsleep;	/* wait for button		*/
	CQUEUE		c_q;		/* input queue 			*/
} CDA;
typedef CDA FAR *LPCDA;

#if I8086
typedef struct uda
{
	WORD	u_insuper;		/* in supervisor flag		*/ 
	WORD	*u_spsuper;		/* supervisor stack offset	*/
	UWORD	u_sssuper;		/* supervisor stack segment	*/
	WORD	*u_spuser;		/* user stack offset		*/
	UWORD	u_ssuser;		/* user stack segment		*/
	UWORD	u_regs[9];		/* ds,es,ax,bx,cx,dx,si,di,bp	*/
	WORD	u_super[STACK_SIZE];
	WORD	u_supstk;
} UDA;
#endif
#if MC68K
typedef struct uda
{
	WORD	u_insuper;		/* in supervisor flag		*/ 
	ULONG	u_regs[15];		/* d0-d7, a0-a6			*/
	ULONG	*u_spsuper;		/* supervisor stack 		*/
	ULONG	*u_spuser;		/* user stack 			*/
	ULONG	u_super[STACK_SIZE];
	ULONG	u_supstk;
} UDA;
#endif
typedef UDA FAR *LPUDA;


#define NOCANCEL 0x0001		/* event is occuring */
#define COMPLETE 0x0002		/* event completed */
#define EVDELAY  0x0004		/* event is delay event */
#define EVMOUT   0x0008		/* event flag for mouse wait outside of rect*/

typedef struct evb 		/* event block structure */
{
	struct evb	*e_nextp;	/* link to next event on PD event list */
	struct evb	*e_link;	/* link to next block on event chain */
	struct evb	*e_pred;	/* link to prev block on event chain */
	BYTE		*e_pd;		/* owner PD (data for fork) */
	LONG		e_parm;		/* parm for request -> event comm */
	WORD		e_flag;
	EVSPEC		e_mask;		/* mask for event notification */
	LONG		e_return;
} EVB;
typedef EVB FAR *LPEVB;

/* pd defines */
/* p_stat */
#define		WAITIN		0x0001
#define		SWITCHIN	0x8000

typedef struct pd
{
	struct pd	*p_link;
	struct pd	*p_thread;
	UDA			*p_uda;

	BYTE		p_name[8];

	CDA			*p_cda;		/* cio data area 	*/
	LPBYTE		p_ldaddr;	/* long addr. of load	*/
	WORD 		p_pid;
	WORD		p_stat;

	EVSPEC		p_evbits;	/* event bits in use 	*/
	EVSPEC		p_evwait;	/* event wait mask 	*/
	EVSPEC		p_evflg;	/* event flags 		*/

	EVB		*p_evlist;
	EVB		*p_qdq;
	EVB		*p_qnq;
	struct qpb FAR *p_qaddr;
	WORD	p_qindex;	
	BYTE	p_queue[QUEUE_SIZE];
} PD;
typedef PD FAR *LPPD;


typedef struct qpb
{
	PD		*qpb_ppd;
	WORD	qpb_cnt;
	LPVOID	qpb_buf;
} QPB;
typedef QPB FAR *LPQPB;


typedef struct spb
{
	WORD	sy_tas;
	PD		*sy_owner;
	EVB		*sy_wait;
} SPB;
typedef SPB FAR *LPSPB;


/* [JCE] for casting in PPD */

typedef WORD (*FPD_CODE)(WORD, WORD);


typedef struct FPD
{
	FPD_CODE f_code;
	LONG	 f_data;
} FPD;
typedef FPD FAR *LPFPD;

