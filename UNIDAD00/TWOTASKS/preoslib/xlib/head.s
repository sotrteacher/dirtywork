/*
 *  head.s contains the 32-bit startup code.
 *  Two L3 task multitasking. The code of tasks are in kernel area, 
 *  just like the Linux. The kernel code is located at 0x10000. 
 */
#include <ll/i386/linkage.h>
#include <ll/i386/defs.h>
#include <ll/i386/mb-hdr.h>

#ifdef __LINUX__ /* ELF mode */
#define MULTIBOOT_FLAGS (MULTIBOOT_MEMORY_INFO)
#else /* else it is COFF! */
#define MULTIBOOT_FLAGS (MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE)
#endif

ASMFILE(HEAD)

KRN_BASE 	= 0x10000
/*KRN_BASE 	= 0x0*/
TSS0_SEL	= 0x20
LDT0_SEL	= 0x28
TSS1_SEL	= 0X30
LDT1_SEL	= 0x38
#if 1 //LMC 2021.08.18
X_FLATDATA_SEL  = 0x50
X_FLATCODE_SEL  = 0x58
#endif

#if 1 //LMC 2021.08.17
.extern SYMBOL_NAME(_stkbase)
.extern SYMBOL_NAME(_stktop)
.extern SYMBOL_NAME(halt)
.data
current:.long 0
scr_loc:.long 0

.align 2
.word 0
#if 1 //LMC 2021.08.18
IDT_descr:
#endif
lidt_opcode:
	.word 256*8-1		# idt contains 256 entries
	.long idt + KRN_BASE	# This will be rewrite by code. 
.align 2
.word 0
#if 1 //LMC 2021.08.18
GDT_descr:
#endif
lgdt_opcode:
	.word (end_gdt-gdt)-1	# so does gdt 
#if 1 //LMC 2021.08.18
SYMBOL_NAME_LABEL(GDT_base) 
#endif
	.long gdt + KRN_BASE	# This will be rewrite by code.

        #if 0 //LMC 2021.08.16
	.align 3
        #else
	.align 8
        #endif
idt:	.fill 256,8,0		# idt is uninitialized

gdt:	.quad 0x0000000000000000	/* NULL descriptor */
	.quad 0x00c09a01000007ff	/* 8Mb 0x08, base = 0x10000 */
	.quad 0x00c09201000007ff	/* 8Mb 0x10 */
	.quad 0x00c0920b80000002	/* screen 0x18 - for display */

	.quad 0x0000e90100000068	# TSS0 descr 0x20
	.quad 0x0000e20100000040	# LDT0 descr 0x28
	.quad 0x0000e90100000068	# TSS1 descr 0x30
	.quad 0x0000e20100000040	# LDT1 descr 0x38
#if 1 //LMC 2021.08.18
			/* X_RM_BACK_GATE *//** 0x40 */
rmBckGateFix1:		/* X_FLATCODE_SEL */
.word	0
.word	X_FLATCODE_SEL	
.word	0x8C00
rmBckGateFix2:
.word	0

.word	0,0,0,0		/* X_PM_BACK_GATE *//** 0x48 LMC*/
.word	0xFFFF		/* X_FLATDATA_SEL *//** 0x50 */ 
.word	0
.word	0x9200
.word	0x00CF
.word	0xFFFF		/* X_FLATCODE_SEL *//** 0x58 LMC*/ 
.word	0
.word	0x9A00
.word	0x00CF
.word	0,0,0,0		/* X_CALLBIOS_SEL *//** 0x60 LMC*/
.word	0,0,0,0		/* X_CALLBIOS_GATE *//** 0x68 */
.word	0,0,0,0		/* X_VM86_TSS */    /** 0x70 */
.word	0,0,0,0		/* X_MAIN_TSS */    /** 0x78 */
#endif
end_gdt:
	.fill 128,4,0
#if 1 //LMC 2021.08.18
base:
.space 	8192,0
tos:
#endif
stack_ptr:
	.long stack_ptr
	.word 0x10

/*************************************/
#if 0 //LMC 2021.08.16
.align 3
#else
.align 8
#endif
ldt0:	.quad 0x0000000000000000
	.quad 0x00c0fa01000003ff	# 0x0f, base = 0x10000
	.quad 0x00c0f201000003ff	# 0x17
tss0:
	.long 0 			/* back link */
	.long stack0_krn_ptr, 0x10	/* esp0, ss0 */
	.long 0, 0			/* esp1, ss1 */
	.long 0, 0			/* esp2, ss2 */
	.long 0				/* cr3 */
	.long task0			/* eip */
	.long 0x200			/* eflags */
	.long 0, 0, 0, 0		/* eax, ecx, edx, ebx */
	.long stack0_ptr, 0, 0, 0	/* esp, ebp, esi, edi */
	.long 0x17,0x0f,0x17,0x17,0x17,0x17 /* es, cs, ss, ds, fs, gs */
	.long LDT0_SEL			/* ldt */
	.long 0x8000000			/* trace bitmap */

	.fill 128,4,0
stack0_krn_ptr:
	.long 0

/************************************/
#if 0 //LMC 2021.08.16
.align 3
#else
.align 8
#endif
ldt1:	.quad 0x0000000000000000
	.quad 0x00c0fa01000003ff	# 0x0f, base = 0x10000
	.quad 0x00c0f201000003ff	# 0x17
tss1:
	.long 0 			/* back link */
	.long stack1_krn_ptr, 0x10	/* esp0, ss0 */
	.long 0, 0			/* esp1, ss1 */
	.long 0, 0			/* esp2, ss2 */
	.long 0				/* cr3 */
	.long task1			/* eip */
	.long 0x200			/* eflags */
	.long 0, 0, 0, 0		/* eax, ecx, edx, ebx */
	.long stack1_ptr, 0, 0, 0	/* esp, ebp, esi, edi */
	.long 0x17,0x0f,0x17,0x17,0x17,0x17 /* es, cs, ss, ds, fs, gs */
	.long LDT1_SEL			/* ldt */
	.long 0x8000000			/* trace bitmap */

	.fill 128,4,0
stack1_krn_ptr:
	.long 0


#endif

.text
#if 1 //LMC 2021.08.16
.globl	SYMBOL_NAME(_start)
.globl start

SYMBOL_NAME_LABEL(_start)
start:
#endif
#if 1 //LMC 2021.08.17  Kernel Header
.align		8
/*start:*/
		jmp	boot_entry		
		/* 
		Here we go with the multiboot header... 
		---------------------------------------
		*/		
.align		8
boot_hdr:
.align		8
		.long	MULTIBOOT_MAGIC
		.long   MULTIBOOT_FLAGS
		/* Checksum */
		.long	-(MULTIBOOT_MAGIC+MULTIBOOT_FLAGS)

boot_entry:	/* Just a brief debug check */	
		#ifdef __DEBUG__
		/* A Brown 1 should appear... */
		    movl	$0xB8000,%edi
		    addl	$158,%edi
		    movb	$'1',%gs:0(%edi)
		    incl	%edi
		    movb	$6,%gs:0(%edi)
		#endif
		/* 
		 * Hopefully if it gets here, CS & DS are
		 * Correctly set for FLAT LINEAR mode
		 */
		/* Test if GDT is usable */
		movl	%gs:0(%ebx),%ecx
		andl	$0x080,%ecx
                #if 1
		jnz	GDT_is_OK
                #else
                jnz     startup_32 
                #endif
		/* 
		 * Fix the X_RM_BACK_GATE with the address of halt()
		*/

/* Now I test if the check mechanism is OK... */
movl	$0xB8000,%edi
addl	$150,%edi
movb	$'1',%gs:0(%edi)
incl	%edi
movb	$6,%gs:0(%edi)
incl	%edi
movb	$'2',%gs:0(%edi)
incl	%edi
movb	$6,%gs:0(%edi)

		movl	$SYMBOL_NAME(halt),%eax
		movw	%ax,%gs:rmBckGateFix1
		shrl	$16,%eax
		movw	%ax,%gs:rmBckGateFix2
		
		/* Load GDT, using the predefined assignment! */
		lgdt	GDT_descr
movl	$0xB8000,%edi
addl	$146,%edi
movb	$'0',%gs:0(%edi)
incl	%edi
movb	$6,%gs:0(%edi)
		
GDT_is_OK:	movw	$(X_FLATDATA_SEL),%ax
		movw	%ax,%ds
		movw	%ax,%es
		movw	%ax,%ss
		movw	%ax,%fs
		movw	%ax,%gs
		movl	$tos,%esp
		movl	$base,SYMBOL_NAME(_stkbase)
		movl	$tos,SYMBOL_NAME(_stktop)
		
#if 1
		/* Store the MULTIBOOT informations */
		movl	%eax,SYMBOL_NAME(mb_signature)
		movl	%ebx,SYMBOL_NAME(mbi)
		
		/* Store the X passed GDT address!
		 * If GDT is not available is a dummy instruction!
		 */
		sgdt 	GDT_descr

		/* Now probably is the case to load CS... */
		ljmp $X_FLATCODE_SEL, $load_cs

load_cs:
		/* Load IDT */
		lidt	IDT_descr
		
		cld
                #if 0
		call SYMBOL_NAME(_startup)
                #else
                call SYMBOL_NAME(startup_32)
                #endif
looplabel:	
		jmp looplabel

#endif
		
#endif
startup_32:
	movl $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
	#movw %ax,%ds
	#movw %ax,%es
	#movw %ax,%fs
	#movw %ax,%gs
	lss stack_ptr,%esp

# setup base fields of descriptors.
	movl $KRN_BASE, %ebx
	movl $gdt, %ecx
	lea tss0, %eax
	movl $TSS0_SEL, %edi	
	call set_base
	lea ldt0, %eax
	movl $LDT0_SEL, %edi
	call set_base
	lea tss1, %eax
	movl $TSS1_SEL, %edi 
	call set_base
	lea ldt1, %eax
	movl $LDT1_SEL, %edi
	call set_base

	call setup_idt
	call setup_gdt
	movl $0x10,%eax		# reload all the segment registers
	mov %ax,%ds		# after changing gdt. 
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
	#movw %ax,%ds		# after changing gdt. 
	#movw %ax,%es
	#movw %ax,%fs
	#movw %ax,%gs
	lss stack_ptr,%esp

# setup up timer 8253 chip.
	#movw %ax,%ds		# after changing gdt. 
	movb $0x36, %al
	movl $0x43, %edx
	outb %al, %dx
	movl $11930, %eax        # timer frequency 100 HZ 
	movl $0x40, %edx
	outb %al, %dx
	movb %ah, %al
	outb %al, %dx

# setup timer & system call interrupt descriptors.
	movl $0x00080000, %eax	
debuglabel0:
        #if 0 //LMC 2021.08.18: In function `debuglabel0': relocation truncated to fit: R_386_16 against `.text'
	movw $timer_interrupt, %ax
        #else
	movl $timer_interrupt, %eax
        #endif
	movw $0x8E00, %dx
	movl $0x20, %ecx
	lea idt(,%ecx,8), %esi
	movl %eax,(%esi) 
	movl %edx,4(%esi)
debuglabel1:
        #if 0 //LMC 2021.08.18: In function `debuglabel1': relocation truncated to fit: R_386_16 against `.text'
	movw $system_interrupt, %ax
        #else
	movl $timer_interrupt, %eax
        #endif
	movw $0xef00, %dx
	movl $0x80, %ecx
	lea idt(,%ecx,8), %esi
	movl %eax,(%esi) 
	movl %edx,4(%esi)

# unmask the timer interrupt.
	movl $0x21, %edx
	inb %dx, %al
	andb $0xfe, %al
	outb %al, %dx

# Move to user mode (task 0)
	pushfl
	andl $0xffffbfff, (%esp)
	popfl
	movl $TSS0_SEL, %eax
	ltr %ax
	movl $LDT0_SEL, %eax
	lldt %ax 
	movl $0, current
	sti
	pushl $0x17
	pushl $stack0_ptr
	pushfl
	pushl $0x0f
	pushl $task0
	iret

/****************************************/
setup_gdt:
	lgdt lgdt_opcode
	ret

setup_idt:
	lea ignore_int,%edx
	movl $0x00080000,%eax
	movw %dx,%ax		/* selector = 0x0008 = cs */
	movw $0x8E00,%dx	/* interrupt gate - dpl=0, present */
	lea idt,%edi
	mov $256,%ecx
rp_sidt:
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	dec %ecx
	jne rp_sidt
	lidt lidt_opcode
	ret

# in: %eax - logic addr; %ebx = base addr ; 
# %ecx - table addr; %edi - descriptors offset.
set_base:
	addl %ebx, %eax
	addl %ecx, %edi
	movw %ax, 2(%edi)
	rorl $16, %eax
	movb %al, 4(%edi)
	movb %ah, 7(%edi)
	rorl $16, %eax
	ret

write_char:
	push %gs
	pushl %ebx
	pushl %eax
	#mov $0x18, %ebx
	#mov %bx, %gs
	movl $0x18, %ebx
	movw %bx, %gs
        #if 0 //LMC 2021.08.16
	#movl scr_loc, %bx
        #else
	movl scr_loc, %ebx
        #endif
	shl $1, %ebx
	movb %al, %gs:(%ebx)
	shr $1, %ebx
	incl %ebx
	cmpl $2000, %ebx
	jb 1f
	movl $0, %ebx
1:	movl %ebx, scr_loc	
	popl %eax
	popl %ebx
	pop %gs
	ret

/***********************************************/
/* This is the default interrupt "handler" :-) */
#if 1 //LMC 2021.08.17
.align 2
#else
.align 8
#endif
ignore_int:
	push %ds
	pushl %eax
	movl $0x10, %eax
	#mov %ax, %ds
	movw %ax, %ds
	movl $67, %eax            /* print 'C' */
	call write_char
	popl %eax
	pop %ds
	iret

/* Timer interrupt handler */ 
.align 2
timer_interrupt:
	push %ds
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	movl $0x10, %eax
	#mov %ax, %ds
	movw %ax, %ds
	movb $0x20, %al
	outb %al, $0x20
	movl $1, %eax
	cmpl %eax, current
	je 1f
	movl %eax, current
	ljmp $TSS1_SEL, $0
	jmp 2f
1:	movl $0, current
	ljmp $TSS0_SEL, $0
2:	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	pop %ds
	iret

/* system call handler */
.align 2
system_interrupt:
	push %ds
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	movl $0x10, %edx
	#mov %dx, %ds
	movw %dx, %ds
	call write_char
	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	pop %ds
	iret

/*********************************************/
/************************************/
task0:
	movl $0x17, %eax
	movw %ax, %ds
        #if 0 //LMC 2021.08.16
	#movl $65, %al              /* print 'A' */
        #else
	movb $65, %al              /* print 'A' */
        #endif
	int $0x80
	movl $0xfff, %ecx
1:	loop 1b
	jmp task0 

	.fill 128,4,0
stack0_ptr:
	.long 0

task1:
	movl $0x17, %eax
	movw %ax, %ds
        #if 0 //LMC 2021.08.16
	#movl $66, %al              /* print 'B' */
        #else
	movb $66, %al              /* print 'B' */
        #endif
	int $0x80
	movl $0xfff, %ecx
1:	loop 1b
	jmp task1

	.fill 128,4,0 
stack1_ptr:
	.long 0

.data
/* MultiBoot Data Stuff definition */
SYMBOL_NAME_LABEL(mb_signature) .long 0
SYMBOL_NAME_LABEL(mbi) .long 0
/*** end ***/
