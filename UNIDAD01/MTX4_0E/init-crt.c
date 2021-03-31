/* ============================================================ */
/* File: INIT-CRT.C						*/
/*								*/
/* Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall	*/
/*								*/
/* Called by INIT-CPU.ASM.					*/
/*								*/
/* Purpose: C run-time system initialization.			*/
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

extern BYTE8 text_frst		__asm__("text_frst")	;
extern BYTE8 text_last		__asm__("text_last")	;
extern BYTE8 data_frst		__asm__("data_frst")	;
extern BYTE8 data_last		__asm__("data_last")	;
extern BYTE8 bss_frst		__asm__("bss_frst")	;
extern BYTE8 bss_last		__asm__("bss_last")	;
extern BYTE8 stack_frst		__asm__("stack_frst")	;
extern BYTE8 stack_last		__asm__("stack_last")	;

PRIVATE BOOL IsReadWrite(DWORD32 top) ;

/* ------------------------------------------------------------ */
/* Public functions ...						*/
/* ------------------------------------------------------------ */
/* Hide this function's name from C programs! */
void Init_CRT(void) __asm__ ("Init_CRT") ;
void Init_CRT(void)
	{
	BYTE8 *bss ;

#ifdef	ROM_BASE
	BYTE8 *ram, *rom ;

	/* Copy program code from ROM to RAM ... */
	rom = ROM_BASE ;
	for (ram = &text_frst; ram <= &text_last; ram++, rom++)
		{
		*ram = *rom ;
		}

	/* Copy initialized data from ROM to RAM ... */
	rom = ROM_BASE + (&text_frst - &data_frst) ;
	for (ram = &text_frst; ram <= &text_last; ram++, rom++)
		{
		*ram = *rom ;
		}
#endif

	/* Initialize bss to all zeroes. */
	for (bss = &bss_frst; bss <= &bss_last; bss++)
		{
		*bss = 0 ;
		}
	}

/* ------------------------------------------------------------ */
/* Determine amount of physical memory installed in this PC.	*/
/* *** NOTE: This is a destructive read/write memory test!	*/
/* ------------------------------------------------------------ */
void *LastMemoryAddress(void)
	{
	static DWORD32 top = 0 ;
	DWORD32 bit ;

	if (!top)
		{
		for (bit = 0x80000000; bit != 0; bit >>= 1)
			{
			top |= bit ;
			if (!IsReadWrite(top)) top &= ~bit ;
			}
		}

	return (void *) top ;
	}

/* ------------------------------------------------------------ */
/* Private functions ...					*/
/* ------------------------------------------------------------ */
PRIVATE BOOL IsReadWrite(DWORD32 top)
	{
	BYTE8 *r, *w ;

	/* 640k - 1MB is NOT usable memory on IBM-PC */
	if (0xA0000 <= top && top <= 0XFFFFF)
		{
		return FALSE ;
		}

	/* Don't modify stack contents! */
	if ((BYTE8 *) top <= &stack_last) return TRUE ;

	w = r = (BYTE8 *) top ;
	*w = 0xAA ;
	if (*r != 0xAA) return FALSE ;
	*w = 0x55 ;
	return *r == 0x55 ;
	}

