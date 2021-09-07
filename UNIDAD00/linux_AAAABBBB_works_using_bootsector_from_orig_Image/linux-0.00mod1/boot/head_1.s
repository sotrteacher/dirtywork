/*
 *  head.s contains the 32-bit startup code.
 *  Two L3 task multitasking. The code of tasks are in kernel area, 
 *  just like the Linux. The kernel code is located at 0x10000. 
 */
#if 1 //LMC 2021.08.29
.globl KRN_BASE
.globl TSS0_SEL
.globl TSS1_SEL
.globl startup_32
.globl stack_ptr
.globl gdt
.globl tss0
.globl set_base
.globl ldt0
.globl tss1
.globl ldt1
.globl setup_idt
.globl setup_gdt
.globl timer_interrupt_dumm
.globl timer_interrupt
.globl idt
.globl system_interrupt
.globl current
.globl stack0_ptr
.globl task0
.globl lgdt_opcode
.globl ignore_int_dumm
.globl ignore_int
.globl rp_sidt
.globl lidt_opcode
.globl write_char
.globl scr_loc
.globl end_gdt
.globl stack0_krn_ptr
.globl stack1_krn_ptr
.globl task1
.globl stack1_ptr
#endif
KRN_BASE 	= 0x10000
TSS0_SEL	= 0x20
LDT0_SEL	= 0x28
TSS1_SEL	= 0X30
LDT1_SEL	= 0x38
#if 1 //LMC 2021.08.25
.section .rawdata
.incbin "zmagic_header.raw"
#endif

.text
startup_32:
	movl $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
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
	lss stack_ptr,%esp

# setup up timer 8253 chip.
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
	movw $timer_interrupt, %ax
	movw $0x8E00, %dx
	movl $0x20, %ecx
	lea idt(,%ecx,8), %esi
	movl %eax,(%esi) 
	movl %edx,4(%esi)
	movw $system_interrupt, %ax
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
	mov $0x18, %ebx
        #if 0 //LMC 2021.08.28
	//mov %bx, %gs
        #else
        .incbin "mov_bx_gs.raw"
        #endif
	#if 0 //LMC 2021.08.26
	//movl scr_loc, %bx
	#else
	//movw scr_loc, %bx
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
.align 2
#if 1 //LMC 2021.08.29
//ignore_int_dumm:
//.incbin "nop_nop.raw"
//.align 4         /** If this is included */
//ignore_int_dumm: /** ignore_int_dumm = 0x0188, 384%4=0,384%8=0 */
//.incbin "nop.raw"/** and if this and */
.align 4           /** this are included */
#endif             /** ignore_int = 0x018c, 388%4=0,388%8>0*/
ignore_int:  /**With only one .align 4 we get ingnore_int = 0x188*/
	push %ds/** Therefore ignore_int must be multiple of 4 */
	pushl %eax
	movl $0x10, %eax
	mov %ax, %ds
	movl $67, %eax            /* print 'C' */
	call write_char
	popl %eax
	pop %ds
	iret

/* Timer interrupt handler */ 
.align 2
#if 1 //LMC 2021.08.29
timer_interrupt_dumm:
//.incbin "nop_nop.raw"
.align 4
#endif
timer_interrupt:
	push %ds
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	movl $0x10, %eax
	mov %ax, %ds
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
#if 1 //LMC 2021.08.29
//.incbin "nop_nop.raw"
.align 4
#endif
system_interrupt:
	push %ds
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	movl $0x10, %edx
	mov %dx, %ds
	call write_char
	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	pop %ds
	iret

/*********************************************/
#if 0 //LMC 2021.08.27
//scr_loc:.long 0
//current:.long 0
#else
//#if 1 //LMC 2021.08.29
//.incbin "nop_nop_nop_nop.raw"
//#endif
current:.long 0
scr_loc:.long 0
//scr_loc:.long 0
//current:.long 0
#endif

.align 2
.word 0
lidt_opcode:
	.word 256*8-1		# idt contains 256 entries
	.long idt + KRN_BASE	# This will be rewrite by code. 
.align 2
.word 0
lgdt_opcode:
	.word (end_gdt-gdt)-1	# so does gdt 
	.long gdt + KRN_BASE	# This will be rewrite by code.

	#if 0 //LMC 2021.08.26
	/*.align 3*/
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
end_gdt:
	.fill 128,4,0
stack_ptr:
	.long stack_ptr
	.word 0x10

/*************************************/
#if 0 //LMC 2021.08.26
/*.align 3*/
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
#if 0 //LMC 2021.08.26
/*.align 3*/
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

/************************************/
task0:
	movl $0x17, %eax
	movw %ax, %ds
	#if 0 //LMC 2021.08.26
	/*movl $65, %al*/              /* print 'A' */
	#else
	movb $65, %al
	#endif
	int $0x80
	movl $0xfff, %ecx
1:	loop 1b
	jmp task0 

#if 1 //LMC 2021.08.29
//.incbin "nop_nop_nop_nop.raw"
#endif
	.fill 128,4,0
stack0_ptr:
	.long 0

task1:
	movl $0x17, %eax
	movw %ax, %ds
	#if 0 //LMC 2021.08.26
	/*movl $66, %al*/              /* print 'B' */
	#else
	movb $66, %al
	#endif
	int $0x80
	movl $0xfff, %ecx
1:	loop 1b
	jmp task1

#if 1 //LMC 2021.08.29
//.incbin "nop_nop_nop_nop.raw"
#endif
	.fill 128,4,0 
stack1_ptr:
	.long 0

//#if 1 //LMC 2021.08.25
//.section .rawdata
//.incbin "zmagic_header.raw"
//#endif
/*** end ***/
