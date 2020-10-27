#********************************************************************
#Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.
#********************************************************************/

# Each non-boot CPU ("AP") is started up in response to a STARTUP
# IPI from the boot CPU.  Section B.4.2 of the Multi-Processor
# Specification says that the AP will start in real mode with CS:IP
# set to XY00:0000, where XY is an 8-bit value sent with the
# STARTUP. Thus this code must start at a 4096-byte boundary.
#
# Because this code sets DS to zero, it must sit
# at an address in the low 2^16 bytes.
#
# Startothers (in main.c) sends the STARTUPs one at a time.
# It copies this code (start) at 0x7000.  It puts the address of
# a newly allocated per-core stack in start-4,the address of the
# place to jump to (mpenter) in start-8, and the physical address
# of entrypgdir in start-12.
#
# This code is identical to bootasm.S except:
#   - it does not need to enable A20
#   - it uses the address at start-4, start-8, and start-12
# display at 0xB800
.text	
           .global print0,print1,print2
.code16           
.globl start
start:
  cli            

#  xorw    %ax,%ax
  movw    %cs,%ax    # cs=0x9100
  movw    %ax,%ds
  movw    %ax,%es
  movw    %ax,%ss
  movw    $2048,%sp

print0:	
#  movw   $26,%cx
#  movw   $0xB800,%ax
#  movw   %ax,%ds
#  movw   $0,%bx
#  movw $0x0D61,%ax
#ha:	
#  movw %ax,0(%bx)
#  addw $2,%bx
#  addw $1,%ax
#  loop ha

  movw   $0xB800,%ax
  movw   %ax,%ds
  movw   $0,%bx

  movw $0x0A46,%ax
  movw %ax,0(%bx)	
  addw $2,%bx
  movw $0x0B55,%ax
  movw %ax,0(%bx)	
  addw $2,%bx

  movw $0x0C43,%ax
  movw %ax,0(%bx)	
  addw $2,%bx

  movw $0x0D4B,%ax
  movw %ax,0(%bx)	
  addw $2,%bx

  movw %cs,%ax
  movw %ax,%ds
	
#xia1:	jmp xia1

# enter protected mode
  lgdt    gdtdesc
  movl    %cr0, %eax
  orl     $0x01, %eax
  movl    %eax, %cr0

# do a ljmp to flush instruction cache and set CS to KCODE_SEL

#	        .byte   0x66, 0xEA   # prefix + ljmp-opcode
#                .long	0x91000+start32
#        	.word	0x08        # CS selector
  ljmpl    $0x08, $(0x91000+start32)  # need ljmpl for 4-byte target address

.code32
start32:
  movl    $0x10, %eax
  movw    %ax, %ds
  movw    %ax, %es
  movw    %ax, %ss
#  movw    $4096, %sp
	
# set fs gs to 0
  movw    $0, %ax
  movw    %ax, %fs
  movw    %ax, %gs

print1:

  movl   $13,%ecx
  movl   $0xB80A0,%ebx

  movw  $0x0C61,%ax

loop1:	
  movw %ax,0(%ebx)
  addl $2,%ebx
  addl $1,%eax
  loop loop1

#isan:	jmp isan
	
	
#try to turn on paging
  movl    $0x101000,%eax
#  movl    $0x081000,%eax
  movl    %eax, %cr3
	
# Turn on paging.
  movl    %cr0, %eax
  orl     $0x80000000,%eax
  movl    %eax, %cr0

   jmp    1f
1:	
#   movl   $2f,%eax
#2: jmp    *%eax

print2:	
  movl   $26,%ecx
  movl   $0x800B8140,%ebx

  movw  $0x0B41,%ax

loop2:	
  movw %ax,0(%ebx)
  addl $2,%ebx
  addl $1,%eax
  loop loop2
cli	
fxia:	jmp fxia
	
# Switch to the stack allocated by startothers()
  movl    (start-4), %esp
# Call mpenter()
#  call	 0x1057C6 #*(start-8)
#  jmp    *(start-8)
#   ljmp   $0x08,$0x1057C6
    ljmp   $0x08,$0x15000
#   call 0x1057c6
	
spin:
  jmp     spin
	
gdt:	
        .quad	0x0000000000000000 # null descriptor
	.quad	0x00cF9a000000FFFF # kcs 00cF 9=PpLS=1001
	.quad	0x00cF92000000FFFF # kds
	.quad	0x0000000000000000 # tss
	.quad	0x00cFFa000000FFFF # ucs 00cF F=PpLS=1111
	.quad	0x00cFF2000000FFFF # uds
	
gdtdesc:
#	.word	.-gdt-1
	.word   48-1
	.long	0x91000+gdt

	
.org 512

