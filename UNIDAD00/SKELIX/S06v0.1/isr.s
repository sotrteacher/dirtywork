# Skelix by Xiaoming Mo (skelixos@gmail.com)
# Licence: GPLv2
		.text
		.include "kernel.inc"
		.globl	default_isr, isr, task1_run, task2_run
task1_run:
		call	do_task1
		jmp		task1_run
task2_run:
		call	do_task2
		jmp		task2_run

		.macro	isrnoerror		nr
		isr\nr:
		pushl	$0
		pushl	$\nr
		jmp		isr_comm
		.endm

		.macro	isrerror		nr
		isr\nr:
		pushl	$\nr
		jmp		isr_comm
		.endm
		
isr:	.long	divide_error, isr0x00, debug_exception, isr0x01
		.long	breakpoint, isr0x02, nmi, isr0x03
		.long	overflow, isr0x04, bounds_check, isr0x05
		.long	invalid_opcode, isr0x06, cop_not_avalid, isr0x07
		.long	double_fault, isr0x08, overrun, isr0x09
		.long	invalid_tss, isr0x0a, seg_not_present, isr0x0b
		.long	stack_exception, isr0x0c, general_protection, isr0x0d
		.long	page_fault, isr0x0e, reversed, isr0x0f
		.long	coprocessor_error, isr0x10, reversed, isr0x11
		.long	reversed, isr0x12, reversed, isr0x13
		.long	reversed, isr0x14, reversed, isr0x15
		.long	reversed, isr0x16, reversed, isr0x17
		.long	reversed, isr0x18, reversed, isr0x19
		.long	reversed, isr0x1a, reversed, isr0x1b
		.long	reversed, isr0x1c, reversed, isr0x1d
		.long	reversed, isr0x1e, reversed, isr0x1f
		.long	do_timer, isr0x20, do_kb, isr0x21
/*
		+-----------+
		|  old  ss  |	76
		+-----------+
		|  old esp  |	72
		+-----------+
		|  eflags   |	68
		+-----------+
		|    cs     |	64
		+-----------+
		|   eip     |	60
		+-----------+
		|  0/err    |	56
		+-----------+
		|  isr_nr   | tmp = esp
		+-----------+
		|   eax     |	48
		+-----------+
		|   ecx     |	44
		+-----------+
		|   edx     |	40
		+-----------+
		|   ebx     |	36
		+-----------+
		|   tmp     |	32
		+-----------+
		|   ebp     |	28
		+-----------+
		|   esi     |	24
		+-----------+
		|   edi     |	20
		+-----------+
		|    ds     |	16
		+-----------+
		|    es     |	12
		+-----------+
		|    fs     |	8
		+-----------+
		|    gs     |	4
		+-----------+
		|    ss     |	0
		+-----------+
*/
isr_comm:
		pushal
		pushl	%ds
		pushl	%es
		pushl	%fs
		pushl	%gs
		pushl	%ss
		movw	$DATA_SEL,%ax
		movw	%ax,	%ds
		movw	%ax,	%es
		movw	%ax,	%fs
		movw	%ax,	%gs
		movl	52(%esp),%ecx
		call	*isr(, %ecx, 8)
		addl	$4,		%esp	# for %ss
		popl	%gs
		popl	%fs
		popl	%es
		popl	%ds
		popal
		addl	$8,		%esp	# for isr_nr and err_code
		iret

		isrNoError		0x00
		isrNoError		0x01
		isrNoError		0x02
		isrNoError		0x03
		isrNoError		0x04
		isrNoError		0x05
		isrNoError		0x06
		isrNoError		0x07
		isrError		0x08
		isrNoError		0x09
		isrError		0x0a
		isrError		0x0b
		isrError		0x0c
		isrError		0x0d
		isrError		0x0e
		isrNoError		0x0f
		isrError		0x10
		isrNoError		0x11
		isrNoError		0x12
		isrNoError		0x13
		isrNoError		0x14
		isrNoError		0x15
		isrNoError		0x16
		isrNoError		0x17
		isrNoError		0x18
		isrNoError		0x19
		isrNoError		0x1a
		isrNoError		0x1b
		isrNoError		0x1c
		isrNoError		0x1d
		isrNoError		0x1e
		isrNoError		0x1f
		isrNoError		0x20
		isrNoError		0x21
default_isr:
		incb	0xb8000
		movb	$2,		0xb8001
		movb	$0x20,	%al
		outb	%al,	$0x20
		outb	%al,	$0xa0
		iret
		
