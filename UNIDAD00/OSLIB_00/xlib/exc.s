/* Project:     OSLib
 * Description: The OS Construction Kit
 * Date:                1.6.2000
 * Idea by:             Luca Abeni & Gerardo Lamastra
 *
 * OSLin is an SO project aimed at developing a common, easy-to-use
 * low-level infrastructure for developing OS kernels and Embedded
 * Applications; it partially derives from the HARTIK project but it
 * currently is independently developed.
 *
 * OSLib is distributed under GPL License, and some of its code has
 * been derived from the Linux kernel source; also some important
 * ideas come from studying the DJGPP go32 extender.
 *
 * We acknowledge the Linux Community, Free Software Foundation,
 * D.J. Delorie and all the other developers who believe in the
 * freedom of software and ideas.
 *
 * For legalese, check out the included GPL license.
 */

/*	Exc/IRQ handlers (asm part)	*/
/* TODO: Unify the Exc/Int Mechanism... */

#include <ll/i386/sel.h>
#include <ll/i386/linkage.h>
#include <ll/i386/defs.h>

#include <ll/sys/ll/exc.h>

.extern SYMBOL_NAME(GDT_base)
.data          

ASMFILE(Exc)

.globl SYMBOL_NAME(ll_irq_table)
.globl SYMBOL_NAME(ll_exc_table)
		
SYMBOL_NAME_LABEL(ll_irq_table)	.space  64, 0
SYMBOL_NAME_LABEL(ll_exc_table)	.space  64, 65

.text

.extern  SYMBOL_NAME(ll_exc_hook)
.extern  SYMBOL_NAME(ll_FPU_hook)

		.globl SYMBOL_NAME(h01)
		.globl SYMBOL_NAME(h02)
		.globl SYMBOL_NAME(h03)
		.globl SYMBOL_NAME(h04)
		.globl SYMBOL_NAME(h05)
		.globl SYMBOL_NAME(h06)
		.globl SYMBOL_NAME(h07)
		.globl SYMBOL_NAME(h08)
		.globl SYMBOL_NAME(h09)
		.globl SYMBOL_NAME(h10)
		.globl SYMBOL_NAME(h11)
		.globl SYMBOL_NAME(h12)
		.globl SYMBOL_NAME(h13)
		.globl SYMBOL_NAME(h13_bis)
		.globl SYMBOL_NAME(h14)
		.globl SYMBOL_NAME(h15)
		.globl SYMBOL_NAME(exc0)
		.globl SYMBOL_NAME(exc1)
		.globl SYMBOL_NAME(exc2)
		.globl SYMBOL_NAME(exc3)
		.globl SYMBOL_NAME(exc4)
		.globl SYMBOL_NAME(exc5)
		.globl SYMBOL_NAME(exc6)
		.globl SYMBOL_NAME(exc7)
		.globl SYMBOL_NAME(exc8)
		.globl SYMBOL_NAME(exc9)
		.globl SYMBOL_NAME(exc10)
		.globl SYMBOL_NAME(exc11)
		.globl SYMBOL_NAME(exc12)
		.globl SYMBOL_NAME(exc13)
		.globl SYMBOL_NAME(exc14)
		.globl SYMBOL_NAME(exc15)
		.globl SYMBOL_NAME(exc16)

/* These are the hardware handlers; they all jump to ll_handler setting */
/* the interrupt number into EAX & save the registers on stack		*/

SYMBOL_NAME_LABEL(h01)	pushal
			movl    $1,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h02)	pushal
			movl    $2,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h03)	pushal
			movl    $3,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h04)  pushal
			movl    $4,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h05)	pushal
			movl    $5,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h06)	pushal
			movl    $6,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h07)	pushal
			movl    $7,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h08)	pushal
			movl    $8,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h09)	pushal
			movl    $9,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h10)	pushal
			movl    $10,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h11)	pushal
			movl    $11,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h12)	pushal
			movl    $12,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h13)	pushal
			movl    $13,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h13_bis)	pushal	
			/* Send EOI for 8086 coprocessor trick	*/
			/* Send 0H on Coprocessor port 0F0H	*/
			
			xorb    %al,%al		
			outb    %al,$0x0F0
			movb    $0x020,%al
			outb    %al,$0x0A0
			outb    %al,$0x020
			pushl   %ds
			pushl   %ss
			pushl   %es
			pushl   %fs
			pushl   %gs
			movw	$(X_FLATDATA_SEL),%ax
			movw	%ax,%es
			movw	%ax,%ds
			movl    $(MATH_EXC),%eax
			pushl	%eax
		/* LL_ABORT OR EXC_HOOK?? */
/*THIS SURELY WRONG!!!! FIX IT!!!! Gerardo, help me!!! */
			hlt
/*			call    SYMBOL_NAME(ll_abort) */
			addl    $4,%esp
			popl    %gs
			popl    %fs
			popl    %es
			popl    %ss
			popl    %ds
			popal
			ret

SYMBOL_NAME_LABEL(h14)	pushal
			movl    $14,%eax
			jmp     ll_handler

SYMBOL_NAME_LABEL(h15)	pushal
			movl    $15,%eax
			jmp     ll_handler


/* The ll_handler process the request using the kernel function act_int() */
/* Then sends EOI & schedules any eventual new task!			  */

ll_handler:             
			/* We do not know what is the DS value	*/
			/* Then we save it & set it correctly  	*/
			
			pushl	%ds
			pushl	%ss
			pushl	%es
			pushl	%fs
			pushl	%gs              
			/* But we first transfer to the _act_int  */
			/* the interrupt number which is required */
			/* as second argument			  */
			pushl   %eax
			movw    $(X_FLATDATA_SEL),%ax
			movw    %ax,%es
			mov     %ax,%ds
			
			/* Now save the actual context on stack		*/
			/* to pass it to _act_int (C caling convention)	*/
			/* CLD is necessary when calling a C function	*/
		
#if 0
			strw	%ax
			pushl	%eax
#endif
			cld
/* DIRTY!!! */
popl %eax
xorl %ebx, %ebx
movw %ds, %bx
/* We must switch to a ``safe stack'' */
/*
 OK, this is the idea: in %esp we have the address of the stack pointer
in the APPLICATION address space... We assume that address spaces are
implemented through segments... What we have to do is to add the
segment base to %esp:
- Load the GDT base in a register
- Add DS * 8 to that value
- Read the corresponding GDT entry (the segment descriptor)
- Compute the base... It is (*p & 0xFC) | (*(p +1)  & 0x0F) << 16) | *(p + 2)
*/
/* We must switch to a ``safe stack'' */
movl SYMBOL_NAME(GDT_base), %edi
addl %ebx, %edi
xorl %ebx, %ebx
movb 7(%edi), %bh
movb 4(%edi), %bl
shl $16, %ebx
movw 2(%edi), %bx
addl %ebx, %esp			/* Is it correct? I think so... Test it!!! */
movw %ds, %bx
movw %bx, %ss
pushl %eax



			movl	SYMBOL_NAME(ll_irq_table)(, %eax, 4), %ebx
			call    *%ebx
/*			call    SYMBOL_NAME(act_int)*/
			
			/* AX now contains the new context value!	*/

#if 0
			addl	$4,%esp /* Drop old context value 	*/
#endif
			popl	%ebx    /* Store in EBX the Int number	*/
			
			/* Send EOI to master & slave (if necessary) PIC */
			
#if 0
			pushl	%eax
#endif
			movb	$0x20,%al
			cmpl	$0x08,%ebx
			jb	eoi_master
eoi_slave:              movl	$0xA0,%edx
			outb	%al,%dx
eoi_master:             movl	$0x20,%edx
			outb	%al,%dx
			
			/* Resume the return value of _act_int 		*/
			/* & do the context switch if necessary!	*/
#ifdef __VIRCSW__
			movw SYMBOL_NAME(currCtx), %ax
			cmpw    JmpSel,%ax
			je      NoPreempt3
			movw    %ax,JmpSel
			ljmp    JmpZone 
#endif			
NoPreempt3:             popl	%gs
			popl	%fs
			popl	%es
			popl	%ss
			popl	%ds
			popal
			iret
				


SYMBOL_NAME_LABEL(exc0)	
			movl	$0,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc1)	
			movl	$1,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc2)	
			movl	$2,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc3)	
			movl	$3,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc4)
			movl	$4,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc5) 
			movl	$5,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc6)	
			movl	$6,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc7)	pushal
            		pushl	%ds
			pushl	%ss
			pushl	%es
			pushl	%fs
			pushl	%gs
			movw	$(X_FLATDATA_SEL),%ax
			movw	%ax,%es
			movw	%ax,%ds
			cld
			call	SYMBOL_NAME(ll_FPU_hook)
			popl	%gs
			popl	%fs
			popl	%es
			popl	%ss
			popl	%ds
			popal
			iret
			
SYMBOL_NAME_LABEL(exc8)	
			movl	$8,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc9)
			movl	$9,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc10) 
			movl	$10,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc11) 
			movl	$11,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc12) 
			movl	$12,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc13) 
			movl	$13,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc14) 
			movl	$14,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc15) 
			movl	$15,%eax
			jmp	ll_handler2
			
SYMBOL_NAME_LABEL(exc16) 
			movl	$16,%eax
			jmp	ll_handler2

ll_handler2:		
			pushl	%eax
			movw	$(X_FLATDATA_SEL),%ax
			movw	%ax,%ds
			movw	%ax,%es
/*ARGG!!!! Very Very Dirty!!! */
/*
    If the Exception raised in a different address space, we have to
    access the task stack from the kernel address space (the linear one...)
*/
popl %eax
movw %ss, %bx
/* We must switch to a ``safe stack'' */
movl SYMBOL_NAME(GDT_base), %edi
addl %ebx, %edi
xorl %ebx, %ebx
movb 7(%edi), %bh
movb 4(%edi), %bl
shl $16, %ebx
movw 2(%edi), %bx
addl %ebx, %esp			/* Is it correct? I think so... Test it!!! */
movw %ds, %bx
movw %bx, %ss
pushl %eax
/*
			movl	SYMBOL_NAME(ll_exc_table)(, %eax, 4), %ebx

			call	*%ebx
*/
			call	*SYMBOL_NAME(ll_exc_table)(, %eax, 4)
/*			call	SYMBOL_NAME(ll_exc_hook)*/
			addl	$4,%esp
			iret
