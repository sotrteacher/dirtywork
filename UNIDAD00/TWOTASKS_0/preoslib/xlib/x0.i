# 1 "x0.s"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "x0.s"
# 28 "x0.s"
# 1 "../ll/i386/sel.h" 1
# 29 "x0.s" 2
# 1 "../ll/i386/linkage.h" 1
# 30 "x0.s" 2
# 1 "../ll/i386/defs.h" 1
# 31 "x0.s" 2
# 1 "../ll/i386/mb-hdr.h" 1
# 32 "x0.s" 2
# 41 "x0.s"
.extern _startup
.extern _stkbase
.extern _stktop

.extern halt

.data

FileName: .string "Profile:""X0"

.globl IDT
.globl GDT_base
.globl mb_signature
.globl mbi


.globl gdt
.globl ldt
gdt:


GDT:
.word 0,0,0,0
.word 0,0,0,0
.word 0,0,0,0
.word 0,0,0,0

rmBckGateFix1:
.word 0
.word 0x38
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
 .fill 256 - 12,8,0

GDT_descr:
.word 256*8-1
GDT_base:
.long GDT


idt:


IDT:
 .fill 256,8,0 # idt is uninitialized
IDT_descr:
 .word 256*8-1 # idt contains 256 entries
 .long IDT


mb_signature: .long 0
mbi: .long 0


base:
.space 8192,0
tos:

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
# 145 "x0.s"
boot_entry:
# 160 "x0.s"
  movl %gs:0(%ebx),%ecx
  andl $0x080,%ecx
  jnz GDT_is_OK





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

GDT_is_OK: movw $(0x30),%ax
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


  ljmp $0x38, $load_cs

load_cs:

  lidt IDT_descr

  cld
  call _startup





  .byte 0x0EA
  .long 0
  .word 0x20
