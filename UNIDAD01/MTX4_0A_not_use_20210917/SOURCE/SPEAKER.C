/* ============================================================ */
/* File: SPEAKER.C						*/
/*								*/
/* Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall	*/
/*								*/
/* Purpose: Library routines for access to the speaker.		*/
/* See file LIBEPC.H for function descriptions.			*/
/*								*/
/* Designed for use with the DJGPP port of the GNU C/C++	*/
/* protected mode 386 compiler.					*/
/*								*/
/* Modification History:					*/
/*								*/
/* ============================================================ */

#include "libepc.h"

#define	SPKR_CTRL_PORT		0x61
#define	SPKR_ENBL_BIT		0x02
#define	SPKR_TMR2_ENBL_BIT	0x01

#define	TIMER_CTRL_PORT		0x43
#define	TIMER_CNTR2_PORT	0x42
#define	TIMER_CLK_FREQ		1193181L

void Sound(int hertz)
	{
	WORD16 counter ;

	if (hertz <= 0)
		{
		/* Disable Speaker */
		outportb(SPKR_CTRL_PORT, 0x00) ;
		return ;
		}

	/* Set speaker frequency by programming timer channel #2 */
	PUSHF ;
	CLI ;
	counter = TIMER_CLK_FREQ / hertz ;
	outportb(TIMER_CTRL_PORT, 0xB6) ;
	outportb(TIMER_CNTR2_PORT, ((BYTE8 *) &counter)[0]) ;
	outportb(TIMER_CNTR2_PORT, ((BYTE8 *) &counter)[1]) ;
	POPF ;

	/* Enable Speaker */
	outportb(SPKR_CTRL_PORT, SPKR_ENBL_BIT | SPKR_TMR2_ENBL_BIT) ;
	}
