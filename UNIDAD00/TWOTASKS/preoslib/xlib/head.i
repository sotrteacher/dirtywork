# 1 "head.s"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "head.s"





# 1 "../ll/i386/linkage.h" 1
# 7 "head.s" 2
# 1 "../ll/i386/defs.h" 1
# 8 "head.s" 2
# 1 "../ll/i386/mb-hdr.h" 1
# 9 "head.s" 2









KRN_BASE = 0x10000

TSS0_SEL = 0x20
LDT0_SEL = 0x28
TSS1_SEL = 0X30
LDT1_SEL = 0x38

X_FLATDATA_SEL = 0x50
X_FLATCODE_SEL = 0x58



.extern _stkbase
.extern _stktop
.extern halt
.data
current:.long 0
scr_loc:.long 0

.align 2
.word 0

IDT_descr:

lidt_opcode:
 .word 256*8-1 # idt contains 256 entries
 .long idt + KRN_BASE # This will be rewrite by code.
.align 2
.word 0

GDT_descr:

lgdt_opcode:
 .word (end_gdt-gdt)-1 # so does gdt

GDT_base:

 .long gdt + KRN_BASE # This will be rewrite by code.




 .align 8

idt: .fill 256,8,0 # idt is uninitialized

gdt: .quad 0x0000000000000000
 .quad 0x00c09a01000007ff
 .quad 0x00c09201000007ff
 .quad 0x00c0920b80000002

 .quad 0x0000e90100000068 # TSS0 descr 0x20
 .quad 0x0000e20100000040 # LDT0 descr 0x28
 .quad 0x0000e90100000068 # TSS1 descr 0x30
 .quad 0x0000e20100000040 # LDT1 descr 0x38


rmBckGateFix1:
.word 0
.word X_FLATCODE_SEL
.word 0x8C00
rmBckGateFix2:
.word 0

.word 0,0,0,0
.word 0xFFFF
.word 0
.word 0x9200
.word 0x00CF
.word 0xFFFF
.word 0
.word 0x9A00
.word 0x00CF
.word 0,0,0,0
.word 0,0,0,0
.word 0,0,0,0
.word 0,0,0,0

end_gdt:
 .fill 128,4,0

base:
.space 8192,0
tos:

stack_ptr:
 .long stack_ptr
 .word 0x10





.align 8

ldt0: .quad 0x0000000000000000
 .quad 0x00c0fa01000003ff # 0x0f, base = 0x10000
 .quad 0x00c0f201000003ff # 0x17
tss0:
 .long 0
 .long stack0_krn_ptr, 0x10
 .long 0, 0
 .long 0, 0
 .long 0
 .long task0
 .long 0x200
 .long 0, 0, 0, 0
 .long stack0_ptr, 0, 0, 0
 .long 0x17,0x0f,0x17,0x17,0x17,0x17
 .long LDT0_SEL
 .long 0x8000000

 .fill 128,4,0
stack0_krn_ptr:
 .long 0





.align 8

ldt1: .quad 0x0000000000000000
 .quad 0x00c0fa01000003ff # 0x0f, base = 0x10000
 .quad 0x00c0f201000003ff # 0x17
tss1:
 .long 0
 .long stack1_krn_ptr, 0x10
 .long 0, 0
 .long 0, 0
 .long 0
 .long task1
 .long 0x200
 .long 0, 0, 0, 0
 .long stack1_ptr, 0, 0, 0
 .long 0x17,0x0f,0x17,0x17,0x17,0x17
 .long LDT1_SEL
 .long 0x8000000

 .fill 128,4,0
stack1_krn_ptr:
 .long 0




.text

.globl _start
.globl start

_start:
start:


.align 8

  jmp boot_entry




.align 8
boot_hdr:
.align 8
  .long 0x1BADB002
  .long (0x00000002)

  .long -(0x1BADB002 +(0x00000002))

boot_entry:


      movl $0xB8000,%edi
      addl $158,%edi
      movb $'1',%gs:0(%edi)
      incl %edi
      movb $6,%gs:0(%edi)






  movl %gs:0(%ebx),%ecx
  andl $0x080,%ecx

  jnz GDT_is_OK
# 214 "head.s"
movl $0xB8000,%edi
addl $150,%edi
movb $'1',%gs:0(%edi)
incl %edi
movb $6,%gs:0(%edi)
incl %edi
movb $'2',%gs:0(%edi)
incl %edi
movb $6,%gs:0(%edi)

  movl $halt,%eax
  movw %ax,%gs:rmBckGateFix1
  shrl $16,%eax
  movw %ax,%gs:rmBckGateFix2


  lgdt GDT_descr
movl $0xB8000,%edi
addl $146,%edi
movb $'0',%gs:0(%edi)
incl %edi
movb $6,%gs:0(%edi)

GDT_is_OK: movw $(X_FLATDATA_SEL),%ax
  movw %ax,%ds
  movw %ax,%es
  movw %ax,%ss
  movw %ax,%fs
  movw %ax,%gs
  movl $tos,%esp
  movl $base,_stkbase
  movl $tos,_stktop



  movl %eax,mb_signature
  movl %ebx,mbi




  sgdt GDT_descr


  ljmp $X_FLATCODE_SEL, $load_cs

load_cs:

  lidt IDT_descr

  cld



                call startup_32

looplabel:
  jmp looplabel




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
 movl $0x10,%eax # reload all the segment registers
 mov %ax,%ds # after changing gdt.
 mov %ax,%es
 mov %ax,%fs
 mov %ax,%gs
 #movw %ax,%ds # after changing gdt.
 #movw %ax,%es
 #movw %ax,%fs
 #movw %ax,%gs
 lss stack_ptr,%esp

# setup up timer 8253 chip.
 #movw %ax,%ds # after changing gdt.
 movb $0x36, %al
 movl $0x43, %edx
 outb %al, %dx
 movl $11930, %eax # timer frequency 100 HZ
 movl $0x40, %edx
 outb %al, %dx
 movb %ah, %al
 outb %al, %dx

# setup timer & system call interrupt descriptors.
 movl $0x00080000, %eax
debuglabel0:



 movl $timer_interrupt, %eax

 movw $0x8E00, %dx
 movl $0x20, %ecx
 lea idt(,%ecx,8), %esi
 movl %eax,(%esi)
 movl %edx,4(%esi)
debuglabel1:



 movl $timer_interrupt, %eax

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


setup_gdt:
 lgdt lgdt_opcode
 ret

setup_idt:
 lea ignore_int,%edx
 movl $0x00080000,%eax
 movw %dx,%ax
 movw $0x8E00,%dx
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



 movl scr_loc, %ebx

 shl $1, %ebx
 movb %al, %gs:(%ebx)
 shr $1, %ebx
 incl %ebx
 cmpl $2000, %ebx
 jb 1f
 movl $0, %ebx
1: movl %ebx, scr_loc
 popl %eax
 popl %ebx
 pop %gs
 ret




.align 2



ignore_int:
 push %ds
 pushl %eax
 movl $0x10, %eax
 #mov %ax, %ds
 movw %ax, %ds
 movl $67, %eax
 call write_char
 popl %eax
 pop %ds
 iret


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
1: movl $0, current
 ljmp $TSS0_SEL, $0
2: popl %eax
 popl %ebx
 popl %ecx
 popl %edx
 pop %ds
 iret


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



task0:
 movl $0x17, %eax
 movw %ax, %ds



 movb $65, %al

 int $0x80
 movl $0xfff, %ecx
1: loop 1b
 jmp task0

 .fill 128,4,0
stack0_ptr:
 .long 0

task1:
 movl $0x17, %eax
 movw %ax, %ds



 movb $66, %al

 int $0x80
 movl $0xfff, %ecx
1: loop 1b
 jmp task1

 .fill 128,4,0
stack1_ptr:
 .long 0

.data

mb_signature: .long 0
mbi: .long 0
