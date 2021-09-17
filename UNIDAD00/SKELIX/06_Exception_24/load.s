# Skelix by Xiaoming Mo (skelixos@gmail.com)
# Licence: GPLv2
MIDT_SEL	= 0x28       # Descriptor de segmento creado en la GDT de bootsect.s
		.text
		.globl	pm_mode
		.globl	idt_loc
		.globl	my_idt_entry
		.globl	write_byte_at_idt
		.include "kernel.inc"
		.org 0
pm_mode:
		movl	$DATA_SEL,%eax
		movw	%ax,	%ds
		movw	%ax,	%es
		movw	%ax,	%fs
		movw	%ax,	%gs
		movw	%ax,	%ss
		movl	$STACK_BOT,%esp

		cld
		movl	$0x10200,%esi
		movl	$0x200,	%edi
		movl	$(KERNEL_SECT-1)<<7,%ecx
		rep
		movsl

		call	init

#if 1 //LMC 2021.09.16
#
# void write_byte_at_idt(char *)
# @param 8(%ebp) direcci\'on del byte a escribir
#
write_byte_at_idt:
		pushl	%ebp
		movl	%esp, %ebp
		push	%gs
		pushl	%ebx
		pushl	%eax
		mov	$MIDT_SEL, %ebx
		mov	%bx, %gs
#dumm_loop:	jmp 	dumm_loop
		#Poner en %al el byte a escribir
		movl	8(%ebp), %ebx
		movb	(%ebx), %al
		movl	idt_loc, %ebx
		movb	%al, %gs:(%ebx)
		incl	idt_loc
		popl	%eax
		popl	%ebx
		movl	%ebp, %esp
		popl	%ebp
		ret

.data
idt_loc:.long 0
my_idt_entry:.quad 0
#endif
