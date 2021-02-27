/* ============================================================ */
/* File: INIT-IDT.C						*/
/*								*/
/* Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall	*/
/*								*/
/* Called by INIT-CPU.ASM.					*/
/*								*/
/* Purpose: Interrupt system initialization. Establishes an	*/
/* Interrtupt Vector Table (IVT) for an initial set of		*/
/* Interrupt Service Routines (ISRs).				*/
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

#define	_HW_INT_BASE	(256-16)	/* Use last 16 vectors */

/* ------------------------------------------------------------ */
/* Format of an entry in the Interrupt Descriptor Table (IDT).	*/
/* Note: Not needed if you use SetISR (see below).		*/
/* ------------------------------------------------------------ */
typedef struct IDT_DESCRIPTOR
	{
	WORD16	offset_0 ;	/* Offset, bits 15-0	*/
	WORD16	selector ;	/* Selector value	*/
	BYTE8	not_used ;	/* Must be set to 0's	*/
	BYTE8	flags	 ;	/* Access rights, etc.	*/
	WORD16	offset_16;	/* Offset, bits 31-16	*/
	} __attribute__((packed)) IDT_DESCRIPTOR ;

/* ------------------------------------------------------------ */
/* Operand used by the LIDT instruction.			*/
/* Note: Not needed if you use SetISR (see below).		*/
/* ------------------------------------------------------------ */
typedef struct IDT_INFO
	{
	WORD16	limit ;
	DWORD32	addrs ;
	} __attribute__((packed)) IDT_INFO ;

extern WORD16 code_selector	__asm__("code_selector") ;

#define	INTEL_INT(n)	(0 <= n && n < 32)
#define	PIC1_INT(n)	(IRQ2INT(0) <= n && n <= IRQ2INT(7))
#define	PIC2_INT(n)	(IRQ2INT(8) <= n && n <= IRQ2INT(15))

#define	PRIVATE	static

PRIVATE void Abort(char *msg)
	{
	CLI ;
	PutString(msg) ;
	PutString("; system halted.") ;
	for (;;) ;
	}

PRIVATE void ISR00(void) { Abort("Divide error"); }
PRIVATE void ISR01(void) { Abort("Debug Interrupt"); }
PRIVATE void ISR02(void) { Abort("Non-Maskable Interrupt"); }
PRIVATE void ISR03(void) { Abort("Breakpoint Interrupt"); }
PRIVATE void ISR04(void) { Abort("Overflow"); }
PRIVATE void ISR05(void) { Abort("BOUND Range Exceeded"); }
PRIVATE void ISR06(void) { Abort("Invalid Opcode"); }
PRIVATE void ISR07(void) { Abort("No Math CoProcessor"); }
PRIVATE void ISR08(void) { Abort("Double Fault"); }
PRIVATE void ISR09(void) { Abort("Math CoProcessor Overrun"); }
PRIVATE void ISR10(void) { Abort("Invalid TSS"); }
PRIVATE void ISR11(void) { Abort("Segment Not Present"); }
PRIVATE void ISR12(void) { Abort("Stack Segment Fault"); }
PRIVATE void ISR13(void) { Abort("General Protection"); }
PRIVATE void ISR14(void) { Abort("Page Fault"); }
PRIVATE void ISR16(void) { Abort("Math CoProcessor Error"); }
PRIVATE void ISR17(void) { Abort("Alignment Check"); }
PRIVATE void ISR18(void) { Abort("Machine Check"); }
PRIVATE void ISR19(void) { Abort("SIMD floating-point exception"); }
PRIVATE void ISR_Rsvd(void) { Abort("Intel Reserved Interrupt"); }
PRIVATE void ISR_User(void) { Abort("Unassigned Interrupt"); }

PRIVATE ISR _intel_isr[] =
	{
	ISR00,		/* Divide by zero		*/
	ISR01,		/* Debug exception		*/
	ISR02,		/* NMI				*/
	ISR03,		/* One byte interrupt		*/
	ISR04,		/* Interrupt on overflow	*/
	ISR05,		/* Array bounds error		*/
	ISR06,		/* Invalid opcode		*/
	ISR07,		/* Math not available		*/
	ISR08,		/* Double fault			*/
	ISR09,		/* Math segment overflow	*/
	ISR10,		/* Invalid TSS			*/
	ISR11,		/* Segment not present		*/
	ISR12,		/* Stack fault			*/
	ISR13,		/* General protection		*/
	ISR14,		/* Page fault			*/
	ISR_Rsvd,	/* Reserved			*/
	ISR16,		/* Math error			*/
	ISR17,		/* Alignment Check		*/
	ISR18,		/* Machine Check		*/
	ISR19,		/* SIMD floating-point except.	*/
	ISR_Rsvd,	/* Reserved			*/
	ISR_Rsvd,	/* Reserved			*/
	ISR_Rsvd,	/* Reserved			*/
	ISR_Rsvd,	/* Reserved			*/
	ISR_Rsvd,	/* Reserved			*/
	ISR_Rsvd,	/* Reserved			*/
	ISR_Rsvd,	/* Reserved			*/
	ISR_Rsvd,	/* Reserved			*/
	ISR_Rsvd,	/* Reserved			*/
	ISR_Rsvd,	/* Reserved			*/
	ISR_Rsvd,	/* Reserved			*/
	ISR_Rsvd	/* Reserved			*/
	} ;

PRIVATE IDT_DESCRIPTOR idt[256] ;
PRIVATE IDT_INFO idt_info = { sizeof(idt) - 1, (DWORD32) idt } ;

int IRQ2INT(int irq)
	{
	return _HW_INT_BASE + irq ;
	}

ISR GetISR(int int_numb)
	{
	IDT_DESCRIPTOR *p = &idt[int_numb] ;
	DWORD32 offset ;
	
	offset = (((DWORD32) p->offset_16) << 16) + p->offset_0 ;
	return (ISR) offset ;
	}

void SetISR(int int_numb, ISR isr)
	{
	IDT_DESCRIPTOR *p = &idt[int_numb] ;

	p->offset_0	= ((WORD16 *) &isr)[0] ;
	p->offset_16	= ((WORD16 *) &isr)[1] ;
	}

/* Hide this function's name from C programs! */
void Init_IDT(void) __asm__ ("Init_IDT") ;
void Init_IDT(void)
	{
	extern void ISR_PIC1(void)	__asm("ISR_PIC1") ;
	extern void ISR_PIC2(void)	__asm("ISR_PIC2") ;
	IDT_DESCRIPTOR *p ;
	int i ;

	p = idt ;
	for (i = 0; i < ENTRIES(idt); i++, p++)
		{
		ISR isr ;

		p->selector	= code_selector ;
		p->not_used	= 0x00 ;
		p->flags	= 0x8E ;

		if (INTEL_INT(i))	isr = _intel_isr[i] ;
		else if (PIC1_INT(i))	isr = ISR_PIC1 ;
		else if (PIC2_INT(i))	isr = ISR_PIC2 ;
		else			isr = ISR_User ;

		p->offset_0	= ((WORD16 *) &isr)[0] ;
		p->offset_16	= ((WORD16 *) &isr)[1] ;
		}

	__asm__ __volatile__ ("lidt %0" : : "m" (idt_info)) ;
	}
