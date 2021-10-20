/* ============================================================ */
/* File: INIT8253.C						*/
/*								*/
/* Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall	*/
/*								*/
/* Called by INIT-CPU.ASM.					*/
/*								*/
/* Purpose: Initializes 8253 Programmable Interrupt Counter.	*/
/*								*/
/* Interrupts: Already disabled; remain disabled.		*/
/*								*/
/* Designed for use with the DJGPP port of the GNU C/C++	*/
/* protected mode 386 compiler.					*/
/*								*/
/* Modification History:					*/
/*								*/
/* ============================================================ */

#include "libepc.h"

#define	TIMER_CLK_FREQ			1193181	/* Hz */

#define	REFRESH_PERIOD			8 /* msec (all 512 rows)	*/
#define	REFRESH_RATE			((512 * 1000) / REFRESH_PERIOD)
#define	TICKS_PER_SEC			1000
#define	SPEAKER_FREQ			1000

/* -------------------------------------------------------------------- */
/* I/O Ports of the 8253 Programmable Timer Chip ...			*/
/* -------------------------------------------------------------------- */
#define	TIMER_CTR0_PORT			0x40	/* Timer tick rate	*/
#define	TIMER_CTR1_PORT			0x41	/* DRAM refresh rate	*/
#define	TIMER_CTR2_PORT			0x42	/* Speaker frequency	*/
#define	TIMER_CTRL_PORT			0x43

/* -------------------------------------------------------------------- */
/* Bit field codes for the control port (TIMER_CTRL_PORT)		*/
/* -------------------------------------------------------------------- */
#define	TIMER_SLCT_CTR0			0x40
#define	TIMER_SLCT_CTR1			0x80
#define	TIMER_SLCT_CTR2			0xC0

#define	TIMER_LATCH_COUNT		0x00
#define	TIMER_LOAD_LSBYTE		0x10
#define	TIMER_LOAD_MSBYTE		0x20
#define	TIMER_LOAD_BOTH			0x30

#define	TIMER_INT_ON_TC			0x00
#define	TIMER_1SHOT			0x02
#define	TIMER_RATE_GEN			0x04
#define	TIMER_SQWAVE_GEN		0x06
#define	TIMER_SW_STROBE			0x08
#define	TIMER_HW_STROBE			0x0A

#define	TIMER_USE_BCD			0x01

/* -------------------------------------------------------------------- */
/* Computed values to load into counter 0 (Timer tick rate)		*/
/* -------------------------------------------------------------------- */
#define	TIMER0_COMMAND			(TIMER_SLCT_CTR1	|\
					 TIMER_LOAD_BOTH	|\
					 TIMER_SQWAVE_GEN)
#define	TIMER0_DIVIDER			(TIMER_CLK_FREQ / TICKS_PER_SEC)

/* -------------------------------------------------------------------- */
/* Computed values to load into counter 1 (DRAM refresh rate)		*/
/* -------------------------------------------------------------------- */
#define	TIMER1_COMMAND			(TIMER_SLCT_CTR1	|\
					 TIMER_LOAD_BOTH	|\
					 TIMER_SQWAVE_GEN)
#define	TIMER1_DIVIDER			(TIMER_CLK_FREQ / REFRESH_RATE)

/* -------------------------------------------------------------------- */
/* Computed values to load into counter 2 (Speaker frequency)		*/
/* -------------------------------------------------------------------- */
#define	TIMER2_COMMAND			(TIMER_SLCT_CTR1	|\
					 TIMER_LOAD_BOTH	|\
					 TIMER_SQWAVE_GEN)
#define	TIMER2_DIVIDER			(TIMER_CLK_FREQ / SPEAKER_FREQ)

/* Hide this function's name from C programs! */
void Init8253(void) __asm__ ("Init8253") ;
void Init8253(void)
	{
	PUSHF ;
	CLI ;

	/* Program Timer0: Timer Tick */
	outportb(TIMER_CTRL_PORT, TIMER0_COMMAND) ;
	outportb(TIMER_CTR0_PORT, LSB(TIMER0_DIVIDER)) ;
	outportb(TIMER_CTR0_PORT, MSB(TIMER0_DIVIDER)) ;

	/* Program Timer1: DRAM Refresh Rate */
	outportb(TIMER_CTRL_PORT, TIMER1_COMMAND) ;
	outportb(TIMER_CTR1_PORT, LSB(TIMER1_DIVIDER)) ;
	outportb(TIMER_CTR1_PORT, MSB(TIMER1_DIVIDER)) ;

	/* Program Timer2: Speaker Frequency */
	outportb(TIMER_CTRL_PORT, TIMER2_COMMAND) ;
	outportb(TIMER_CTR2_PORT, LSB(TIMER2_DIVIDER)) ;
	outportb(TIMER_CTR2_PORT, MSB(TIMER2_DIVIDER)) ;

	POPF ;
	}
