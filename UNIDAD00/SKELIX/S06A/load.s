# Skelix by Xiaoming Mo (skelixos@gmail.com)
# Licence: GPLv2
#if 1 //LMC 2021.09.17
KRN_BASE 	= 0x0000     # See cooments at bootsect.s
MKRD_SEL	= 0x10       # Descriptor de segmento creado en la GDT de bootsect.s
TSS_SEL		= 0x18       # after TSS_SEL in include/kernel.h
LDT_SEL		= 0x20       # after LDT_SEL in include/kernel.h
TSS1_SEL	= 0x28       # after TSS1_SEL in include/kernel.h
LDT1_SEL	= 0x30       # after LDT1_SEL in include/kernel.h
MIDT_SEL	= 0x38       # Descriptor de segmento creado en la GDT de bootsect.s
MGDT_SEL	= 0x40       # Descriptor de segmento creado en la GDT de bootsect.s
USER_CODE_SEL	= 0x0f	     # /* code selector in ldt  00000000 00001 1 11 = 0x0f */
USER_DATA_SEL	= 0x17	     # /* data and stack selector in ldt 00000000 00010 1 11 = 0x17 */
VIDEO_RAM	= 0x0b8000   # Starting address of video ram (after at include/scr.h)
#endif
		.text
		.globl	pm_mode
		.globl	idt_loc
		.globl	my_idt_entry
		.globl	write_byte_at_idt
		.globl	write_byte_at_gdt
		.globl	the_gdt_sel
		.globl	m_ltrw
		.globl	copy_data_byte
		.globl	go_to_user_mode
		.globl	move_to_user_mode
		.globl	task1_run
		.globl	task2_run
		.globl	tss0
		.globl	tss1
		.globl	ldt0
		.globl	ldt1
		.include "kernel.inc"
		.org 0
pm_mode:
		movl	$DATA_SEL,%eax
		movw	%ax,	%ds
		movw	%ax,	%es
		movw	%ax,	%fs
		movw	%ax,	%gs
		movw	%ax,	%ss
	#if 1 //LMC 2021.09.24
		movl	$STACK_BOT,%esp
	#else
		#lss	stack_ptr, %esp
	#endif
#dumm_loop0:	jmp 	dumm_loop0
	#if 1 //LMC 2021.09.24
		cld
		movl	$0x10200,%esi
		movl	$0x200,	%edi
		movl	$(KERNEL_SECT-1)<<7,%ecx
		rep
		movsl
	#else
		#movl	$KRN_BASE, %ebx
	#endif
#################WRITE CALLING TO setup_base HERE!#################
# setup base fields of descriptors (of descriptors in GDT).
		pushl	$TSS_SEL	# Selector of TSSD (of GDT) to edit
		pushl	$tss0		# Address of TSS 
		pushl	$0x080800 	# Address of GDT
		call	setup_base
		pushl	$LDT_SEL	# Selector of LDTD (of GDT) to edit
		pushl	$ldt0		# Address of ldt0 
		pushl	$0x080800 	# Address of GDT
		call	setup_base
		pushl	$TSS1_SEL	# Selector of TSSD (of GDT) to edit
		pushl	$tss1		# Address of TSS 
		pushl	$0x080800 	# Address of GDT
		call	setup_base
		pushl	$LDT1_SEL	# Selector of LDTD (of GDT) to edit
		pushl	$ldt1		# Address of ldt1 
		pushl	$0x080800 	# Address of GDT
		call	setup_base

###################################################################
	call setup_idt
	#call setup_gdt
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
#if 1 //LMC 2021.09.25
	movl $0x00080000, %eax		# %eax=0x00080000
#else
	#movl $0x00000000, %eax		# %eax=0x00000000
#endif
	movw $timer_interrupt, %ax	# %eax=0x00080000+$timer_interrupt
	movw $0x8E00, %dx		# %dx=0x8E00    (interrupt gate)
	movl $0x20, %ecx		# %ecx=0x20     (timer interrupt)
	#lea idt(,%ecx,8), %esi
	lea IDT_ADDR(,%ecx,8), %esi	# It must put 0x80000+(0x20)\times 8 at %esi
					#           = 0x80000+0x100 = 0x80100
	movl %eax,(%esi) 		# (0x80100) <-- 0x00080000+$timer_interrupt
#dumm_loop0: jmp dumm_loop0
	movl %edx,4(%esi)
	movw $system_interrupt, %ax
	movw $0xef00, %dx		# (trap gate)
	movl $0x80, %ecx		# (system interrupt)
	#lea idt(,%ecx,8), %esi
	lea IDT_ADDR(,%ecx,8), %esi
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
	#movl $TSS0_SEL, %eax
	movl $TSS_SEL, %eax
	ltr %ax
	#movl $LDT0_SEL, %eax
	movl $LDT_SEL, %eax
	lldt %ax 
	movl $0, current
	sti
	pushl $0x17
	pushl $stack0_ptr
	pushfl
	pushl $0x0f
	pushl $task0
	#pushl $task1_run
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
	#lea idt,%edi
	lea IDT_ADDR, %edi
	mov $256,%ecx
rp_sidt:
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	dec %ecx
	jne rp_sidt
	lidt lidt_opcode
	ret

write_char:
	push %gs
	pushl %ebx
	pushl %eax
   #if 0 //LMC 2021.09.24
	#mov $0x18, %ebx
   #else
	mov DATA_SEL, %ebx
   #endif
        mov %bx, %gs
	movl scr_loc, %ebx
    #if 1 //LMC 2021.09.24
	addl $VIDEO_RAM, %ebx
    #endif
	#shl $1, %ebx
	movb %al, %gs:(%ebx)
	#shr $1, %ebx
    #if 1 //LMC 2021.09.24
	subl $VIDEO_RAM, %ebx
    #endif
	incl %ebx
    #if 0 //LMC 2021.09.25    to avoid background pink 
	#incl %ebx
    #endif
    #if 0 //LMC 2021.09.26    to avoid background pink 
	#cmpl $2000, %ebx
    #else
	cmpl $4000, %ebx
    #endif
	jb 1f
	movl $0, %ebx
1:	movl %ebx, scr_loc	
	popl %eax
	popl %ebx
	pop %gs
	ret

/***********************************************/
/* This is the default interrupt "handler" :-) */
.align 4           
ignore_int: 
	push %ds
	pushl %eax
	movl $0x10, %eax
	mov %ax, %ds
	movl $67, %eax            /* print 'C' */
	call write_char
	popl %eax
	pop %ds
	iret

/* Timer interrupt handler */ 
.align 4
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
#dumm_loop0:jmp dumm_loop0
	ljmp $TSS1_SEL, $0
	jmp 2f
1:	movl $0, current
	#ljmp $TSS0_SEL, $0
	ljmp $TSS_SEL, $0
2:	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	pop %ds
	iret

/* system call handler */
.align 4
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
###################################################################
#		call	init
mistery_loop:	jmp mistery_loop


#if 1 //LMC 2021.09.16
#
# void write_byte_at_idt(char *)
# @param 8(%ebp) direcci\'on del byte a escribir
# Tal como est\'a escrita esta funci\'on solo se 
# debe usar para colocar los descriptores de la 
# IDT, y posiblemente despu\'es, colocar los 
# descriptores de la GDT.
#
write_byte_at_idt:
		pushl	%ebp
		movl	%esp, %ebp
		push	%gs
		pushl	%ebx
		pushl	%eax
		mov	$MIDT_SEL, %ebx
		mov	%bx, %gs
		#Poner en %al el byte a escribir
		movl	8(%ebp), %ebx
		movb	(%ebx), %al
		movl	idt_loc, %ebx
		movb	%al, %gs:(%ebx)
		incl	idt_loc
		popl	%eax
		popl	%ebx
		pop	%gs
		movl	%ebp, %esp
		popl	%ebp
		ret

m_ltrw:
		pushl	%ebp
		movl	%esp, %ebp
		#push	%gs
		#pushl	%ebx
		pushl	%eax
		mov	$TSS_SEL, %eax
		ltr	%ax
		popl	%eax
		#popl	%ebx
		#pop	%gs
		movl	%ebp, %esp
		popl	%ebp
		ret

#
# void write_byte_at_gdt(char *byte,unsigned int offset)
#
write_byte_at_gdt:
		pushl	%ebp
		movl	%esp, %ebp
		push	%gs
		pushl	%ebx
		pushl	%eax
		mov	$MGDT_SEL, %ebx
		mov	%bx, %gs
		#Poner en %al el byte a escribir
		movl	8(%ebp), %ebx
		movb	(%ebx), %al
		#Poner en %ebx el offset en el que se va a escribir dentro de la gdt
		movl	12(%ebp), %ebx
		movb	%al, %gs:(%ebx)
		popl	%eax
		popl	%ebx
		pop	%gs
		movl	%ebp, %esp
		popl	%ebp
		ret

# Function writen to be used on the x86 segmented memory model (flat 
# memory model). 
# Writes byte pointed by first argument at address <base address> + offset 
# on the kernel data segment (note the use of the selector MKRD_SEL).
#
#void copy_data_byte(const char *s,char *offset);
#
copy_data_byte:
		pushl	%ebp
		movl	%esp, %ebp
		push	%gs
		pushl	%ebx
		pushl	%eax
		mov	$MKRD_SEL, %ebx
		mov	%bx, %gs
		#Poner en %al el byte a escribir
		movl	8(%ebp), %ebx
		#movb	(%ebx), %al
		movb	%gs:(%ebx), %al
		#Poner en %ebx el offset en el que se va a escribir 
		#dentro del segmento de datos del kernel.
		movl	12(%ebp), %ebx
		#shl	$1, %ebx       # Used in linux-0.00 at write_char func, why?
		movb	%al, %gs:(%ebx)
#movl mycounter, %ebx
##cmpl $294, %ebx         # Con 294 No hay INTERRUPT 655040
#cmpl $295, %ebx         # Con 295 Si hay INTERRUPT 655040
#jbe 1f                  # Lo cual significa que hay INTERRUPT 655040 a partir de mycounter
#dumm_loop0:	jmp	dumm_loop0 #  igual a 296=2*148. Ya que cuando se usa 294 
#1:                      # cuando por primera vez mycounter == 295 es verdadero, el prog
##incl mycounter          # se queda en el ciclo infinito. Por otra parte, cuando se usa
#                        # 295, cuando por primera vez mycounter == 296 es verdadero, el prog
#                        # no llega al ciclo infinito.
		popl	%eax
		popl	%ebx
		pop	%gs
		movl	%ebp, %esp
		popl	%ebp
		ret
		
go_to_user_mode:
		#pushfl	                        #
		#andl	$0xffffbfff, (%esp)     #
		#popfl                          #
		movl	%esp, %eax
		movl	$USER_DATA_SEL, %ecx
		pushl	%ecx
		pushl	%eax
		movl	$USER_CODE_SEL, %ebx
		pushl	%ebx
		pushl	1f
#dumm_loop1:	jmp dumm_loop1
		iret
1:		movw	%cx, %ds
		movw	%cx, %es
		movw	%cx, %fs
		movw	%cx, %gs
		ret


move_to_user_mode:	# Move to user mode (task 0)
	pushfl
	andl $0xffffbfff, (%esp)
	popfl
     #if 0 //LMC 2021.09.20
	#movl $TSS0_SEL, %eax
     #else 
	movl $TSS_SEL, %eax
     #endif
	ltr %ax
     #if 0 //LMC 2021.09.20
	#movl $LDT0_SEL, %eax
     #else 
	movl $LDT_SEL, %eax
     #endif
	lldt %ax 
	movl $0, current
	sti
     #if 0 //LMC 2021.09.20
	#pushl $0x17
     #else 
	pushl $0x17
     #endif
	pushl $stack0_ptr
	pushfl
	pushl $0x0f
     #if 1 //LMC 2021.09.20
	pushl $task0
     #else 
	#pushl $task1_run
     #endif
	iret

/****************************************/
#
#void setup_base(const char *gdt,
#                const char * tss0/     ldt0/     tss1/     ldt1,
#                char    $TSS0_SEL/$LDT0_SEL/$TSS1_SEL/$LDT1_SEL)
#void setup_base(const char *gdt,const char *addr,char sel)
#
setup_base:
	pushl	%ebp		#
	movl	%esp, %ebp	# Pr\'ologo
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	movl 	$KRN_BASE, %ebx	# definir KERN_BASE al inicio de este archivo
	movl	8(%ebp), %ecx	# movl $gdt, %ecx	# movl $gdt, %ecx     # 
	movl	12(%ebp), %eax	# lea tss0, %eax	# lea ldt0, %eax      # 
	movl	16(%ebp), %edi	# movl $TSS0_SEL, %edi	# movl $TSS0_SEL, %edi# 
	call	set_base
	popl	%edi
	popl	%ecx
	popl	%ebx
	popl	%eax
	movl	%ebp, %esp	# Ep\'ilogo
	popl	%ebp
	ret

/****************************************/
# in: %eax - logic addr; %ebx = base addr ; 
# %ecx - table addr; %edi - descriptors offset.
set_base:                 #En el primer llamado a set_base 
	addl %ebx, %eax	  #%eax contendr\'a <dir base del kernel> + <dir efectiva de tss0/ldt0>
	addl %ecx, %edi   #%edi contendr\'a <dir base de GDT> + <offset de desc TSS0D>
	movw %ax, 2(%edi)       # Aqu\'i, esta escribiendo los bytes 0 y 1 de la dir base
	rorl $16, %eax
	movb %al, 4(%edi)       # aqu\'i y
	movb %ah, 7(%edi)       # aqu\'i esta escribiendo los bytes 2 y 3 de la dir base
	rorl $16, %eax          # del segmento en la gdt (o en lo que ser\'a la GDT).
	ret

/****************************************/
#if 1 //LMC 2021.09.24
current:.long 0
scr_loc:.long 0

.align 2
.word 0
lidt_opcode:
	.word 256*8-1			# idt contains 256 entries
	#.long idt + KRN_BASE		# This will be rewrite by code. 
	.long IDT_ADDR + KRN_BASE	# This will be rewrite by code. 
.align 2
.word 0
lgdt_opcode:
#	.word (end_gdt-gdt)-1	# so does gdt 
#	.long gdt + KRN_BASE	# This will be rewrite by code.
	.word GDT_SIZE-1	# so does gdt 
	.long GDT_ADDR + KRN_BASE	# This will be rewrite by code.

	.fill 128,4,0
stack_ptr:
	.long stack_ptr
	.word 0x10

#endif

#.data
#if 1 //LMC 2021.09.23
/*************************************/
.align 8
ldt0:	.quad 0x0000000000000000
    #if 0 //LMC 2021.09.23
	#.quad 0x00c0fa01000003ff	# 0x0f, base = 0x10000
	#.quad 0x00c0f201000003ff	# 0x17
    #else // See file DEFAULT_LDT_CODE and DEFAULT_LDT_DATA at include/task.h
	.quad 0x00cffa000000ffff	# 0x0f, base = 0x0000
	.quad 0x00cff2000000ffff	# 0x17
	#.quad 0x00c0fa00000003ff	# 0x0f, base = 0x0000
	#.quad 0x00c0f200000003ff	# 0x17
    #endif
/************************************/
.align 8
tss0:
	.long 0 			/* back link */
      #if 0 //LMC 2021.09.20
      #	.long stack0_krn_ptr, 0x10	/* esp0, ss0 */
      #else
	.long stack0_krn_ptr, DATA_SEL	/* esp0, ss0,  interrupt process stack */
      #endif
	.long 0, 0			/* esp1, ss1 */
	.long 0, 0			/* esp2, ss2 */
	.long 0				/* cr3 */
      #if 1 //LMC 2021.09.20
      	#.long task1_run			/* eip */
      	.long task0			/* eip */
      	.long 0x200			/* eflags */
      #else
	#.long 0				/* eip */
	#.long 0				/* eflags */
      #endif
	.long 0, 0, 0, 0		/* eax, ecx, edx, ebx */
      #if 1 //LMC 2021.09.20
      	.long stack0_ptr, 0, 0, 0	/* esp, ebp, esi, edi */
      #else
	#.long 0, 0, 0, 0		/* esp, ebp, esi, edi */
      #endif
      #if 0 //LMC 2021.09.20
      #	.long 0x17,0x0f,0x17,0x17,0x17,0x17 /* es, cs, ss, ds, fs, gs */
      #else
	.long USER_DATA_SEL,USER_CODE_SEL,USER_DATA_SEL /* es, cs, ss */
	.long USER_DATA_SEL,USER_DATA_SEL,USER_DATA_SEL /* ds, fs, gs */
      #endif
      #if 1 //LMC 2021.09.20
      	.long LDT_SEL			/* ldt */
      #else
	#.long 0x20			/* ldt */
      #endif
      #if 1 //LMC 2021.09.20
	.long 0x8000000			/* trace bitmap */
      #else
	#.long 0x00000000			/* trace bitmap */
      #endif

#if 0 //LMC 2021.09.20
#	.fill 128,4,0
#else
	.fill 256,4,0			# sizeof TASK0_STACK is 256
#endif
stack0_krn_ptr:
	.long 0

/************************************/
.align 8
ldt1:	.quad 0x0000000000000000
    #if 0 //LMC 2021.09.23
	#.quad 0x00c0fa01000003ff	# 0x0f, base = 0x10000
	#.quad 0x00c0f201000003ff	# 0x17
    #else // See file DEFAULT_LDT_CODE and DEFAULT_LDT_DATA at include/task.h
	.quad 0x00cffa000000ffff	# 0x0f, base = 0x0000
	.quad 0x00cff2000000ffff	# 0x17
	#.quad 0x00c0fa000000003ff	# 0x0f, base = 0x0000
	#.quad 0x00c0f2000000003ff	# 0x17
    #endif
/************************************/
.align 8
tss1:
	.long 0 			/* back link */
      #if 0 //LMC 2021.09.20
      #	.long stack1_krn_ptr, 0x10	/* esp0, ss0 */
      #else
	.long stack1_krn_ptr, DATA_SEL	/* esp0, ss0,  interrupt process stack */
      #endif
	.long 0, 0			/* esp1, ss1 */
	.long 0, 0			/* esp2, ss2 */
	.long 0				/* cr3 */
      #if 1 //LMC 2021.09.20
      	#.long task2_run			/* eip */
      	.long task1			/* eip */
	#if 1 //LMC 2021.09.20
      	.long 0x200			/* eflags */
	#else
      	#.long 0x3202			/* eflags */
	#endif
      #else
	#.long 0				/* eip */
	#.long 0				/* eflags */
      #endif
	.long 0, 0, 0, 0		/* eax, ecx, edx, ebx */
      #if 1 //LMC 2021.09.20
      	.long stack1_ptr, 0, 0, 0	/* esp, ebp, esi, edi */
      #else
	#.long 0, 0, 0, 0		/* esp, ebp, esi, edi */
      #endif
      #if 0 //LMC 2021.09.20
      #	.long 0x17,0x0f,0x17,0x17,0x17,0x17 /* es, cs, ss, ds, fs, gs */
      #else
	.long USER_DATA_SEL,USER_CODE_SEL,USER_DATA_SEL /* es, cs, ss */
	.long USER_DATA_SEL,USER_DATA_SEL,USER_DATA_SEL /* ds, fs, gs */
      #endif
      #if 0 //LMC 2021.09.20
      #	.long LDT0_SEL			/* ldt */
      #else
	.long LDT1_SEL			/* ldt */
      #endif
      #if 1 //LMC 2021.09.20
	.long 0x8000000			/* trace bitmap */
      #else
	#.long 0x00000000			/* trace bitmap */
      #endif

#if 0 //LMC 2021.09.20
#	.fill 128,4,0
#else
	.fill 256,4,0			# sizeof TASK0_STACK is 256
#endif
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

	.fill 128,4,0 
stack1_ptr:
	.long 0
#endif

idt_loc:.long 0		     # Usada para indicar el offset en el cual la funci\'on
                             # write_byte_at_idt escribir\'a cada byte.
my_idt_entry:.quad 0         # Used at init.c
the_gdt_sel:.byte 0x30       # $MGDT_SEL  # Used at task.c
my_gdt_entry:.quad 0         # Used at task.c
mycounter:.long 0
#endif
