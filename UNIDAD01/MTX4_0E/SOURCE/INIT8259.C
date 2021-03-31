/* ============================================================ */
/* File: INIT8259.C						*/
/*								*/
/* Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall	*/
/*								*/
/* Called by INIT-CPU.ASM.					*/
/*								*/
/* Purpose: Initializes 8259 Programmable Interrupt Controller.	*/
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

#define	PIC1_BASE_VCTR	IRQ2INT(0)
#define	PIC2_BASE_VCTR	IRQ2INT(8)

/* ------------------------------------------------------------	*/
/* 8259 Command and Data ports ...				*/
/* ------------------------------------------------------------	*/
#define	PIC1_CMND	0x20
#define	PIC1_DATA	0x21
#define	PIC2_CMND	0xA0
#define	PIC2_DATA	0xA1

/* ------------------------------------------------------------	*/
/* Bit fields in 1st Control Word (ICW1)			*/
/* ------------------------------------------------------------	*/
#define	ICW1_USE_ICW4	0x01	/* 0/1 => ICW4 no/yes		*/
#define	ICW1_SINGLE	0x02	/* 0/1 => cascaded/single PICs	*/
#define	ICW1_INTERVAL4	0x04	/* 0/1 => MOD 8/4 vectors	*/
#define	ICW1_LEVEL	0x08	/* 0/1 => Edge/Level triggered	*/
#define	ICW1_INIT	0x10	/* 1 => Initialization		*/
#define	ICW1_BASE_VCTR	0xE0	/* Bit field (not value)	*/

/* ------------------------------------------------------------	*/
/* Bit fields in 2nd Control Word (ICW2)			*/
/* ------------------------------------------------------------	*/
#define	ICW2_BASE_VCTR	0xFF	/* Bit field (not value)	*/

/* ------------------------------------------------------------	*/
/* Bit fields in 3rd Control Word (ICW3)			*/
/* ------------------------------------------------------------	*/
#define	ICW3_SLAVE_ID	0x07	/* Bit field (not value)	*/
#define	ICW3_IR7_SLAVE	0x80
#define	ICW3_IR6_SLAVE	0x40
#define	ICW3_IR5_SLAVE	0x20
#define	ICW3_IR4_SLAVE	0x10
#define	ICW3_IR3_SLAVE	0x08
#define	ICW3_IR2_SLAVE	0x04
#define	ICW3_IR1_SLAVE	0x02
#define	ICW3_IR0_SLAVE	0x01

/* ------------------------------------------------------------	*/
/* Bit fields in 4th Control Word (ICW4)			*/
/* ------------------------------------------------------------	*/
#define	ICW4_8086	0x01	/* 8086/88 (MCS-80/85) mode	*/
#define	ICW4_AUTO	0x02	/* Auto (normal) EOI		*/
#define	ICW4_BUF_SLAVE	0x08	/* Buffered mode/slave		*/
#define	ICW4_BUF_MASTER	0x0C	/* Buffered mode/master		*/
#define	ICW4_SFNM	0x10	/* Special fully nested (not)	*/

#define	PIC1_ICW1	(ICW1_INIT | ICW1_USE_ICW4)
#define	PIC1_ICW2	PIC1_BASE_VCTR
#define	PIC1_ICW3	ICW3_IR2_SLAVE
#define	PIC1_ICW4	ICW4_8086

#define	PIC2_ICW1	(ICW1_INIT | ICW1_USE_ICW4)
#define	PIC2_ICW2	PIC2_BASE_VCTR
#define	PIC2_ICW3	2 /* PIC2 Slave ID # */
#define	PIC2_ICW4	ICW4_8086

#define	PIC1_OCW1	0xFB	/* Mask interrupts except PIC2	*/
#define	PIC2_OCW1	0xFF	/* Mask off all interrupts	*/

/* Hide this function's name from C programs! */
void Init8259(void) __asm__ ("Init8259") ;
void Init8259(void)
	{
	PUSHF ;
	CLI ;

	outportb(PIC1_CMND, PIC1_ICW1) ;
	outportb(PIC2_CMND, PIC2_ICW1) ;

	/* Position hardware vectors just after Intel vectors */
	outportb(PIC1_DATA, PIC1_ICW2) ;
	outportb(PIC2_DATA, PIC2_ICW2) ;

	/* Master: 20h/21h (ch 2), Slave: A0h/A1h (ID #2) */
	outportb(PIC1_DATA, PIC1_ICW3) ;
	outportb(PIC2_DATA, PIC2_ICW3) ;

	/* 8086 mode (use vectors) */
	outportb(PIC1_DATA, PIC1_ICW4) ;
	outportb(PIC2_DATA, PIC2_ICW4) ;

	/* mask all interrupts */
	outportb(PIC1_DATA, PIC1_OCW1) ;
	outportb(PIC2_DATA, PIC2_OCW1) ;

	POPF ;
	}

