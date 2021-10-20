/* ============================================================ */
/* File: TIMER.C						*/
/*								*/
/* Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall	*/
/*								*/
/* Purpose: Library routines for access to the 8253 timer.	*/
/* See file LIBEPC.H for function descriptions.			*/
/*								*/
/* Designed for use with the DJGPP port of the GNU C/C++	*/
/* protected mode 386 compiler.					*/
/*								*/
/* Modification History:					*/
/*								*/
/* ============================================================ */

#include "libepc.h"

extern	void	TimerTickISR(void)	__asm__("TimerTickISR")	;
extern	DWORD32	msec			__asm__("msec")		;
extern	ISR	old_tick_isr		__asm__("old_tick_isr")	;

PRIVATE void InstallTimerTickISR(void)
	{
	static BOOL virgin = TRUE ;

	if (virgin)
		{
		old_tick_isr = GetISR(IRQ2INT(IRQ_TICK)) ;
		SetISR(IRQ2INT(IRQ_TICK), TimerTickISR) ;
		outportb(0x21, inportb(0x21) & ~0x01) ;
		STI ;
		virgin = FALSE ;
		}
	}

DWORD32 Milliseconds(void)
	{
	InstallTimerTickISR() ;
	return msec ;
	}

DWORD32 Now_Plus(int seconds)
	{
	InstallTimerTickISR() ;
	return msec + 1000 * seconds ;
	}

